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

//! \file PrimeFieldArithmetic.c Definitions for PrimeFieldArithmetic library
//! methods.

#include "../include/PrimeFieldArithmeticInt.h"
#include <malloc.h>
#include <string.h>

// Prototypes

// Implementation

unsigned int PFEquals(pfelement * a, pfelement * b, pfproperties * field)
{
	unsigned int i;
	for (i = 0; i < field->chunksNumber; i++)
	{
		if (a->data[i] != b->data[i])
		{
			return 0;
		}
	}
	return 1;
}

void InitFieldProperties(
	pfproperties * field,
	unsigned int fieldBitSize,
	char * characteristics)
{
	// Assign field bits
	field->bits = fieldBitSize;
	// Evaluate chunks number
	field->chunksNumber = fieldBitSize / ARCHITECTURE_BITS;
	if (fieldBitSize % ARCHITECTURE_BITS)
	{
		field->chunksNumber++;
	}
	// Allocate characteristics data
	field->characteristics.data =
		(chunk *)malloc(field->chunksNumber * (sizeof(chunk)));
	// Write characteristics data
	field->characteristics.size = field->chunksNumber;
	SetString(
		&field->characteristics,
		characteristics,
		field->chunksNumber,
		fieldBitSize);
	// Precompute some data required by Barret reduction algorithm
	// k = chunks number of field characteristics
	// b = close to word size
	// mu = floor(b^{2k} / char)
	// @@ TODO
}

void InitElementByString(
	pfelement * element,
	char * hexString,
	pfproperties * field)
{
	element->data = (chunk *)malloc(field->chunksNumber * (sizeof(chunk)));
	element->size = field->chunksNumber;
	// Write characteristic data
	SetString(element, hexString, field->chunksNumber, field->bits);
}

void FreeElement(pfelement * element)
{
	free(element->data);
}

void FreeFieldProperties(pfproperties * field)
{
	FreeNumber(&field->characteristics);
	//FreeNumber(&field->mu); // TODO
}

unsigned int GreaterOrEqual(pfelement * a, pfelement * b, pfproperties * field)
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

