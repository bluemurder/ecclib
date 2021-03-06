/***********************************************************************\
* ecclib - Efficient library for elliptic curve cryptography            *
* Copyright (C) 2015  Alessio Leoncini                                  *
*                                                                       *
* This program is free software: you can redistribute it and/or modify  *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* This program is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program.  If not, see <http://www.gnu.org/licenses/>. *
\***********************************************************************/

/*!
* \file MultiprecisionArithmetic.h
* Header file for multiple precision positive integer arithmetic functions.
*
* References:
* [1] D.E. Knuth, The Art of Computer Programming � Seminumerical Algorithms,
* volume 2, Addison-Wesley, Reading, Massachusetts, 2nd edition, 1981.
* [2] D. Hankerson, S. Vanstone, and A.J. Menezes. Guide to Elliptic Curve
* Cryptography. Springer Professional Computing. Springer, 2004.
* [3] D. Harvey, and P. Zimmermann. Short division of long integers. Computer
* Arithmetic (ARITH), 2011 20th IEEE Symposium on. IEEE, 2011.
*/

#ifndef MULTIPRECISIONARITHMETICINT_H
#define MULTIPRECISIONARITHMETICINT_H

#include "Globals.h"

//! Generic multiple precision number structure
struct _mpnumber
{
	chunk * data;
	unsigned int size;
};

//! Generic multiple precision number type
typedef struct _mpnumber mpnumber;

//! Writes the hexdecimal value from hexString to element, strictly following 
//! the specified bitsize
//! \param hexString The hexdecimal value to write (MSB letf)
//! \param chunksNumber The number of chunks already allocated
//! \param bitSize The size in bits of the underlying field
//! \param number The multiple precision number to fill
void SetString(
	mpnumber * number,
	char * hexString,
	unsigned int chunksNumber,
	unsigned int bitSize);

//! Give a string containing the hexadecimal representation of the number.
//! This is not an efficient implementation, use only for debug purposes.
//! \param chunksNumber The number of chunks of given prime field element
//! \param bitSize The size in bits of prime field element
//! \param element The prime field element
//! \returns The hexadecimal representation string, remember to free such
//! memory
char * GetString(
	unsigned int chunksNumber,
	unsigned int bitSize,
	mpnumber * number);

//! Sets the specified value expressed via a hex string in the number.
//! \param number The target element to write
//! \param hexString The string value to write
//! \param bitSize Size in bits of number
void InitNumberByString(
	mpnumber * number,
	char * hexString,
	unsigned int bitSize);

//! Initialize memory of specified number.
//! \param number The target element to write
//! \param chunksNumber Chunks number
void InitNumber(mpnumber * number, unsigned int chunksNumber);

//! Erase the memory of internal members of number
void FreeNumber(mpnumber * number);

//! Sums two chunks with carry
void ChunksAddition(
	chunk * result,
	unsigned int * carry,
	chunk a,
	chunk b,
	unsigned int carryin);

//! Subtract two chunks with borrow
void ChunksSubtraction(
	chunk * result,
	unsigned int * borrow,
	chunk a,
	chunk b,
	unsigned int borrowin);

//! Checks if a is equal to b
//! \returns 1 if a is equal to b, 0 otherwise
unsigned int MPEquals(mpnumber * a, mpnumber * b);

//! Integer multiplication, product scanning form
//! algorithm 2.10 [2]
void MPIntegerMul(mpnumber * mul, mpnumber * a, mpnumber * b);

//! Left shift of multiple precision number, with specified number of shifts. 
//! Caller must assure the correct size of the result is already allocated. 
void MPLeftShift(mpnumber * res, mpnumber * a, unsigned int shifts);

//! Long division algorithm [1], computing the quotient of u/v with the 
//! corresponding remainder. Note: operands and results must be already 
//! allocated by the caller, with the following sizes, and m1 >= n1.
//! \param div The quotient, of chunk size = m1
//! \param rem The remainder, with chunk size = n1
//! \param u The dividend, of chunk size = m1
//! \param v The divisor, of chunk size = n1
void LongDivision(mpnumber * div, mpnumber * rem, mpnumber * u, mpnumber * v);

//! Short division algorithm inspired by an implementation of B.S. Kaliski Jr.
//! It works only for 2-word dividend, 1-word divisor and 1-word quotient.
void ChunksDivisionSingleDivisor(chunk * div, chunk * a, chunk b);

#endif // MULTIPRECISIONARITHMETICINT_H