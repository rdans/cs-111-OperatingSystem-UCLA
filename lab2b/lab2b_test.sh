#NAME: Reinaldo Daniswara
#EMAIL: rdaniswara@g.ucla.edu
#ID: 604840665

# Implementation of lab2b_1.png and lab2b_2.png
# Mutex synchronized list operations, 1,000 iterations, 1,2,4,8,12,16,24 threads
./lab2_list --iterations=1000 --threads=1 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=2 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=4 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=8 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=12 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=16 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=24 --sync=m >> lab2b_list.csv
# Spin-lock synchronized list operations, 1,000 iterations, 1,2,4,8,12,16,24 threads
./lab2_list --iterations=1000 --threads=1 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=2 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=4 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=8 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=12 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=16 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=24 --sync=s >> lab2b_list.csv

# Implementation of lab2b_3.png
# --yield=id, 4 lists, 1,4,8,12,16 threads,
# and 1, 2, 4, 8, 16 iterations (and no synchronization)
#./lab2_list --threads=1 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=1 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=1 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=1 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=1 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=4 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=4 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=4 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=4 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=8 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=8 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=8 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=8 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=12 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=12 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=12 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=12 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=16 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=16 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=16 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=16 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=16 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv
#--yield=id, 4 lists, 1,4,8,12,16 threads,
# and 10, 20, 40, 80 iterations, --sync=s and --sync=m
#./lab2_list --threads=1 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=16 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=16 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=16 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=16 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=1 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=1 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=1 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=16 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=16 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=16 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=16 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv

# Rerun both synchronized versions, without yields,
# for 1000 iterations, 1,2,4,8,12 threads, and 1,4,8,16 lists.
# Implementation of lab2b_4.png (sync=m)
# for everything that lists=1 have already tested by the first part.
# If I test again in here, it would not produce a nice single line graph.
#./lab2_list --threads=1 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --threads=1 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --threads=1 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
./lab2_list --threads=1 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv
#./lab2_list --threads=2 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv
#./lab2_list --threads=4 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv
#./lab2_list --threads=8 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv
#./lab2_list --threads=12 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv

# Implementation of lab2b_5.png (sync=s)
#./lab2_list --threads=1 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv
#./lab2_list --threads=2 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv
#./lab2_list --threads=4 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv
#./lab2_list --threads=8 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv
#./lab2_list --threads=12 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv
