# 简介
本作业的目的是让您更加熟悉进程控制和信号处理的概念。您将通过编写一个支持作业控制的简单 Unix shell 程序来实现这一目标。

# 作业说明
您最多可以与一名组员合作完成本作业。所有提交均为电子形式。作业的任何澄清或修订内容将在课程网页上发布。

# 分发说明
**具体站点说明**：此处需插入一段说明，描述教师如何将 `shlab-handout.tar` 文件分发给学生。以下是卡内基梅隆大学使用的指导说明。

请先将 `shlab-handout.tar` 文件复制到您计划工作的受保护目录（实验目录）中，然后执行以下操作：
- 输入命令 `tar xvf shlab-handout.tar` 解压文件。
- 输入命令 `make` 编译并链接测试程序。
- 在 `tsh.c` 文件顶部的注释头中添加您的团队成员姓名和学号。

查看 `tsh.c`（微型 shell）文件，您会发现它包含一个简单 Unix shell 的功能框架。为帮助您入门，我们已经实现了部分基础功能。您的任务是完成以下列出的剩余空函数。作为参考，我们列出了参考答案中每个函数的大致代码行数（包含大量注释）：
- **`eval`**：解析和解释命令行的主例程。[70 行]
- **`builtin_cmd`**：识别并处理内置命令 `quit`、`fg`、`bg` 和 `jobs`。[25 行]
- **`do_bgfg`**：实现 `bg` 和 `fg` 内置命令。[50 行]
- **`waitfg`**：等待前台作业完成。[20 行]
- **`sigchld_handler`**：捕获 `SIGCHLD` 信号。[80 行]
- **`sigint_handler`**：捕获 `SIGINT`（Ctrl-C）信号。[15 行]
- **`sigtstp_handler`**：捕获 `SIGTSTP`（Ctrl-Z）信号。[15 行]

每次修改 `tsh.c` 文件后，输入 `make` 重新编译。要运行 shell，请在命令行输入 `tsh`：
```
unix> ./tsh
tsh> [在此输入命令]
```

# Unix Shell 概述
Shell 是一个交互式命令行解释器，代表用户运行程序。它会重复执行以下操作：打印提示符、等待标准输入中的命令行，然后根据命令行内容执行相应操作。

命令行是由空格分隔的 ASCII 文本序列。第一个词是内置命令名称或可执行文件的路径，其余词为命令行参数。若第一个词是内置命令，Shell 会立即在当前进程中执行；否则视为可执行程序路径，Shell 会创建子进程并在其中加载运行该程序。由单条命令行创建的子进程统称为**作业**。一个作业可由多个通过管道连接的子进程组成。

若命令行以 `&` 结尾，则作业在**后台**运行，Shell 无需等待作业结束即可继续处理下一条命令；否则作业在**前台**运行，Shell 需等待其结束后才接受新命令。因此，同一时间最多有一个前台作业，但可有任意数量的后台作业。

例如，输入以下命令：
```
tsh> jobs
```
会执行内置的 `jobs` 命令，而输入：
```
tsh> /bin/ls -l -d
```
会运行 `ls` 程序。若在此命令后添加 `&`，则程序会在后台运行。

---

会执行内置的 `jobs` 命令。输入以下命令行：
```
tsh> /bin/ls -l -d
```
会在前台运行 `ls` 程序。根据惯例，Shell 需确保当程序开始执行其主函数：
```c
int main(int argc, char *argv[])
```
时，参数 `argc` 和 `argv` 的值为：
- `argc == 3`,
- `argv[0] == "/bin/ls"`,
- `argv[1] == "-l"`,
- `argv[2] == "-d"`.

若输入以下命令行：
```
tsh> /bin/ls -l -d &
```
则 `ls` 程序会在后台运行。

Unix Shell 支持**作业控制**，允许用户在后台和前台之间切换作业，并修改作业中进程的状态（运行、停止或终止）。按下 `Ctrl-C` 会向**前台作业**的所有进程发送 `SIGINT` 信号，默认行为是终止进程；按下 `Ctrl-Z` 会发送 `SIGTSTP` 信号，默认行为是将进程置于**停止状态**，直到收到 `SIGCONT` 信号恢复运行。Shell 还提供以下内置命令支持作业控制：
- **`jobs`**：列出所有后台作业（运行或停止）。
- **`bg <job>`**：向停止的后台作业发送 `SIGCONT`，使其在后台继续运行。`<job>` 可以是进程 ID（PID）或作业 ID（JID）。
- **`fg <job>`**：向停止或运行的后台作业发送 `SIGCONT`，使其在前台继续运行。
- **`kill <job>`**：终止指定作业。

