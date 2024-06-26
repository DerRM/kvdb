#pragma once

#include "target.h"
#include "eventflag.h"

#include <psp2kern/kernel/sysmem.h>
#include <psp2kern/kernel/sysroot.h>

#include <cstdint>
#include <unordered_set>

class Command;

typedef int(*putchar_handler_t)(void*,char c);

struct SWBreak {
    uint32_t addr = 0;
    uint32_t inst = 0;
    uint32_t size = 0;
};

class Debugger
{
public:
    enum class Signal : unsigned int
    {
        BKPT = (1 << 0),
        PROCESS_CREATED = (1 << 1)
    };

public:
    static Debugger *instance();

    void signal(Signal signal);
    int waitForSignal(Signal signal, unsigned int *pattern = nullptr);
    int pollSignal(Signal signal, unsigned int *pattern = nullptr);

    int start();

    void attach(int pid);
    bool attached() const;

    bool halt(int reason = 0x11);
    bool isHalted() const;

    bool resume();
    bool isRunning() const;

    Target *target();
    
    uint32_t m_instruction = 0;
    uint32_t m_pc_prev_addr = 0;
    uint32_t m_pc_addr = 0;

    SWBreak m_sw_breakpoints[16] = {};
    uint32_t m_sw_breakpoint_count = 0;

private:
    Debugger();

    int on_process_created();
    static int on_process_created_handler(int a1, int a2, int a3);

    int on_putchar(char ch);
    static int putchar_handler(void *args, char ch);

    enum class State
    {
        NotAttached,
        Halted,
        Running
    };

private:
    Command * const *m_cmd_list = nullptr;
    std::size_t m_cmd_list_n = 0;
    EventFlag m_state_flag;
    SceSysrootProcessHandler m_proc_handler;
    Target m_target;
    bool m_attached = false;
    char m_stdout_cache[32];
    int m_stdout_cache_indx = 0;
    State m_state = State::NotAttached;
    putchar_handler_t m_previous_putchar_handler = nullptr;
};

namespace debugger
{
    Debugger *get();
}
