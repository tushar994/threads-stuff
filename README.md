# Q1


## RUNNING
to run
```shell
$ gcc -std=gnu99 q1.c -lpthread -lrt 
$ ./a.out
```


## normal mergesort
- This is a simple mergesort, that does selection sort if the number of elements in the sub-problem is less than 5.

## mergesort with processes
- This is mergesort implementated such that for every subproblem there is a subprocess that solves it. The process that calls the two subprocesses waits until bot the subprocesses end, and continues to do what it's supposed to do.
- if the number of elements in a subprocess is less than 5, then it does selection sort.
- INITIALLY THE the subprocesses communicated with each other through pipes. After that I changed it and used shared memory. Just wanted to flex.


## mergesort with threads
- This is mergesort implementated such that for every subproblem there is a thread that solves it. The thread that calls the two threads waits until both the threads end, and continues to do what it's supposed to do.
- if the number of elements in a subprocess is less than 5, then it does selection sort.
- INITIALLY THE the subprocesses communicated with each other through pipes. After that I changed it and used shared memory. Just wanted to flex.

## compairing times
We see that the threaded mergesort and the process mergesort take much more time than the normal mergesort.
This is because threaded mergesort will create ALOT of threads, and most of those threads will do every little work (like sort 4 numbers)
This is because process mergesort will create ALOT of processes, and most of those threads will do every little work (like sort 4 numbers)

for example, if there are 10,000 integers, then there will be two processes(or threads) that sort approx 5,000 integers. There will be approx 2500 processes(threads) that so selection sort on 4 integers. The total number of processes will be approx ![equation]( 2^(log2(10000) ) )


