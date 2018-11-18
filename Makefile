server:
	gcc -o server server.c socket.c
client:
	gcc -o client client.c socket.c
clean:
	rm server client
