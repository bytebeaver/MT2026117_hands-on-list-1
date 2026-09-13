/*
Program Number: 11
Student Name: Yashik Jain
Register Number: MT2026117
Date: 12-09-2026
Description: This program opens a file, duplicates its file
             descriptor using dup(), appends to the file using
             both the original and duplicated descriptor, and
             verifies the file was updated correctly by reading
             it back.
*/

#include <stdio.h>      // WHAT: printf, fprintf, perror
                        // WHY: displaying messages and errors
#include <stdlib.h>     // WHAT: exit()
                        // WHY: stop the program with a status code on failure
#include <fcntl.h>      // WHAT: open() and its flags
                        // WHY: open() is a syscall, declared here
#include <unistd.h>     // WHAT: write(), read(), dup(), close()
                        // WHY: these are also raw syscalls declared here
#include <string.h>     // WHAT: strlen()
                        // WHY: to know exactly how many bytes to write

int main() {

    int fd1 = open("file1.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    /* WHAT: open (creating if needed) file1.txt for writing.
     * WHY O_APPEND: every write through this fd (or any fd sharing its
     *       open file description) automatically jumps to the current
     *       end of the file first - this is what makes "appending"
     *       through two different descriptors safe and predictable. */

    if (fd1 < 0) {
        perror("open");
        exit(1);
    }

    int fd2 = dup(fd1);
    /* WHAT: ask the kernel for a brand-new file descriptor number that
     *       points at the SAME open file description as fd1.
     * WHY: this is the entire point of the practical - fd2 is a second
     *       "remote control" for the exact same underlying connection,
     *       not a second independent connection to the file.
     * WHAT HAPPENS: the kernel picks the lowest currently unused fd
     *       number (commonly fd1 + 1, if nothing else is open) and makes
     *       it reference the same open file description as fd1 - same
     *       current position, same access mode. */

    if (fd2 < 0) {
        perror("dup");
        close(fd1);
        exit(1);
    }

    printf("Original file descriptor (fd1): %d\n", fd1);
    printf("Duplicated file descriptor (fd2): %d\n", fd2);

    const char *msg1 = "Hello ";
    const char *msg2 = "World\n";

    ssize_t written;

    /* ---- Write the first message through fd1 ---- */
    written = write(fd1, msg1, strlen(msg1));
    if (written != (ssize_t) strlen(msg1)) {
        perror("write via fd1");
        close(fd1);
        close(fd2);
        exit(1);
    }

    /* ---- Write the second message through fd2 (the DUPLICATE) ---- */
    written = write(fd2, msg2, strlen(msg2));
    /* WHAT: this write happens through fd2, a completely different
     *       integer than fd1 - yet because fd2 shares fd1's open file
     *       description (and O_APPEND is set), this text lands right
     *       after "Hello ", not overwriting it and not landing at some
     *       unrelated position. */

    if (written != (ssize_t) strlen(msg2)) {
        perror("write via fd2");
        close(fd1);
        close(fd2);
        exit(1);
    }

    /* ---- Close both descriptors ---- */
    close(fd1);
    close(fd2);
    /* WHAT: release both fds.
     * WHY closing fd1 first is safe: fd1 and fd2 are independent entries
     *       in the descriptor table, even though they share one open
     *       file description underneath - closing one never invalidates
     *       the other. The open file description itself is only fully
     *       freed once BOTH are closed. */

    /* ---- Verify: reopen and read the file back ---- */
    int fd3 = open("file1.txt", O_RDONLY);
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
    // WHAT: manually null-terminate, since read() does NOT do this for us -
    //       it only tells us how many raw bytes it read.

    printf("File contents after both writes: %s", buffer);

    close(fd3);
    return 0;
}


/*sample output 
------------------------------------------------------------------------------------------------------
yashik@LAPTOP-UV7S5VGB:~/linux-learning$ cd 02-HOL1
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ mkdir prac-11
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ cd prac-11
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$ code .
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$ gcc 11a.c -o 11a
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$ ./11a
Original file descriptor (fd1): 3
Duplicated file descriptor (fd2): 4
File contents after both writes: Hello World
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$ cat file1.txt
Hello World
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$

---------------------------------------------------------------------------------------------------------------
*/