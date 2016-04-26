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

//! \file MultiprecisionArithmetic.c 

#include "../include/MultiprecisionArithmeticInt.h"
#include <malloc.h>
#include <string.h>

void SetString(
	mpnumber * number,
	char * hexString,
	unsigned int chunksNumber,
	unsigned int bitSize)
{
	size_t hexStringLen = strlen(hexString);
	char * stringCursor = hexString + hexStringLen - 1;
	size_t i, j;
	unsigned int maxChunkCharacters = ARCHITECTURE_BITS / 4;
	chunk value;
	unsigned char readstring = 1;
	if (hexStringLen == 0)
	{
		readstring = 0;
	}
	size_t charactersCounter = 0;
	size_t bitsCounter = 0;
	// For every chunk
	for (i = 0; i < chunksNumber; i++)
	{
		number->data[i] = 0;
		for (
			j = 0;
			(j < maxChunkCharacters && readstring == 1);
		j++, charactersCounter++, bitsCounter += 4)
		{
			// Numbers
			if (*stringCursor > 47 && *stringCursor < 58)
			{
				value = *stringCursor - 48;
			}
			// Uppercase letters
			else if (*stringCursor > 64 && *stringCursor < 71)
			{
				value = *stringCursor - 65 + 10;
			}
			// lowercase letters
			else if (*stringCursor > 96 && *stringCursor < 103)
			{
				value = *stringCursor - 97 + 10;
			}
			// If declared input bits does not coner entirely current chunk:
			if (bitsCounter + 4 > bitSize)
			{
				// Apply proper bitmask
				size_t difference = bitsCounter + 4 - bitSize;
				if (difference == 4)
				{
					value = 0;
				}
				else if (difference == 3)
				{
					value = value & 0x1;
				}
				else if (difference == 2)
				{
					value = value & 0x3;
				}
				else if (difference == 1)
				{
					value = value & 0x7;
				}
				// exit
				readstring = 0;
			}
			// Shift when reading higher chunk bits
			value = value << (4 * j);
			// Sum to current chunk integer
			number->data[i] += value;
			// If input string with no more characters, exit
			if (charactersCounter + 1 == hexStringLen)
			{
				readstring = 0;
			}
			// Read next string character
			stringCursor--;
		}
	}
}

char * GetString(
	unsigned int chunksNumber,
	unsigned int bitSize,
	mpnumber * number)
{
	unsigned int characters = bitSize / 4;
	if (bitSize % 4)
	{
		characters++;
	}
	unsigned int chunkCharacters = ARCHITECTURE_BITS / 4;
	// Allocation of string
	char * hexdump = (char *)malloc(characters * sizeof(char) + 1);
	// Null termination
	hexdump[characters] = 0;
	unsigned int charCounter = 0;
	chunk chunk;
	for (unsigned int c = 0; c < chunksNumber; c++)
	{
		for (unsigned int i = 0; i < chunkCharacters; i++, charCounter++)
		{
			if (charCounter == characters)
			{
				break;
			}
			//chunk = data[c] & (0x0f << (4 * i));
			//chunk = chunk >> (4 * i);
			chunk = number->data[c] >> (4 * i);
			chunk = chunk & 0x0f;
			if (chunk < 10)
			{
				hexdump[characters - 1 - charCounter] = (char)chunk + 48;
			}
			else
			{
				hexdump[characters - 1 - charCounter] = (char)chunk + 65 - 10;
			}
		}
	}
	return hexdump;
}

void InitNumberByString(
	mpnumber * number,
	char * hexString,
	unsigned int bitSize)
{
	// Evaluate chunks number
	number->size = bitSize / ARCHITECTURE_BITS;
	if (bitSize % ARCHITECTURE_BITS)
	{
		number->size++;
	}
	// Allocate memory
	number->data = (chunk *)malloc(number->size * (sizeof(chunk)));
	// Write value
	SetString(number, hexString, number->size, bitSize);
}

