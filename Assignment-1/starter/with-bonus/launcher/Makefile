#Compile the launch.c by linking it with the lib_simpleloader.so
launcher: launch.c ../bin/lib_simpleloader.so
	gcc -m32 -c launch.c && gcc -m32 -o launcher launch.o -L../bin -llib_simpleloader 
	
#Provide the command for cleanup
clean:
	-@rm -f launch.o