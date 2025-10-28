## Project 3: Parallel Merge Sort (Pthreads) 
* Authors: **Ngoc Thanh Uyen Ho (a1875049)**, **Gia Bao Au (a1897967)**
* Group name: **UG Group 1**

## Overview
We implemented a **multi-threaded merge sort** using **POSIX Threads (pthreads)**.
The program sorts a randomly generated integer array. When `cutoff = 0`, it runs the **sequential** merge sort; when `cutoff > 0`, it performs a **parallel** merge sort by spawning two threads per level until the given cutoff depth, then merges results.

## Manifest

* `mergesort.c` — Our implementation:

  * `merge(...)`: merges two sorted subarrays using global temp buffer `B`.
  * `my_mergesort(left, right)`: recursive **sequential** merge sort.
  * `parallel_mergesort(void *arg)`: recursive **parallel** merge sort driven by pthreads; falls back to `my_mergesort` at/after `cutoff`.
  * `buildArgs(left, right, level)`: packs indices/level into a heap-allocated `struct argument` for thread start.
* `mergesort.h`: Header with function prototypes, `struct argument`, and extern globals `A`, `B`, `cutoff` (Do not modify part).
* `test-mergesort.c`: Provided harness: allocates `A`/`B`, fills `A` with random data, calls our sorter, checks order, and prints timing.
* `Makefile`:  Builds `test-mergesort` with `-pthread`.
* `README.md`: This document.

## Building the project (NEED IMPROVED)

Requirements: `gcc` (or compatible), POSIX pthreads.

```bash
make            # builds ./test-mergesort
make clean      # removes objects and binary
```

## Features and usage

**Binary:** `./test-mergesort`

**Syntax:**

```
./test-mergesort <input size n> <cutoff level> <seed>
```

**Examples:**

```bash
# Sequential (single-threaded baseline)
./test-mergesort 1000000 0 1234

# Parallel with two levels (4 leaf tasks)
./test-mergesort 1000000 2 1234

# Larger input to observe speedup (machine dependent)
./test-mergesort 100000000 3 42
```

## Testing

**Correctness**

* We relied on the harness function `check_if_sorted(A, n)` in `test-mergesort.c`, which verifies non-decreasing order.
* Ran with multiple sizes and seeds:

  * Small sanity checks: `n = 10, 100, 1000` to visually inspect and catch off-by-one errors.
  * Mixed seeds (e.g., `1, 5, 42, 1234`) to vary inputs.
  * Edge cases: `n = 2`, already-sorted data (by reseeding until duplicates), and reverse-like patterns (stochastic with seeds).
* Verified that **sequential** and **parallel** outputs match for the same `(n, seed)`.

**Performance**

* Compared timing of `cutoff = 0` vs `cutoff > 0` on the same machine (release build via `Makefile`).
* Observed the expected trend: increasing `cutoff` improves runtime up to a point (CPU saturation and thread overhead limit gains).
* For large `n` (e.g., `100,000,000`), parallel runs target ≥ **2× speedup** vs. sequential (as per assignment requirement; actual numbers depend on CPU cores/clock/memory bandwidth).

**Robustness**

* Included a **fallback**: if `pthread_create` fails, that branch sorts sequentially to remain correct, then continues.

## Known Bugs
* `printA()`/`printB()` in `test-mergesort.c` assume arrays have at least 100 elements (commented as `FIXME`). We do not use these in normal runs, but printing with very small `n` would be unsafe without changing those helpers.
* Extremely constrained environments (very low RAM) may not handle `n = 100,000,000` due to memory for `A` and `B`.


**What we built & why it works**

* Designed a **binary tree** of work: each node sorts a subarray; children complete before the parent merges.
* Used **`pthread_join`** to enforce ordering; thus, no races during merge and no need for locks.
* Globals `A`, `B`, and `cutoff` are simple and effective for this assignment’s API and testing harness.

**Challenges & fixes**

* Getting the split/indices right (`mid`, inclusive ranges) to avoid off-by-one errors.
* Ensuring argument lifetime is correct: `buildArgs` allocates on the heap; we free those structs after the threads finish.
* Handling thread creation failure safely; we added a sequential fallback and appropriate joins where needed.

**What clicked**

* The idea of **indirect recursion** in pthreads: you don’t call `parallel_mergesort` directly; you spawn a thread whose start routine is `parallel_mergesort`.
* Correctness by **designing disjoint write sets** and deferring merges until after joins—no mutexes necessary.

**Process**

* Iterated from a working sequential `my_mergesort`, then layered parallelism and validated progressively (small → large inputs).
* Used the harness timing to reason about useful `cutoff` values relative to available cores.

---

## Sources Used

* Course lectures and textbook chapters on threads and the Pthreads API.
* Linux man pages: `pthread_create(3)`, `pthread_join(3)`, `memcpy(3)`, `malloc(3)`, `free(3)`.
* Assignment specification and starter code comments.
