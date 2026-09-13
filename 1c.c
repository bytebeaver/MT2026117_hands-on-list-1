/*
Program Number: 1c
Student Name: Yashik Jain
Register Number: MT2026117
Date: 11-09-2026
Description: This program creates a FIFO (named pipe) called myfifo,
             using the mkfifo() library function.
*/

#include <stdio.h>          // WHAT: perror
#include <sys/types.h>      // WHAT: type definitions used by sys/stat.h
#include <sys/stat.h>       // WHAT: mkfifo() itself
#include <unistd.h>         // WHAT: conventionally included alongside
                            //       file-creation syscalls in this series

int main()
{
	if(mkfifo("myfifo", 0644) == -1)
	/* WHAT: create a special FIFO file named "myfifo" with permission
	 *       bits 0644 (rw-r--r--, subject to umask, exactly like
	 *       open()'s mode argument in earlier practicals).
	 * WHAT HAPPENS: the kernel creates a new inode of type S_IFIFO
	 *       (practical 14's file-type family) - a name with no disk
	 *       data storage behind it. It is purely a RENDEZVOUS POINT:
	 *       later, when one process opens it for reading and another
	 *       opens it for writing, the kernel connects them via an
	 *       in-memory pipe buffer - nothing here creates that buffer
	 *       yet, only the NAME that future opens will use to find
	 *       each other. */
	{
		perror("mkfifo");
		/* WHAT: if mkfifo() returns -1, print the reason - in testing,
		 *       the common cause was EEXIST ("File exists"), since
		 *       mkfifo() never overwrites an existing name, matching
		 *       the same convention as symlink() (1a) and link() (1b). */
		return 1;
	}

	return 0;
}







/*SAMPLE OUTPUT:::

-----------------------------------------------------------------------------------------------------

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ cd prac-01
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-01$ code .
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-01$ gcc 1c.c -o 1c

##########################################
Test 1 — creation, verified:

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-01$ ./1c

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-01$ ls -la myfifo
prw-r--r-- 1 yashik yashik 0 Sep 13 18:55 myfifo

##############################################
Test 2 — file type via stat, verified:

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-01$ stat -c "%n is a %F" myfifo
myfifo is a fifo



###############################################
Test 3 — running mkfifo() again, verified:

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-01$ ./1c
mkfifo: File exists


###################################3
Test 4 — the rendezvous behavior, verified across two terminals:

Terminal A:
cat myfifo

(hangs here, producing no output — confirmed blocked for as long as no writer exists)
Terminal B, once A is already waiting

terminal A ::

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-01$ cat myfifo 

terminal B : 
yashik@LAPTOP-UV7S5VGB:~/linux-learning$ cd 02-HOL1
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ cd prac-01
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-01$ echo "hello through the FIFO" > myfifo
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-01$

terminal A : 
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-01$ cat myfifo
hello through the FIFO

-------------------------------------------------------------------------------------------------------
*/