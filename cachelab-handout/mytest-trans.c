#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>


int main() {
  int n = 61;
  int m = 67;
  int A[n][m];
  int B[m][n];
  int C[n][m];

  for (int i = 0, counter = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      C[i][j] = A[i][j] = ++counter;
      B[j][i] = 0;
    }
  }

  transpose_submit(m, n, A, B);

  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      if (A[i][j] != C[i][j]) {
        printf("数组A被修改了.\n");
        printf("A :\n");
        for (int i = 0; i < n; ++i) {
          for (int j = 0; j < m; ++j) {
            printf("%d\t", A[i][j]);
          }
          printf("\n");
        }
        return 0;
      }
    }
  }

  int flag = 1;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      if (A[i][j] != B[j][i]) {
        printf("数组B有误.\n");
        flag = 0;
        break;
      }
    }

    if (flag == 0) {
      break;
    }
  }

  if (flag == 0) {
    printf("A :\n");
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < m; ++j) {
        printf("%d\t", A[i][j]);
      }
      printf("\n");
    }

    printf("B :\n");
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
        printf("%d\t", B[i][j]);
      }
      printf("\n");
    }

    trans(m, n, A, B);
    printf("expected B :\n");
    for (int i = 0; i < m; ++i) {
      for (int j = 0; j < n; ++j) {
        printf("%d\t", B[i][j]);
      }
      printf("\n");
    }
  } else {
    printf("over.\n");
  }
}