---

# tsh 规格要求
您的 `tsh` Shell 需满足以下功能：
- **提示符**：固定为 `tsh> `。
- **命令行解析**：用户输入的命令行由名称（内置命令或可执行文件路径）和若干参数组成，以空格分隔。若为内置命令，立即执行；否则，创建子进程运行该程序（此时**作业**即指该子进程）。
- **不支持管道和重定向**：无需处理 `|`、`<` 或 `>`。
- **信号处理**：
  - `Ctrl-C`（`SIGINT`）和 `Ctrl-Z`（`SIGTSTP`）信号需发送给**前台作业及其所有子进程**。若无前台作业，信号无效。
- **前后台作业**：
  - 命令行以 `&` 结尾时，作业在后台运行；否则在前台运行。
  - 同一时间最多一个前台作业，可多个后台作业。
- **作业标识**：
  - 作业可通过**进程 ID（PID）**或**作业 ID（JID，由 `tsh` 分配的正整数）**标识。JID 需以 `%` 前缀表示，如 `%5` 表示 JID 5，`5` 表示 PID 5。
- **内置命令**：
  - **`quit`**：退出 Shell。
  - **`jobs`**：列出所有后台作业。
  - **`bg <job>`**：重启停止的作业并在后台运行。
  - **`fg <job>`**：重启作业并在前台运行。
- **僵尸进程回收**：`tsh` 需回收所有僵尸子进程。若作业因未捕获的信号终止，需输出其 PID 和信号描述。

---

# 测试方法
我们提供了以下工具帮助测试：
**参考实现**：可执行文件 `tshref` 是 Shell 的参考答案。运行它以验证您实现的预期行为（除 PID 外，输出应完全一致）。

**Shell 驱动工具**：`sdriver.pl` 通过跟踪文件（trace file）向 Shell 发送命令和信号，并捕获输出。使用 `-h` 查看帮助：
```bash
unix> ./sdriver.pl -h
用法：sdriver.pl [-hv] -t <trace> -s <shellprog> -a <args>
选项：
  -h          显示帮助信息
  -v          显示详细输出
  -t <trace>  指定跟踪文件
  -s <shell>  指定待测试的 Shell 程序
  -a <args>   传递给 Shell 的参数
  -g          生成自动评分输出
```

**跟踪文件**：提供 16 个跟踪文件（`trace01.txt` 至 `trace16.txt`），与驱动工具配合测试。编号越低的测试越简单，越高的越复杂。例如，运行 `trace01.txt`：
```bash
unix> ./sdriver.pl -t trace01.txt -s ./tsh -a "-p"  # -a "-p" 表示不显示提示符
或
unix> make test01
```
与参考答案对比：
```bash
unix> ./sdriver.pl -t trace01.txt -s ./tshref -a "-p"
或
unix> make rtest01
```
文件 `tshref.out` 包含参考答案在所有跟踪文件上的输出，可供快速对照。

**示例输出**（以 `trace15.txt` 为例）：
```
bass> make test15
./sdriver.pl -t trace15.txt -s ./tsh -a "-p"
#
# trace15.txt - 综合测试
#
tsh> ./bogus
./bogus: 命令未找到
tsh> ./myspin 10
作业 (9721) 被信号 2 终止
tsh> ./myspin 3 &
[1] (9723) ./myspin 3 &
tsh> ./myspin 4 &
[2] (9725) ./myspin 4 &
tsh> jobs
[1] (9723) 运行中 ./myspin 3 &
[2] (9725) 运行中 ./myspin 4 &
tsh> fg %1
作业 [1] (9723) 被信号 20 停止
tsh> jobs
[1] (9723) 已停止 ./myspin 3 &
[2] (9725) 运行中 ./myspin 4 &
tsh> bg %3
%3: 无此作业
tsh> bg %1
[1] (9723) ./myspin 3 &
tsh> jobs
[1] (9723) 运行中 ./myspin 3 &
[2] (9725) 运行中 ./myspin 4 &
tsh> fg %1
tsh> quit
bass>
```

