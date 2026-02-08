# ==================================================
# RISC-V Audiomark - Coding Challenge
# ==================================================

# ==================================================
# File: Makefile
# Author: ishant615
# Date: Feb 8, 2026
# 
# Description: Compile, run and time system for 
# 			   main.c and q15_axpy.c
# ==================================================

# Compiler
GCC64 = riscv64-unknown-elf-gcc

# ISA
SCALAR_ARCH = rv64gc
RVV_ARCH = rv64gcv

# Flags
CFLAGS_SCALAR = -march=$(SCALAR_ARCH) -O2 -Wall
CFLAGS_RVV = -march=$(RVV_ARCH) -O2 -Wall

# Source
SRCS = q15_axpy.c main.c

# ==================================================
# Build Targets
# ==================================================

build-scalar:
	$(GCC64) $(CFLAGS_SCALAR) $(SRCS) -o scalar.elf

build-rvv:
	$(GCC64) $(CFLAGS_RVV) $(SRCS) -o rvv.elf

build: build-scalar build-rvv

# ==================================================
# Run Targets
# ==================================================

run-scalar: scalar
	spike pk scalar.elf

run-rvv: rvv
	spike --isa=$(RVV_ARCH) pk rvv.elf

run: run-scalar run-rvv

# ==================================================
# Timing (Using Linux 'time')
# ==================================================

time: build
	@echo "Scalar Timing:"  >  timing.txt
	@echo "Scalar Implementation"
	@/usr/bin/time -p spike pk scalar.elf 2>> timing.txt
	@echo "" >> timing.txt
	@echo "RVV Timing:" >> timing.txt
	@echo "RVV Implementation"
	@/usr/bin/time -p spike --isa=$(RVV_ARCH) pk rvv.elf 2>> timing.txt
	@echo "Timings written to timing.txt"

# ==================================================
# Clean
# ==================================================

clean:
	rm -f scalar.elf rvv.elf timing.txt output.txt

.PHONY: scalar rvv all run-scalar run-rvv time clean
