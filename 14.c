/*
Program Number: 14
Student Name: Yashik Jain
Register Number: MT2026117
Date: 12-09-2026
Description: This program takes a filename from the command line and
             identifies its file type (regular file, directory, symbolic
             link, character device, block device, FIFO, or socket)
             using the lstat() system call and the S_IS* macros.
*/

#include <sys/stat.h>   // WHAT: struct stat, lstat(), and all S_IS*/S_IF* macros
#include <stdio.h>      // WHAT: printf, fprintf, perror

int main(int argc, char *argv[]) {
    struct stat st;
    // WHAT: an empty "form" that lstat() will fill in with the target's
    //       inode metadata - same struct used in practical 9's stat().

    if (argc != 2) {
        // WHAT: the practical requires taking the filename from the
        //       command line, so exactly one extra argument is required.
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // Use lstat to get file info
    if (lstat(argv[1], &st) < 0) {
        /* WHAT: ask the kernel for argv[1]'s metadata WITHOUT following
         *       it if it happens to be a symbolic link.
         * WHY lstat() and not stat(): stat() would silently chase a
         *       symlink through to whatever it points at, and report
         *       THAT target's type instead - meaning S_ISLNK could never
         *       come back true. lstat() stops at the link itself, which
         *       is exactly what "identify the file type" requires when
         *       the file in question might itself be a link. */
        perror("lstat");
        return 1;
    }

    printf("File type of '%s': ", argv[1]);

    if (S_ISREG(st.st_mode))
        printf("Regular File\n");
        // WHAT: an ordinary file containing data - the most common type.
    else if (S_ISDIR(st.st_mode))
        printf("Directory\n");
        // WHAT: a special file that lists other files/directories inside it.
    else if (S_ISLNK(st.st_mode))
        printf("Symbolic Link\n");
        // WHAT: a file whose only content is a path to ANOTHER file -
        //       only detectable at all because lstat() was used above.
    else if (S_ISCHR(st.st_mode))
        printf("Character Device\n");
        // WHAT: a device handled one character/byte at a time, e.g. /dev/null.
    else if (S_ISBLK(st.st_mode))
        printf("Block Device\n");
        // WHAT: a device handled in fixed-size blocks, e.g. a disk partition.
    else if (S_ISFIFO(st.st_mode))
        printf("FIFO (Pipe)\n");
        // WHAT: a named pipe - data written by one process, read by another,
        //       in the order it was written, with no disk storage involved.
    else if (S_ISSOCK(st.st_mode))
        printf("Socket\n");
        // WHAT: an endpoint for two-way communication between processes.
    else
        printf("Unknown\n");
        // WHAT: a safety net - every real file on Linux matches one of the
        //       seven types above, so this branch should never trigger.

    return 0;
}




/* SA<PLE OUTPUT
--------------------------------------------------------------------------------------------------

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ mkdir prac-14
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ cd prac-14
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-14$ code .
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-14$ gcc -Wall -o 14 14.c
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-14$ echo "hello" > regular.txt
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-14$ mkdir testdir
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-14$ ln -s regular.txt mylink
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-14$ mkfifo myfifo
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-14$ ./14 regular.txt
File type of 'regular.txt': Regular File
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-14$ ./14 testdir
File type of 'testdir': Directory
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-14$ ./14 mylink
File type of 'mylink': Symbolic Link
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-14$ ./14 /dev/null
File type of '/dev/null': Character Device
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-14$ ./14 myfifo
File type of 'myfifo': FIFO (Pipe)
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-14$ ./14 /dev/loop0
File type of '/dev/loop0': Block Device
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-14$ ./14 mysocket
lstat: No such file or directory
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-14$ ./14
Usage: ./14 <filename>
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-14$

--------------------------------------------------------------------------------------------------
*/
