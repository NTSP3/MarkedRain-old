#include "shell.h"

#include "drivers/screen.h"
#include "drivers/console.h"
#include "drivers/keyboard.h"
#include "builtins.h"
#include "topsecretstuff.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <k/syscalls.h>
#include <k/io.h>
#include <k/finfo.h>

#define CMD_MAX_SIZE (SCREEN_WIDTH * 3)
#define CMD_MAX_ARGS 32

#define BUILTIN_NOT_FOUND 0x7F100001

//Operating System version
char osver[25] = "0.02_2/24/2024@22:36";
//unsigned int osvnum = 0.01;

static unsigned int userInputBegin;
static bool shellRunning;
static int shellExitCode;
bool nextlevel = false;

// !!! Root is represented by an empty string, not /
static char *shellCwd;

// Moves the begining of user inputdirPath
static void resetUserInput()
{
   userInputBegin = getCaret();
}

static unsigned int shellExit(__attribute__((unused)) int argc, __attribute__((unused)) char **argv)
{
    shellRunning = false;

    return 0;
}

static int shellCd(int argc, char **argv)
{
    if (argc > 2)
    {
        fprintf(stderr, " Too many arguments!\n");
        return -1;
    }
    /*else if (argc == 1 || strcmp(argv[2], "/") == 0)
    {
        free(shellCwd);
        shellCwd = strdup("");
    }*/

    else // argc == 2
    {
        char *newCwd;

        if (strcmp(argv[1], "..") == 0)
        {
            // Root case
            if (shellCwd[0] == '\0')
                return 0;

            // Retrieve the last name
            newCwd = dirPath(shellCwd);
            free(shellCwd);
            shellCwd = newCwd;

            return 0;
        }

        // Absolute path (but has errors if enabled)
        if (argv[1][0] == '/') {
            //newCwd = strdup(argv[1]);
            fprintf(stderr, "\n The parameter '/' has an error when changing dirs using cd and is disabled.\n\n");
            return -1;
        }
        else
        {
            size_t len = strlen(argv[1]);
            size_t cwdLen = strlen(shellCwd);

            // cwd/dir\0
            newCwd = malloc(cwdLen + 1 + len + 1);

            memcpy(newCwd, shellCwd, cwdLen);
            newCwd[cwdLen] = '/';
            memcpy(newCwd + cwdLen + 1, argv[1], len + 1);
        }

        FInfo *info = finfo(newCwd);
        if (info == NULL)
        {

            //fprintf(stderr, " Directory '%s' is not found.\n", newCwd);
            fprintf(stderr, "\n Cannot access \"%s\": No such directory\n\n", newCwd);
            return -1;
        }

        if (info->directory)
        {
            free(shellCwd);
            shellCwd = newCwd;
            free(info);
        }
        else
        {
            //fprintf(stderr, " '%s' is not a directory.\n", newCwd);
            fprintf(stderr, "\n Cannot access \"%s\": No such directory\n\n", newCwd);
            //Right now, it shows like this: Cannot access "/dir/dir2". Make it only show "dir2" like in ls e.
            free(info);
            return -1;
        }
    }

    return 0;
}

int secretShell(char *input) {
    if (nextlevel == false)
        return BUILTIN_NOT_FOUND;
    else {
        if (strcmp(input, "crash!") == 0)
            return whyAreWeStillHere("wow you found it. This was used for testing the crash screen.");
        else if (strcmp(input, "shelp") == 0)
            return secretHelp();
        else
            return BUILTIN_NOT_FOUND;
    }
}

unsigned int versions() {
/*    if (argc > 1) {
        if (strcmp(argv, "--help") == 0 || strcmp(argv, "/?") == 0) {
            puts("\n The version command prints the os version.");
            puts("\n Syntax: ver <type>");
            puts(" Example: ver --num\n");
            puts(" Parameters: --num");
            puts(" Aliases: ver\n");
            puts(" Aliases for help: --help, /?");
            puts("\n Parameter details:\n");
            puts("  --num: Only prints the version number.\n");
        }

        else if (strcmp(argv, "--num") == 0)
            printf("%d", osvnum);
        else
            fprintf(stderr, "\n Invalid argument. Use --help for more info.\n\n");
    }
    else*/
        printf("\n Operating System codename \"Marked Rain\" version %s\n Based on Os 2020 v0.02\n Expect bugs, this ain't big!\n\n", osver);
    return 0;
}

// Tries to executes a builtin command
// Returns the exit code of the command
// or BUILTIN_NOT_FOUND if no builtin found
static int tryExecBuiltin(Context *ctxt, const char oldapp[], int argc, char **argv)
{
    //Make the command case-insensitive
    unsigned int i = 0;
    char app[strlen(oldapp)];
    while (i <= strlen(oldapp)) {
        app[i] = tolower(oldapp[i]);
        i++;
    }

    // Static builtins
    if (strcmp(app, "exit") == 0)
        return shellExit(argc, argv);

    if (strcmp(app, "cd") == 0)
        return shellCd(argc, argv);

    if (strcmp(app, "cd.") == 0) {
        char *args[] = {"cd", "."};
        return shellCd(2, args);
    }

    if (strcmp(app, "cd..") == 0) {
        char *args[] = {"cd", ".."};
        return shellCd(2, args);
    }

    if (strcmp(app, "secrets") == 0) {
        nextlevel = !(nextlevel);
        return 0;
    }

    if (strcmp(app, "ver") == 0)
        return versions();

    // Builtins
    if (strcmp(app, "cat") == 0)
        return enter(ctxt, cat, argc, argv);

    if (strcmp(app, "color") == 0)
        return enter(ctxt, colorMain, argc, argv);

    if (strcmp(app, "clear") == 0 || strcmp(app, "cls") == 0)
        return enter(ctxt, clear, argc, argv);

    if (strcmp(app, "echo") == 0)
        return enter(ctxt, echo, argc, argv);

    if (strcmp(app, "ls") == 0 || strcmp(app, "dir") == 0)
        return enter(ctxt, lsMain, argc, argv);

    if (strcmp(app, "mkdir") == 0 || strcmp(app, "md") == 0)
        return enter(ctxt, mkdir, argc, argv);

    if (strcmp(app, "help") == 0)
        return enter(ctxt, help, argc, argv);

    if (strcmp(app, "whatsnew") == 0)
        return enter(ctxt, whatsNew, argc, argv);

    // secret Program pass

    return secretShell(app);
}

