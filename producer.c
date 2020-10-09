#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <signal.h>

const int MMAP_SIZE = 4096;
const char *name = "OS-IPC";


#define MMAP_SIZE 4096
#define BUFFER_SIZE 100
#define PAYLOAD_SIZE 34
void sig_handler(int sig){
    printf("\nCtrlc found\n");
    /* remove the shared memory object */
    shm_unlink(name);
    printf("Exiting\n");
    exit(1);
}

typedef struct {
  int item_no;          //number of the item produced
  unsigned short cksum;         //16-bit Internet checksum
  unsigned char payload[PAYLOAD_SIZE];      //random generated data
} item;

item buffer_item[BUFFER_SIZE];
int in = 0;
int out = 0;


extern unsigned int ip_checksum(unsigned char *data, int length);

int main(int argc, char *argv[])
{
    int   i,shm_fd;
    int   nbytes;
    void  *ptr;
    //char  *message;

    item next_produced; //item defined above

    //int bufferCount = 0; //bufferCount
    unsigned short cksum;
    //unsigned char *buffer;   //change item buffer

    item next_consumed; //item defined above
    unsigned short cksum1,cksum2;
    unsigned char *buffer;   //change item buffer

    struct sigaction act;
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (argc != 2) {
        printf("Usage: %s <nbytes> \n", argv[0]);
        return -1;
    }

    nbytes = atoi(argv[1]);

    /* create the shared memory object */
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

    //error checking
    if (shm_fd == -1) {
        perror("Error creating shared memory");
        return -1;
    }

    /* configure the size of the shared memory object */
    ftruncate(shm_fd, MMAP_SIZE);

    /* memory map the shared memory object */
    ptr = mmap(0, MMAP_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    buffer = (unsigned char *)ptr;
    next_produced.item_no = 0;

    while(1){
      //produce an item in next_produced
      //1. increment the buffer count (item_no)
      next_produced.item_no++;
      for(i=0;i<nbytes;i++){
        //3. generate the payload data
        next_produced.payload[i] = (unsigned char) rand() % 256;
      }

      //2. calculate the 16-bit checksum (cksum)
      next_produced.cksum = (unsigned short) ip_checksum(&buffer[0],nbytes);
      //printf("Checksum :0x%x (%s) \n",cksum,argv[1]);

      while (((in + 1) % BUFFER_SIZE) == out){
          sleep(1);
          buffer_item[in] = next_produced;       //store next_produced into share buffer size
          in = (in+1) % BUFFER_SIZE;
      }    //do nothing but sleep for 1 second



      // message = argv[1];

      /* write the message to shared memory */
      //sprintf(ptr, "%s", message);

      memcpy((void *)&buffer_item[in],&next_produced,sizeof(next_produced));

      sigaction(SIGINT, &act, 0);
    }

    return 0;

}
