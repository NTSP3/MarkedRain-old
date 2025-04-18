#include "shell.h"
#include <stdio.h>

int help(__attribute__((unused)) int argc, __attribute__((unused)) char **argv)
{
    // TODO : When processes : List also apps in fs
    
    puts("\n OS Codename \"MarkedRain\"");
    puts(" For more help on a specific command, type <cmd> --help (only for some cmds)");
    puts("  -cat      :Concatenate files");
    puts("  -cd       :Change directory (only for shell)");
    puts("  -clear    :Clears tdhe screen");
    puts("  -color    :Sets the color of the background / foreground of the console");
    puts("  -echo     :Displays arguments on stdout");
    puts("  -exit     :Terminate shell (only for shell)");
    puts("  -help     :Display all commands");
    puts("  -ls       :List directory");
    puts("  -mkdir    :Create an empty directory");
    puts("  -ver      :Shows current OS Version");
    puts("  -whatsnew :Shows the new stuff introduced in the current and prior versions\n");
    puts(" Also there's one new hidden command I will not list here.\n");

    return 0;
}
