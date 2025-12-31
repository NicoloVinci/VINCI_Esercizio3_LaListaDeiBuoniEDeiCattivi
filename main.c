#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    int valid, input;
    char c;
    while (1) {
        valid = 1;
        printf("Inserisci il numero di bambini: ");
        if (scanf("%d", &input) != 1 || input < -1) {
            valid = 0;
        }
        do {
            c = getchar();
            if (c != '\n' && c != ' ' && c != '\t') {
                valid = 0;
            }
        } while (c != '\n' && c != EOF);
        if (!valid) {
            printf("Numero non valido, inserisci un numero intero positivo.\n");
            continue;
        }
        break;
    }
    int fileDescriptor[2];
    if (pipe(fileDescriptor) == -1) {
        perror("pipe");
        exit(-1);
    }
    int fileDescriptor2[2];
    if (pipe(fileDescriptor2) == -1) {
        perror("pipe");
        exit(-1);
    }
    pid_t Elf1;
    if ((Elf1 = fork()) == -1) {
        perror("fork");
        exit(-1);
    }
    if (Elf1 == 0) {
        close(fileDescriptor2[0]);
        close(fileDescriptor2[1]);
        int kidsNumber;
        read(fileDescriptor[0], &kidsNumber, sizeof(int));
        close(fileDescriptor[0]);
        int goodKids = 0;
        for (int i = 1; i <= kidsNumber; i += 2) {
            goodKids++;
        }
        write(fileDescriptor[1], &goodKids, sizeof(int));
        close(fileDescriptor[1]);
    } else {
        pid_t Elf2;
        if ((Elf2 = fork()) == -1) {
            perror("fork");
            exit(-1);
        }
        if (Elf2 == 0) {
            close(fileDescriptor[0]);
            close(fileDescriptor[1]);
            int kidsNumber;
            read(fileDescriptor2[0], &kidsNumber, sizeof(int));
            close(fileDescriptor2[0]);
            int badKids = 0;
            for (int i = 2; i <= kidsNumber; i += 2) {
                badKids++;
            }
            write(fileDescriptor2[1], &badKids, sizeof(int));
            close(fileDescriptor2[1]);
        } else {
            write(fileDescriptor[1], &input, sizeof(int));
            write(fileDescriptor2[1], &input, sizeof(int));
            close(fileDescriptor[1]);
            close(fileDescriptor2[1]);
            wait(NULL);
            wait(NULL);
        }
    }
    return 0;
}