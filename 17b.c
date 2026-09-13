/*
 * ============================================================
 * Program Number : 17b
 * Program Name   : Online Ticket Reservation Simulation
 * Register No.   : MT2026117
 * Date           : 13-09-2026
 *
 * Description:
 * Opens tickets.txt, obtains an exclusive write lock,
 * reads the current ticket count, increments it, writes
 * the new count back, releases the lock and exits.
 * ============================================================
 */
/*
Program Number: 17b
Student Name: Yashik Jain
Register Number: MT2026117
Date: <date>
Description: This program simulates a single online ticket reservation:
             it opens tickets.txt, takes a write lock on the whole file,
             reads the current ticket count, increments it, writes the
             new count back, then releases the lock - safe to run
             concurrently as many times as needed without losing updates.
*/

#include <stdio.h>      // WHAT: printf, perror
#include <fcntl.h>      // WHAT: open(), fcntl(), struct flock, F_SETLKW
#include <unistd.h>     // WHAT: read(), write(), lseek(), close()

int main() {
    int fd = open("tickets.txt", O_CREAT | O_RDWR, 0644);
    // WHAT: open for BOTH reading (to see the current count) and
    //       writing (to update it) - O_CREAT here as a safety net, in
    //       case this runs before 17a.c ever has.
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct flock fl = {0};
    fl.l_type = F_WRLCK;    // write lock
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;           // lock whole file
    /* WHAT: describes an exclusive lock covering the entire file - same
     *       struct/fields as practical 16. Exclusive (not a read lock)
     *       is essential here: this isn't just reading data, it's about
     *       to modify it, so no other process should even be allowed to
     *       READ a value that's about to become stale. */

    // Try to acquire lock
    if (fcntl(fd, F_SETLKW, &fl) < 0) {
        /* WHAT: block here until this process is the only one allowed
         *       to touch the file's contents.
         * WHY BEFORE the read(): this is the critical placement - the
         *       lock must be acquired before EITHER the read or the
         *       write, so the whole read-increment-write sequence runs
         *       as one uninterrupted unit from every other process's
         *       point of view. */
        perror("fcntl");
        close(fd);
        return 1;
    }

    printf("Lock acquired. Reserving ticket...\n");

    int tickets = 0;
    read(fd, &tickets, sizeof(int));
    // WHAT: read the 4 bytes currently in the file into `tickets` -
    //       guaranteed to be genuinely up to date, since no other
    //       process could have modified the file while this lock is held.

    tickets++;
    // WHAT: compute the new ticket number in memory - this line touches
    //       nothing on disk yet.

    lseek(fd, 0, SEEK_SET);
    /* WHAT: move the file's read/write position back to byte 0.
     * WHY THIS IS REQUIRED: read() just advanced the file position
     *       forward by sizeof(int) bytes - without seeking back, the
     *       next write() would land AFTER the original 4 bytes instead
     *       of overwriting them. */

    write(fd, &tickets, sizeof(int));
    // WHAT: write the incremented count back to the same 4 bytes at the
    //       start of the file.

    printf("Ticket reserved. Total tickets: %d\n", tickets);

    // Release lock
    fl.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &fl);
    /* WHAT: release the lock, allowing the next waiting process (if any)
     *       to proceed.
     * WHY F_SETLK here, not F_SETLKW: releasing a lock never has
     *       anything to wait FOR - F_SETLK (non-blocking) is the
     *       correct, conventional choice for an unlock request. */

    close(fd);
    return 0;
}
/* SAMPLE OUTPUT

---------------------------------------------------------------------------------------------------

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ mkdir prac-17
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ cd prac-17
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-17$ code .
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-17$ gcc 17a.c -o 17a
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-17$ gcc 17b.c -o 17b
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-17$ ./17a
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-17$ ./17b
Lock acquired. Reserving ticket...
Ticket reserved. Total tickets: 1
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-17$ ./17b
Lock acquired. Reserving ticket...
Ticket reserved. Total tickets: 2
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-17$ ./17b
Lock acquired. Reserving ticket...
Ticket reserved. Total tickets: 3

-------------------------------------------------------------------------------------------------------
*/