#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <unistd.h>


void checkForErrors(char programName[], int errnoValue);

int main(int argc, char *argv[]){
  int shm_id;
  void* shm_address;
  int n = (int)strtol(argv[1], NULL, 0);
  // create key using the same file as parent (same shared memory)
  key_t shm_key = ftok("./CreateKeyFile", 1);
  checkForErrors(argv[0], errno);
  shm_id = shmget(shm_key, sizeof(int) * 2, 0);
  checkForErrors(argv[0], errno);
  shm_address = shmat(shm_id, (void*)0, 0);
  int* shm_clock = shm_address;

  int i;
  for (i=1; i< n * 1000000; i++){
      if(shm_clock[1] >= 999){
        shm_clock[1]= 0;
        shm_clock[0]++;
      } else {
        shm_clock[1]++;
      }
  }


}

void checkForErrors(char programName[], int errnoValue){
  if (errnoValue) {
    fprintf(stderr, "%s: Error: %s\n", programName, strerror(errno));
    exit (1);
  }
}
