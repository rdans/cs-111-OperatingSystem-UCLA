TARGET = lab1b-client.c lab1b-server.c README Makefile my.key

default: lab1b-client lab1b-server

lab1b-client:
	gcc -Wall -Wextra -lmcrypt -o lab1b-client -g lab1b-client.c

lab1b-server:
	gcc -Wall -Wextra -lmcrypt -o lab1b-server -g lab1b-server.c 

dist:
	tar -czf lab1b-604840665.tar.gz $(TARGET)

clean: 
	rm -rf lab1b-client lab1b-server *.gz
