all:
	gcc -o obj/djbll.o -fPIC -c src/djbll.c
	gcc -shared -o obj/libdjbll.so obj/djbll.o

install:
	cp obj/*.so /usr/local/lib
	cp src/*.h /usr/local/include

clean:
	rm -f obj/*
	rm -f djbll

test:
	gcc -o djbll test.c -ldjbll
