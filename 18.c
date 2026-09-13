/*
 * Program Number : 18
 * Student Name    : Yashik Jain
 * Register Number : MT2026117
 * Date            : 14-09-2026
 * Description     : Implements record-level (byte-range) locking on a
 *                    single file using fcntl() — a write (exclusive) lock
 *                    for part (a) and a read (shared) lock for part (b) —
 *                    applied independently to each of three fixed-size
 *                    integer records, to demonstrate that fcntl locks are
 *                    scoped to byte ranges, not to the whole file.
 */

#include <stdio.h>      // printf(), scanf(), perror()
#include <stdlib.h>     // general utilities
#include <unistd.h>     // close(), lseek(), read(), write()
#include <fcntl.h>      // open(), fcntl(), O_* flags, F_* flags, struct flock

// added: RECORDS was referenced in the original code's validation check but
// never actually defined anywhere - without this, the program simply does
// not compile. 3 is the correct value here because the prompt text says
// "0-2" and the practical itself requires exactly three records.
#define RECORDS 3

int main() {

    /*
     * WHAT: Open (or create, if it doesn't exist) "records.txt" for both
     *       reading and writing.
     * WHY:  O_RDWR is required because this program both reads records
     *       (for the read-lock experiment) and writes records (for the
     *       write-lock experiment) in the same file. O_CREAT ensures the
     *       very first run doesn't fail just because the file is missing.
     * WHAT HAPPENS: if the file already exists, it is opened as-is with
     *       its current contents intact (no O_TRUNC here, unlike creat()).
     */
    int fd = open("records.txt", O_CREAT | O_RDWR, 0644);
    if (fd < 0) {
        // WHAT: open() failed - could be a permissions issue on the
        // current directory, or the disk being full, etc.
        perror("open");
        return 1;
    }

    /*
     * ============================================================
     * ADDED: ONE-TIME INITIALIZATION OF ALL THREE RECORDS
     * ============================================================
     *
     * WHY THIS IS NECESSARY:
     *
     * The read-lock experiment (part b) needs to read an *existing*
     * record's value. But on a completely fresh run, records.txt is
     * brand new and 0 bytes long - trying to read() a record's bytes
     * from an empty file returns 0 bytes read, leaving that data
     * variable holding garbage/uninitialized memory instead of a
     * real, defined value.
     *
     * So: before anything else happens, we check the file's current
     * size, and if it's shorter than RECORDS integers' worth of
     * space, we pad it out with three zero-valued records.
     */

    // WHAT: seek to the very end of the file and capture the resulting
    //       offset - this offset IS the file's current size in bytes.
    // WHY:  this is the standard way to get a file's length via lseek()
    //       without a separate stat() call.
    off_t size = lseek(fd, 0, SEEK_END);

    if (size < (off_t)(RECORDS * sizeof(int)))
    {
        // WHAT: the file is too short (or brand new) to hold all three
        //       records, so we write three zero values into it.
        int zero = 0;

        // WHAT: move the file pointer back to the very beginning before
        //       writing, since the lseek() above left it at the end.
        lseek(fd, 0, SEEK_SET);

        for (int i = 0; i < RECORDS; i++)
            // WHAT HAPPENS: each write() here advances the file pointer
            // by sizeof(int) bytes automatically, so this loop lays down
            // three consecutive 4-byte zero values with no gaps.
            write(fd, &zero, sizeof(int));
    }

    // WHAT: struct flock describes exactly which lock we want: what kind
    //       (read/write/unlock), and which byte range of the file it
    //       applies to. Zero-initializing it first avoids garbage values
    //       in any fields we don't explicitly set below.
    struct flock fl = {0};

    int rec_no;
    printf("Enter record number to lock (0-2): ");
    scanf("%d", &rec_no);

    /*
     * WHAT: reject any record number outside the valid range.
     * WHY:  without this check, a bad rec_no would compute an l_start
     *       value that lands outside the three records we actually
     *       manage, silently locking (and possibly reading/writing)
     *       the wrong part of the file - or a region past EOF.
     */
    if (rec_no < 0 || rec_no >= RECORDS) {
        printf("Invalid record number\n");
        close(fd);
        return 1;
    }

    /*
     * ============================================================
     * ADDED: CHOOSE WHICH LOCK TYPE TO DEMONSTRATE
     * ============================================================
     *
     * WHY THIS IS NECESSARY:
     *
     * The original code always set fl.l_type = F_WRLCK, meaning it
     * could only ever demonstrate part (a), the write lock. Part (b)
     * of this practical explicitly asks for a read lock too, so the
     * user is asked here which one to exercise on this run.
     */
    char lock_choice;
    printf("Lock type - (w)rite or (r)ead? ");
    // WHAT: the leading space in " %c" tells scanf() to skip any
    // leftover whitespace (including the newline left behind by the
    // previous "%d" read) before reading the actual character.
    scanf(" %c", &lock_choice);

    /*
     * ============================================================
     * SET UP THE LOCK FOR THIS SPECIFIC RECORD
     * ============================================================
     *
     * l_type:
     *      F_RDLCK -> shared lock, for reading only (part b)
     *      F_WRLCK -> exclusive lock, for writing (part a)
     *
     * l_whence + l_start:
     *      SEEK_SET means l_start is measured from the beginning of
     *      the file. rec_no * sizeof(int) gives each record its own
     *      non-overlapping 4-byte starting offset:
     *
     *          record 0 -> byte offset 0
     *          record 1 -> byte offset 4
     *          record 2 -> byte offset 8
     *
     * l_len:
     *      sizeof(int) -> the lock covers exactly one record's worth
     *      of bytes (4 bytes), nothing more, nothing less.
     */
    fl.l_type = (lock_choice == 'r') ? F_RDLCK : F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = rec_no * sizeof(int);
    fl.l_len = sizeof(int);

    /*
     * WHAT: request the lock, and BLOCK (wait) here if it's currently
     *       held by another process in a conflicting way.
     * WHY:  F_SETLKW ("W" = wait) is used instead of F_SETLK because we
     *       want this program to patiently wait its turn rather than
     *       immediately fail if the record is busy.
     * WHAT HAPPENS: this call only returns once the lock has actually
     *       been granted to this process (or an error occurs).
     */
    if (fcntl(fd, F_SETLKW, &fl) < 0) {
        perror("fcntl");
        close(fd);
        return 1;
    }

    if (lock_choice == 'r') {
        /*
         * ============================================================
         * PART (b): READ LOCK PATH
         * ============================================================
         *
         * WHAT: we now safely have shared read access to this record.
         *       Move the file pointer to the record's exact byte offset
         *       and read its current 4-byte integer value back.
         * WHY:  a read lock's whole purpose is to let us safely observe
         *       a record's value with a guarantee that no one else can
         *       be mid-write on it at the same time - so we only read
         *       here, we never modify anything.
         */
        printf("Record %d locked. Reading data...\n", rec_no);

        int data;
        lseek(fd, rec_no * sizeof(int), SEEK_SET);
        read(fd, &data, sizeof(int));

        printf("Record %d contains: %d\n", rec_no, data);

    } else {
        /*
         * ============================================================
         * PART (a): WRITE LOCK PATH (your original logic, unchanged)
         * ============================================================
         *
         * WHAT: we now have exclusive write access to this record - no
         *       other process can be reading or writing it right now.
         *       Ask the user for a new value, then overwrite the
         *       record's bytes with it.
         */
        printf("Record %d locked. Writing data...\n", rec_no);

        int data;
        printf("Enter data for record %d: ", rec_no);
        scanf("%d", &data);

        // WHAT: reposition to the exact start of this record before
        // writing, in case the file pointer moved elsewhere earlier.
        lseek(fd, rec_no * sizeof(int), SEEK_SET);
        write(fd, &data, sizeof(int));
    }

    /*
     * WHAT: release the lock on this record.
     * WHY:  F_UNLCK tells the kernel to remove whichever lock (read or
     *       write) this process currently holds on the specified
     *       range, making it available to any other waiting process.
     * WHY F_SETLK (not F_SETLKW) HERE: releasing a lock can never
     *       conflict with anything else, so there is nothing to ever
     *       wait for - the non-blocking call is simply the natural fit.
     */
    fl.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &fl);

    printf("Record %d unlocked.\n", rec_no);

    // WHAT: release the file descriptor itself back to the kernel.
    // WHY:  good practice once we're completely done with the file -
    // note this would ALSO release any lock we forgot to explicitly
    // unlock above, since closing the last fd on a file drops every
    // fcntl lock this process holds on it.
    close(fd);

    return 0;
}




/*SAMPLE OUTPUT:


yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-18$ ./18
Enter record number to lock (0-2): 0
Lock type - (w)rite or (r)ead? w
Record 0 locked. Writing data...
Enter data for record 0: 111
Record 0 unlocked.

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-18$ ./18
Enter record number to lock (0-2): 1
Lock type - (w)rite or (r)ead? r
Record 1 locked. Reading data...
Record 1 contains: 0

Record 1 unlocked.
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-18$ ./18
Enter record number to lock (0-2): 2
Lock type - (w)rite or (r)ead? w
Record 2 locked. Writing data...
Enter data for record 2: yo yo
Record 2 unlocked.
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-18$

*/