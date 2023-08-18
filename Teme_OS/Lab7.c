#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void write_file_list(int pipe_write, const char* dir_name) {
    DIR *dir = opendir(dir_name); // open the directory
    if (dir == NULL) {
        char* error = "Error. Directory has not been found.";
        write(pipe_write, error, strlen(error) + 1); // send the error message through the pipe
        return;
    }

    struct dirent* entry;   // store an address of type struct dirent
    char* file_list = malloc(BUFFER_SIZE); // dynamically allocate memory for the file list
    memset(file_list, 0, BUFFER_SIZE);      // initialize the file_list

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {  // include regular files in file list
            char* file_extension = strrchr(entry->d_name, '.');
            if (file_extension != NULL && strcmp(file_extension, ".txt") == 0) { // if the file has .txt extension
                if (strlen(file_list) + strlen(entry->d_name) + 2 < BUFFER_SIZE) { // check if there is enough space
                    strncat(file_list, entry->d_name, strlen(entry->d_name)); // append file name to file list
                    strcat(file_list, "\n");                                  // add a newline character
                }
            }
        } else if (entry->d_type == DT_DIR) { // directory found
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) { // '.' refers to current directory, '..' refers to parent directory
                char subdir_path[BUFFER_SIZE]; // create the full path of the subdirectory
                snprintf(subdir_path, BUFFER_SIZE, "%s/%s", dir_name, entry->d_name); // concatenate strings to create a full path for a subdirectory
                write_file_list(pipe_write, subdir_path); // recursively send file list for the subdirectory
            }
        }
    }

    closedir(dir);                           // close the directory
    write(pipe_write, file_list, strlen(file_list)); // send the file list through the pipe
    free(file_list);                                // deallocate the memory for the file list
}

int main() {
    int pipe1[2], pipe2[2]; // initialize the pipes
    pid_t pid; 

    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        perror("Error. Pipes couldn't be initialized");
        exit(EXIT_FAILURE);
    }

    pid = fork(); // fork the process to create a child process
    if (pid < 0) {
        perror("Error. Child process couldn't be forked");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // child process (Client)
        close(pipe1[0]); // close read end of pipe1 because child process only writes in the pipe
        close(pipe2[1]); // close write end of pipe2 because the child process only reads from the pipe

        char dir_name[BUFFER_SIZE];
        memset(dir_name, 0, BUFFER_SIZE); // initialize the dir_name
        read(pipe2[0], dir_name, BUFFER_SIZE); // read the directory name from the parent process

        write_file_list(pipe1[1], dir_name); // send file list or error message to parent process

        close(pipe1[1]); // close the write end of pipe1
        close(pipe2[0]); // close the read end of pipe2
        exit(EXIT_SUCCESS);
    } else {
        // parent process (Server)
        close(pipe1[1]); // close the write end of pipe1 because parent process only reads from the pipe
        close(pipe2[0]); // close the read end of pipe2 because the parent process only writes in the pipe

        char dir_name[BUFFER_SIZE];
        printf("Enter a directory name: ");   // get user input for the directory
        fgets(dir_name, BUFFER_SIZE, stdin);
        dir_name[strcspn(dir_name, "\n")] = '\0';         // remove the newline character

        write(pipe2[1], dir_name, strlen(dir_name) + 1);  // send the directory name to the child process

        char response[BUFFER_SIZE];
        memset(response, 0, BUFFER_SIZE); // initialize the response
        read(pipe1[0], response, BUFFER_SIZE); // read the file list or error message from the child process

        if (strlen(response) > 0)
            printf("File List:\n%s\n", response); // print the file list
        else
            printf("Error: %s\n", response); // print the error message

        close(pipe1[0]); // close read end of pipe1
        close(pipe2[1]); // close write end of pipe2
    }
    return 0;
}
