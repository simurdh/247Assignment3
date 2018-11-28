#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>
#include "shm.h"

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char* argv[])
{
  int retVal = 0;
  int fd;

  // <Use the POSIX "shm_open" API to open file descriptor with
  //   "O_RDWR" options and the "0666" permissions>
  fd = shm_open(name, O_RDWR, 0666);
  if (fd == -1) {
  	handle_error_en(fd, "shm_open");
	}

  // <Use the "mmap" API to memory map the file descriptor>
  void *arg;
  arg = mmap(NULL, sizeof(ShmData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (arg == MAP_FAILED) {
    printf("mmap: MAP_FAILED"); exit(EXIT_FAILURE);
  }
  ShmData *shmPtr;
  shmPtr = (ShmData *)arg;


  printf("[Client]: Waiting for valid data ...\n");

  while(shmPtr->status != VALID)
    {
      sleep(1);
    }

  printf("[Client]: Received %d\n",shmPtr->data);

  shmPtr->status = CONSUMED;

  // <use the "munmap" API to unmap the pointer>
   retVal = munmap(NULL,sizeof(ShmData));
   if (retVal) {
 		handle_error_en(retVal, "munmap");
 	}

  printf("[Client]: Client exiting...\n");

  return(retVal);

}
