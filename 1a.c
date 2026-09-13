/*
Program Number: 1a
Student Name: Yashik Jain
Register Number: MT2026117
Date: 11-09-2026
Description: This program creates a symbolic (soft) link named
             softlink2.txt that points to original.txt, using the
             symlink() system call.
*/

#include <unistd.h>     // WHAT: symlink()
#include <stdio.h>      // WHAT: perror

int main()
{
	if(symlink("original.txt","softlink2.txt") == -1)
	/* WHAT: create a new file "softlink2.txt" whose only content is the
	 *       TEXT STRING "original.txt" - a path, not a copy of any data.
	 * WHY this argument order: matches the shell command
	 *       `ln -s original.txt softlink2.txt` exactly - target path
	 *       first, then the new link's name.
	 * WHAT HAPPENS: the kernel creates a brand-new inode of type
	 *       S_IFLNK (see practical 14) whose data blocks store the
	 *       target path string - it does NOT check whether
	 *       "original.txt" actually exists; that check only happens
	 *       later, whenever something tries to follow the link. */
	{
		perror("symlink");
		/* WHAT: if symlink() returns -1, print the reason - the most
		 *       common cause in testing is EEXIST ("File exists"),
		 *       since symlink() will never overwrite an existing
		 *       "softlink2.txt", unlike open()'s O_TRUNC option. */
		return 1;

	}

	return 0;
}

/* SAMPLE OUTPUT:
--------------------------------------------------------------------------------------------------
###Test 1 — creating the link before the target exists (dangling link), verified:

yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ ./1a
yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ ls -la softlink2.txt
lrwxrwxrwx 1 yashik yashik 12 Sep 13 18:35 softlink2.txt -> original.txt

###Exit code 0 — symlink() succeeded even with no real target present yet. The leading l and the -> original.txt confirm it's a soft link.


###Test 2 — creating the target and reading through the link:
yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ echo "hello from original file" > original.txt
yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ cat softlink2.txt
hello from original file


###Test 3 — running the program a second time, verified:
yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$ ./1a
symlink: File exists
yashik@LAPTOP-UV7S5VGB:~/linux-learning/prac-01$

-------------------------------------------------------------------------------------------------
*/