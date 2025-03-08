#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

static const int N = 1e7;
static const int loop = 10000;

void merge1(long src1[], long src2[], long dest[], long n) {
  long i1 = 0;
  long i2 = 0;
  long id = 0;

  while (i1 < n && i2 < n) {
    if (src1[i1] < src2[i2])
      dest[id++] = src1[i1++];
    else
      dest[id++] = src2[i2++];
  }
}

void merge2(long src1[], long src2[], long dest[], long n) {
  long i1 = 0;
  long i2 = 0;
  long id = 0;

  while (i1 < n && i2 < n) {
    dest[id++] = src1[i1] < src2[i2] ? src1[i1++] : src2[i2++];
  }
}

void merge3(long src1[], long src2[], long dest[], long n) {
  long i1 = 0;
  long i2 = 0;
  long id = 0;

  while (i1 < n && i2 < n) {
    long v1 = src1[i1];
    long v2 = src2[i2];
    long take = v1 < v2;
    dest[id++] = take ? v1 : v2;
    i1 += take;
    i2 += (1-take);
  }
}

void shuffle(long src[], long n) {
  for (long i = 0; i < n; ++i) {
    src[i] = random() % n;
  }
}

int main(int argc, const char *args[]) {
  char c = args[1][0];
  long src1[N];
  long src2[N];
  long dest[2 * N];

  for (long i = 0; i < loop; ++i) {
    shuffle(src1, N);
    shuffle(src2, N);

    switch (c) {
      case '1':
        merge1(src1, src2, dest, N);
        break;
      case '2':
        merge2(src1, src2, dest, N);
        break;
      case '3':
        merge3(src1, src2, dest, N);
        break;
    }
  }
}