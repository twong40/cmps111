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

int main(void){
  FILE *fp = NULL;
  fp = fopen("benchmark.txt" ,"a");
  pid_t child_a, child_b;
  int lo_pri = -10;
  int hi_pri = 10;
  id_t pid;
  int which = PRIO_PROCESS;
  child_a  = fork();
  if(child_a == 0){
    //child A code
    pid = getpid();
    setpriority(which, pid, lo_pri);
    for(int i = 0; i < 1000; i ++){
      fprintf(fp, "Child_A \n");
      fprintf(fp, "This is doing work \n");
    }
  }
  else{
    child_b = fork();
    if(child_b == 0){
      //child_b code
      pid = getpid();
      setpriority(which, pid, hi_pri);
      for(int i = 0; i < 1000; i ++){
        fprintf(fp, "Child_B \n");
        fprintf(fp, "This is doing work \n");
      }
    }
    else{
      //parent code
    }
  }
  fclose(fp);
  return 1;
}
