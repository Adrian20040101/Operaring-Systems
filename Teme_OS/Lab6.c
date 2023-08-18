#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

#define RANGE 52
#define PROCESSES 3
#define MAX_SIZE 1000

void count_frequency(char *str, int len, int start, int end, int *freq) {
    int i;
    for (i=0; i<len; i++) {
        char ch = str[i];
        if((ch>='a'+start && ch<='a'+end)) {
            freq[ch-'a']++;
        }
	if (ch>='A'+start && ch<='A'+end){
	    freq[ch-'A']++;
	}
    }
}

int main() {
    char str[MAX_SIZE];
    int freq[RANGE] = {0};
    int fd[PROCESSES][2];
    int i;

    FILE* file = fopen("helper.txt", "r");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return 1;
    }

    int len = fread(str, sizeof(char), MAX_SIZE, file);
    fclose(file);

    // create a new pipe for each process and store it into the fd array
    for (i = 0; i < PROCESSES; i++) {
        if (pipe(fd[i]) == -1) {
            printf("Failed to create pipe.\n");
            return 1;
        }
    }

    for (i = 0; i < PROCESSES; i++) {
        pid_t pid = fork();

	// child process
        if (pid == 0) {
            close(fd[i][0]);
	    // start index of the char range to be processed, respectively end index
            count_frequency(str, len, i*RANGE/PROCESSES, (i+1)*RANGE/PROCESSES, freq);
            write(fd[i][1], freq, sizeof(freq));
            close(fd[i][1]);
            return 0;
        } else if (pid < 0) {
            printf("Failed to fork.\n");
            return 1;
        } else {
	    // close the write end in the parent process
            close(fd[i][1]);
        }
    }

    for (i = 0; i < PROCESSES; i++) {
	// for each process, read the partial frequency that each child has done
        int partial_freq[RANGE] = {0};
        read(fd[i][0], partial_freq, sizeof(partial_freq));
        close(fd[i][0]);

	// store it to the original frequency
        for (int j = 0; j < RANGE; j++) {
            freq[j] += partial_freq[j];
        }
    }

    // print the character out
    for (i = 0; i < RANGE; i++) {
        if (freq[i] != 0) {
	   if(i < 26){
            printf("'%c' = %d\n", (i + 'a'), freq[i]);
	   }
	  // else{
	  //  printf("'%c' = %d\n", (i - 26 + 'A'), freq[i]);
	  // }
        }
    }
    return 0;
}
