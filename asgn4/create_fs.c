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
		}
		//else make the file and ftruncate 400000 bytes, first 96 bytes are metadata
		//place magic number in the first part of metadata  0xfa19283e = 4195952702
	else{
		file_descriptor = open("FS_FILE", O_RDWR| O_CREAT, S_IRWXU);
		if (file_descriptor < 0)
	    perror("Error making new file");
		if (ftruncate(file_descriptor, 409600) != 0)
        perror("ftruncate() error");
    else {
      fstat(file_descriptor, &st);
      printf("the file has %ld bytes\n", (long) st.st_size);
      close(file_descriptor);
			//adds the magic number, backwards to make hexdump print correctly?
      int magic[4] = {0xfa19283e};
      //create bitmap of unsigned chars with 12 entries, 99/8 = 12, each entry has 8 bits
      unsigned char bitmap[99];
      memset(bitmap, 0, sizeof(bitmap));
			FILE *fp;
			fp = fopen("FS_FILE","r+");  // create a file that can be read and written
			/* Write data to the file */
      //go to the start of FS_FILE
      fseek(fp, 0, SEEK_SET);
      //write the magic number
	    fwrite(magic, sizeof(magic), 1, fp);
      fwrite(bitmap,sizeof(bitmap),1,fp);
      //close the file
			fclose(fp);
    }
	}
  return 0;
}
