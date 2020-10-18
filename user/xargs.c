/*
 * Write a simple version of the UNIX xargs program: read lines from standard input and run a command for each line, supplying the line as arguments to the command. Your solution should be in the file user/xargs.c.
 *
1. Use fork and exec system call to invoke the command on each line of input. Use wait in the parent to wait for the child to complete running the command.
2. Read from stdin a character at the time until the newline character ('\n').
3. kernel/param.h declares MAXARG, which may be useful if you need to declare an argv.
4. Changes to the file system persist across runs of qemu; to get a clean file system run make clean and then make qemu.
 * */

/*
 * xargs util [arguments]
 *
 * */
#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int getarg(char *buf, int nbuf){
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  // remove \n
  if(buf[0]==0){
    // EOF
    return -1;
  }
  if(sizeof(buf)>0){
    int i =0;
    while (buf[i]!=0) {
      i++;
    }
    if(buf[i]==0){
      buf[i-1]=0; 
    }
  }
  return 0;
}

int main(int argc,char* argv[]){
  char buf[255];
  char* argvs[MAXARG];
  int count = 0;
  /* printf("%d %s\n",argc,argv[1]); */
  // read input
  while (getarg(buf, sizeof(buf))>=0) {
    argvs[count] = malloc(sizeof(buf));
    strcpy(argvs[count],buf);
    count++;
  }
  // execute all input
  for (int i =0; i<count; i++) {
    if(fork()>0){
      // parent 
      wait();
    }else{
      // children
/* int */
/* exec(char *path, char **argv) */
    if(argc>1){
      char* sub_argv[MAXARG];
      for (int j =0; j<argc-1; j++) {
	sub_argv[j]=malloc(sizeof(argv[j+1]));
	strcpy(sub_argv[j],argv[j+1]);
      }
      // reuse mem
      sub_argv[argc-1]=argvs[i];
      /* printf("running %s with \n", sub_argv[0],sub_argv[argc-1]); */
      /* for (int j =0; j<argc-1; j++) { */
	/* printf("%s\n", sub_argv[j+1]); */
      /* } */
      if(exec(argv[1],sub_argv)<0){
	fprintf(2,"failed with %s\n",argvs[i]);
      }
      free(sub_argv);
      for (int j =0; j<argc-1; j++) {
	free(sub_argv[j]);
      }
    }else{
	fprintf(1,"%s",argvs[i]);
    }
      exit();
    }
  }
  for (int i = 0; i<count; i++) {
    free(argvs[i]); 
  }
  exit();
}