int shellMain(int argc, char **argv)
{
    shellExitCode = 0;

    if (argc == 2)
    {
        // Display help
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
        {
            puts("Usage : shell [<dir>='']");
            return 0;
        }

        shellCwd = absPath(argv[1]);

        // If last char is /, remove it
        size_t len = strlen(shellCwd);
        if (shellCwd[len - 1] == '/')
            shellCwd[len - 1] = '\0';

        // Verify validity
        FInfo *info = finfo(shellCwd);

        if (info == NULL)
        {
            shellExitCode = -1;
            goto shellExit;
        }

        if (!info->directory)
        {
            shellExitCode = -1;
            free(info);
            goto shellExit;
        }

        free(info);
    }
    else
        // Root
        shellCwd = strdup("");

    shellRunning = true;

    // Init display
    // TODO : Use syscalls
    fillScreen('\0', (FMT_BLACK << 4) | FMT_GRAY);
    setCaret(0, 0);

    // Init message
    puts("\nThe MarkedRain shell - started: 21/2/2024 17:12");
    puts("Made by @Superbyte: https://wintelic.weebly.com");
    puts("Based on Os 2020 by Cc618");
    puts("\nRun 'whatsnew' for a list of updated changes.");
    puts("Run 'help' to view help.\n");

    char cmd[CMD_MAX_SIZE];
    while (shellRunning)
    {
        // PS1
        shellPS1();

        // Get input
        gets(cmd);

        // Almost fix string edit after the user pressed enter pt 1
        keyboardTerminate();

        // Evaluate command
        shellEval(cmd);

        // Almost fix string edit after the user pressed enter pt 2
        keyboardInit();
    }

shellExit:;
    free(shellCwd);

    return shellExitCode;
}

void shellPS1()
{
    // TODO : Push / pop format to have console format

    printf("%s>", shellCwd[0] == '\0' ? "/" : shellCwd);

    resetUserInput();
}

bool shellDelete()
{
    // Only if possible
    bool possible = getCaret() > userInputBegin;

    if (possible)
        consoleDel();

    return possible;
}

void shellEval(const char *CMD)
{
    char *cmd = strdup(CMD);

    char *argv[CMD_MAX_ARGS];
    argv[0] = strdup(shellCwd);

    const char *delim = " \n";

    // App name
    char *token = strtok(cmd, delim);
    char *appName = token;

    // Retrieve arguments
    int argc = 1;
    for ( ; (token = strtok(NULL, delim)); ++argc)
        argv[argc] = token;

    // Detect redirections
    bool stdoutRedirected = false;
    if (argc > 2 && strcmp(argv[argc - 2], ">") == 0)
        stdoutRedirected = true;
    
    bool stderrRedirected = false;
    if (argc > 2 && strcmp(argv[argc - 2], "2>") == 0)
        stderrRedirected = true;
    
    bool stdinRedirected = false;
    if (argc > 2 && strcmp(argv[argc - 2], "<") == 0)
        stdinRedirected = true;

    Context *ctxt = Context_new(shellCwd);

    // Open file for redirections
    if (stdoutRedirected)
    {
        char *path = absPathFrom(shellCwd, argv[argc - 1]);
        ctxt->stdout = open(path, F_WRITE);
        if (ctxt->stdout == INVALID_FD)
        {
            fprintf(stderr, "File '%s' can't be opened\n", argv[argc - 1]);
            goto end;
        }
    }

    if (stderrRedirected)
    {
        char *path = absPathFrom(shellCwd, argv[argc - 1]);
        ctxt->stderr = open(path, F_WRITE);
        if (ctxt->stderr == INVALID_FD)
        {
            fprintf(stderr, "File '%s' can't be opened\n", argv[argc - 1]);
            goto end;
        }
    }

    if (stdinRedirected)
    {
        char *path = absPathFrom(shellCwd, argv[argc - 1]);
        ctxt->stdin = open(path, F_READ);
        if (ctxt->stdin == INVALID_FD)
        {
            fprintf(stderr, "File '%s' can't be opened\n", argv[argc - 1]);
            goto end;
        }
    }

    // Execute command
    int ret = tryExecBuiltin(ctxt, appName, stdoutRedirected || stderrRedirected || stdinRedirected ? argc - 2 : argc, argv);

    if (ret == BUILTIN_NOT_FOUND)
        fprintf(stderr, "\n The command or executable file '%s' is not found within the library.\n\n", appName);

    // TODO : When apps : Exec
    // {
    //     ret = exec(appName, argc, argv);

    //     if (ret != 0)
    //         printf("App exits with code %d\n", ret);
    // }

    // Close redirections
    if (stdoutRedirected)
        close(ctxt->stdout);
    else if (stderrRedirected)
        close(ctxt->stderr);
    else if (stdinRedirected)
        close(ctxt->stdin);

end:;
    Context_del(ctxt);

    free(cmd);
    for (size_t i = 0; i < (size_t)argc; ++i)
        free(argv[i]);
}
