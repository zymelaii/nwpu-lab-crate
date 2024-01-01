# lab 4 - linklab

> Authored by **XingLuoXL**

这次实验虽然不简单，但是还是有很多可以取巧使用技巧的地方的。

## Phase1

反汇编一下链接生成的可执行程序，发现 do_phase 函数做了这些事：

```plain
080491a2 <do_phase>:
  80491a2:   55              push  %ebp
  80491a3:   89 e5           mov %esp,%ebp
  80491a5:   83 ec 08        sub $0x8,%esp
  80491a8:   b8 da c0 04 08  mov $0x804c0da,%eax
  80491ad:   83 ec 0c        sub $0xc,%esp
  80491b0:   50              push %eax
  80491b1:   e8 7a fe ff ff  call 8049030
```

我们大概知道 phase1 的代码干了这样一件事：调用 puts 函数，打印这个奇怪的字符串。所以直接在 vim 里面把输出的字符串改成学号就好了。

总结：几个里面最简单的教学关，按照 PPT 做就好了。

## Phase2

简单地反汇编一下 phase2.o，发现 do_phase 里面有一堆 nop。那么显然，这道题需要我们填充 do_phase 函数中的指令，从而打印出学号。

方法很多，但是最简单的莫过于直接在里面写一个输出学号的函数并直接返回了。

```assembly
push $0x3330
push $0x31303033
push $0x32323032
mov  %esp,%edx
push %edx
lea  -0x38(%eax),%eax
call *%eax
mov  %ebp,%esp
```

把这段攻击代码转换为汇编代码，学号改成自己的学号，填到 nop 就大功告成了。

## Phase3

为了取巧，先直接运行链接后的程序试试，发现什么都输出不了。

首先看看 do_phase 反汇编代码。

```plain
080491b2 <do_phase>:
  80491b2:   55                     push %ebp
  80491b3:   89 e5                  mov %esp,%ebp
  80491b5:   83 ec 18               sub $0x18,%esp
  80491b8:   c7 45 ea 79 7a 67 69   movl $0x69677a79,-0x16(%ebp)
  80491bf:   c7 45 ee 75 68 6e 62   movl $0x626e6875,-0x12(%ebp)
  80491c6:   66 c7 45 f2 65 00      movw $0x65,-0xe(%ebp)
  80491cc:   c7 45 f4 00 00 00 00   movl $0x0,-0xc(%ebp)
  80491d3:   eb 28                  jmp 80491fd <do_phase+0x4b>
  80491d5:   8d 55 ea               lea -0x16(%ebp),%edx
  80491d8:   8b 45 f4               mov -0xc(%ebp),%eax
  80491db:   01 d0                  add %edx,%eax
  80491dd:   0f b6 00               movzbl (%eax),%eax
  80491e0:   0f b6 c0               movzbl %al,%eax
  80491e3:   0f b6 80 60 c0 04 08   movzbl 0x804c060(%eax),%eax
  80491ea:   0f be c0               movsbl %al,%eax
  80491ed:   83 ec 0c               sub $0xc,%esp
  80491f0:   50                     push %eax
  80491f1:   e8 5a fe ff ff         call 8049050 <putchar@plt>
  80491f6:   83 c4 10               add $0x10,%esp
  80491f9:   83 45 f4 01            addl $0x1,-0xc(%ebp)
  80491fd:   8b 45 f4               mov -0xc(%ebp),%eax
  8049200:   83 f8 08               cmp $0x8,%eax
  8049203:   76 d0                  jbe 80491d5 <do_phase+0x23>
  8049205:   83 ec 0c               sub $0xc,%esp
  8049208:   6a 0a                  push $0xa
  804920a:   e8 41 fe ff ff         call 8049050 <putchar@plt>
  804920f:   83 c4 10               add $0x10,%esp
  8049212:   90                     nop
  8049213:   c9                     leave
  8049214:   c3                     ret
```

看来，这个函数先是把 9 个字节的信息放入了栈里，然后进入一个循环次数为 9 的循环。对于刚刚那 9 个字节的信息，每次循环会从中取出 1 个字节的信息，第 x 次循环就是取第 x 个字节的信息。然后将这个第 x 个字节的信息作为一个数组的索引，得到一个 char 字符，并将其打印出来。

这时候在 gdb 查看一下这个数组：

```plain
(gdb) x /72wx 0x804c060

0x804c060 <NQqPQyqUth>:     0x00000000  0x00000000  0x00000000  0x00000000
0x804c070 <NQqPQyqUth+16>:  0x00000000  0x00000000  0x00000000  0x00000000
0x804c080 <NQqPQyqUth+32>:  0x00000000  0x00000000  0x00000000  0x00000000
0x804c090 <NQqPQyqUth+48>:  0x00000000  0x00000000  0x00000000  0x00000000
0x804c0a0 <NQqPQyqUth+64>:  0x00000000  0x00000000  0x00000000  0x00000000
0x804c0b0 <NQqPQyqUth+80>:  0x00000000  0x00000000  0x00000000  0x00000000
0x804c0c0 <NQqPQyqUth+96>:  0x00000000  0x00000000  0x00000000  0x00000000
0x804c0d0 <NQqPQyqUth+112>: 0x00000000  0x00000000  0x00000000  0x00000000
0x804c0e0 <NQqPQyqUth+128>: 0x00000000  0x00000000  0x00000000  0x00000000
0x804c0f0 <NQqPQyqUth+144>: 0x00000000  0x00000000  0x00000000  0x00000000
0x804c100 <NQqPQyqUth+160>: 0x00000000  0x00000000  0x00000000  0x00000000
0x804c110 <NQqPQyqUth+176>: 0x00000000  0x00000000  0x00000000  0x00000000
0x804c120 <NQqPQyqUth+192>: 0x00000000  0x00000000  0x00000000  0x00000000
0x804c130 <NQqPQyqUth+208>: 0x00000000  0x00000000  0x00000000  0x00000000
0x804c140 <NQqPQyqUth+224>: 0x00000000  0x00000000  0x00000000  0x00000000
0x804c150 <NQqPQyqUth+240>: 0x00000000  0x00000000  0x00000000  0x00000000
0x804c160: 0x00000000  0x00000000  0x00000000  0x00000000
0x804c170: 0x00000000  0x00000000  0x00000000  0x00000000
```

好了，一切明了了。这个程序会在一个 256 位的数组里面找指定位置内容输出，但是现在里面元素全是 0，所以什么都输出不了。

到这里我们大概知道这个阶段要干些啥了：创建 phase3_patch.o，定义一个赋有初始值的强符号 NQqPQyqUth，使得 phase3.o 中的弱符号 NQqPQyqUth 能够引用 phase3_patch.o 中的数组。这样，我们就能在 phase3_patch.o 中给 NQqPQyqUth 赋上合适的值，使其能在执行 do_phase 函数时根据索引值打印字符数组 NQqPQyqUth 中的某些元素，所有打印出来的内容连起来就是我们想要的学号。索引位置很好找，直接在 readelf 里面计算出偏移量就可以了。

但是都已经可以修改数组内容了，何必再计算偏移量？直接把数组写成 0~255 字符，看它输出什么，就把对应位置改成学号就好了。

## Phase4

Phase3 和 Phase4 本质差别不大，甚至 4 比 3 还简单。反汇编之后可以看出 4 就是在不断循环，输出提前规定的位置的字节。当然可以通过查跳转表找到输出的字节位置并修改。但是既然都给出了原本输出内容，直接在 vim 里面把对应的字符修改成自己学号不是更快？

## Phase5

最佳答案：vim 里面每个人都是一样的空白处，直接照着填。
