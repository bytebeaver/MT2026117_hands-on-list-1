/*
 * Program Number : 3
 * Student Name    : Yashik Jain
 * Register Number : MT2026117
 * Date            : <fill in after running>
 * Description     : Creates a new file using the creat() system call and
 *                    prints the file descriptor value returned by the
 *                    kernel. Output is done using only the write() system
 *                    call (no printf/stdio), and the integer fd value is
 *                    converted to ASCII digits manually before printing.
 */

#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;

    // WHAT: Create (or truncate, if it already exists) "test.txt" with
    //       permission bits 0644 (rw-r--r--, subject to umask).
    // WHY:  creat() is shorthand for open(path, O_WRONLY|O_CREAT|O_TRUNC, mode) —
    //       it always opens write-only and always empties an existing file.
    // WHAT HAPPENS: the kernel allocates a new entry in this process's file
    //       descriptor table and returns the smallest unused integer (fd 0,1,2
    //       are already taken by stdin/stdout/stderr, so this is usually 3).
    fd = creat("test.txt", 0644);

    // Check for failure (permissions, invalid path, etc. all show up as -1).
    if (fd == -1)
    {
        // FD 2 = standard error. Using write() here (not printf) keeps the
        // whole program at the raw syscall level, with no stdio buffering.
        write(2, "File creation failed\n", 21);
        return 1;
    }

    // Buffer to hold the fd's value converted into ASCII digit characters.
    // WHY needed at all: write() only understands raw bytes — it has no
    // concept of "print this int". fd is a binary number in memory, not
    // yet the printable character '3', '1', etc.
    char buffer[20];
    int i = 0;

    // Preserve the original fd (needed later for close()) while a separate
    // copy gets destructively divided down to 0 during digit extraction.
    int temp_fd = fd;

    // WHAT: Peel off one decimal digit at a time, least-significant first.
    // WHY:  '0' + digit converts the numeric digit (0-9) into its matching
    //       ASCII character ('0'-'9') using the fact that digit characters
    //       are contiguous in ASCII.
    // WHAT HAPPENS: for fd = 3, this loop runs once and buffer[0] = '3'.
    //       For a hypothetical multi-digit fd like 12, it would produce
    //       '2' then '1' (reverse order) — hence the reversal step next.
    while (temp_fd > 0)
    {
        buffer[i] = '0' + (temp_fd % 10);
        temp_fd = temp_fd / 10;
        i++;
    }

    // WHAT: Reverse the digits in place so they read in normal left-to-right
    //       order (e.g. "21" -> "12") before being written out.
    // WHY:  the extraction loop above necessarily produces digits backwards,
    //       since % 10 always yields the current *last* digit first.
    int left = 0;
    int right = i - 1;

    while (left < right)
    {
        char temp = buffer[left];
        buffer[left] = buffer[right];
        buffer[right] = temp;

        left++;
        right--;
    }

    // FD 1 = standard output. Writes exactly `i` bytes — the digit
    // characters we built above — with no implicit null terminator or
    // formatting, unlike printf().
    write(1, buffer, i);

    // Print newline separately, since write() doesn't add one automatically.
    write(1, "\n", 1);

    // WHAT: Release the file descriptor back to the kernel's table.
    // WHY:  every open fd is a limited resource (per-process fd limits
    //       exist, see `ulimit -n`); once you're done with a file you
    //       should close it so the slot can be reused.
    close(fd);

    return 0;
}

/* sample o/p
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-03$ ls
3.c
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-03$ gcc 3.c -o 3
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-03$ ./3
3
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-03$ ls -l test.txt
-rw-r--r-- 1 yashik yashik 0 Sep 13 19:18 test.txt
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-03$ cat test.txt
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-03$ rm test.txt
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-03$ ./3
3


*/