default: lab4c

lab4c: lab4c_tcp lab4c_tls

lab4c_tcp:
ifeq ($(shell uname -r | grep 'edison'),)
	gcc -lssl -lcrypto -lmraa -lm -Wall -Wextra -g -o lab4c_tcp lab4c_tcp.c -DDUMMY
else
	gcc -lssl -lcrypto -lmraa -lm -Wall -Wextra -g -o lab4c_tcp lab4c_tcp.c
endif

lab4c_tls:
ifeq ($(shell uname -r | grep 'beaglebone'),)
	gcc -lssl -lcrypto -lmraa -lm -Wall -Wextra -g -o lab4c_tls lab4c_tls.c -DDUMMY
else
	gcc -lssl -lcrypto -lmraa -lm -Wall -Wextra -g -o lab4c_tls lab4c_tls.c
endif

tcp:
	./lab4c_tcp --id=101010111 --host=131.179.192.136 --log=log.txt 18000

tls:
	./lab4c_tls --id=101010111 --host=131.179.192.136 --log=log.txt 19000
	
check:
	./P4C_check.sh 604840665

clean:
	rm -f *.o lab4c_tcp lab4c_tls *.log
	rm -f *.tar.gz

dist: 
	tar -czf lab4c-604840665.tar.gz Makefile README lab4c_tcp.c lab4c_tls.c