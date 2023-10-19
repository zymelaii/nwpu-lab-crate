# NWPU 操作系统实验

记录时使用的讲义为 `计算机操作系统实验讲义-2022年修订.pdf`，由于是内部资料故对讲义内容不作上传，仅在该部分作实验内容的简要概括。

由于 os 实验的各项内容对第三方的依赖程度较高不易分离，故不再对每个实验进行文件夹级的区分，转而使用 git tag 对各项实验内容进行分层。

# 实验环境

OS 目标架构：i386

讲义推荐环境：

- ubuntu 10.04.2-desktop-i386
- i486-linux-gnu (GCC) 4.4.3
- nasm 2.08.02
- bochs 2.6.8

个人环境：

- wsl2-archlinux 5.10.102.1-microsoft-standard-WSL2
- i386-elf-gcc (GCC) 12.2.0
- nasm 2.16.01
- qemu-system-i386 8.1.1
- nushell 0.85.0

# 实验内容

## 第〇次实验

实验目的：不详

实验内容：搭建环境+学习使用环境+读讲义

### 内容 1

> 使用 x86 汇编编写 boot 程序，实现输出 `Hello, OS world!`，并在虚拟机中引导启动。

```asm
    org    0x7c00
    mov    ax, cs
    mov    ds, ax
    mov    es, ax
    call   DispStr
    jmp    $
DispStr:
    mov    ax, BootMessage
    mov    bp, ax
    mov    cx, 16
    mov    ax, 01301h
    mov    bx, 000ch
    mov    dl, 0
    int    10h
    ret
BootMessage:
    db     "Hello, OS world!"
    times  510-($-$$)  db  0
    dw     0xaa55
```

```sh
nasm -o boot.bin boot.asm
```

```sh
dd if=boot.bin of=a.img bs=512 count=1 conv=notrunc
```

```sh
qemu-system-i386 -drive format=raw,file=a.img -display curses
```

或者可以使用 lab0 中提供的 Makefile 直接执行 `make run`。

### 内容 2

> 启动 geekos project0

可以使用 geekos scripts 目录下提供的 startProject 拷贝项目目录。其用法如下：

```sh
startProject <project name> <master directory> [<previous project>]
```

在此处，将每个项目拷贝到 geekos-ws/ 下进行实验。

根据对应情况修改项目文件 build 下的 Makefile 文件：

1. 交叉编译

取消 `TARGET_CC_PREFIX := i386-elf-` 的注释

2. 使用较新版本的 GCC

删除 `CC_GENERAL_OPTS` 变量的 `-Werror` 并添加 `-O0 -fno-stack-protector`

3. 使用 qemu 代替 bochs

将 fd.img 目标的构建命令修改如下：

```makefile
fd.img : geekos/fd_boot.bin geekos/setup.bin geekos/kernel.bin
	cat geekos/fd_boot.bin geekos/setup.bin geekos/kernel.bin > $@
    truncate -s 1474560 $@
```

完成以上步骤后，修改 lowlevel.asm 下的 Int_With_Err 宏，替换如下：

```asm
%macro Int_With_Err 1
align 8
	push	dword %1	 ; push interrupt number
	jmp	Handle_Interrupt ; jump to common handler
    nop
	nop				 	 ; NOTE: align Int_With_Err to 16 bytes
%endmacro
```

该修改针对是 geekos 自身的错误断言，x86 是变长指令编码，jmp 短跳转 2 字节，push dword 5 字节，nop 1 字节，故相邻的 Int_No_Err 8 字节对齐后占 16 字节，此处在 Int_With_Err 插入 nop 将其对齐到 16 字节以过 idt.c 下的断言。

make 构建并使用 qemu 启动，注意为 qemu 指定拓展内存过 mem.c 下的断言：

```sh
qemu-system-i386 -boot a -fda fd.img -m 10M -display curses -m 10M
```

使用 `make run` 运行，完成实验。

## 第壹次实验

实验目的：了解中断控制

实验内容：完成 geekos project0

### 内容 1

> 启动线程处理键盘按键消息的回显

阅读 keyboard.h，了解 Keycode 的位结构，依据标志位信息利用小端序联合体将 Keycode 重写为 keycode_des_t 以便访问。

```c
typedef union keycode_des_s {
    struct {
        char code;

        union {
            struct {
                char special  : 1;
                char keypad   : 1;
                char reserved : 2;
                char shift    : 1;
                char alt      : 1;
                char ctrl     : 1;
                char release  : 1;
            } flags;

            char raw_flags;
        };
    };

    short raw_data;
} keycode_des_t;
```

简单编写线程的 worker 回调，其中特判 `CTRL+d` 处理工作线程的结束，并使用控制字符变美化输出样式：

> 为了更改输出文本样式，保险起见应该使用 geekos 给出的 Set_Current_Attr 实现，此处图方便直接使用控制字符。

