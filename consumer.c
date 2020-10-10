#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <signal.h>

const int MMAP_SIZE = 4096;
const char *name = "OS-IPC";


#define BUFFER_SIZE 100
#define PAYLOAD_SIZE 34
void sig_handler(int sig){

    printf("\nCtrlc found\n");
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



extern unsigned int ip_checksum(unsigned char *data, int nbytes);

int main(int argc, char *argv[]){
    int   shm_fd;
    void  *ptr;
    int   nbytes;

    item next_consumed; //item defined above
    unsigned short cksum1,cksum2;
    unsigned char *buffer;   //change item buffer

    struct sigaction act;

    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (argc != 2) {
        printf("Usage: %s <buffer-size> \n", argv[1]);
        return -1;
    }

    name = argv[1];
    /* create the shared memory object */
    shm_fd = shm_open(name, O_RDONLY, 0666);
    //error checking
    if (shm_fd == -1) {
        perror("Error creating shared memory");
        return -1;
    }

    int bufferCount=0;
    int count=0;
    while(true){

      while(in == out){
        sleep(1);             //do nothing but sleep for 1 second
        sigaction(SIGINT, &act, 0);
      }

      memcpy((void*)&cksum2, (void*)&buffer[PAYLOAD_SIZE],sizeof(unsigned short));
      next_consumed = buffer_item[out];
      out = (out+1) % BUFFER_SIZE;

      //consumer the item in next_consumed
      //1. check for no skipped buffers (item_no is continguous)
      if(!(bufferCount==next_consumed.item_no)){
        printf("Skipped buffers\n");
        printf("Exiting\n");
        exit(1);
      }

      /* configure the size of the shared memory object */
      ftruncate(shm_fd, MMAP_SIZE);

      /* memory map the shared memory object */
      ptr = mmap(0, MMAP_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

      /* read the message to shared memory */
      //printf("%s\n", (char *)ptr);

      buffer = (unsigned char *)ptr;
      cksum1 = (unsigned short )ip_checksum (&buffer[0],PAYLOAD_SIZE);


      //2. verify the calculated checksum matches what is stored in next_consumed
      if(cksum1!=cksum2){
        printf("checksum mismatch: received 0x%x, expected 0x%x \n",cksum2,cksum1);
        break;
      }
      sigaction(SIGINT, &act, 0);
      bufferCount++;
      next_consumed.item_no++;
    }
    return 0;
}
