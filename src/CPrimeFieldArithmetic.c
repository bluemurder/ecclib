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

void SetElement(element_t * element, char * hexString, field_t * field)
{
	element->data = (chunk_t *)malloc(field->chunksNumber * (sizeof(chunk_t)));
	// Write characteristic data
	SetString(hexString, field->chunksNumber, field->bits, element->data);
}

void FreeElement(element_t * element)
{
	free(element->data);
}

//! Checks if a is greater or equal than b
//! \returns 1 if a is greater or equal to b
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

//! Fast modulo reduction for p = 2^192 − 2^64 − 1
//! Algorithm 2.27 [1]
void FastReductionFIPSp192(element_t * red, element_t * a, field_t * field)
{
	element_t s1, s2, s3, s4, partialres1, partialres2;

	// Init partial results data
	chunk_t s1data[3];
	s1.data = s1data;
	chunk_t s2data[3];
	s2.data = s2data;
	chunk_t s3data[3];
	s3.data = s3data;
	chunk_t s4data[3];
	s4.data = s4data;

	// Assuming a = (a5,a4,a3,a2,a1,a0) (64-bit chunks)
	// s1 = (c2, c1, c0)
	s1.data[0] = a->data[0];
	s1.data[1] = a->data[1];
	s1.data[2] = a->data[2];
	// s2 = (0, c3, c3)
	s2.data[0] = a->data[3];
	s2.data[1] = a->data[3];
	s2.data[2] = 0;

	// s3 = (c4, c4, 0)
	s3.data[0] = 0;
	s3.data[1] = a->data[4];
	s3.data[2] = a->data[4];

	// s4 = (c5, c5, c5).
	s4.data[0] = a->data[5];
	s4.data[1] = a->data[5];
	s4.data[2] = a->data[5];

	// Allocate space for sum results
	SetElement(&partialres1, "", field);
	SetElement(&partialres2, "", field);

	Addition(&partialres1, &s1, &s2, field);
	Addition(&partialres2, &partialres1, &s3, field);
	Addition(red, &partialres2, &s4, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);
}


//! Create a new prime field element from an hexadecimal string and a field 
//! size. Note that the field size overcomes the string if it is passed with 
//! higher dimension than specified field size.
//PrimeFieldElement * NewElement(char * hexString, size_t bitSize, unsigned int fieldBitSize)
//{
//	size_t hexStringLen = strlen(hexString);
//	PrimeFieldElement * element = (PrimeFieldElement *)malloc(sizeof(PrimeFieldElement));
//	element->m_bits = bitSize;
//	element->m_fieldBits = fieldBitSize;
//	element->m_ChunksNumber = bitSize / ARCHITECTURE_BITS;
//	if (bitSize % ARCHITECTURE_BITS)
//	{
//		element->m_ChunksNumber++;
//	}
//	element->m_data = (chunk_t *)malloc(element->m_ChunksNumber * sizeof(chunk_t));
//	char * stringCursor = hexString + hexStringLen - 1;
//	size_t i, j;
//	unsigned int maxChunkCharacters = ARCHITECTURE_BITS / 4;
//	chunk_t value;
//	unsigned char readstring = 1;
//	size_t charactersCounter = 0;
//	size_t bitsCounter = 0;
//	// For every chunk
//	for (i = 0; i < element->m_ChunksNumber; i++)
//	{
//		element->m_data[i] = 0;
//		for (j = 0; (j < maxChunkCharacters && readstring == 1); j++, charactersCounter++, bitsCounter += 4)
//		{
//			// Numbers
//			if (*stringCursor > 47 && *stringCursor < 58)
//			{
//				value = *stringCursor - 48;
//			}
//			// Uppercase letters
//			else if (*stringCursor > 64 && *stringCursor < 71)
//			{
//				value = *stringCursor - 65 + 10;
//			}
//			// lowercase letters
//			else if (*stringCursor > 96 && *stringCursor < 103)
//			{
//				value = *stringCursor - 97 + 10;
//			}
//			// If declared input bits does not coner entirely current chunk:
//			if (bitsCounter + 4 > bitSize)
//			{
//				// Apply proper bitmask
//				size_t difference = bitsCounter + 4 - bitSize;
//				if (difference == 4)
//				{
//					value = 0;
//				}
//				else if (difference == 3)
//				{
//					value = value & 0x1;
//				}
//				else if (difference == 2)
//				{
//					value = value & 0x3;
//				}
//				else if (difference == 1)
//				{
//					value = value & 0x7;
//				}
//				// exit
//				readstring = 0;
//			}
//			// Shift when reading higher chunk bits
//			value = value << (4 * j);
//			// Sum to current chunk integer
//			element->m_data[i] += value;
//			// If input string with no more characters, exit
//			if (charactersCounter + 1 == hexStringLen)
//			{
//				readstring = 0;
//			}
//			// Read next string character
//			stringCursor--;
//		}
//	}
//	return element;
//}

