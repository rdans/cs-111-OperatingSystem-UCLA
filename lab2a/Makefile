TARGET = lab2_add.c lab2_list.c SortedList.c SortedList.h Makefile lab2_add.csv lab2_list.csv lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png lab2_add.gp lab2_list.gp sample.sh README

default:
	gcc -Wall -Wextra -lpthread -g -o lab2_add lab2_add.c
	gcc -Wall -Wextra -lpthread -g -o lab2_list lab2_list.c
tests:
	./sample.sh
graphs:
	gnuplot lab2_add.gp
	gnuplot lab2_list.gp

dist: default tests graphs
	tar -czf lab2a-604840665.tar.gz $(TARGET)
clean:
	rm lab2_add lab2_list *.png *.csv
	rm lab2a-604840665.tar.gz
