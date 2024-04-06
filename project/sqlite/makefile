
all:
	./build.sh 

set:
	mkdir ../lib/
	mkdir ../include/	
	mv ./install/lib/libsqlite3.so ../lib/
	mv ./install/include/sqlite3.h ../include/

clean:
	rm -rf install
	./build.sh -c
