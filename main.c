#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int main(void) {
    int input;
    while (1) {
        char buf[64];
        char *endptr;
        printf("Inserisci il numero di bambini: ");
        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("Numero non valido, inserisci un numero intero positivo.\n");
            continue;
        }
        if (buf[0] == '\n') {
            printf("Numero non valido, inserisci un numero intero positivo.\n");
            continue;
        }
        errno = 0;
        long val = strtol(buf, &endptr, 10);
        if (endptr == buf || errno == ERANGE || val <= 0) {
            printf("Numero non valido, inserisci un numero intero positivo.\n");
            continue;
        }
        while (*endptr == ' ' || *endptr == '\t') {
            endptr++;
        }
        if (*endptr != '\n' && *endptr != '\0') {
            printf("Numero non valido, inserisci un numero intero positivo.\n");
            continue;
        }
        input = (int)val;
        break;
    }

    int fileDescriptor[2];
    if (pipe(fileDescriptor) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    int fileDescriptor2[2];
    if (pipe(fileDescriptor2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid_t elf1;
    if ((elf1 = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (elf1 == 0) {
        close(fileDescriptor2[0]);
        close(fileDescriptor2[1]);
        int kidsNumber;
        if (read(fileDescriptor[0], &kidsNumber, sizeof(int)) != sizeof(int)) {
            perror("read from Santa");
            exit(EXIT_FAILURE);
        }
        close(fileDescriptor[0]);
        int goodKids = 0;
        for (int i = 1; i <= kidsNumber; i += 2) {
            goodKids++;
        }
        if (write(fileDescriptor[1], &goodKids, sizeof(int)) != sizeof(int)) {
            perror("write to Santa");
            exit(EXIT_FAILURE);
        }
        close(fileDescriptor[1]);
        exit(EXIT_SUCCESS);
    } else {
        pid_t elf2;
        if ((elf2 = fork()) == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (elf2 == 0) {
            close(fileDescriptor[0]);
            close(fileDescriptor[1]);
            int kidsNumber;
            if (read(fileDescriptor2[0], &kidsNumber, sizeof(int)) != sizeof(int)) {
                perror("read from Santa");
                exit(EXIT_FAILURE);
            }
            close(fileDescriptor2[0]);
            int badKids = 0;
            for (int i = 2; i <= kidsNumber; i += 2) {
                badKids++;
            }
            if (write(fileDescriptor2[1], &badKids, sizeof(int)) != sizeof(int)) {
                perror("write to Santa");
                exit(EXIT_FAILURE);
            }
            close(fileDescriptor2[1]);
            exit(EXIT_SUCCESS);
        } else {
            if (write(fileDescriptor[1], &input, sizeof(int)) != sizeof(int)) {
                perror("write to elf1");
                exit(EXIT_FAILURE);
            }
            if (write(fileDescriptor2[1], &input, sizeof(int)) != sizeof(int)) {
                perror("write to elf2");
                exit(EXIT_FAILURE);
            }
            close(fileDescriptor[1]);
            close(fileDescriptor2[1]);
            wait(NULL);
            wait(NULL);
            int goodKids, badKids;
            if (read(fileDescriptor[0], &goodKids, sizeof(int)) != sizeof(int)) {
                perror("read from elf1");
                exit(EXIT_FAILURE);
            }
            if (read(fileDescriptor2[0], &badKids, sizeof(int)) != sizeof(int)) {
                perror("read from elf2");
                exit(EXIT_FAILURE);
            }
            close(fileDescriptor[0]);
            close(fileDescriptor2[0]);
            printf("Quest'anno consegnerò %d regali e %d pezzi di carbone!\n", goodKids, badKids);
        }
    }
    return 0;
}