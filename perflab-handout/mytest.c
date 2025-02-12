#include <stdalign.h>
#include <stdlib.h>
#include <stdio.h>

#define RIDX(i,j,n) ((i)*(n)+(j))

typedef struct {
  unsigned short red;
  unsigned short green;
  unsigned short blue;
} pixel;

typedef struct {
  int red;
  int green;
  int blue;
  int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/*
 * initialize_pixel_sum - Initializes all fields of sum to 0
 */
static void initialize_pixel_sum1(pixel_sum *sum)
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/*
 * accumulate_sum - Accumulates field values of p in corresponding
 * fields of sum
 */
static void accumulate_sum(pixel_sum *sum, pixel p)
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/*
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel
 */
static void assign_sum_to_pixel1(pixel *current_pixel, pixel_sum sum)
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

static pixel avg1(int dim, int i, int j, pixel *src)
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum1(&sum);

    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++)
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++)
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel1(&current_pixel, sum);
    return current_pixel;
}

void naive_smooth(int dim, pixel *src, pixel *dst)
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(i, j, dim)] = avg1(dim, i, j, src);
}

#define pixel_add0(x, y, z)   z.blue = (int)x.blue + (int)y.blue; z.green = (int)x.green + (int)y.green; z.red = (int)x.red + (int)y.red;
// #define pixel_sub0(x, y, z)   z.blue = (int)x.blue - (int)y.blue; z.green = (int)x.green - (int)y.green; z.red = (int)x.red - (int)y.red;

#define pixel_add1(x, y)   y.blue += (int)x.blue; y.green += (int)x.green; y.red += (int)x.red;
// #define pixel_sub1(x, y)   y.blue -= (int)x.blue; y.green -= (int)x.green; y.red -= (int)x.red;

#define sum4_assign(x, y)  y.blue = (unsigned short)(x.blue >> 2); y.red = (unsigned short)(x.red >> 2); y.green = (unsigned short)(x.green >> 2);
#define sum6_assign(x, y)  y.blue = (unsigned short)(x.blue / 6); y.red = (unsigned short)(x.red / 6); y.green = (unsigned short)(x.green / 6);
#define sum9_assign(x, y)  y.blue = (unsigned short)(x.blue / 9); y.red = (unsigned short)(x.red / 9); y.green = (unsigned short)(x.green / 9);

