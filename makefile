lib:
	gcc -c -Wall -Werror -fpic -g -lm *.c
	gcc -shared -o delaunay.so *.o
	rm *.o

exec:
	gcc -Wall -Werror -g -lm *c -o delaunay.exe

test:
	gcc -Wall -Werror -g array.c -D_TEST_ARRAY_ -o array.exe
