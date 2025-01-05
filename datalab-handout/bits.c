/*
 * CS:APP Data Lab
 *
 * <Please put your name and userid here>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 *
 * 警告 ： 不要包含头文件 stdio.h ，他会让 dlc 困惑。你仍然可以使用 printf
 * 来debug
 * 即使不包含这个头文件，虽然你会得到编译器的警告。正常来说，忽视编译器警告是不合适的操作，
 * 但在这里是可以的。
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code
  must conform to the following style:

  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>



  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.




  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.


You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.



/*
 * STEP 2: Modify the following functions according the coding rules.
 *
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce
 *      the correct answers.
 */

#endif
// 1
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
// x ^ y == (~x & y) | (x & ~y) = ~(~(~x & y) & ~(x & ~y))
int bitXor(int x, int y) { return ~(~(~x & y) & ~(x & ~y)); }
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 *
 */
int tmin(void) { return 1 << 31; }
// 2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 * Tmax == 0111, Tmax + Tmax == 0111 + 0111 == 1110
 * Tmax + 1 == ~Tmax ->  x+1 == ~x -> ((x+1) ^ (~x))
 * 错误样例为 -1， -1 是 1111，也符合这个条件，需要排除
 * -1 + 1 == 0，可以排除。
 * 故： 用 (x+1) ^ (~x) 判断是否为 -1 或者 1111, 用 !(x+1) 判断是否为-1
 * 再用 ！ 规整到 0,1 最后进行 & 操作，得到 0,1
 */
int isTmax(int x) { return ((!((x + 1) ^ (~x))) & (!!(x + 1))); }
/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 * 最小满足条件就是 0xAAAAAAAA，然后 x & a == a 即可。
 */
int allOddBits(int x) {
  int a = 0xAA;
  a = a | (a << 8);
  a = a | (a << 16);
  a = a | (a << 24);
  return !((a & x) ^ a);
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) { return ~x + 1; }
// 3
/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0'
 * to '9') Example: isAsciiDigit(0x35) = 1. isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 * 思路 ： 利用 Tmin - 1 >=0。
 * 那么 Tmin + 0x2F - x >= 0 ===> x >= 0x30 且
 *      Tmin + 0x39 - x < 0 ===> x <= 0x39
 */

// 但是这样写出来的代码虽然正确，但是会出现溢出
// 在O0以上的优化的时候，会出现UB，导致输出答案错误，只能使用O0优化。
// int isAsciiDigit(int x) {
//   int tmp = 1 << 31;
//   int a = tmp + 0x2F;
//   int flag1 = !(((a + (~x + 1)) >> 31) & 1);
//   int b = tmp + 0x39;
//   int flag2 = (((b + (~x + 1)) >> 31) & 1);
//   return flag1 & flag2;
// }

// 正确答案 ： 仅仅跨过0，而不利用溢出。
int isAsciiDigit(int x) {
  int a = ~0x30 + 1;
  int b = ~0x3a + 1;
  int sign1 = ((a + x) >> 31) & 1;
  int sign2 = ((b + x) >> 31) & 1;
  return (!sign1) & sign2;
}

/*
 * conditional - same as x ? y : z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 *  利用算术右移，把x变成符号位，实现全0，全1.
 */
