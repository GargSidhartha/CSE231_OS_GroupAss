#include <iostream>
#include <list>
#include <functional>
#include <stdlib.h>
#include <cstring>



typedef struct {
  int element;
} thread_args;

int user_main(int argc, char **argv);

/* Demonstration on how to pass lambda as parameter.
 * "&&" means r-value reference. You may read about it online.
 */
void demonstration(std::function<void()> && lambda) {
  lambda();
}

int main(int argc, char **argv) {
  /* 
   * Declaration of a sample C++ lambda function
   * that captures variable 'x' by value and 'y'
   * by reference. Global variables are by default
   * captured by reference and are not to be supplied
   * in the capture list. Only local variables must be 
   * explicity captured if they are used inside lambda.
   */
  int x=5,y=1;
  // Declaring a lambda expression that accepts void type parameter
  auto /*name*/ lambda1 = /*capture list*/[/*by value*/ x, /*by reference*/ &y](void) {
    /* Any changes to 'x' will throw compilation error as x is captured by value */
    y = 5;
    std::cout<<"====== Welcome to Assignment-"<<y<<" of the CSE231(A) ======\n";
    /* you can have any number of statements inside this lambda body */
  };
  // Executing the lambda function
  demonstration(lambda1); // the value of x is still 5, but the value of y is now 5

  int rc = user_main(argc, argv);
 
  auto /*name*/ lambda2 = [/*nothing captured*/]() {
    std::cout<<"====== Hope you enjoyed CSE231(A) ======\n";
    /* you can have any number of statements inside this lambda body */
  };
  demonstration(lambda2);
  return rc;
}

#define main user_main


void parallel_for(int start, int end, std::function<void(int)> && lambda, int numThread){

  thread_args args[numThread];
  pthread_t tid[numThread];

  int chunk = (end - start) / numThread;

  for(int i = 0; i < numThread; i++){
    args[i].element = i;
    auto thread_func = [&](void *ptr)->void*{
      thread_args *args = (thread_args*)ptr;
      lambda(i);
      return NULL;
    };
    pthread_create(&tid[i], NULL, thread_func, (void*)&args[i]);
  }

  


}

void parallel_for(int start1, int end1, int start2, int end2, std::function<void(int, int)> && lambda, int numThread){
  for(int i = start1; i < end1; i++){
    for(int j = start2; j < end2; j++){
      lambda(i, j);
    }
  }
}