### 1 实验安排
本项目为个人独立完成。你必须在64位x86-64架构的机器上运行本实验。

**站点特定说明**：此处可添加其他管理事项，例如如何寻求帮助。

### 2 概述
本实验将帮助你理解高速缓存存储器对C程序性能的影响。
实验分为两部分：
- **第一部分**：编写一个模拟缓存行为的C程序（约200-300行代码）。
- **第二部分**：优化小型矩阵转置函数，目标是最小化缓存未命中次数。

### 3 下载实验材料
**站点特定说明**：此处需指导教师说明如何向学生分发`cachelab-handout.tar`文件。

操作步骤：
1. 将`cachelab-handout.tar`复制到受保护的Linux工作目录。
2. 执行以下命令解压：
   ```bash
   linux> tar xvf cachelab-handout.tar
   ```
   这将生成名为`cachelab-handout`的目录，内含多个文件。你需要修改以下两个文件：
   - `csim.c`
   - `trans.c`

3. 编译文件：
   ```bash
   linux> make clean
   linux> make
   ```

**重要警告**：
- 请勿使用Windows的WinZip程序打开`.tar`文件（部分浏览器默认会调用此类工具）。
- 应先将文件保存至Linux目录，再使用Linux的`tar`命令解压。
- **本课程所有文件操作必须严格在Linux系统下完成**，其他平台可能导致数据丢失（包括重要成果）！


### 4 实验描述
本实验分为两部分：
- **Part A**：实现一个缓存模拟器。
- **Part B**：编写一个针对缓存性能优化的矩阵转置函数。

#### 4.1 参考跟踪文件
实验材料目录中的`traces`子目录包含一组参考跟踪文件，这些文件将用于评估你在Part A中编写的缓存模拟器的正确性。这些跟踪文件由Linux程序`valgrind`生成。例如，在命令行中输入以下命令：
```bash
linux> valgrind --log-fd=1 --tool=lackey -v --trace-mem=yes ls -l
```
该命令会运行可执行程序`ls -l`，捕获其内存访问的顺序跟踪，并将结果打印到标准输出。

Valgrind内存跟踪的格式如下：
```
I 0400d7d4,8
M 0421c7f0,4
L 04f6b868,8
S 7ff0005c8,8
```
每一行表示一次或两次内存访问。每行的格式为：
```
[空格]操作 地址,大小
```
- **操作字段**表示内存访问的类型：
  - `I`表示指令加载（instruction load），
  - `L`表示数据加载（data load），
  - `S`表示数据存储（data store），
  - `M`表示数据修改（data modify，即一次数据加载后跟一次数据存储）。
- **注意**：
  - 每个`I`前没有空格，而每个`M`、`L`和`S`前都有一个空格。
- **地址字段**指定一个64位的十六进制内存地址。
- **大小字段**指定操作访问的字节数。

#### 4.2 Part A：编写缓存模拟器
在Part A中，你需要在`csim.c`中编写一个缓存模拟器。该模拟器以valgrind内存跟踪为输入，模拟缓存对该跟踪的命中/未命中行为，并输出总命中次数、未命中次数和替换次数。

我们为你提供了一个参考缓存模拟器的二进制可执行文件`csim-ref`，它可以模拟任意大小和关联度的缓存在valgrind跟踪文件上的行为。它使用LRU（最近最少使用）替换策略来选择要替换的缓存行。

参考模拟器的命令行参数如下：
```
用法：./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>
```
- `-h`：可选帮助标志，打印用法信息。
- `-v`：可选详细模式标志，显示跟踪信息。
- `-s <s>`：组索引位数（S = 2^s 是组数）。
- `-E <E>`：关联度（每组中的行数）。
- `-b <b>`：块位数（B = 2^b 是块大小）。
- `-t <tracefile>`：要回放的valgrind跟踪文件名。

这些命令行参数基于CS:APP2e教材第597页的符号（s、E和b）。例如：
```bash
linux> ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace
hits:4 misses:5 evictions:3
```
在详细模式下的相同示例：
```bash
linux> ./csim-ref -v -s 4 -E 1 -b 4 -t traces/yi.trace
L 10,1 miss
M 20,1 miss hit
L 22,1 hit
S 18,1 hit
L 110,1 miss eviction
L 210,1 miss eviction
M 12,1 miss eviction hit
hits:4 misses:5 evictions:3
```

