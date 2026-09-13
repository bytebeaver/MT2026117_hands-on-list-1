/*
Program Number: 11c
Student Name: Yashik Jain
Register Number: MT2026117
Date: 12-09-2026
Description: This program opens a file, duplicates its file
             descriptor using fcntl() with the F_DUPFD command,
             appends to the file using both the original and
             duplicated descriptor, and verifies the file was
             updated correctly by reading it back.
*/

#include <stdio.h>      // WHAT: printf, fprintf, perror
                        // WHY: displaying messages and errors
#include <stdlib.h>     // WHAT: exit()
                        // WHY: stop the program with a status code on failure
#include <fcntl.h>      // WHAT: open(), its flags, AND fcntl() + F_DUPFD
                        // WHY: fcntl() and its command constants are
                        //      declared in this same header as open()
#include <unistd.h>     // WHAT: write(), read(), close()
                        // WHY: these are also raw syscalls declared here
#include <string.h>     // WHAT: strlen()
                        // WHY: to know exactly how many bytes to write

#define MIN_FD 5
/* WHAT: the minimum fd number we're requiring the duplicate to have.
 * WHY:  this is the whole point of F_DUPFD - we get to set a FLOOR,
 *       unlike dup() (no control at all) or dup2() (exact number,
 *       forcibly reassigned). */

int main() {

    int fd1 = open("file_for_fcntl.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    /* WHAT: open/create the file, with O_APPEND so every write is
     *       forced to the current end of file first - same reasoning
     *       as parts (a) and (b). */

    if (fd1 < 0) {
        perror("open");
        exit(1);
    }

    int fd2 = fcntl(fd1, F_DUPFD, MIN_FD);
    /* WHAT: duplicate fd1 onto a NEW fd number that is guaranteed to be
     *       >= MIN_FD (5).
     * WHY F_DUPFD specifically: fcntl() is a multi-purpose syscall - its
     *       SECOND argument is a command telling it what to actually do.
     *       F_DUPFD is the command that means "duplicate this fd."
     * WHAT HAPPENS: the kernel searches upward starting at MIN_FD for the
     *       lowest fd number that is currently free, and makes THAT
     *       number point at fd1's open file description. Unlike dup2(),
     *       it will NEVER close an fd that's already in use - if a
     *       number is taken, it's simply skipped over. */

    if (fd2 < 0) {
        perror("fcntl F_DUPFD");
        close(fd1);
        exit(1);
    }

    printf("Original file descriptor (fd1): %d\n", fd1);
    printf("fcntl(F_DUPFD) gave us fd2: %d (guaranteed to be >= %d)\n", fd2, MIN_FD);

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

    /* ---- Write the second message through fd2 (the fcntl duplicate) ---- */
    written = write(fd2, msg2, strlen(msg2));
    /* WHAT: writing through fd2 lands right after "Hello ", for the same
     *       underlying reason as parts (a) and (b): fd1 and fd2 share
     *       one open file description and one shared position,
     *       reinforced by O_APPEND. */

    if (written != (ssize_t) strlen(msg2)) {
        perror("write via fd2");
        close(fd1);
        close(fd2);
        exit(1);
    }

    /* ---- Close both descriptors ---- */
    close(fd1);
    close(fd2);

    /* ---- Verify: reopen and read the file back ---- */
    int fd3 = open("file_for_fcntl.txt", O_RDONLY);
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
---------------------------------------------------------------------------------------------------
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$ ls
11a  11a.c  11b  11b.c  11c.c  file1.txt  file_for_dup2.txt
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$ gcc 11c.c -o 11c
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$ ./11c
Original file descriptor (fd1): 3
fcntl(F_DUPFD) gave us fd2: 5 (guaranteed to be >= 5)
File contents after both writes: Hello World
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$ cat file_for_fcntl.txt
Hello World
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-11$

*/

