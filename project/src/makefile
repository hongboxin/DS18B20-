#定义库的名称
LIBNAME=project

#定义头文件的路径
HEADPATH=`pwd`/../include/

#定义库文件的路径
LIBPATH=`pwd`/../lib/

#定义编译器
CC=gcc
AR=ar

HEAD+=-I${HEADPATH}
LIB+=-L${LIBPATH}

all:dynamic_lib static_lib
	@make clear
	@make install

dynamic_lib:
	${CC} -shared -fPIC *.c -o lib${LIBNAME}.so ${HEAD}

static_lib:
	${CC} -c *.c ${HEAD}
	${AR} -rcs lib${LIBNAME}.a *.o

clear:
	rm -f *.o

install:
	cp -rf lib${LIBNAME}.* ${LIBPATH}

uninstall:
	rm -rf ${LIBPATH}/lib${LIBNAME}.*

clean:clear
	rm -f lib${LIBNAME}.*

