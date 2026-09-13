/*
Program Number: 10
Student Name: Yashik Jain
Register Number: MT2026117
Date: 12-09-2026
Description: This program opens a file in read-write mode, writes
             10 bytes, moves the file pointer forward by 10 bytes
             using lseek(), and writes another 10 bytes, leaving a
             gap (hole) of unwritten bytes in between.
*/

#include <stdio.h>      // WHAT: printf, fprintf, perror
                        // WHY: displaying messages and errors
#include <stdlib.h>     // WHAT: exit()
                        // WHY: stop the program with a status code on failure
#include <fcntl.h>      // WHAT: open() and its flags
                        // WHY: open() is a syscall, declared here
#include <unistd.h>     // WHAT: write(), lseek(), close()
                        // WHY: these are also raw syscalls declared here

int main() {

    int fd = open("file1.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    /* WHAT: open (creating if needed) file1.txt in READ-WRITE mode.
     * WHY O_RDWR specifically: this practical both writes AND later could
     *       read the file, so unlike previous practicals (which were
     *       purely read-only or write-only), we need both permissions on
     *       the same fd.
     * WHY O_TRUNC: if this program is run more than once, we don't want
     *       leftover bytes from a previous run confusing the gap we're
     *       about to create. */

    if (fd < 0) {
        perror("open");
        exit(1);
    }

    ssize_t written;
    // WHAT: return value holder for write() - bytes actually written, or -1.

    /* ---- Step 1: write the first 10 bytes ---- */
    written = write(fd, "AAAAAAAAAA", 10);
    /* WHAT: write exactly 10 'A' characters starting at the current file
     *       position (which is 0, right after opening).
     * WHAT HAPPENS: the kernel writes these 10 bytes, and automatically
     *       advances the file's internal position to byte 10. */

    if (written != 10) {
        perror("write (first block)");
        close(fd);
        exit(1);
    }

    /* ---- Step 2: move the pointer forward by 10 MORE bytes ---- */
    off_t new_pos = lseek(fd, 10, SEEK_CUR);
    /* WHAT: move the file's read/write position forward by 10 bytes,
     *       relative to wherever it currently is (SEEK_CUR).
     * WHY this does NOT write anything: lseek() only updates an internal
     *       "where am I in this file" counter - it never touches disk
     *       data. Since the pointer is currently at byte 10 (right after
     *       the first write), it now becomes byte 20.
     * WHY check the return value: lseek() returns the NEW position on
     *       success, or -1 if the seek failed (e.g. on a file type that
     *       doesn't support seeking, like a pipe). The task specifically
     *       asks us to check this. */

    if (new_pos < 0) {
        perror("lseek");
        close(fd);
        exit(1);
    }

    printf("lseek moved the pointer to byte position: %ld\n", (long) new_pos);
    // WHAT: confirm exactly where we ended up - should print 20.

    /* ---- Step 3: write the second 10 bytes ---- */
    written = write(fd, "BBBBBBBBBB", 10);
    /* WHAT: write 10 'B' characters, but this time starting at byte 20,
     *       not byte 10 - because that's where lseek() just moved us.
     * WHAT HAPPENS: bytes 10 through 19 were NEVER written by this
     *       program. They become a "hole" - when read back, the
     *       filesystem reports them as zero-bytes, even though nothing
     *       was ever explicitly stored there. */

    if (written != 10) {
        perror("write (second block)");
        close(fd);
        exit(1);
    }

    close(fd);
    /* WHAT: release the file descriptor.
     * WHY:  same habit as every previous practical - always match open()
     *       with close(). */

    printf("Done. Inspect file1.txt with: od -c file1.txt\n");
    return 0;
}


/* SAMPLE OUTPUT
----------------------------------------------------------------------------------------------
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ cd prac-10
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-10$ code .
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-10$ gcc 10.c -o 10
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-10$ ./10
lseek moved the pointer to byte position: 20
Done. Inspect file1.txt with: od -c file1.txt
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-10$ od -c file1.txt
0000000   A   A   A   A   A   A   A   A   A   A  \0  \0  \0  \0  \0  \0
0000020  \0  \0  \0  \0   B   B   B   B   B   B   B   B   B   B
0000036
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-10$
-----------------------------------------------------------------------------------------------------
*/

