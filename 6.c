/*
 * Program Number : 6
 * Student Name    : Yashik Jain
 * Register Number : MT2026117
 * Date            : 11-09-2026

 * Description     : Reads input from STDIN and writes it back to STDOUT
 *                    using only the raw read() and write() system calls,
 *                    with no C standard library buffered I/O (no printf,
 *                    scanf, or stdio formatting) anywhere in the data path.
 */

#include <unistd.h>     // read() and write()
#include <stdio.h>      // perror()

int main()
{
    /*
     * ============================================================
     * PRACTICAL 6
     * Input and Output Using System Calls
     * ============================================================
     *
     * Objective:
     *
     * Take input from STDIN and display it on STDOUT
     * using ONLY:
     *
     *      read()
     *      write()
     *
     *
     * Standard File Descriptors:
     *
     *      0 -> STDIN
     *      1 -> STDOUT
     *      2 -> STDERR
     *
     * read():
     *      Reads data from a file descriptor into a buffer.
     *
     * write():
     *      Writes data from a buffer to a file descriptor.
     */

    char buffer[100];

    /*
     * buffer is an array of characters where the input
     * received from STDIN will be temporarily stored.
     *
     * We have allocated space for 100 characters.
     */


    /*
     * ============================================================
     * READ INPUT FROM STDIN
     * ============================================================
     *
     * File descriptor:
     *
     *      0 -> STDIN
     *
     * We ask read() to:
     *
     *      read from STDIN
     *      store the data in buffer
     *      read at most 99 bytes
     *
     * We use 99 instead of 100 so that there is room if we
     * later want to add a '\0' character.
     */

    ssize_t bytes_read;

    bytes_read = read(0, buffer, 99);


    /*
     * Check whether read() was successful.
     *
     * read() returns:
     *
     *      > 0  -> number of bytes actually read
     *       0   -> end of input
     *      -1   -> error
     */

    if (bytes_read == -1)
    {
        perror("read");
        return 1;
    }


    /*
     * ============================================================
     * WRITE OUTPUT TO STDOUT
     * ============================================================
     *
     * File descriptor:
     *
     *      1 -> STDOUT
     *
     * We write exactly 'bytes_read' bytes.
     *
     * This is important because read() may not fill the
     * entire buffer.
     */

    ssize_t bytes_written;

    bytes_written = write(1, buffer, bytes_read);


    /*
     * Check whether write() was successful.
     */

    if (bytes_written == -1)
    {
        perror("write");
        return 1;
    }


    /*
     * Program completed successfully.
     */

    return 0;
}

/*SAMPLE O/P:

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-06$ gcc 6.c -o 6

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-06$ echo "hello world" | ./6
hello world

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-06$ printf "no newline test" | ./6

no newline testyashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-06$ 

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-06$  ./6 < /dev/null

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-06$ echo "exit code: $?"
exit code: 0

