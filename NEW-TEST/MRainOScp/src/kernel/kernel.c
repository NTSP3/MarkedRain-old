#include "int/interrupts.h"
#include "drivers/console.h"
#include "drivers/keyboard.h"
#include "drivers/fat32.h"
#include "io/file.h"
#include "fs/fs.h"
#include "apps/shell.h"
#include "syscalls/syscalls.h"
#include "_libc.h"
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
 
/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This file will only compile for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif
 
// Inits all modules
static void initKernel()
{
    initInterrupts();

    // File system init
    fatInit();
    fsInit();
    filesInit();

    // Push system context
    appContexts = Vector_new();
    Vector_add(appContexts, Context_new(""));

    // Inputs init
    keyboardInit();

    __libc_init();
}

// After init, the user can access the kernel
static void userAct()
{
    // TODO : Warnings
    // TODO : v0.2 !

    // Launch the shell from root
    char *shellArgv[2];
    shellArgv[0] = "";
    shellArgv[1] = "";

    sys_enter(Context_new(""), shellMain, 2, shellArgv);

    consoleNewLine();
    //puts("No process running");

    puts("Shell closed or value changed. Restart your computer");
    puts("Terminating Kernel.");
}

// Terminates all modules
// !!! This function doesn't return
void terminateKernel()
{
    // Terminate system context
    Vector_del(appContexts);

    keyboardTerminate();
    filesTerminate();
    fatTerminate();
    fsTerminate();

    while (1);
}

// Entry from stage2
void main()
{
    initKernel();

    userAct();

    terminateKernel();
}
