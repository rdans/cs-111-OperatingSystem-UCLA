TARGET = lab2_list.c SortedList.c SortedList.h Makefile lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png lab2b_list.csv lab2b_list.gp lab2b_test.sh README profile.out

default:
	gcc -Wall -Wextra -lpthread -g -o lab2_list lab2_list.c SortedList.c

tests:
	./lab2b_test.sh

profile:
	gcc -Wall -Wextra -lpthread -g -o lab2_list lab2_list.c SortedList.c -lprofiler
	rm -f ./raw.out
	CPUPROFILE=raw.out ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text lab2_list ./raw.out > profile.out
	pprof --list=thread_run lab2_list ./raw.out >> profile.out
	rm -f ./raw.out

graphs:
	gnuplot lab2b_list.gp

dist: default tests graphs profile
	tar -czf lab2b-604840665.tar.gz $(TARGET)

clean:
	rm lab2_list *.png *.csv 
	rm lab2b-604840665.tar.gz
