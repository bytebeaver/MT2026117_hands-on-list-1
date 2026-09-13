/*
 * Program Number : 5
 * Student Name    : Yashik Jain
 * Register Number : MT2026117
 * Date            : 11-09-2026
 * Description     : Continuously creates new files in an infinite loop,
 *                    deliberately never closing any file descriptor, so
 *                    that the process's growing fd table can be observed
 *                    live via /proc/<PID>/fd while it runs in the
 *                    background.
 */

#include <stdio.h>      // printf()
#include <fcntl.h>      // open(), O_CREAT, O_WRONLY
#include <unistd.h>     // close(), sleep()
#include <stdlib.h>     // exit()
#include <string.h>     // strerror()
#include <errno.h>      // errno

int main()
{
    int fd;
    int file_number = 1;
    char filename[100];

    /*
     * ============================================================
     * PRACTICAL 5
     * ============================================================
     *
     * Create files continuously in an infinite loop.
     *
     * We will:
     *
     * 1. Create a new file.
     * 2. Keep its file descriptor open.
     * 3. Create another file.
     * 4. Keep going forever.
     *
     * The program will later be run in the background.
     *
     * We can then use:
     *
     *      /proc/<PID>/fd
     *
     * to see the file descriptors currently opened by the process.
     */


    while (1)
    {
        /*
         * Create a filename such as:
         *
         *      file1.txt
         *      file2.txt
         *      file3.txt
         *      ...
         */

        sprintf(filename, "file%d.txt", file_number);


        /*
         * Open/create the file.
         *
         * O_CREAT:
         *      Create the file if it does not exist.
         *
         * O_WRONLY:
         *      Open the file for writing only.
         *
         * 0644:
         *      Permissions given to a newly created file.
         */

        fd = open(filename, O_CREAT | O_WRONLY, 0644);


        /*
         * Check whether open() was successful.
         *
         * open() returns:
         *
         *      >= 0  -> valid file descriptor
         *      -1    -> error
         */

        if (fd == -1)
        {
            printf("Error opening %s: %s\n",
                   filename,
                   strerror(errno));

            return 1;
        }


        /*
         * Display which file was created and which
         * file descriptor was assigned to it.
         */

        printf("Created %s -> FD = %d\n", filename, fd);


        /*
         * IMPORTANT:
         *
         * We deliberately DO NOT call close(fd).
         *
         * Therefore, the file descriptor remains open.
         *
         * This allows us to later inspect the process using:
         *
         *      /proc/<PID>/fd
         */


        /*
         * Move to the next file number.
         */

        file_number++;


        /*
         * Wait for one second before creating the next file.
         *
         * This makes it easier to observe the process while
         * it is running.
         */

        sleep(1);
    }


    return 0;
}



/* sample output
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-05$ ls
5.c
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-05$ $ gcc -Wall -o 5 5.c
$: command not found
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-05$  gcc -Wall -o 5 5.c
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-05$ ./5 &
[1] 3633
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-05$ Created file1.txt -> FD = 3
Created file2.txt -> FD = 4
Created file3.txt -> FD = 5
Created file4.txt -> FD = 6
Created file5.txt -> FD = 7
Created file6.txt -> FD = 8
Created file7.txt -> FD = 9
Created file8.txt -> FD = 10
Created file9.txt -> FD = 11

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-05$ ls -l file*.txt
-rw-r--r-- 1 yashik yashik 0 Sep 13 19:35 file1.txt
-rw-r--r-- 1 yashik yashik 0 Sep 13 19:35 file10.txt
-rw-r--r-- 1 yashik yashik 0 Sep 13 19:35 file11.txt
-rw-r--r-- 1 yashik yashik 0 Sep 13 19:35 file12.txt
-rw-r--r-- 1 yashik yashik 0 Sep 13 19:35 file13.txt

