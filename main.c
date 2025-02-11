#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <ctype.h>
#include "appEnums.h"

#define MAX_LINES 1000      // Max lines the editor can handle
#define MAX_LINE_LENGTH 256 // Max characters per line

HANDLE console_handler;
char file_path[2048];
long file_size = 0;
char text[MAX_LINES][MAX_LINE_LENGTH];
int file_lines = 0;

int console_width;
int console_height;

int cursor_x = 0, cursor_y = 0; // Virtual cursor
int top_line = 0;

void terminateProgram();
void handleKeyInput(Key key, BOOL *expectLetter);
void printChar(COORD coord, char ch);
COORD getCursorPosition();
void setCursorPosition(int x, int y);
void loadFile();
void getConsoleSize(int *width, int *height);
void drawScreen();
void moveCursorUp();
void moveCursorDown();
void insertChar(int row, int pos, char ch);
void removeChar(int row, int pos, Key key);
void writeToFile();
int getLength(char string[]);
void fixCursorXPosition(int *x);

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
    BOOL expectLetter = TRUE;

    while (1)
    {
        key = getch();
        handleKeyInput(key, &expectLetter);
    }

    return EXIT_SUCCESS;
}

void terminateProgram()
{
    setCursorPosition(cursor_x, console_height - 1);
    CloseHandle(console_handler);
    exit(EXIT_SUCCESS);
}

void handleKeyInput(Key key, BOOL *expectLetter)
{
    COORD coord = getCursorPosition();
    if (*expectLetter && (isalpha((char)key) || isdigit((char)key) || key == Space))
    {
        insertChar(cursor_y, cursor_x, (char)key);
        int length = getLength(text[cursor_y]);

        for (int i = 0; i < length; i++)
        {
            COORD coord = {i, cursor_y - top_line};
            printChar(coord, text[cursor_y][i]);
        }

        cursor_x++;
        setCursorPosition(cursor_x, cursor_y - top_line);
        return;
    }

    switch (key)
    {
    case Up:
        moveCursorUp();
        *expectLetter = TRUE;
        break;

    case Down:
        moveCursorDown();
        *expectLetter = TRUE;
        break;

    case Right:
        cursor_x++;
        setCursorPosition(cursor_x, cursor_y - top_line);
        *expectLetter = TRUE;
        break;

    case Left:
        if (cursor_x > 0)
        {
            cursor_x--;
            setCursorPosition(cursor_x, cursor_y - top_line);
            *expectLetter = TRUE;
        }
        break;

    case Delete:
        removeChar(cursor_y, cursor_x, Delete);
        printf("\033[P");
        *expectLetter = TRUE;
        break;

    case Backspace:
        if (cursor_x > 0)
        {
            removeChar(cursor_y, cursor_x, Backspace);
            cursor_x--;
            setCursorPosition(cursor_x, cursor_y - top_line);
            printf("\033[P");
        }
        *expectLetter = TRUE;
        break;

    case CtrlC:
        terminateProgram();
        break;

    case CtrlS:
        writeToFile();
        terminateProgram();
        break;

    case 224:
        *expectLetter = FALSE;
        break;

    default:
        break;
    }
}

void printChar(COORD coord, char ch)
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

    fixCursorXPosition(&x);

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

    file_lines = 0;
    while (fgets(text[file_lines], MAX_LINE_LENGTH, pFile) && file_lines < MAX_LINES)
    {
        text[file_lines][strcspn(text[file_lines], "\r\n")] = 0; // Remove new lines
        file_lines++;
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

    printf("\e[1;1H\e[2J");
    for (int i = 0; i < console_height && (top_line + i) < file_lines; i++)
    {
        // Last line so do not print \n
        if (i == console_height - 1 || (top_line + i) == file_lines - 1)
        {
            printf("%s", text[top_line + i]);
        }
        else
        {
            printf("%s\n", text[top_line + i]);
        }
    }

    setCursorPosition(cursor_x, cursor_y - top_line); // Adjust cursor in viewport
}

void moveCursorUp()
{
    if (cursor_y > 0)
    {
        cursor_y--;
        if (cursor_y < top_line)
        {
            top_line--; // Scroll up
            drawScreen();
        }
        else
        {
            setCursorPosition(cursor_x, cursor_y - top_line);
        }
    }
}

void moveCursorDown()
{
    if (cursor_y < file_lines - 1)
    {
        cursor_y++;
        getConsoleSize(&console_width, &console_height);

        if (cursor_y >= top_line + console_height)
        {
            top_line++; // Scroll down
            drawScreen();
        }
        else
        {
            setCursorPosition(cursor_x, cursor_y - top_line);
        }
    }
}

void insertChar(int row, int pos, char ch)
{
    int length = getLength(text[row]);

    if (length == MAX_LINE_LENGTH)
    {
        // No more space in this line
        return;
    }

    char result_start[length + 1];
    strncpy(result_start, text[row], pos);
    result_start[pos] = ch;

    // There is something to append
    if (length != pos)
    {
        char result_end[length - pos];
        strncpy(result_end, text[row] + pos, length - pos);
        strcpy(result_start + pos + 1, result_end);
    }

    strncpy(text[row], result_start, length + 1);
}

void removeChar(int row, int pos, Key key)
{
    if (key == Backspace && pos == 0)
    {
        // Beginning of the line nothing to remove
        return;
    }

    int length = getLength(text[row]);

    if (key == Delete && pos == length)
    {
        // End of line nothing to delete
        return;
    }

    char result_start[length - 1];
    char result_end[length - pos];
    if (key == Backspace)
    {
        strncpy(result_start, text[row], pos - 1);
        strncpy(result_end, text[row] + pos, length - pos);
    }

    else if (key == Delete)
    {
        strncpy(result_start, text[row], pos);
        strncpy(result_end, text[row] + pos + 1, length - pos - 1);
    }

    strcat(result_start, result_end);
    strncpy(text[row], result_start, length - 1);
    text[row][length - 1] = '\000';
}

void writeToFile()
{
    FILE *pFile = fopen(file_path, "w");

    for (int i = 0; i < file_lines; i++)
    {
        fprintf(pFile, text[i]);
        fprintf(pFile, "\n");
    }

    fclose(pFile);
}

int getLength(char string[])
{
    int length = 0;

    while (string[length] != '\0')
    {
        length++;
    }

    return length;
}

void fixCursorXPosition(int *x)
{
    // Move cursor to the end of the current line and not outside of it
    int length = getLength(text[cursor_y]);
    if (cursor_x > length)
    {
        cursor_x = length;
        *x = length;
    }
}