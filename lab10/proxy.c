/*
 * proxy.c - CS:APP Web proxy
 *
 * Name: Yuchen Cheng
 * ID: 515030910477
 * E-mail: rudeigerc@sjtu.edu.cn
 * 
 * IMPORTANT: Give a high level description of your code here. You
 * must also provide a header comment at the beginning of each
 * function that describes what that function does.
 */ 

#include "csapp.h"
#define NTHREADS 8
#define SBUFSIZE 32

typedef struct{
     int clientfd;
     struct sockaddr_in clientaddr;
}Client;

typedef struct{
     Client **buf;
     int n;
     int front;
     int rear;
     sem_t mutex;
     sem_t slots;
     sem_t items;
}sbuf_t;

/*
 * Function prototypes
 */
int parse_uri(char *uri, char *target_addr, char *path, int *port);
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, int size);
void *thread(void *vargp);
void doit(Client *client);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
ssize_t Rio_readn_w(int fd, void *ptr, size_t nbytes);
ssize_t Rio_readnb_w(rio_t *rp, void *usrbuf, size_t n);
ssize_t Rio_readlineb_w(rio_t *rp, void *usrbuf, size_t maxlen);
ssize_t Rio_writen_w(int fd, void *usrbuf, size_t n);
void write_log(char *log);
void sbuf_init(sbuf_t *sp, int n);
void sbuf_deinit(sbuf_t *sp);
void sbuf_insert(sbuf_t *sp, Client *client);
Client *sbuf_remove(sbuf_t *sp);
int Open_clientfd_ts(char *hostname, int port);

FILE *logfile;
sem_t mutex_1, mutex_2;
sbuf_t sbuf;

/* 
 * main - Main routine for the proxy program 
 */
int main(int argc, char **argv)
{
    int listenfd, port;
    Client *client;
    socklen_t clientlen = sizeof(struct sockaddr_in);
    struct sockaddr_in clientaddr;
    pthread_t tid;
    
    /* Check arguments */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
        exit(0);
    }
    
    Signal(SIGPIPE, SIG_IGN);
    port = atoi(argv[1]);
    listenfd = Open_listenfd(port);
    sbuf_init(&sbuf, SBUFSIZE);
    Sem_init(&mutex_1, 0, 1);
    Sem_init(&mutex_2, 0, 1);

    int i;
    for(i = 0; i < NTHREADS; i++)
        Pthread_create(&tid, NULL, thread, NULL);


    while (1) {
        client = (Client *)Malloc(sizeof(Client));
        clientaddr = client->clientaddr;
        client->clientfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        sbuf_insert(&sbuf, client);
    }

    exit(0);
}


/*
 * parse_uri - URI parser
 * 
 * Given a URI from an HTTP proxy GET request (i.e., a URL), extract
 * the host name, path name, and port.  The memory for hostname and
 * pathname must already be allocated and should be at least MAXLINE
 * bytes. Return -1 if there are any problems.
 */
int parse_uri(char *uri, char *hostname, char *pathname, int *port)
{
    char *hostbegin;
    char *hostend;
    char *pathbegin;
    int len;

    if (strncasecmp(uri, "http://", 7) != 0) {
        hostname[0] = '\0';
        return -1;
    }
       
    /* Extract the host name */
    hostbegin = uri + 7;
    hostend = strpbrk(hostbegin, " :/\r\n\0");
    len = hostend - hostbegin;
    strncpy(hostname, hostbegin, len);
    hostname[len] = '\0';
    
    /* Extract the port number */
    *port = 80; /* default */
    if (*hostend == ':')   
	*port = atoi(hostend + 1);
    
    /* Extract the path */
    pathbegin = strchr(hostbegin, '/');
    if (pathbegin == NULL) {
	    pathname[0] = '\0';
    }
    else {
        //pathbegin++;	
        strcpy(pathname, pathbegin);
    }

    return 0;
}

/*
 * format_log_entry - Create a formatted log entry in logstring. 
 * 
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), and the size in bytes
 * of the response from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, 
		      char *uri, int size)
{
    time_t now;
    char time_str[MAXLINE];
    unsigned long host;
    unsigned char a, b, c, d;

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    /* 
     * Convert the IP address in network byte order to dotted decimal
     * form. Note that we could have used inet_ntoa, but chose not to
     * because inet_ntoa is a Class 3 thread unsafe function that
     * returns a pointer to a static variable (Ch 13, CS:APP).
     */
    host = ntohl(sockaddr->sin_addr.s_addr);
    a = host >> 24;
    b = (host >> 16) & 0xff;
    c = (host >> 8) & 0xff;
    d = host & 0xff;


    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %d.%d.%d.%d %s", time_str, a, b, c, d, uri);
}

/*
 * thread - Thread routine
 */
void *thread(void *vargp)
{
    Pthread_detach(pthread_self());
    while(1) {
        Client *client = sbuf_remove(&sbuf);
        doit(client);
    }
}

