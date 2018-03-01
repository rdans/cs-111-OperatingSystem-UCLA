#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2b_1.png ... Thoughput vs Threads for mutex and spin-lock
#   lab2b_2.png ... mean time per mutex wait and mean time per operation for mutex-synchronized list operations.
#   lab2b_3.png ... successful iterations vs. threads for each synchronization method.
#   lab2b_4.png ... throughput vs. number of threads for mutex synchronized partitioned lists.
#   lab2b_5.png ... throughput vs. number of threads for spin-lock-synchronized partitioned lists.
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","

#lab2b_1.png
# how many threads/iterations we can run without failure (w/o yielding)
set title "lab2b_1: Thoughput vs Threads for mutex and spin-lock"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_1.png'

# grep out only single threaded, un-protected, non-yield results
plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
     title 'list w/mutex' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000)/($7) \
     title 'list w/spin-lock' with linespoints lc rgb 'orange'

#lab2b_2.png
set title "Average Mutex Wait and Average Time per Operations"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Time (nanoseconds)"
set logscale y
set output 'lab2b_2.png'
plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
     title 'Avg Time per Operation' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
     title 'Lock Wait Time' with linespoints lc rgb 'orange'

#lab2b_3.png 
set title "Iterations Without Failure"
set logscale x 2
set xrange [0.75:]
set xlabel "Threads"
set ylabel "Successful iterations"
set logscale y 10
set output 'lab2b_3.png'
set key right top
plot \
    "< grep list-id-none lab2b_list.csv" using ($2):($3) \
	title "Unprotected" with points lc rgb 'red', \
    "< grep list-id-m lab2b_list.csv" using ($2):($3) \
	title "Mutex" with points lc rgb 'green', \
    "< grep list-id-s lab2b_list.csv" using ($2):($3) \
	title "Spin-Lock" with points lc rgb 'blue' 

# lab2b_4.png
set title "Throughput vs Threads Number (Mutex)"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Operations per Second"
set logscale y 10
set output 'lab2b_4.png'
set key left bottom
plot \
     "< grep -e 'list-none-m,[0-9]2*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title 'list = 1' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title 'list = 4' with linespoints lc rgb 'green', \
     "< grep -e 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title 'list = 8' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title 'list = 16' with linespoints lc rgb 'orange'

# lab2b_5.png
set title "Throughput vs Number of Threads (Spin lock)"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Operations per Second"
set logscale y 10
set output 'lab2b_5.png'
set key left bottom
plot \
     "< grep -e 'list-none-s,[0-9]2*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title 'list = 1' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title 'list = 4' with linespoints lc rgb 'green', \
     "< grep -e 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title 'list = 8' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title 'list = 16' with linespoints lc rgb 'orange'
