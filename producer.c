#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
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

unsigned int ip_checksum(char *data, int length);



int main(int argc, char *argv[])
{
    int   shm_fd;
    void  *ptr;
    char  *message;


    item next_produced; //item defined above


    int bufferCount = 0;
    unsigned short cksum = 0;

    if (argc != 2) {
        printf("Usage: %s <message> \n", argv[0]);
        return -1;
    }



    while (true)
    {

      //produce an item in next_produced
      //1. increment the buffer count (item_no)
      bufferCount++;

      //2. calculate the 16-bit checksum (cksum)
      cksum = ip_checksum(argv[1],strlen(argv[1]));

      //print checksum...
      printf("Checksum :0x%x (%s) \n",cksum,argv[1]);

      //3. generate the payload data
      // next_produced.payload[n] = (unsigned char) rand() % 256

      while (((in + 1) % BUFFER_SIZE) == out)
          sleep(1);       //do nothing but sleep for 1 second
      buffer[in] = next_produced;       //store next_produced into share buffer size

      in = (in+1) % BUFFER_SIZE;
      break;

    }

    message = argv[1];

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



    /* write the message to shared memory */
    sprintf(ptr, "%s", message);
    return 0;

}

//calculate checksum
unsigned int ip_checksum(char *data, int length){
  unsigned int sum = 0xffff;
  unsigned short word;

  int i;

  //handle complete 16-bit block
  for(i=0;i+1<length;i+=2){
    memcpy(&word,data+i,2);
    sum+=word;
    if(sum>0xffff){
      sum-=0xffff;
    }
  }

  //return the Checksum
  return -sum;
}
