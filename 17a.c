/*
 * ============================================================
 * Program Number : 17a
 * Program Name   : Ticket File Initialization
 * Register No.   : MT2026117
 * Date           : 13-09-2026
 *
 * Description:
 * Creates tickets.txt and initializes the ticket count to 0.
 * ============================================================
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    // Open/create the ticket file.
    // O_WRONLY  -> write only
    // O_CREAT  -> create if it doesn't exist
    // O_TRUNC  -> erase old contents and start fresh
    int fd = open("tickets.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);

    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    // Initial number of reserved tickets.
    int tickets = 0;

    // Store 0 in the file.
    write(fd, &tickets, sizeof(int));

    // Close the file.
    close(fd);

    return 0;
}