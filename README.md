# ecclib
Efficient library for cryptographic operations on elliptic curves.

## Target platforms
Present library is designed to allow deploying on 8, 16, 32 and 64 bit platforms. User have to define word size in "Globals.h" accordingly. The source code is in C language to allow deploying on the largest devices spectrum possible.

## Architecture
Present library is organized by hierarchic components, or groups of functions. Functions at level _i_ are algorithms built exploiting ones at _i-1_ level, and are building blocks for _i+1_ level algorithms. List of components:

1. Multiprecision positive integer arithmetic functions (under completion)
2. Prime field arithmetic functions (under completion)
3. Elliptic curve arithmetic functions (to be defined)
4. Elliptic curve-based protocols (to be defined)

## Tests
The `test` folder shows C examples for testing the correctness of functions, and Wolfram Mathematica(R) codes allowing test vectors generation.

## References
+ A.J. Menezes, P.C. van Oorschot, S.A. Vanstone. Handbook of Applied Cryptography. Taylor & Francis, 2010.
+ D. Hankerson, A.J. Menezes, S.A. Vanstone. Guide to Elliptic Curve Cryptography. Springer, 2004.
+ D.E. Knuth, The Art of Computer Programming – Seminumerical Algorithms, volume 2, Addison-Wesley, Reading, Massachusetts, 2nd edition, 1981.
+ W. Hasenplaugh, G. Gaubatz, V. Gopal. Fast Modular Reduction. Proceedings of ARITH'07, 2007.
+ NIST FIPS 186-4, available at http://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.186-4.pdf
+ Random number generator, available at https://www.random.org/bytes/
