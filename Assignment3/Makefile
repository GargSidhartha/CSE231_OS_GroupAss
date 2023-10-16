clean:
	rm a.out fib hello SimpleScheduler

fib_hello:
	gcc -o fib fib.c
	gcc -o hello hello.c

scheduler: fib_hello
	gcc -o SimpleScheduler SimpleScheduler.c