你在Part A的任务是填充`csim.c`文件，使其能够接受相同的命令行参数并生成与参考模拟器相同的输出。注意，该文件几乎完全为空，你需要从头开始编写。

#### Part A的编程规则
- 在`csim.c`文件的头部注释中包含你的姓名和登录ID。
- 你的`csim.c`文件必须能够无警告编译，否则无法获得分数。
- 你的模拟器必须能够正确处理任意的`s`、`E`和`b`。这意味着你需要使用`malloc`函数为模拟器的数据结构分配存储空间。输入`man malloc`可以查看该函数的相关信息。
- 在本实验中，我们只关注数据缓存的性能，因此你的模拟器应忽略所有指令缓存访问（以`I`开头的行）。注意，valgrind始终将`I`放在第一列（前面没有空格），而`M`、`L`和`S`放在第二列（前面有空格）。这可能有助于你解析跟踪文件。
- 为了获得Part A的分数，你必须在`main`函数的末尾调用`printSummary`函数，并传入总命中次数、未命中次数和替换次数：
  ```c
  printSummary(hit_count, miss_count, eviction_count);
  ```
- 在本实验中，你可以假设内存访问是正确对齐的，因此单个内存访问不会跨越块边界。基于这一假设，你可以忽略valgrind跟踪文件中的请求大小。


### 4.3 Part B: 优化矩阵转置

在 Part B 中，你将编写一个 transpose 函数（位于 trans.c 中），使其尽可能减少缓存未命中次数。

设 A 表示一个矩阵，Aij 表示第 i 行第 j 列的元素。A 的转置矩阵，记为 AT，是一个矩阵，使得 Aij = AT_ji。

为了帮助你入门，我们已在 trans.c 中提供了一个示例转置函数，该函数计算 N × M 矩阵 A 的转置，并将结果存储在 M × N 矩阵 B 中：
```c
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
```
示例转置函数是正确的，但由于其访问模式导致相对较多的缓存未命中，因此效率较低。
你在 Part B 中的任务是编写一个类似的函数，名为 transpose_submit，使其在不同大小的矩阵上尽可能减少缓存未命中次数：
```c
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]);
```
请勿更改你的 transpose_submit 函数的描述字符串（“Transpose submission”）。自动评分程序会搜索此字符串，以确定要评估的转置函数。
Part B 的编程规则：
在 trans.c 的文件头注释中包含你的姓名和登录 ID。
你的 trans.c 代码必须在编译时无警告，才能获得评分。
每个转置函数最多允许定义 12 个 int 类型的局部变量。
  The reason for this restriction is that our testing code is not able to count references to the stack. We want you to limit your references to the stack and focus on the access patterns of the source and destination arrays.
不允许通过使用 long 类型变量或任何位技巧来绕过上述规则，将多个值存储到单个变量中。
你的转置函数不得使用递归。
如果你选择使用辅助函数，则在你的辅助函数和顶层转置函数之间，任何时候堆栈上的局部变量总数不得超过 12 个。例如，如果你的转置函数声明了 8 个变量，然后调用一个使用 4 个变量的函数，该函数又调用另一个使用 2 个变量的函数，你将有 14 个变量在堆栈上，这将违反规则。
你的转置函数不得修改数组 A。然而，你可以对数组 B 的内容进行任意操作。
你不得在代码中定义任何数组，也不得使用任何 malloc 的变体。


### 5 评估

本节描述了如何评估你的工作。本次实验的总分为 60 分：
- Part A：27 分
- Part B：26 分
- 风格：7 分

### 5.1 Part A 的评估

对于 Part A，我们将使用不同的缓存参数和跟踪文件运行你的缓存模拟器。共有八个测试用例，每个用例值 3 分，最后一个用例值 6 分：
linux> ./csim -s 1 -E 1 -b 1 -t traces/yi2.trace
linux> ./csim -s 4 -E 2 -b 4 -t traces/yi.trace
linux> ./csim -s 2 -E 1 -b 4 -t traces/dave.trace
linux> ./csim -s 2 -E 1 -b 3 -t traces/trans.trace
linux> ./csim -s 2 -E 2 -b 3 -t traces/trans.trace
linux> ./csim -s 2 -E 4 -b 3 -t traces/trans.trace
linux> ./csim -s 5 -E 1 -b 5 -t traces/trans.trace
linux> ./csim -s 5 -E 1 -b 5 -t traces/long.trace
复制