void InitNumber(mpnumber * number, unsigned int chunksNumber)
{
	// Write chunks number
	number->size = chunksNumber;
	// Allocate memory
	number->data = (chunk *)malloc(number->size * (sizeof(chunk)));
}

void FreeNumber(mpnumber * number)
{
	free(number->data);
}

void LongDivision(mpnumber * div, mpnumber * rem, mpnumber * u, mpnumber * v)
{
#if ARCHITECTURE_BITS == 8
	chunk b = 0x10;
	chunk lowHalfMask = 0x0f;
	chunk highHalfMask = 0xf0;
	unsigned int halfBits = 4;
#elif ARCHITECTURE_BITS == 16
	chunk b = 0x100;
	chunk lowHalfMask = 0x0ff;
	chunk highHalfMask = 0xff00;
	unsigned int halfBits = 8;
#elif ARCHITECTURE_BITS == 64
	chunk b = 0x100000000;
	chunk lowHalfMask = 0x0ffffffff;
	chunk highHalfMask = 0xffffffff00000000;
	unsigned int halfBits = 32;
#else
	chunk b = 0x10000;
	chunk lowHalfMask = 0x0ffff;
	chunk highHalfMask = 0xffff0000;
	unsigned int halfBits = 16;
#endif
	// To allow the computation of word-by-word operations, the operands will
	// be stored into multiprecision numbers with a double chunk size; every 
	// chunk will contain a half chunk of the original operand.

	// n1 is the number of not null chunks of v
	unsigned int i, j, m1, n1;
	for (i = v->size - 1;; i--)
	{
		if (i == 0 || v->data[i] != 0)
		{
			break;
		}
	}
	n1 = i + 1;

	// m1 is the number of not null chunks of u
	for (i = u->size - 1;; i--)
	{
		if (i == 0 || u->data[i] != 0)
		{
			break;
		}
	}
	m1 = i + 1;

	// Allocation of space for all the halfwords
	mpnumber doubledu, doubledv, doubleddiv, doubledrem;
	unsigned int doubledusize, doubledvsize;
	doubledusize = m1 << 1;
	doubledvsize = n1 << 1;
	InitNumber(&doubledu, doubledusize);
	InitNumber(&doubledv, doubledvsize);
	InitNumber(&doubleddiv, doubledusize);
	InitNumber(&doubledrem, doubledvsize);

	// Reset results memory (div->size >= rem->size)
	for (i = 0; i < rem->size; i++)
	{
		rem->data[i] = 0;
		div->data[i] = 0;
	}
	for (; i < div->size; i++)
	{
		div->data[i] = 0;
	}

	// Reset temporary results memory (doubledusize >= doubledvsize)
	for (i = 0; i < doubledvsize; i++)
	{
		doubledrem.data[i] = 0;
		doubleddiv.data[i] = 0;
	}
	for (; i < doubledusize; i++)
	{
		doubleddiv.data[i] = 0;
	}

	// Copy operands on doubled size buffers
	for (i = 0; i < n1; i++)
	{
		doubledv.data[i << 1] = v->data[i] & lowHalfMask;
		doubledv.data[(i << 1) + 1] = (v->data[i] & highHalfMask) >> halfBits;
		doubledu.data[i << 1] = u->data[i] & lowHalfMask;
		doubledu.data[(i << 1) + 1] = (u->data[i] & highHalfMask) >> halfBits;
	}
	for (; i < m1; i++)
	{
		doubledu.data[i << 1] = u->data[i] & lowHalfMask;
		doubledu.data[(i << 1) + 1] = (u->data[i] & highHalfMask) >> halfBits;
	}

	// n is the number of not null chunks of doubledv
	unsigned int m, n;
	for (i = doubledv.size - 1;; i--)
	{
		if (i == 0 || doubledv.data[i] != 0)
		{
			break;
		}
	}
	n = i + 1;

	// m is the number of not null chunks of doubledu
	for (i = doubledu.size - 1;; i--)
	{
		if (i == 0 || doubledu.data[i] != 0)
		{
			break;
		}
	}
	m = i + 1;

	// Take care of the case of a single-digit divisor here.
	if (n == 1)
	{
		chunk k = 0;
		for (j = m - 1;; j--)
		{
			doubleddiv.data[j] = (k * b + doubledu.data[j]) /
				doubledv.data[0];
			k = (k * b + doubledu.data[j]) -
				doubleddiv.data[j] * doubledv.data[0];
			if (j == 0)
			{
				break;
			}
		}
		rem->data[0] = k;
		for (i = 0; i < doubleddiv.size; i++)
		{
			if (i & 1) // odd i
			{
				div->data[(i - 1) >> 1] |= (doubleddiv.data[i]) << halfBits;
			}
			else  // even i
			{
				div->data[i >> 1] = doubleddiv.data[i];
			}
		}
		FreeNumber(&doubledu);
		FreeNumber(&doubledv);
		FreeNumber(&doubleddiv);
		FreeNumber(&doubledrem);
		return;
	}



	// D1. Normalize

	// Given a base b, the number u is represented as (u_{n-1},...,u_1,u_0)b. 
	// Normalization factor d must satisfy u_{n-1} >= floor(b/2)
	mpnumber normalizedu;
	InitNumber(&normalizedu, m + 1);
#if ARCHITECTURE_BITS == 8
	chunk halfword = 0x10;
#elif ARCHITECTURE_BITS == 16
	chunk halfword = 0x0100;
#elif ARCHITECTURE_BITS == 64
	chunk halfword = 0x0000000100000000;
#else
	chunk halfword = 0x00010000;
#endif
	// d is a power of 2 so only the number of left shifts, or log_2(d) is 
	// evaluated.
	unsigned int log2d = 0;
	while ((u->data[n - 1] << log2d) >= halfword)
	{
		log2d++;
	}
	// If d==1, normalization is skipped, normalizedu is set equal to u but 
	// with an additional word set to 0 on the left  
	if (log2d == 0)
	{
		for (i = 0; i < u->size; i++)
		{
			normalizedu.data[i] = u->data[i];
		}
		normalizedu.data[m] = 0;
	}
	else
	{
		MPLeftShift(&normalizedu, u, log2d);
	}

	// D2. Initialize j
	j = m;

	// D3. Calculate hat(q)

	// hat(q) = floor( (b*u_{j+n} + u_{j+n-1}) / u_{n-1} )
	mpnumber hatq;
	chunk hatqdata[2];
	hatq.data = hatqdata;
	hatq.size = 2;
	hatq.data[1] = normalizedu.data[j + n] / normalizedu.data[n - 1];
	hatq.data[0] = normalizedu.data[j + n - 1] / normalizedu.data[n - 1];
	mpnumber hatr;
	chunk hatrdata;
	hatr.data = &hatrdata;
	hatr.size = 1;
	hatr.data[0] = normalizedu.data[n - 1];
	//ShortDivision(&hatq, &hatr, )

	// hat(r) = remainder of division above
	// Test: if hat(q) == b or hat(q) > b*hat(r) + u_{j+n-2} :
	// - Decrease hat(q) by 1
	// - Increase hat(r) by u_{n-1}
	// If hat(r) < b, repeat the test

	// D4. Multiply and subtract

	// Replace (u_{j+n}, ..., u_{j}) with 
	// (u_{j+n}, ..., u_{j}) - hat(q)*(u_{n-1}, ..., u_0)

	// If result is negative, replace it with its b's complement, and save a 
	// borrow

	// D5. Test remainder

	// If borrow, 

	FreeNumber(&normalizedu);
	FreeNumber(&doubledu);
	FreeNumber(&doubledv);
	FreeNumber(&doubleddiv);
	FreeNumber(&doubledrem);
}



