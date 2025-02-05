#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include "appEnums.h"

#define MAX_LINES 1000 // Max lines the editor can handle
#define MAX_LINE_LENGTH 256 // Max characters per line

HANDLE hConsole;
char filePath[2048];
long fileSize = 0;
int consoleWidth;
int consoleHeight;
int cursor_x = 0, cursor_y = 0; // Virtual cursor
int top_line = 0;
char text[MAX_LINES][MAX_LINE_LENGTH];
int total_lines = 0;

void terminateProgram();
void handleKeyInput(Key key);
void printCharacter(COORD coord, char ch);
COORD getCursorPosition();
void setCursorPosition(int x, int y);

void loadFile()
{
    FILE *file = fopen(filePath, "r");
    if (!file)
    {
        printf("Error: Unable to open file %s\n", filePath);
        exit(1);
    }

    total_lines = 0;
    while (fgets(text[total_lines], MAX_LINE_LENGTH, file) && total_lines < MAX_LINES)
    {
        text[total_lines][strcspn(text[total_lines], "\r\n")] = 0; // Remove new lines
        total_lines++;
    }
    fclose(file);
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

void draw_screen()
{
    getConsoleSize(&consoleWidth, &consoleHeight);

    // system("cls");
    printf("\e[1;1H\e[2J");
    for (int i = 0; i < consoleHeight && (top_line + i) < total_lines; i++) {
        printf("%s\n", text[top_line + i]);  
    }

    setCursorPosition(cursor_x, cursor_y - top_line); // Adjust cursor in viewport
}

void move_cursor_up() {
    if (cursor_y > 0) {
        cursor_y--;
        if (cursor_y < top_line) {
            top_line--;  // Scroll up
            draw_screen();
        } else {
            setCursorPosition(cursor_x, cursor_y - top_line);
        }
    }
}

void move_cursor_down() {
    if (cursor_y < total_lines - 1) {
        cursor_y++;
        getConsoleSize(&consoleWidth, &consoleHeight);

        if (cursor_y >= top_line + consoleHeight) {
            top_line++;  // Scroll down
            draw_screen();
        } else {
            setCursorPosition(cursor_x, cursor_y - top_line);
        }
    }
}

int main(int argc, char *argv[])
{
    strcpy(filePath, argv[1]);
    if (filePath == NULL)
    {
        printf("No file path was provided\n");
        return EXIT_FAILURE;
    }

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    loadFile();
    draw_screen();

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
    CloseHandle(hConsole);
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
        move_cursor_up();
        break;

    case Down:
        move_cursor_down();
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

        FILE *pFile = fopen(filePath, "r+");
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
    FillConsoleOutputCharacter(hConsole, ch, 1, coord, &bytesWritten);
    setCursorPosition(coord.X + 1, coord.Y);
}

COORD getCursorPosition()
{
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;

    COORD cursorPosition;
    cursorPosition.X = -10;
    cursorPosition.Y = -10;
    if (GetConsoleScreenBufferInfo(hConsole, &bufferInfo))
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
    SetConsoleCursorPosition(hConsole, cursorPosition);
}