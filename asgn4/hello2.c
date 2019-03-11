#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/stat.h>

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";
FILE * fd;


           // struct stat {
           //     dev_t     st_dev;         /* ID of device containing file */
           //     ino_t     st_ino;         /* Inode number */
           //     mode_t    st_mode;        /* File type and mode */
           //     nlink_t   st_nlink;       /* Number of hard links */
           //     uid_t     st_uid;         /* User ID of owner */
           //     gid_t     st_gid;         /* Group ID of owner */
           //     dev_t     st_rdev;        /* Device ID (if special file) */
           //     off_t     st_size;        /* Total size, in bytes */
           //     blksize_t st_blksize;     /* Block size for filesystem I/O */
           //     blkcnt_t  st_blocks;      /* Number of 512B blocks allocated */

           //     /* Since Linux 2.6, the kernel supports nanosecond
           //        precision for the following timestamp fields.
           //        For the details before Linux 2.6, see NOTES. */

           //     struct timespec st_atim;  /* Time of last access */
           //     struct timespec st_mtim;  /* Time of last modification */
           //     struct timespec st_ctim;  /* Time of last status change */

           // #define st_atime st_atim.tv_sec      /* Backward compatibility */
           // #define st_mtime st_mtim.tv_sec
           // #define st_ctime st_ctim.tv_sec
           // };

static int hello_getattr(const char *path, struct stat *stbuf,  struct fuse_file_info *fi)
{


        printf("hello_getattr\n");
        printf("path: %s\n",path);
        printf("stat->st_mtime: %s\n",ctime(&stbuf->st_mtime));
        printf("fi->writepage : %i\n", fi->writepage);
        int res = 0;

        memset(stbuf, 0, sizeof(struct stat));
        if (strcmp(path, "/") == 0) {
            printf("print things in directory\n");
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
        } else if (strcmp(path, hello_path) == 0) { //if path == hello_path
                printf("read file\n");
                stbuf->st_mode = S_IFREG | 0444;
                stbuf->st_nlink = 1;
                stbuf->st_size = strlen(hello_str);
        } else{
            //create file
            printf("else condition \n");        
            res = -ENOENT;
        }
        //I'm assuming we return 0 if res exists 
        return res;
}


static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi)
{
        printf("hello_readdir\n");
        (void) offset;
        (void) fi;
        if (strcmp(path, "/") != 0)
                return -ENOENT;
        filler(buf, ".", NULL, 0);
        filler(buf, "..", NULL, 0);
        filler(buf, hello_path + 1, NULL, 0);
        return 0;
}

//Passsed in file info and a path, so it should probably the file info
static int hello_open(const char *path, struct fuse_file_info *fi)
{
        printf("hello_open\n");

        if (strcmp(path, hello_path) != 0)
                return -ENOENT;
        if ((fi->flags & 3) != O_RDONLY)
                return -EACCES;
        return 0;
}


//I think it reads the file?
static int hello_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{        
        //buf is what we put in to display current directory
        printf("hello_read\n");
        printf("path : %s\n", path);
        size_t len;
        (void) fi;
        if(strcmp(path, hello_path) != 0)
        {
        printf("file dont exist?\n");
        return -ENOENT;
        }
        len = strlen(hello_str);
        if (offset < len) {
                if (offset + size > len)
                        size = len - offset;
                memcpy(buf, hello_str + offset, size);
        } else
                size = 0;
        return size;
}


static void *hello_init(struct fuse_conn_info *conn, struct fuse_config *cfg)
{

    printf("Hello initialized\n");

    return NULL;
}

static int hello_write(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi){
    printf("hello_write\n");
    fclose(fd);
    return pwrite(fi->fh, buf, size, offset);
}
    
int allocate_space(){
    
    FILE *fp = fopen("FS_FILE", "r+");
    int max = 10, bitmap_starting_location = 0,  i, j = 0, counter = 0, flag = 0;;
    char arr[max];
    fseek(fp, bitmap_starting_location, SEEK_SET);
    fgets(arr, max, fp);
    printf("%s\n",arr);
    char byte;
    for(j = 0; j < max ; j++)
    {
        byte = arr[j];
        for(i = 7; 0 <= i; i--)
        {
            ++counter;
            if((byte >> i) & 0x01)
            {
            }
            else
            {
                byte = byte ^ (1 << i);
                arr[j] = byte;
                flag = 1;
                break;
            }
        }
        if(flag)
        {
            break;
        }
    }

    fseek(fp, bitmap_starting_location, SEEK_SET);
    fputs(arr,fp);
    fclose(fp);
    printf("%s\n",arr);
    printf(" place: %i \n", counter);
    if(flag)
        return counter;
    return -1;

}

static int hello_create(const char* name, mode_t mode, struct fuse_file_info *fi)
{
    printf("hello_create\n");
    allocate_space();

    return 0;
}
//I believe these are initialized on start
static struct fuse_operations hello_oper = {
        .init           = hello_init,
        .getattr        = hello_getattr,
        .readdir        = hello_readdir,
        .open           = hello_open,
        .read           = hello_read,
        .write          = hello_write, 
        .create         = hello_create,
};
int main(int argc, char *argv[])
{
    printf("Creating Hello Fuse\n");
    // fd = open("FS_FILE", O_APPEND |O_RDWR | O_CREAT, 0777);

    return fuse_main(argc, argv, &hello_oper, NULL);
}