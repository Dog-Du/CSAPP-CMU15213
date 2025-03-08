/*
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

#define swap_int(a, b, tmp)     \
    do {                        \
        tmp = a;                \
        a = b;                  \
        b = tmp;                \
    } while (0)

#define transpose_matrix(i, j, N, A)                             \
    do {                                                         \
        for (k = 0; k < N; ++k) {                                \
            for (l = 0; l < k; ++l) {                            \
                swap_int(A[i + k][j + l], A[i + l][j + k], a0);  \
            }                                                    \
        }                                                        \
    } while (0)

#define transpose8_matrix(i, j, A, B)                            \
    do {                                                         \
        /* 处理左上和右上 */                                      \
        for (k = 0; k < 4; ++k) {                                \
            a0 = A[i + k][j + 0];                                \
            a1 = A[i + k][j + 1];                                \
            a2 = A[i + k][j + 2];                                \
            a3 = A[i + k][j + 3];                                \
            a4 = A[i + k][j + 4];                                \
            a5 = A[i + k][j + 5];                                \
            a6 = A[i + k][j + 6];                                \
            a7 = A[i + k][j + 7];                                \
                                                                 \
            /* 行上写八个*/                                       \
            B[j + k][i + 0] = a0;                                \
            B[j + k][i + 1] = a1;                                \
            B[j + k][i + 2] = a2;                                \
            B[j + k][i + 3] = a3;                                \
            B[j + k][i + 4] = a4;                                \
            B[j + k][i + 5] = a5;                                \
            B[j + k][i + 6] = a6;                                \
            B[j + k][i + 7] = a7;                                \
        }                                                        \
                                                                 \
        transpose_matrix(j, i, 4, B);                            \
        transpose_matrix(j, i + 4, 4, B);                        \
                                                                 \
        /* 左下和右下。*/                                         \
        for (k = 0; k < 4; ++k) {                                \
            a0 = B[j + k][i + 4 + 0]; /* B 右上 */               \
            a1 = B[j + k][i + 4 + 1];                            \
            a2 = B[j + k][i + 4 + 2];                            \
            a3 = B[j + k][i + 4 + 3];                            \
                                                                 \
            B[j + k][i + 4 + 0] = A[i + 4 + 0][j + k];           \
            B[j + k][i + 4 + 1] = A[i + 4 + 1][j + k];           \
            B[j + k][i + 4 + 2] = A[i + 4 + 2][j + k];           \
            B[j + k][i + 4 + 3] = A[i + 4 + 3][j + k];           \
                                                                 \
            B[j + k + 4][i + 0 + 0] = a0;                        \
            B[j + k + 4][i + 0 + 1] = a1;                        \
            B[j + k + 4][i + 0 + 2] = a2;                        \
            B[j + k + 4][i + 0 + 3] = a3;                        \
            B[j + k + 4][i + 4 + 0] = A[i + 4 + 0][j + 4 + k];   \
            B[j + k + 4][i + 4 + 1] = A[i + 4 + 1][j + 4 + k];   \
            B[j + k + 4][i + 4 + 2] = A[i + 4 + 2][j + 4 + k];   \
            B[j + k + 4][i + 4 + 3] = A[i + 4 + 3][j + 4 + k];   \
        }                                                        \
    } while (0)


