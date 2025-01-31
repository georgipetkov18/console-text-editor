#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include "appEnums.h"

HANDLE hConsole;
char filePath[2048];

void terminateProgram();
void handleKeyInput(Key key);
void printCharacter(COORD coord, char ch);
COORD getCursorPosition();
void setCursorPosition(int x, int y);

int main(int argc, char *argv[])
{
    strcpy(filePath, argv[1]);

    if (filePath == NULL)
    {
        printf("No file path was provided\n");
        return EXIT_FAILURE;
    }

    FILE *pFile = fopen(filePath, "r");
    char buffer[255];

    if (pFile == NULL)
    {
        printf("Unable to find file at: %s", filePath);
        return EXIT_FAILURE;
    }

    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Clear console
    printf("\e[1;1H\e[2J");

    while (fgets(buffer, 255, pFile) != NULL)
    {
        printf("%s", buffer);
    }

    fclose(pFile);

    int key;

    // Save cursor positon at the end of the text
    printf("\033[s");
    while (1)
    {
        key = getch();
        // printf("\033[H");
        handleKeyInput(key);
    }

    return EXIT_SUCCESS;
}

void terminateProgram()
{
    CloseHandle(hConsole);
    // Move cursor to the end of the text
    printf("\033[u");
    printf("\nProgram terminated by Ctrl+C\n");
    exit(EXIT_SUCCESS);
}

void handleKeyInput(Key key)
{
    COORD coord = getCursorPosition();
    switch (key)
    {
    case Up:
        setCursorPosition(coord.X, coord.Y - 1);
        // printf("\033[1A");
        break;

    case Down:
        setCursorPosition(coord.X, coord.Y + 1);
        // printf("\033[1B");
        break;

    case Right:
        setCursorPosition(coord.X + 1, coord.Y);
        // printf("\033[1C");
        break;

    case Left:
        setCursorPosition(coord.X - 1, coord.Y);
        // printf("\033[1D");
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
        // printf("%c", (char)key);

        FILE *pFile = fopen(filePath, "r+");
        char ch;
        int lineCounter = 0;
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
    // WriteConsoleOutputCharacter(hConsole, &ch, 1, point, NULL);
    // WriteFile(hConsole, &ch, 1, &bytesWritten, NULL);
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