/*
 * smooth - Your current working version of smooth.
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst)
{
    int i, j;
    pixel_sum clr = {0,0,0, 0};
    pixel_sum b = clr, c = clr, d = clr;
    pixel_sum tmp = clr;

    // pixel_add1(src[RIDX(0, 1, dim)], c);
    c.blue += (int)src[((0) * (dim) + (1))].blue;
    c.green += (int)src[((0) * (dim) + (1))].green;
    c.red += (int)src[((0) * (dim) + (1))].red;

    // pixel_add1(src[RIDX(1, 1, dim)], c);
    c.blue += (int)src[((1) * (dim) + (1))].blue;
    c.green += (int)src[((1) * (dim) + (1))].green;
    c.red += (int)src[((1) * (dim) + (1))].red;

    b = c;
    pixel_add1(src[RIDX(0, 0, dim)], c);
    pixel_add1(src[RIDX(1, 0, dim)], c);
    sum4_assign(c, dst[RIDX(0, 0, dim)]);

    for (i = 1; i < dim - 1; ++i) {
        pixel_add0(src[RIDX(0, i + 1, dim)], src[RIDX(1, i + 1, dim)], d);
        pixel_add0(c, d, tmp);
        sum6_assign(tmp, dst[RIDX(0, i, dim)]);
        pixel_add0(b, d, c);
        b = d;
    }

    sum4_assign(c, dst[RIDX(0, dim - 1, dim)]);

    for (i = 1; i < dim - 1; ++i) {
        c = clr;
        pixel_add1(src[RIDX(i - 1, 1, dim)], c);
        pixel_add1(src[RIDX(i    , 1, dim)], c);
        pixel_add1(src[RIDX(i + 1, 1, dim)], c);
        b = c;
        pixel_add1(src[RIDX(i - 1, 0, dim)], c);
        pixel_add1(src[RIDX(i    , 0, dim)], c);
        pixel_add1(src[RIDX(i + 1, 0, dim)], c);
        sum6_assign(c, dst[RIDX(i, 0, dim)]);

        for (j = 1; j < dim - 1; ++j) {
            d = clr;
            pixel_add1(src[RIDX(i - 1, j + 1, dim)], d);
            pixel_add1(src[RIDX(i    , j + 1, dim)], d);
            pixel_add1(src[RIDX(i + 1, j + 1, dim)], d);
            pixel_add0(c, d, tmp);
            sum9_assign(tmp, dst[RIDX(i, j, dim)]);
            pixel_add0(b, d, c);
            b = d;
        }

        sum6_assign(c, dst[RIDX(i, dim - 1, dim)]);
    }

    c = clr;
    pixel_add1(src[RIDX(dim - 1, 1, dim)], c);
    pixel_add1(src[RIDX(dim - 2, 1, dim)], c);
    b = c;
    pixel_add1(src[RIDX(dim - 1, 0, dim)], c);
    pixel_add1(src[RIDX(dim - 2, 0, dim)], c);
    sum4_assign(c, dst[RIDX(dim - 1, 0, dim)]);

    for (i = 1; i < dim - 1; ++i) {
        pixel_add0(src[RIDX(dim - 1, i + 1, dim)], src[RIDX(dim - 2, i + 1, dim)], d);
        pixel_add0(c, d, tmp);
        sum6_assign(tmp, dst[RIDX(dim - 1, i, dim)]);
        pixel_add0(b, d, c);
        b = d;
    }

    sum4_assign(c, dst[RIDX(dim - 1, dim - 1, dim)]);
}

int main() {
  int n = 96;
  pixel a[n][n];
  pixel b[n][n];
  pixel c[n][n];

  for (int i = 0, counter = 0;i < n; ++i) {
    for (int j = 0;j < n; ++j) {
      ++counter;
      pixel tmp = {counter, counter, counter};
      c[i][j] = b[i][j] = a[i][j] = tmp;
    }
  }

  smooth(n, a, b);

  for (int i = 0; i < n ; ++i) {
    for (int j = 0; j < n; ++j) {
      if (a[i][j].blue != c[i][j].blue || a[i][j].green != c[i][j].green || a[i][j].red != c[i][j].red) {
        printf("A被修改\n");
        printf("origin A :\n");
        for (int i = 0;i < n; ++i) {
          for (int j = 0;j <n; ++j) {
            printf("%d\t", c[i][j].blue);
          }
          printf("\n");
        }

        printf("A :\n");
        for (int i = 0;i < n; ++i) {
          for (int j = 0;j <n; ++j) {
            printf("%d\t", a[i][j].blue);
          }
          printf("\n");
        }
        exit(0);
      }
    }
  }

  naive_smooth(n, a, c);

  for (int i = 0; i < n ; ++i) {
    for (int j = 0; j < n; ++j) {
      if (b[i][j].blue != c[i][j].blue || b[i][j].green != c[i][j].green || b[i][j].red != c[i][j].red) {
        printf("error\n");
        printf("expected B :\n");
        for (int i = 0;i < n; ++i) {
          for (int j = 0;j <n; ++j) {
            printf("%d\t", c[i][j].blue);
          }
          printf("\n");
        }

        printf("B :\n");
        for (int i = 0;i < n; ++i) {
          for (int j = 0;j <n; ++j) {
            printf("%d\t", b[i][j].blue);
          }
          printf("\n");
        }
        exit(0);
      }
    }
  }

  printf("over.\n");
}