// TODO test again
void ChunksDivisionSingleDivisor(chunk * div, chunk * a, chunk b)
{
	chunk t[2], u, v;
	chunk resultHigh, resultLow, bHigh, bLow;
	chunk max, halfMax;
	unsigned int shiftBits;

#if ARCHITECTURE_BITS == 8
	max = 0xff;
	halfMax = 0xf;
	shiftBits = 4;
#elif ARCHITECTURE_BITS == 16
	max = 0xffff;
	halfMax = 0xff;
	shiftBits = 8;
#elif ARCHITECTURE_BITS == 64
	max = 0xffffffffffffffff;
	halfMax = 0xffffffff;
	shiftBits = 32;
#else
	max = 0xffffffff;
	halfMax = 0xffff;
	shiftBits = 16;
#endif

	bHigh = b >> shiftBits;
	bLow = b & halfMax;

	t[0] = a[0];
	t[1] = a[1];

	// Estimation of high result part
	if (bHigh == halfMax)
	{
		resultHigh = (t[1] >> shiftBits);
	}
	else
	{
		resultHigh = (t[1] / (bHigh + 1));
	}
	u = resultHigh * bLow;
	v = resultHigh * bHigh;
	if ((t[0] -= (u << shiftBits)) > (max - (u << shiftBits)))
	{
		t[1]--;
	}
	t[1] -= (u >> shiftBits);
	t[1] -= v;

	// Evaluation of high result part
	while ((t[1] > bHigh) ||
		((t[1] == bHigh) && (t[0] >= (bLow << shiftBits))))
	{
		if ((t[0] -= (bLow << shiftBits)) > max - (bLow << shiftBits))
		{
			t[1]--;
		}
		t[1] -= bHigh;
		resultHigh++;
	}

	// Estimation of low result part
	if (bHigh == halfMax)
	{
		resultLow = t[1] & halfMax;
	}
	else
	{
		resultLow = (((t[1] << shiftBits) + (t[0] >> shiftBits)) /
			(bHigh + 1));
	}
	u = resultLow * bLow;
	v = resultLow * bHigh;
	if ((t[0] -= u) > (max - u))
	{
		t[1]--;
	}
	if ((t[0] -= (v << shiftBits)) > (max - (v << shiftBits)))
	{
		t[1]--;
	}
	t[1] -= (v >> shiftBits);

	// Evaluation of low result part
	while ((t[1] > 0) || ((t[1] == 0) && t[0] >= b))
	{
		if ((t[0] -= b) > (max - b))
		{
			t[1]--;
		}
		resultLow++;
	}

	*div = (resultHigh << shiftBits) + resultLow;
}

