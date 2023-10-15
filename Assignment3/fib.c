#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>

int fib(int n){
    if (n == 0 || n == 1) return n;
    return fib(n-1) + fib(n-2);
}
double a = 0;
int main(){
    for (int i = 0; i < 10000000; i++){
        a++;
    }
    
    printf("%d\n",fib(20));

    

    return 0;
}