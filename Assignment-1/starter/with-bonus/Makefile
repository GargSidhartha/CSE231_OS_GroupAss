#invoke make inside following directories and in this order: loader, launch, fib

all: loader launch fib

loader:  
	cd loader && make loader
launch: 
	cd launcher && make launcher
fib: 
	cd test && make all
	
loader_clean:
	cd loader
	make clean
	
launch_clean:
	cd launch
	make clean
	
test_clean:
	cd test
	make clean 
	


#move the lib_simpleloader.so and launch binaries inside bin directory


#Provide the command for cleanup
clean:
	loader_clean
	launch_clean
	test_clean