```c
void cb_keyboard_echo(ulong_t arg) {
    const int QUIT_KEYCODE = KEY_CTRL_FLAG | 'd';
    Print("\e[32mStart keyboard echo service, press `CTRL+d` to quit the "
          "server.\e[0m\n");

    while (1) {
        keycode_des_t key_des = {.raw_data = Wait_For_Key()};

        int         color_code = key_des.flags.release ? 32 : 31;
        const char* hint_text  = key_des.flags.release ? "RELEASE" : " PRESS ";
        Print("\e[1;%dm[%s]\e[0m ", color_code, hint_text);

        if (key_des.flags.ctrl) { Print("CTRL+"); }
        if (key_des.flags.alt) { Print("ALT+"); }
        if (key_des.flags.shift) { Print("SHIFT+"); }
        if (key_des.flags.special && !key_des.flags.keypad) {
            Print("%s", SPECIAL_KEY_STRTABLE[key_des.code]);
        } else if (key_des.flags.keypad) {
            Print("%s", KEYPAD_KEY_STRTABLE[key_des.code & 0x7f]);
        } else if (key_des.code > 0x20 && key_des.code < 0x7f) {
            Print("%#c", key_des.code);
        } else if (key_des.code == ' ') {
            Print("SPACE");
        } else if (key_des.code == '\t') {
            Print("TAB");
        } else if (key_des.code == '\r') {
            Print("RETURN");
        } else if (key_des.code == ASCII_BS) {
            Print("BACKSPACE");
        } else if (key_des.code == ASCII_ESC) {
            Print("ESCAPE");
        } else {
            Print("%#hhx", key_des.code);
        }
        Print("\n");

        if (key_des.raw_data == QUIT_KEYCODE) { break; }
    }

    Print("\e[32mKeyboard echo service safely quited .\e[0m\n");
}
```

其中部分 unprintable char 使用查表法替换按键名称。

最后使用 Start_Kernel_Thread 以一般优先级添加工作线程并立即非阻塞加入：

```c
Start_Kernel_Thread(cb_keyboard_echo, 0, PRIORITY_NORMAL, 1);
```

### 内容 2

> 启动多个输出线程处理键盘按键消息的回显

如下，打印数字及当前线程 pid，待打印的数字在创建线程时作为参数传入。

使用 PAUSE 空循环消耗 CPU 时间实现延时的作用，防止输出刷屏。

```c
void cb_stable_print(ulong_t arg) {
    struct Kernel_Thread* self = Get_Current();
    while (1) {
        Print("msg `%d` from thread [pid=%d]\n", arg, self->pid);
        PAUSE(5e8);
    }
}
```

### 内容 3

> 实现更有意思的线程函数

实现一个简易命令行，在 dev mode 下激活。

用一个单写多读的全局原子量 ATOMIC_FLAG 平替线程锁的功能，多线程逻辑如下：

1. ATOMIC_FLAG 无效时，dev 忙等。
2. ATOMIC_FLAG 有效时，keyboard echo 与 stable print 忙等。
3. keyboard echo 线程监听到 `CTRL+p` 后原子写 ATOMIC_FLAG 使有效。
4. ATOMIC_FLAG 有效时进入 dev 事务逻辑，处理命令键入与命令执行。

dev 线程事务逻辑如下：

1. ATOMIC_FLAG 无效时忙等。
2. ATOMIC_FLAG 有效时处理事务，阻塞其它线程。
3. 命令键入阶段，循环读定长内的可打印字符；遇到 '\b' 缓冲区退格并 flush 打印；遇到 '\r' 结束命令键入；读入字符数达上限结束命令键入并抛出警告。
4. 命令执行阶段，字符串全串匹配分发到各命令处理方法；对于 quit 命令，原子写 ATOMIC_FLAG 使无效；完成命令执行进入下一趟事务处理。

工作回调基本结构如下：

```c
void cb_dev_handler(ulong_t arg) {
    while (1) {
        if (ATOMIC_FLAG == 0) { continue; }
        char cmdbuf[32] = {}, ch = 0;
        while (1) {
            char* p = cmdbuf;
            Print("(dev)> ");
            do {
                ch = get_printable_char();
                if (ch == '\r') { break; }
                if (ch == '\b') {
                    if (p == cmdbuf) { continue; }
                    *--p = '\0';
                    Print(" (flush)\n(dev)> %s", cmdbuf);
                } else {
                    Print("%c", ch);
                    *p++ = ch;
                }
            } while (p - cmdbuf < 31);
            Print("\n");
            if (ch != '\r') { Print("(warning) cmd is too long\n"); }
            *p = '\0';
            if (strcmp(cmdbuf, "version") == 0) {
                // 打印版本信息
            } else if (strcmp(cmdbuf, "help") == 0) {
                // 打印帮助信息
            } else if (strcmp(cmdbuf, "info thread") == 0) {
                // 打印当前线程信息
            } else if (strcmp(cmdbuf, "quit") == 0) {
                break;
            } else if (strlen(cmdbuf) != 0) {
                // 抛出未知命令错误
            }
        }
        ATOMIC_FLAG = 0;
    }
}
```

其中获取单个有效字符：

```c
char get_printable_char() {
    while (1) {
        keycode_des_t key_des = {.raw_data = Wait_For_Key()};
        if ((key_des.raw_flags & 0b11110011) != 0) { continue; }
        char code = key_des.code;
        if (code > 0x20 && code < 0x7f || code == ' ' || code == '\r'
            || code == '\b') {
            return code;
        }
    }
}
```
