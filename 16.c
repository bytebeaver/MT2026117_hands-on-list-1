/*
 * ============================================================
 * Program Number : 16
 * Program Name   : Mandatory Locking Implementation
 * Register No.   : MT2026117
 * Date           : 13-09-2026
 *
 * Description:
 * This program demonstrates file locking using the fcntl()
 * system call.
 *
 * The program supports:
 *      w -> acquire a write lock
 *      r -> acquire a read lock
 *      u -> release a lock
 *
 * The lock is applied to the entire file.
 *
 * Usage:
 *      ./16 <file> <r|w|u>
 *
 * Examples:
 *      ./16 file.txt w
 *      ./16 file.txt r
 *      ./16 file.txt u
 *
 * Important:
 * F_SETLKW makes the process WAIT if a conflicting lock
 * is already held by another process.
 *
 * ============================================================
 */

#include <fcntl.h>      // open(), fcntl(), O_CREAT, O_RDWR,
                        // F_WRLCK, F_RDLCK, F_UNLCK, F_SETLKW,
                        // struct flock

#include <unistd.h>     // close(), pause()

#include <stdio.h>      // printf(), fprintf(), perror()

#include <string.h>     // Included in the supplied code;
                        // not directly used in this program.


/*
 * ------------------------------------------------------------
 * Function: lock_region()
 *
 * Purpose:
 *      Acquire or release a lock on the entire file.
 *
 * Parameters:
 *      fd   -> file descriptor of the file to lock
 *      type -> type of operation:
 *              F_WRLCK : write lock
 *              F_RDLCK : read lock
 *              F_UNLCK : unlock
 *
 * Return value:
 *      Returns the result of fcntl().
 *
 *      fcntl() returns:
 *          >= 0  -> success
 *          -1    -> error
 * ------------------------------------------------------------
 */

static int lock_region(int fd, short type)
{
    /*
     * struct flock describes a file/record lock.
     *
     * Think of 'fl' as a form that tells the kernel:
     *
     *      What type of lock?
     *      Where does the lock start?
     *      How much of the file should be locked?
     */
    struct flock fl = {0};


    /*
     * Specify the type of lock.
     *
     * type can be:
     *
     *      F_WRLCK -> exclusive write lock
     *      F_RDLCK -> shared read lock
     *      F_UNLCK -> remove lock
     */
    fl.l_type = type;


    /*
     * Specify how l_start should be interpreted.
     *
     * SEEK_SET means:
     *
     *      Interpret l_start relative to the
     *      beginning of the file.
     */
    fl.l_whence = SEEK_SET;


    /*
     * Start the lock at offset 0.
     *
     * Since l_whence = SEEK_SET,
     * offset 0 means the beginning of the file.
     */
    fl.l_start = 0;


    /*
     * A length of 0 has a special meaning for fcntl()
     * record locking:
     *
     *      lock from l_start until the end of the file.
     *
     * Since l_start = 0, this effectively locks
     * the entire file.
     */
    fl.l_len = 0;


    /*
     * Ask the kernel to apply the lock.
     *
     * fd:
     *      identifies the file.
     *
     * F_SETLKW:
     *      Set the requested lock.
     *      The 'W' means WAIT.
     *
     *      If another process holds a conflicting
     *      lock, this process blocks until the lock
     *      becomes available.
     *
     * &fl:
     *      address of the struct flock containing
     *      our locking instructions.
     *
     * The result of fcntl() is returned directly.
     */
    return fcntl(fd, F_SETLKW, &fl);
}


/*
 * ------------------------------------------------------------
 * main()
 * ------------------------------------------------------------
 */

