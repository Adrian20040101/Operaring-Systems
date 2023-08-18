#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
   float price;
   pid_t pid;
   int pipe_client[2];
   int pipe_server[2];;

   // verifying if there were errors initializing the pipes
   if (pipe(pipe_client) < 0 || pipe(pipe_server) < 0){
      perror ("Couldn't initialize the pipes!");
      exit(EXIT_FAILURE);
   }

   // getting the price input
   printf("Enter the price: ");
   if (scanf("%f", &price) != true || price <= 0){
      printf("Invalid input!\n");
      exit(EXIT_FAILURE);
   }

   // verifying if there were errors forking the child process
   pid = fork();
   if (pid < 0){
      perror("Couldn't fork the child process!");
      exit(EXIT_FAILURE);
   // child process
   } else if(pid == 0){
      // Close the read end of the client pipe and the write end of the server pipe
      close(pipe_client[0]);
      close(pipe_server[1]);
      write(pipe_client[1], &price, sizeof(price));
      float new_price;
      // read the information processed by the server
      read(pipe_server[0], &new_price, sizeof(new_price));
      printf("The total cost: %.2f Lei\n", new_price);
   } else {
      // parent process
      // Close the read end of the server pipe and the read end of the client pipe
      close(pipe_client[1]);
      close(pipe_server[0]);
      // read the information that has been transmitted from the child process
      read(pipe_client[0], &price, sizeof(price));
      float new_price;
      // process the total price
      new_price = price * 1.1;
      //send it back to the child process
      write(pipe_server[1], &new_price, sizeof(new_price));
      close(pipe_client[0]);
      close(pipe_server[1]);
      wait(NULL);
   }
   return 0;
}
