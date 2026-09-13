/*
Program Number: 12
Student Name: Yashik Jain
Register Number: MT2026117
Date: 12-09-2026
Description: This program opens a file and uses the fcntl() system call
             with F_GETFL to ask the kernel what access mode (read-only,
             write-only, or read-write) that file descriptor was opened
             in, then decodes and prints the result.
*/

#include <fcntl.h>      // WHAT: open(), fcntl(), and the O_*/F_* constants
#include <stdio.h>      // WHAT: printf
#include <unistd.h>     // WHAT: close()

int main() {

    int fd = open("mode.txt", O_RDWR | O_CREAT, 0644);
    /* WHAT: open (creating if needed) "mode.txt" in read-write mode.
     * WHY O_RDWR here: this is the mode we're choosing to test - change
     *       this single line to O_RDONLY or (O_WRONLY | O_CREAT) to see
     *       the program correctly detect each of the other two modes. */

    int flags = fcntl(fd, F_GETFL);
    /* WHAT: ask the KERNEL (not our own program's memory) what mode fd
     *       is actually in. This is the core system call of this practical.
     * WHAT HAPPENS: the kernel looks up fd's open file description and
     *       returns its stored flags as one packed integer. */

    printf("Raw flags value: %d (0x%x)\n", flags, flags);
    // WHAT: print the raw number so we can see exactly what the kernel
    //       returned, before we decode it - this is purely for learning;
    //       remove it if your instructor wants only the final label.

    if ((flags & O_ACCMODE) == O_RDONLY) 
        printf("Read Only\n");
    else if ((flags & O_ACCMODE) == O_WRONLY) 
        printf("Write Only\n");
    else 
        printf("Read & Write\n");
    /* WHAT: `flags & O_ACCMODE` extracts ONLY the access-mode bits from
     *       the raw flags value, discarding every other bit (see the full
     *       bit-by-bit walkthrough below - this line is the entire point
     *       of the practical). The `else` branch catches O_RDWR without
     *       needing to name it explicitly, since only 3 outcomes exist. */

    close(fd);
    return 0;
}



/*
SAMPLE OUTPUT 
-----------------------------------------------------
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-12$ gcc -Wall -o 12 12.c
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-12$ ./12.c
-bash: ./12.c: Permission denied
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-12$ ./12
Raw flags value: 32770 (0x8002)
Read & Write
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-12$

---------------------------------------------------------

*/