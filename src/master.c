#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void deallocateMemory();

int shm_id;

int main(int argc, char *argv[]){

  key_t shm_key = ftok("./CreateKeyFile", 1);
  shm_id = shmget(shm_key, sizeof(int) * 2, IPC_CREAT | 0644);
  if (shmtime_id < 0){
    printf("error getting shared memory\n");
    exit(1);
  }
  deallocateMemory();

}

void deallocateMemory() {
  if (shm_id)
    shmctl(shm_id, IPC_RMID, NULL);
}
