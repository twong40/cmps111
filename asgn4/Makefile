all: create fs

create: create_fs.c
	cc create_fs.c -o create_fs `pkgconf fuse --cflags --libs`
fs: hello.c
	cc hello.c -o fs `pkgconf fuse --cflags --libs`
clean:
	rm -f create_fs fs
clean_all:
	rm -f create_fs fs FS_FILE