// n <= 8
#define transposen_matrix(i, j, n, A, B)                        \
    do {                                                        \
                for (k = 0; k < n; ++k) {                       \
                    switch (n) {                                \
                        case 8: a7 = A[i + k][j + 7];           \
                        case 7: a6 = A[i + k][j + 6];           \
                        case 6: a5 = A[i + k][j + 5];           \
                        case 5: a4 = A[i + k][j + 4];           \
                        case 4: a3 = A[i + k][j + 3];           \
                        case 3: a2 = A[i + k][j + 2];           \
                        case 2: a1 = A[i + k][j + 1];           \
                        case 1: a0 = A[i + k][j + 0];           \
                    }                                           \
                    switch (n) {                                \
                        case 8: B[j + k][i + 7] = a7;           \
                        case 7: B[j + k][i + 6] = a6;           \
                        case 6: B[j + k][i + 5] = a5;           \
                        case 5: B[j + k][i + 4] = a4;           \
                        case 4: B[j + k][i + 3] = a3;           \
                        case 3: B[j + k][i + 2] = a2;           \
                        case 2: B[j + k][i + 1] = a1;           \
                        case 1: B[j + k][i + 0] = a0;           \
                    }                                           \
                }                                               \
                transpose_matrix(j, i, n, B);                   \
    } while (0)

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
    int a0, a1, a2, a3, a4, a5, a6, a7;
    if (M == 32) {
        for (i = 0;i < N; i += 8) {
            for (j = 0;j < M; j += 8) {
                for (k = 0;k < 8; ++k) {
                    // 一次读 8 个，正好是 32 字节
                    a0 = A[i + k][j + 0];
                    a1 = A[i + k][j + 1];
                    a2 = A[i + k][j + 2];
                    a3 = A[i + k][j + 3];
                    a4 = A[i + k][j + 4];
                    a5 = A[i + k][j + 5];
                    a6 = A[i + k][j + 6];
                    a7 = A[i + k][j + 7];

                    // 在行上写八个
                    B[j + k][i + 0] = a0;
                    B[j + k][i + 1] = a1;
                    B[j + k][i + 2] = a2;
                    B[j + k][i + 3] = a3;
                    B[j + k][i + 4] = a4;
                    B[j + k][i + 5] = a5;
                    B[j + k][i + 6] = a6;
                    B[j + k][i + 7] = a7;
                }

                // 这个时候，8 * 8 的 B 全部都在缓存中，不会出现miss。
                transpose_matrix(j, i, 8, B);
            }
        }
    } else if (M == 64) {
        // 直接把 32 * 32 的代码放上去会有很高的miss。
        // 分析：
        // cache 是 32 组， 每组一块， 一块内 32 字节
        // 地址组成： Tag(64 - 5 - 5) | Index(5位) | Offset(5位)
        // 所以对于 A[4][0] 和 A[0][0], 因为一行为 64 个int，共 256 字节。
        // 256 * 4 = 1024 字节，这个时候 正好 A[4][0] 和 A[0][0] 的地址的低 10 位相同。
        // 所以会发生分到同一组的情况，并且导致 A[0][0] 被驱逐。因此 miss 率超高。
        // 一次最多处理 4 行，考虑 4 * 8, 也不行，因为这样解决不了写入 B 的时候同样情况。
        // 必须以 4 * 4 为单位进行处理，可是这样会导致每一行同时读入的 8 个 int 中后面会有 4 个int 用不到。
        // 下次再使用的时候会导致重新读入。
        // -------
        // | A| B|
        // -------
        // | C| D|
        // -------
        // 上面 ABCD 四个区域均为 4 * 4，用 AA 表示 A 的左上，BC表示 B 的左下，由此类推。
        // 先复制， AA -> BA, AB -> BB
        // 转置， 转置 BA， 转置 BB
        // 再复制， AC -> BC, AD -> BD
        // 转置， 转置 BC, 转置 BD
        // 这个时候 BA， BD 已经完成，不需要管。
        // 只需要按行交换 BC 即可

        for (i = 0;i < N; i += 8) {
            for (j = 0;j < M; j += 8) {
                // 左上 和 右上
                for (k = 0; k < 4; ++k) {
                    a0 = A[i + k][j + 0];
                    a1 = A[i + k][j + 1];
                    a2 = A[i + k][j + 2];
                    a3 = A[i + k][j + 3];
                    a4 = A[i + k][j + 4];
                    a5 = A[i + k][j + 5];
                    a6 = A[i + k][j + 6];
                    a7 = A[i + k][j + 7];

                    // 在行上写八个
                    B[j + k][i + 0] = a0;
                    B[j + k][i + 1] = a1;
                    B[j + k][i + 2] = a2;
                    B[j + k][i + 3] = a3;
                    B[j + k][i + 4] = a4;
                    B[j + k][i + 5] = a5;
                    B[j + k][i + 6] = a6;
                    B[j + k][i + 7] = a7;
                }

                transpose_matrix(j, i, 4, B);
                transpose_matrix(j, i + 4, 4, B);

                // 左下和右下。
                for (k = 0; k < 4; ++k) {
                    a0 = B[j + k][i + 4 + 0]; // B 右上
                    a1 = B[j + k][i + 4 + 1];
                    a2 = B[j + k][i + 4 + 2];
                    a3 = B[j + k][i + 4 + 3];

                    B[j + k][i + 4 + 0] = A[i + 4 + 0][j + k];
                    B[j + k][i + 4 + 1] = A[i + 4 + 1][j + k];
                    B[j + k][i + 4 + 2] = A[i + 4 + 2][j + k];
                    B[j + k][i + 4 + 3] = A[i + 4 + 3][j + k];

                    B[j + k + 4][i + 0 + 0] = a0;
                    B[j + k + 4][i + 0 + 1] = a1;
                    B[j + k + 4][i + 0 + 2] = a2;
                    B[j + k + 4][i + 0 + 3] = a3;
                    B[j + k + 4][i + 4 + 0] = A[i + 4 + 0][j + 4 + k];
                    B[j + k + 4][i + 4 + 1] = A[i + 4 + 1][j + 4 + k];
                    B[j + k + 4][i + 4 + 2] = A[i + 4 + 2][j + 4 + k];
                    B[j + k + 4][i + 4 + 3] = A[i + 4 + 3][j + 4 + k];
                }
            }
        }
    } else {
        // 61 * 67 其中 61 和 67 为质数
        // 8 -> 4 -> 2 -> 1 结果还是直接用 块大小为 16 更好。
        for (i = 0; i < N; i += 16) {
            for (j = 0; j < M; j += 16) {
                for (k = i; k < i + 16 && k < N; ++k) {
                    for (l = j; l < j + 16 && l < M; ++l) {
                        B[l][k] = A[k][l];
                    }
                }
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

