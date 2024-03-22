all:
	@echo "Start to compile library..."
	make -C src
	@echo "Start to compile test program..."
	make -C server
	make -C client

server_run:
	@echo "Start to running server..."
	make server_run -C server

client_run:
	@echo "Start to running client..."
	make client_run -C client

clean:
	make clean -C src
	make clean -C server
	make clean -C client

uninstall:
	rm ./lib/libproject.*
