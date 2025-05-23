#include "keyboard.h"

#include "drivers/ports.h"
#include "drivers/console.h"
#include "apps/shell.h"
#include "syscalls/syscalls.h"
#include <k/io.h>
#include <stdio.h>
#include <ctype.h>

#define KBD_DATA 0x60

#define KEY_MAP_SIZE 128

typedef char key_t;

// For strings
#define KEY_SPECIAL             0xE0

// Pressed //
// Special
#define KEY_PRESSED_SPACE       0x39
#define KEY_PRESSED_ENTER       0x1C
#define KEY_PRESSED_BACKSPACE   0x0E
#define KEY_PRESSED_TAB         0x0F
#define KEY_PRESSED_LSHIFT      0x2A
#define KEY_PRESSED_RSHIFT      0x36
#define KEY_PRESSED_LCTRL       0x1D
#define KEY_PRESSED_MOD         0x5B
#define KEY_PRESSED_CAPSLOCK    0x3A

// Digits
#define KEY_PRESSED_DIGIT_0 0x0B
#define KEY_PRESSED_DIGIT_1 0x02
#define KEY_PRESSED_DIGIT_2 0x03
#define KEY_PRESSED_DIGIT_3 0x04
#define KEY_PRESSED_DIGIT_4 0x05
#define KEY_PRESSED_DIGIT_5 0x06
#define KEY_PRESSED_DIGIT_6 0x07
#define KEY_PRESSED_DIGIT_7 0x08
#define KEY_PRESSED_DIGIT_8 0x09
#define KEY_PRESSED_DIGIT_9 0x0A

// Letters
#define KEY_PRESSED_LETTER_Q 0x10
#define KEY_PRESSED_LETTER_W 0x11
#define KEY_PRESSED_LETTER_E 0x12
#define KEY_PRESSED_LETTER_R 0x13
#define KEY_PRESSED_LETTER_T 0x14
#define KEY_PRESSED_LETTER_Y 0x15
#define KEY_PRESSED_LETTER_U 0x16
#define KEY_PRESSED_LETTER_I 0x17
#define KEY_PRESSED_LETTER_O 0x18
#define KEY_PRESSED_LETTER_P 0x19
#define KEY_PRESSED_LETTER_A 0x1E
#define KEY_PRESSED_LETTER_S 0x1F
#define KEY_PRESSED_LETTER_D 0x20
#define KEY_PRESSED_LETTER_F 0x21
#define KEY_PRESSED_LETTER_G 0x22
#define KEY_PRESSED_LETTER_H 0x23
#define KEY_PRESSED_LETTER_J 0x24
#define KEY_PRESSED_LETTER_K 0x25
#define KEY_PRESSED_LETTER_L 0x26
#define KEY_PRESSED_LETTER_Z 0x2C
#define KEY_PRESSED_LETTER_X 0x2D
#define KEY_PRESSED_LETTER_C 0x2E
#define KEY_PRESSED_LETTER_V 0x2F
#define KEY_PRESSED_LETTER_B 0x30
#define KEY_PRESSED_LETTER_N 0x31
#define KEY_PRESSED_LETTER_M 0x32

// Released //
// Special
#define KEY_RELEASED_LSHIFT 0xAA
#define KEY_RELEASED_RSHIFT 0xB6
#define KEY_RELEASED_LCTRL  0x9D

// Digits
#define KEY_RELEASED_DIGIT_0 0x8B
#define KEY_RELEASED_DIGIT_1 0x82
#define KEY_RELEASED_DIGIT_2 0x83
#define KEY_RELEASED_DIGIT_3 0x84
#define KEY_RELEASED_DIGIT_4 0x85
#define KEY_RELEASED_DIGIT_5 0x86
#define KEY_RELEASED_DIGIT_6 0x87
#define KEY_RELEASED_DIGIT_7 0x88
#define KEY_RELEASED_DIGIT_8 0x89
#define KEY_RELEASED_DIGIT_9 0x8A

// Letters
#define KEY_RELEASED_LETTER_Q 0x90
#define KEY_RELEASED_LETTER_W 0x91
#define KEY_RELEASED_LETTER_E 0x92
#define KEY_RELEASED_LETTER_R 0x93
#define KEY_RELEASED_LETTER_T 0x94
#define KEY_RELEASED_LETTER_Y 0x95
#define KEY_RELEASED_LETTER_U 0x96
#define KEY_RELEASED_LETTER_I 0x97
#define KEY_RELEASED_LETTER_O 0x98
#define KEY_RELEASED_LETTER_P 0x99
#define KEY_RELEASED_LETTER_A 0x9E
#define KEY_RELEASED_LETTER_S 0x9F
#define KEY_RELEASED_LETTER_D 0xA0
#define KEY_RELEASED_LETTER_F 0xA1
#define KEY_RELEASED_LETTER_G 0xA2
#define KEY_RELEASED_LETTER_H 0xA3
#define KEY_RELEASED_LETTER_J 0xA4
#define KEY_RELEASED_LETTER_K 0xA5
#define KEY_RELEASED_LETTER_L 0xA6
#define KEY_RELEASED_LETTER_Z 0xAC
#define KEY_RELEASED_LETTER_X 0xAD
#define KEY_RELEASED_LETTER_C 0xAE
#define KEY_RELEASED_LETTER_V 0xAF
#define KEY_RELEASED_LETTER_B 0xB0
#define KEY_RELEASED_LETTER_N 0xB1
#define KEY_RELEASED_LETTER_M 0xB2

bool shiftPressed = false;
bool ctrlPressed = false;
bool keyboardEnabled = false;
static bool capsLocked = false;

