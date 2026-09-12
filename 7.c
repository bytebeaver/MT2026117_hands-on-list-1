/*
Program Number: 7
Student Name: yashik jain
Register Number: MT2026117
Date: 12-09-2026
Description: This program will implement cp command using file
             system related system calls.
*/

/*
 * Goal: copy the contents of file1 into file2, like `cp file1 file2`.
 *
 * We use low-level Linux SYSTEM CALLS (open, read, write, close) instead of
 * fopen/fread/fwrite/fclose. System calls talk directly to the kernel with
 * no extra buffering layer on top, which is exactly what `cp` itself does
 * under the hood.
 */

#include <stdio.h>      // WHAT: gives us printf, fprintf, perror
                        // WHY: we need to print usage messages and errors
#include <stdlib.h>     // WHAT: gives us exit()
                        // WHY: exit() lets us stop the program with a status code
#include <fcntl.h>      // WHAT: gives us open() and its flags (O_RDONLY, O_WRONLY, ...)
                        // WHY: open() is a system call, not a normal C function -
                        //      this header declares it so the compiler knows its signature
#include <unistd.h>     // WHAT: gives us read(), write(), close()
                        // WHY: these are also raw system calls declared here

#define BUF_SIZE 1024
/* WHAT: a compile-time constant, the size of our temporary "bucket" for copying.
 * WHY:  we can't read a whole file into memory in one shot safely (files can be huge),
 *       so we copy it in small chunks of 1024 bytes at a time. */

int main(int argc, char *argv[]) {
    /* WHAT: argc = argument count, argv = argument values (the words typed on
     *       the command line, e.g. ./7 file1.txt file2.txt -> argc = 3).
     * WHY:  we need the two filenames the user types after the program name. */

    if (argc != 3) {
        // WHAT: check that the user gave exactly 2 extra arguments (source, dest).
        // WHY:  argv[0] is always the program name itself, so we need argc == 3
        //       (program name + source + destination).
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argv[0]);
        exit(1);
        // WHAT: stop the program immediately with exit code 1 (non-zero = failure).
    }

    int src_fd, dst_fd;
    /* WHAT: file descriptors - small non-negative integers the kernel hands back
     *       to represent an "open file" (0, 1, 2 are already taken by
     *       stdin, stdout, stderr).
     * WHY:  every subsequent syscall (read/write/close) refers to the file
     *       using this integer, not the filename. */

    ssize_t bytes_read, bytes_written;
    // WHAT: signed size type, can hold a byte count OR -1 to signal an error.
    // WHY:  read()/write() return how many bytes they actually processed,
    //       or -1 if something failed.

    char buffer[BUF_SIZE];
    // WHAT: the actual "bucket" of memory we read into and write out of.

    /* ---- Step 1: open the source file for reading ---- */
    src_fd = open(argv[1], O_RDONLY);
    /* WHAT: ask the kernel to open argv[1] (source filename) in read-only mode.
     * WHY:  O_RDONLY means we only intend to read - the kernel will refuse
     *       any write attempt on this descriptor, protecting the source file.
     * WHAT HAPPENS: the kernel checks the file exists and you have permission,
     *       creates an entry in the process's open-file table, and returns
     *       a small integer (the fd) - or -1 on failure. */

    if (src_fd < 0) {
        perror("open source");
        // WHAT: perror prints your message PLUS the human-readable reason
        //       for the last failed syscall (using the global `errno`).
        exit(1);
    }

    /* ---- Step 2: open/create the destination file for writing ---- */
    dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    /* WHAT: open argv[2] for writing, with three flags OR'd together:
     *   O_WRONLY - write-only
     *   O_CREAT  - create the file if it doesn't already exist
     *   O_TRUNC  - if it DOES exist, erase its old contents first
     * WHY the extra 0644 argument: when O_CREAT is used, open() needs a
     *       permissions mode for the NEW file. 0644 = rw-r--r--. */

    if (dst_fd < 0) {
        perror("open destination");
        close(src_fd);
        // WHY close src_fd here: we already opened it, so release it
        // before exiting instead of leaking it.
        exit(1);
    }

    /* ---- Step 3: copy loop ---- */
    while ((bytes_read = read(src_fd, buffer, BUF_SIZE)) > 0) {
        /* WHAT: read() asks the kernel to copy up to BUF_SIZE bytes from the
         *       source file into our buffer.
         * WHY the while condition: read() returns 0 at end-of-file and a
         *       negative number on error - so "> 0" means "keep going". */

        bytes_written = write(dst_fd, buffer, bytes_read);
        /* WHAT: write() copies `bytes_read` bytes FROM our buffer INTO the
         *       destination file.
         * WHY bytes_read (not BUF_SIZE): the last chunk of the file is
         *       usually smaller than BUF_SIZE, so we only write what we
         *       actually read. */

        if (bytes_written != bytes_read) {
            perror("write");
            close(src_fd);
            close(dst_fd);
            exit(1);
        }
    }

    if (bytes_read < 0) {
        // WHAT: distinguishes "file fully copied" (loop ended via 0) from
        // "something went wrong while reading" (loop ended via -1).
        perror("read");
    }

    /* ---- Step 4: close both files ---- */
    close(src_fd);
    close(dst_fd);
    /* WHAT: tell the kernel we're done with these file descriptors.
     * WHY:  frees kernel resources, and for the destination file, ensures
     *       any buffered data is flushed so the copy is truly complete. */

    printf("File copied successfully.\n");
    return 0;
}



/*
============================================================================
Sample Output:
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-07$ gcc 7.c -o 7
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-07$ echo "hello from file1" > file1.txt
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-07$ ./7 file1.txt file2.txt
File copied successfully.
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-07$ cat file2.txt
hello from file1
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-07$ diff file1.txt file2.txt
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-07$
============================================================================
*/