int main(int argc, char **argv)
{
    /*
     * The program expects exactly three arguments:
     *
     *      argv[0] -> program name
     *      argv[1] -> file name
     *      argv[2] -> mode: r, w, or u
     *
     * Example:
     *
     *      ./16 file.txt w
     *
     * Therefore argc must be 3.
     */
    if (argc != 3)
    {
        /*
         * Print usage information to stderr because
         * incorrect command-line arguments are an error.
         */
        fprintf(stderr,
                "Usage: %s <file> <r|w|u>\n",
                argv[0]);

        /*
         * Return non-zero to indicate an error.
         */
        return 1;
    }


    /*
     * Open the file.
     *
     * argv[1] contains the filename.
     *
     * O_CREAT:
     *      Create the file if it does not exist.
     *
     * O_RDWR:
     *      Open the file for both reading and writing.
     *
     * 0644:
     *      Permissions used when a new file is created.
     */
    int fd = open(argv[1], O_CREAT | O_RDWR, 0644);


    /*
     * open() returns -1 when it fails.
     *
     * Therefore fd < 0 indicates an error.
     */
    if (fd < 0)
    {
        /*
         * perror() prints a human-readable error
         * corresponding to errno.
         */
        perror("open");

        return 1;
    }


    /*
     * argv[2] contains the mode.
     *
     * argv[2][0] means:
     *
     *      first character of argv[2].
     *
     * For:
     *
     *      ./16 file.txt w
     *
     * mode becomes:
     *
     *      'w'
     */
    char mode = argv[2][0];


    /*
     * --------------------------------------------------------
     * WRITE LOCK
     * --------------------------------------------------------
     *
     * If the user entered:
     *
     *      ./16 file.txt w
     *
     * acquire an exclusive write lock.
     */
    if (mode == 'w')
    {
        /*
         * Request an exclusive write lock.
         *
         * F_WRLCK means:
         *      only one writer can hold this lock.
         */
        if (lock_region(fd, F_WRLCK) < 0)
        {
            /*
             * fcntl() failed.
             */
            perror("F_WRLCK");
        }
        else
        {
            /*
             * fcntl() successfully acquired the
             * write lock.
             */
            printf("Write lock acquired on %s\n",
                   argv[1]);
        }
    }


    /*
     * --------------------------------------------------------
     * READ LOCK
     * --------------------------------------------------------
     *
     * If the user entered:
     *
     *      ./16 file.txt r
     *
     * acquire a shared read lock.
     */
    else if (mode == 'r')
    {
        /*
         * F_RDLCK means:
         *      shared read lock.
         *
         * Multiple compatible readers can hold
         * read locks simultaneously.
         */
        if (lock_region(fd, F_RDLCK) < 0)
        {
            /*
             * fcntl() failed.
             */
            perror("F_RDLCK");
        }
        else
        {
            /*
             * Read lock successfully acquired.
             */
            printf("Read lock acquired on %s\n",
                   argv[1]);
        }
    }


    /*
     * --------------------------------------------------------
     * UNLOCK
     * --------------------------------------------------------
     *
     * If the user entered:
     *
     *      ./16 file.txt u
     *
     * request removal of the lock.
     */
    else if (mode == 'u')
    {
        /*
         * F_UNLCK means:
         *      unlock the specified region.
         */
        if (lock_region(fd, F_UNLCK) < 0)
        {
            /*
             * fcntl() failed.
             */
            perror("F_UNLCK");
        }
        else
        {
            /*
             * Unlock request succeeded.
             */
            printf("Lock released on %s\n",
                   argv[1]);
        }
    }


    /*
     * --------------------------------------------------------
     * INVALID MODE
     * --------------------------------------------------------
     *
     * The only valid modes are:
     *
     *      r
     *      w
     *      u
     */
    else
    {
        fprintf(stderr,
                "mode must be r/w/u\n");
    }


    /*
     * Keep the process alive.
     *
     * Why?
     *
     * The process that acquired the lock must remain alive
     * so that we can test the lock from another terminal.
     *
     * pause() suspends the process until a signal is received.
     *
     * Therefore:
     *
     *      lock acquired
     *           ↓
     *      pause()
     *           ↓
     *      process remains alive
     *           ↓
     *      lock remains held
     */
    pause();


    /*
     * When pause() eventually returns, close the file
     * descriptor.
     */
    close(fd);


    
    return 0;
}







//## TERMINAL 1::
// yashik@LAPTOP-UV7S5VGB:~$ cd linux-learning
// yashik@LAPTOP-UV7S5VGB:~/linux-learning$ cd 02-HOL1
// yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ mkdir prac-16
// yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ cd prac-16
// yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-16$ code .
// yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-16$ gcc 16.c -o 16
// yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-16$ gcc 16.c -o 16
// yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-16$ ./16 file.txt w
// Write lock acquired on file.txt
// It's sitting at:

// pause();


//## terminal 2: 
// yashik@LAPTOP-UV7S5VGB:~$ cd linux-learning
// yashik@LAPTOP-UV7S5VGB:~/linux-learning$ cd 02-HOL1
// yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ cd prac-16
// yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-16$ ./16 file.txt w

// the process waits.

// Terminal 1

// F_WRLCK
//    ↓
// 🔒 file.txt
//    ↓
// pause()


//## TERMINAL 1:: LOCK released pressing ctrl + c
// yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-16$ ./16 file.txt w
// Write lock acquired on file.txt
// ^C
// yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-16$

// /## terminal 2:: LOCK acquired
// yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-16$ ./16 file.txt w
// Write lock acquired on file.txt


