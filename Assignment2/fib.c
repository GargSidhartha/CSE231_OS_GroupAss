/*
 * No changes are allowed in this file
 */
#include<stdio.h>
int fib(int n) {
  if(n<2) return n;
  else return fib(n-1)+fib(n-2);
}

int main(int argc, char** argv) {
  int vale = 0;
  if (argv[1] == "40"){
    vale = 40;
  }
	int val = fib(40);
  printf("%d\n",val);
	return val;
}
