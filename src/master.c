#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <signal.h>
#include "validation.h"

void deallocateMemory();
void timeoutKillAll();
void handleCtrlC();
void checkForErrors(char programName[], int errnoValue);
int shm_id;
int* shm_clock;
int option, n, s, customErrorCode;
void* shm_address;
static char usageError[] = "%s: Usage: %s [-n <number of processes (max 20)> -s <number of processes running at any given time>]\n";


int main(int argc, char *argv[]){
  // set signals
  alarm(2);
  signal(SIGALRM, timeoutKillAll);
  signal(SIGINT, handleCtrlC);


  extern char *optarg;
  while ((option = getopt(argc, argv, "hn:s:")) != -1) {
    switch (option) {
     //user asked for help
     case('h'):
       printf("Help: to run this program run the following command:\n%s -n <number of processes (max 20)> -s <number of processes running at any given time>\n", argv[0]);
       return 0;
     case('n'):
       if (isNumber(optarg)) {
         n = (int)strtol(optarg, NULL, 0);
       } else {
         customErrorCode = 1;
       }
       break;
     case('s'):
       // outputting a test error
       if (isNumber(optarg)) {
         s = (int)strtol(optarg, NULL, 0);
         if(s > 20) {
           customErrorCode = 5;
         }
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
 if (s > n) {
   customErrorCode = 4;
 }
 switch (customErrorCode) {
   case 1:
    fprintf(stderr, "%s: Error: %s\n", argv[0], "invalid n value");
    fprintf(stderr, usageError, argv[0], argv[0]);
    exit(1);
   case 2:
    fprintf(stderr, "%s: Error: %s\n", argv[0], "Error Requesting shared memory");
    exit(1);
   case 3:
    fprintf(stderr, usageError, argv[0], argv[0]);
    exit(1);
   case 4:
    fprintf(stderr, "%s: Error: %s\n", argv[0], "the -s value cannot be greater than the -n value");
    fprintf(stderr, usageError, argv[0], argv[0]);
    exit(1);
   case 5:
    fprintf(stderr, "%s: Error: %s\n", argv[0], "-s value cannot be greater than 20");
    fprintf(stderr, usageError, argv[0], argv[0]);
    exit(1);
 }

  // create shm_key using inode of CreateKeyFile
  key_t shm_key = ftok("./CreateKeyFile", 1);
  checkForErrors(argv[0], errno);

  printf("done creating shared memory key\n");

  // Request shared memory
  shm_id = shmget(shm_key, sizeof(int) * 2, IPC_CREAT | 0644);
  checkForErrors(argv[0], errno);

  // attach master to shared memory
  shm_address = shmat(shm_id, (void*)0, 0);
  shm_clock = shm_address;
  checkForErrors(argv[0], errno);

  // TODO: Add logic to simulate adding (in child after attaching)
  shm_clock[0] = 0; //seconds
  shm_clock[1] = 0; // milliseconds


  pid_t pid;
  for (int i = 0; i < s; i++) {
    pid = fork();
    if (pid == 0) {

      // convert n to string to be passed into child
      int length = snprintf( NULL, 0, "%d", n );
      char* numString = malloc( length + 1 );
      snprintf( numString, length + 1, "%d", n );

      execl("./child", "child", numString, NULL);
    }
  }
  //Initial Process Spawn
  for (int i = 0; i < n - s; i++) {
    waitpid(pid, NULL, 0);
    pid = fork();
    if (pid == 0) {
      // convert n to string to be passed into child
      int length = snprintf( NULL, 0, "%d", n );
      char* numString = malloc( length + 1 );
      snprintf( numString, length + 1, "%d", n );

      execl("./child", "child", numString, NULL);
    }
  }
  waitpid(pid, NULL, 0);
  printf("from parent clock value -> %d:%d\n", shm_clock[0],shm_clock[1]);

  deallocateMemory();
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

void timeoutKillAll() {
  printf("from parent clock value -> %d:%d\n", shm_clock[0],shm_clock[1]);
  fprintf(stderr, "PID: %ld Master process is timing out. terminating all children\n", (long)getpid());
  kill(0, SIGTERM);
  deallocateMemory();
  exit(1);
}

void handleCtrlC() {
  printf("from parent clock value -> %d:%d\n", shm_clock[0],shm_clock[1]);
  fprintf(stderr, "PID: %ld Master process is timing out due to Ctrl + C. Terminating children\n", (long)getpid());
  kill(0, SIGTERM);
  deallocateMemory();
  exit(1);
}