你可以使用参考模拟器 `csim-ref` 来获取每个测试用例的正确答案。
在调试过程中，使用 `-v` 选项可以获取每次命中和未命中的详细记录。
对于每个测试用例，输出正确的缓存命中、未命中和驱逐次数将为你赢得该测试用例的全部分数。
每个测试用例中，命中、未命中和驱逐次数的报告各占该测试用例分数的 1/3。也就是说，如果某个测试用例值 3 分，而你的模拟器输出了正确的命中和未命中次数，但报告了错误的驱逐次数，那么你将获得 2 分。

### 5.2 Part B 的评估

对于 Part B，我们将评估你的 `transpose_submit` 函数在三种不同大小的输出矩阵上的正确性和性能：
- 32 × 32 (M = 32, N = 32)
- 64 × 64 (M = 64, N = 64)
- 61 × 67 (M = 61, N = 67)

#### 5.2.1 性能 (26 分)

对于每种矩阵大小，你的 `transpose_submit` 函数的性能将通过以下方式评估：
使用 `valgrind` 提取你的函数的地址跟踪，然后使用参考模拟器在缓存参数为 (s = 5, E = 1, b = 5) 的缓存上重放此跟踪。

每种矩阵大小的性能分数会根据未命中次数 `m` 线性变化，直到某个阈值：
- 32 × 32：如果 `m < 300`，得 8 分；如果 `m > 600`，得 0 分
- 64 × 64：如果 `m < 1,300`，得 8 分；如果 `m > 2,000`，得 0 分
- 61 × 67：如果 `m < 2,000`，得 10 分；如果 `m > 3,000`，得 0 分

你的代码必须正确才能获得特定大小的性能分数。你的代码只需在这三种情况下正确，并且可以针对这三种情况专门优化。特别是，你的函数可以明确检查输入大小，并为每种情况实现专门优化的代码，这是完全可以的。

### 5.3 风格评估

代码风格有 7 分。这些分数将由课程工作人员手动分配。风格指南可以在课程网站上找到。
课程工作人员将检查你在 Part B 中的代码是否存在非法数组和过多的局部变量。


## 6 实验室工作

### 6.1 Part A 的工作

我们为你提供了一个自动评分程序，名为 `test-csim`，用于测试你的缓存模拟器在参考跟踪文件上的正确性。在运行测试之前，请确保编译你的模拟器：

