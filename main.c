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
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>



#define COL_BAK_BLACK           "40"
#define COL_BAK_BLUE            "44"
#define COL_BAK_CYAN            "46"
#define COL_BAK_GREEN           "42"
#define COL_BAK_GREY            "40"
#define COL_BAK_MAGENTA         "45"
#define COL_BAK_RED             "41"
#define COL_BAK_WHITE           "47"
#define COL_BAK_YELLOW          "43"
#define COL_FOR_BLACK           "0;30"
#define COL_FOR_BLUE            "0;34"
#define COL_FOR_BOLD_BLUE       "1;34"
#define COL_FOR_BOLD_CYAN       "1;36"
#define COL_FOR_BOLD_GREEN      "1;32"
#define COL_FOR_BOLD_MAGENTA    "1;35"
#define COL_FOR_BOLD_RED        "1;31"
#define COL_FOR_BOLD_WHITE      "1;37"
#define COL_FOR_BOLD_YELLOW     "1;33"
#define COL_FOR_CYAN            "0;36"
#define COL_FOR_GREEN           "0;32"
#define COL_FOR_GREY            "1;30"
#define COL_FOR_MAGENTA         "0;35"
#define COL_FOR_RED             "0;31"
#define COL_FOR_WHITE           "0;37"
#define COL_FOR_YELLOW          "0;33"
#define COL_BAK_RESET           "49"
#define COL_FOR_RESET           "39"
#define COL_RESET               "0"



#define SHORK_WIDTH     36
#define SHORK_HEIGHT    8



static int BOBBING = 1;
static int BOBBING_CHANCE = 10;
static int BOBBING_COOLDOWN = 0;
static int BOBBING_DIR = 0;
static int BOBBING_DIR_PREV = -1;
static char *COL_BAK = COL_BAK_BLUE;
static char *COL_FOR = COL_FOR_BOLD_WHITE;
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
 * Adds new lines to a given string based on the requested line width.
 * @param buffer Input string
 * @param width Characters per line
 * @param indent Indent to include after newly inserted new line
 * @return Number of lines in the string
 */
