/***********************************************************************\
* CPrimeFieldArithmetic - Prime Field Arithmetic primitives             *
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

//! \file CPrimeFieldArithmetic.c Definitions for current library methods.
//! Some references for following algorithms:
//! [1] D. Hankerson, S. Vanstone, and A.J. Menezes. Guide to Elliptic Curve
//! Cryptography. Springer Professional Computing.Springer, 2004.

#include "../include/CPrimeFieldArithmeticInt.h"
#include <malloc.h>
#include <string.h>

//! Writes the hexdecimal value from hexString to data, strictly following 
//! the specified bitsize
//! \param hexString The hexdecimal value to write (MSB letf)
//! \param chunksNumber The number of chunks already allocated
//! \param bitSize The size in bits of the underlying field
//! \param data The vector of chunks to fill
void SetString(char * hexString, unsigned int chunksNumber, unsigned int bitSize, chunk_t * data)
{
	size_t hexStringLen = strlen(hexString);
	char * stringCursor = hexString + hexStringLen - 1;
	size_t i, j;
	unsigned int maxChunkCharacters = ARCHITECTURE_BITS / 4;
	chunk_t value;
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
		data[i] = 0;
		for (j = 0; (j < maxChunkCharacters && readstring == 1); j++, charactersCounter++, bitsCounter += 4)
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
			data[i] += value;
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

//! Sets the specified bit size and characteristics in the field object
//! \param field The field to fill (must point to already allocated memory)
//! \param fieldBitSize The field bis size
//! \param characteristics Hex string describing the field characteristics
void SetField(field_t * field, unsigned int fieldBitSize, char * characteristics)
{
	// Assign field bits
	field->bits = fieldBitSize;
	// Evaluate chunks number
	field->chunksNumber = fieldBitSize / ARCHITECTURE_BITS;
	if (fieldBitSize % ARCHITECTURE_BITS)
	{
		field->chunksNumber++;
	}
	// Allocate characteristic data
	field->characteristics.data = (chunk_t *)malloc(field->chunksNumber * (sizeof(chunk_t)));
	// Write characteristic data
	SetString(characteristics, field->chunksNumber, fieldBitSize, field->characteristics.data);
}

//! Sets the specified value expressed via a hex string in the field element 
//! object. Needs the presence of a field_t object storing field bitsize, 
//! chunks number and characteristics.
//! \param element The target element to write
//! \param hexString The string value to write
//! \param field The object storing field data
void SetElement(element_t * element, char * hexString, field_t * field)
{
	element->data = (chunk_t *)malloc(field->chunksNumber * (sizeof(chunk_t)));
	// Write characteristic data
	SetString(hexString, field->chunksNumber, field->bits, element->data);
}

//! Erase the memory of internal members of element
void FreeElement(element_t * element)
{
	free(element->data);
}

//! Checks if a is greater or equal than b
//! \returns 1 if a is greater than or equal to b, 0 otherwise
unsigned int GreaterOrEqual(element_t * a, element_t * b, field_t * field)
{
	unsigned int i;
	for (i = field->chunksNumber - 1;; i--)
	{
		if (a->data[i] < b->data[i])
		{
			return 0;
		}
		else if (a->data[i] > b->data[i])
		{
			return 1;
		}
		if (i == 0)
		{
			return 1;
		}
	}
}

//! Checks if a is equal to b
//! \returns 1 if a is equal to b, 0 otherwise
unsigned int Equals(element_t * a, element_t * b, field_t * field)
{
	unsigned int i;
	for (i = 0; i< field->chunksNumber; i++)
	{
		if (a->data[i] != b->data[i])
		{
			return 0;
		}
	}
	return 1;
}

//! Addition of a and b over a prime field. Let p the field characteristics, 
//! the method evaluates a+b(mod p)
//! Algorithm 2.7 [1]
void Addition(element_t * sum, element_t * a, element_t * b, field_t * field)
{
	// Binary sum
	chunk_t carry = 0;
	chunk_t borrow = 0;
	chunk_t firstOperand;
	unsigned int i;
	unsigned int equals = 1;
	for (i = 0; i < field->chunksNumber; i++)
	{
		if (carry)
		{
			sum->data[i] = a->data[i] + b->data[i] + carry;
			carry = (a->data[i] >= sum->data[i]);
		}
		else
		{
			sum->data[i] = a->data[i] + b->data[i];
			carry = (a->data[i] > sum->data[i]);
		}
	}
	// If carry, or if sum greater or equal to p: modulo reduction
	if (carry || GreaterOrEqual(sum, &(field->characteristics), field))
	{
		for (i = 0; i < field->chunksNumber; i++)
		{
			if (borrow)
			{
				firstOperand = sum->data[i];
				sum->data[i] = sum->data[i] - field->characteristics.data[i] - borrow;
				borrow = (sum->data[i] >= firstOperand);
			}
			else
			{
				firstOperand = sum->data[i];
				sum->data[i] = sum->data[i] - field->characteristics.data[i];
				borrow = (sum->data[i] > firstOperand);
			}
		}
	}
}

//! Subtraction of b from a over a prime field. Let p the field 
//! characteristics, the method evaluates a-b(mod p)
//! Algorithm 2.8 [1]
void Subtraction(element_t * sub, element_t * a, element_t * b, field_t * field)
{
	// Binary subtraction
	chunk_t carry = 0;
	chunk_t borrow = 0;
	chunk_t firstOperand;
	unsigned int i;
	for (i = 0; i < field->chunksNumber; i++)
	{
		if (borrow)
		{
			sub->data[i] = a->data[i] - b->data[i] - borrow;
			borrow = (a->data[i] <= sub->data[i]);
		}
		else
		{
			sub->data[i] = a->data[i] - b->data[i];
			borrow = (a->data[i] < sub->data[i]);
		}
	}
	// If borrow, modulo reduction
	if (borrow)
	{
		for (i = 0; i < field->chunksNumber; i++)
		{
			if (carry)
			{
				firstOperand = sub->data[i];
				sub->data[i] = sub->data[i] + field->characteristics.data[i] + carry;
				carry = (sub->data[i] <= firstOperand);
			}
			else
			{
				firstOperand = sub->data[i];
				sub->data[i] = sub->data[i] + field->characteristics.data[i];
				carry = (sub->data[i] < firstOperand);
			}
		}
	}
}

//! Fast modulo reduction for p = 2^192 − 2^64 − 1 (FIPS 186-2 prime)
//! Algorithm 2.27 [1]
void FastReductionFIPSp192(element_t * red, element_t * a, field_t * field)
{
#if ARCHITECTURE_BITS == 8

	// Note that a presents 48 chunks

	element_t s1, s2, s3, s4, partialres1, partialres2;

	// Init partial results data
	chunk_t s1data[24]; // 8 * 24 = 192
	s1.data = s1data;
	chunk_t s2data[24];
	s2.data = s2data;
	chunk_t s3data[24];
	s3.data = s3data;
	chunk_t s4data[24];
	s4.data = s4data;

	// Assuming a = (a5,a4,a3,a2,a1,a0) (64-bit chunks)
	// s1 = (a2, a1, a0)
	s1.data[0] = a->data[0];
	s1.data[1] = a->data[1];
	s1.data[2] = a->data[2];
	s1.data[3] = a->data[3];
	s1.data[4] = a->data[4];
	s1.data[5] = a->data[5];
	s1.data[6] = a->data[6];
	s1.data[7] = a->data[7];
	s1.data[8] = a->data[8];
	s1.data[9] = a->data[9];
	s1.data[10] = a->data[10];
	s1.data[11] = a->data[11];
	s1.data[12] = a->data[12];
	s1.data[13] = a->data[13];
	s1.data[14] = a->data[14];
	s1.data[15] = a->data[15];
	s1.data[16] = a->data[16];
	s1.data[17] = a->data[17];
	s1.data[18] = a->data[18];
	s1.data[19] = a->data[19];
	s1.data[20] = a->data[20];
	s1.data[21] = a->data[21];
	s1.data[22] = a->data[22];
	s1.data[23] = a->data[23];
	// s2 = (0, a3, a3)
	s2.data[0] = a->data[0];
	s2.data[1] = a->data[0];
	s2.data[2] = a->data[0];
	s2.data[3] = a->data[0];
	s2.data[4] = a->data[0];
	s2.data[5] = a->data[0];
	s2.data[6] = a->data[0];
	s2.data[7] = a->data[0];
	s2.data[8] = a->data[24];
	s2.data[9] = a->data[25];
	s2.data[10] = a->data[26];
	s2.data[11] = a->data[27];
	s2.data[12] = a->data[28];
	s2.data[13] = a->data[29];
	s2.data[14] = a->data[30];
	s2.data[15] = a->data[31];
	s2.data[16] = a->data[24];
	s2.data[17] = a->data[25];
	s2.data[18] = a->data[26];
	s2.data[19] = a->data[27];
	s2.data[20] = a->data[28];
	s2.data[21] = a->data[29];
	s2.data[22] = a->data[30];
	s2.data[23] = a->data[31];
	// s3 = (a4, a4, 0)
	s3.data[0] = a->data[0];
	s3.data[1] = a->data[0];
	s3.data[2] = a->data[0];
	s3.data[3] = a->data[0];
	s3.data[4] = a->data[0];
	s3.data[5] = a->data[0];
	s3.data[6] = a->data[0];
	s3.data[7] = a->data[0];
	s3.data[8] = a->data[32];
	s3.data[9] = a->data[33];
	s3.data[10] = a->data[34];
	s3.data[11] = a->data[35];
	s3.data[12] = a->data[36];
	s3.data[13] = a->data[37];
	s3.data[14] = a->data[38];
	s3.data[15] = a->data[39];
	s3.data[16] = a->data[32];
	s3.data[17] = a->data[33];
	s3.data[18] = a->data[34];
	s3.data[19] = a->data[35];
	s3.data[20] = a->data[36];
	s3.data[21] = a->data[37];
	s3.data[22] = a->data[38];
	s3.data[23] = a->data[39];
	// s4 = (a5, a5, a5).
	s4.data[0] = a->data[40];
	s4.data[1] = a->data[41];
	s4.data[2] = a->data[42];
	s4.data[3] = a->data[43];
	s4.data[4] = a->data[44];
	s4.data[5] = a->data[45];
	s4.data[6] = a->data[46];
	s4.data[7] = a->data[47];
	s4.data[8] = a->data[40];
	s4.data[9] = a->data[41];
	s4.data[10] = a->data[42];
	s4.data[11] = a->data[43];
	s4.data[12] = a->data[44];
	s4.data[13] = a->data[45];
	s4.data[14] = a->data[46];
	s4.data[15] = a->data[47];
	s4.data[16] = a->data[40];
	s4.data[17] = a->data[41];
	s4.data[18] = a->data[42];
	s4.data[19] = a->data[43];
	s4.data[20] = a->data[44];
	s4.data[21] = a->data[45];
	s4.data[22] = a->data[46];
	s4.data[23] = a->data[47];

	// Allocate space for sum results
	SetElement(&partialres1, "", field);
	SetElement(&partialres2, "", field);

	// red = s1 + s2 + s3 + s4
	Addition(&partialres1, &s1, &s2, field);
	Addition(&partialres2, &partialres1, &s3, field);
	Addition(red, &partialres2, &s4, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#elif ARCHITECTURE_BITS == 16

	// Note that a presents 24 chunks

	element_t s1, s2, s3, s4, partialres1, partialres2;

	// Init partial results data
	chunk_t s1data[12]; // 16 * 12 = 192
	s1.data = s1data;
	chunk_t s2data[12];
	s2.data = s2data;
	chunk_t s3data[12];
	s3.data = s3data;
	chunk_t s4data[12];
	s4.data = s4data;

	// Assuming a = (a5,a4,a3,a2,a1,a0) (64-bit chunks)
	// s1 = (a2, a1, a0)
	s1.data[0] = a->data[0];
	s1.data[1] = a->data[1];
	s1.data[2] = a->data[2];
	s1.data[3] = a->data[3];
	s1.data[4] = a->data[4];
	s1.data[5] = a->data[5];
	s1.data[6] = a->data[6];
	s1.data[7] = a->data[7];
	s1.data[8] = a->data[8];
	s1.data[9] = a->data[9];
	s1.data[10] = a->data[10];
	s1.data[11] = a->data[11];
	// s2 = (0, a3, a3)
	s2.data[0] = a->data[12];
	s2.data[1] = a->data[13];
	s2.data[2] = a->data[14];
	s2.data[3] = a->data[15];
	s2.data[4] = a->data[12];
	s2.data[5] = a->data[13];
	s2.data[6] =a->data[14];
	s2.data[7] = a->data[15];
	s2.data[8] = 0;
	s2.data[9] = 0;
	s2.data[10] = 0;
	s2.data[11] = 0;
	// s3 = (a4, a4, 0)
	s3.data[0] = a->data[0];
	s3.data[1] = a->data[0];
	s3.data[2] = a->data[0];
	s3.data[3] = a->data[0];
	s3.data[4] = a->data[16];
	s3.data[5] = a->data[17];
	s3.data[6] = a->data[18];
	s3.data[7] = a->data[19];
	s3.data[8] = a->data[16];
	s3.data[9] = a->data[17];
	s3.data[10] = a->data[18];
	s3.data[11] = a->data[19];
	// s4 = (a5, a5, a5).
	s4.data[0] = a->data[20];
	s4.data[1] = a->data[21];
	s4.data[2] = a->data[22];
	s4.data[3] = a->data[23];
	s4.data[4] = a->data[20];
	s4.data[5] = a->data[21];
	s4.data[6] = a->data[22];
	s4.data[7] = a->data[23];
	s4.data[8] = a->data[20];
	s4.data[9] = a->data[21];
	s4.data[10] = a->data[22];
	s4.data[11] = a->data[23];

	// Allocate space for sum results
	SetElement(&partialres1, "", field);
	SetElement(&partialres2, "", field);

	// red = s1 + s2 + s3 + s4
	Addition(&partialres1, &s1, &s2, field);
	Addition(&partialres2, &partialres1, &s3, field);
	Addition(red, &partialres2, &s4, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#elif ARCHITECTURE_BITS == 64

	// Note that a presents 6 chunks

	element_t s1, s2, s3, s4, partialres1, partialres2;

	// Init partial results data
	chunk_t s1data[3]; // 64 * 3 = 192
	s1.data = s1data;
	chunk_t s2data[3];
	s2.data = s2data;
	chunk_t s3data[3];
	s3.data = s3data;
	chunk_t s4data[3];
	s4.data = s4data;

	// Assuming a = (a5,a4,a3,a2,a1,a0) (64-bit chunks)
	// s1 = (a2, a1, a0)
	s1.data[0] = a->data[0];
	s1.data[1] = a->data[1];
	s1.data[2] = a->data[2];
	// s2 = (0, a3, a3)
	s2.data[0] = a->data[3];
	s2.data[1] = a->data[3];
	s2.data[2] = 0;
	// s3 = (a4, a4, 0)
	s3.data[0] = 0;
	s3.data[1] = a->data[4];
	s3.data[2] = a->data[4];
	// s4 = (a5, a5, a5).
	s4.data[0] = a->data[5];
	s4.data[1] = a->data[5];
	s4.data[2] = a->data[5];

	// Allocate space for sum results
	SetElement(&partialres1, "", field);
	SetElement(&partialres2, "", field);

	// red = s1 + s2 + s3 + s4
	Addition(&partialres1, &s1, &s2, field);
	Addition(&partialres2, &partialres1, &s3, field);
	Addition(red, &partialres2, &s4, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#else // Set chunks to 32 bit

	// Note that a presents 12 chunks

	element_t s1, s2, s3, s4, partialres1, partialres2;

	// Init partial results data
	chunk_t s1data[6]; // 32 * 6 = 192
	s1.data = s1data;
	chunk_t s2data[6];
	s2.data = s2data;
	chunk_t s3data[6];
	s3.data = s3data;
	chunk_t s4data[6];
	s4.data = s4data;

	// Assuming a = (a5,a4,a3,a2,a1,a0) (64-bit chunks)
	// s1 = (a2, a1, a0)
	s1.data[0] = a->data[0];
	s1.data[1] = a->data[1];
	s1.data[2] = a->data[2];
	s1.data[3] = a->data[3];
	s1.data[4] = a->data[4];
	s1.data[5] = a->data[5];
	// s2 = (0, a3, a3)
	s2.data[0] = a->data[6];
	s2.data[1] = a->data[7];
	s2.data[2] = a->data[6];
	s2.data[3] = a->data[7];
	s2.data[4] = 0;
	s2.data[5] = 0;
	// s3 = (a4, a4, 0)
	s3.data[0] = 0;
	s3.data[1] = 0;
	s3.data[2] = a->data[8];
	s3.data[3] = a->data[9];
	s3.data[4] = a->data[8];
	s3.data[5] = a->data[9];
	// s4 = (a5, a5, a5).
	s4.data[0] = a->data[10];
	s4.data[1] = a->data[11];
	s4.data[2] = a->data[10];
	s4.data[3] = a->data[11];
	s4.data[4] = a->data[10];
	s4.data[5] = a->data[11];

	// Allocate space for sum results
	SetElement(&partialres1, "", field);
	SetElement(&partialres2, "", field);

	// red = s1 + s2 + s3 + s4
	Addition(&partialres1, &s1, &s2, field);
	Addition(&partialres2, &partialres1, &s3, field);
	Addition(red, &partialres2, &s4, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#endif
}