//! Frees memory of specified element
//void FreeElement(PrimeFieldElement * element)
//{
//	if (element != NULL)
//	{
//		free(element->m_data);
//		element->m_data = NULL;
//		free(element);
//		element = NULL;
//	}
//}

//! Returns a + b (mod p)
//PrimeFieldElement * AddMod(PrimeFieldElement * a, PrimeFieldElement * b, PrimeFieldElement * p)
//{
//	// Assuming that a, b amd c are not null pointers. Care of caller to ensure this
//	// Assuming that a and b present the SAME number of chunks. Care of caller to ensure this
//	// Allocate 
//	PrimeFieldElement * element = (PrimeFieldElement *)malloc(sizeof(PrimeFieldElement));
//	element->m_data = (chunk_t *)malloc(a->m_ChunksNumber * sizeof(chunk_t));
//	element->m_ChunksNumber = a->m_ChunksNumber;
//	element->m_fieldBits = a->m_fieldBits;
//	element->m_bits = a->m_fieldBits;
//	// Binary sum
//	chunk_t carry = 0;
//	chunk_t borrow = 0;
//	size_t i;
//	for (i = 0; i < a->m_ChunksNumber; i++)
//	{
//		if (carry)
//		{
//			element->m_data[i] = a->m_data[i] + b->m_data[i] + carry;
//			carry = (a->m_data[i] >= element->m_data[i]);
//		}
//		else
//		{
//			element->m_data[i] = a->m_data[i] + b->m_data[i];
//			carry = (a->m_data[i] > element->m_data[i]);
//		}
//	}
//	// If carry, modulo reduction
//	if (carry)
//	{
//		for (i = 0; i < element->m_ChunksNumber; i++)
//		{
//			if (borrow)
//			{
//				element->m_data[i] = element->m_data[i] - p->m_data[i] - borrow;
//				borrow = (element->m_data[i] <= p->m_data[i]);
//			}
//			else
//			{
//				element->m_data[i] = element->m_data[i] - p->m_data[i];
//				borrow = (element->m_data[i] < p->m_data[i]);
//			}
//		}
//	}
//	return element;
//}

//! Returns a - b (mod p)
//PrimeFieldElement * SubMod(PrimeFieldElement * a, PrimeFieldElement * b, PrimeFieldElement * p)
//{
//	// Assuming that a, b amd c are not null pointers. Care of caller to ensure this
//	// Assuming that a and b present the SAME number of chunks. Care of caller to ensure this
//	// Allocate 
//	PrimeFieldElement * element = (PrimeFieldElement *)malloc(sizeof(PrimeFieldElement));
//	element->m_data = (chunk_t *)malloc(a->m_ChunksNumber * sizeof(chunk_t));
//	element->m_ChunksNumber = a->m_ChunksNumber;
//	element->m_fieldBits = a->m_fieldBits;
//	element->m_bits = a->m_fieldBits;
//	// Binary subtraction
//	chunk_t carry = 0;
//	chunk_t borrow = 0;
//	size_t i;
//	for (i = 0; i < a->m_ChunksNumber; i++)
//	{
//		if (borrow)
//		{
//			element->m_data[i] = a->m_data[i] - b->m_data[i] - borrow;
//			borrow = (a->m_data[i] <= element->m_data[i]);
//		}
//		else
//		{
//			element->m_data[i] = a->m_data[i] - b->m_data[i];
//			borrow = (a->m_data[i] < element->m_data[i]);
//		}
//	}
//	// If borrow, modulo reduction
//	if (borrow)
//	{
//		for (i = 0; i < element->m_ChunksNumber; i++)
//		{
//			if (carry)
//			{
//				element->m_data[i] = element->m_data[i] + p->m_data[i] + carry;
//				carry = (element->m_data[i] <= p->m_data[i]);
//			}
//			else
//			{
//				element->m_data[i] = element->m_data[i] + p->m_data[i];
//				carry = (element->m_data[i] < p->m_data[i]);
//			}
//		}
//	}
//	return element;
//}

