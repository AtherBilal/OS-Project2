#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include "validation.h"

void deallocateMemory();
void checkForErrors(char programName[], int errnoValue);

int shm_id;
int option, n, s, customErrorCode;
void* shm_address;
static char usageError[] = "%s: Usage: %s [-n <number of processes> -s <number of processes running at any given time>]\n";


int main(int argc, char *argv[]){
  extern char *optarg;
  while ((option = getopt(argc, argv, "hn:s:")) != -1) {
    switch (option) {
     //user asked for help
     case('h'):
       printf("Help: to run this program run the following command:\n%s -n <number of processes> -s <number of processes running at any given time>\n", argv[0]);
       return 0;
     case('n'):
       if (isNumber(optarg)) {
         n = (int)strtol(optarg, NULL, 0);
         printf("n: %d\n", n );
       } else {
         customErrorCode = 1;
       }
       break;
     case('s'):
       // outputting a test error
       if (isNumber(optarg)) {
         s = (int)strtol(optarg, NULL, 0);
         printf("s: %d\n", s );
       } else {
         customErrorCode = 1;
       }
       break;
     case ('?'):
       customErrorCode = 3;
       break;
    }
  }
  if (argc != 5){
   customErrorCode = 3;
 }

  switch (customErrorCode) {
   case 1:
     fprintf(stderr, "%s: Error: %s\n", argv[0], "invalid n value");
     fprintf(stderr, usageError, argv[0], argv[0]);
     exit(1);
   case 2:
    fprintf(stderr, "%s: Error: %s\n", argv[0], "Error Requesting shared memory");
   case 3:
     fprintf(stderr, usageError, argv[0], argv[0]);
     exit(1);
 }
 // checkForErrors(argv[0], errno);


  // create shm_key using inode of CreateKeyFile

  printf("creating shared memory key\n");


  key_t shm_key = ftok("./CreateKeyFile", 1);

  printf("%d\n", shm_key);
  checkForErrors(argv[0], errno);
  printf("done creating shared memory key\n");

  // Request shared memory
  shm_id = shmget(shm_key, sizeof(int) * 2, IPC_CREAT | 0644);

  // attach master to shared memory
  shm_address = shmat(shm_id, (void*)0, 0);
  int* shm_clock = shm_address;
  // checkForErrors(argv[0], errno);

  // TODO: Add logic to simulate adding (in child after attaching)
  shm_clock[0] = 0; //seconds
  shm_clock[1] = 0; // nanoseconds



  int running_count = 0;
  pid_t pid;

  fprintf(stderr, "HI");
  //Initial Process Spawn
  for (int i = 0; i < n; i++) {
    pid = fork();
    // checkForErrors(argv[0], errno);
    if (pid == 0) {
      printf("Successfully created child process\n");
      execl("./child", "child", NULL);
    }
    if (pid) {
      running_count++;
    }
    wait(NULL);
    printf("from parent 0:%d\n", shm_clock[0]);
    printf("from parent 1:%d\n", shm_clock[1]);
  }

  // deallocateMemory();
}

void deallocateMemory() {
  if (shm_id)
    shmctl(shm_id, IPC_RMID, NULL);
}

void checkForErrors(char programName[], int errnoValue){
  if (errnoValue) {
    fprintf(stderr, "%s: Error: %s\n", programName, strerror(errno));
    deallocateMemory();
    exit (1);
  }
}
