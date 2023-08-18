#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define PIPE1 "pipe1"
#define PIPE2 "pipe2"
#define PIPE3 "pipe3"
#define PIPE4 "pipe4"
#define PIPE5 "pipe5"

void process1() {
    char buffer[31];
    int fd1 = open(PIPE1, O_WRONLY);
    int fd5 = open(PIPE5, O_WRONLY);

    printf("Enter text: ");
    fgets(buffer, sizeof(buffer), stdin);
    write(fd1, buffer, strlen(buffer) + 1);
    write(fd5, buffer, strlen(buffer) + 1); // Write the same string to another pipe for process 3

    close(fd1);
    close(fd5);
}

void process2() {
    char buffer[100];
    int fd1 = open(PIPE1, O_RDONLY);
    int fd2 = open(PIPE2, O_WRONLY);

    read(fd1, buffer, sizeof(buffer));

    for (int i = 0; i < strlen(buffer); i++) {
        if (isdigit(buffer[i])) {
            buffer[i] = '9';
        }
    }

    write(fd2, buffer, strlen(buffer) + 1);

    close(fd1);
    close(fd2);
}

void process3() {
    char buffer[100];
    int fd5 = open(PIPE5, O_RDONLY);
    int fd3 = open(PIPE3, O_WRONLY);

    read(fd5, buffer, sizeof(buffer));

    for (int i = 0; i < strlen(buffer); i++) {
        if (isalpha(buffer[i])) {
            buffer[i] = '0';
        }
    }

    write(fd3, buffer, strlen(buffer) + 1);

    close(fd5);
    close(fd3);
}

void process4() {
    char buffer2[100];
    char buffer3[100];
    char buffer[100];

    int fd2 = open(PIPE2, O_RDONLY);
    int fd3 = open(PIPE3, O_RDONLY);

    read(fd2, buffer2, sizeof(buffer2));
    read(fd3, buffer3, sizeof(buffer3));

    // Merging process
    for (int i = 0; i < strlen(buffer2); i++) {
        if (buffer2[i] == '9') {
            buffer[i] = '9';
        } else {
            buffer[i] = buffer3[i];
        }
    }

    printf("Modified text: %s\n", buffer);

    close(fd2);
    close(fd3);
}


int main() {
    mkfifo(PIPE1, 0666);
    mkfifo(PIPE2, 0666);
    mkfifo(PIPE3, 0666);
    mkfifo(PIPE4, 0666);
    mkfifo(PIPE5, 0666);

    pid_t pid1, pid2, pid3, pid4;
    pid1 = fork();

    if (pid1 == 0) {
        process1();
        exit(0);
    } else {
        pid2 = fork();
        if (pid2 == 0) {
            process2();
            exit(0);
        }
    }

    pid3 = fork();

    if (pid3 == 0) {
        process3();
        exit(0);
    } else {
        pid4 = fork();
        if (pid4 == 0) {
            process4();
            exit(0);
        }
    }

    // Parent process waits for all child processes to complete
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    unlink(PIPE1);
    unlink(PIPE2);
    unlink(PIPE3);
    unlink(PIPE4);
    unlink(PIPE5);

    return 0;
}
