
.PHONY: all clean
all:
	make clean
	mkdir obj
	gcc -O3 -fPIC -I.  -shared shared_malloc.c nanoMPK.c -g -Wall -o ./obj/libnano.so
clean:
	rm -rf ./obj