int conditional(int x, int y, int z) {
  int f = !!x;
  int xx = (f << 31) >> 31;
  int not_x = ~xx;
  return (not_x & z) | (xx & y);
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 * 思路 ： 先排除正负影响，保证x,y同号。
 * 并且排除 x==y， 最后查看 x < y即可
 * 想利用 先减再加或者先加再减 判断符号是否变化
 * 然后使用 Tmax 先减去x，再加上 y 判断符号位即可。
 * 不用 Tmin 是因为会导致操作符过多
 */
int isLessOrEqual(int x, int y) {
  int a = ~(1 << 31) + ~x + 1;
  int sign_x = (x >> 31) & 1;
  int sign_y = (y >> 31) & 1;
  int flag1 = sign_x & !sign_y;   // x负，y正
  int flag2 = (!sign_x) & sign_y; // x正，y负
  return (!flag2) & (flag1 | !(x ^ y) | (((a + y) >> 31) & 1));
}
// 4
/*
 * logicalNeg - implement the ! operator, using all of
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 *
 *  如果是 0， 改成 1即可，如果是非0，则要么是正数，要么是负数。
 *  取补码，如果符号位相或为1，则为非0，
 */

int logicalNeg(int x) { return ((x | (~x + 1)) >> 31) + 1; }

// int logicalNeg(int x) {
//   int sign1 = (x >> 31) & 1;
//   int sign2 = ((~x + 1) >> 31) & 1;
//   int flag = (sign1 | sign2); // 规整到了 0,1
//   return (~flag) & 1;
// }

/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 *  思路： 在补码下，即求正数最高位的1和负数最高位的0，把这个位置 + 1即可。
 *  为了统一处理，把负数求0转化成求1，即翻转。
 *  方式是：利用近似二分的方法，
 */
int howManyBits(int x) {
  int b16, b8, b4, b2, b1, b0;
  int sign = x >> 31;
  x = sign ^ x; // 如果x是负，则取反，如果x是正，则不变。

  // 查看高16位是否有1， 如果有1，则b16被置为 2^4
  b16 = (!!(x >> 16)) << 4;
  x = x >>
      b16; // 然后舍弃，如果高16位有，则不再使用低16位；如果高16位没有，则数字不变。下面同理。
  b8 = (!!(x >> 8)) << 3;
  x = x >> b8;
  b4 = (!!(x >> 4)) << 2;
  x = x >> b4;
  b2 = (!!(x >> 2)) << 1;
  x = x >> b2;
  b1 = (!!(x >> 1)) << 0;
  x = x >> b1;
  b0 = x;

  // 把位置转化为数字。
  return b16 + b8 + b4 + b2 + b1 + b0 + 1;
}
// float
/*
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  unsigned sign = uf & (1 << 31);
  unsigned exp = (uf >> 23) & 0xFF;
  unsigned frc = (uf << 9) >> 9;

  // 特殊值，NaN或者无穷，不需要处理
  if (exp == 0xFF) {
    return uf;
  }

  // 非规格值，说白了就是尾数*2，但是要考虑进位，因为exp为0，所以直接这样进位即可。
  if (exp == 0) {
    return sign | (frc << 1);
  }

  // 规格值，exp + 1即可。
  return sign | ((exp + 1) << 23) | frc;
}

/*
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  int sign = uf & (1 << 31);
  int exp = (uf >> 23) & 0xFF;
  int frc = (uf << 9) >> 9;
  int out_of_range = 0x80000000u;

  // 非规格值，可以直接返回0.
  if (exp == 0) {
    return 0;
  }

  // 特殊值，NaN或者无穷，肯定超出范围
  if (exp == 0xFF) {
    return out_of_range;
  }

  // 规格值。
  exp -= 0x7F; // 原本阶数

  if (exp < 0) { // 阶数小于0，必然是0
    return 0;
  }

  if (exp == 0) { // 阶数==0，负数为-1，正数为1
    if (sign) {
      return -1;
    }
    return 1;
  }

  frc = frc | (1 << 23); // 把省去的1补上。

  // 现在是 1.******, 小数点后为23位，int为31表达数字，一位表达符号
  // 所以当exp > 31时，移位超过符号位，所以超过范围。
  if (exp > 31) {
    return out_of_range;
  }

  // 如果x<=23，需要舍去部分小数点。
  if (exp <= 23) {
    frc = frc >> (23 - exp);
  }

  // 大于23，全部都为正数。
  if (exp > 23) {
    frc = frc << (exp - 23);
  }

  // 取补。
  if (sign) {
    frc = ~frc + 1;
  }
  return frc;
}
/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  if (x <= -150) {
    return 0;
  } else if (x <= -127) {
    return (1 << 23) >> (-126 - x);
  } else if (x <= 127) {
    return (0x7f + x) << 23;
  } else {
    return 0x7f800000u;
  }
}