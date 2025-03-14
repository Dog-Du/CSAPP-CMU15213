（由于技术原因，联网搜索暂不可用）

引言
本次作业将帮助您深入理解IA-32调用约定与栈结构。您需要通过实验目录下的可执行文件`bufbomb`，完成一系列缓冲区溢出攻击实践。

**注意事项**
本实验将让您亲身实践一种常用于利用操作系统与网络服务器安全漏洞的攻击方法。实验目的是帮助您：
1. 学习程序运行时的工作原理
2. 理解此类安全漏洞的本质特征
3. 掌握在编写系统代码时规避此类问题的方法

我们明确声明：**不鼓励**使用本实验或其他任何形式的攻击手段非法获取系统资源。此类行为受刑事法规约束。

实验安排
本实验需**个人独立完成**。

我们使用GCC的`-m32`编译选项构建实验环境，因此即使宿主机为x86-64架构系统，编译器生成的所有代码仍遵循IA-32规范。这充分说明：只要保持调用约定的一致性，编译器可采用任意自定义的调用规则。

服务器将通过浏览器返回名为`buflab-handout.tar`的压缩文件。操作步骤如下：

使用引言：

1. 将`buflab-handout.tar`复制至您准备工作的**受保护目录**
2. 执行解压命令：
   ```bash
   tar xvf buflab-handout.tar
   ```
   该命令将生成`buflab-handout`目录，内含以下三个可执行文件：
   - `bufbomb`: 待攻击的缓冲区炸弹程序
   - `makecookie`: 根据用户ID生成唯一"身份凭证"（cookie）
   - `hex2raw`: 字符串格式转换工具

**环境要求**
后续操作均假设：
- 您已将上述三个程序复制到本地受保护目录
- 所有命令均在该目录下执行

BUFBOMB 程序从标准输入读取一个字符串。它通过下面定义的 getbuf 函数来实现：
1 /* getbuf 的缓冲区大小 */
2 #define NORMAL_BUFFER_SIZE 32
3
4 int getbuf()
5 {
6     char buf[NORMAL_BUFFER_SIZE];
7     Gets(buf);
8     return 1;
9 }
Gets 函数类似于标准库函数 gets——它从标准输入读取一个字符串（以 ‘\n’ 或文件结束符结尾），并将其（连同空终止符）存储在指定的目标位置。在这段代码中，你可以看到目标是一个名为 buf 的数组，它有足够的空间存储 32 个字符。