/*
 * doit - Core function
 */
void doit(Client *client) 
{
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char ctos[MAXLINE];
    char servername[MAXLINE], pathname[MAXLINE];
    int serverfd, port;
    rio_t client_rio, server_rio;

    int clientfd = client->clientfd;
    struct sockaddr_in clientaddr = client->clientaddr;
    Free(client);

    Rio_readinitb(&client_rio, clientfd);
    Rio_readlineb_w(&client_rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcasecmp(method, "GET")) {
        clienterror(clientfd, method, "501", "Not Implemented", "Proxy does not implement this method");
        return;
    }
    parse_uri(uri, servername, pathname, &port);

    serverfd = Open_clientfd_ts(servername, port);
    sprintf(ctos, "%s %s %s\r\n", method, pathname, version);
    while(strcmp(buf, "\r\n")) {
        Rio_readlineb_w(&client_rio, buf, MAXLINE);
        strcat(ctos, buf);
    }

    Rio_writen_w(serverfd, ctos, sizeof(ctos));
    Rio_readinitb(&server_rio, serverfd);
    int n, size = 0;
    while((n = Rio_readnb_w(&server_rio, buf, MAXLINE)) > 0) {
        if(Rio_writen_w(clientfd, buf, n) != n) break;
        size += n;
    }
    Close(serverfd);
    format_log_entry(buf, &clientaddr, uri, size);
    write_log(buf);
    Close(clientfd);

}

/*
 * clienterror - Error handling
 */
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}

/*
 * Rio_readn_w - Override Rio_readn
 */
ssize_t Rio_readn_w(int fd, void *ptr, size_t nbytes) 
{
    ssize_t n;
  
    if ((n = rio_readn(fd, ptr, nbytes)) < 0) {
	    printf("Rio_readn error\n");
        return 0;
    }

    return n;
}

/*
 * Rio_readnb_w - Override Rio_readnb_w
 */
ssize_t Rio_readnb_w(rio_t *rp, void *usrbuf, size_t n) 
{
    ssize_t rc;

    if ((rc = rio_readnb(rp, usrbuf, n)) < 0) {
	    printf("Rio_readnb error\n");
        return 0;
    }

    return rc;
}

/*
 * Rio_readlineb_w - Override Rio_readlineb
 */
ssize_t Rio_readlineb_w(rio_t *rp, void *usrbuf, size_t maxlen) 
{
    ssize_t rc;

    if ((rc = rio_readlineb(rp, usrbuf, maxlen)) < 0) {
	    printf("Rio_readlineb error\n");
        return 0;
    }

    return rc;
} 

/*
 * Rio_writen_w - Override Rio_writen
 */
ssize_t Rio_writen_w(int fd, void *usrbuf, size_t n) 
{
    ssize_t rc;

    if ((rc = rio_writen(fd, usrbuf, n)) != n) {
        printf("Rio_writen error\n");
        return 0;
    }
    
    return rc;
}

/*
 * write_log - Generate the log file
 */
void write_log(char *log)
{
    P(&mutex_1);
    logfile = Fopen("proxy.log", "a");
    fprintf(logfile, "%s", log);
    Fclose(logfile);
    V(&mutex_1);
}

/*
 * sbuf_init - Create an empty, shared FIFO buffer with n slots
 */
void sbuf_init(sbuf_t *sp, int n)
{
     sp->buf = Calloc(n,sizeof(Client *));
     sp->n = n;
     sp->front = sp->rear = 0;
     Sem_init(&sp->mutex, 0, 1);
     Sem_init(&sp->slots, 0, n);
     Sem_init(&sp->items, 0, 0);
}

/*
 * sbuf_deinit - Clean up buffer sp
 */
void sbuf_deinit(sbuf_t *sp)
{
     Free(sp->buf);
}

/*
 * sbuf_insert - Insert them onto the rear of shared buffer up
 */
void sbuf_insert(sbuf_t *sp, Client *item)
{
     P(&sp->slots);
     P(&sp->mutex);
     sp->buf[(++sp->rear)%(sp->n)] = item;
     V(&sp->mutex);
     V(&sp->items);
}

/*
 * sbuf_remove - Remove and return the first item from buffer up
 */
Client *sbuf_remove(sbuf_t *sp)
{
     Client *item;
     P(&sp->items);
     P(&sp->mutex);
     item = sp->buf[(++sp->front)%(sp->n)];
     V(&sp->mutex);
     V(&sp->slots);
     return item;
}

/*
 * Open_clientfd_ts - A thread-safe version of open_clientfd
 */
int Open_clientfd_ts(char *hostname, int port) 
{
    P(&mutex_2);
    int rc;
    
    if ((rc = open_clientfd(hostname, port)) < 0) {
	    printf("Open_clientfd error");
        return 0;

    }
    V(&mutex_2);
    return rc;
}