/*
    ######################################################
    ##          SHORK UTILITY - SHORKLOCOMOTIVE         ##
    ######################################################
    ## A cute SHORK-themed sl implementation that       ##
    ## "cures your bad habit of mistyping" ls. A homage ##
    ## to the original sl by Toyoda Masashi.            ##
    ######################################################
    ## Licence: GNU GENERAL PUBLIC LICENSE Version 3    ##
    ######################################################
    ## Kali (links.sharktastica.co.uk)                  ##
    ######################################################
*/



#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>



#define SHORK_WIDTH     36
#define SHORK_HEIGHT    8



static struct termios OLD_TERMIOS;
static int RAW_MODE_ENABLED = 0;
static int ROW_SKIP = 0;
const char SHORK[SHORK_HEIGHT][SHORK_WIDTH] = {
    "                ^`.                 ",
    "^_              \\  \\                ",
    "\\ \\            /    `~~~--__        ",
    "{  \\___----~~'              `~~-_   ",
    "\\                      \\\\\\\\\\  o  `. ",
    "/ /~~~-, ,__.    ,    ,      __,,,,)",
    "\\/     \\/    `~~~;   ,---~~--`      ",
    "                '._.'               "
};
static struct winsize TERM_SIZE;



/**
 * Moves the cursor to topleft-most position and clears below cursor.
 */
void clearScreen(void)
{
    printf("\033[H\033[J");
}

/**
 * @return winsize struct containing the current terminal size in columns and rows
 */
struct winsize getTerminalSize(void)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
        ws.ws_col = 80;
        ws.ws_row = 24;
    }
    return ws;
}

void printFrame(int frame)
{
    int padding = -(SHORK_WIDTH - frame);

    clearScreen();

    for (int i = 0; i < ROW_SKIP; i++)
        printf("\n");

    for (int i = 0; i < SHORK_HEIGHT; i++)
    {
        int visibleStart = padding;
        if (visibleStart < 0)
            visibleStart = 0;

        for (int s = 0; s < visibleStart; s++)
            printf(" ");

        int shorkStart = visibleStart - padding;
        int maxVisible = TERM_SIZE.ws_col - visibleStart;
        int shorkLen = SHORK_WIDTH - shorkStart;

        if (shorkLen > maxVisible)
            shorkLen = maxVisible;

        for (int j = 0; j < shorkLen; j++)
            printf("%c", SHORK[i][shorkStart + j]);
        printf("\n");
    }
}

void showCursor(void)
{
    printf("\033[?25h");
}



int main(int argc, char *argv[])
{
    atexit(showCursor);

    TERM_SIZE = getTerminalSize();

    if (TERM_SIZE.ws_row > SHORK_HEIGHT)
    {
        int test = (TERM_SIZE.ws_row - SHORK_HEIGHT) / 2;
        if (test > 1) test--;
        ROW_SKIP = test;
    }

    printf("\033[?25l");
    for (int i = 1; i <= TERM_SIZE.ws_col + SHORK_WIDTH; i++)
    {
        printFrame(i);
        usleep(30000);
    }

    return 0;
}