void ChunksAddition(
	chunk * result,
	unsigned int * carry,
	chunk a,
	chunk b,
	unsigned int carryin)
{
	if (carryin)
	{
		*result = a + b + carryin;
		*carry = (a >= *result);
	}
	else
	{
		*result = a + b;
		*carry = (a > *result);
	}
}

void ChunksSubtraction(
	chunk * result,
	unsigned int * borrow,
	chunk a,
	chunk b,
	unsigned int borrowin)
{
	if (borrowin)
	{
		*result = a - b - borrowin;
		*borrow = (a <= *result);
	}
	else
	{
		*result = a - b;
		*borrow = (a < *result);
	}
}

unsigned int MPEquals(mpnumber * a, mpnumber * b)
{
	if (a->size != b->size)
	{
		return 0;
	}
	unsigned int i;
	for (i = 0; i < a->size; i++)
	{
		if (a->data[i] != b->data[i])
		{
			return 0;
		}
	}
	return 1;
}

void MPIntegerMul(mpnumber * mul, mpnumber * a, mpnumber * b)
{
	// Original code by Knuth refers to equal size operands (a and b with t 
	// number of chunks). This lead k to run from 0 to 2t-2.
	// Present code will read directly the result number of chunks. The caller
	// of MPIntegerMul function has to ensure that the result was allocated 
	// with a bitsize equal to the SUM of operands bitsizes.
	chunk a0, a1, b0, b1;
	chunk m0, m1, m2, m4;
	chunk V;
	// If result does not exceed a single chunk, only inner product is 
	// evaluated. Code is slightly optimized to produce a 1-chunk output.
	if (mul->size == 1)
	{
#if ARCHITECTURE_BITS == 8
		a0 = 0x0f & a->data[0];
		a1 = (0xf0 & a->data[0]) >> 4;
		b0 = 0x0f & b->data[0];
		b1 = (0xf0 & b->data[0]) >> 4;
		m0 = a0 * b0;
		m1 = a1 * b0;
		m2 = a0 * b1;
		//m3 = a1 * b1;
		V = (m0 & 0x0f);
		m4 = (m1 & 0x0f) + (m2 & 0x0f) + (m0 >> 4);
		V = V + (m4 << 4);
		//U = m3 + (m1 >> 4) + (m2 >> 4) + (m4 >> 4);
#elif ARCHITECTURE_BITS == 16
		a0 = 0x00ff & a->data[0];
		a1 = (0xff00 & a->data[0]) >> 8;
		b0 = 0x00ff & b->data[0];
		b1 = (0xff00 & b->data[0]) >> 8;
		m0 = a0 * b0;
		m1 = a1 * b0;
		m2 = a0 * b1;
		//m3 = a1 * b1;
		V = (m0 & 0x00ff);
		m4 = (m1 & 0x00ff) + (m2 & 0x00ff) + (m0 >> 8);
		V = V + (m4 << 8);
		//U = m3 + (m1 >> 8) + (m2 >> 8) + (m4 >> 8);
#elif ARCHITECTURE_BITS == 64
		a0 = 0x00000000ffffffff & a->data[0];
		a1 = (0xffffffff00000000 & a->data[0]) >> 32;
		b0 = 0x00000000ffffffff & b->data[0];
		b1 = (0xffffffff00000000 & b->data[0]) >> 32;
		m0 = a0 * b0;
		m1 = a1 * b0;
		m2 = a0 * b1;
		//m3 = a1 * b1;
		V = (m0 & 0x00000000ffffffff);
		m4 = (m1 & 0x00000000ffffffff) +
			(m2 & 0x00000000ffffffff) +
			(m0 >> 32);
		V = V + (m4 << 32);
		//U = m3 + (m1 >> 32) + (m2 >> 32) + (m4 >> 32);
#else
		a0 = 0x0000ffff & a->data[0];
		a1 = (0xffff0000 & a->data[0]) >> 16;
		b0 = 0x0000ffff & b->data[0];
		b1 = (0xffff0000 & b->data[0]) >> 16;
		m0 = a0 * b0;
		m1 = a1 * b0;
		m2 = a0 * b1;
		//m3 = a1 * b1;
		V = (m0 & 0x0000ffff);
		m4 = (m1 & 0x0000ffff) + (m2 & 0x0000ffff) + (m0 >> 16);
		V = V + (m4 << 16);
		//U = m3 + (m1 >> 16) + (m2 >> 16) + (m4 >> 16);
#endif
		mul->data[0] = V;
		return;
	}
	unsigned int resultSizeMinusOne = a->size + b->size - 1;
	unsigned int i, j, k, carry;
	chunk R0, R1, R2, U, m3;
	R0 = R1 = R2 = 0;
	for (k = 0; k < resultSizeMinusOne; k++)
	{
		for (i = 0; i < a->size; i++)
		{
			for (j = 0; j < b->size; j++)
			{
				if (i + j == k)
				{
#if ARCHITECTURE_BITS == 8
					a0 = 0x0f & a->data[i];
					a1 = (0xf0 & a->data[i]) >> 4;
					b0 = 0x0f & b->data[j];
					b1 = (0xf0 & b->data[j]) >> 4;
					m0 = a0 * b0;
					m1 = a1 * b0;
					m2 = a0 * b1;
					m3 = a1 * b1;
					V = (m0 & 0x0f);
					m4 = (m1 & 0x0f) + (m2 & 0x0f) + (m0 >> 4);
					V = V + (m4 << 4);
					U = m3 + (m1 >> 4) + (m2 >> 4) + (m4 >> 4);
#elif ARCHITECTURE_BITS == 16
					a0 = 0x00ff & a->data[i];
					a1 = (0xff00 & a->data[i]) >> 8;
					b0 = 0x00ff & b->data[j];
					b1 = (0xff00 & b->data[j]) >> 8;
					m0 = a0 * b0;
					m1 = a1 * b0;
					m2 = a0 * b1;
					m3 = a1 * b1;
					V = (m0 & 0x00ff);
					m4 = (m1 & 0x00ff) + (m2 & 0x00ff) + (m0 >> 8);
					V = V + (m4 << 8);
					U = m3 + (m1 >> 8) + (m2 >> 8) + (m4 >> 8);
#elif ARCHITECTURE_BITS == 64
					a0 = 0x00000000ffffffff & a->data[i];
					a1 = (0xffffffff00000000 & a->data[i]) >> 32;
					b0 = 0x00000000ffffffff & b->data[j];
					b1 = (0xffffffff00000000 & b->data[j]) >> 32;
					m0 = a0 * b0;
					m1 = a1 * b0;
					m2 = a0 * b1;
					m3 = a1 * b1;
					V = (m0 & 0x00000000ffffffff);
					m4 = (m1 & 0x00000000ffffffff) +
						(m2 & 0x00000000ffffffff) +
						(m0 >> 32);
					V = V + (m4 << 32);
					U = m3 + (m1 >> 32) + (m2 >> 32) + (m4 >> 32);
#else
					a0 = 0x0000ffff & a->data[i];
					a1 = (0xffff0000 & a->data[i]) >> 16;
					b0 = 0x0000ffff & b->data[j];
					b1 = (0xffff0000 & b->data[j]) >> 16;
					m0 = a0 * b0;
					m1 = a1 * b0;
					m2 = a0 * b1;
					m3 = a1 * b1;
					V = (m0 & 0x0000ffff);
					m4 = (m1 & 0x0000ffff) + (m2 & 0x0000ffff) + (m0 >> 16);
					V = V + (m4 << 16);
					U = m3 + (m1 >> 16) + (m2 >> 16) + (m4 >> 16);
#endif
					// c,R0 = sum(R0, V)
					ChunksAddition(&R0, &carry, R0, V, 0);
					// c,R1 = sum(R1 + U + c)
					ChunksAddition(&R1, &carry, R1, U, carry);
					// R2 = R2 + c
					R2 = R2 + carry;
				}
			}
		}
		mul->data[k] = R0;
		R0 = R1;
		R1 = R2;
		R2 = 0;
	}
	// Present implementation does not assure that result size (in chunks) is
	// exactly the sum of chunk sizes of operands. The chunk size of the result
	// is evaluated by adding the bit sizes of operands, so result chunk size 
	// can equal to the sum of operands chunk sizes, or to the sum of operands 
	// chunk sizes - 1.
	if (resultSizeMinusOne == mul->size - 1)
	{
		mul->data[resultSizeMinusOne] = R0;
	}
}

void MPLeftShift(mpnumber * res, mpnumber * a, unsigned int shifts)
{
	// Caller must assure that res is properly allocated
	// i is the chunk index for source mpnumber, j for result
	unsigned int i = 0;
	unsigned int j = shifts / ARCHITECTURE_BITS;
	unsigned int chunkshifts = shifts % ARCHITECTURE_BITS;
	// Reset all destination bits
	for (; i < res->size; i++)
	{
		res->data[i] = 0;
	}
	// If shift multiple of chunk size, only replace chunks
	if (chunkshifts == 0)
	{
		for (i = 0; (i < a->size) && (j < res->size); i++, j++)
		{
			res->data[j] = a->data[i];
		}
		return;
	}
	// Else, shift chunks
	unsigned int rightshifts = ARCHITECTURE_BITS - chunkshifts;
	for (i = 0; (i < a->size) && (j < res->size); i++, j++)
	{
		res->data[j] = a->data[i] << chunkshifts;
		if (i > 0)
		{
			res->data[j] |= a->data[i - 1] >> rightshifts;
		}
	}
	if (((i - 1) < a->size) && (j < res->size))
	{
		res->data[j] |= a->data[i - 1] >> rightshifts;
	}
}