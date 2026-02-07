// ==================================================
// RISC-V Audiomark - Coding Challenge
// ==================================================

// ================================================================
//  File:        q15_axpy_rvv.c
//  Author:      ishant615
//  Date:        date
//  Description:
//  ------------
//  RISC-V Vector (RVV) accelerated implementation of a saturating
//  AXPY kernel operating on 16-bit signed integers.
//      y[i] = sat_q15( a[i] + alpha * b[i] )
//  where:
//    - a[i], b[i], y[i], alpha are int16_t
//    - intermediate computation is performed in int32_t
//    - final result is saturated to the range [-32768, 32767]
//  The implementation is:
//    - Vector-length agnostic (VLA)
//    - Compatible with RV32 and RV64
//    - Based on RVV intrinsics (v1.0)
// ================================================================

// Including standard header files
#include <stdio.h>
#include <stdint.h>

// Some older toolchains have (__riscv_v_intrinsic = 12000)
// Newer toolchains have (__riscv_v_intrinsic = 1000000)
// Using the condition (__riscv_v_intrinsic >= 1000000) does not work with the older toolchains as the 
// file (riscv_vector.h) is not included
// Use of defined() eliminates the compatibility issues when running on slightly older toolchains 
#if defined(__riscv_v_intrinsic)
    #include <riscv_vector.h>
#endif

// Reference function (for verification)
void q15_axpy_ref(int16_t *a, int16_t *b, int16_t *y, int n, int16_t alpha){

    int32_t d[n];

    for(int i = 0; i < n; i++){

        d[i] = (alpha * a[i]) + b[i];   // Using temporary 32-bit integer array 'temp'

        if(d[i] >= 32767) d[i] = 32767;
        if(d[i] <= -32768) d[i] = -32768;

        y[i] = (int16_t)d[i];
    }
}

// TODO
// This function is the main deliverable of the coding challenge
void q15_axpy_rvv(int16_t *a, int16_t *b, int16_t *y, int n, int16_t alpha){

    #if !defined(__riscv) || !defined(__riscv_vector) || !defined(__riscv_v_intrinsic)

        // Fallback
        // For correct functioning when not running on a RISC-V core

        q15_axpy_ref(a, b, y, n, alpha);

    #else

        // Vectorization for parallellism on a RISC-V core

        size_t s = n;   // Preserving the original size variable (n)

        // Repeating until all the elements have been processed
        while(s > 0){

            // Getting vl using s
            size_t vl = __riscv_vsetvl_e32m2(s);

            // Initializing the vectors and loading elements from memory to the 
            // vector registers
            vint16m1_t va = __riscv_vle16_v_i16m1(a, vl);
            vint16m1_t vb = __riscv_vle16_v_i16m1(b, vl);

            // Scalar multiplication with widening (16-bit --> 32-bit)
            vint32m2_t vprod = __riscv_vwmul_vx_i32m2(va, alpha, vl);

            // Widening vb (16-bit --> 32-bit) for addition to vprod
            vint32m2_t vb32 = __riscv_vwcvt_x_x_v_i32m2(vb, vl);

            // Vector addition 
            vint32m2_t vacc32  = __riscv_vadd_vv_i32m2(vprod, vb32, vl);

            // Defining mask vectors for saturation
            vbool16_t m_hi = __riscv_vmsgt_vx_i32m2_b16(vacc32, 32767, vl);     // 16-bit signed upper bound
            vbool16_t m_lo = __riscv_vmslt_vx_i32m2_b16(vacc32, -32768, vl);    // 16-bit signed lower bound

            // Applying saturation mask
            vacc32 = __riscv_vmerge_vxm_i32m2(vacc32, 32767, m_hi, vl);
            vacc32 = __riscv_vmerge_vxm_i32m2(vacc32, -32768, m_lo, vl);

            // Truncating to 16-bit vector 
            vint16m1_t vy = __riscv_vncvt_x_x_w_i16m1(vacc32, vl);

            // Storing vector from vector registers into the memory 
            // at the given location
            __riscv_vse16_v_i16m1(y, vy, vl);

            // Updating pointers
            s -= vl;
            a += vl;
            b += vl;
            y += vl;
        }

    #endif
}

