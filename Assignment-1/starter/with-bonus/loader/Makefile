#Create lib_simpleloader.so from loader.c
loader: loader.c loader.h
	gcc -m32 -shared -o ../bin/lib_simpleloader.so loader.c loader.h
#Provide the command for cleanup
clean:
	-@rm -f ../bin/lib_simpleloader.so
