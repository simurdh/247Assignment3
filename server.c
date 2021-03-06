// compile run: gcc -g server.c -lrt -o server
//
// Analysis
//
// Shared Memory is the fastest method for interprocess communication because information is mapped directly to memory; there is less communication time.
// This IPC would be helpful for debugging because it is more accurate and can provide more information than print statements (expecially in situations where gdb is less helpful, like multithreaded programs).
// Using timers like in Assignment 2, I could update a shared memory object at regular intervals of time (like when the timer expires and a signal is sent out).
// This would provide me with helpful data to track the program's progress and if there is a bug in the program (I could isolate at what time and where it occurs).


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "shm.h"

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)


int main(int argc, char* argv[])
{
	int retVal = 0;
  extern const char* name;
	int fd;
  int serverData;

  //<Confirm argc is 2 and if not print a usage string. Confirm argv[1] is an int
	if (argc != 2) {
		printf("Usage: ./server <int>\n");
		return 1;
	}

  int res = sscanf(argv[1], "%d", &serverData);
  if (!res || res == EOF) {
    printf("Could not parse %s\n", argv[1]);
    return 1;
  }

  //<Use the POSIX "shm_open" API to open file descriptor with
  //"O_CREAT | O_RDWR" options and the "0666" permissions>
	fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	if (fd == -1) {
		handle_error_en(fd, "shm_open");
	}

  //<Use the "ftruncate" API to set the size to the size of your structure shm.h>
  retVal = ftruncate(fd, sizeof(ShmData));
	if (retVal) {
		handle_error_en(retVal, "ftruncate");
	}

  //<Use the "mmap" API to memory map the file descriptor>
  void *arg;
  arg = mmap(NULL, sizeof(ShmData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (arg == MAP_FAILED) {
		printf("mmap: MAP_FAILED\n"); exit(EXIT_FAILURE);
	}
  ShmData *shmPtr;
  shmPtr = (ShmData *)arg;

  //<Set the "status" field to INVALID>
  shmPtr->status = 0;

  //<Set the "data" field to atoi(argv[1])>
  shmPtr->data = serverData;

  //<Set the "status" field to VALID>
  shmPtr->status = 1;

  printf("[Server]: Server data Valid... waiting for client\n");

  while(shmPtr->status != CONSUMED)
    {
      sleep(1);
    }

  printf("[Server]: Server Data consumed!\n");

  //<use the "munmap" API to unmap the pointer>
  retVal = munmap(NULL,sizeof(ShmData));
  if (retVal) {
		handle_error_en(retVal, "munmap");
	}

  //<use the "close" API to close the file Descriptor>
  retVal = close(fd);
  if (retVal) {
		handle_error_en(retVal, "close");
	}

  //<use the "shm_unlink" API to revert the shm_open call above>
  retVal = shm_unlink(name);
  if (retVal) {
		handle_error_en(retVal, "shm_unlink");
	}

  printf("[Server]: Server exiting...\n");

  return(retVal);

}
