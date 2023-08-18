#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
   if (argc < 3) {
      printf("Usage: %s <file_name> <word_to_delete>\n", argv[0]);
      return 1;
   }

   FILE* input_fp;
   FILE* output_fp;
   char buffer[100];
   char* token;
   int deleteCount = 0;

   input_fp = fopen(argv[1], "r");
   if (input_fp == NULL) {
      printf("Error opening file.\n");
      return 1;
   }

   output_fp = fopen("temp_file.txt", "w");
   if (output_fp == NULL) {
      printf("Error opening file.\n");
      fclose(input_fp);
      return 1;
   }

   while (fgets(buffer, 100, input_fp)) {
      token = strtok(buffer, " \n");
      int deleted = 0;

      //loop through the tokens and write to the temporary file
      while (token != NULL) {
         if (strcmp(token, argv[2]) != 0) {
            fputs(token, output_fp);
            fputc(' ', output_fp);
         }
         else{
	    deleted++;
         }
         token = strtok(NULL, " \n");
      }
      fputc('\n', output_fp);
      deleteCount = deleteCount + deleted;
   }

   fclose(input_fp);
   fclose(output_fp);

   remove(argv[1]);
   rename("temp_file.txt", argv[1]);

   if (deleteCount == 0){
      printf("The word %s has not been found in the file.\n", argv[2]);
   }
   else{
      printf("The word %s has been deleted %d times.\n", argv[2], deleteCount);
   }

   return 0;
}
