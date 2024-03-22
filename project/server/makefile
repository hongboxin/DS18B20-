#定义头文件的路径
HEADPATH=`pwd`/../include/

#定义库文件的路径
LIBPATH=`pwd`/../lib/

HEAD+=-I${HEADPATH}
LIB+=-L${LIBPATH}

CC=gcc

all:
	${CC} server.c -o server ${HEAD} ${LIB} -lproject -lsqlite3

clean:
	rm server.db
	rm server

server_run:
	export LD_LIBRARY_PATH=/home/iot24/xinhongbo/project/lib/ && ./server -p 8787
