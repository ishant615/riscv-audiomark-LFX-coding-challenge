# Risc-V Vector-Accelerated Q15 AXPY (Saturating Multiply-Accumulate)

This repository contains a RISC-V Vector (RVV) accelerated implementation of a **Q15 saturating AXPY kernel**:

y [ i ]  =  sat_q15 ( a [ i ] + α · b [ i ] )

where:

* a [ i ], b [ i ], y [ i ], and α are signed 16-bit integers
* Intermediate computation is performed in 32-bit precision
* Results are saturated to the Q15 range [ -32768,  32767 ]

The implementation is:

* **<u>Vector-length agnostic (VLA)</u>**: The RVV implementation is written using a vector-length agnostic loop structure based on vsetvl. Rather than assuming a fixed vector width, the kernel dynamically adapts to the hardware’s maximum supported vector length at runtime.
* **<u>Compatible with RV32 or RV64</u>**
* **<u>Verified against a scalar reference</u>**
* **<u>Contains scalar and RVV implementations in the same codebase</u>**

## Project Structure

```
.
├── Makefile            # Build, run, and timing automation
├── README.md           # Project documentation
├── q15_axpy.c          # Scalar reference + RVV implementation
├── q15_axpy.h          # Function definitions (implemented in q15_axpy.c)
├── main.c              # Test harness, verification, file I/O
├── gen_tests.py        # Random test-case generator
├── input.txt           # Generated input data
├── output.txt          # Kernel output
└── timing.txt          # Wall-clock timing results (make time)
```

## Implementations

### Scalar Reference

Straightforward C implementation using 32-bit accumulation and explicit saturation.
Used as the golden reference.

### RVV Optimized Kernel

Uses RISC-V Vector intrinsics:

* Vector loads/stores (vle16, vse16)
* Widening multiply (vwmul)
* Widening convert (vwcvt)
* Vector add (vadd)
* Vector compares for saturation
* Masked merge to clamp results

## Portability and Fallback

The function:

void q15_axpy_rvv(...)

internally looks for the definition of RISC-V macros. If the macros are not found, the function falls back to the scalar implementation 
and calls the function: 

void q15_axpy_ref(...)

and if the macros are found the parallelized vector implementation takes place

So:

* On non-RISC-V → scalar runs
* On RISC-V with RVV → vector runs

## Requirements

* RVV-enabled RISC-V GCC
* Spike simulator
* Proxy kernel (pk)
* Python 3

## Build

### make build

Builds:

* scalar.elf
* rvv.elf

using the files main.c and q15_axpy.c

### make run

Program runs the .elf files with and without the vector extension enabled and prints PASS or FAIL for each test case (PASS if the RVV output matches the golden reference) while writing both the outputs to "output.txt".

### make time

Builds and runs the files and uses "/usr/bin/time" to time the scalar and RVV implementations, while writing the timings to timing.txt.

### make clean

Cleans the slate for a new run by removing "scalar.elf", "rvv.elf", and "timing.txt".

## Timing

Accurate timing analysis of the vector implementation could not be done using Spike with proxy kernel since Spike does not simulate the actual processing. It translates the RVV instructions into scalar instructions in order to process on the host CPU which actually shows a performance drop while using vector intrinsics. The theoretical speed-up calculations have been given in the **Performance Estimation** section. 

## Performance Estimation

Let:

VLEN = vector register width (bits)
Element width = 16 bits

VL = VLEN / 16

Example:

VLEN = 256 → VL = 16 elements

Scalar processes 1 element per iteration.
RVV processes VL elements per iteration.

Ideal throughput speedup ≈ VL×

For the simulated environment (Spike + PK) the value of VLEN is 128, thus, VL comes out to be 8.

Thus, a speed-up of about 8× is expected on the use of this code.

## Simplicity and Clarity over Micro-Optimization

The implementation favors clarity and correctness over aggressive micro-optimizations such as:

* Manual unrolling
* Complex LMUL scaling
* Architecture-specific scheduling assumptions

### Program Workflow 

1. Compute scalar reference
2. Compute RVV output
3. Compare element-wise
4. Report difference array

Kernel must be bit-identical.

## Design Choices

* **<u>Widening arithmetic to avoid overflow</u>**: All arithmetic is performed in 32-bit precision before narrowing the final result back to 16 bits.
* **<u>Mask-based saturation</u>**: Saturation is implemented explicitly using vector comparisons and masks:
* **<u>Vector-length agnostic loops</u>**
* **<u>Separate reference function</u>**: The scalar reference implementation is kept separate and unmodified, serving as a golden model for straightforward verification and clear correctness comparison.

## Author

Ishant Arora (ishant615)
