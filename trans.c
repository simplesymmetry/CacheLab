/**
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/**
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i; //index of row
    int j; //index of column
    int r; //rows
    int c; //columns
    int transpose = 0;
    int temp = 0;

    //We will use a switch to determine the case by sorting by N.
    //There are three test matrices used. There is 32x32, 64x64, and 61x67.
    //This solution will not work on other sizes.
    switch(N){
        case 64:
            //we will loop through the columns then rows
            //but increasing by 4 to simulate a block
            for (c = 0; c < N; c += 4)
            {
                for (r = 0; r < N; r += 4)
                {
                    for (i = r; i < r + 4; i++)
                    {
                        for (j = c; j < c + 4; j++)
                        {
                            if (i != j) {
                                B[j][i] = A[i][j];
                            }
                            else{
                                temp = A[i][j];
                                transpose = i;
                            }
                        }
                        if (r == c)
                        {
                            B[transpose][transpose] = temp;
                        }
                    }
                }
            }
            break;
        case 32:
            //first loop through columns
            //incrementing c by 8 is best way for 32
            for (c = 0; c < N; c += 8)
            {
            //and through rows
                for (r = 0; r < N; r += 8)
                {
                    for (i = r; i < r + 8; i++)
                    {
                        for (j = c; j < c + 8; j++)
                        {
                            if (i != j)
                            {
                                B[j][i] = A[i][j];
                                }
                            else{
                                temp = A[i][j];
                                transpose = i;
                                }
                        }
                            if (r == c)
                            {
                            B[transpose][transpose] = temp;
                            }
                    }
                }
            }
            break;
        default:
            for (c = 0; c < M; c += 16)
            {
                for (r = 0; r < N; r += 16)
                {
                    for (i = r; (i < r + 16) && (i < N); i++)
                    {
                        for (j = c; (j < c + 16) && (j < M); j++)
                        {
                            if (i != j)
                            {
                                B[j][i] = A[i][j];
                            }
                            else
                                {
                                temp = A[i][j];
                                transpose = i;
                                }
                        }
                        if (r == c)
                        {
                        B[transpose][transpose] = temp;
                        }
                }
            }
        }
    } //end switch
} //end function

/**
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/**
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /** Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /** Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}