int formatNewLines(char *buffer, int width, char *indent)
{
    if (!buffer || width < 1) return 0;

    size_t bufferStrLen = strlen(buffer);
    size_t indentLen = indent ? strlen(indent) : 0;
    int lines = 1;
    int lastSpace = -1;
    int widthCount = 1;

    for (int i = 0; i < bufferStrLen; i++)
    {
        if (buffer[i] == '\033')
        {
            while (i < bufferStrLen && buffer[i] != 'm') i++;
            if (i >= bufferStrLen) break;
            continue; 
        }
        
        if (buffer[i] == ' ') lastSpace = i;
        else if (buffer[i] == '\n')
        {
            lines++;
            widthCount = 0;
            continue;
        }

        if (widthCount == width)
        {
            if (lastSpace != -1)
            {
                buffer[lastSpace] = '\n';
                lines++;

                if (indent && indentLen > 0)
                {
                    memmove(buffer + lastSpace + 1 + indentLen, buffer + lastSpace + 1, bufferStrLen - lastSpace);
                    memcpy(buffer + lastSpace + 1, indent, indentLen);
                    bufferStrLen += indentLen;
                    if (lastSpace <= i) i += indentLen;
                }
            }
            widthCount = i - lastSpace;
        }

        widthCount++;
    }

    return lines;
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

    if (BOBBING)
    {
        if (BOBBING_COOLDOWN == 0)
        {
            if (rand() % BOBBING_CHANCE == (BOBBING_CHANCE / 2))
            {
                BOBBING_COOLDOWN = SHORK_WIDTH;
                if (BOBBING_DIR == 0)
                    BOBBING_DIR = (rand() % 2 == 0) ? -1 : 1;
                else
                {
                    BOBBING_DIR = 0;
                }
            }
        }
        else BOBBING_COOLDOWN--;
    }

    // What row to start printing at
    int row = ROW_SKIP + BOBBING_DIR;

    if (BOBBING)
    {
        if (BOBBING_DIR_PREV == -1) BOBBING_DIR_PREV = row;

        // If bobbing direction changed, clear the previous start/end row that may still have
        // residual ASCII art on
        if (row != BOBBING_DIR_PREV)
        {
            int clearTop = BOBBING_DIR_PREV;
            if (clearTop < 1) clearTop = 1;
            int clearBottom = BOBBING_DIR_PREV + SHORK_HEIGHT - 1;
            if (clearBottom > TERM_SIZE.ws_row) clearBottom = TERM_SIZE.ws_row;

            for (int r = clearTop; r <= clearBottom; r++)
            {
                printf("\033[%d;1H", r);
                for (int c = 0; c < TERM_SIZE.ws_col; c++) printf(" ");
            }
            
            BOBBING_DIR_PREV = row;
        }
    }

    // Move cursor down to starting row
    printf("\033[%d;1H", row);

    // Print art
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

/**
 * Makes the terminal cursor visible again, resets the terminal's colours and clears the screen
 * upon exiting.
 */
void onExit(void)
{
    printf("\033[?25h");
    printf("\033[%sm", COL_RESET);
    clearScreen();
}

void showHelp(void)
{
    char desc[140] = "A cute, shark-themed take on Toyoda Masashi's sl command that kindly pokes fun at making typos when trying to type ls.\n";
    formatNewLines(desc, TERM_SIZE.ws_col, NULL);
    printf("%s\n", desc);

    char usage[40] = "Usage: sl [OPTIONS]\n\n";
    formatNewLines(usage, TERM_SIZE.ws_col, NULL);
    printf("%s", usage);

    char options[350] = "Options:\n-h, --help       Displays help information and exits\n-nb, --no-bob    Disables bobbing (moving the shark up and down)\n-nc, --no-col    Disables all coloured output\n-u, --update     Custom value for update speed (how quick the shark moves) (be must positive whole number)\n\n";
    formatNewLines(options, TERM_SIZE.ws_col, "                 ");
    printf("%s", options);

    char notes[80];
    snprintf(notes, 80, "Notes:\nThe host terminal size must be %dx%d before starting.\n", SHORK_WIDTH, SHORK_HEIGHT + 4);
    formatNewLines(notes, TERM_SIZE.ws_col, NULL);
    printf("%s", notes);
}



int main(int argc, char *argv[])
{
    TERM_SIZE = getTerminalSize();
    int update = 40000;

    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0))
        {
            showHelp();
            return 0;
        }
        else if ((strcmp(argv[i], "-nb") == 0) || (strcmp(argv[i], "--no-bob") == 0))
        {
            BOBBING = 0;
        }
        else if ((strcmp(argv[i], "-nc") == 0) || (strcmp(argv[i], "--no-col") == 0))
        {
            COL_BAK = COL_BAK_RESET;
            COL_FOR = COL_FOR_RESET;
        }
        else if ((strcmp(argv[i], "-u") == 0) || (strcmp(argv[i], "--update") == 0))
        {
            if (i + 1 >= argc)
            {
                printf("ERROR: update value is missing\n");
                return 1;
            }

            char *endptr = NULL;
            long val = strtol(argv[i + 1], &endptr, 10);

            if (*endptr != '\0' || val <= 0)
            {
                printf("ERROR: update value must be a positive whole number\n");
                return 1;
            }

            update = (int)val;
            printf("%d\n", update);
            continue;
        }
    }

    if (TERM_SIZE.ws_col < SHORK_WIDTH || TERM_SIZE.ws_row < SHORK_HEIGHT + 4)
    {
        printf("ERROR: terminal size too small (must be %dx%d or more)\n", SHORK_WIDTH, SHORK_HEIGHT + 4);
        return 1;
    }



    atexit(onExit);

    if (TERM_SIZE.ws_row > SHORK_HEIGHT)
        ROW_SKIP = 1 + ((TERM_SIZE.ws_row - SHORK_HEIGHT) / 2);
    if (TERM_SIZE.ws_row < SHORK_HEIGHT + 2)
        BOBBING = 0;

    srand(time(NULL));
    printf("\033[?25l\033[%s;%sm", COL_FOR, COL_BAK);
    clearScreen();

    for (int i = 1; i <= TERM_SIZE.ws_col + SHORK_WIDTH; i++)
    {
        printFrame(i);
        usleep(update);
    }

    return 0;
}
