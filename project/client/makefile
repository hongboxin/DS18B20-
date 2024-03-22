#定义头文件的路径
HEADPATH=`pwd`/../include/

#定义库文件的路径
LIBPATH=`pwd`/../lib/

HEAD+=-I${HEADPATH}
LIB+=-L${LIBPATH}

CC=gcc

all:
	${CC} client.c -o client ${HEAD} ${LIB} -lproject -lsqlite3

clean:
	rm client.db
	rm client

client_run:
	export LD_LIBRARY_PATH=/home/iot24/xinhongbo/project/lib/ && ./client -i127.0.0.1 -p 8787 -s10
