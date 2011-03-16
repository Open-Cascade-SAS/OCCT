
#include <stdio.h>
#include <stdlib.h>



int main(int argc,char **argv) { 
  char t[10000];
  do { 
    if(fgets(t,10000,stdin)) { 
      if (t[0] == 'D' && t[1] == 'r' && t[2] == 'a' && t[3] == 'w' &&
	  t[4] == '[' && t[5] == '1' && t[6] == ']')
	fputs(t+9,stdout);
      else
	fputs(t,stdout);
    }
  }
  while(!feof(stdin));

  return(1);
}
