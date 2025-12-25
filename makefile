make:
	gcc ./src/client/client.c -o client
	gcc ./src/server/server.c -o server -lsqlite3
debug:
	gcc -g ./src/server/server.c -o server_debug -lsqlite3
clean:
	rm client server server_debug