//! Fast modulo reduction for p = 2^192 − 2^64 − 1
//! \param a A number up to 2*192 bits
//! \param p The number p of 193 bits
//! Algorithm 2.27 [1]
//PrimeFieldElement * FastReductionFIPSp192(PrimeFieldElement * a, PrimeFieldElement * p)
//{
//	PrimeFieldElement * element = (PrimeFieldElement *)malloc(sizeof(PrimeFieldElement));
//	size_t chunksNumber = 192 / ARCHITECTURE_BITS;
//	if (192 % ARCHITECTURE_BITS)
//	{
//		chunksNumber++;
//	}
//	element->m_data = (chunk_t *)malloc(chunksNumber * sizeof(chunk_t));
//	element->m_fieldBits = 192;
//
//#if ARCHITECTURE_BITS == 8
//
//	//#elif ARCHITECTURE_BITS == 16
//	//	
//	//#elif ARCHITECTURE_BITS == 64
//	// Allocation
//	PrimeFieldElement * s1 = (PrimeFieldElement *)malloc(sizeof(PrimeFieldElement));
//	PrimeFieldElement * s2 = (PrimeFieldElement *)malloc(sizeof(PrimeFieldElement));
//	PrimeFieldElement * s3 = (PrimeFieldElement *)malloc(sizeof(PrimeFieldElement));
//	PrimeFieldElement * s4 = (PrimeFieldElement *)malloc(sizeof(PrimeFieldElement));
//
//	// Create space for every element (no dynamic allocation)
//	s1->m_ChunksNumber = 3;
//	chunk_t s1data[3] = { 0, 0, 0 };
//	s1->m_data = s1data;
//	s1->m_fieldBits = a->m_fieldBits;
//
//	s2->m_ChunksNumber = 3;
//	chunk_t s2data[3] = { 0, 0, 0 };
//	s2->m_data = s2data;
//	s2->m_fieldBits = a->m_fieldBits;
//
//	s3->m_ChunksNumber = 3;
//	chunk_t s3data[3] = { 0, 0, 0 };
//	s3->m_data = s3data;
//	s3->m_fieldBits = a->m_fieldBits;
//
//	s4->m_ChunksNumber = 3;
//	chunk_t s4data[3] = { 0, 0, 0 };
//	s4->m_data = s4data;
//	s4->m_fieldBits = a->m_fieldBits;
//
//	// s1 = (c2, c1, c0)
//	if (a->m_ChunksNumber > 0)
//	{
//		s1->m_data[0] = a->m_data[0];
//		s1->m_data[1] = a->m_data[1];
//	}
//	if (a->m_ChunksNumber > 1)
//	{
//		s1->m_data[2] = a->m_data[2];
//	}
//
//	// s2 = (0, c3, c3)
//	if (a->m_ChunksNumber > 2)
//	{
//		s2->m_data[0] = a->m_data[3];
//		s2->m_data[1] = a->m_data[3];
//	}
//
//	// s3 = (c4, c4, 0)
//	if (a->m_ChunksNumber > 3)
//	{
//		s3->m_data[1] = a->m_data[4];
//		s3->m_data[2] = a->m_data[4];
//	}
//
//	// s4 = (c5, c5, c5).
//	if (a->m_ChunksNumber > 4)
//	{
//		s4->m_data[0] = a->m_data[5];
//		s4->m_data[1] = a->m_data[5];
//		s4->m_data[2] = a->m_data[5];
//	}
//
//	PrimeFieldElement * temp1 = AddMod(s1, s2, p);
//	PrimeFieldElement * temp2 = AddMod(temp2, s2, p);
//
//	// Free temporary elements
//	s1->m_data = NULL;
//	s2->m_data = NULL;
//	s3->m_data = NULL;
//	s4->m_data = NULL;
//	FreeElement(s1);
//	FreeElement(s2);
//	FreeElement(s3);
//	FreeElement(s4);
//#else
//
//#endif
//
//	return element;
//}