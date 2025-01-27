#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <conio.h>

enum ArrowKey
{
    Up = 72,
    Down = 80,
    Left = 75,
    Right = 77
};

void terminateProgramHandler(int sig_num);
void moveCursor(enum ArrowKey key);

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

    signal(SIGINT, terminateProgramHandler);

    int key;
    while (1)
    {
        key = getch();
        moveCursor(key);
    }

    return EXIT_SUCCESS;
}

void terminateProgramHandler(int sig_num)
{
    printf("\nProgram terminated by Ctrl+C\n");
    exit(EXIT_SUCCESS);
}

void moveCursor(enum ArrowKey key)
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