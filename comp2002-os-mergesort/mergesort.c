/**
 * This file implements parallel mergesort.
 */

#include <stdio.h>
#include <string.h> /* for memcpy */
#include <stdlib.h> /* for malloc */
#include "mergesort.h"

/* this function will be called by mergesort() and also by parallel_mergesort(). */
void merge(int leftstart, int leftend, int rightstart, int rightend){
	// merge 2 sorted subarrays A[leftstart..leftend] and A[rightstart..rightend] into B[]
	int l = leftstart;				// index for the left subarray
	int r = rightstart;				// index for the right subarray
	int B_index = leftstart;		// index for the merged subarray B[]

	// merge the two subarrays into B[]
	while (l <= leftend && r <= rightend) {
		// Insert the smaller element between the 2 subarr into B[]
		if (A[l] <= A[r]) {
			B[B_index] = A[l];
			l++;
			B_index++;
		} else {
			B[B_index] = A[r];	
			r++;
			B_index++;
		}
	}

	// Clean up the remaining elements of the left subarray if any
	while (l <= leftend) {
		B[B_index] = A[l];
		l++;
		B_index++;		
	}

	// Clean up the remaining elements of the right subarray if any
	while (r <= rightend) {
		B[B_index] = A[r];
		r++;
		B_index++;
	}

	// Copy the merged subarray B[] back to A[]
	memcpy(&A[leftstart], &B[leftstart], (rightend - leftstart + 1) * sizeof(int));
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
	// cast the argument to the correct type
	struct argument *args = (struct argument *) arg;
	int left = args->left;
	int right = args->right;
	int level = args->level;
	
	// base case: too snall subarray, use standard mergesort
	if(left >= right) {
		return NULL;
	}

	// If we have reached the cutoff level, use standard mergesort
	if(level >= cutoff) {
		my_mergesort(left, right);
		return NULL;
	}

	// recursive case: keep creating threads to sort the left and right halves
	int mid = (left + right) / 2;											// Find the mid point	
	pthread_t left_thread, right_thread;									// Thread identifiers 
	struct argument *left_args = buildArgs(left, mid, level + 1);			// Arguments for the left half
	struct argument *right_args = buildArgs(mid + 1, right, level + 1);		// Arguments for the right half

	// create threads to sort the left
	if (pthread_create(&left_thread, NULL, parallel_mergesort, left_args) != 0) {
		perror("Failed to create left thread");
		// Fall-back to sequential sort if thread creation fails
		my_mergesort(left, mid);
		free(right_args);
		return NULL;
	}

	// create threads to sort the right
	if (pthread_create(&right_thread, NULL, parallel_mergesort, right_args) != 0) {
		perror("Failed to create right thread");
		
		// if the code reached this point, it means the left thread was created successfully
		// so we need to wait for it to finish before falling back to sequential sort
		pthread_join(left_thread, NULL);
		my_mergesort(mid + 1, right);
		free(left_args);
		return NULL;
	}
	
	// parent thread waits for both child threads to finish
	pthread_join(left_thread, NULL);
	pthread_join(right_thread, NULL);

	// merge the two sorted halves
	merge(left, mid, mid + 1, right);

	
	// free the dynamically allocated arguments
	free(left_args);
	free(right_args);

	return NULL;
}

/* we build the argument for the parallel_mergesort function. */
struct argument * buildArgs(int left, int right, int level){
	// Allocate memory for the struct argument for each thread
	struct argument *arg = (struct argument *) malloc(sizeof(struct argument));
	
	// Assign values to the struct members
	arg->left = left;
	arg->right = right;
	arg->level = level;
	
	// Return the pointer to the struct argument 
	return arg;
}