Gets（以及 gets）从输入流中获取一个字符串并将其存储到目标地址（此处为 buf）。然而，Gets() 无法确定 buf 是否足够大以存储整个输入。它只是简单地复制整个输入字符串，可能会超出分配给目标的存储空间的边界。
如果用户输入的字符串长度不超过 31 个字符，很明显 getbuf 将返回 1，如下执行示例所示：
unix> ./bufbomb -u bovik
输入字符串：I love 15-213.
哑弹：getbuf 返回了 0x1
通常，如果我们输入一个更长的字符串，就会发生错误：
unix> ./bufbomb -u bovik
输入字符串：当你是助教时，爱这门课更容易。
哎呀！你导致了一个段错误！
正如错误消息所指出的，超出缓冲区通常会导致程序状态被破坏，从而引发内存访问错误。你的任务是更巧妙地输入字符串给 BUFBOMB，让它做一些更有趣的事情。这些字符串称为利用字符串。
BUFBOMB 接受几个不同的命令行参数：
-u userid：为指定的 userid 操作炸弹。你应该始终提供此参数，原因如下：
• 它是向评分服务器提交成功攻击的必需条件。
• BUFBOMB 根据你的 userid 确定你将使用的 cookie，MAKECOOKIE 程序也是如此。
• 我们在 BUFBOMB 中构建了一些功能，使得你需要使用的一些关键栈地址取决于你的 userid 的 cookie。
-h：打印可能的命令行参数列表。
-n：以“Nitro”模式操作，如下文 Level 4 中使用的那样。
-s：向评分服务器提交你的解决方案利用字符串。
此时，你应该思考一下 x86 栈结构，并确定你将针对栈中的哪些条目。你可能还想知道为什么最后一个示例导致了段错误，尽管这不太清楚。
你的利用字符串通常包含不对应于可打印字符的 ASCII 值的字节值。程序 HEX2RAW 可以帮助你生成这些原始字符串。它接受一个十六进制格式的字符串作为输入。在这种格式中，每个字节值由两个十六进制数字表示。例如，字符串 “012345” 可以以十六进制格式输入为 “30 31 32 33 34 35”。（回忆一下，十进制数字 x 的 ASCII 码是 0x3x。）
传递给 HEX2RAW 的十六进制字符应该以空白（空格或换行符）分隔。我建议在处理时用换行符分隔利用字符串的不同部分。HEX2RAW 还支持 C 风格的块注释，因此你可以标记利用字符串的部分。例如：
bf 66 7b 32 78 /* 将 $0x78327b66 移动到 %edi /
确保在开始和结束注释字符串（‘/’ 和 ‘*/’）周围留有空白，以便它们能被正确忽略。
如果在文件 exploit.txt 中生成了一个十六进制格式的利用字符串，你可以通过几种不同的方式将原始字符串应用于 BUFBOMB：
你可以设置一系列管道，将字符串通过 HEX2RAW 传递。
unix> cat exploit.txt | ./hex2raw | ./bufbomb -u bovik
你可以将原始字符串存储在文件中，并使用 I/O 重定向将其提供给 BUFBOMB：
unix> ./hex2raw < exploit.txt > exploit-raw.txt
unix> ./bufbomb -u bovik < exploit-raw.txt
这种方法也可以在 GDB 中运行 BUFBOMB 时使用：
unix> gdb bufbomb
(gdb) run -u bovik < exploit-raw.txt
重要提示：
• 你的利用字符串在任何中间位置都不能包含字节值 0x0A，因为这是换行符（‘\n’）的 ASCII 码。当 Gets 遇到这个字节时，它会假设你打算终止字符串。
• HEX2RAW 期望两个数字的十六进制值以空白分隔。因此，如果你想创建一个十六进制值为 0 的字节，你需要指定 00。要创建字 0xDEADBEEF，你应该传递 EF BE AD DE 给 HEX2RAW。
当你正确解决了一个关卡，比如关卡 0 时：
../hex2raw < smoke-bovik.txt | ../bufbomb -u bovik
用户ID：bovik
Cookie：0x1005b2b7
输入字符串：Smoke!：你调用了 smoke()
有效
干得好！
然后你可以使用 -s 选项将你的解决方案提交到评分服务器：
./hex2raw < smoke-bovik.txt | ./bufbomb -u bovik -s
用户ID：bovik
Cookie：0x1005b2b7
输入字符串：Smoke!：你调用了 smoke()
有效
已将利用字符串发送到服务器进行验证。
干得好！

服务器将测试你的利用字符串以确保其确实有效，并将更新缓冲区实验室的记分牌页面，表明你的用户ID（通过你的 Cookie 列出，以保护隐私）已完成此关卡。
你可以通过将 Web 浏览器指向
http://$Buflab::SERVER_NAME:18213/scoreboard
查看记分牌
与炸弹实验室不同，这个实验室不会因为犯错而受到惩罚。尽管向 BUFBOMB 输入任何你喜欢的字符串吧。当然，你也不应该对这个实验室进行暴力破解，因为这会比你完成作业的时间更长。
重要提示：你可以在任何 Linux 机器上进行缓冲区炸弹的制作，但为了提交你的解决方案，你需要在以下机器之一上运行：
讲师：插入你在 buflab/src/config.h 中建立的合法域名列表。

