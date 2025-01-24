#include <stdio.h>

int main()
{
    printf("Enter file path: ");

    char filePath[1024];
    scanf("%s", &filePath);

    FILE *pFile = fopen(filePath, "r");
    char buffer[255];

    if (pFile == NULL)
    {
        printf("Unable to find file at: %s", filePath);
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