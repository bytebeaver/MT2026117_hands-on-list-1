/*
Program Number: 9
Student Name: Yashik Jain
Register Number: MT2026117
Date: 12-09-2026
Description: This program prints file information (inode, hard
             link count, UID, GID, size, block size, number of
             blocks, and access/modification/change times) for a
             given file, using the stat() system call.
*/

#include <stdio.h>      // WHAT: printf, fprintf, perror
                        // WHY: displaying values and error messages
#include <stdlib.h>     // WHAT: exit()
                        // WHY: stop the program with a status code on failure
#include <sys/stat.h>   // WHAT: struct stat and the stat() system call
                        // WHY: this header defines the struct's field
                        //      names (st_ino, st_nlink, etc.) that hold
                        //      all the file metadata we need
#include <time.h>       // WHAT: ctime() - converts a raw timestamp into a
                        // readable string like "Thu Sep 11 19:47:38 2026\n"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        // WHAT: we expect exactly one extra argument - the filename.
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    struct stat file_stat;
    /* WHAT: an empty "form" (struct) that stat() will fill in for us with
     *       every piece of metadata the kernel keeps about the file - none
     *       of this data lives inside the file's own bytes; it's stored
     *       separately, in the file's inode on disk. */

    if (stat(argv[1], &file_stat) < 0) {
        /* WHAT: ask the kernel to look up argv[1] and fill `file_stat`
         *       with its inode information.
         * WHY &file_stat (address-of): stat() needs to WRITE into our
         *       struct, so we pass a pointer to it rather than a copy -
         *       this is how a C function can "return" more than one value.
         * WHAT HAPPENS: the kernel locates the file's inode (the on-disk
         *       record holding its metadata, separate from its actual
         *       content blocks) and copies its fields into file_stat. */
        perror("stat");
        exit(1);
    }

    printf("File: %s\n", argv[1]);

    printf("Inode number: %ld\n", (long) file_stat.st_ino);
    /* WHAT: the inode number - a unique ID for this file within its
     *       filesystem (NOT the filename - a file can have several names/
     *       hard links but only one inode number). */

    printf("Number of hard links: %ld\n", (long) file_stat.st_nlink);
    /* WHAT: how many directory entries (filenames) point to this same
     *       inode. A brand-new file normally has 1. */

    printf("UID: %d\n", file_stat.st_uid);
    // WHAT: numeric user ID of the file's owner.

    printf("GID: %d\n", file_stat.st_gid);
    // WHAT: numeric group ID that owns the file.

    printf("Size: %ld bytes\n", (long) file_stat.st_size);
    // WHAT: total size of the file's contents, in bytes.

    printf("Block size: %ld bytes\n", (long) file_stat.st_blksize);
    /* WHAT: the preferred I/O block size for this filesystem - the chunk
     *       size the filesystem likes to read/write in at once (this is a
     *       filesystem property, not related to any BUF_SIZE in our own code). */

    printf("Number of blocks: %ld\n", (long) file_stat.st_blocks);
    /* WHAT: how many 512-byte blocks are actually allocated on disk for
     *       this file. Note: this can differ slightly from
     *       size / block_size due to how disk space is allocated. */

    printf("Last access time: %s", ctime(&file_stat.st_atime));
    /* WHAT: when the file's contents were last READ (st_atime = access time).
     * WHY ctime(&...): st_atime is stored as a raw number (seconds since
     *       1 Jan 1970, called "Unix epoch time") - ctime() converts that
     *       number into a human-readable date string, and importantly it
     *       already ends with '\n', which is why we use %s with no extra
     *       newline in the format string. */

    printf("Last modification time: %s", ctime(&file_stat.st_mtime));
    /* WHAT: when the file's CONTENTS were last changed (st_mtime).
     * WHY it differs from access time: reading a file updates atime;
     *       only writing to it updates mtime. */

    printf("Last status change time: %s", ctime(&file_stat.st_ctime));
    /* WHAT: when the file's INODE metadata (permissions, owner, link
     *       count, etc.) was last changed (st_ctime = "change" time, NOT
     *       "creation" time - Linux does not generally track creation time). */

    return 0;
}



/* SAMPLE OUTPUT


yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ mkdir prac-09
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ cd prac-09
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-09$ code .
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-09$ gcc 9.c -o 9
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-09$ echo "sample content" > file1.txt
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-09$ ./9 file1.txt
File: file1.txt
Inode number: 68260
Number of hard links: 1
UID: 1001
GID: 1001
Size: 15 bytes
Block size: 4096 bytes
Number of blocks: 8
Last access time: Fri Sep 11 20:59:42 2026
Last modification time: Fri Sep 11 20:59:42 2026
Last status change time: Fri Sep 11 20:59:42 2026
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-09$

*/