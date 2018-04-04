#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int tail(char *input, int lines) {

  FILE *fp;
  if(strcmp(input,"stdin")) { // if not * stdin
    fp = fopen(input,"r");
    if(fp == NULL) {
    return 1;
    }
  }
  else // if * stdin
  fp = stdin;

  int c = 0; // char for reading
  int count = -2; // fseek counter
  int *buffer = (int*)malloc((lines+1)*sizeof(int)); // save positions of line starts to LIFO buffer
  bool stop = true;
  bool report = true;
  unsigned int it = 1; // global iterator, how many REAL times did it read line (for input > than actual count of lines)
  fseek(fp,0,SEEK_END);
  buffer[0] = ftell(fp); // save ftell EOF to buffer, to find out length of last line in the end

  fseek(fp,-2,SEEK_END); // set fseek to last char
  while(it < lines+1 && stop) {
  while((c = fgetc(fp)) != '\n' && ftell(fp) > 1) {
    count--;
    fseek(fp,count,SEEK_END);
  }
  if(ftell(fp) == 1) {// ugly hack: if file is exact lines as LINES, than auto read first line
    fseek(fp,0,SEEK_SET);
    buffer[it] = ftell(fp);
    stop = false;
  }
  buffer[it] = ftell(fp);
  count--;
  fseek(fp,count,SEEK_END); // move fseek to upper line
  it++;
  }

char line[1024] = {'\0'}; // string for print line

for(int i = it-1; i > 0; i--) { // printf lines from file by their positions from buffer[]
  fseek(fp,buffer[i],SEEK_SET);
  if(buffer[i-1]-buffer[i]-1 > 1024 && report) {
    fprintf(stderr,"Some lines are too long. Trimming..\n");
    report = false;
  }
  fgets(line,1024,fp);
  printf("%s",line);
}

free(buffer);
fclose(fp);
return 0;
}

int main(int argc, char *argv[]) {

	if((argc == 3 || argc == 4) && !strcmp(argv[1],"-n")) {
	  char *p;
	  long n = strtol(argv[2], &p, 10);
	  if(argc == 3 && n >= 0) {// stdin, N lines - tail -n 35 <file
	    printf("jedu stdin.\n");
      if(tail("stdin",n))
        goto f_error;
	  }
    else if(argc == 4 && n >= 0) {// file, N lines - tail -n 38 file
      if(tail(argv[3],n))
        goto f_error;
    }
	}
  else if(argc == 1) {// stdin, 10 - tail <file
    if(tail("stdin",10))
      goto f_error;
  }
  else if(argc == 2) {// file, 10 -tail file
    if(tail(argv[1],10))
      goto f_error;
  }
	else {
    fprintf(stderr, "Incorrect argument input.\n");
    return 2;
	}

	return 0;

  f_error: // file error
  fprintf(stderr, "File error.\n");
  return 1;
}
