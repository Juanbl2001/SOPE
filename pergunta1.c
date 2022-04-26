#include "pergunta1.h"
#define MAX_STR_SIZE 60

int peek(FILE *stream)
{
    int c;

    c = fgetc(stream);
    ungetc(c, stream);

    return c;
}

int main(int argc, char *argv[])
{
    char *str = (char *)malloc(MAX_STR_SIZE * sizeof(char));
    strcpy(str, argv[1]);

    FILE* txt = fopen(str,"r");
    char ch;

    if (NULL == txt) {
        printf("file can't be opened \n");
    }
    int counter = 1;
    printf("[%d] ",counter);
    counter++;

    do {
        ch = fgetc(txt);
        if((ch == '.' || ch == '!' || ch=='?')&& peek(txt)!= EOF ){
            printf("%c\n", ch);
            printf("[%d] ",counter);
            counter++;
        }
        else if(ch == EOF){
            printf("\n");
            break;
        }
        else{
           printf("%c", ch); 
        }
    } while (ch != EOF);
 
    // Closing the file
    fclose(txt);
    return 0;
}