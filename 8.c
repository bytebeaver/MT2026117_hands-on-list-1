/*
Program Number: 8
Student Name: Yashik Jain
Register Number: MT2026117
Date: 12-09-2026
Description: This program opens a file in read-only mode and reads
             it line by line using system calls, displaying each
             line as it is read, and closes the file at EOF.
*/

#include <stdio.h>      // WHAT: printf, fprintf, perror
                        // WHY: displaying lines and error messages
#include <stdlib.h>     // WHAT: exit()
                        // WHY: stop the program with a status code on failure
#include <fcntl.h>      // WHAT: open() and O_RDONLY
                        // WHY: open() is a syscall, declared here
#include <unistd.h>     // WHAT: read(), close()
                        // WHY: these are also raw syscalls declared here

#define LINE_SIZE 256
/* WHAT: max characters allowed in a single line (including the
 *       terminating '\0').
 * WHY:  a fixed-size buffer keeps this beginner-friendly; a production
 *       program would grow the buffer dynamically instead. */

int main(int argc, char *argv[]) {

    if (argc != 2) {
        // WHAT: we expect exactly one extra argument - the filename.
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    /* WHAT: open the file in read-only mode.
     * WHY O_RDONLY specifically: it documents intent - this program never
     *       modifies the file - and the kernel rejects any accidental
     *       write attempt on this fd. */

    if (fd < 0) {
        perror("open");
        exit(1);
    }

    char line[LINE_SIZE];
    // WHAT: buffer that holds ONE line at a time while we build it up.

    int idx = 0;
    // WHAT: how many characters of the current line collected so far -
    //       also the next free slot in `line`.

    char ch;
    // WHAT: one single character read from the file.

    ssize_t n;
    // WHAT: return value of read() - bytes actually read, or -1 on error.

    /* ---- Read the file ONE BYTE AT A TIME using the read() system call ---- */
    while ((n = read(fd, &ch, 1)) > 0) {
        /* WHAT: ask the kernel for exactly 1 byte at the file's current
         *       read position, store it in `ch`.
         * WHY one byte at a time: read() has no concept of "lines" - it
         *       only knows raw bytes. To detect line boundaries ourselves,
         *       we must inspect each byte and look for '\n'.
         * WHAT HAPPENS: kernel copies 1 byte into &ch, advances the file
         *       position by 1, returns 1 (success) - or 0 at end-of-file,
         *       or -1 on error. */

        if (ch == '\n' || idx == LINE_SIZE - 1) {
            // WHAT: either a real newline, OR the buffer is full (safety
            //       net so an unusually long line can't overflow `line`).
            line[idx] = '\0';
            // WHAT: terminate the C string so printf/%s knows where it ends.
            printf("%s\n", line);
            // WHAT: display the completed line immediately, as required.
            idx = 0;
            // WHAT: reset the buffer position to start the next line.
        } else {
            line[idx++] = ch;
            // WHAT: not a newline yet - store this character, move on.
        }
    }

    /* ---- Handle a file that doesn't end with a trailing newline ---- */
    if (idx > 0) {
        // WHAT: loop ended (EOF) but characters are still sitting in
        //       `line`, never terminated by '\n' - print that last
        //       partial line too.
        line[idx] = '\0';
        printf("%s\n", line);
    }

    if (n < 0) {
        // WHAT: distinguishes "reached end-of-file cleanly" (n == 0)
        //       from "read() actually failed" (n == -1).
        perror("read");
    }

    close(fd);
    /* WHAT: release the file descriptor back to the kernel.
     * WHY:  every open() should be matched with a close() - leaving files
     *       open wastes a limited kernel resource. */

    return 0;
}

/*
============================================================================
Sample Output:
----------------------------------------------------------------------------
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ mkdir prac-08
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ cd prac-08
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-08$ code .
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-08$ gcc 8.c -o 8
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-08$ ./8 file1.txt
open: No such file or directory
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-08$ {
  echo "hello world"
  printf 'A%.0s' {1..300}
  echo
  printf "no trailing newline here"
} > file1.txt
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-08$ ./8 file1.txt
hello world
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
no trailing newline here
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-08$

============================================================================
*/