---

# 实现提示
- **阅读教材**：仔细阅读教材第 8 章（异常控制流）。
- **逐步开发**：从 `trace01.txt` 开始，确保输出与参考答案一致，再逐步处理更复杂的测试。
- **关键函数**：`waitpid`、`kill`、`fork`、`execve`、`setpgid` 和 `sigprocmask` 是核心。`waitpid` 的 `WUNTRACED` 和 `WNOHANG` 选项非常有用。
- **信号处理**：向**前台进程组**发送 `SIGINT` 或 `SIGTSTP` 时，需在 `kill` 函数中使用 `-pid`（进程组 ID）而非 `pid`。`sdriver.pl` 会检测此错误。
- **僵尸进程回收策略**：
  - 在 `waitfg` 中使用忙等待（`sleep` 循环）。
  - 在 `sigchld_handler` 中仅调用一次 `waitpid`。
  其他方案（如在两个函数中均调用 `waitpid`）可能导致混乱，建议仅在处理函数中回收。
- **信号阻塞**：在 `eval` 中，父进程需在 `fork` 前用 `sigprocmask` 阻塞 `SIGCHLD`，并在调用 `addjob` 添加作业后解除阻塞。子进程需在 `exec` 前解除 `SIGCHLD` 阻塞，因为其会继承父进程的阻塞信号集。


父进程需要以这种方式阻塞 `SIGCHLD` 信号，以避免**竞态条件**：子进程在父进程调用 `addjob` 之前就被 `sigchld_handler` 回收（从而从作业列表中移除）。

# 注意事项：
- 避免在 Shell 中运行 `more`、`less`、`vi`、`emacs` 等会修改终端设置的程序。建议使用 `/bin/ls`、`bin/ps`、`/bin/echo` 等简单文本程序。
- 当您从标准 Unix Shell 运行自己的 Shell 时，您的 Shell 属于**前台进程组**。默认情况下，其创建的子进程也会加入该组。此时按下 `Ctrl-C` 会向整个前台进程组（包括您的 Shell 及其所有子进程）发送 `SIGINT` 信号，这显然不符合预期。

**解决方案**：在 `fork` 后、`execve` 前，子进程需调用 `setpgid(0, 0)`，将其放入一个**新进程组**（组 ID 等于子进程 PID）。这确保前台进程组仅包含您的 Shell 自身。当按下 `Ctrl-C` 时，Shell 应捕获 `SIGINT` 并将其转发给对应的前台作业（确切地说，是包含该作业的进程组）。

---

### 评分标准
总分 90 分，分配如下：
- **正确性**（80 分）：通过 16 个跟踪文件测试，每个 5 分。
- **代码风格**（10 分）：
  - **注释质量**（5 分）：需有清晰的注释。
  - **系统调用检查**（5 分）：检查每个系统调用的返回值。

您的 Shell 将在 Linux 机器上使用实验目录中的驱动工具和跟踪文件进行测试。除以下情况外，输出应与参考实现一致：
- **PID 不同**：这是正常现象。
- **`/bin/ps` 输出差异**：在 `trace11.txt`、`trace12.txt`、`trace13.txt` 中，`/bin/ps` 的输出可能因运行而异，但其中 `mysplit` 进程的**运行状态**必须一致。

---

### 提交说明
**具体站点说明**：此处需插入一段说明，描述学生如何提交 `tsh.c` 文件。以下是卡内基梅隆大学使用的指导说明。

- **团队信息**：在 `tsh.c` 文件头部的注释中写明团队成员姓名和学号。
- **团队命名规则**：
  - 单人作业：格式为 `"ID"`（`ID` 为学号）。
  - 双人作业：格式为 `"ID1+ID2"`（`ID1` 和 `ID2` 分别为两人学号）。
- **提交命令**：
  ```bash
  make handin TEAM=teamname  # 将 teamname 替换为您的团队名称
  ```
- **重复提交**：若需修正后重新提交，每次递增版本号：
  ```bash
  make handin TEAM=teamname VERSION=2  # 版本号从 2 开始递增
  ```
- **提交验证**：检查文件是否成功提交至目录 `/afs/cs.cmu.edu/academic/class/15213-f01/L5/handin`（您有查看和写入权限，但无读取权限）。

祝您好运！