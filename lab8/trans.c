/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 *
 * Name: Yuchen Cheng
 * ID: 515030910477
 *
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k, l;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    if (M == 32 && N == 32)
    {
        for (j = 0; j < 32; j += 8) 
        {
            for (i = 0; i < 32; i ++) 
            {
                tmp0 = A[i][j];
                tmp1 = A[i][j+1];
                tmp2 = A[i][j+2];
                tmp3 = A[i][j+3];
                tmp4 = A[i][j+4];
                tmp5 = A[i][j+5];
                tmp6 = A[i][j+6];
                tmp7 = A[i][j+7];

                B[j][i] = tmp0;
                B[j+1][i] = tmp1;
                B[j+2][i] = tmp2;
                B[j+3][i] = tmp3;
                B[j+4][i] = tmp4;
                B[j+5][i] = tmp5;
                B[j+6][i] = tmp6;
                B[j+7][i] = tmp7;
            }
        }
    }

    else if (M == 64 && N == 64)
    {
        for (i = 0; i < 64; i += 8) 
        {
            for (j = 0; j < 64; j += 8) 
            {
                for (k = i; k < i + 4; k++)
                {
                    /* A[i][j] - A[i+3][j] */
                    tmp0 = A[k][j];
                    tmp1 = A[k][j+1];
                    tmp2 = A[k][j+2];
                    tmp3 = A[k][j+3];

                    /* A[i+4][j] - A[i+7][j] */
                    tmp4 = A[k][j+4];
                    tmp5 = A[k][j+5];
                    tmp6 = A[k][j+6];
                    tmp7 = A[k][j+7];

                    /* B[j][k] - A[j+3][k] */
                    B[j][k] = tmp0;
                    B[j+1][k] = tmp1;
                    B[j+2][k] = tmp2;
                    B[j+3][k] = tmp3;

                    /* A[j][k+4] - A[j+3][k+4] */
					B[j][k+4] = tmp4;
					B[j+1][k+4] = tmp5;
					B[j+2][k+4] = tmp6;
					B[j+3][k+4] = tmp7;
                }

                for (l = j; l < j + 4; l++)
                {
                    /* B[l][i+4] - B[l][i+7] */
                    tmp0 = B[l][i+4];
					tmp1 = B[l][i+5];
					tmp2 = B[l][i+6];
					tmp3 = B[l][i+7];

                    /* B[l][i+4] - B[l][i+7] */
                    /* A[i+4][l] - A[i+7][l] */
                    B[l][i+4] = A[i+4][l];
					B[l][i+5] = A[i+5][l];
					B[l][i+6] = A[i+6][l];
					B[l][i+7] = A[i+7][l];

                    /* B[l+4][i] - B[l+4][i+3] */
                    B[l+4][i] = tmp0;
					B[l+4][i+1] = tmp1;
					B[l+4][i+2] = tmp2;
					B[l+4][i+3] = tmp3;
                }

                for (k = i + 4; k < i + 8; k++)
                {
                    /* A[k][j+4] - A[k][j+7] */
					tmp0 = A[k][j+4];
					tmp1 = A[k][j+5];
					tmp2 = A[k][j+6];
					tmp3 = A[k][j+7];

                    /* B[j+4][k] - B[j+7][k] */
					B[j+4][k] = tmp0;
					B[j+5][k] = tmp1;
					B[j+6][k] = tmp2;
					B[j+7][k] = tmp3;
				}
            }
        }
    }

    else if (M == 61 && N == 67)
    {
        for (i = 0; i < 64; i += 8) 
        {
            for (j = 0; j < 61; j++) 
            {
                tmp0 = A[i][j];
                tmp1 = A[i+1][j];
                tmp2 = A[i+2][j];
                tmp3 = A[i+3][j];
                tmp4 = A[i+4][j];
                tmp5 = A[i+5][j];
                tmp6 = A[i+6][j];
                tmp7 = A[i+7][j];

                B[j][i] = tmp0;
                B[j][i+1] = tmp1;
                B[j][i+2] = tmp2;
                B[j][i+3] = tmp3;
                B[j][i+4] = tmp4;
                B[j][i+5] = tmp5;
                B[j][i+6] = tmp6;
                B[j][i+7] = tmp7;
            }
        }

        for (j = 0; j < 61; j++) 
        {
            tmp0 = A[64][j];
            tmp1 = A[65][j];
            tmp2 = A[66][j];

            B[j][64] = tmp0;
            B[j][65] = tmp1;
            B[j][66] = tmp2;
        }
    }

    else
    {
        for (i = 0; i < N; i++) 
        {
            for (j = 0; j < M; j++) 
            {
                tmp0 = A[i][j];
                B[j][i] = tmp0;
            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}