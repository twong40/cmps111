/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall hello.c `pkg-config fuse --cflags --libs` -o hello
*/

//links to understand FUSE
//http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/
//https://engineering.facile.it/blog/eng/write-filesystem-fuse/
//links to understand read/write to binary
//https://stackoverflow.com/questions/17598572/read-write-to-binary-files-in-c
//https://www.tutorialspoint.com/c_standard_library/c_function_fread.htm
//https://linux.die.net/man/3/fwrite
//https://overiq.com/c-programming-101/fwrite-function-in-c/
//link to hexdump
//https://www.geeksforgeeks.org/hexdump-command-in-linux-with-examples/
#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";

static int hello_getattr(const char *path, struct stat *stbuf)
{
	printf( "getattr called\n" );
	printf( "\tAttributes of %s requested\n", path );
	//set last access time and modification time to now
	stbuf->st_atime = time( NULL );
	stbuf->st_mtime  = time( NULL );
	printf("stat->st_mtime: %s\n",ctime(&stbuf->st_mtime));

	int res = 0;
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, hello_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(hello_str);
	} else
		res = -ENOENT;

	return res;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;
	printf( "readdir called\n" );
	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, hello_path + 1, NULL, 0);

	return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
		printf( "open called\n" );
	if (strcmp(path, hello_path) != 0)
		return -ENOENT;

	// if ((fi->flags & 3) != O_RDONLY)
	// 	return -EACCES;

	return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;
	printf( "read called\n" );
	if(strcmp(path, hello_path) != 0)
		return -ENOENT;

	len = strlen(hello_str);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, hello_str + offset, size);
	} else
		size = 0;

	return size;
}

static int hello_create(const char *path, mode_t mode, struct fuse_file_info *fi){
	printf( "create called\n" );
	if (open(path, fi->flags, mode) == -1){
		return -errno;
	}
	return 0;
}
static struct fuse_operations hello_oper = {
	.getattr	= hello_getattr,
	.readdir	= hello_readdir,
	.open		= hello_open,
	.read		= hello_read,
	.create = hello_create,
};

int find_block(){
	//open the file
	FILE *fp = fopen("FS_FILE", "r+");
	//go the superblock after magic number
	fseek(fp,16,SEEK_SET);
	//set up the array
	unsigned char bitmap[12];
	//copy the bitmap into a temp to search through
	fgets(bitmap,12,fp);
	//go through each char
	char byte;
	int found = 0;
	int free_block = 100;
	for(int i = 0; i < 12; i++){
		//for each, check where the first free block is
		byte = bitmap[i];
		//go through each bit to look for the first 0, starting at left most bit
		//example: byte = 11101100
		for(int j = 7; j >= 0; j--){
			//check if that bit shifted by j is 1
			if(!((byte >> j) & 0x01)){
				//if it is, then flip that bit to 1
				byte ^= 1 << j;
				//save that new byte into the bitmap
				bitmap[i] = byte;
				//indicate that we have found the space
				found = 1;
				//rememeber the free block
				//add 1 to i since we started at 0, 8 -j because we need to find which bit
				free_block = (i+1) * (8-j);
				//exit inner loop
				break;
			}
			//if it is not 1, then go to the next bit
		}
		//if we found the open space, then we can exit going through the bitmap
		if(found) break;
		//else go to the next byte
	}
	//write the new bitmap into the file
	//go the superblock after magic number
	fseek(fp,16,SEEK_SET);
	//write the new metadata
  fwrite(bitmap,sizeof(bitmap),1,fp);
	//close file
	fclose(fp);
	return free_block;
}
//go through all the used blocks and find the entry containging the metadata needed
char* find_block_data(const char *path){
	char meta[50];
	int offset;
	//open the file
	FILE *fp = fopen("FS_FILE", "r+");
	//go the superblock after magic number
	fseek(fp,16,SEEK_SET);
	//set up the array
	unsigned char bitmap[12];
	//copy the bitmap into a temp to search through
	fgets(bitmap,12,fp);
	//go through each char
	char byte;
	for(int i = 0; i < 12; i++){
		//for each, check where the first free block is
		byte = bitmap[i];
		//go through each bit to look for the first 0, starting at left most bit
		//example: byte = 11101100
		for(int j = 7; j >= 0; j--){
			//check if that bit shifted by j is 1
			if(!((byte >> j) & 0x01)){
				//if it is, then flip that bit to 1
				byte ^= 1 << j;
				//save that new byte into the bitmap
				bitmap[i] = byte;
				//go to that location and copy 50 bytes of metadata into meta;
				offset = (i+1) * (8-j);
				//go to the used block
				fseek(fp,(offset*1024*4),SEEK_SET);
				fgets(meta,50,fp);
				//check if this is the correct file and return if it is
				//I DONT KNOW WHAT ELSE TO ADD HERE???
				//exit inner loop
				break;
			}
			//if it is not 1, then go to the next bit
		}
		//else go to the next byte
	}
	//close file
	fclose(fp);
	return strdup(meta);
}

int main(int argc, char *argv[])
{
	printf("AOFS started on new directory\n");
	return fuse_main(argc, argv, &hello_oper, NULL);
}
