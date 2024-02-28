#include "drivers/screen.h"
#include "color.h"
#include <stdio.h>
#include <string.h>
#include <stdio.h>

static const uint8_t colFmt[] = {
    FMT_BLACK,           
    FMT_BLUE,            
    FMT_GREEN,           
    FMT_CYAN,            
    FMT_RED,             
    FMT_PURPLE,          
    FMT_BROWN,           
    FMT_GRAY,            
    FMT_DARK_GRAY,       
    FMT_LIGHT_BLUE,      
    FMT_LIGHT_GREEN,     
    FMT_LIGHT_CYAN,      
    FMT_LIGHT_RED,       
    FMT_LIGHT_PURPLE,    
    FMT_YELLOW,          
    FMT_WHITE,           
};

int clear(int argc, char **argv) {
    if (argc >= 2) {

        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "/?") == 0) {
            puts("\n The clear command clears the screen.");
            puts("\n Running without any option clears the screen with the current screen color.");
            puts("\n Syntax: clear <type>");
            puts(" Example: whatsnew --nocolor\n");
            puts(" Parameters: --nocolor, --color");
            puts(" Aliases: clear, cls\n");
            puts(" Aliases for help: --help, /?");
            puts("\n Parameter details:\n");
            puts("  --nocolor: Specifies to clear the screen as gray on black.\n");
        }
        
        else if (strcmp(argv[1], "--nocolor") == 0) {
            fillScreen('\0', (FMT_BLACK << 4) | FMT_GRAY);
            setCaret(0, 0);  // Set cursor to the beginning of the screen
        }

        else if (strcmp(argv[1], "--color") == 0) {
            goto colorjmp;
        }

        else
            fprintf(stderr, "\n Invalid argument. Use --help for more info.\n\n");

    }

    else {    
        colorjmp:
            int tmp = getColor("front");
            uint8_t front = colFmt[tmp];
            tmp = getColor("back");
            uint8_t back = colFmt[tmp];
            tmp = 0;
            fillScreen('\0', (back << 4) | front);
            setCaret(0, 0);  // Set cursor to the beginning of the screen
    }

    return 0;
}
