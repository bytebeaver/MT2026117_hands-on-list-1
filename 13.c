/*
Program Number: 13
Student Name: Yashik Jain
Register Number: MT2026117
Date: 12-09-26
Description: This program waits for input on STDIN for up to 10 seconds
             using the select() system call, printing whether data
             became available in time or the wait timed out.
*/

#include <stdio.h>          // WHAT: printf, perror
#include <unistd.h>         // WHAT: read(), STDIN_FILENO
#include <sys/select.h>     // WHAT: select(), fd_set, struct timeval, and
                            //       the FD_ZERO/FD_SET/FD_ISSET macros

int main() {
    fd_set readfds;
    // WHAT: a SET of file descriptors we want select() to watch for
    //       readability - not a single fd, a collection (here it will
    //       end up containing just one: STDIN).

    struct timeval timeout;
    /* WHAT: the maximum time to wait, split into whole seconds (tv_sec)
     *       and additional microseconds (tv_usec).
     * WHY select() mutates this on Linux: after select() returns, this
     *       struct may hold how much time was LEFT, not the original
     *       value - so it should be reset before every new select() call
     *       if you plan to reuse it in a loop (this program only calls
     *       select() once, so that doesn't bite us here). */

    int ret;
    // WHAT: select()'s return value - -1 (error), 0 (timeout), or a
    //       positive count of how many fds in the set are now ready.

    char buffer[100];
    // WHAT: where we'll store whatever bytes read() pulls in from STDIN.

    // Initialize fd set
    FD_ZERO(&readfds);
    /* WHAT: clear every bit in readfds - fd_sets are NOT guaranteed to
     *       start empty just because they're declared, so this is
     *       mandatory before use, not optional cleanup. */

    FD_SET(STDIN_FILENO, &readfds);
    /* WHAT: add STDIN's fd number (0) to the set - this is the fd we're
     *       telling select() to watch. If we wanted to also watch a
     *       socket fd, we'd call FD_SET on that fd too, right here. */

    // Set timeout to 10 seconds
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    printf("You have 10 seconds to type something:\n");

    // Wait for input
    ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);
    /* WHAT: block until EITHER (a) STDIN becomes readable, or (b) 10
     *       seconds pass, whichever happens first.
     * WHY STDIN_FILENO + 1 as the first argument: select() needs to know
     *       the highest fd number in play, plus one, so it knows how far
     *       to scan internally - NOT a count of how many fds we're
     *       watching.
     * WHY the two NULL arguments: select() can also watch for
     *       "write-ready" fds and "exceptional condition" fds - we don't
     *       care about either here, so we pass NULL for both.
     * WHAT HAPPENS: the kernel puts the process to sleep (without
     *       spinning/busy-waiting) until STDIN has data OR the timer
     *       fires - then readfds is overwritten to reflect only the fds
     *       that are ACTUALLY ready right now. */

    if (ret == -1) {
        // WHAT: select() itself failed (rare - e.g. interrupted by a signal).
        perror("select");
    } else if (ret == 0) {
        // WHAT: the full 10 seconds elapsed with nothing ready.
        printf("No input after 10 seconds.\n");
    } else {
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            /* WHAT: confirm STDIN specifically is the fd that became
             *       ready (with only one fd in the set this is somewhat
             *       redundant here, but it's the correct pattern for
             *       when multiple fds are being watched at once). */
            read(STDIN_FILENO, buffer, sizeof(buffer));
            // WHAT: now that we KNOW data is waiting, read() will return
            //       immediately without blocking - this is the entire
            //       point of checking with select() first.
            printf("You typed: %s", buffer);
        }
    }

    return 0;
}

/* SAMPLE OUTPUT
-------------------------------------------------------------------------------------------------------
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ mkdir prac-13
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ cd prac-13
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-13$ code .
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-13$ gcc 13.c -o 13
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-13$ ./13
You have 10 seconds to type something:
yo here we go
You typed: yo here we go
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-13$

-------------------------------------------------------------------------------------------------
*/
