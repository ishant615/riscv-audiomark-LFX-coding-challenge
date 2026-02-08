// ==================================================
// RISC-V Audiomark - Coding Challenge
// ==================================================

// ==================================================
// File: q15_axpy.h
// Author: ishant615
// Date: Feb 8, 2026
// 
// Description: Header file for functions 
//              Implemented in q15_axpy.c
// ==================================================

#ifndef Q15_AXPY_H
#define Q15_AXPY_H

#include <stdint.h>

// Function definitions (implemented in "q15_axpy.c")
void q15_axpy_ref(const int16_t *a, const int16_t *b, int16_t *y, int n, int16_t alpha);
void q15_axpy_rvv(const int16_t *a, const int16_t *b, int16_t *y, int n, int16_t alpha);

#endif