#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <time.h>	/* for clock_gettime */
#define BILLION 1000000000L
int main(void){
  FILE *fp = NULL;
  fp = fopen("benchmark-A.txt" ,"a");
  uint64_t diff;
	struct timespec start, end;
  pid_t child_a;
  int lo_pri = -10;
  id_t pid;
  int which = PRIO_PROCESS;
  child_a  = fork();
  if(child_a == 0){
    //child A code
    pid = getpid();
    setpriority(which, pid, lo_pri);
    clock_gettime(CLOCK_MONOTONIC, &start);	/* mark start time */
    for(int i = 0; i < 1000; i ++){
      fprintf(fp, "Child_A Doing Work\n");
    }
    clock_gettime(CLOCK_MONOTONIC, &end);	/* mark the end time */
    diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    fprintf(fp,"Benchmark A elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);
    printf("Benchmark A elapsed time = %llu nanoseconds\n", (long long unsigned int) diff);
  }
  else{
  }
  fclose(fp);
  return 1;
}
