#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char const *argv[])
{
    int ChildtoParent[2], ParenttoChild[2];

    // Pipe Creation
    if (pipe(ChildtoParent) == -1)
        return 1;
    if (pipe(ParenttoChild) == -1)
        return 1;

    // Fork
    int check = fork();
    if (check == -1)
        return 1;

    if (check == 0)
    {
        // Child process
        close(ChildtoParent[0]);
        close(ParenttoChild[1]);

        char dataSize[1000];
        int n;
        read(ParenttoChild[0], &n, sizeof(int));
        read(ParenttoChild[0], dataSize, sizeof(char) * n);

        FILE *cypherf;

        cypherf = fopen("cypher.txt", "r");
        if (cypherf == NULL)
        {
            printf("Error opening cypher.txt file");
            return 1;
        }

        char decryption[50], encryption[50];

        while (1)
        {
            if (fscanf(cypherf, "%s", decryption) != 1)
                break;
            if (fscanf(cypherf, "%s", encryption) != 1)
                break;

            int changed = 0;
            char *ptr;
            ptr = strstr(dataSize, decryption);
            memmove(ptr + strlen(encryption), ptr + strlen(decryption), strlen(ptr + strlen(decryption)) + 1);
            if (ptr != NULL)
            {
                changed = 1;
                strncpy(ptr, encryption, strlen(encryption));
            }
            if (!changed)
            {
                ptr = strstr(dataSize, encryption);
                memmove(ptr + strlen(decryption), ptr + strlen(encryption), strlen(ptr + strlen(encryption)) + 1);
                if (ptr != NULL)
                {
                    strncpy(ptr, decryption, strlen(decryption));
                }
            }
        }
        fclose(cypherf);

        n = strlen(dataSize) + 1;
        write(ChildtoParent[1], &n, sizeof(int));
        write(ChildtoParent[1], &dataSize, sizeof(char) * n);

        close(ChildtoParent[1]);
        close(ParenttoChild[0]);
    }
    else
    {
        // Parent process
        close(ChildtoParent[1]);
        close(ParenttoChild[0]);

        char fileData[200], dataSize[1000] = "";
        while (fgets(fileData, 200, stdin) != NULL)
        {
            strcat(dataSize, fileData);
        }

        dataSize[strlen(dataSize) - 1] = '\n';

        int n = strlen(dataSize) + 1;
        write(ParenttoChild[1], &n, sizeof(int));
        write(ParenttoChild[1], &dataSize, sizeof(char) * n);

        char processedData[1000];
        read(ChildtoParent[0], &n, sizeof(int));
        read(ChildtoParent[0], processedData, sizeof(char) * n);

        printf("%s", processedData);

        FILE *cypheredf;

        cypheredf = fopen("text_cyphered.txt", "w");
        if (cypheredf == NULL)
        {
            printf("Error opening text.txt file");
            return 1;
        }

        fprintf(cypheredf, "%s", processedData);

        fclose(cypheredf);

        close(ChildtoParent[0]);
        close(ParenttoChild[1]);
        wait(NULL);
    }

    return 0;
}
