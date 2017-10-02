/*
 * Name: Yuchen Cheng
 * ID: 515030910477
 */
 
#include <stdio.h>
#include <getopt.h> 
#include <stdlib.h> 
#include <unistd.h>
#include "cachelab.h"

#define false 0
#define true 1

typedef struct {
    unsigned long long tag;
    unsigned long long set;
    unsigned long long block;
} Address;

typedef unsigned long long Cache_line;
typedef Cache_line * Cache_set;
typedef Cache_set * Cache;
Cache cache;

typedef enum {Hit, Miss, Eviction} Behavior;
int hit, miss, eviction;
int s, E, b; 

int verbose = false;
char *trace = NULL;

/*
 * print_usage_info - print the usage information
 */
void print_usage_info(char *argv[]) {
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options: \n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n\n");
    printf("Examples: \n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
}

/*
 * init_cache - init the cache
 */
void init_cache() {
    int S = 1 << s;
    int i, j;
    cache = (Cache)malloc(sizeof(Cache_set) * S);
    for (i = 0; i < S; i++) {
        cache[i]= (Cache_line *)malloc(sizeof(Cache_line) * E);
        for (j = 0; j < E; j++) {
            cache[i][j] = 1 << (64 - s - b);
        }
        hit = 0;
        miss = 0;
        eviction = 0;
    }
}

/*
 * free_cache - free the cache in the end
 */
void free_cache() {
    int S = 1 << s;
    int i;
    for (i = 0; i < S; i++) free(cache[i]);
    free(cache);
}

/*
 * set_address - split the address to tag, set and block
 */
Address set_address(unsigned long long address) {
    Address res;
    res.tag = address >> (s + b);
    res.set = (address >> b) & ((1 << s) - 1);
    res.block =  address & ((1 << b) - 1);
    return res;

}

void hit_inc() {
    hit++;
    if (verbose) printf(" hit");
}

void miss_inc() {
    miss++;
    if (verbose) printf(" miss");
}
void eviction_inc() {
    miss++;
    eviction++;
    if (verbose) printf(" miss eviction");
}

/*
 * process - set behavior according to the tag and set
 */
Behavior process(unsigned long long tag, unsigned long long set) {
    int i;
    Cache_line line;
    for (i = 0; i < E; i++) {
        line = cache[set][i];
        if (line == tag || line == 1 << (64 - s - b)) break;
    }
    Behavior behav;
    if (i == E) {
        behav = Eviction;
        i = 0;
    }
    else if (line == tag) behav = Hit;
    else behav = Miss;

    for (; i < E - 1; i++) {
        cache[set][i] = cache[set][i+1];
        line = cache[set][i];
        if (line == 1 << (64 - s - b)) break;
    }
    cache[set][i] = tag;
    return behav;
}

/*
 * execute - execute the code
 */
void execute(char op, unsigned long long address, unsigned long long size) {
    Address addr = set_address(address);
    Behavior behav = process(addr.tag, addr.set);
    if (verbose) printf("%c %llx,%llx", op, address, size);

    if (behav == Hit) hit_inc();
    else if (behav == Miss) miss_inc(); 
    else if (behav == Eviction) eviction_inc();
    if (op == 'M') hit_inc();

    if (verbose) printf("\n");
}


/*
 * parse_valgrind - parse valgrind from the files
 */
void parse_valgrind() {
    FILE *trace_file;
    trace_file = fopen(trace, "r");
    if (!trace_file) {
        printf("Error.");
        return;
    }
    char op;
    unsigned long long address, size;
    while (fscanf(trace_file, " %c %llx,%llx", &op, &address, &size) > 0) {
        if (op == 'I') continue;
        execute(op, address, size);
    }
    fclose(trace_file);
}

int main(int argc, char *argv[]) {
    char op;
    while ((op = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (op) {
            case 'v':
                verbose = true;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                trace = optarg;
                break;
            case 'h':
            default:
                print_usage_info(argv);
                break;
        }
    }
    init_cache();
    parse_valgrind();
    printSummary(hit, miss, eviction);
    free_cache();
    return 0;
}
