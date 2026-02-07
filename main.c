
// Including standard header files
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Header file containing reference and target function definitions
#include "q15_axpy.h"

// Maximum vector length. This is just an example. 
// The actual maximum value depends on the size of BSS memory, since the initialization is global static.
#define MAX_N 16384  

// Defining program result
#define PASS 0
#define FAIL 1
  
// Memory allocation in BSS to preserve heap memory for program ececution
static int16_t a[MAX_N];
static int16_t b[MAX_N];
static int16_t yref[MAX_N];
static int16_t yrvv[MAX_N];

// Verification helper
static int8_t verify_equal(const int16_t *ref, const int16_t *test, int n, int32_t *diff){
    
    int ok = 1;

    for(int i = 0; i < n; i++){

        int32_t d = (int32_t)ref[i] - (int32_t)test[i];

        // If there is a difference between 'yref' and 'yrvv' something is wrong.
        if(d) ok = 0;

        // diff[i] = abs(yref[i] - yrvv[i])
        diff[i] = d;
    }

    return ok;
}

// ================================================
// Main
// ================================================

int main(void){

    int result = PASS;

    FILE *fin = fopen("input.txt", "r");

    if (!fin){

        printf("ERROR: Could not open input.txt\n");
        return 1;
    }

    FILE *fout = fopen("output.txt", "w");

    if(!fout) printf("WARNING: Could not open \"output.txt\" for writing. Outputs will not be logged.\n");

    int16_t t = 0;
    int16_t testcase = 0;

    fscanf(fin, "%hd", &t);

    while(testcase++ < t){

        int n;
        int16_t alpha;

        // Reading input from file "input.txt"

        // Reading vector length 'n' and scalar 'alpha'
        fscanf(fin, "%d %hd", &n, &alpha);

        // Reading first array 'a'
        for(int i = 0; i < n; i++){
            
            fscanf(fin, "%hd", &a[i]);
        }

        // Reading second array 'b'
        for(int i = 0; i < n; i++){
            
            fscanf(fin, "%hd", &b[i]);
        }

        // Run reference
        q15_axpy_ref(a, b, yref, n, alpha);

        // Run target (RVV)
        q15_axpy_rvv(a, b, yrvv, n, alpha);

        // Writing output to file "output.txt" for easy debugging
        if(fout){

            fprintf(fout, "Test Case: %d\n", testcase);
            
            // Writing reference output
            for (int i = 0; i < n; i++){

                fprintf(fout, "%hd ", yref[i]);
            }
            fprintf(fout, "\n");

            // Writing vector implementation output
            for (int i = 0; i < n; i++){

                fprintf(fout, "%hd ", yrvv[i]);
            }
            fprintf(fout, "\n\n");
        }

        // Verify
        int32_t diff[n];      // diff[i] = abs(yrvv[i] - yref[i]) For easy error lookup and debugging
        int8_t ok = verify_equal(yref, yrvv, n, diff);

        // Result for the test case
        if(ok) printf("Test Case %d: PASS\n", testcase);

        else {

            // Calculating maximum difference (adhering to boiler plate standard)
            int32_t max_diff = 0;

            for(int i = 0; i < n; i++){

                max_diff = (diff[i] > max_diff) ? diff[i]:max_diff;
            }

            printf("Test Case %d: FAIL\n", testcase);

            // Full implementation result (FAIL if one or more test cases fail)
            result = FAIL;
        }

        // Printing 'diff' for debugging purposes
        // for(int i = 0; i < n; i++){

        //     printf("%d  %d\n", i + 1, diff[i]);
        // }
    }

    return result;     // Full implementation result
}
