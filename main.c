#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include "appEnums.h"

#define MAX_LINES 1000 // Max lines the editor can handle
#define MAX_LINE_LENGTH 256 // Max characters per line

HANDLE console_handler;
char file_path[2048];
long file_size = 0;
char text[MAX_LINES][MAX_LINE_LENGTH];
int lines_read = 0;

int console_width;
int console_height;

int cursor_x = 0, cursor_y = 0; // Virtual cursor
int top_line = 0;

void terminateProgram();
void handleKeyInput(Key key);
void printCharacter(COORD coord, char ch);
COORD getCursorPosition();
void setCursorPosition(int x, int y);
void loadFile();
void getConsoleSize(int *width, int *height);
void drawScreen();
void moveCursorUp();
void moveCursorDown();


int main(int argc, char *argv[])
{
    strcpy(file_path, argv[1]);
    if (file_path == NULL)
    {
        printf("No file path was provided\n");
        return EXIT_FAILURE;
    }

    console_handler = GetStdHandle(STD_OUTPUT_HANDLE);

    loadFile();
    drawScreen();

    int key;

    printf("\033[s"); // Save cursor positon at the end of the text
    while (1)
    {
        key = getch();
        handleKeyInput(key);
    }

    return EXIT_SUCCESS;
}

void terminateProgram()
{
    CloseHandle(console_handler);
    printf("\033[u"); // Move cursor to the end of the text
    printf("\nProgram terminated by Ctrl+C\n");
    exit(EXIT_SUCCESS);
}

void handleKeyInput(Key key)
{
    COORD coord = getCursorPosition();
    switch (key)
    {
    case Up:
        moveCursorUp();
        break;

    case Down:
        moveCursorDown();
        break;

    case Right:
        setCursorPosition(coord.X + 1, coord.Y);
        break;

    case Left:
        setCursorPosition(coord.X - 1, coord.Y);
        break;

    case CtrlC:
        terminateProgram();
        break;

    case Delete:
        printf("\033[P");
        break;

    case Backspace:
        printf("\033[1D");
        printf("\033[P");
        break;

    case 224:

        break;

    default:
        printCharacter(coord, (char)key);

        char ch;
        int lineCounter = 0;

        FILE *pFile = fopen(file_path, "r+");
        while ((ch = fgetc(pFile)) != EOF)
        {
            if (lineCounter == coord.Y)
            {
                fseek(pFile, coord.X - 1, SEEK_CUR);
                fputc((char)key, pFile);
                break;
            }

            else if (ch == '\n')
            {
                lineCounter++;
            }
        }

        fclose(pFile);
        break;
    }
}

void printCharacter(COORD coord, char ch)
{
    DWORD bytesWritten;
    FillConsoleOutputCharacter(console_handler, ch, 1, coord, &bytesWritten);
    setCursorPosition(coord.X + 1, coord.Y);
}

COORD getCursorPosition()
{
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;

    COORD cursorPosition;
    cursorPosition.X = -10;
    cursorPosition.Y = -10;
    if (GetConsoleScreenBufferInfo(console_handler, &bufferInfo))
    {
        cursorPosition = bufferInfo.dwCursorPosition;
    }

    return cursorPosition;
}

void setCursorPosition(int x, int y)
{
    if (x < 0 || y < 0)
    {
        return;
    }

    COORD cursorPosition;
    cursorPosition.X = x;
    cursorPosition.Y = y;
    SetConsoleCursorPosition(console_handler, cursorPosition);
}

void loadFile()
{
    FILE *pFile = fopen(file_path, "r");
    if (!pFile)
    {
        printf("Error: Unable to open file %s\n", file_path);
        exit(EXIT_FAILURE);
    }

    lines_read = 0;
    while (fgets(text[lines_read], MAX_LINE_LENGTH, pFile) && lines_read < MAX_LINES)
    {
        text[lines_read][strcspn(text[lines_read], "\r\n")] = 0; // Remove new lines
        lines_read++;
    }
    fclose(pFile);
}

void getConsoleSize(int *width, int *height)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (GetConsoleScreenBufferInfo(hStdOut, &csbi))
    {
        *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    else
    {
        *width = 80; // Default fallback
        *height = 25;
    }
}

void drawScreen()
{
    getConsoleSize(&console_width, &console_height);

    // system("cls");
    printf("\e[1;1H\e[2J");
    for (int i = 0; i < console_height && (top_line + i) < lines_read; i++) {
        printf("%s\n", text[top_line + i]);  
    }

    setCursorPosition(cursor_x, cursor_y - top_line); // Adjust cursor in viewport
}

void moveCursorUp() {
    if (cursor_y > 0) {
        cursor_y--;
        if (cursor_y < top_line) {
            top_line--;  // Scroll up
            drawScreen();
        } else {
            setCursorPosition(cursor_x, cursor_y - top_line);
        }
    }
}

void moveCursorDown() {
    if (cursor_y < lines_read - 1) {
        cursor_y++;
        getConsoleSize(&console_width, &console_height);

        if (cursor_y >= top_line + console_height) {
            top_line++;  // Scroll down
            drawScreen();
        } else {
            setCursorPosition(cursor_x, cursor_y - top_line);
        }
    }
}