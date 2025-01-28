#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "appEnums.h"

void terminateProgram();
void moveCursor(Key key);

int main(int argc, char *argv[])
{
    if (argv[1] == NULL)
    {
        printf("No file path was provided\n");
        return EXIT_FAILURE;
    }

    FILE *pFile = fopen(argv[1], "r");
    char buffer[255];

    if (pFile == NULL)
    {
        printf("Unable to find file at: %s", argv[1]);
        return EXIT_FAILURE;
    }

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
        // char a = (char)key;
        // printf("%d", key);

        if (key == CtrlC)
        {
            terminateProgram(key);
        }

        if (key == Delete)
        {
            printf("\033[P");
        }
        
        if (key == Backspace)
        {
            printf("\033[1D");
            printf("\033[P");
        }

        moveCursor(key);
    }

    return EXIT_SUCCESS;
}

void terminateProgram()
{
    // Move cursor to the end of the text
    printf("\033[u");
    printf("\nProgram terminated by Ctrl+C\n");
    exit(EXIT_SUCCESS);
}

void moveCursor(Key key)
{
    char directionChar;
    switch (key)
    {
    case Up:
        directionChar = 'A';
        break;

    case Down:
        directionChar = 'B';
        break;

    case Right:
        directionChar = 'C';
        break;

    case Left:
        directionChar = 'D';
        break;

    default:
        return;
    }

    printf("\033[1%c", directionChar);
}