#include <iostream>
#include <list>
#include <functional>
#include <stdlib.h>
#include <cstring>



typedef struct {
  std::function<void(int)> lambda;
  // int index;
  int start;
  int end;
} thread_args_vect;

typedef struct {
  std::function<void(int, int)> lambda;
  // int index;
  int start1;
  int end1;
  int start2;
  int end2;
} thread_args_matr;

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


void* thread_func_vect(void *ptr){
  thread_args_vect *t = (thread_args_vect*)ptr;
  // t->lambda(t->index);
  for(int i = t->start; i < t->end; i++){
    t->lambda(i);
  }
  return NULL;
}

void* thread_func_matr(void *ptr){
  thread_args_matr *t = (thread_args_matr*)ptr;
  // t->lambda(t->index);
  for(int i = t->start1; i < t->end1; i++){
    for(int j = t->start2; j < t->end2; j++){
      t->lambda(i, j);
    }
  }
  return NULL;
}


void parallel_for(int start, int end, std::function<void(int)> && lambda, int numThread){

  thread_args_vect args[numThread];
  pthread_t tid[numThread];

  int chunk = (end - start) / numThread;

  for(int i = 0; i < numThread; i++){
    args[i].lambda = lambda;
    args[i].start = start + i * chunk;
    if (i == numThread - 1) {
        args[i].end = end;
    } 
    else {
        args[i].end = args[i].start + chunk;
    }
    if(pthread_create(&tid[i], NULL, thread_func_vect, (void*)&args[i])!=0){
      std::cerr << "Error creating thread" << std::endl;
    }
  }

  for(int i = 0; i < numThread; i++){
    if(pthread_join(tid[i], NULL) != 0){
      std::cerr << "Error joining thread" << std::endl;
    }
  }
}

void parallel_for(int start1, int end1, int start2, int end2, std::function<void(int, int)> && lambda, int numThread){

  thread_args_matr args[numThread];
  pthread_t tid[numThread];

  int chunk = (end1 - start1) / numThread;

  for(int i = 0; i < numThread; i++){
    args[i].lambda = lambda;
    args[i].start1 = start1 + i * chunk;
    args[i].start2 = start2;
    args[i].end2 = end2;

    if (i == numThread - 1) {
        args[i].end1 = end1;
    } 
    else {
        args[i].end1 = args[i].start1 + chunk;
    }
    if (pthread_create(&tid[i], NULL, thread_func_matr, (void*)&args[i]) != 0){
      std::cerr << "Error creating thread" << std::endl;
    }
  }

  for(int i = 0; i < numThread; i++){
    if(pthread_join(tid[i], NULL) != 0){
      std::cerr << "Error joining thread" << std::endl;
    }
  }


  // for(int i = start1; i < end1; i++){
  //   for(int j = start2; j < end2; j++){
  //     lambda(i, j);
  //   }
  // }
}