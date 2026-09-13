/*
Program Number: 15
Student Name: Yashik Jain
Register Number: MT2026117
Date: 12-09-2026
Description: This program displays all environment variables inherited
             by the current process, by walking the global environ
             array provided by the C runtime.
*/

#include <stdio.h>      // WHAT: puts()
#include <unistd.h>     // WHAT: conventionally included alongside environ
                        //       usage, though environ itself is declared
                        //       via the extern line below, not this header

extern char **environ;
/* WHAT: a pointer to an array of strings ("KEY=VALUE" pairs), terminated
 *       by a NULL pointer - NOT defined anywhere in this file.
 * WHY extern: this variable is created and filled in by the C runtime
 *       startup code before main() runs, using the environment the
 *       parent process (the shell) handed down at fork/exec time. extern
 *       tells the compiler "trust me, this exists elsewhere - just link
 *       against it." */

int main() {

    for (char **p = environ; *p; ++p)
        /* WHAT: p starts pointing at environ[0] and walks forward one
         *       entry at a time.
         * WHY the loop condition is just `*p`: *p is the string p
         *       currently points to - the loop continues as long as that
         *       string pointer isn't NULL. The array's real end is
         *       marked by a NULL entry, not by a separate count variable
         *       (there's no "envc" the way there's an argc for argv). */
        puts(*p);
        // WHAT: print the current "KEY=VALUE" string followed by a
        //       newline - one environment variable per line of output.

    return 0;
}


/* SAMPLE OUTPUT 
--------------------------------------------------------------------------------------------------
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ mkdir prac-15
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1$ cd prac-15
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-15$ code .
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-15$ gcc -Wall -o 15 15.c
yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-15$ ./15
SHELL=/bin/bash
WSL2_GUI_APPS_ENABLED=1
WSL_DISTRO_NAME=Ubuntu
NAME=LAPTOP-UV7S5VGB
PWD=/home/yashik/linux-learning/02-HOL1/prac-15
LOGNAME=yashik
HOME=/home/yashik
LANG=C.UTF-8
WSL_INTEROP=/run/WSL/9_interop

yashik@LAPTOP-UV7S5VGB:~/linux-learning/02-HOL1/prac-15$ ./15 | wc -l
26
-----------------------------------------------------------------------------------------------------
*/