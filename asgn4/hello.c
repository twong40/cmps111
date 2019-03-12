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

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, hello_path + 1, NULL, 0);

	return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
	if (strcmp(path, hello_path) != 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;
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

static struct fuse_operations hello_oper = {
	.getattr	= hello_getattr,
	.readdir	= hello_readdir,
	.open		= hello_open,
	.read		= hello_read,
};

int main(int argc, char *argv[])
{
	printf("AOFS started on new directory\n");
	return fuse_main(argc, argv, &hello_oper, NULL);
}
