#include "drivers/screen.h"
#include "drivers/console.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Current screen color record for other functions
static unsigned int bscrColor = 0;
static unsigned int fscrColor = 8;

// Return the current color numbers to a function that request it
unsigned int getColor(char *type) {
    if (strcmp(type, "back") == 0)
      return bscrColor;
    else if (strcmp(type, "front") == 0)
      return fscrColor;
    else
      return -1;
}

// All colors by name for comparing
char *colNames[] = {
    "black",
    "blue",
    "green",
    "cyan",
    "red",
    "purple",
    "brown",
    "gray",
    "dark_gray",
    "light_blue",
    "light_green",
    "light_cyan",
    "light_red",
    "light_purple",
    "yellow",
    "white",
};

// All colors by name for listing
char *colNamesList[] = {
    "",
    "-----------------------------------------+",
    " A list of colors supported by color 2.0 |",
    "-----------------------------------------+",
    "",
    "  Black",
    "  Blue",
    "  Green",
    "  Cyan",
    "  Red",
    "  Purple",
    "  Brown",
    "  Gray",
    "  Dark_Gray",
    "  Light_Blue",
    "  Light_Green",
    "  Light_Cyan",
    "  Light_Red",
    "  Light_Purple",
    "  Yellow",
    "  White",
    "",
    " Make sure to write two-worded colors with underscores!",
    " Example: color Light_purple light_cyan",
    "",
};

// Formats (default)
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

// Searches a command
// - Returns len(colNames) if not found
static size_t findColor(char name[])
{
    unsigned int i = 0;
    while (i <= strlen(name)) {
        name[i] = tolower(name[i]);
        i++;
    }
    i = 0;

    // Number of colors
    size_t n = sizeof(colNames) / sizeof(const char*);

    // Find color
    for (size_t i = 0; i < n; ++i)
        if (strcmp(name, colNames[i]) == 0)
            return i;

    // Not found
    return n;
}

// TODO : Use syscalls
int colorMain(int argc, char **argv)
{
    size_t nColors = sizeof(colNamesList) / sizeof(const char*);

    // TODO : To lower

    if (argc == 2)
    {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "/?") == 0)
        {
            puts("\n The color commands lets you change the prompt color.");
            puts("\n Syntax: color <foreground> <background>");
            puts(" Example: color blue black\n");
            puts(" Parameters: <color>, <help>, list, reset");
            puts(" Aliases for help: --help, /?");
            puts("\n Parameters details:\n");
            puts("  list : Lists all colors");
            puts("  reset : Sets the format to default");
            puts("  <color> : Sets foreground color");
            puts("  <foreground> <background> : Sets foreground and background color\n");
        }
        else if (strcmp(argv[1], "reset") == 0)
            consoleFmt = FMT_DEFAULT;
        else if (strcmp(argv[1], "list") == 0)
            for (size_t i = 0; i < nColors; ++i)
                puts(colNamesList[i]);
        else
        {
            // Foreground
            size_t colId = findColor(argv[1]);

            if (colId == nColors)
            {
                fprintf(stderr, "\n Color \"%s\" not found, use color list to show available colors\n\n", argv[1]);
                return -1;
            }
            
            fscrColor = colId;
            uint8_t color = colFmt[colId];

            // Set foreground color
            consoleFmt = (consoleFmt & 0b11110000) | FMT_TO_LIGHT(color);
        }
    }
    else if (argc == 3)
    {
        // Foreground
        size_t colId = findColor(argv[1]);

        if (colId == nColors)
        {
            fprintf(stderr, "\n Color \"%s\" not found, use color list to show available colors\n\n", argv[1]);
            return -1;
        }
        
        fscrColor = colId;
        uint8_t fg = FMT_TO_LIGHT(colFmt[colId]);

        // Background
        colId = findColor(argv[2]);

        if (colId == nColors)
        {
            fprintf(stderr, "\n Color \"%s\" not found, use color list to show available colors\n\n", argv[2]);
            return -1;
        }
        
        bscrColor = colId;
        uint8_t bg = colFmt[colId];

        // Set format
        consoleFmt = (bg << 4) | fg;
    }
    else
    {
        //fprintf(stderr, "Invalid arguments, use color --help to see the usage\n");   
        fprintf(stderr, "\n Invalid argument. Use --help for more info.\n\n");
        return -1;
    }

    return 0;
}
