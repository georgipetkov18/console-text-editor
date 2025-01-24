#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void terminateProgramHandler(int sig_num);

int main(int argc, char *argv[])
{
    FILE *pFile = fopen(argv[1], "r");
    char buffer[255];

    if (argv[1] == NULL)
    {
        printf("No file path was provided\n");
        return EXIT_FAILURE;
    }

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

    while (1)
    {
    }

    return EXIT_SUCCESS;
}

void terminateProgramHandler(int sig_num)
{
    printf("\nProgram terminated by Ctrl+C\n");
    exit(EXIT_SUCCESS);
}