```bash
linux> make
linux> ./test-csim
以下是一个示例输出，显示了你的模拟器与参考模拟器的结果对比：
复制
Your simulator Reference simulator
Points (s,E,b) Hits Misses Evicts Hits Misses Evicts
3 (1,1,1) 9 8 6 9 8 6 traces/yi2.trace
3 (4,2,4) 4 5 2 4 5 2 traces/yi.trace
3 (2,1,4) 2 3 1 2 3 1 traces/dave.trace
3 (2,1,3) 167 71 67 167 71 67 traces/trans.trace
3 (2,2,3) 201 37 29 201 37 29 traces/trans.trace
3 (2,4,3) 212 26 10 212 26 10 traces/trans.trace
3 (5,1,5) 231 7 0 231 7 0 traces/trans.trace
6 (5,1,5) 265189 21775 21743 265189 21775 21743 traces/long.trace
```
对于每次测试，它会显示你获得的分数、缓存参数、输入跟踪文件，以及你的模拟器与参考模拟器结果的对比。
以下是一些关于 Part A 的提示和建议：
在小型跟踪文件（如 traces/dave.trace）上进行初步调试。
参考模拟器接受一个可选的 -v 参数，用于启用详细输出，显示每次内存访问的命中、未命中和驱逐情况。你无需在 csim.c 中实现此功能，但我们强烈建议你这样做，因为它可以帮助你通过直接比较参考模拟器的行为来调试。
我们建议你使用 getopt 函数解析命令行参数。你需要以下头文件：
```c
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
```
详情请参见 man 3 getopt。
每次数据加载（L）或存储（S）操作最多导致一次缓存未命中。数据修改操作（M）被视为一次加载后跟一次存储到同一地址。因此，M 操作可能导致两次命中，或一次未命中和一次命中加上可能的驱逐。
如果你想使用 15-122 课程中的 C0 风格契约，可以包含我们为方便起见在讲义目录中提供的 contracts.h。
6.2 Part B 的工作
我们为你提供了一个自动评分程序，名为 test-trans.c，用于测试你注册的每个转置函数的正确性和性能。
你可以在 trans.c 文件中注册多达 100 个转置函数版本。每个转置函数版本的格式如下：
```c
/* 头部注释 */
char trans_simple_desc[] = "A simple transpose";
void trans_simple(int M, int N, int A[N][M], int B[M][N])
{
    /* 你的转置代码 */
}
```
通过在 trans.c 的 registerFunctions 函数中调用以下形式的代码，将特定的转置函数注册到自动评分程序中：
```c
registerTransFunction(trans_simple, trans_simple_desc);
```
在运行时，自动评分程序将评估每个注册的转置函数并打印结果。当然，其中一个注册的函数必须是你提交以获得分数的 transpose_submit 函数：
```c
registerTransFunction(transpose_submit, transpose_submit_desc);
```
请参阅默认的 trans.c 函数以了解其工作原理。
自动评分程序将矩阵大小作为输入。它使用 valgrind 为每个注册的转置函数生成跟踪文件。然后，它通过在缓存参数为 (s = 5, E = 1, b = 5) 的缓存上运行参考模拟器来评估每个跟踪文件。
例如，要测试你在 32 × 32 矩阵上的注册转置函数，请重新构建 test-trans，然后使用适当的 M 和 N 值运行它：
```bash
linux> make
linux> ./test-trans -M 32 -N 32
```
示例输出如下：
```bash
Step 1: Evaluating registered transpose funcs for correctness:
func 0 (Transpose submission): correctness: 1
func 1 (Simple row-wise scan transpose): correctness: 1
func 2 (column-wise scan transpose): correctness: 1
func 3 (using a zig-zag access pattern): correctness: 1
Step 2: Generating memory traces for registered transpose funcs.
Step 3: Evaluating performance of registered transpose funcs (s=5, E=1, b=5)
func 0 (Transpose submission): hits:1766, misses:287, evictions:255
func 1 (Simple row-wise scan transpose): hits:870, misses:1183, evictions:1151
func 2 (column-wise scan transpose): hits:870, misses:1183, evictions:1151
func 3 (using a zig-zag access pattern): hits:1076, misses:977, evictions:945
Summary for official submission (func 0): correctness=1 misses=287
```
在这个示例中，我们在 trans.c 中注册了四个不同的转置函数。test-trans 程序测试了每个注册的函数，显示了每个函数的结果，并提取了官方提交的结果。
以下是一些关于 Part B 的提示和建议：
test-trans 程序将函数 i 的跟踪文件保存在 trace.fi 文件中。这些跟踪文件是无价的调试工具，可以帮助你了解每个转置函数的命中和未命中来源。要调试特定函数，只需使用详细选项运行其跟踪文件通过参考模拟器：
```bash
linux> ./csim-ref -v -s 5 -E 1 -b 5 -t trace.f0
S 68312c,1 miss
L 683140,8 miss
L 683124,4 hit
L 683120,4 hit
L 603124,4 miss eviction
S 6431a0,4 miss
...
```
由于你的转置函数是在直接映射缓存上评估的，冲突未命中是一个潜在问题。考虑你的代码中冲突未命中的可能性，特别是在对角线上。尝试想出减少这些冲突未命中数量的访问模式。
阻塞是一种减少缓存未命中的有用技术。更多信息请参见 http://csapp.cs.cmu.edu/public/waside/waside-blocking.pdf。
6.3 整合工作
我们为你提供了一个驱动程序，名为 ./driver.py，用于对你的模拟器和转置代码进行全面评估。这与你的老师用来评估你提交作业的程序相同。该驱动程序使用 test-csim 评估你的模拟器，并使用 test-trans 评估你提交的转置函数在三种矩阵大小上的性能。然后，它打印你的结果摘要和你获得的分数。
要运行驱动程序，请输入：
```bash
linux> ./driver.py
```
7 提交作业
每次你在 cachelab-handout 目录中输入 make 时，Makefile 会创建一个名为 userid-handin.tar 的压缩包，其中包含你当前的 csim.c 和 trans.c 文件。
SITE-SPECIFIC: 在此处插入文本，告诉每个学生如何在你的学校提交他们的 userid-handin.tar 文件。
重要提示：不要在 Windows 或 Mac 机器上创建提交压缩包，也不要提交其他压缩格式的文件，如 .zip、.gzip 或 .tgz 文件。