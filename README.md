# thread_stuff


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

# Q2

## RUNNING
to run
```shell
$ gcc -std=gnu99 q2.c -lpthread -lrt 
$ ./a.out
```

## ASSUMPTIONS
- Students reach the gate in between 0 and 5 seconds (a random integer bbetween them).


## function breakdown
- `pharma_zone :` This is the function that is the thread for all pharmaceutical companies. While the number of students who aren't done for the day is more than zero, it will producea random number of batches (1-5) where each batch can have (10-20) vaccines. It will take a (2-5) seconds to do this.
- `stud_func : ` This is the function that represents a student. It waits for the student to arrive (sleeps for (1-5)). then the student looks for a slot in a vaccination zone. He leaves when he gets anti-bodies or when he execeedes 3 tries.
- `zone_func : ` This is the function the represents the zone. It looks for vaccine when it has no vaccines. If it has vaccines, it gives slots and waits for either the total number of slots to be filled or the number of waiting students to become zero (if it has already given atleast one slot). It then waits for it's students to get vaccinated and then gets onto the next slot allocation. If the total number of vaccines is zero, it looks for more vaccines from a zone.

## Number of Mutexs used
- We use n+m+1 mutexs. 
- In the code, i have used a linked list to store the companies and the zones with their mutexs. The start of the linked list is a dummy, with an uninitialised and unused mutex.

