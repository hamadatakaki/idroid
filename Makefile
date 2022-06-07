_build:
	gcc -Wall -o bin/client src/droid_client.c src/io/pointers.c src/io/launch.c -lpthread
	gcc -Wall -o bin/server src/droid_server.c src/io/pointers.c src/io/launch.c -lpthread 