第0级：蜡烛（10分）
函数getbuf在BUFBOMB中被函数test调用，test函数的C代码如下：
1 void test()
2 {
3 int val;
4 /* 在栈上放置一个哨兵值，以检测可能的破坏 /
5 volatile int local = uniqueval();
6
7 val = getbuf();
8
9 / 检查栈是否被破坏 */
10 if (local != uniqueval()) {
11 printf("破坏！：栈已被破坏\n");
12 }
13 else if (val == cookie) {
14 printf("爆炸！：getbuf返回了0x%x\n", val);
15 validate(3);
16 } else {
17 printf("哑弹：getbuf返回了0x%x\n", val);
18 }
19 }
当getbuf执行其返回语句（getbuf的第5行）时，程序通常会在函数test中恢复执行（在此函数的第7行）。我们希望改变这种行为。在bufbomb文件中，有一个名为smoke的函数，其C代码如下：
void smoke()
{
printf("烟雾！：你调用了smoke()\n");
validate(0);
exit(0);
}
你的任务是让BUFBOMB在getbuf执行其返回语句时执行smoke的代码，而不是返回到test。请注意，你的利用字符串也可能破坏与这一阶段不直接相关的栈的部分，但这不会造成问题，因为smoke会导致程序直接退出。
一些建议：
•  通过检查BUFBOMB的反汇编版本，可以确定为这一级别设计利用字符串所需的所有信息。使用objdump -d获取这个反汇编版本。
•  注意字节顺序。
•  你可能想使用GDB逐步执行程序的最后几条指令，以确保它按预期执行。
•  buf在getbuf的栈帧中的位置取决于用于编译bufbomb的GCC版本，因此你需要阅读一些汇编代码来确定它的真实位置。

第1级：火花（10分）
在bufbomb文件中，还有一个名为fizz的函数，其C代码如下：
void fizz(int val)
{
if (val == cookie) {
printf("Fizz!: You called fizz(0x%x)\n", val);
validate(1);
} else
printf("Misfire: You called fizz(0x%x)\n", val);
exit(0);
}
与第0级类似，你的任务是让BUFBOMB执行fizz的代码，而不是返回到test。然而，在这种情况下，你必须让fizz看起来像是你将你的cookie作为其参数传递给了它。你该怎么做呢？
一些建议：
• 请注意，程序实际上不会调用fizz——它只会执行其代码。这对你想在栈帧中放置cookie的位置有重要的启示。


第2级：鞭炮（15分）
一种更为复杂的缓冲区攻击形式是提供一个编码了实际机器指令的字符串。利用字符串随后会用这些指令的起始地址覆盖返回指针。当调用函数（在这种情况下是getbuf）执行其ret指令时，程序将开始执行栈上的指令，而不是返回。通过这种攻击方式，你可以让程序几乎做任何事情。你放在栈上的代码被称为利用代码。不过，这种攻击方式很棘手，因为你必须将机器码放到栈上，并将返回指针设置为这段代码的起始位置。
在bufbomb文件中，有一个名为bang的函数，其C代码如下：
int global_value = 0;
void bang(int val)
{
if (global_value == cookie) {
printf("Bang!: You set global_value to 0x%x\n", global_value);
validate(2);
} else
printf("Misfire: global_value = 0x%x\n", global_value);
exit(0);
}
与第0级和第1级类似，你的任务是让BUFBOMB执行bang的代码，而不是返回到test。在此之前，你必须将全局变量global_value设置为你的userid的cookie。你的利用代码应该设置global_value，将bang的地址推入栈中，然后执行一个ret指令以跳转到bang的代码。
一些建议：
• 你可以使用GDB获取构建利用字符串所需的信息。在getbuf中设置断点并运行到该断点。确定global_value的地址和缓冲区的位置等参数。
•  手动确定指令序列的字节编码既繁琐又容易出错。你可以通过编写一个包含要放在栈上的指令和数据的汇编代码文件，让工具完成所有工作。使用gcc -m32 -c组装此文件，并使用objdump -d进行反汇编。你应该能够获得在提示符下输入的确切字节序列。（本文末尾包含了一个如何进行操作的简要示例。）
• 请记住，你的利用字符串取决于你的机器、编译器，甚至你的userid的cookie。在讲师分配的机器上完成所有工作，并确保在BUFBOMB的命令行中包含正确的userid。
• 在编写汇编代码时，注意地址模式的使用。请注意，movl $0x4, %eax将值0x00000004移入寄存器%eax；而movl 0x4, %eax将内存位置0x00000004中的值移入%eax。由于该内存位置通常未定义，第二个指令将导致段错误！
• 不要尝试使用jmp或call指令跳转到bang的代码。这些指令使用PC相对寻址，很难正确设置。相反，将一个地址推入栈中并使用ret指令。


