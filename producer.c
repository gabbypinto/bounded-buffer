#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

const int MMAP_SIZE = 4096;
const char *name = "OS-IPC";

int main(int argc, char *argv[])
{

    int   shm_fd;
    void  *ptr;
    char  *message;

    if (argc != 2) {
        printf("Usage: %s <message> \n", argv[0]);
        return -1;
    }

    message = argv[1];

    /* create the shared memory object */
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error creating shared memory");
        return -1;
    }

    /* configure the size of the shared memory object */
    ftruncate(shm_fd, MMAP_SIZE);

    /* memory map the shared memory object */
    ptr = mmap(0, MMAP_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    /* write the message to shared memory */
    sprintf(ptr, "%s", message);

    return 0;

}
