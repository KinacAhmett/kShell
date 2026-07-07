#include <stdio.h>      // printf, fgets
#include <stdlib.h>     // exit
#include <string.h>     // strcmp, strcspn, strtok
#include <unistd.h>     // fork, execvp
#include <sys/wait.h>   // wait, waitpid

#define MAX_CHAR 1024   // bir satırda en fazla karakter
#define MAX_ARG  64     // bir komutta en fazla kelime

char *kshell_read_line(void) {
    char *buffer = malloc(MAX_CHAR);
    int position = 0;
    int c;
    
    while(1) {

        c = getchar();  

        if (!buffer) {
            fprintf(stderr, "kshell: allocation error\n");
            exit(EXIT_FAILURE);
        }

        // If we hit EOF, replace it with a null character and return.
        if (c == EOF) {
            free(buffer);
            return NULL;      
        }
        if (c == '\n') {
            buffer[position] = '\0';
            return buffer;
        }
        buffer[position] = c;
        position++;
    }

    /*if (fgets(buffer, MAX_CHAR, stdin) == NULL) {   // Ctrl+D?
        free(buffer);          // ← buffer'a ne yapmalı? (sızıntı!)
        return NULL;     // Ctrl+D → NULL döndür
    }

    buffer[strcspn(buffer, "\n")] = '\0';           // \n temizle
    return buffer;
    */
}

// --- Satırı kelimelere böl, argv dizisini doldur ---
void kshell_split_line(char *satir, char *argv[]) {

    int argc = 0;          // kaçıncı kelimedeyiz (argv index'i)
    int in_word = 0;       // şu an bir kelimenin içinde miyim? (bayrak)

    for (int i = 0; satir[i] != '\0'; i++) {
        if (satir[i] == ' ') {
            satir[i] = '\0';   // kelimeyi bitir (son harf kontrolü YOK)
            in_word = 0;       // bayrağı kapat (++ değil, = 0)
        } else {
            if (in_word == 0 && argc < MAX_ARG - 1) {          // az önce kelimede DEĞİLDİM → yeni kelime!
                argv[argc] = &satir[i];
                argc++;
                in_word = 1;             // artık kelimedeyim
            }
        }
    }
    argv[argc] = NULL;

    /*int i = 0;
    char *kelime = strtok(satir, " ");
    while (kelime != NULL && i < MAX_ARG - 1) {
        argv[i] = kelime;
        i++;
        kelime = strtok(NULL, " ");
    }
    argv[i] = NULL; */
}

int kshell_cd(char **argv) {

    if (argv[1] == NULL) {
        fprintf(stderr, "kshell : expected argument to \"cd\"\n");
    }
    else {
        if (chdir(argv[1]) != 0)
        {
            perror("kshell");
        }
    }
    return 1;
}

int kshell_exit(char **argv) {
    return 0;
}

char *builtin_str[] = {
    "cd",
    "exit"
};

int (*builtin_func[])(char **) = {
    &kshell_cd,
    &kshell_exit
};

int kshell_num_builtins(void) {
    return sizeof(builtin_str) / sizeof(char *);
}

// --- Komutu çalıştır. Devam edilecekse 1, çıkılacaksa 0 döner ---
int kshell_launch(char **argv) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(argv[0], argv);
        perror("kshell");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        wait(NULL);
    } else {
        perror("kshell");
    }
    return 1;
}

// karar ver: built-in mi dış komut mu (Brennan'ın lsh_execute'u)
int kshell_execute(char **argv) {
    if (argv[0] == NULL) return 1;

    for (int i = 0; i < kshell_num_builtins(); i++) {
        if (strcmp(argv[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(argv);
        }
    }

    return kshell_launch(argv);   // built-in değil → launch'a devret
}

// --- Ana döngü: shell yaşadıkça komut al ---
void kshell_loop(void) {
    char *argv[MAX_ARG];

    while (1) {
    printf("kshell> ");
    fflush(stdout);

    char *satir = kshell_read_line();   // kutuyu al
    if (satir == NULL) break;           // Ctrl+D → çık

    kshell_split_line(satir, argv);
    if (!kshell_execute(argv)) {
        free(satir);                     // çıkmadan önce temizle
        break;
    }
    free(satir);                         // komut bitti, kutuyu iade et
}
}

int main(void) {
    kshell_loop();
    return 0;
}