第3级：炸药（20分）
我们之前的攻击都导致程序跳转到其他函数的代码，然后导致程序退出。因此，使用破坏栈的利用字符串是可以接受的。
缓冲区溢出攻击的最复杂形式是使程序执行一些利用代码，改变程序的寄存器/内存状态，但使程序返回到原始调用函数（本例中为test）。调用函数对攻击毫无察觉。然而，这种攻击方式很棘手，因为你必须：1）将机器码放到栈上，2）将返回指针设置为这段代码的起始位置，3）撤销对栈状态的任何破坏。
你在这个级别的任务是提供一个利用字符串，使getbuf返回你的cookie给test，而不是值1。你可以从test的代码中看到，这将导致程序进入“Boom!”状态。你的利用代码应该将cookie设置为返回值，恢复任何被破坏的状态，将正确的返回位置推入栈中，并执行一个ret指令以真正返回到test。
一些建议：
• 你可以使用GDB获取构建利用字符串所需的信息。在getbuf中设置断点并运行到该断点。确定诸如保存的返回地址等参数。
•  手动确定指令序列的字节编码既繁琐又容易出错。你可以通过编写一个包含要放在栈上的指令和数据的汇编代码文件，让工具完成所有工作。使用GCC汇编此文件，并使用OBJDUMP进行反汇编。你应该能够获得在提示符下输入的确切字节序列。
• 请记住，你的利用字符串取决于你的机器、编译器，甚至你的userid的cookie。在讲师分配的机器上完成所有工作，并确保在BUFBOMB的命令行中包含正确的userid。


第4级：硝化甘油（10分）
请注意：你需要使用命令行标志“-n”来运行这个阶段。
从一次运行到另一次运行，尤其是不同用户之间，给定过程使用的精确栈位置会有所不同。这种差异的一个原因是，当程序开始执行时，所有环境变量的值都放置在栈的底部附近。环境变量作为字符串存储，根据其值需要不同的存储量。因此，为给定用户分配的栈空间取决于其环境变量的设置。在GDB下运行程序时，栈位置也会有所不同，因为GDB使用栈空间来存储其自身的一些状态。
在调用getbuf的代码中，我们加入了稳定栈的功能，使得getbuf的栈帧位置在不同运行之间保持一致。这使得你可以编写一个利用字符串，知道buf的确切起始地址。如果你尝试在普通程序上使用这样的利用字符串，你会发现它有时有效，但有时会导致段错误。因此得名“炸药”——一种由阿尔弗雷德·诺贝尔开发的爆炸物，含有稳定元素，使其不易意外爆炸。
在这个级别，我们反其道而行之，使得栈位置比正常情况下更不稳定。因此得名“硝化甘油”——一种以极不稳定著称的爆炸物。
当你使用命令行标志“-n”运行BUFBOMB时，它将以“Nitro”模式运行。程序不是调用函数getbuf，而是调用一个略有不同的函数getbufn：
/* getbufn的缓冲区大小 */
#define KABOOM_BUFFER_SIZE 512
这个函数与getbuf类似，只是它有一个512个字符的缓冲区。你需要这个额外的空间来创建一个可靠的利用。调用getbufn的代码首先在栈上分配一个随机数量的存储空间，因此如果你在两次连续执行getbufn期间采样%ebp的值，你会发现它们相差多达±240。
此外，当以Nitro模式运行时，BUFBOMB要求你提供你的字符串5次，它将执行getbufn 5次，每次都有不同的栈偏移。你的利用字符串必须在这5次中每次都返回你的cookie。
你的任务与炸药级别中的任务相同。再次，你在这个级别的任务是提供一个利用字符串，使getbufn返回你的cookie给test，而不是值1。你可以从test的代码中看到，这将导致程序进入“KABOOM!”状态。你的利用代码应该将cookie设置为返回值，恢复任何被破坏的状态，将正确的返回位置推入栈中，并执行一个ret指令以真正返回到testn。
一些建议：
• 你可以使用HEX2RAW程序发送多个你的利用字符串副本。如果你在文件exploit.txt中有一个副本，你可以使用以下命令：
unix> cat exploit.txt | ./hex2raw -n | ./bufbomb -n -u bovik
你必须在getbufn的所有5次执行中使用相同的字符串。否则，它将无法通过我们的评分服务器使用的测试代码。
•  窍门是利用nop指令。它用一个字节编码（代码0x90）。阅读CS:APP2e教材第262页关于“nop sleds”的内容可能会有用。


