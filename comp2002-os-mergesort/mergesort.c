/**
 * This file implements parallel mergesort.
 */

#include <stdio.h>
#include <string.h> /* for memcpy */
#include <stdlib.h> /* for malloc */
#include "mergesort.h"

/* this function will be called by mergesort() and also by parallel_mergesort(). */
void merge(int leftstart, int leftend, int rightstart, int rightend){
}

/* this function will be called by parallel_mergesort() as its base case. */
void my_mergesort(int left, int right){
	// base case 
	if(left >= right) return;

	// recursive case (divide and conquer standard mergesort)
	int mid = (left + right) / 2;				// Find the mid point
	my_mergesort(left, mid);					// Sort the left half
	my_mergesort(mid + 1, right);				// Sort the right half
	merge(left, mid, mid + 1, right);			// Merge the two halves
}

/* this function will be called by the testing program. */
void * parallel_mergesort(void *arg){
		return NULL;
}

/* we build the argument for the parallel_mergesort function. */
struct argument * buildArgs(int left, int right, int level){
		return NULL;
}

