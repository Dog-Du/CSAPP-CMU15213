/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"

/*
 * Please fill in the following team struct
 */
team_t team = {
    "DogDu",              /* Team name */

    "DogDu",     /* First member full name */
    "dog_du@foxmail.com",  /* First member email address */

    "",                   /* Second member full name (leave blank if none) */
    ""                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/*
 * naive_rotate - The naive baseline version of rotate
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst)
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}

/*
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate 16 * 16分块";
void rotate(int dim, pixel *src, pixel *dst)
{
    int i, j;
    int k;
    // 优化点：
    // 1.分块 16 * 16
    // 2.最内层循环展开
    // 3.dst[RIDX(dim-1-j,i,dim)] = src[RIDX(i,j,dim)]; 每次 j 变化就需要重新进行乘法，
    //      dim - 1 - j 计算了 dim ^ 2 次，而且编译器不容易优化。
    //      修改为 dst[RIDX(i,j,dim)] = src[RIDX(j,dim-1-i,dim)]; 可以避免重复计算。

    // 最有效的优化是 1. 2. 至于 3. 优化的程度可能还没误差大。
     for (i = 0; i < dim; i += 16) {
         for (j = 0; j < dim; j += 16) {
             for (k = 0; k < 16; ++k) {
                 dst[RIDX(i + 0, j + k, dim)] = src[RIDX(j + k, dim - 1 - i - 0, dim)];
                 dst[RIDX(i + 1, j + k, dim)] = src[RIDX(j + k, dim - 1 - i - 1, dim)];
                 dst[RIDX(i + 2, j + k, dim)] = src[RIDX(j + k, dim - 1 - i - 2, dim)];
                 dst[RIDX(i + 3, j + k, dim)] = src[RIDX(j + k, dim - 1 - i - 3, dim)];
                 dst[RIDX(i + 4, j + k, dim)] = src[RIDX(j + k, dim - 1 - i - 4, dim)];
                 dst[RIDX(i + 5, j + k, dim)] = src[RIDX(j + k, dim - 1 - i - 5, dim)];
                 dst[RIDX(i + 6, j + k, dim)] = src[RIDX(j + k, dim - 1 - i - 6, dim)];
                 dst[RIDX(i + 7, j + k, dim)] = src[RIDX(j + k, dim - 1 - i - 7, dim)];
                 dst[RIDX(i + 8, j + k, dim)] = src[RIDX(j + k, dim - 1 - i - 8, dim)];
                 dst[RIDX(i + 9, j + k, dim)] = src[RIDX(j + k, dim - 1 - i - 9, dim)];
                 dst[RIDX(i +10, j + k, dim)] = src[RIDX(j + k, dim - 1 - i -10, dim)];
                 dst[RIDX(i +11, j + k, dim)] = src[RIDX(j + k, dim - 1 - i -11, dim)];
                 dst[RIDX(i +12, j + k, dim)] = src[RIDX(j + k, dim - 1 - i -12, dim)];
                 dst[RIDX(i +13, j + k, dim)] = src[RIDX(j + k, dim - 1 - i -13, dim)];
                 dst[RIDX(i +14, j + k, dim)] = src[RIDX(j + k, dim - 1 - i -14, dim)];
                 dst[RIDX(i +15, j + k, dim)] = src[RIDX(j + k, dim - 1 - i -15, dim)];
             }
         }
     }
}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_rotate_functions()
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);
    add_rotate_function(&rotate, rotate_descr);
    /* ... Register additional test functions here */
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
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
static void initialize_pixel_sum(pixel_sum *sum)
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
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum)
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/*
 * avg - Returns averaged pixel value at (i,j)
 */
static pixel avg(int dim, int i, int j, pixel *src)
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);

    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++)
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++)
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst)
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
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
char smooth_descr0[] = "smooth -- 重复使用计算结果，可适用于 dim >= 2";
void smooth(int dim, pixel *src, pixel *dst)
{
    if (dim <= 0) {
        return;
    }

    if (dim == 1) {
        *dst = *src;
        return;
    }

    int i, j;
    pixel_sum clr = {0,0,0, 0};
    pixel_sum b = clr, c = clr, d = clr;
    pixel_sum tmp = clr;

    pixel_add1(src[RIDX(0, 1, dim)], c);
    pixel_add1(src[RIDX(1, 1, dim)], c);

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


char smooth_descr1[] = "smooth -- 分块使用前缀和，仅可用于 dim % 16 == 0, 因为边界条件比较复杂而且不通用，懒得实现了。";
void smooth1(int dim, pixel *src, pixel *dst)
{
#define BLOCK_SIZE 16
#define BUFF_SIZE  (BLOCK_SIZE * BLOCK_SIZE)

    int i, j;
    pixel sum[BLOCK_SIZE][BLOCK_SIZE];

    naive_smooth(dim, src, dst);
}

/*********************************************************************
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&smooth, smooth_descr0);
    add_smooth_function(&smooth1, smooth_descr1);
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    /* ... Register additional test functions here */
}

