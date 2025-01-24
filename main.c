#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *pFile = fopen(argv[1], "r");
    char buffer[255];

    if (pFile == NULL)
    {
        printf("Unable to find file at: %s", argv[1]);
    }

    else
    {
        while (fgets(buffer, 255, pFile) != NULL)
        {
            printf("%s", buffer);
        }
    }

    fclose(pFile);
    return 0;
}