void PFAddition(
	pfelement * sum,
	pfelement * a,
	pfelement * b,
	pfproperties * field)
{
	// Binary sum
	unsigned int carry = 0;
	unsigned int borrow = 0;
	chunk firstOperand;
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
				sum->data[i] = sum->data[i] -
					field->characteristics.data[i] -
					borrow;
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

void PFSubtraction(
	pfelement * sub,
	pfelement * a,
	pfelement * b,
	pfproperties * field)
{
	// Binary subtraction
	unsigned int carry = 0;
	unsigned int borrow = 0;
	chunk firstOperand;
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
				sub->data[i] = sub->data[i] +
					field->characteristics.data[i] +
					carry;
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

void PFMultiplication(
	pfelement * mul,
	pfelement * a,
	pfelement * b,
	pfproperties * field)
{
	// Call to proper multiplication algorithm
	// Call to proper modulo reduction algorithm
}

void PFDivision(
	pfelement * div,
	pfelement * a,
	pfelement * b,
	pfproperties * field)
{
	// Call to proper modular inversion algorithm
	// Call to Multiplication()
}

void BarrettReduction(
	pfelement * red,
	mpnumber * a, 
	pfproperties * field)
{
	//
}

void ModifiedBarretReduction(
	pfelement * red,
	mpnumber * a,
	pfproperties * field)
{

}

void FastReductionFIPSp192(
	pfelement * red,
	pfelement * a,
	pfproperties * field)
{
#if ARCHITECTURE_BITS == 8

	// Note that a presents 48 chunks

	pfelement s1, s2, s3, s4, partialres1, partialres2;

	// Init partial results data
	chunk s1data[24]; // 8 * 24 = 192
	s1.data = s1data;
	chunk s2data[24];
	s2.data = s2data;
	chunk s3data[24];
	s3.data = s3data;
	chunk s4data[24];
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
	s2.data[0] = a->data[24];
	s2.data[1] = a->data[25];
	s2.data[2] = a->data[26];
	s2.data[3] = a->data[27];
	s2.data[4] = a->data[28];
	s2.data[5] = a->data[29];
	s2.data[6] = a->data[30];
	s2.data[7] = a->data[31];
	s2.data[8] = a->data[24];
	s2.data[9] = a->data[25];
	s2.data[10] = a->data[26];
	s2.data[11] = a->data[27];
	s2.data[12] = a->data[28];
	s2.data[13] = a->data[29];
	s2.data[14] = a->data[30];
	s2.data[15] = a->data[31];
	s2.data[16] = 0;
	s2.data[17] = 0;
	s2.data[18] = 0;
	s2.data[19] = 0;
	s2.data[20] = 0;
	s2.data[21] = 0;
	s2.data[22] = 0;
	s2.data[23] = 0;
	// s3 = (a4, a4, 0)
	s3.data[0] = 0;
	s3.data[1] = 0;
	s3.data[2] = 0;
	s3.data[3] = 0;
	s3.data[4] = 0;
	s3.data[5] = 0;
	s3.data[6] = 0;
	s3.data[7] = 0;
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

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(24, 192, &s1);
	char * s2dump = GetString(24, 192, &s2);
	char * s3dump = GetString(24, 192, &s3);
	char * s4dump = GetString(24, 192, &s4);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// red = s1 + s2 + s3 + s4
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s3, field);
	PFAddition(red, &partialres2, &s4, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#elif ARCHITECTURE_BITS == 16

	// Note that a presents 24 chunks

	pfelement s1, s2, s3, s4, partialres1, partialres2;

	// Init partial results data
	chunk s1data[12]; // 16 * 12 = 192
	s1.data = s1data;
	chunk s2data[12];
	s2.data = s2data;
	chunk s3data[12];
	s3.data = s3data;
	chunk s4data[12];
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
	s2.data[6] = a->data[14];
	s2.data[7] = a->data[15];
	s2.data[8] = 0;
	s2.data[9] = 0;
	s2.data[10] = 0;
	s2.data[11] = 0;
	// s3 = (a4, a4, 0)
	s3.data[0] = 0;
	s3.data[1] = 0;
	s3.data[2] = 0;
	s3.data[3] = 0;
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

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(12, 192, &s1);
	char * s2dump = GetString(12, 192, &s2);
	char * s3dump = GetString(12, 192, &s3);
	char * s4dump = GetString(12, 192, &s4);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// red = s1 + s2 + s3 + s4
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s3, field);
	PFAddition(red, &partialres2, &s4, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#elif ARCHITECTURE_BITS == 64

	// Note that a presents 6 chunks

	pfelement s1, s2, s3, s4, partialres1, partialres2;

	// Init partial results data
	chunk s1data[3]; // 64 * 3 = 192
	s1.data = s1data;
	chunk s2data[3];
	s2.data = s2data;
	chunk s3data[3];
	s3.data = s3data;
	chunk s4data[3];
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

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(3, 192, &s1);
	char * s2dump = GetString(3, 192, &s2);
	char * s3dump = GetString(3, 192, &s3);
	char * s4dump = GetString(3, 192, &s4);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// red = s1 + s2 + s3 + s4
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s3, field);
	PFAddition(red, &partialres2, &s4, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#else // Set chunks to 32 bit

	// Note that a presents 12 chunks

	pfelement s1, s2, s3, s4, partialres1, partialres2;

	// Init partial results data
	chunk s1data[6]; // 32 * 6 = 192
	s1.data = s1data;
	chunk s2data[6];
	s2.data = s2data;
	chunk s3data[6];
	s3.data = s3data;
	chunk s4data[6];
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

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(6, 192, &s1);
	char * s2dump = GetString(6, 192, &s2);
	char * s3dump = GetString(6, 192, &s3);
	char * s4dump = GetString(6, 192, &s4);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// red = s1 + s2 + s3 + s4
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s3, field);
	PFAddition(red, &partialres2, &s4, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#endif
}

void FastReductionFIPSp224(
	pfelement * red,
	pfelement * a, 
	pfproperties * field)
{
#if ARCHITECTURE_BITS == 8

	// Note that a presents 56 chunks

	pfelement s1, s2, s3, s4, s5, partialres1, partialres2;

	// Init partial results data
	chunk s1data[28]; // 8 * 28 = 224
	s1.data = s1data;
	chunk s2data[28];
	s2.data = s2data;
	chunk s3data[28];
	s3.data = s3data;
	chunk s4data[28];
	s4.data = s4data;
	chunk s5data[28];
	s5.data = s5data;

	// Assuming a = (a13,...,a0) (32-bit chunks)
	// s1 = (a6, a5, a4, a3, a2, a1, a0)
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
	s1.data[24] = a->data[24];
	s1.data[25] = a->data[25];
	s1.data[26] = a->data[26];
	s1.data[27] = a->data[27];
	// s2 = (a10, a9, a8, a7, 0, 0, 0)
	s2.data[0] = 0;
	s2.data[1] = 0;
	s2.data[2] = 0;
	s2.data[3] = 0;
	s2.data[4] = 0;
	s2.data[5] = 0;
	s2.data[6] = 0;
	s2.data[7] = 0;
	s2.data[8] = 0;
	s2.data[9] = 0;
	s2.data[10] = 0;
	s2.data[11] = 0;
	s2.data[12] = a->data[28];
	s2.data[13] = a->data[29];
	s2.data[14] = a->data[30];
	s2.data[15] = a->data[31];
	s2.data[16] = a->data[32];
	s2.data[17] = a->data[33];
	s2.data[18] = a->data[34];
	s2.data[19] = a->data[35];
	s2.data[20] = a->data[36];
	s2.data[21] = a->data[37];
	s2.data[22] = a->data[38];
	s2.data[23] = a->data[39];
	s2.data[24] = a->data[40];
	s2.data[25] = a->data[41];
	s2.data[26] = a->data[42];
	s2.data[27] = a->data[43];
	// s3 = (0, a13, a12, a11, 0, 0, 0)
	s3.data[0] = 0;
	s3.data[1] = 0;
	s3.data[2] = 0;
	s3.data[3] = 0;
	s3.data[4] = 0;
	s3.data[5] = 0;
	s3.data[6] = 0;
	s3.data[7] = 0;
	s3.data[8] = 0;
	s3.data[9] = 0;
	s3.data[10] = 0;
	s3.data[11] = 0;
	s3.data[12] = a->data[44];
	s3.data[13] = a->data[45];
	s3.data[14] = a->data[46];
	s3.data[15] = a->data[47];
	s3.data[16] = a->data[48];
	s3.data[17] = a->data[49];
	s3.data[18] = a->data[50];
	s3.data[19] = a->data[51];
	s3.data[20] = a->data[52];
	s3.data[21] = a->data[53];
	s3.data[22] = a->data[54];
	s3.data[23] = a->data[55];
	s3.data[24] = 0;
	s3.data[25] = 0;
	s3.data[26] = 0;
	s3.data[27] = 0;
	// s4 = (a13, a12, a11, a10, a9, a8, a7)
	s4.data[0] = a->data[28];
	s4.data[1] = a->data[29];
	s4.data[2] = a->data[30];
	s4.data[3] = a->data[31];
	s4.data[4] = a->data[32];
	s4.data[5] = a->data[33];
	s4.data[6] = a->data[34];
	s4.data[7] = a->data[35];
	s4.data[8] = a->data[36];
	s4.data[9] = a->data[37];
	s4.data[10] = a->data[38];
	s4.data[11] = a->data[39];
	s4.data[12] = a->data[40];
	s4.data[13] = a->data[41];
	s4.data[14] = a->data[42];
	s4.data[15] = a->data[43];
	s4.data[16] = a->data[44];
	s4.data[17] = a->data[45];
	s4.data[18] = a->data[46];
	s4.data[19] = a->data[47];
	s4.data[20] = a->data[48];
	s4.data[21] = a->data[49];
	s4.data[22] = a->data[50];
	s4.data[23] = a->data[51];
	s4.data[24] = a->data[52];
	s4.data[25] = a->data[53];
	s4.data[26] = a->data[54];
	s4.data[27] = a->data[55];
	// s5 = (0, 0, 0, 0, a13, a12, a11)
	s5.data[0] = a->data[44];
	s5.data[1] = a->data[45];
	s5.data[2] = a->data[46];
	s5.data[3] = a->data[47];
	s5.data[4] = a->data[48];
	s5.data[5] = a->data[49];
	s5.data[6] = a->data[50];
	s5.data[7] = a->data[51];
	s5.data[8] = a->data[52];
	s5.data[9] = a->data[53];
	s5.data[10] = a->data[54];
	s5.data[11] = a->data[55];
	s5.data[12] = 0;
	s5.data[13] = 0;
	s5.data[14] = 0;
	s5.data[15] = 0;
	s5.data[16] = 0;
	s5.data[17] = 0;
	s5.data[18] = 0;
	s5.data[19] = 0;
	s5.data[20] = 0;
	s5.data[21] = 0;
	s5.data[22] = 0;
	s5.data[23] = 0;
	s5.data[24] = 0;
	s5.data[25] = 0;
	s5.data[26] = 0;
	s5.data[27] = 0;

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(28, 224, &s1);
	char * s2dump = GetString(28, 224, &s2);
	char * s3dump = GetString(28, 224, &s3);
	char * s4dump = GetString(28, 224, &s4);
	char * s5dump = GetString(28, 224, &s5);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
	free(s5dump);

#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// red = s1 + s2 + s3 - s4 - s5
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s3, field);
	PFSubtraction(&partialres1, &partialres2, &s4, field);
	PFSubtraction(red, &partialres1, &s5, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#elif ARCHITECTURE_BITS == 16

	// Note that a presents 28 chunks

	pfelement s1, s2, s3, s4, s5, partialres1, partialres2;

	// Init partial results data
	chunk s1data[14]; // 16 * 14 = 224
	s1.data = s1data;
	chunk s2data[14];
	s2.data = s2data;
	chunk s3data[14];
	s3.data = s3data;
	chunk s4data[14];
	s4.data = s4data;
	chunk s5data[14];
	s5.data = s5data;

	// Assuming a = (a13,...,a0) (32-bit chunks)
	// s1 = (a6, a5, a4, a3, a2, a1, a0)
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
	// s2 = (a10, a9, a8, a7, 0, 0, 0)
	s2.data[0] = 0;
	s2.data[1] = 0;
	s2.data[2] = 0;
	s2.data[3] = 0;
	s2.data[4] = 0;
	s2.data[5] = 0;
	s2.data[6] = a->data[14];
	s2.data[7] = a->data[15];
	s2.data[8] = a->data[16];
	s2.data[9] = a->data[17];
	s2.data[10] = a->data[18];
	s2.data[11] = a->data[19];
	s2.data[12] = a->data[20];
	s2.data[13] = a->data[21];
	// s3 = (0, a13, a12, a11, 0, 0, 0)
	s3.data[0] = 0;
	s3.data[1] = 0;
	s3.data[2] = 0;
	s3.data[3] = 0;
	s3.data[4] = 0;
	s3.data[5] = 0;
	s3.data[6] = a->data[22];
	s3.data[7] = a->data[23];
	s3.data[8] = a->data[24];
	s3.data[9] = a->data[25];
	s3.data[10] = a->data[26];
	s3.data[11] = a->data[27];
	s3.data[12] = 0;
	s3.data[13] = 0;
	// s4 = (a13, a12, a11, a10, a9, a8, a7)
	s4.data[0] = a->data[14];
	s4.data[1] = a->data[15];
	s4.data[2] = a->data[16];
	s4.data[3] = a->data[17];
	s4.data[4] = a->data[18];
	s4.data[5] = a->data[19];
	s4.data[6] = a->data[20];
	s4.data[7] = a->data[21];
	s4.data[8] = a->data[22];
	s4.data[9] = a->data[23];
	s4.data[10] = a->data[24];
	s4.data[11] = a->data[25];
	s4.data[12] = a->data[26];
	s4.data[13] = a->data[27];
	// s5 = (0, 0, 0, 0, a13, a12, a11)
	s5.data[0] = a->data[22];
	s5.data[1] = a->data[23];
	s5.data[2] = a->data[24];
	s5.data[3] = a->data[25];
	s5.data[4] = a->data[26];
	s5.data[5] = a->data[27];
	s5.data[6] = 0;
	s5.data[7] = 0;
	s5.data[8] = 0;
	s5.data[9] = 0;
	s5.data[10] = 0;
	s5.data[11] = 0;
	s5.data[12] = 0;
	s5.data[13] = 0;

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(14, 224, &s1);
	char * s2dump = GetString(14, 224, &s2);
	char * s3dump = GetString(14, 224, &s3);
	char * s4dump = GetString(14, 224, &s4);
	char * s5dump = GetString(14, 224, &s5);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
	free(s5dump);
#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// red = s1 + s2 + s3 - s4 - s5
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s3, field);
	PFSubtraction(&partialres1, &partialres2, &s4, field);
	PFSubtraction(red, &partialres1, &s5, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#elif ARCHITECTURE_BITS == 64

	// Note that a presents 7 chunks

	pfelement s1, s2, s3, s4, s5, partialres1, partialres2;

	// Init partial results data
	chunk s1data[4]; // 64 * 4 contains 224
	s1.data = s1data;
	chunk s2data[4];
	s2.data = s2data;
	chunk s3data[4];
	s3.data = s3data;
	chunk s4data[4];
	s4.data = s4data;
	chunk s5data[4];
	s5.data = s5data;

	// Assuming a = (a13,...,a0) (32-bit chunks)
	// s1 = (a6, a5, a4, a3, a2, a1, a0)
	s1.data[0] = a->data[0];
	s1.data[1] = a->data[1];
	s1.data[2] = a->data[2];
	s1.data[3] = a->data[3] & 0x00000000FFFFFFFF;
	// s2 = (a10, a9, a8, a7, 0, 0, 0)
	s2.data[0] = 0;
	s2.data[1] = a->data[3] & 0xFFFFFFFF00000000;
	s2.data[2] = a->data[4];
	s2.data[3] = a->data[5] & 0x00000000FFFFFFFF;
	// s3 = (0, a13, a12, a11, 0, 0, 0)
	s3.data[0] = 0;
	s3.data[1] = a->data[5] & 0xFFFFFFFF00000000;
	s3.data[2] = a->data[6];
	s3.data[3] = 0;
	// s4 = (a13, a12, a11, a10, a9, a8, a7)
	s4.data[0] = (a->data[3] >> 32) + (a->data[4] << 32);
	s4.data[1] = (a->data[4] >> 32) + (a->data[5] << 32);
	s4.data[2] = (a->data[5] >> 32) + (a->data[6] << 32);
	s4.data[3] = a->data[6] >> 32;
	// s5 = (0, 0, 0, 0, a13, a12, a11)
	s5.data[0] = s4.data[2];
	s5.data[1] = s4.data[3];
	s5.data[2] = 0;
	s5.data[3] = 0;

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(7, 224, &s1);
	char * s2dump = GetString(7, 224, &s2);
	char * s3dump = GetString(7, 224, &s3);
	char * s4dump = GetString(7, 224, &s4);
	char * s5dump = GetString(7, 224, &s5);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
	free(s5dump);
#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// red = s1 + s2 + s3 - s4 - s5
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s3, field);
	PFSubtraction(&partialres1, &partialres2, &s4, field);
	PFSubtraction(red, &partialres1, &s5, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#else // Set chunks to 32 bit

	// Note that a presents 14 chunks

	pfelement s1, s2, s3, s4, s5, partialres1, partialres2;

	// Init partial results data
	chunk s1data[7]; // 32 * 7 = 224
	s1.data = s1data;
	chunk s2data[7];
	s2.data = s2data;
	chunk s3data[7];
	s3.data = s3data;
	chunk s4data[7];
	s4.data = s4data;
	chunk s5data[7];
	s5.data = s5data;

	// Assuming a = (a13,...,a0) (32-bit chunks)
	// s1 = (a6, a5, a4, a3, a2, a1, a0)
	s1.data[0] = a->data[0];
	s1.data[1] = a->data[1];
	s1.data[2] = a->data[2];
	s1.data[3] = a->data[3];
	s1.data[4] = a->data[4];
	s1.data[5] = a->data[5];
	s1.data[6] = a->data[6];
	// s2 = (a10, a9, a8, a7, 0, 0, 0)
	s2.data[0] = 0;
	s2.data[1] = 0;
	s2.data[2] = 0;
	s2.data[3] = a->data[7];
	s2.data[4] = a->data[8];
	s2.data[5] = a->data[9];
	s2.data[6] = a->data[10];
	// s3 = (0, a13, a12, a11, 0, 0, 0)
	s3.data[0] = 0;
	s3.data[1] = 0;
	s3.data[2] = 0;
	s3.data[3] = a->data[11];
	s3.data[4] = a->data[12];
	s3.data[5] = a->data[13];
	s3.data[6] = 0;
	// s4 = (a13, a12, a11, a10, a9, a8, a7)
	s4.data[0] = a->data[7];
	s4.data[1] = a->data[8];
	s4.data[2] = a->data[9];
	s4.data[3] = a->data[10];
	s4.data[4] = a->data[11];
	s4.data[5] = a->data[12];
	s4.data[6] = a->data[13];
	// s5 = (0, 0, 0, 0, a13, a12, a11)
	s5.data[0] = a->data[11];
	s5.data[1] = a->data[12];
	s5.data[2] = a->data[13];
	s5.data[3] = 0;
	s5.data[4] = 0;
	s5.data[5] = 0;
	s5.data[6] = 0;

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(7, 224, &s1);
	char * s2dump = GetString(7, 224, &s2);
	char * s3dump = GetString(7, 224, &s3);
	char * s4dump = GetString(7, 224, &s4);
	char * s5dump = GetString(7, 224, &s5);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
	free(s5dump);
#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// red = s1 + s2 + s3 - s4 - s5
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s3, field);
	PFSubtraction(&partialres1, &partialres2, &s4, field);
	PFSubtraction(red, &partialres1, &s5, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#endif
}

void FastReductionFIPSp256(
	pfelement * red,
	pfelement * a,
	pfproperties * field)
{
#if ARCHITECTURE_BITS == 8

	// Note that a presents 64 chunks

	pfelement s1, s2, s3, s4, s5, s6, s7, s8, s9, partialres1, partialres2;

	// Init partial results data
	chunk s1data[32]; // 8 * 32 = 256
	s1.data = s1data;
	chunk s2data[32];
	s2.data = s2data;
	chunk s3data[32];
	s3.data = s3data;
	chunk s4data[32];
	s4.data = s4data;
	chunk s5data[32];
	s5.data = s5data;
	chunk s6data[32];
	s6.data = s6data;
	chunk s7data[32];
	s7.data = s7data;
	chunk s8data[32];
	s8.data = s8data;
	chunk s9data[32];
	s9.data = s9data;

	// Assuming a = (c15,...,c0) (32-bit chunks)
	// s1 = (c7, c6, c5, c4, c3, c2, c1, c0)
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
	s1.data[24] = a->data[24];
	s1.data[25] = a->data[25];
	s1.data[26] = a->data[26];
	s1.data[27] = a->data[27];
	s1.data[28] = a->data[28];
	s1.data[29] = a->data[29];
	s1.data[30] = a->data[30];
	s1.data[31] = a->data[31];
	// s2 = (c15, c14, c13, c12, c11, 0, 0, 0)
	s2.data[0] = 0;
	s2.data[1] = 0;
	s2.data[2] = 0;
	s2.data[3] = 0;
	s2.data[4] = 0;
	s2.data[5] = 0;
	s2.data[6] = 0;
	s2.data[7] = 0;
	s2.data[8] = 0;
	s2.data[9] = 0;
	s2.data[10] = 0;
	s2.data[11] = 0;
	s2.data[12] = a->data[44];
	s2.data[13] = a->data[45];
	s2.data[14] = a->data[46];
	s2.data[15] = a->data[47];
	s2.data[16] = a->data[48];
	s2.data[17] = a->data[49];
	s2.data[18] = a->data[50];
	s2.data[19] = a->data[51];
	s2.data[20] = a->data[52];
	s2.data[21] = a->data[53];
	s2.data[22] = a->data[54];
	s2.data[23] = a->data[55];
	s2.data[24] = a->data[56];
	s2.data[25] = a->data[57];
	s2.data[26] = a->data[58];
	s2.data[27] = a->data[59];
	s2.data[28] = a->data[60];
	s2.data[29] = a->data[61];
	s2.data[30] = a->data[62];
	s2.data[31] = a->data[63];
	// s3 = (0, c15, c14, c13, c12, 0, 0, 0)
	s3.data[0] = 0;
	s3.data[1] = 0;
	s3.data[2] = 0;
	s3.data[3] = 0;
	s3.data[4] = 0;
	s3.data[5] = 0;
	s3.data[6] = 0;
	s3.data[7] = 0;
	s3.data[8] = 0;
	s3.data[9] = 0;
	s3.data[10] = 0;
	s3.data[11] = 0;
	s3.data[12] = a->data[48];
	s3.data[13] = a->data[49];
	s3.data[14] = a->data[50];
	s3.data[15] = a->data[51];
	s3.data[16] = a->data[52];
	s3.data[17] = a->data[53];
	s3.data[18] = a->data[54];
	s3.data[19] = a->data[55];
	s3.data[20] = a->data[56];
	s3.data[21] = a->data[57];
	s3.data[22] = a->data[58];
	s3.data[23] = a->data[59];
	s3.data[24] = a->data[60];
	s3.data[25] = a->data[61];
	s3.data[26] = a->data[62];
	s3.data[27] = a->data[63];
	s3.data[28] = 0;
	s3.data[29] = 0;
	s3.data[30] = 0;
	s3.data[31] = 0;
	// s4 = (c15, c14, 0, 0, 0, c10, c9, c8)
	s4.data[0] = a->data[32];
	s4.data[1] = a->data[33];
	s4.data[2] = a->data[34];
	s4.data[3] = a->data[35];
	s4.data[4] = a->data[36];
	s4.data[5] = a->data[37];
	s4.data[6] = a->data[38];
	s4.data[7] = a->data[39];
	s4.data[8] = a->data[40];
	s4.data[9] = a->data[41];
	s4.data[10] = a->data[42];
	s4.data[11] = a->data[43];
	s4.data[12] = 0;
	s4.data[13] = 0;
	s4.data[14] = 0;
	s4.data[15] = 0;
	s4.data[16] = 0;
	s4.data[17] = 0;
	s4.data[18] = 0;
	s4.data[19] = 0;
	s4.data[20] = 0;
	s4.data[21] = 0;
	s4.data[22] = 0;
	s4.data[23] = 0;
	s4.data[24] = a->data[56];
	s4.data[25] = a->data[57];
	s4.data[26] = a->data[58];
	s4.data[27] = a->data[59];
	s4.data[28] = a->data[60];
	s4.data[29] = a->data[61];
	s4.data[30] = a->data[62];
	s4.data[31] = a->data[63];
	// s5 = (c8, c13, c15, c14, c13, c11, c10, c9)
	s5.data[0] = a->data[36];
	s5.data[1] = a->data[37];
	s5.data[2] = a->data[38];
	s5.data[3] = a->data[39];
	s5.data[4] = a->data[40];
	s5.data[5] = a->data[41];
	s5.data[6] = a->data[42];
	s5.data[7] = a->data[43];
	s5.data[8] = a->data[44];
	s5.data[9] = a->data[45];
	s5.data[10] = a->data[46];
	s5.data[11] = a->data[47];
	s5.data[12] = a->data[52];
	s5.data[13] = a->data[53];
	s5.data[14] = a->data[54];
	s5.data[15] = a->data[55];
	s5.data[16] = a->data[56];
	s5.data[17] = a->data[57];
	s5.data[18] = a->data[58];
	s5.data[19] = a->data[59];
	s5.data[20] = a->data[60];
	s5.data[21] = a->data[61];
	s5.data[22] = a->data[62];
	s5.data[23] = a->data[63];
	s5.data[24] = a->data[52];
	s5.data[25] = a->data[53];
	s5.data[26] = a->data[54];
	s5.data[27] = a->data[55];
	s5.data[28] = a->data[32];
	s5.data[29] = a->data[33];
	s5.data[30] = a->data[34];
	s5.data[31] = a->data[35];
	// s6 = (c10, c8, 0, 0, 0, c13, c12, c11)
	s6.data[0] = a->data[44];
	s6.data[1] = a->data[45];
	s6.data[2] = a->data[46];
	s6.data[3] = a->data[47];
	s6.data[4] = a->data[48];
	s6.data[5] = a->data[49];
	s6.data[6] = a->data[50];
	s6.data[7] = a->data[51];
	s6.data[8] = a->data[52];
	s6.data[9] = a->data[53];
	s6.data[10] = a->data[54];
	s6.data[11] = a->data[55];
	s6.data[12] = 0;
	s6.data[13] = 0;
	s6.data[14] = 0;
	s6.data[15] = 0;
	s6.data[16] = 0;
	s6.data[17] = 0;
	s6.data[18] = 0;
	s6.data[19] = 0;
	s6.data[20] = 0;
	s6.data[21] = 0;
	s6.data[22] = 0;
	s6.data[23] = 0;
	s6.data[24] = a->data[32];
	s6.data[25] = a->data[33];
	s6.data[26] = a->data[34];
	s6.data[27] = a->data[35];
	s6.data[28] = a->data[40];
	s6.data[29] = a->data[41];
	s6.data[30] = a->data[42];
	s6.data[31] = a->data[43];
	// s7 = (c11, c9, 0, 0, c15, c14, c13, c12)
	s7.data[0] = a->data[48];
	s7.data[1] = a->data[49];
	s7.data[2] = a->data[50];
	s7.data[3] = a->data[51];
	s7.data[4] = a->data[52];
	s7.data[5] = a->data[53];
	s7.data[6] = a->data[54];
	s7.data[7] = a->data[55];
	s7.data[8] = a->data[56];
	s7.data[9] = a->data[57];
	s7.data[10] = a->data[58];
	s7.data[11] = a->data[59];
	s7.data[12] = a->data[60];
	s7.data[13] = a->data[61];
	s7.data[14] = a->data[62];
	s7.data[15] = a->data[63];
	s7.data[16] = 0;
	s7.data[17] = 0;
	s7.data[18] = 0;
	s7.data[19] = 0;
	s7.data[20] = 0;
	s7.data[21] = 0;
	s7.data[22] = 0;
	s7.data[23] = 0;
	s7.data[24] = a->data[36];
	s7.data[25] = a->data[37];
	s7.data[26] = a->data[38];
	s7.data[27] = a->data[39];
	s7.data[28] = a->data[44];
	s7.data[29] = a->data[45];
	s7.data[30] = a->data[46];
	s7.data[31] = a->data[47];
	// s8 = (c12, 0, c10, c9, c8, c15, c14, c13)
	s8.data[0] = a->data[52];
	s8.data[1] = a->data[53];
	s8.data[2] = a->data[54];
	s8.data[3] = a->data[55];
	s8.data[4] = a->data[56];
	s8.data[5] = a->data[57];
	s8.data[6] = a->data[58];
	s8.data[7] = a->data[59];
	s8.data[8] = a->data[60];
	s8.data[9] = a->data[61];
	s8.data[10] = a->data[62];
	s8.data[11] = a->data[63];
	s8.data[12] = a->data[32];
	s8.data[13] = a->data[33];
	s8.data[14] = a->data[34];
	s8.data[15] = a->data[35];
	s8.data[16] = a->data[36];
	s8.data[17] = a->data[37];
	s8.data[18] = a->data[38];
	s8.data[19] = a->data[39];
	s8.data[20] = a->data[40];
	s8.data[21] = a->data[41];
	s8.data[22] = a->data[42];
	s8.data[23] = a->data[43];
	s8.data[24] = 0;
	s8.data[25] = 0;
	s8.data[26] = 0;
	s8.data[27] = 0;
	s8.data[28] = a->data[48];
	s8.data[29] = a->data[49];
	s8.data[30] = a->data[50];
	s8.data[31] = a->data[51];
	// s9 = (c13, 0, c11, c10, c9, 0, c15, c14)
	s9.data[0] = a->data[56];
	s9.data[1] = a->data[57];
	s9.data[2] = a->data[58];
	s9.data[3] = a->data[59];
	s9.data[4] = a->data[60];
	s9.data[5] = a->data[61];
	s9.data[6] = a->data[62];
	s9.data[7] = a->data[63];
	s9.data[8] = 0;
	s9.data[9] = 0;
	s9.data[10] = 0;
	s9.data[11] = 0;
	s9.data[12] = a->data[36];
	s9.data[13] = a->data[37];
	s9.data[14] = a->data[38];
	s9.data[15] = a->data[39];
	s9.data[16] = a->data[40];
	s9.data[17] = a->data[41];
	s9.data[18] = a->data[42];
	s9.data[19] = a->data[43];
	s9.data[20] = a->data[44];
	s9.data[21] = a->data[45];
	s9.data[22] = a->data[46];
	s9.data[23] = a->data[47];
	s9.data[24] = 0;
	s9.data[25] = 0;
	s9.data[26] = 0;
	s9.data[27] = 0;
	s9.data[28] = a->data[52];
	s9.data[29] = a->data[53];
	s9.data[30] = a->data[54];
	s9.data[31] = a->data[55];

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(32, 256, &s1);
	char * s2dump = GetString(32, 256, &s2);
	char * s3dump = GetString(32, 256, &s3);
	char * s4dump = GetString(32, 256, &s4);
	char * s5dump = GetString(32, 256, &s5);
	char * s6dump = GetString(32, 256, &s6);
	char * s7dump = GetString(32, 256, &s7);
	char * s8dump = GetString(32, 256, &s8);
	char * s9dump = GetString(32, 256, &s9);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
	free(s5dump);
	free(s6dump);
	free(s7dump);
	free(s8dump);
	free(s9dump);
#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// s1 + 2*s2 + 2*s3 + s4 + s5 − s6 − s7 − s8 − s9
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s2, field);
	PFAddition(&partialres1, &partialres2, &s3, field);
	PFAddition(&partialres2, &partialres1, &s3, field);
	PFAddition(&partialres1, &partialres2, &s4, field);
	PFAddition(&partialres2, &partialres1, &s5, field);
	PFSubtraction(&partialres1, &partialres2, &s6, field);
	PFSubtraction(&partialres2, &partialres1, &s7, field);
	PFSubtraction(&partialres1, &partialres2, &s8, field);
	PFSubtraction(red, &partialres1, &s9, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#elif ARCHITECTURE_BITS == 16

	// Note that a presents 32 chunks

	pfelement s1, s2, s3, s4, s5, s6, s7, s8, s9, partialres1, partialres2;

	// Init partial results data
	chunk s1data[16]; // 16 * 16 = 256
	s1.data = s1data;
	chunk s2data[16];
	s2.data = s2data;
	chunk s3data[16];
	s3.data = s3data;
	chunk s4data[16];
	s4.data = s4data;
	chunk s5data[16];
	s5.data = s5data;
	chunk s6data[16];
	s6.data = s6data;
	chunk s7data[16];
	s7.data = s7data;
	chunk s8data[16];
	s8.data = s8data;
	chunk s9data[16];
	s9.data = s9data;

	// Assuming a = (c15,...,c0) (32-bit chunks)
	// s1 = (c7, c6, c5, c4, c3, c2, c1, c0)
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
	// s2 = (c15, c14, c13, c12, c11, 0, 0, 0)
	s2.data[0] = 0;
	s2.data[1] = 0;
	s2.data[2] = 0;
	s2.data[3] = 0;
	s2.data[4] = 0;
	s2.data[5] = 0;
	s2.data[6] = a->data[22];
	s2.data[7] = a->data[23];
	s2.data[8] = a->data[24];
	s2.data[9] = a->data[25];
	s2.data[10] = a->data[26];
	s2.data[11] = a->data[27];
	s2.data[12] = a->data[28];
	s2.data[13] = a->data[29];
	s2.data[14] = a->data[30];
	s2.data[15] = a->data[31];
	// s3 = (0, c15, c14, c13, c12, 0, 0, 0)
	s3.data[0] = 0;
	s3.data[1] = 0;
	s3.data[2] = 0;
	s3.data[3] = 0;
	s3.data[4] = 0;
	s3.data[5] = 0;
	s3.data[6] = a->data[24];
	s3.data[7] = a->data[25];
	s3.data[8] = a->data[26];
	s3.data[9] = a->data[27];
	s3.data[10] = a->data[28];
	s3.data[11] = a->data[29];
	s3.data[12] = a->data[30];
	s3.data[13] = a->data[31];
	s3.data[14] = 0;
	s3.data[15] = 0;
	// s4 = (c15, c14, 0, 0, 0, c10, c9, c8)
	s4.data[0] = a->data[16];
	s4.data[1] = a->data[17];
	s4.data[2] = a->data[18];
	s4.data[3] = a->data[19];
	s4.data[4] = a->data[20];
	s4.data[5] = a->data[21];
	s4.data[6] = 0;
	s4.data[7] = 0;
	s4.data[8] = 0;
	s4.data[9] = 0;
	s4.data[10] = 0;
	s4.data[11] = 0;
	s4.data[12] = a->data[28];
	s4.data[13] = a->data[29];
	s4.data[14] = a->data[30];
	s4.data[15] = a->data[31];
	// s5 = (c8, c13, c15, c14, c13, c11, c10, c9)
	s5.data[0] = a->data[18];
	s5.data[1] = a->data[19];
	s5.data[2] = a->data[20];
	s5.data[3] = a->data[21];
	s5.data[4] = a->data[22];
	s5.data[5] = a->data[23];
	s5.data[6] = a->data[26];
	s5.data[7] = a->data[27];
	s5.data[8] = a->data[28];
	s5.data[9] = a->data[29];
	s5.data[10] = a->data[30];
	s5.data[11] = a->data[31];
	s5.data[12] = a->data[26];
	s5.data[13] = a->data[27];
	s5.data[14] = a->data[16];
	s5.data[15] = a->data[17];
	// s6 = (c10, c8, 0, 0, 0, c13, c12, c11)
	s6.data[0] = a->data[22];
	s6.data[1] = a->data[23];
	s6.data[2] = a->data[24];
	s6.data[3] = a->data[25];
	s6.data[4] = a->data[26];
	s6.data[5] = a->data[27];
	s6.data[6] = 0;
	s6.data[7] = 0;
	s6.data[8] = 0;
	s6.data[9] = 0;
	s6.data[10] = 0;
	s6.data[11] = 0;
	s6.data[12] = a->data[16];
	s6.data[13] = a->data[17];
	s6.data[14] = a->data[20];
	s6.data[15] = a->data[21];
	// s7 = (c11, c9, 0, 0, c15, c14, c13, c12)
	s7.data[0] = a->data[24];
	s7.data[1] = a->data[25];
	s7.data[2] = a->data[26];
	s7.data[3] = a->data[27];
	s7.data[4] = a->data[28];
	s7.data[5] = a->data[29];
	s7.data[6] = a->data[30];
	s7.data[7] = a->data[31];
	s7.data[8] = 0;
	s7.data[9] = 0;
	s7.data[10] = 0;
	s7.data[11] = 0;
	s7.data[12] = a->data[18];
	s7.data[13] = a->data[19];
	s7.data[14] = a->data[22];
	s7.data[15] = a->data[23];
	// s8 = (c12, 0, c10, c9, c8, c15, c14, c13)
	s8.data[0] = a->data[26];
	s8.data[1] = a->data[27];
	s8.data[2] = a->data[28];
	s8.data[3] = a->data[29];
	s8.data[4] = a->data[30];
	s8.data[5] = a->data[31];
	s8.data[6] = a->data[16];
	s8.data[7] = a->data[17];
	s8.data[8] = a->data[18];
	s8.data[9] = a->data[19];
	s8.data[10] = a->data[20];
	s8.data[11] = a->data[21];
	s8.data[12] = 0;
	s8.data[13] = 0;
	s8.data[14] = a->data[24];
	s8.data[15] = a->data[25];
	// s9 = (c13, 0, c11, c10, c9, 0, c15, c14)
	s9.data[0] = a->data[28];
	s9.data[1] = a->data[29];
	s9.data[2] = a->data[30];
	s9.data[3] = a->data[31];
	s9.data[4] = 0;
	s9.data[5] = 0;
	s9.data[6] = a->data[18];
	s9.data[7] = a->data[19];
	s9.data[8] = a->data[20];
	s9.data[9] = a->data[21];
	s9.data[10] = a->data[22];
	s9.data[11] = a->data[23];
	s9.data[12] = 0;
	s9.data[13] = 0;
	s9.data[14] = a->data[26];
	s9.data[15] = a->data[27];

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(16, 256, &s1);
	char * s2dump = GetString(16, 256, &s2);
	char * s3dump = GetString(16, 256, &s3);
	char * s4dump = GetString(16, 256, &s4);
	char * s5dump = GetString(16, 256, &s5);
	char * s6dump = GetString(16, 256, &s6);
	char * s7dump = GetString(16, 256, &s7);
	char * s8dump = GetString(16, 256, &s8);
	char * s9dump = GetString(16, 256, &s9);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
	free(s5dump);
	free(s6dump);
	free(s7dump);
	free(s8dump);
	free(s9dump);
#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// s1 + 2*s2 + 2*s3 + s4 + s5 − s6 − s7 − s8 − s9
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s2, field);
	PFAddition(&partialres1, &partialres2, &s3, field);
	PFAddition(&partialres2, &partialres1, &s3, field);
	PFAddition(&partialres1, &partialres2, &s4, field);
	PFAddition(&partialres2, &partialres1, &s5, field);
	PFSubtraction(&partialres1, &partialres2, &s6, field);
	PFSubtraction(&partialres2, &partialres1, &s7, field);
	PFSubtraction(&partialres1, &partialres2, &s8, field);
	PFSubtraction(red, &partialres1, &s9, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#elif ARCHITECTURE_BITS == 64

	// Note that a presents 8 chunks

	pfelement s1, s2, s3, s4, s5, s6, s7, s8, s9, partialres1, partialres2;

	// Init partial results data
	chunk s1data[4]; // 64 * 4 = 256
	s1.data = s1data;
	chunk s2data[4];
	s2.data = s2data;
	chunk s3data[4];
	s3.data = s3data;
	chunk s4data[4];
	s4.data = s4data;
	chunk s5data[4];
	s5.data = s5data;
	chunk s6data[4];
	s6.data = s6data;
	chunk s7data[4];
	s7.data = s7data;
	chunk s8data[4];
	s8.data = s8data;
	chunk s9data[4];
	s9.data = s9data;

	// Assuming a = (c15,...,c0) (32-bit chunks)
	// s1 = (c7, c6, c5, c4, c3, c2, c1, c0)
	s1.data[0] = a->data[0];
	s1.data[1] = a->data[1];
	s1.data[2] = a->data[2];
	s1.data[3] = a->data[3];
	// s2 = (c15, c14, c13, c12, c11, 0, 0, 0)
	s2.data[0] = 0;
	s2.data[1] = a->data[5] & 0xffffffff00000000;
	s2.data[2] = a->data[6];
	s2.data[3] = a->data[7];
	// s3 = (0, c15, c14, c13, c12, 0, 0, 0)
	s3.data[0] = 0;
	s3.data[1] = a->data[6] << 32;
	s3.data[2] = (a->data[7] << 32) + (a->data[6] >> 32);
	s3.data[3] = a->data[7] >> 32;
	// s4 = (c15, c14, 0, 0, 0, c10, c9, c8)
	s4.data[0] = a->data[4];
	s4.data[1] = a->data[5] & 0x00000000ffffffff;
	s4.data[2] = 0;
	s4.data[3] = a->data[7];
	// s5 = (c8, c13, c15, c14, c13, c11, c10, c9)
	s5.data[0] = (a->data[5] << 32) + (a->data[4] >> 32);
	s5.data[1] = (a->data[6] & 0xffffffff00000000) + (a->data[5] >> 32);
	s5.data[2] = a->data[7];
	s5.data[3] = (a->data[4] << 32) + (a->data[6] >> 32);
	// s6 = (c10, c8, 0, 0, 0, c13, c12, c11)
	s6.data[0] = (a->data[6] << 32) + (a->data[5] >> 32);
	s6.data[1] = a->data[6] >> 32;
	s6.data[2] = 0;
	s6.data[3] = (a->data[5] << 32) + (a->data[4] & 0x00000000ffffffff);
	// s7 = (c11, c9, 0, 0, c15, c14, c13, c12)
	s7.data[0] = a->data[6];
	s7.data[1] = a->data[7];
	s7.data[2] = 0;
	s7.data[3] = (a->data[5] & 0xffffffff00000000) + (a->data[4] >> 32);
	// s8 = (c12, 0, c10, c9, c8, c15, c14, c13)
	s8.data[0] = (a->data[7] << 32) + (a->data[6] >> 32);
	s8.data[1] = (a->data[4] << 32) + (a->data[7] >> 32);
	s8.data[2] = (a->data[5] << 32) + (a->data[4] >> 32);
	s8.data[3] = a->data[6] << 32;
	// s9 = (c13, 0, c11, c10, c9, 0, c15, c14)
	s9.data[0] = a->data[7];
	s9.data[1] = a->data[4] & 0xffffffff00000000;
	s9.data[2] = a->data[5];
	s9.data[3] = a->data[6] & 0xffffffff00000000;

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(4, 256, &s1);
	char * s2dump = GetString(4, 256, &s2);
	char * s3dump = GetString(4, 256, &s3);
	char * s4dump = GetString(4, 256, &s4);
	char * s5dump = GetString(4, 256, &s5);
	char * s6dump = GetString(4, 256, &s6);
	char * s7dump = GetString(4, 256, &s7);
	char * s8dump = GetString(4, 256, &s8);
	char * s9dump = GetString(4, 256, &s9);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
	free(s5dump);
	free(s6dump);
	free(s7dump);
	free(s8dump);
	free(s9dump);
#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// s1 + 2*s2 + 2*s3 + s4 + s5 − s6 − s7 − s8 − s9
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s2, field);
	PFAddition(&partialres1, &partialres2, &s3, field);
	PFAddition(&partialres2, &partialres1, &s3, field);
	PFAddition(&partialres1, &partialres2, &s4, field);
	PFAddition(&partialres2, &partialres1, &s5, field);
	PFSubtraction(&partialres1, &partialres2, &s6, field);
	PFSubtraction(&partialres2, &partialres1, &s7, field);
	PFSubtraction(&partialres1, &partialres2, &s8, field);
	PFSubtraction(red, &partialres1, &s9, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#else // Set chunks to 32 bit

	// Note that a presents 16 chunks

	pfelement s1, s2, s3, s4, s5, s6, s7, s8, s9, partialres1, partialres2;

	// Init partial results data
	chunk s1data[8]; // 32 * 8 = 256
	s1.data = s1data;
	chunk s2data[8];
	s2.data = s2data;
	chunk s3data[8];
	s3.data = s3data;
	chunk s4data[8];
	s4.data = s4data;
	chunk s5data[8];
	s5.data = s5data;
	chunk s6data[8];
	s6.data = s6data;
	chunk s7data[8];
	s7.data = s7data;
	chunk s8data[8];
	s8.data = s8data;
	chunk s9data[8];
	s9.data = s9data;

	// Assuming a = (c15,...,c0) (32-bit chunks)
	// s1 = (c7, c6, c5, c4, c3, c2, c1, c0)
	s1.data[0] = a->data[0];
	s1.data[1] = a->data[1];
	s1.data[2] = a->data[2];
	s1.data[3] = a->data[3];
	s1.data[4] = a->data[4];
	s1.data[5] = a->data[5];
	s1.data[6] = a->data[6];
	s1.data[7] = a->data[7];
	// s2 = (c15, c14, c13, c12, c11, 0, 0, 0)
	s2.data[0] = 0;
	s2.data[1] = 0;
	s2.data[2] = 0;
	s2.data[3] = a->data[11];
	s2.data[4] = a->data[12];
	s2.data[5] = a->data[13];
	s2.data[6] = a->data[14];
	s2.data[7] = a->data[15];
	// s3 = (0, c15, c14, c13, c12, 0, 0, 0)
	s3.data[0] = 0;
	s3.data[1] = 0;
	s3.data[2] = 0;
	s3.data[3] = a->data[12];
	s3.data[4] = a->data[13];
	s3.data[5] = a->data[14];
	s3.data[6] = a->data[15];
	s3.data[7] = 0;
	// s4 = (c15, c14, 0, 0, 0, c10, c9, c8)
	s4.data[0] = a->data[8];
	s4.data[1] = a->data[9];
	s4.data[2] = a->data[10];
	s4.data[3] = 0;
	s4.data[4] = 0;
	s4.data[5] = 0;
	s4.data[6] = a->data[14];
	s4.data[7] = a->data[15];
	// s5 = (c8, c13, c15, c14, c13, c11, c10, c9)
	s5.data[0] = a->data[9];
	s5.data[1] = a->data[10];
	s5.data[2] = a->data[11];
	s5.data[3] = a->data[13];
	s5.data[4] = a->data[14];
	s5.data[5] = a->data[15];
	s5.data[6] = a->data[13];
	s5.data[7] = a->data[8];
	// s6 = (c10, c8, 0, 0, 0, c13, c12, c11)
	s6.data[0] = a->data[11];
	s6.data[1] = a->data[12];
	s6.data[2] = a->data[13];
	s6.data[3] = 0;
	s6.data[4] = 0;
	s6.data[5] = 0;
	s6.data[6] = a->data[8];
	s6.data[7] = a->data[10];
	// s7 = (c11, c9, 0, 0, c15, c14, c13, c12)
	s7.data[0] = a->data[12];
	s7.data[1] = a->data[13];
	s7.data[2] = a->data[14];
	s7.data[3] = a->data[15];
	s7.data[4] = 0;
	s7.data[5] = 0;
	s7.data[6] = a->data[9];
	s7.data[7] = a->data[11];
	// s8 = (c12, 0, c10, c9, c8, c15, c14, c13)
	s8.data[0] = a->data[13];
	s8.data[1] = a->data[14];
	s8.data[2] = a->data[15];
	s8.data[3] = a->data[8];
	s8.data[4] = a->data[9];
	s8.data[5] = a->data[10];
	s8.data[6] = 0;
	s8.data[7] = a->data[12];
	// s9 = (c13, 0, c11, c10, c9, 0, c15, c14)
	s9.data[0] = a->data[14];
	s9.data[1] = a->data[15];
	s9.data[2] = 0;
	s9.data[3] = a->data[9];
	s9.data[4] = a->data[10];
	s9.data[5] = a->data[11];
	s9.data[6] = 0;
	s9.data[7] = a->data[13];

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(8, 256, &s1);
	char * s2dump = GetString(8, 256, &s2);
	char * s3dump = GetString(8, 256, &s3);
	char * s4dump = GetString(8, 256, &s4);
	char * s5dump = GetString(8, 256, &s5);
	char * s6dump = GetString(8, 256, &s6);
	char * s7dump = GetString(8, 256, &s7);
	char * s8dump = GetString(8, 256, &s8);
	char * s9dump = GetString(8, 256, &s9);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
	free(s5dump);
	free(s6dump);
	free(s7dump);
	free(s8dump);
	free(s9dump);
#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// s1 + 2*s2 + 2*s3 + s4 + s5 − s6 − s7 − s8 − s9
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s2, field);
	PFAddition(&partialres1, &partialres2, &s3, field);
	PFAddition(&partialres2, &partialres1, &s3, field);
	PFAddition(&partialres1, &partialres2, &s4, field);
	PFAddition(&partialres2, &partialres1, &s5, field);
	PFSubtraction(&partialres1, &partialres2, &s6, field);
	PFSubtraction(&partialres2, &partialres1, &s7, field);
	PFSubtraction(&partialres1, &partialres2, &s8, field);
	PFSubtraction(red, &partialres1, &s9, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#endif
}

void FastReductionFIPSp384(
	pfelement * red, 
	pfelement * a, 
	pfproperties * field)
{
#if ARCHITECTURE_BITS == 8

	// Note that a presents 96 chunks

	pfelement s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, partialres1, partialres2;

	// Init partial results data
	chunk s1data[48]; // 8 * 48 = 384
	s1.data = s1data;
	chunk s2data[48];
	s2.data = s2data;
	chunk s3data[48];
	s3.data = s3data;
	chunk s4data[48];
	s4.data = s4data;
	chunk s5data[48];
	s5.data = s5data;
	chunk s6data[48];
	s6.data = s6data;
	chunk s7data[48];
	s7.data = s7data;
	chunk s8data[48];
	s8.data = s8data;
	chunk s9data[48];
	s9.data = s9data;
	chunk s10data[48];
	s10.data = s10data;

	// Assuming a = (c23,...,c0) (32-bit chunks)
	// s1 = (c11, c10, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0)
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
	s1.data[24] = a->data[24];
	s1.data[25] = a->data[25];
	s1.data[26] = a->data[26];
	s1.data[27] = a->data[27];
	s1.data[28] = a->data[28];
	s1.data[29] = a->data[29];
	s1.data[30] = a->data[30];
	s1.data[31] = a->data[31];
	s1.data[32] = a->data[32];
	s1.data[33] = a->data[33];
	s1.data[34] = a->data[34];
	s1.data[35] = a->data[35];
	s1.data[36] = a->data[36];
	s1.data[37] = a->data[37];
	s1.data[38] = a->data[38];
	s1.data[39] = a->data[39];
	s1.data[40] = a->data[40];
	s1.data[41] = a->data[41];
	s1.data[42] = a->data[42];
	s1.data[43] = a->data[43];
	s1.data[44] = a->data[44];
	s1.data[45] = a->data[45];
	s1.data[46] = a->data[46];
	s1.data[47] = a->data[47];
	// s2 = (0 ,0, 0, 0, 0, c23, c22, c21, 0, 0, 0, 0)
	s2.data[0] = 0;
	s2.data[1] = 0;
	s2.data[2] = 0;
	s2.data[3] = 0;
	s2.data[4] = 0;
	s2.data[5] = 0;
	s2.data[6] = 0;
	s2.data[7] = 0;
	s2.data[8] = 0;
	s2.data[9] = 0;
	s2.data[10] = 0;
	s2.data[11] = 0;
	s2.data[12] = 0;
	s2.data[13] = 0;
	s2.data[14] = 0;
	s2.data[15] = 0;
	s2.data[16] = a->data[84];
	s2.data[17] = a->data[85];
	s2.data[18] = a->data[86];
	s2.data[19] = a->data[87];
	s2.data[20] = a->data[88];
	s2.data[21] = a->data[89];
	s2.data[22] = a->data[90];
	s2.data[23] = a->data[91];
	s2.data[24] = a->data[92];
	s2.data[25] = a->data[93];
	s2.data[26] = a->data[94];
	s2.data[27] = a->data[95];
	s2.data[28] = 0;
	s2.data[29] = 0;
	s2.data[30] = 0;
	s2.data[31] = 0;
	s2.data[32] = 0;
	s2.data[33] = 0;
	s2.data[34] = 0;
	s2.data[35] = 0;
	s2.data[36] = 0;
	s2.data[37] = 0;
	s2.data[38] = 0;
	s2.data[39] = 0;
	s2.data[40] = 0;
	s2.data[41] = 0;
	s2.data[42] = 0;
	s2.data[43] = 0;
	s2.data[44] = 0;
	s2.data[45] = 0;
	s2.data[46] = 0;
	s2.data[47] = 0;
	// s3 = (c23, c22, c21, c20, c19, c18, c17, c16, c15, c14, c13, c12)
	s3.data[0] = a->data[48];
	s3.data[1] = a->data[49];
	s3.data[2] = a->data[50];
	s3.data[3] = a->data[51];
	s3.data[4] = a->data[52];
	s3.data[5] = a->data[53];
	s3.data[6] = a->data[54];
	s3.data[7] = a->data[55];
	s3.data[8] = a->data[56];
	s3.data[9] = a->data[57];
	s3.data[10] = a->data[58];
	s3.data[11] = a->data[59];
	s3.data[12] = a->data[60];
	s3.data[13] = a->data[61];
	s3.data[14] = a->data[62];
	s3.data[15] = a->data[63];
	s3.data[16] = a->data[64];
	s3.data[17] = a->data[65];
	s3.data[18] = a->data[66];
	s3.data[19] = a->data[67];
	s3.data[20] = a->data[68];
	s3.data[21] = a->data[69];
	s3.data[22] = a->data[70];
	s3.data[23] = a->data[71];
	s3.data[24] = a->data[72];
	s3.data[25] = a->data[73];
	s3.data[26] = a->data[74];
	s3.data[27] = a->data[75];
	s3.data[28] = a->data[76];
	s3.data[29] = a->data[77];
	s3.data[30] = a->data[78];
	s3.data[31] = a->data[79];
	s3.data[32] = a->data[80];
	s3.data[33] = a->data[81];
	s3.data[34] = a->data[82];
	s3.data[35] = a->data[83];
	s3.data[36] = a->data[84];
	s3.data[37] = a->data[85];
	s3.data[38] = a->data[86];
	s3.data[39] = a->data[87];
	s3.data[40] = a->data[88];
	s3.data[41] = a->data[89];
	s3.data[42] = a->data[90];
	s3.data[43] = a->data[91];
	s3.data[44] = a->data[92];
	s3.data[45] = a->data[93];
	s3.data[46] = a->data[94];
	s3.data[47] = a->data[95];
	// s4 = (c20, c19, c18, c17, c16, c15, c14, c13, c12, c23, c22, c21)
	s4.data[0] = a->data[84];
	s4.data[1] = a->data[85];
	s4.data[2] = a->data[86];
	s4.data[3] = a->data[87];
	s4.data[4] = a->data[88];
	s4.data[5] = a->data[89];
	s4.data[6] = a->data[90];
	s4.data[7] = a->data[91];
	s4.data[8] = a->data[92];
	s4.data[9] = a->data[93];
	s4.data[10] = a->data[94];
	s4.data[11] = a->data[95];
	s4.data[12] = a->data[48];
	s4.data[13] = a->data[49];
	s4.data[14] = a->data[50];
	s4.data[15] = a->data[51];
	s4.data[16] = a->data[52];
	s4.data[17] = a->data[53];
	s4.data[18] = a->data[54];
	s4.data[19] = a->data[55];
	s4.data[20] = a->data[56];
	s4.data[21] = a->data[57];
	s4.data[22] = a->data[58];
	s4.data[23] = a->data[59];
	s4.data[24] = a->data[60];
	s4.data[25] = a->data[61];
	s4.data[26] = a->data[62];
	s4.data[27] = a->data[63];
	s4.data[28] = a->data[64];
	s4.data[29] = a->data[65];
	s4.data[30] = a->data[66];
	s4.data[31] = a->data[67];
	s4.data[32] = a->data[68];
	s4.data[33] = a->data[69];
	s4.data[34] = a->data[70];
	s4.data[35] = a->data[71];
	s4.data[36] = a->data[72];
	s4.data[37] = a->data[73];
	s4.data[38] = a->data[74];
	s4.data[39] = a->data[75];
	s4.data[40] = a->data[76];
	s4.data[41] = a->data[77];
	s4.data[42] = a->data[78];
	s4.data[43] = a->data[79];
	s4.data[44] = a->data[80];
	s4.data[45] = a->data[81];
	s4.data[46] = a->data[82];
	s4.data[47] = a->data[83];
	// s5 = (c19, c18, c17, c16, c15, c14, c13, c12, c20, 0, c23, 0)
	s5.data[0] = 0;
	s5.data[1] = 0;
	s5.data[2] = 0;
	s5.data[3] = 0;
	s5.data[4] = a->data[92];
	s5.data[5] = a->data[93];
	s5.data[6] = a->data[94];
	s5.data[7] = a->data[95];
	s5.data[8] = 0;
	s5.data[9] = 0;
	s5.data[10] = 0;
	s5.data[11] = 0;
	s5.data[12] = a->data[80];
	s5.data[13] = a->data[81];
	s5.data[14] = a->data[82];
	s5.data[15] = a->data[83];
	s5.data[16] = a->data[48];
	s5.data[17] = a->data[49];
	s5.data[18] = a->data[50];
	s5.data[19] = a->data[51];
	s5.data[20] = a->data[52];
	s5.data[21] = a->data[53];
	s5.data[22] = a->data[54];
	s5.data[23] = a->data[55];
	s5.data[24] = a->data[56];
	s5.data[25] = a->data[57];
	s5.data[26] = a->data[58];
	s5.data[27] = a->data[59];
	s5.data[28] = a->data[60];
	s5.data[29] = a->data[61];
	s5.data[30] = a->data[62];
	s5.data[31] = a->data[63];
	s5.data[32] = a->data[64];
	s5.data[33] = a->data[65];
	s5.data[34] = a->data[66];
	s5.data[35] = a->data[67];
	s5.data[36] = a->data[68];
	s5.data[37] = a->data[69];
	s5.data[38] = a->data[70];
	s5.data[39] = a->data[71];
	s5.data[40] = a->data[72];
	s5.data[41] = a->data[73];
	s5.data[42] = a->data[74];
	s5.data[43] = a->data[75];
	s5.data[44] = a->data[76];
	s5.data[45] = a->data[77];
	s5.data[46] = a->data[78];
	s5.data[47] = a->data[79];
	// s6 = (0, 0, 0, 0, c23, c22, c21, c20, 0, 0, 0, 0)
	s6.data[0] = 0;
	s6.data[1] = 0;
	s6.data[2] = 0;
	s6.data[3] = 0;
	s6.data[4] = 0;
	s6.data[5] = 0;
	s6.data[6] = 0;
	s6.data[7] = 0;
	s6.data[8] = 0;
	s6.data[9] = 0;
	s6.data[10] = 0;
	s6.data[11] = 0;
	s6.data[12] = 0;
	s6.data[13] = 0;
	s6.data[14] = 0;
	s6.data[15] = 0;
	s6.data[16] = a->data[80];
	s6.data[17] = a->data[81];
	s6.data[18] = a->data[82];
	s6.data[19] = a->data[83];
	s6.data[20] = a->data[84];
	s6.data[21] = a->data[85];
	s6.data[22] = a->data[86];
	s6.data[23] = a->data[87];
	s6.data[24] = a->data[88];
	s6.data[25] = a->data[89];
	s6.data[26] = a->data[90];
	s6.data[27] = a->data[91];
	s6.data[28] = a->data[92];
	s6.data[29] = a->data[93];
	s6.data[30] = a->data[94];
	s6.data[31] = a->data[95];
	s6.data[32] = 0;
	s6.data[33] = 0;
	s6.data[34] = 0;
	s6.data[35] = 0;
	s6.data[36] = 0;
	s6.data[37] = 0;
	s6.data[38] = 0;
	s6.data[39] = 0;
	s6.data[40] = 0;
	s6.data[41] = 0;
	s6.data[42] = 0;
	s6.data[43] = 0;
	s6.data[44] = 0;
	s6.data[45] = 0;
	s6.data[46] = 0;
	s6.data[47] = 0;
	// s7 = (0, 0, 0, 0, 0, 0, c23, c22, c21, 0, 0, c20)
	s7.data[0] = a->data[80];
	s7.data[1] = a->data[81];
	s7.data[2] = a->data[82];
	s7.data[3] = a->data[83];
	s7.data[4] = 0;
	s7.data[5] = 0;
	s7.data[6] = 0;
	s7.data[7] = 0;
	s7.data[8] = 0;
	s7.data[9] = 0;
	s7.data[10] = 0;
	s7.data[11] = 0;
	s7.data[12] = a->data[84];
	s7.data[13] = a->data[85];
	s7.data[14] = a->data[86];
	s7.data[15] = a->data[87];
	s7.data[16] = a->data[88];
	s7.data[17] = a->data[89];
	s7.data[18] = a->data[90];
	s7.data[19] = a->data[91];
	s7.data[20] = a->data[92];
	s7.data[21] = a->data[93];
	s7.data[22] = a->data[94];
	s7.data[23] = a->data[95];
	s7.data[24] = 0;
	s7.data[25] = 0;
	s7.data[26] = 0;
	s7.data[27] = 0;
	s7.data[28] = 0;
	s7.data[29] = 0;
	s7.data[30] = 0;
	s7.data[31] = 0;
	s7.data[32] = 0;
	s7.data[33] = 0;
	s7.data[34] = 0;
	s7.data[35] = 0;
	s7.data[36] = 0;
	s7.data[37] = 0;
	s7.data[38] = 0;
	s7.data[39] = 0;
	s7.data[40] = 0;
	s7.data[41] = 0;
	s7.data[42] = 0;
	s7.data[43] = 0;
	s7.data[44] = 0;
	s7.data[45] = 0;
	s7.data[46] = 0;
	s7.data[47] = 0;
	// s8 = (c22, c21, c20, c19, c18, c17, c16, c15, c14, c13, c12, c23)
	s8.data[0] = a->data[92];
	s8.data[1] = a->data[93];
	s8.data[2] = a->data[94];
	s8.data[3] = a->data[95];
	s8.data[4] = a->data[48];
	s8.data[5] = a->data[49];
	s8.data[6] = a->data[50];
	s8.data[7] = a->data[51];
	s8.data[8] = a->data[52];
	s8.data[9] = a->data[53];
	s8.data[10] = a->data[54];
	s8.data[11] = a->data[55];
	s8.data[12] = a->data[56];
	s8.data[13] = a->data[57];
	s8.data[14] = a->data[58];
	s8.data[15] = a->data[59];
	s8.data[16] = a->data[60];
	s8.data[17] = a->data[61];
	s8.data[18] = a->data[62];
	s8.data[19] = a->data[63];
	s8.data[20] = a->data[64];
	s8.data[21] = a->data[65];
	s8.data[22] = a->data[66];
	s8.data[23] = a->data[67];
	s8.data[24] = a->data[68];
	s8.data[25] = a->data[69];
	s8.data[26] = a->data[70];
	s8.data[27] = a->data[71];
	s8.data[28] = a->data[72];
	s8.data[29] = a->data[73];
	s8.data[30] = a->data[74];
	s8.data[31] = a->data[75];
	s8.data[32] = a->data[76];
	s8.data[33] = a->data[77];
	s8.data[34] = a->data[78];
	s8.data[35] = a->data[79];
	s8.data[36] = a->data[80];
	s8.data[37] = a->data[81];
	s8.data[38] = a->data[82];
	s8.data[39] = a->data[83];
	s8.data[40] = a->data[84];
	s8.data[41] = a->data[85];
	s8.data[42] = a->data[86];
	s8.data[43] = a->data[87];
	s8.data[44] = a->data[88];
	s8.data[45] = a->data[89];
	s8.data[46] = a->data[90];
	s8.data[47] = a->data[91];
	// s9 = (0, 0, 0, 0, 0, 0, 0, c23, c22, c21, c20, 0)
	s9.data[0] = 0;
	s9.data[1] = 0;
	s9.data[2] = 0;
	s9.data[3] = 0;
	s9.data[4] = a->data[80];
	s9.data[5] = a->data[81];
	s9.data[6] = a->data[82];
	s9.data[7] = a->data[83];
	s9.data[8] = a->data[84];
	s9.data[9] = a->data[85];
	s9.data[10] = a->data[86];
	s9.data[11] = a->data[87];
	s9.data[12] = a->data[88];
	s9.data[13] = a->data[89];
	s9.data[14] = a->data[90];
	s9.data[15] = a->data[91];
	s9.data[16] = a->data[92];
	s9.data[17] = a->data[93];
	s9.data[18] = a->data[94];
	s9.data[19] = a->data[95];
	s9.data[20] = 0;
	s9.data[21] = 0;
	s9.data[22] = 0;
	s9.data[23] = 0;
	s9.data[24] = 0;
	s9.data[25] = 0;
	s9.data[26] = 0;
	s9.data[27] = 0;
	s9.data[28] = 0;
	s9.data[29] = 0;
	s9.data[30] = 0;
	s9.data[31] = 0;
	s9.data[32] = 0;
	s9.data[33] = 0;
	s9.data[34] = 0;
	s9.data[35] = 0;
	s9.data[36] = 0;
	s9.data[37] = 0;
	s9.data[38] = 0;
	s9.data[39] = 0;
	s9.data[40] = 0;
	s9.data[41] = 0;
	s9.data[42] = 0;
	s9.data[43] = 0;
	s9.data[44] = 0;
	s9.data[45] = 0;
	s9.data[46] = 0;
	s9.data[47] = 0;
	// s10 = (0, 0, 0, 0, 0, 0, 0, c23, c23, 0, 0, 0)
	s10.data[0] = 0;
	s10.data[1] = 0;
	s10.data[2] = 0;
	s10.data[3] = 0;
	s10.data[4] = 0;
	s10.data[5] = 0;
	s10.data[6] = 0;
	s10.data[7] = 0;
	s10.data[8] = 0;
	s10.data[9] = 0;
	s10.data[10] = 0;
	s10.data[11] = 0;
	s10.data[12] = a->data[92];
	s10.data[13] = a->data[93];
	s10.data[14] = a->data[94];
	s10.data[15] = a->data[95];
	s10.data[16] = a->data[92];
	s10.data[17] = a->data[93];
	s10.data[18] = a->data[94];
	s10.data[19] = a->data[95];
	s10.data[20] = 0;
	s10.data[21] = 0;
	s10.data[22] = 0;
	s10.data[23] = 0;
	s10.data[24] = 0;
	s10.data[25] = 0;
	s10.data[26] = 0;
	s10.data[27] = 0;
	s10.data[28] = 0;
	s10.data[29] = 0;
	s10.data[30] = 0;
	s10.data[31] = 0;
	s10.data[32] = 0;
	s10.data[33] = 0;
	s10.data[34] = 0;
	s10.data[35] = 0;
	s10.data[36] = 0;
	s10.data[37] = 0;
	s10.data[38] = 0;
	s10.data[39] = 0;
	s10.data[40] = 0;
	s10.data[41] = 0;
	s10.data[42] = 0;
	s10.data[43] = 0;
	s10.data[44] = 0;
	s10.data[45] = 0;
	s10.data[46] = 0;
	s10.data[47] = 0;

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(48, 384, &s1);
	char * s2dump = GetString(48, 384, &s2);
	char * s3dump = GetString(48, 384, &s3);
	char * s4dump = GetString(48, 384, &s4);
	char * s5dump = GetString(48, 384, &s5);
	char * s6dump = GetString(48, 384, &s6);
	char * s7dump = GetString(48, 384, &s7);
	char * s8dump = GetString(48, 384, &s8);
	char * s9dump = GetString(48, 384, &s9);
	char * s10dump = GetString(48, 384, &s10);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
	free(s5dump);
	free(s6dump);
	free(s7dump);
	free(s8dump);
	free(s9dump);
	free(s10dump);
#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// s1 + 2s2 + s3 + s4 + s5 + s6 + s7 − s8 − s9 − s10
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s2, field);
	PFAddition(&partialres1, &partialres2, &s3, field);
	PFAddition(&partialres2, &partialres1, &s4, field);
	PFAddition(&partialres1, &partialres2, &s5, field);
	PFAddition(&partialres2, &partialres1, &s6, field);
	PFAddition(&partialres1, &partialres2, &s7, field);
	PFSubtraction(&partialres2, &partialres1, &s8, field);
	PFSubtraction(&partialres1, &partialres2, &s9, field);
	PFSubtraction(red, &partialres1, &s10, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#elif ARCHITECTURE_BITS == 16

	// Note that a presents 48 chunks

	pfelement s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, partialres1, partialres2;

	// Init partial results data
	chunk s1data[24]; // 16 * 24 = 384
	s1.data = s1data;
	chunk s2data[24];
	s2.data = s2data;
	chunk s3data[24];
	s3.data = s3data;
	chunk s4data[24];
	s4.data = s4data;
	chunk s5data[24];
	s5.data = s5data;
	chunk s6data[24];
	s6.data = s6data;
	chunk s7data[24];
	s7.data = s7data;
	chunk s8data[24];
	s8.data = s8data;
	chunk s9data[24];
	s9.data = s9data;
	chunk s10data[24];
	s10.data = s10data;

	// Assuming a = (c23,...,c0) (32-bit chunks)
	// s1 = (c11, c10, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0)
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
	// s2 = (0 ,0, 0, 0, 0, c23, c22, c21, 0, 0, 0, 0)
	s2.data[0] = 0;
	s2.data[1] = 0;
	s2.data[2] = 0;
	s2.data[3] = 0;
	s2.data[4] = 0;
	s2.data[5] = 0;
	s2.data[6] = 0;
	s2.data[7] = 0;
	s2.data[8] = a->data[42];
	s2.data[9] = a->data[43];
	s2.data[10] = a->data[44];
	s2.data[11] = a->data[45];
	s2.data[12] = a->data[46];
	s2.data[13] = a->data[47];
	s2.data[14] = 0;
	s2.data[15] = 0;
	s2.data[16] = 0;
	s2.data[17] = 0;
	s2.data[18] = 0;
	s2.data[19] = 0;
	s2.data[20] = 0;
	s2.data[21] = 0;
	s2.data[22] = 0;
	s2.data[23] = 0;
	// s3 = (c23, c22, c21, c20, c19, c18, c17, c16, c15, c14, c13, c12)
	s3.data[0] = a->data[24];
	s3.data[1] = a->data[25];
	s3.data[2] = a->data[26];
	s3.data[3] = a->data[27];
	s3.data[4] = a->data[28];
	s3.data[5] = a->data[29];
	s3.data[6] = a->data[30];
	s3.data[7] = a->data[31];
	s3.data[8] = a->data[32];
	s3.data[9] = a->data[33];
	s3.data[10] = a->data[34];
	s3.data[11] = a->data[35];
	s3.data[12] = a->data[36];
	s3.data[13] = a->data[37];
	s3.data[14] = a->data[38];
	s3.data[15] = a->data[39];
	s3.data[16] = a->data[40];
	s3.data[17] = a->data[41];
	s3.data[18] = a->data[42];
	s3.data[19] = a->data[43];
	s3.data[20] = a->data[44];
	s3.data[21] = a->data[45];
	s3.data[22] = a->data[46];
	s3.data[23] = a->data[47];
	// s4 = (c20, c19, c18, c17, c16, c15, c14, c13, c12, c23, c22, c21)
	s4.data[0] = a->data[42];
	s4.data[1] = a->data[43];
	s4.data[2] = a->data[44];
	s4.data[3] = a->data[45];
	s4.data[4] = a->data[46];
	s4.data[5] = a->data[47];
	s4.data[6] = a->data[24];
	s4.data[7] = a->data[25];
	s4.data[8] = a->data[26];
	s4.data[9] = a->data[27];
	s4.data[10] = a->data[28];
	s4.data[11] = a->data[29];
	s4.data[12] = a->data[30];
	s4.data[13] = a->data[31];
	s4.data[14] = a->data[32];
	s4.data[15] = a->data[33];
	s4.data[16] = a->data[34];
	s4.data[17] = a->data[35];
	s4.data[18] = a->data[36];
	s4.data[19] = a->data[37];
	s4.data[20] = a->data[38];
	s4.data[21] = a->data[39];
	s4.data[22] = a->data[40];
	s4.data[23] = a->data[41];
	// s5 = (c19, c18, c17, c16, c15, c14, c13, c12, c20, 0, c23, 0)
	s5.data[0] = 0;
	s5.data[1] = 0;
	s5.data[2] = a->data[46];
	s5.data[3] = a->data[47];
	s5.data[4] = 0;
	s5.data[5] = 0;
	s5.data[6] = a->data[40];
	s5.data[7] = a->data[41];
	s5.data[8] = a->data[24];
	s5.data[9] = a->data[25];
	s5.data[10] = a->data[26];
	s5.data[11] = a->data[27];
	s5.data[12] = a->data[28];
	s5.data[13] = a->data[29];
	s5.data[14] = a->data[30];
	s5.data[15] = a->data[31];
	s5.data[16] = a->data[32];
	s5.data[17] = a->data[33];
	s5.data[18] = a->data[34];
	s5.data[19] = a->data[35];
	s5.data[20] = a->data[36];
	s5.data[21] = a->data[37];
	s5.data[22] = a->data[38];
	s5.data[23] = a->data[39];
	// s6 = (0, 0, 0, 0, c23, c22, c21, c20, 0, 0, 0, 0)
	s6.data[0] = 0;
	s6.data[1] = 0;
	s6.data[2] = 0;
	s6.data[3] = 0;
	s6.data[4] = 0;
	s6.data[5] = 0;
	s6.data[6] = 0;
	s6.data[7] = 0;
	s6.data[8] = a->data[40];
	s6.data[9] = a->data[41];
	s6.data[10] = a->data[42];
	s6.data[11] = a->data[43];
	s6.data[12] = a->data[44];
	s6.data[13] = a->data[45];
	s6.data[14] = a->data[46];
	s6.data[15] = a->data[47];
	s6.data[16] = 0;
	s6.data[17] = 0;
	s6.data[18] = 0;
	s6.data[19] = 0;
	s6.data[20] = 0;
	s6.data[21] = 0;
	s6.data[22] = 0;
	s6.data[23] = 0;
	// s7 = (0, 0, 0, 0, 0, 0, c23, c22, c21, 0, 0, c20)
	s7.data[0] = a->data[40];
	s7.data[1] = a->data[41];
	s7.data[2] = 0;
	s7.data[3] = 0;
	s7.data[4] = 0;
	s7.data[5] = 0;
	s7.data[6] = a->data[42];
	s7.data[7] = a->data[43];
	s7.data[8] = a->data[44];
	s7.data[9] = a->data[45];
	s7.data[10] = a->data[46];
	s7.data[11] = a->data[47];
	s7.data[12] = 0;
	s7.data[13] = 0;
	s7.data[14] = 0;
	s7.data[15] = 0;
	s7.data[16] = 0;
	s7.data[17] = 0;
	s7.data[18] = 0;
	s7.data[19] = 0;
	s7.data[20] = 0;
	s7.data[21] = 0;
	s7.data[22] = 0;
	s7.data[23] = 0;
	// s8 = (c22, c21, c20, c19, c18, c17, c16, c15, c14, c13, c12, c23)
	s8.data[0] = a->data[46];
	s8.data[1] = a->data[47];
	s8.data[2] = a->data[24];
	s8.data[3] = a->data[25];
	s8.data[4] = a->data[26];
	s8.data[5] = a->data[27];
	s8.data[6] = a->data[28];
	s8.data[7] = a->data[29];
	s8.data[8] = a->data[30];
	s8.data[9] = a->data[31];
	s8.data[10] = a->data[32];
	s8.data[11] = a->data[33];
	s8.data[12] = a->data[34];
	s8.data[13] = a->data[35];
	s8.data[14] = a->data[36];
	s8.data[15] = a->data[37];
	s8.data[16] = a->data[38];
	s8.data[17] = a->data[39];
	s8.data[18] = a->data[40];
	s8.data[19] = a->data[41];
	s8.data[20] = a->data[42];
	s8.data[21] = a->data[43];
	s8.data[22] = a->data[44];
	s8.data[23] = a->data[45];
	// s9 = (0, 0, 0, 0, 0, 0, 0, c23, c22, c21, c20, 0)
	s9.data[0] = 0;
	s9.data[1] = 0;
	s9.data[2] = a->data[40];
	s9.data[3] = a->data[41];
	s9.data[4] = a->data[42];
	s9.data[5] = a->data[43];
	s9.data[6] = a->data[44];
	s9.data[7] = a->data[45];
	s9.data[8] = a->data[46];
	s9.data[9] = a->data[47];
	s9.data[10] = 0;
	s9.data[11] = 0;
	s9.data[12] = 0;
	s9.data[13] = 0;
	s9.data[14] = 0;
	s9.data[15] = 0;
	s9.data[16] = 0;
	s9.data[17] = 0;
	s9.data[18] = 0;
	s9.data[19] = 0;
	s9.data[20] = 0;
	s9.data[21] = 0;
	s9.data[22] = 0;
	s9.data[23] = 0;
	// s10 = (0, 0, 0, 0, 0, 0, 0, c23, c23, 0, 0, 0)
	s10.data[0] = 0;
	s10.data[1] = 0;
	s10.data[2] = 0;
	s10.data[3] = 0;
	s10.data[4] = 0;
	s10.data[5] = 0;
	s10.data[6] = a->data[46];
	s10.data[7] = a->data[47];
	s10.data[8] = a->data[46];
	s10.data[9] = a->data[47];
	s10.data[10] = 0;
	s10.data[11] = 0;
	s10.data[12] = 0;
	s10.data[13] = 0;
	s10.data[14] = 0;
	s10.data[15] = 0;
	s10.data[16] = 0;
	s10.data[17] = 0;
	s10.data[18] = 0;
	s10.data[19] = 0;
	s10.data[20] = 0;
	s10.data[21] = 0;
	s10.data[22] = 0;
	s10.data[23] = 0;

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(24, 384, &s1);
	char * s2dump = GetString(24, 384, &s2);
	char * s3dump = GetString(24, 384, &s3);
	char * s4dump = GetString(24, 384, &s4);
	char * s5dump = GetString(24, 384, &s5);
	char * s6dump = GetString(24, 384, &s6);
	char * s7dump = GetString(24, 384, &s7);
	char * s8dump = GetString(24, 384, &s8);
	char * s9dump = GetString(24, 384, &s9);
	char * s10dump = GetString(24, 384, &s10);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
	free(s5dump);
	free(s6dump);
	free(s7dump);
	free(s8dump);
	free(s9dump);
	free(s10dump);
#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// s1 + 2s2 + s3 + s4 + s5 + s6 + s7 − s8 − s9 − s10
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s2, field);
	PFAddition(&partialres1, &partialres2, &s3, field);
	PFAddition(&partialres2, &partialres1, &s4, field);
	PFAddition(&partialres1, &partialres2, &s5, field);
	PFAddition(&partialres2, &partialres1, &s6, field);
	PFAddition(&partialres1, &partialres2, &s7, field);
	PFSubtraction(&partialres2, &partialres1, &s8, field);
	PFSubtraction(&partialres1, &partialres2, &s9, field);
	PFSubtraction(red, &partialres1, &s10, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#elif ARCHITECTURE_BITS == 64

	// Note that a presents 12 chunks

	pfelement s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, partialres1, partialres2;

	// Init partial results data
	chunk s1data[6]; // 64 * 6 = 384
	s1.data = s1data;
	chunk s2data[6];
	s2.data = s2data;
	chunk s3data[6];
	s3.data = s3data;
	chunk s4data[6];
	s4.data = s4data;
	chunk s5data[6];
	s5.data = s5data;
	chunk s6data[6];
	s6.data = s6data;
	chunk s7data[6];
	s7.data = s7data;
	chunk s8data[6];
	s8.data = s8data;
	chunk s9data[6];
	s9.data = s9data;
	chunk s10data[6];
	s10.data = s10data;

	// Assuming a = (c23,...,c0) (32-bit chunks)
	// s1 = (c11, c10, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0)
	s1.data[0] = a->data[0];
	s1.data[1] = a->data[1];
	s1.data[2] = a->data[2];
	s1.data[3] = a->data[3];
	s1.data[4] = a->data[4];
	s1.data[5] = a->data[5];
	// s2 = (0 ,0, 0, 0, 0, c23, c22, c21, 0, 0, 0, 0)
	s2.data[0] = 0;
	s2.data[1] = 0;
	s2.data[2] = (a->data[10] >> 32) + (a->data[11] << 32);
	s2.data[3] = a->data[11] >> 32;
	s2.data[4] = 0;
	s2.data[5] = 0;
	// s3 = (c23, c22, c21, c20, c19, c18, c17, c16, c15, c14, c13, c12)
	s3.data[0] = a->data[6];
	s3.data[1] = a->data[7];
	s3.data[2] = a->data[8];
	s3.data[3] = a->data[9];
	s3.data[4] = a->data[10];
	s3.data[5] = a->data[11];
	// s4 = (c20, c19, c18, c17, c16, c15, c14, c13, c12, c23, c22, c21)
	s4.data[0] = (a->data[10] >> 32) + (a->data[11] << 32);
	s4.data[1] = (a->data[11] >> 32) + (a->data[6] << 32);
	s4.data[2] = (a->data[6] >> 32) + (a->data[7] << 32);
	s4.data[3] = (a->data[7] >> 32) + (a->data[8] << 32);
	s4.data[4] = (a->data[8] >> 32) + (a->data[9] << 32);
	s4.data[5] = (a->data[9] >> 32) + (a->data[10] << 32);
	// s5 = (c19, c18, c17, c16, c15, c14, c13, c12, c20, 0, c23, 0)
	s5.data[0] = a->data[11] & 0xffffffff00000000;
	s5.data[1] = a->data[10] << 32;
	s5.data[2] = a->data[6];
	s5.data[3] = a->data[7];
	s5.data[4] = a->data[8];
	s5.data[5] = a->data[9];
	// s6 = (0, 0, 0, 0, c23, c22, c21, c20, 0, 0, 0, 0)
	s6.data[0] = 0;
	s6.data[1] = 0;
	s6.data[2] = a->data[10];
	s6.data[3] = a->data[11];
	s6.data[4] = 0;
	s6.data[5] = 0;
	// s7 = (0, 0, 0, 0, 0, 0, c23, c22, c21, 0, 0, c20)
	s7.data[0] = a->data[10] & 0x00000000ffffffff;
	s7.data[1] = a->data[10] & 0xffffffff00000000;
	s7.data[2] = a->data[11];
	s7.data[3] = 0;
	s7.data[4] = 0;
	s7.data[5] = 0;
	// s8 = (c22, c21, c20, c19, c18, c17, c16, c15, c14, c13, c12, c23)
	s8.data[0] = (a->data[11] >> 32) + (a->data[6] << 32);
	s8.data[1] = (a->data[6] >> 32) + (a->data[7] << 32);
	s8.data[2] = (a->data[7] >> 32) + (a->data[8] << 32);
	s8.data[3] = (a->data[8] >> 32) + (a->data[9] << 32);
	s8.data[4] = (a->data[9] >> 32) + (a->data[10] << 32);
	s8.data[5] = (a->data[10] >> 32) + (a->data[11] << 32);
	// s9 = (0, 0, 0, 0, 0, 0, 0, c23, c22, c21, c20, 0)
	s9.data[0] = a->data[10] << 32;
	s9.data[1] = (a->data[10] >> 32) + (a->data[11] << 32);
	s9.data[2] = a->data[11] >> 32;
	s9.data[3] = 0;
	s9.data[4] = 0;
	s9.data[5] = 0;
	// s10 = (0, 0, 0, 0, 0, 0, 0, c23, c23, 0, 0, 0)
	s10.data[0] = 0;
	s10.data[1] = a->data[11] & 0xffffffff00000000;;
	s10.data[2] = a->data[11] >> 32;
	s10.data[3] = 0;
	s10.data[4] = 0;
	s10.data[5] = 0;

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(6, 384, &s1);
	char * s2dump = GetString(6, 384, &s2);
	char * s3dump = GetString(6, 384, &s3);
	char * s4dump = GetString(6, 384, &s4);
	char * s5dump = GetString(6, 384, &s5);
	char * s6dump = GetString(6, 384, &s6);
	char * s7dump = GetString(6, 384, &s7);
	char * s8dump = GetString(6, 384, &s8);
	char * s9dump = GetString(6, 384, &s9);
	char * s10dump = GetString(6, 384, &s10);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
	free(s5dump);
	free(s6dump);
	free(s7dump);
	free(s8dump);
	free(s9dump);
	free(s10dump);
#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// s1 + 2s2 + s3 + s4 + s5 + s6 + s7 − s8 − s9 − s10
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s2, field);
	PFAddition(&partialres1, &partialres2, &s3, field);
	PFAddition(&partialres2, &partialres1, &s4, field);
	PFAddition(&partialres1, &partialres2, &s5, field);
	PFAddition(&partialres2, &partialres1, &s6, field);
	PFAddition(&partialres1, &partialres2, &s7, field);
	PFSubtraction(&partialres2, &partialres1, &s8, field);
	PFSubtraction(&partialres1, &partialres2, &s9, field);
	PFSubtraction(red, &partialres1, &s10, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#else // Set chunks to 32 bit

	// Note that a presents 24 chunks

	pfelement s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, partialres1, partialres2;

	// Init partial results data
	chunk s1data[12]; // 32 * 12 = 384
	s1.data = s1data;
	chunk s2data[12];
	s2.data = s2data;
	chunk s3data[12];
	s3.data = s3data;
	chunk s4data[12];
	s4.data = s4data;
	chunk s5data[12];
	s5.data = s5data;
	chunk s6data[12];
	s6.data = s6data;
	chunk s7data[12];
	s7.data = s7data;
	chunk s8data[12];
	s8.data = s8data;
	chunk s9data[12];
	s9.data = s9data;
	chunk s10data[12];
	s10.data = s10data;

	// Assuming a = (c23,...,c0) (32-bit chunks)
	// s1 = (c11, c10, c9, c8, c7, c6, c5, c4, c3, c2, c1, c0)
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
	// s2 = (0 ,0, 0, 0, 0, c23, c22, c21, 0, 0, 0, 0)
	s2.data[0] = 0;
	s2.data[1] = 0;
	s2.data[2] = 0;
	s2.data[3] = 0;
	s2.data[4] = a->data[21];
	s2.data[5] = a->data[22];
	s2.data[6] = a->data[23];
	s2.data[7] = 0;
	s2.data[8] = 0;
	s2.data[9] = 0;
	s2.data[10] = 0;
	s2.data[11] = 0;
	// s3 = (c23, c22, c21, c20, c19, c18, c17, c16, c15, c14, c13, c12)
	s3.data[0] = a->data[12];
	s3.data[1] = a->data[13];
	s3.data[2] = a->data[14];
	s3.data[3] = a->data[15];
	s3.data[4] = a->data[16];
	s3.data[5] = a->data[17];
	s3.data[6] = a->data[18];
	s3.data[7] = a->data[19];
	s3.data[8] = a->data[20];
	s3.data[9] = a->data[21];
	s3.data[10] = a->data[22];
	s3.data[11] = a->data[23];
	// s4 = (c20, c19, c18, c17, c16, c15, c14, c13, c12, c23, c22, c21)
	s4.data[0] = a->data[21];
	s4.data[1] = a->data[22];
	s4.data[2] = a->data[23];
	s4.data[3] = a->data[12];
	s4.data[4] = a->data[13];
	s4.data[5] = a->data[14];
	s4.data[6] = a->data[15];
	s4.data[7] = a->data[16];
	s4.data[8] = a->data[17];
	s4.data[9] = a->data[18];
	s4.data[10] = a->data[19];
	s4.data[11] = a->data[20];
	// s5 = (c19, c18, c17, c16, c15, c14, c13, c12, c20, 0, c23, 0)
	s5.data[0] = 0;
	s5.data[1] = a->data[23];
	s5.data[2] = 0;
	s5.data[3] = a->data[20];
	s5.data[4] = a->data[12];
	s5.data[5] = a->data[13];
	s5.data[6] = a->data[14];
	s5.data[7] = a->data[15];
	s5.data[8] = a->data[16];
	s5.data[9] = a->data[17];
	s5.data[10] = a->data[18];
	s5.data[11] = a->data[19];
	// s6 = (0, 0, 0, 0, c23, c22, c21, c20, 0, 0, 0, 0)
	s6.data[0] = 0;
	s6.data[1] = 0;
	s6.data[2] = 0;
	s6.data[3] = 0;
	s6.data[4] = a->data[20];
	s6.data[5] = a->data[21];
	s6.data[6] = a->data[22];
	s6.data[7] = a->data[23];
	s6.data[8] = 0;
	s6.data[9] = 0;
	s6.data[10] = 0;
	s6.data[11] = 0;
	// s7 = (0, 0, 0, 0, 0, 0, c23, c22, c21, 0, 0, c20)
	s7.data[0] = a->data[20];
	s7.data[1] = 0;
	s7.data[2] = 0;
	s7.data[3] = a->data[21];
	s7.data[4] = a->data[22];
	s7.data[5] = a->data[23];
	s7.data[6] = 0;
	s7.data[7] = 0;
	s7.data[8] = 0;
	s7.data[9] = 0;
	s7.data[10] = 0;
	s7.data[11] = 0;
	// s8 = (c22, c21, c20, c19, c18, c17, c16, c15, c14, c13, c12, c23)
	s8.data[0] = a->data[23];
	s8.data[1] = a->data[12];
	s8.data[2] = a->data[13];
	s8.data[3] = a->data[14];
	s8.data[4] = a->data[15];
	s8.data[5] = a->data[16];
	s8.data[6] = a->data[17];
	s8.data[7] = a->data[18];
	s8.data[8] = a->data[19];
	s8.data[9] = a->data[20];
	s8.data[10] = a->data[21];
	s8.data[11] = a->data[22];
	// s9 = (0, 0, 0, 0, 0, 0, 0, c23, c22, c21, c20, 0)
	s9.data[0] = 0;
	s9.data[1] = a->data[20];
	s9.data[2] = a->data[21];
	s9.data[3] = a->data[22];
	s9.data[4] = a->data[23];
	s9.data[5] = 0;
	s9.data[6] = 0;
	s9.data[7] = 0;
	s9.data[8] = 0;
	s9.data[9] = 0;
	s9.data[10] = 0;
	s9.data[11] = 0;
	// s10 = (0, 0, 0, 0, 0, 0, 0, c23, c23, 0, 0, 0)
	s10.data[0] = 0;
	s10.data[1] = 0;
	s10.data[2] = 0;
	s10.data[3] = a->data[23];
	s10.data[4] = a->data[23];
	s10.data[5] = 0;
	s10.data[6] = 0;
	s10.data[7] = 0;
	s10.data[8] = 0;
	s10.data[9] = 0;
	s10.data[10] = 0;
	s10.data[11] = 0;

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(12, 384, &s1);
	char * s2dump = GetString(12, 384, &s2);
	char * s3dump = GetString(12, 384, &s3);
	char * s4dump = GetString(12, 384, &s4);
	char * s5dump = GetString(12, 384, &s5);
	char * s6dump = GetString(12, 384, &s6);
	char * s7dump = GetString(12, 384, &s7);
	char * s8dump = GetString(12, 384, &s8);
	char * s9dump = GetString(12, 384, &s9);
	char * s10dump = GetString(12, 384, &s10);
	free(s1dump);
	free(s2dump);
	free(s3dump);
	free(s4dump);
	free(s5dump);
	free(s6dump);
	free(s7dump);
	free(s8dump);
	free(s9dump);
	free(s10dump);
#endif

	// Allocate space for sum results
	InitElementByString(&partialres1, "", field);
	InitElementByString(&partialres2, "", field);

	// s1 + 2s2 + s3 + s4 + s5 + s6 + s7 − s8 − s9 − s10
	PFAddition(&partialres1, &s1, &s2, field);
	PFAddition(&partialres2, &partialres1, &s2, field);
	PFAddition(&partialres1, &partialres2, &s3, field);
	PFAddition(&partialres2, &partialres1, &s4, field);
	PFAddition(&partialres1, &partialres2, &s5, field);
	PFAddition(&partialres2, &partialres1, &s6, field);
	PFAddition(&partialres1, &partialres2, &s7, field);
	PFSubtraction(&partialres2, &partialres1, &s8, field);
	PFSubtraction(&partialres1, &partialres2, &s9, field);
	PFSubtraction(red, &partialres1, &s10, field);

	// Free partial results space
	FreeElement(&partialres1);
	FreeElement(&partialres2);

#endif
}

void FastReductionFIPSp521(
	pfelement * red,
	pfelement * a, 
	pfproperties * field)
{
#if ARCHITECTURE_BITS == 8

	// a is 1042 bit wide, so 131 chunks of 8 bit are necessary

	pfelement s1, s2;

	// Init partial results data
	chunk s1data[66]; // 8 * 66 = 528 = 521 + 7
	s1.data = s1data;
	chunk s2data[66];
	s2.data = s2data;

	// Assuming a = (a1041,...,a0) (1-bit chunks)
	// s1 = (a1041, ..., a521)
	s1.data[0] = (a->data[65] >> 1) + (a->data[66] << 7);
	s1.data[1] = (a->data[66] >> 1) + (a->data[67] << 7);
	s1.data[2] = (a->data[67] >> 1) + (a->data[68] << 7);
	s1.data[3] = (a->data[68] >> 1) + (a->data[69] << 7);
	s1.data[4] = (a->data[69] >> 1) + (a->data[70] << 7);
	s1.data[5] = (a->data[70] >> 1) + (a->data[71] << 7);
	s1.data[6] = (a->data[71] >> 1) + (a->data[72] << 7);
	s1.data[7] = (a->data[72] >> 1) + (a->data[73] << 7);
	s1.data[8] = (a->data[73] >> 1) + (a->data[74] << 7);
	s1.data[9] = (a->data[74] >> 1) + (a->data[75] << 7);
	s1.data[10] = (a->data[75] >> 1) + (a->data[76] << 7);
	s1.data[11] = (a->data[76] >> 1) + (a->data[77] << 7);
	s1.data[12] = (a->data[77] >> 1) + (a->data[78] << 7);
	s1.data[13] = (a->data[78] >> 1) + (a->data[79] << 7);
	s1.data[14] = (a->data[79] >> 1) + (a->data[80] << 7);
	s1.data[15] = (a->data[80] >> 1) + (a->data[81] << 7);
	s1.data[16] = (a->data[81] >> 1) + (a->data[82] << 7);
	s1.data[17] = (a->data[82] >> 1) + (a->data[83] << 7);
	s1.data[18] = (a->data[83] >> 1) + (a->data[84] << 7);
	s1.data[19] = (a->data[84] >> 1) + (a->data[85] << 7);
	s1.data[20] = (a->data[85] >> 1) + (a->data[86] << 7);
	s1.data[21] = (a->data[86] >> 1) + (a->data[87] << 7);
	s1.data[22] = (a->data[87] >> 1) + (a->data[88] << 7);
	s1.data[23] = (a->data[88] >> 1) + (a->data[89] << 7);
	s1.data[24] = (a->data[89] >> 1) + (a->data[90] << 7);
	s1.data[25] = (a->data[90] >> 1) + (a->data[91] << 7);
	s1.data[26] = (a->data[91] >> 1) + (a->data[92] << 7);
	s1.data[27] = (a->data[92] >> 1) + (a->data[93] << 7);
	s1.data[28] = (a->data[93] >> 1) + (a->data[94] << 7);
	s1.data[29] = (a->data[94] >> 1) + (a->data[95] << 7);
	s1.data[30] = (a->data[95] >> 1) + (a->data[96] << 7);
	s1.data[31] = (a->data[96] >> 1) + (a->data[97] << 7);
	s1.data[32] = (a->data[97] >> 1) + (a->data[98] << 7);
	s1.data[33] = (a->data[98] >> 1) + (a->data[99] << 7);
	s1.data[34] = (a->data[99] >> 1) + (a->data[100] << 7);
	s1.data[35] = (a->data[100] >> 1) + (a->data[101] << 7);
	s1.data[36] = (a->data[101] >> 1) + (a->data[102] << 7);
	s1.data[37] = (a->data[102] >> 1) + (a->data[103] << 7);
	s1.data[38] = (a->data[103] >> 1) + (a->data[104] << 7);
	s1.data[39] = (a->data[104] >> 1) + (a->data[105] << 7);
	s1.data[40] = (a->data[105] >> 1) + (a->data[106] << 7);
	s1.data[41] = (a->data[106] >> 1) + (a->data[107] << 7);
	s1.data[42] = (a->data[107] >> 1) + (a->data[108] << 7);
	s1.data[43] = (a->data[108] >> 1) + (a->data[109] << 7);
	s1.data[44] = (a->data[109] >> 1) + (a->data[110] << 7);
	s1.data[45] = (a->data[110] >> 1) + (a->data[111] << 7);
	s1.data[46] = (a->data[111] >> 1) + (a->data[112] << 7);
	s1.data[47] = (a->data[112] >> 1) + (a->data[113] << 7);
	s1.data[48] = (a->data[113] >> 1) + (a->data[114] << 7);
	s1.data[49] = (a->data[114] >> 1) + (a->data[115] << 7);
	s1.data[50] = (a->data[115] >> 1) + (a->data[116] << 7);
	s1.data[51] = (a->data[116] >> 1) + (a->data[117] << 7);
	s1.data[52] = (a->data[117] >> 1) + (a->data[118] << 7);
	s1.data[53] = (a->data[118] >> 1) + (a->data[119] << 7);
	s1.data[54] = (a->data[119] >> 1) + (a->data[120] << 7);
	s1.data[55] = (a->data[120] >> 1) + (a->data[121] << 7);
	s1.data[56] = (a->data[121] >> 1) + (a->data[122] << 7);
	s1.data[57] = (a->data[122] >> 1) + (a->data[123] << 7);
	s1.data[58] = (a->data[123] >> 1) + (a->data[124] << 7);
	s1.data[59] = (a->data[124] >> 1) + (a->data[125] << 7);
	s1.data[60] = (a->data[125] >> 1) + (a->data[126] << 7);
	s1.data[61] = (a->data[126] >> 1) + (a->data[127] << 7);
	s1.data[62] = (a->data[127] >> 1) + (a->data[128] << 7);
	s1.data[63] = (a->data[128] >> 1) + (a->data[129] << 7);
	s1.data[64] = (a->data[129] >> 1) + (a->data[130] << 7);
	s1.data[65] = a->data[130] >> 1;
	// s2 = (a520, ..., a2, a1, a0)
	s2.data[0] = a->data[0];
	s2.data[1] = a->data[1];
	s2.data[2] = a->data[2];
	s2.data[3] = a->data[3];
	s2.data[4] = a->data[4];
	s2.data[5] = a->data[5];
	s2.data[6] = a->data[6];
	s2.data[7] = a->data[7];
	s2.data[8] = a->data[8];
	s2.data[9] = a->data[9];
	s2.data[10] = a->data[10];
	s2.data[11] = a->data[11];
	s2.data[12] = a->data[12];
	s2.data[13] = a->data[13];
	s2.data[14] = a->data[14];
	s2.data[15] = a->data[15];
	s2.data[16] = a->data[16];
	s2.data[17] = a->data[17];
	s2.data[18] = a->data[18];
	s2.data[19] = a->data[19];
	s2.data[20] = a->data[20];
	s2.data[21] = a->data[21];
	s2.data[22] = a->data[22];
	s2.data[23] = a->data[23];
	s2.data[24] = a->data[24];
	s2.data[25] = a->data[25];
	s2.data[26] = a->data[26];
	s2.data[27] = a->data[27];
	s2.data[28] = a->data[28];
	s2.data[29] = a->data[29];
	s2.data[30] = a->data[30];
	s2.data[31] = a->data[31];
	s2.data[32] = a->data[32];
	s2.data[33] = a->data[33];
	s2.data[34] = a->data[34];
	s2.data[35] = a->data[35];
	s2.data[36] = a->data[36];
	s2.data[37] = a->data[37];
	s2.data[38] = a->data[38];
	s2.data[39] = a->data[39];
	s2.data[40] = a->data[40];
	s2.data[41] = a->data[41];
	s2.data[42] = a->data[42];
	s2.data[43] = a->data[43];
	s2.data[44] = a->data[44];
	s2.data[45] = a->data[45];
	s2.data[46] = a->data[46];
	s2.data[47] = a->data[47];
	s2.data[48] = a->data[48];
	s2.data[49] = a->data[49];
	s2.data[50] = a->data[50];
	s2.data[51] = a->data[51];
	s2.data[52] = a->data[52];
	s2.data[53] = a->data[53];
	s2.data[54] = a->data[54];
	s2.data[55] = a->data[55];
	s2.data[56] = a->data[56];
	s2.data[57] = a->data[57];
	s2.data[58] = a->data[58];
	s2.data[59] = a->data[59];
	s2.data[60] = a->data[60];
	s2.data[61] = a->data[61];
	s2.data[62] = a->data[62];
	s2.data[63] = a->data[63];
	s2.data[64] = a->data[64];
	s2.data[65] = a->data[65] & 0x0000000000000001;

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(66, 521, &s1);
	char * s2dump = GetString(66, 521, &s2);
	free(s1dump);
	free(s2dump);
#endif

	// s1 + s2
	PFAddition(red, &s1, &s2, field);

#elif ARCHITECTURE_BITS == 16

	// a is 1042 bit wide, so 66 chunks of 16 bit are necessary

	pfelement s1, s2;

	// Init partial results data
	chunk s1data[33]; // 16 * 33 = 528 = 521 + 7
	s1.data = s1data;
	chunk s2data[33];
	s2.data = s2data;

	// Assuming a = (a1041,...,a0) (1-bit chunks)
	// s1 = (a1041, ..., a521)
	s1.data[0] = (a->data[32] >> 9) + (a->data[33] << 7);
	s1.data[1] = (a->data[33] >> 9) + (a->data[34] << 7);
	s1.data[2] = (a->data[34] >> 9) + (a->data[35] << 7);
	s1.data[3] = (a->data[35] >> 9) + (a->data[36] << 7);
	s1.data[4] = (a->data[36] >> 9) + (a->data[37] << 7);
	s1.data[5] = (a->data[37] >> 9) + (a->data[38] << 7);
	s1.data[6] = (a->data[38] >> 9) + (a->data[39] << 7);
	s1.data[7] = (a->data[39] >> 9) + (a->data[40] << 7);
	s1.data[8] = (a->data[40] >> 9) + (a->data[41] << 7);
	s1.data[9] = (a->data[41] >> 9) + (a->data[42] << 7);
	s1.data[10] = (a->data[42] >> 9) + (a->data[43] << 7);
	s1.data[11] = (a->data[43] >> 9) + (a->data[44] << 7);
	s1.data[12] = (a->data[44] >> 9) + (a->data[45] << 7);
	s1.data[13] = (a->data[45] >> 9) + (a->data[46] << 7);
	s1.data[14] = (a->data[46] >> 9) + (a->data[47] << 7);
	s1.data[15] = (a->data[47] >> 9) + (a->data[48] << 7);
	s1.data[16] = (a->data[48] >> 9) + (a->data[49] << 7);
	s1.data[17] = (a->data[49] >> 9) + (a->data[50] << 7);
	s1.data[18] = (a->data[50] >> 9) + (a->data[51] << 7);
	s1.data[19] = (a->data[51] >> 9) + (a->data[52] << 7);
	s1.data[20] = (a->data[52] >> 9) + (a->data[53] << 7);
	s1.data[21] = (a->data[53] >> 9) + (a->data[54] << 7);
	s1.data[22] = (a->data[54] >> 9) + (a->data[55] << 7);
	s1.data[23] = (a->data[55] >> 9) + (a->data[56] << 7);
	s1.data[24] = (a->data[56] >> 9) + (a->data[57] << 7);
	s1.data[25] = (a->data[57] >> 9) + (a->data[58] << 7);
	s1.data[26] = (a->data[58] >> 9) + (a->data[59] << 7);
	s1.data[27] = (a->data[59] >> 9) + (a->data[60] << 7);
	s1.data[28] = (a->data[60] >> 9) + (a->data[61] << 7);
	s1.data[29] = (a->data[61] >> 9) + (a->data[62] << 7);
	s1.data[30] = (a->data[62] >> 9) + (a->data[63] << 7);
	s1.data[31] = (a->data[63] >> 9) + (a->data[64] << 7);
	s1.data[32] = (a->data[64] >> 9) + (a->data[65] << 7);
	// s2 = (a520, ..., a2, a1, a0)
	s2.data[0] = a->data[0];
	s2.data[1] = a->data[1];
	s2.data[2] = a->data[2];
	s2.data[3] = a->data[3];
	s2.data[4] = a->data[4];
	s2.data[5] = a->data[5];
	s2.data[6] = a->data[6];
	s2.data[7] = a->data[7];
	s2.data[8] = a->data[8] ;
	s2.data[9] = a->data[9];
	s2.data[10] = a->data[10];
	s2.data[11] = a->data[11];
	s2.data[12] = a->data[12];
	s2.data[13] = a->data[13];
	s2.data[14] = a->data[14];
	s2.data[15] = a->data[15];
	s2.data[16] = a->data[16];
	s2.data[17] = a->data[17];
	s2.data[18] = a->data[18];
	s2.data[19] = a->data[19];
	s2.data[20] = a->data[20];
	s2.data[21] = a->data[21];
	s2.data[22] = a->data[22];
	s2.data[23] = a->data[23];
	s2.data[24] = a->data[24];
	s2.data[25] = a->data[25];
	s2.data[26] = a->data[26];
	s2.data[27] = a->data[27];
	s2.data[28] = a->data[28];
	s2.data[29] = a->data[29];
	s2.data[30] = a->data[30];
	s2.data[31] = a->data[31];
	s2.data[32] = a->data[32] & 0x00000000000001ff;

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(33, 521, &s1);
	char * s2dump = GetString(33, 521, &s2);
	free(s1dump);
	free(s2dump);
#endif

	// s1 + s2
	PFAddition(red, &s1, &s2, field);

#elif ARCHITECTURE_BITS == 64

	// a is 1042 bit wide, so 17 chunks of 64 bit are necessary

	pfelement s1, s2;

	// Init partial results data
	chunk s1data[9]; // 64 * 9 = 576 = 521 + 55
	s1.data = s1data;
	chunk s2data[9];
	s2.data = s2data;

	// Assuming a = (a1041,...,a0) (1-bit chunks)
	// s1 = (a1041, ..., a521)
	s1.data[0] = (a->data[8] >> 9) + (a->data[9] << 55);
	s1.data[1] = (a->data[9] >> 9) + (a->data[10] << 55);
	s1.data[2] = (a->data[10] >> 9) + (a->data[11] << 55);
	s1.data[3] = (a->data[11] >> 9) + (a->data[12] << 55);
	s1.data[4] = (a->data[12] >> 9) + (a->data[13] << 55);
	s1.data[5] = (a->data[13] >> 9) + (a->data[14] << 55);
	s1.data[6] = (a->data[14] >> 9) + (a->data[15] << 55);
	s1.data[7] = (a->data[15] >> 9) + (a->data[16] << 55);
	s1.data[8] = a->data[16] >> 9;
	// s2 = (a520, ..., a2, a1, a0)
	s2.data[0] = a->data[0];
	s2.data[1] = a->data[1];
	s2.data[2] = a->data[2];
	s2.data[3] = a->data[3];
	s2.data[4] = a->data[4];
	s2.data[5] = a->data[5];
	s2.data[6] = a->data[6];
	s2.data[7] = a->data[7];
	s2.data[8] = a->data[8] & 0x00000000000001ff;

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(9, 521, &s1);
	char * s2dump = GetString(9, 521, &s2);
	free(s1dump);
	free(s2dump);
#endif

	// s1 + s2
	PFAddition(red, &s1, &s2, field);

#else // Set chunks to 32 bit

	// a is 1042 bit wide, so 33 chunks of 32 bit are necessary

	pfelement s1, s2;

	// Init partial results data
	chunk s1data[17]; // 32 * 17 = 544 = 521 + 23
	s1.data = s1data;
	chunk s2data[17];
	s2.data = s2data;

	// Assuming a = (a1041,...,a0) (1-bit chunks)
	// s1 = (a1041, ..., a521)
	s1.data[0] = (a->data[16] >> 9) + (a->data[17] << 23);
	s1.data[1] = (a->data[17] >> 9) + (a->data[18] << 23);
	s1.data[2] = (a->data[18] >> 9) + (a->data[19] << 23);
	s1.data[3] = (a->data[19] >> 9) + (a->data[20] << 23);
	s1.data[4] = (a->data[20] >> 9) + (a->data[21] << 23);
	s1.data[5] = (a->data[21] >> 9) + (a->data[22] << 23);
	s1.data[6] = (a->data[22] >> 9) + (a->data[23] << 23);
	s1.data[7] = (a->data[23] >> 9) + (a->data[24] << 23);
	s1.data[8] = (a->data[24] >> 9) + (a->data[25] << 23);
	s1.data[9] = (a->data[25] >> 9) + (a->data[26] << 23);
	s1.data[10] = (a->data[26] >> 9) + (a->data[27] << 23);
	s1.data[11] = (a->data[27] >> 9) + (a->data[28] << 23);
	s1.data[12] = (a->data[28] >> 9) + (a->data[29] << 23);
	s1.data[13] = (a->data[29] >> 9) + (a->data[30] << 23);
	s1.data[14] = (a->data[30] >> 9) + (a->data[31] << 23);
	s1.data[15] = (a->data[31] >> 9) + (a->data[32] << 23);
	s1.data[16] = a->data[32] >> 9;
	// s2 = (a520, ..., a2, a1, a0)
	s2.data[0] = a->data[0];
	s2.data[1] = a->data[1];
	s2.data[2] = a->data[2];
	s2.data[3] = a->data[3];
	s2.data[4] = a->data[4];
	s2.data[5] = a->data[5];
	s2.data[6] = a->data[6];
	s2.data[7] = a->data[7];
	s2.data[8] = a->data[8];
	s2.data[9] = a->data[9];
	s2.data[10] = a->data[10];
	s2.data[11] = a->data[11];
	s2.data[12] = a->data[12];
	s2.data[13] = a->data[13];
	s2.data[14] = a->data[14];
	s2.data[15] = a->data[15];
	s2.data[16] = a->data[16] & 0x00000000000001ff;

	// Debug info
#ifdef _DEBUG
	char * s1dump = GetString(17, 521, &s1);
	char * s2dump = GetString(17, 521, &s2);
	free(s1dump);
	free(s2dump);
#endif

	// s1 + s2
	PFAddition(red, &s1, &s2, field);

#endif
}