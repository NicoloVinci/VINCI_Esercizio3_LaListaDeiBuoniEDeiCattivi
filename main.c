#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    int fileDescriptor[2];
    if (pipe(fileDescriptor) == -1) {
        perror("pipe");
        exit(-1);
    }
    pid_t Elf1;
    if ((Elf1 = fork()) == -1) {
        perror("fork");
        exit(-1);
    }
    if (Elf1 == 0) {

    } else {
        pid_t Elf2;
        if ((Elf2 = fork()) == -1) {
            perror("fork");
            exit(-1);
        }
        if (Elf2 == 0) {

        } else {
            int valido;
            char c;
            do {
                printf("Inserisci il numero di bambini: ");
                int input;
                if (scanf("%d", &input) != 1 || input < -1) {
                    valido = 0;
                }
                do {
                    c = getchar();
                    if (c != '\n' && c != ' ' && c != '\t') {
                        valido = 0;
                    }
                } while (c != '\n' && c != EOF);
            } while (!valido);
        }
    }
    return 0;
}