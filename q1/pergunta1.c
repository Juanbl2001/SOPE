#include "pergunta1.h"
#include <ctype.h>
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
    char ch, next_ch, prev_ch;
    ch = 'a';


    if (NULL == txt) {
        printf("file can't be opened \n");
    }
    int counter = 1;
    printf("[%d] ",counter);
    counter++;

    do {
        prev_ch = ch;
        ch = fgetc(txt);
        next_ch = peek(txt);
        if((ch == '.' || ch == '!' || ch=='?') && next_ch != EOF ){
            printf("%c\n", ch);
            printf("[%d]",counter);
            counter++;
        }
        else if(ch == EOF){
            printf("%c", 10);
            break;
        }
        else{
           if(ch == 13){ //prevent newline in between sentences
            ch = fgetc(txt);
            printf(" ");
           }
           else{
               printf("%c", ch); 
           }
        }
        //fflush(stdout);
    } while (ch != EOF);
 
    // Closing the file
    fclose(txt);
    return 0;
}