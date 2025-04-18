#include "shell.h"
#include <string.h>
#include <stdio.h>

void b3() {

}

void b2() {
    puts("\n OS Codename \"MarkedRain\" 0.02 changelog - updated 28-2-2024 10:28");
    puts("\n -Added aliases: ('ls', 'dir') ('mkdir', 'md')");
    puts(" -Added convenience commands 'cd.' and 'cd..' for 'cd .' and 'cd ..'");
    puts(" -Added full 16 colors to 'color' command");
    puts(" -Added support for CAPS LOCK.");
    puts(" -All commands are case-insensitive (not parameters, they're handeled by apps)");
    puts(" -Applications can now get the current color using color.h");
    puts(" -Changed '/boot/' to simply '/'");
    puts(" -Colors given to 'color' are now case-insensitive");
    puts(" -Command 'clear' can now do colored screen clearing");
    puts(" -Disabled 'cd /' because of a known 'cd' error");
    puts(" -Fixed string edits after 'ENTER' is pressed");
    puts(" -Most error messages are now red, although new ones should be handeled by apps");
    puts(" -Updated 'help' command's UI");
    puts(" -Updated the system crash screen.\n");
}

void b1() {
    puts("\n OS Codename \"MarkedRain\" 0.01 changelog - updated 24-2-2024 17:36");
    puts("\n -Added support for clearing the screen - use 'clear' or 'cls'");
    puts(" -Added 'whatsnew' command");
    puts(" -Added 'ver' command");
    puts(" -Applications can now set the screen cursor properly using screen.h");
    puts(" -Wording changes\n");
}

int whatsNew(int argc, char **argv) {
    if (argc >= 2) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "/?") == 0) {
            puts("\n The WhatsNew command lists new features added to each version of the MRainOS.");
            puts("\n Syntax: whatsnew <version>");
            puts(" Example: whatsnew 0.01\n");
            puts(" Parameters: <version>, <help>");
            puts(" Aliases for help: --help, /?\n");
        }
        //Userinput version check
        else if (strcmp(argv[1], "0.01") == 0)
            b1();
        else if (strcmp(argv[1], "0.02") == 0)
            b2();
        else if (strcmp(argv[1],"0.03") == 0)
            b3();
        else {
            fprintf(stderr, "\n Invalid argument. Use --help for more info.\n\n");
            return -1;
        }
    }
    
    else {
        b2();
    }

    return 0;
}