/*
 * GeekOS C code entry point
 * Copyright (c) 2001,2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2004, Iulian Neamtiu <neamtiu@cs.umd.edu>
 * $Revision: 1.51 $
 *
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/bootinfo.h>
#include <geekos/string.h>
#include <geekos/screen.h>
#include <geekos/mem.h>
#include <geekos/crc32.h>
#include <geekos/tss.h>
#include <geekos/int.h>
#include <geekos/kthread.h>
#include <geekos/trap.h>
#include <geekos/timer.h>
#include <geekos/keyboard.h>

/*
 * project0 implements powered by zymelaii 2023-10
 */

#define IMPL(impl, todo) impl

static int ATOMIC_FLAG = 0;

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

const char* SPECIAL_KEY_STRTABLE[24] = {
    "UNKNOWN", "F1",        "F2",       "F3",      "F4",      "F5",
    "F6",      "F7",        "F8",       "F9",      "F10",     "F11",
    "F12",     "LCTRL",     "RCTRL",    "LSHIFT",  "RSHIFT",  "LALT",
    "RALT",    "PRINTSCRN", "CAPSLOCK", "NUMLOCK", "SCRLOCK", "SYSREQ",
};

const char* KEYPAD_KEY_STRTABLE[13] = {
    "KPHOME",
    "KPUP",
    "KPPGUP",
    "KPMINUS",
    "KPLEFT",
    "KPCENTER",
    "KPRIGHT",
    "KPPLUS",
    "KPEND",
    "KPDOWN",
    "KPPGDN",
    "KPINSERT",
    "KPDEL",
};

void cb_keyboard_echo(ulong_t arg) {
    const int QUIT_KEYCODE = KEY_CTRL_FLAG | 'd';
    const int DEV_KEYCODE  = KEY_CTRL_FLAG | 'p';
    Print("\e[32mStart keyboard echo service, press `CTRL+d` to quit the "
          "server,\n");
    Print("or press `CTRL+p` to entry the dev mode.\e[0m\n");

    while (1) {
        while (ATOMIC_FLAG == 1) { continue; }
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
        if (key_des.raw_data == DEV_KEYCODE) { ATOMIC_FLAG = 1; }
    }

    Print("\e[32mKeyboard echo service safely quited .\e[0m\n");
}

void cb_stable_print(ulong_t arg) {
    struct Kernel_Thread* self = Get_Current();
    while (1) {
        while (ATOMIC_FLAG == 1) { continue; }
        Print("msg `%d` from thread [pid=%d]\n", arg, self->pid);
        PAUSE(5e8);
    }
}

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
            //! handle cmdline
            if (strcmp(cmdbuf, "version") == 0) {
                Print("geekos-0.3.0 project0 powered by zymelaii (" __DATE__
                      ").\n");
            } else if (strcmp(cmdbuf, "help") == 0) {
                Print("* help         Print help info\n"
                      "* version      Print version info\n"
                      "* info thread  Print current thread info\n"
                      "* quit         Quit dev mode\n");
            } else if (strcmp(cmdbuf, "info thread") == 0) {
                struct Kernel_Thread* self  = Get_Current();
                int                   ticks = self->numTicks;
                Print(
                    "thread [pid=%d] {\n"
                    "  ticks: %d\n"
                    "  priority: %d\n"
                    "  stack page: %#p\n"
                    "  user context: %#p\n"
                    "  ref count: %d\n"
                    "  owner: %#p\n"
                    "}\n",
                    self->pid,
                    ticks,
                    self->priority,
                    self->stackPage,
                    self->userContext,
                    self->refCount,
                    self->owner);
            } else if (strcmp(cmdbuf, "quit") == 0) {
                break;
            } else if (strlen(cmdbuf) != 0) {
                Print("unknown cmd `%s`\n", cmdbuf);
            }
        }
        ATOMIC_FLAG = 0;
    }
}

void todo_thread_impl() {
    struct Kernel_Thread* t0 =
        Start_Kernel_Thread(cb_keyboard_echo, 0, PRIORITY_NORMAL, 1);
    struct Kernel_Thread* t1 =
        Start_Kernel_Thread(cb_stable_print, 233, PRIORITY_NORMAL, 1);
    struct Kernel_Thread* t2 =
        Start_Kernel_Thread(cb_stable_print, 666, PRIORITY_NORMAL, 1);
    struct Kernel_Thread* t_dev =
        Start_Kernel_Thread(cb_dev_handler, 0, PRIORITY_NORMAL, 1);
}

/*
 * Kernel C code entry point.
 * Initializes kernel subsystems, mounts filesystems,
 * and spawns init process.
 */
void Main(struct Boot_Info* bootInfo) {
    Init_BSS();
    Init_Screen();
    Init_Mem(bootInfo);
    Init_CRC32();
    Init_TSS();
    Init_Interrupts();
    Init_Scheduler();
    Init_Traps();
    Init_Timer();
    Init_Keyboard();

    Set_Current_Attr(ATTRIB(BLACK, GREEN | BRIGHT));
    Print("Welcome to GeekOS!\n");
    Set_Current_Attr(ATTRIB(BLACK, GRAY));

    IMPL(
        todo_thread_impl(),
        TODO("Start a kernel thread to echo pressed keys and print counts"));

    /* Now this thread is done. */
    Exit(0);
}
