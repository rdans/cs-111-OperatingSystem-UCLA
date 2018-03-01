TARGET = Makefile README lab4b.c P4B_check.sh 

default: 
		gcc -lmraa -lm -Wall -Wextra -g -o lab4b lab4b.c

check:
	./P4B_check.sh 604840665

clean:
	rm -f lab4b *.gz

dist:
	tar -czf lab4b-604840665.tar.gz $(TARGET)


