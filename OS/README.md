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
