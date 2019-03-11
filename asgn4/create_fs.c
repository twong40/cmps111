#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/stat.h>

int main(int argc, char *argv[])
{
  int  file_descriptor;
	struct stat st, buffer;
	//check if file exists first
	if (stat("FS_FILE",&buffer) == 0){
    printf("File already exists\n");
		//read the file and produce the Filesystem
		unsigned char buffer[96];
		unsigned char new_buffer[96] = "hello fren";
		FILE *ptr;
		ptr = fopen("FS_FILE","rb+");  // r for read, b for binary, + for write
		//puts the pointer after 4 bytes after magic number
		fseek(ptr, 4, SEEK_SET);
		/* Write data to the file */
	  fwrite(new_buffer, sizeof(new_buffer), 1, ptr);
		/* Seek to the beginning of the file */
		fseek(ptr, 4, SEEK_SET);
		fread(buffer,sizeof(buffer),1,ptr); // read 96 bytes to our buffer
		for(int i = 0; i<96; i++)
			printf("%u ", buffer[i]); // prints a series of bytes
		printf("\n");
		fclose(ptr);
		}
		//else make the file and ftruncate 400000 bytes, first 96 bytes are metadata
		//place magic number in the first part of metadata  0xfa19283e = 4195952702
	else{
		file_descriptor = open("FS_FILE", O_RDWR| O_CREAT, S_IRWXU);
		if (file_descriptor < 0)
	    perror("Error making new file");
		if (ftruncate(file_descriptor, 400000) != 0)
        perror("ftruncate() error");
    else {
      fstat(file_descriptor, &st);
      printf("the file has %ld bytes\n", (long) st.st_size);
			//adds the magic number
			int magic[4] = {0xfa19283e};
			unsigned char test[4];
			FILE *fp;
			fp = fopen("FS_FILE","rb+");  // r for read, b for binary, + for write
			/* Write data to the file */
		  fwrite(magic, sizeof(magic), 1, fp);
			/* Seek to the beginning of the file */
			fseek(fp, 0, SEEK_SET);
			fread(test,sizeof(test),1,fp); // read 4 bytes to our test
			printf("Check for magic number ");
			for(int i = 0; i<4; i++)
				printf("%u ", test[i]); // prints a series of  4 bytes
			printf("\n");
			fclose(fp);
    }
	}
  return 0;
}
