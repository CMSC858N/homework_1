# Homework 1 (Practice)

In this homework you will start to build your own library of parallel
primitives. We will start from the basics and gradually implement more
interesting and sophisticated algorithms. For this first homework,
we'll implement efficient parallel algorithms for the `reduce`
primitive, the `scan` (or prefix-sum) primitive, and the `listrank`
primitive. These primitives can be found in their respective
directories.

The rest of the instructions assume that you have cd'd into a given
directory, and uses reduce as an example.

# Instructions
Create a new repository by clicking ``Use this template``->``Create a new repository`` and work on that.

## What you should submit

Please submit your code on Gradescope (you can directly link your github repo). Also,
please submit a pdf with answers for the questions in this README on Gradescope.
We have put all of the questions in quotes, e.g.:

> Make sure your UID and name are clearly visible on your submission.

## Grading

There are 100 points in total.
- 10 points: auto-graded tests to verify correctness, but not efficiency
- 40 points: code satisfying the required work/depth bounds, breakdown
  as follows:
  - 10 points: scan code
  - 15 points: wyllie code
  - 15 points: sampling-based code
- 20 points: writeup for reduce
- 15 points: writeup for scan
- 15 points: writeup for list-ranking

## Can you refer to material online?

Yes! Please go ahead. Just don't blindly copy code. If you found some
resource useful, please include a link to it in your submission.

## Compiling the code

A Makefile is given in each directory, simply use ``make`` to compile the code.

## Running the code

You can run the code with:  
```bash
./reduce [num_elements] [num_rounds]  
```
If not specified, the default values are ``num_elements=1000000000`` and ``num_rounds=3``.
Note that `num_elements` is smaller for listrank.

## Changing the number of threads  
In your **command line**, set the environment variable ``PARLAY_NUM_THREADS`` using ``export``. For example, set the number of threads to 4:  
```bash
export PARLAY_NUM_THREADS=4  
```

You can combine this with running your code, e.g.,:
```bash
PARLAY_NUM_THREADS=8 ./reduce 100000000 3
```

If your machine has 4 cores that are 2-way hyper-threaded, you can
expect reasonable performance up to 8 threads. There are many ways to
find out the number of threads supported by your machine, but one
portable approach (in C++) is 

```C++
#include <thread>
auto num_threads = std::thread::hardware_concurrency();
```

