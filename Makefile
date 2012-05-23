all:
	gcc -o obj/djbhash.o -fPIC -c src/djbhash.c
	gcc -shared -o obj/libdjbhash.so obj/djbhash.o

install:
	cp obj/*.so /usr/local/lib
	cp src/*.h /usr/local/include

clean:
	rm -f obj/*
	rm -f djbhash

test:
	gcc -o djbhash test.c src/djbhash.c -Isrc/ -g