后勤说明
提交作业到评分服务器会在你正确解决一个关卡并使用 -s 选项时发生。收到你的解决方案后，服务器将验证你的字符串并更新缓冲区实验室记分牌网页，你可以通过将网页浏览器指向以下地址来查看：
http://$Buflab::SERVER_NAME:18213/scoreboard
提交后请务必检查该页面，以确保你的字符串已通过验证。（如果你确实解决了关卡，你的字符串应该是有效的。）
请注意，每个关卡都是单独评分的。你不需要按照指定的顺序完成它们，但服务器只会为收到有效消息的关卡给予学分。你可以查看缓冲区实验室记分牌，了解你已经完成了哪些关卡。
评分服务器通过使用它为每个阶段拥有的最新结果来创建记分牌。
祝你好运，玩得开心！
生成字节码
使用 GCC 作为汇编器和 OBJDUMP 作为反汇编器，可以方便地生成指令序列的字节码。例如，假设我们编写了一个名为 example.S 的文件，包含以下汇编代码：
手工生成的汇编代码示例
push $0xabcdef # 将值推入栈
add $17,%eax # 将 17 加到 %eax
.align 4 # 下面的内容将对齐到 4 的倍数
.long 0xfedcba98 # 一个 4 字节常量
代码可以包含指令和数据的混合。‘#’字符右侧的内容是注释。
我们现在可以对这个文件进行汇编和反汇编：
unix> gcc -m32 -c example.S
unix> objdump -d example.o > example.d
生成的文件 example.d 包含以下内容：
0: 68 ef cd ab 00 push $0xabcdef
5: 83 c0 11 add $0x11,%eax
8: 98 cwtl
9: ba .byte 0xba
a: dc fe fdivr %st,%st(6)
每一行显示一个单独的指令。左侧的数字表示起始地址（从 0 开始），而‘:’字符后的十六进制数字表示指令的字节码。因此，我们可以看到指令 push $0xABCDEF 的十六进制格式字节码为 68 ef cd ab 00。
从地址 8 开始，反汇编器变得困惑。它试图将 example.o 文件中的字节解释为指令，但这些字节实际上对应于数据。不过，如果我们从地址 8 开始读取 4 个字节，我们得到：98 ba dc fe。这是数据字 0xFEDCBA98 的字节反转版本。
这种字节反转表示了在小端字节序机器上以字符串形式提供字节的正确方式。
最后，我们可以将我们的代码的字节序列读取为：
68 ef cd ab 00 83 c0 11 98 ba dc fe
这个字符串可以通过 HEX2RAW 生成一个合适的输入字符串，供我们提供给 BUFBOMB。
或者，我们可以将 example.d 编辑为以下形式：
68 ef cd ab 00 /* push $0xabcdef /
83 c0 11 / add $0x11,%eax */
98
ba dc fe
这也是一个有效的输入，我们可以在发送给 BUFBOMB 之前通过 HEX2RAW 处理。