## Using the Tester
We have provided a tester (that we used in Gradescope) in case you wanted something to test your code locally. To set it up, cd to the ``tester`` directory and run the following command (assuming you have ``python`` installed):
```bash
pip3 install -r requirements.txt
```
To run the tester, just run the following command (after cd'ing to the ``tester`` directory):
```bash
bash run
```

# Reduce

For this first primitive we have given a faithful implementation of
the reduce primitive that runs in $O(n)$ work and $O(log n)$ depth.

The algorithm uses a single parallel primitive, `parlay::par_do(f,
g)`, which simply runs the functions `f` and `g` in parallel, and
continues the main thread after both functions finish. This is exactly
what `fork` does in the binary-forking model.

### Initial benchmarking, serial elision

> Try running reduce on some large inputs, e.g., n=1e8 or n=1e9.
  Try running on both 1 thread and all threads on your machine and
  report the running times in your writeup. 

As a point of comparison, on a 2021 M1Pro, this serial version on
n=1e8 elements takes 1.45 seconds, and the parallel version takes
0.238 seconds (6.1x speedup).

The speedup looks high (big numbers good!), but you may be rightfully
skeptical.

> You can measure the speedup this way, but is this the right serial baseline?

This 1-thread running time is the so-called "serial elision" of your
parallel program, i.e., the serial program that we get when for each
call to
``
par_do(f, g);
``
we simply run
``
f();
g();
``

There is still some (small) scheduling overheads incurred when we run
using `PARLAY_NUM_THREADS=1 ./reduce ...`. You can make the serial
elision a little faster by ignoring the scheduler altogether and
compiling using `make SERIAL=1` (the difference is about 18% on my
machine).

## Implementing a better serial baseline

Instead of the recursive serial elision we're using, write a better
serial algorithm for reduce which just goes over the input array once
and computes the sum of all of the elements.

> What is the running time of this serial implementation? Does your
> previous parallel code still get speedup?

## Adding granularity control  

Next, we'll see how to make the parallel algorithm run faster by
performing granularity control, i.e., tuning the point at which we
switch from forking, to using a serial implementation.

Edit the parallel reduce implementation to use a better serial
baseline. In particular, when $n$ is small enough, add the sum
iteratively in sequential instead of dividing the tasks and computing
the sum in parallel.  

> What is the running time of your parallel implementation now, and
> what is the speedup it obtains over the optimized serial
> implementation?


# Scan

We will go through a similar set of steps as in reduce, but now with the
scan primitive (and a bit faster). You will implement the scan
algorithm we designed together in class which runs in $O(n)$ work and
$O(\log n)$ depth.

We have provided a serial implementation of an in-place scan in
scan.h.

Your parallel scan implementation should be implemented in the
`scan_inplace` function. Note that your implementation should mutate
the input array, `A`. You may need to allocate some additional memory
in your implementation (you can see an example of this allocation in
scan.cpp).

> Measure the self-speedup and the speedup with respect to the serial
> baseline on n=1e9. Are the two numbers close, or quite different?

If the performance of your parallel implementation was poor relative
to the serial implementation, try to optimize your implementation
further. Did you try applying granularity control?

# List Ranking

In this last part of this homework, you will implement and evaluate
three different list ranking algorithms. We have implemented the first
one for you, which simply performs list-ranking on an input list
sequentially.

The input to the problem is a single linked-list. A list node is
defined as

```C++
struct ListNode {
  ListNode* next;
  size_t rank;
  ListNode(ListNode* next) : next(next), rank(std::numeric_limits<size_t>::max()) {}
};
```

The problem is to write the correct value (distance) into each node's
`rank` field.

We consider two different distributions.
- The first is a random cyclic permutation, which we generate using a
  variant of the Knuth Shuffle. (flag = 0)
- The second is a linked list where succ(node(i)) = node(i+1). (flag =
  1)

The first list has essentially no locality, whereas the second list
has very high locality. Unless specified otherwise, please use the
first distribution (the default) for all tests. You can use the second
distribution by specifying a command-line flag:
```bash
./serial 100000000 3 1
```
where the third argument sets the distribution flag to 1.

## Measuring the serial baseline

> Run the serial algorithm on some large lists, e.g., n=1e7 or n=1e8.
> What is the largest input that you can solve using the serial
> implementation in under a minute?

## Wyllie's algorithm

First, please implement Wyllie's algorithm as we have described in
class. 

You may need to allocate some extra memory to save information (e.g.,
pointers, rank information) within a round. Make sure you free any
temporary arrays that you have allocated.

> What is the parallel running time of your Wyllie's implementation?
> What speedup does it get over serial list ranking for a reasonably
> large value of n?

## Sampling-based algorithm

Lastly, and as probably the most involved part of this homework, you
will implement a sampling-based method for list-ranking similar to the
one we discussed in class. Instead of aiming to get
$O(\mathsf{polylog}(n))$ depth, we will see that a $O(\sqrt n)$ depth
algorithm is actually highly practical and provides plenty of
parallelism on current multicore machines.

Your algorithm should be similar to the algorithm we described in
class, with a few changes:
- sample $O(\sqrt n)$ points
- ensure that the head and tail of the list are included in the sample

Now, the algorithm runs in three steps:
1. Build a linked-list only on the sampled nodes, where each sample
has a "weight" proportional to the number of non-sampled neighbors
between it and the next sample
2. Run a serial (weighted) list-ranking algorithm on the sample-only
linked-list. At this point, the ranks for samples are correctly
computed.
3. Assign ranks for the non-sampled nodes.

You should carefully think about how to implement this algorithm so as
to ensure that your overall algorithm runs in $O(n)$ work and
$O(\sqrt n\log(n))$ depth whp.

> What is the parallel performance of your sampling-based
> implementation compared to your implementation of Wyllie's
> algorithm?

> What is the self-speedup of your sampling based algorithm? When you
> run the serial elision, how fast is it relative to the sequential
> list-ranker? The ratio of these two running times empirically
> measures the work-inefficiency of your parallel algorithm.
