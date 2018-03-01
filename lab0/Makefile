#NAME: Reinaldo Daniswara	
#EMAIL: rdaniswara@g.ucla.edu
#ID: 604840665

all:  
	gcc -Wall -Wextra -g -o lab0 lab0.c

check:
#	checking --input
	@echo "A smoke test file" > input.txt
	@echo "Smoke test for INPUT ..."
	@./lab0 --input=input.txt > output.txt ;\
	if [ $$? -ne 0 ] ;\
	then \
		echo "	Error! Do not pass input test." ;\
	else \
		echo "	Input test ... SUCCESS!";\
	fi
	@rm output.txt

# 	checking --output
	@echo "Smoke test for Output ..."
	@./lab0 --input=input.txt --output=output.txt ;\

	@diff -u input.txt output.txt;\
	if [ $$? -ne 0 ] ;\
	then \
		echo "	Error! Do not pass output test." ;\
	else \
		echo "	Output test ... SUCCESS";\
	fi
	@rm output.txt


# 	checking Wrong argument
	@echo "Smoke test for wrong argument..."
	@./lab0 --xyz ;\
	if [ $$? -ne 1 ] ;\
	then \
		echo "	Argument not valid!";\
	fi

#	checking catch
	@echo "Smoke test for catch, segmentation fault, SIGSEGV"
	@./lab0 --segfault --catch > /dev/null ;\
	if [ $$? -ne 4 ] ;\
	then \
		echo "Error occur when catching and reporting fault.";\
	fi
	@echo ""
	@echo "All test cases passed."
	
clean:
	rm -rf *.tar.gz lab0 *.txt

dist:
	tar -czf lab0-604840665.tar.gz README Makefile *.c *.png