// All displayable keys (without shift pressed)
static key_t DISPLAYABLE_PRESSED_MAP[KEY_MAP_SIZE] = {
    // 0x00
    0, 0, '1', '2', '3', '4', '5', '6',
    // 0x08
    '7', '8', '9', '0', '-', '=', 0, 0,
    // 0x10
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    // 0x18
    'o', 'p', '[', ']', 0, 0, 'a', 's',
    // 0x20
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    // 0x28
    '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    // 0x30
    'b', 'n', 'm', ',', '.', '/', 0, 0,
    // 0x38
    0, ' ', 0, 0, 0, 0, 0, 0,
    // 0x40
    0, 0, 0, 0, 0, 0, 0, 0,
    // 0x48
    0, 0, 0, 0, 0, 0, 0, 0,
    // 0x50
    0, 0, 0, 0, 0, 0, '<', 0,
    // 0x58
    0, 0, 0, 0, 0, 0, 0, 0,
    // 0x60
    0, 0, 0, 0, 0, 0, 0, 0,
    // 0x68
    0, 0, 0, 0, 0, 0, 0, 0,
    // 0x70
    0, 0, 0, 0, 0, 0, 0, 0,
    // 0x78
    0, 0, 0, 0, 0, 0, 0, 0,
};

// When shift is pressed
static key_t DISPLAYABLE_PRESSED_MAP_UPPER[KEY_MAP_SIZE] = {
    // 0x00
    0, 0, '!', '@', '#', '$', '%', '^',
    // 0x08
    '&', '*', '(', ')', '_', '+', 0, 0,
    // 0x10
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    // 0x18
    'O', 'P', '{', '}', 0, 0, 'A', 'S',
    // 0x20
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    // 0x28
    '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
    // 0x30
    'B', 'N', 'M', '<', '>', '?', 0, 0,
    // 0x38
    0, ' ', 0, 0, 0, 0, 0, 0,
    // 0x40
    0, 0, 0, 0, 0, 0, 0, 0,
    // 0x48
    0, 0, 0, 0, 0, 0, 0, 0,
    // 0x50
    0, 0, 0, 0, 0, 0, '>', 0,
    // 0x58
    0, 0, 0, 0, 0, 0, 0, 0,
    // 0x60
    0, 0, 0, 0, 0, 0, 0, 0,
    // 0x68
    0, 0, 0, 0, 0, 0, 0, 0,
    // 0x70
    0, 0, 0, 0, 0, 0, 0, 0,
    // 0x78
    0, 0, 0, 0, 0, 0, 0, 0,
};

static char *lineBuffer;
static size_t lineBufferI;

// Sends the buffer to stdin
static void flushBuffer()
{
    // Useless
    if (lineBufferI == 0)
        return;

    sys_write(STDIN_FD, lineBuffer, lineBufferI);
    lineBufferI = 0;
}

// Pushes a char to lineBuffer, can send to stdin if the buffer is full
static void pushChar(char c)
{
    lineBuffer[lineBufferI] = c;
    ++lineBufferI;

    // Flush
    if (lineBufferI >= KBD_BUFFER_SIZE)
        flushBuffer();
}

void keyboardInit()
{
    lineBuffer = malloc(KBD_BUFFER_SIZE);

    keyboardEnabled = true;
}

void keyboardTerminate()
{
    // TODO : Disable cursor

    keyboardEnabled = false;

    free(lineBuffer);
}

void onKeyPressed()
{
    static bool wasString = false;

    unsigned char data = inb(KBD_DATA);

    if (!keyboardEnabled)
        return;

    // String
    if (wasString)
    {
        switch (data)
        {
        case KEY_PRESSED_MOD:
            // TODO : On mod pressed ()
            break;
        }

        wasString = false;

        return;
    }

    // Test controls
    switch (data)
    {
    case KEY_PRESSED_BACKSPACE:
        // Delete (maybe) the char in the console
        if (lineBufferI > 0 && shellDelete())
            // Ignore last char
            --lineBufferI;

        return;

    case KEY_PRESSED_ENTER:
        pushChar('\n');
        flushBuffer();
        consoleNewLine();

        return;

    case KEY_PRESSED_CAPSLOCK:
        capsLocked = !(capsLocked);
        return;

    case KEY_PRESSED_LSHIFT:
    case KEY_PRESSED_RSHIFT:
        shiftPressed = true;
        return;

    case KEY_RELEASED_LSHIFT:
    case KEY_RELEASED_RSHIFT:
        shiftPressed = false;
        return;

    case KEY_PRESSED_LCTRL:
        ctrlPressed = true;
        return;

    case KEY_RELEASED_LCTRL:
        ctrlPressed = false;
        return;

    case KEY_SPECIAL:
        wasString = true;
        return;
    }

    key_t key = 0;

    if (data < KEY_MAP_SIZE)
        // Key may be 0
        key = (shiftPressed ? DISPLAYABLE_PRESSED_MAP_UPPER : DISPLAYABLE_PRESSED_MAP)[data];

    // The key can be displayed
    if (key)
    {
        // Key strokes
        if (ctrlPressed)
        {
            switch (key)
            {
            case 'c':
                consolePut('^');
                consolePut('C');
                consoleNewLine();

                // Ignore buffer
                lineBufferI = 0;

                sys_terminate();
                break;
            }
        }
        else
        {
            if (isalpha(key) && capsLocked == true && shiftPressed == false) { // Shiftpress and CAPS can go very wrong
                key -= 32; // Subtract num 32 to get capital version
            }

            else if (isalpha(key) && capsLocked == true && shiftPressed == true) {
                key +=32; // Increase value so that it would look like shift and caps are interacting properly
            }

            // Display key
            consolePut(key);

            pushChar(key);
        }

        return;
    }
}
