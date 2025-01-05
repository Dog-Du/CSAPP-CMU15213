***********************
The CS:APP Data Lab
Directions to Students
***********************

Your goal is to modify your copy of bits.c so that it passes all the
tests in btest without violating any of the coding guidelines.

你的目标是修改你的 bits.c 附件，让它通过所有在 btest 中的测试，同时不违反任何编码要求。
*********
0. Files:
*********

Makefile	- Makes btest, fshow, and ishow 编译 btest, fshow, ishow
README		- This file  本文件
bits.c		- The file you will be modifying and handing in 这是你需要修改和提交的文件
bits.h		- Header file  头文件
btest.c		- The main btest program 主要的 btest 程序
  btest.h	- Used to build btest   以下四个文件都是用来构建 btest
  decl.c	- Used to build btest
  tests.c       - Used to build btest
  tests-header.c- Used to build btest
dlc*		- Rule checking compiler binary (data lab compiler) 检查编译所得的二进制的规则
driver.pl*	- Driver program that uses btest and dlc to autograde bits.c 驱动程序使用 btest 和 dlc 来检测 bits.c
Driverhdrs.pm   - Header file for optional "Beat the Prof" contest  头文件，用于可选 "Beat the Prof" 内容
fshow.c		- Utility for examining floating-point representations  用于测试浮点数
ishow.c		- Utility for examining integer representations  用于测试整数

***********************************************************
1. Modifying bits.c and checking it for compliance with dlc
1. 修改 bits.c 并且检查它符合dlc的要求
***********************************************************

IMPORTANT: Carefully read the instructions in the bits.c file before
you start. These give the coding rules that you will need to follow if
you want full credit.

重要: 仔细阅读在 bits.c 中的指导在开始之前。他会告诉你需要遵守的编码要求。

Use the dlc compiler (./dlc) to automatically check your version of
bits.c for compliance with the coding guidelines:

       unix> ./dlc bits.c

使用 dlc 编译器，来自动检查你的 bits.c 是否遵守编码要求

dlc returns silently if there are no problems with your code.
Otherwise it prints messages that flag any problems.  Running dlc with
the -e switch:

    	unix> ./dlc -e bits.c

dlc 会沉默的返回，如果你的代码没有任何问题。否则他会打印信息。
运行 dlc 带着 -e 参数

causes dlc to print counts of the number of operators used by each function.

dlc打印在每个函数中的使用的运算符的数量。

Once you have a legal solution, you can test it for correctness using
the ./btest program.

如果你有合适的解决方法，你可以用 ./btest 程序来测试你的正确性

*********************
2. Testing with btest
2. 用 btest 测试
*********************

The Makefile in this directory compiles your version of bits.c with
additional code to create a program (or test harness) named btest.

这个目录下的 Makefile 编译你的 bits.c 带有额外代码，来构建一个叫做 btest 的测试程序

To compile and run the btest program, type:

    unix> make btest
    unix> ./btest [optional cmd line args]

You will need to recompile btest each time you change your bits.c
program. When moving from one platform to another, you will want to
get rid of the old version of btest and generate a new one.  Use the
commands:

    unix> make clean
    unix> make btest

你需要重新编译 btest 每次你修改 bits.c 程序。如果你把程序迁移到了另一个平台，
你会想要舍弃旧版本，生成新版本。请使用上面的命令

Btest tests your code for correctness by running millions of test
cases on each function.  It tests wide swaths around well known corner
cases such as Tmin and zero for integer puzzles, and zero, inf, and
the boundary between denormalized and normalized numbers for floating
point puzzles. When btest detects an error in one of your functions,
it prints out the test that failed, the incorrect result, and the
expected result, and then terminates the testing for that function.

btest 测试你的代码的正确性，通过运算上百万个测试情况再每个函数中。
他会测试很宽，包括但不限于 Tmin, zero 对于整型测试, 还有 zero, inf 和边界条件对于非规整和规整的数字
对于浮点数测试。当 btest 发现了你函数中有任何一个错误， 他会打印出来测试失败，失败的结果和希望的输出，并且终止该函数的测试。

Here are the command line options for btest:

  unix> ./btest -h
  Usage: ./btest [-hg] [-r <n>] [-f <name> [-1|-2|-3 <val>]*] [-T <time limit>]
    -1 <val>  Specify first function argument     指定第一个函数参数
    -2 <val>  Specify second function argument
    -3 <val>  Specify third function argument
    -f <name> Test only the named function      测试该函数
    -g        Format output for autograding with no error messages 整齐打印，而无错误信息。
    -h        Print this message  打印本信息
    -r <n>    Give uniform weight of n for all problems 给出规定的宽度，对于所有问题
    -T <lim>  Set timeout limit to lim  设置超时时间。

Examples:
示例 ：
  Test all functions for correctness and print out error messages:
  unix> ./btest

  Test all functions in a compact form with no error messages:
  unix> ./btest -g

  Test function foo for correctness:
  unix> ./btest -f foo

  Test function foo for correctness with specific arguments:
  unix> ./btest -f foo -1 27 -2 0xf

Btest does not check your code for compliance with the coding
guidelines.  Use dlc to do that.

btest 不会检查你的代码规范。请用 dlc 去检查。

*******************
3. Helper Programs
*******************

We have included the ishow and fshow programs to help you decipher
integer and floating point representations respectively. Each takes a
single decimal or hex number as an argument. To build them type:

    unix> make

我们会包含 ishow 和 fshow 程序来帮助清晰地你理解整型和浮点型表达。
每一个十进制或者16进制的数字都可以作为一个参数。来构建他们的类型。

Example usages:

    unix> ./ishow 0x27
    Hex = 0x00000027,	Signed = 39,	Unsigned = 39

    unix> ./ishow 27
    Hex = 0x0000001b,	Signed = 27,	Unsigned = 27

    unix> ./fshow 0x15213243
    Floating point value 3.255334057e-26
    Bit Representation 0x15213243, sign = 0, exponent = 0x2a, fraction = 0x213243
    Normalized.  +1.2593463659 X 2^(-85)

    linux> ./fshow 15213243
    Floating point value 2.131829405e-38
    Bit Representation 0x00e822bb, sign = 0, exponent = 0x01, fraction = 0x6822bb
    Normalized.  +1.8135598898 X 2^(-126)



