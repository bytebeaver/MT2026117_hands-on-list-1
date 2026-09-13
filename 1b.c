/*
Program Number: 1b
Student Name: Yashik Jain
Register Number: MT2026117
Date: 11-09-1026
Description: This program creates a hard link named hardlink.txt that
             points to the same underlying inode as original.txt, using
             the link() system call.
*/

#include <unistd.h>     // WHAT: link()
#include <stdio.h>      // WHAT: perror

int main()
{
	if(link("original.txt","hardlink.txt") == -1)
	/* WHAT: create a new directory entry "hardlink.txt" pointing at the
	 *       SAME inode as "original.txt" - NOT a new file, NOT a copy,
	 *       and NOT a path string like symlink() would store (practical
	 *       1a) - just one more name for the exact same underlying data.
	 * WHY this argument order: matches the shell command
	 *       `ln original.txt hardlink.txt` (no -s flag) - the EXISTING
	 *       file first, the new link name second.
	 * WHAT HAPPENS: the kernel increments original.txt's inode's link
	 *       count (st_nlink) by 1, and adds a new entry in the current
	 *       directory that points at that same inode number. Requires
	 *       original.txt to genuinely already exist and be on the same
	 *       filesystem - unlike symlink(), there is no "dangling" case. */
	{
		perror("link");
		/* WHAT: if link() returns -1, print the reason. In testing,
		 *       common causes were EEXIST ("File exists", when
		 *       hardlink.txt already exists) and EPERM ("Operation not
		 *       permitted", when the existing path is a directory). */
		return 1;
	}

	return 0;
}


/* SAMPLE OUTPUTS TESTS: 

yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ gcc 1b.c -o 1b
yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ echo "hello from original file" > original.txt

yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ ./1b

yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ ls -la original.txt hardlink.txt
-rw-r--r-- 2 yashik yashik 25 Sep 13 18:43 hardlink.txt
-rw-r--r-- 2 yashik yashik 25 Sep 13 18:43 original.txt

yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ ls -i original.txt hardlink.txt
59946 hardlink.txt  59946 original.txt

yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ stat -c "%n has link count %h" original.txt hardlink.txt
original.txt has link count 2
hardlink.txt has link count 2

yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ echo "appended via hardlink.txt" >> hardlink.txt

yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ cat original.txt
hello from original file
appended via hardlink.txt

yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ ./1b
link: File exists

yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ rm original.txt

yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ cat hardlink.txt
hello from original file
appended via hardlink.txt

yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ stat -c "%n has link count %h" hardlink.txt
hardlink.txt has link count 1

*/