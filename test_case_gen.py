# ==================================================
# RISC-V Audiomark - Coding Challenge
# ==================================================

# ==================================================
#  File:        test_case_gen.py
#  Author:      ishant615
#  Date:        date
# ==================================================

import random
import sys

# Test case generator function
def gen(n, filename):

    # random generation of alpha, a, b in 16-bit integer range
    alpha = random.randint(-32768, 32767)
    a = [random.randint(-32768, 32767) for _ in range(n)]
    b = [random.randint(-32768, 32767) for _ in range(n)]

    # Writing the test cases to file "input.txt"
    with open(filename, "a") as f:
        f.write(f"{n} {alpha}\n")
        f.write(" ".join(map(str, a)) + "\n")
        f.write(" ".join(map(str, b)) + "\n\n")

if __name__ == "__main__":

    outfile = "input.txt"
    t = int(sys.argv[1])    # Number of test cases required

    with open(outfile, "w") as f:
        f.write(f"{t}\n\n")

    while (t):
        # Range of vector length used here is (1, 16384)
        # This is just an example. The maximum vector length depends on MAX_N in main.c
        n = random.randint(1, 16384)     
        gen(n, outfile)
        t = t - 1
