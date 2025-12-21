make:
	gcc ./src/client/client.c -o client
	gcc ./src/server/server.c -o server -lsqlite3
debug:
	gcc -g ./server.c -o server_debug
clean:
	rm client server server_debug client_online