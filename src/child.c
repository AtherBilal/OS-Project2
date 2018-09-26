#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

void checkForErrors(char programName[], int errnoValue);

int main(int argc, char *argv[]){
  int shm_id;
  void* shm_address;

  printf("Child running bitch\n" );
  key_t shm_key = ftok("./CreateKeyFile", 1);
  shm_id = shmget(shm_key, sizeof(int) * 2, 0);
  checkForErrors(argv[0], errno);

  shm_address = shmat(shm_id, (void*)0, 0);
  int* shm_clock = shm_address;
  shm_clock[0] = 123; //seconds
  shm_clock[1] = 456; // nanoseconds
  checkForErrors(argv[0], errno);


  printf("from child, 0:%d\n", shm_clock[0]);
  printf("from child, 1:%d\n", shm_clock[1]);

}

void checkForErrors(char programName[], int errnoValue){
  if (errnoValue) {
    fprintf(stderr, "%s: Error: %s\n", programName, strerror(errno));
    exit (1);
  }
}
