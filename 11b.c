/*
Program Number: 11b
Student Name: Yashik Jain
Register Number: MT2026117
Date: 12-09-2026
Description: This program opens a file, duplicates its file
             descriptor onto a SPECIFIC chosen fd number using
             dup2(), appends to the file using both the original
             and duplicated descriptor, and verifies the file was
             updated correctly by reading it back.
*/

#include <stdio.h>      // WHAT: printf, fprintf, perror
                        // WHY: displaying messages and errors
#include <stdlib.h>     // WHAT: exit()
                        // WHY: stop the program with a status code on failure
#include <fcntl.h>      // WHAT: open() and its flags
                        // WHY: open() is a syscall, declared here
#include <unistd.h>     // WHAT: write(), read(), dup2(), close()
                        // WHY: these are also raw syscalls declared here
#include <string.h>     // WHAT: strlen()
                        // WHY: to know exactly how many bytes to write

#define TARGET_FD 10
/* WHAT: the exact fd number we want our duplicate to use.
 * WHY:  this is the whole point of dup2() over dup() - WE are choosing
 *       this number, not letting the kernel pick automatically. */

int main() {

    int fd1 = open("file_for_dup2.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    /* WHAT: same as part (a) - open/create the file, with O_APPEND so
     *       every write is forced to the current end of file first.
     * WHY a different filename from part (a): file1.txt already exists
     *       from the dup() practical - using a separate file here keeps
     *       each part's output independent and easy to verify on its own. */

    if (fd1 < 0) {
        perror("open");
        exit(1);
    }

    int result = dup2(fd1, TARGET_FD);
    /* WHAT: make file descriptor number TARGET_FD (10) point at the SAME
     *       open file description as fd1.
     * WHY two arguments instead of one (unlike dup()): dup2() lets us
     *       specify exactly which number to use, rather than accepting
     *       whatever the kernel's "lowest free number" happens to be.
     * WHAT HAPPENS: if fd number 10 was already open and pointing at
     *       something else, the kernel would silently close that first,
     *       THEN make 10 point at fd1's open file description. In this
     *       program, fd 10 wasn't open yet, so it's simply claimed fresh. */

    if (result < 0) {
        perror("dup2");
        close(fd1);
        exit(1);
    }

    printf("Original file descriptor (fd1): %d\n", fd1);
    printf("dup2() forced the duplicate onto fd: %d (we chose this number)\n", TARGET_FD);

    const char *msg1 = "Hello ";
    const char *msg2 = "World\n";

    ssize_t written;

    /* ---- Write the first message through fd1 ---- */
    written = write(fd1, msg1, strlen(msg1));
    if (written != (ssize_t) strlen(msg1)) {
        perror("write via fd1");
        close(fd1);
        close(TARGET_FD);
        exit(1);
    }

    /* ---- Write the second message through TARGET_FD (the dup2 duplicate) ---- */
    written = write(TARGET_FD, msg2, strlen(msg2));
    /* WHAT: writing through fd 10 - a number we chose ourselves - lands
     *       right after "Hello ", for the exact same reason as part (a):
     *       fd1 and TARGET_FD share one open file description and one
     *       shared position, reinforced by O_APPEND. */

    if (written != (ssize_t) strlen(msg2)) {
        perror("write via TARGET_FD");
        close(fd1);
        close(TARGET_FD);
        exit(1);
    }

    /* ---- Close both descriptors ---- */
    close(fd1);
    close(TARGET_FD);

    /* ---- Verify: reopen and read the file back ---- */
    int fd3 = open("file_for_dup2.txt", O_RDONLY);
    if (fd3 < 0) {
        perror("open for verification");
        exit(1);
    }

    char buffer[100];
    ssize_t n = read(fd3, buffer, sizeof(buffer) - 1);
    if (n < 0) {
        perror("read");
        close(fd3);
        exit(1);
    }
    buffer[n] = '\0';

    printf("File contents after both writes: %s", buffer);

    close(fd3);
    return 0;
}


/* SAMPLE OUTPUT
-------------------------------------------------------------------------------------------------
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$ ls
11a  11a.c  file1.txt
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$ code .
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$ gcc 11b.c -o 11b
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$ ./11b
Original file descriptor (fd1): 3
dup2() forced the duplicate onto fd: 10 (we chose this number)
File contents after both writes: Hello World
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$ cat file_for_dup2.txt
Hello World
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$ ls
11a  11a.c  11b  11b.c  file1.txt  file_for_dup2.txt
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$

----------------------------------------------------------------------------------------------------
*/

