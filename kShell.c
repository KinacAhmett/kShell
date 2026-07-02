#include <stdio.h>      //writing on screen, reading sentences
#include <stdlib.h>      //for exit
#include <string.h>     //comparing string

#define MAX_CHAR 1024

int main(void) {
    char satir[MAX_CHAR];
    while (1)
    {
        printf("Listening: ");
        fflush(stdout);
        if (fgets(satir, MAX_CHAR, stdin) == NULL) {
            break;
        };
        satir[strcspn(satir, "\n")] = '\0';
        if (strcmp(satir, "exit") == 0) break;
        printf("You've been written:  %s\n", satir);
    }

    return 0;
    
}

