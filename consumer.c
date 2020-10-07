#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdbool.h>

const int MMAP_SIZE = 4096;
const char *name = "OS-IPC";


#define MMAP_SIZE 4096
#define BUFFER_SIZE 100
#define PAYLOAD_SIZE 34

typedef struct {
  int item_no;          //number of the item produced
  unsigned short cksum;         //16-bit Internet checksum
  unsigned char payload[PAYLOAD_SIZE];      //random generated data
} item;

item buffer[BUFFER_SIZE];
int in = 0;
int out = 0;


int main(int argc, char *argv[])
{
    int   shm_fd;
    void  *ptr;


    item next_consumed; //item defined above

    /* create the shared memory object */
    shm_fd = shm_open(name, O_RDONLY, 0666);

    //error checking
    if (shm_fd == -1) {
        perror("Error creating shared memory");
        return -1;
    }


    while(true)
    {

        while(in == out)
          sleep(1);       //do nothing but sleep for 1 second
        next_consumed = buffer[out];
        out = (out+1) % BUFFER_SIZE;

        //consumer the item in next_consumed
        //1. check for no skipped buffers (item_no is continguous)
        //2. verify the calculated checksum matches what is stored in next_consumed
        break;
    }


    /* configure the size of the shared memory object */
    ftruncate(shm_fd, MMAP_SIZE);

    /* memory map the shared memory object */
    ptr = mmap(0, MMAP_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

    /* read the message to shared memory */
    printf("%s\n", (char *)ptr);

    /* remove the shared memory object */
    shm_unlink(name);
    return 0;

}
