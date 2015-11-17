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

#ifndef CPRIMEFIELFARITHMETICINT_H
#define CPRIMEFIELFARITHMETICINT_H

#include "PrimeFieldGlobals.h"
#include "PrimeFieldElement.h"

//! Field element structure
struct _element_t
{
	chunk_t * data; //!< The data array
};

typedef struct _element_t element_t;

//! Field properties structure
struct _field_t
{
	unsigned int bits;			//!< Field dimension in bits
	unsigned int chunksNumber;	//!< Number of chunks of every field element
	element_t characteristics;	//!< Field characteristics
};

typedef struct _field_t field_t;

// Create new prime field element
//PrimeFieldElement * NewElement(char * hexString, unsigned int bitSize, unsigned int fieldBitSize);

unsigned int GreaterOrEqual(element_t * a, element_t * b, field_t * field);

void SetString(char * hexString, unsigned int chunksNumber, unsigned int bitSize, chunk_t * data);

void SetField(field_t * field, unsigned int fieldBitSize, char * characteristics);

void SetElement(element_t * element, char * hexString, field_t * field);

void FreeElement(element_t * element);

void Addition(element_t * sum, element_t * a, element_t * b, field_t * field);

void Subtraction(element_t * sub, element_t * a, element_t * b, field_t * field);

// Create new prime field element
//void FreeElement(PrimeFieldElement * element);

// Modular addition of two elements, with specified modulus
//PrimeFieldElement * AddMod(PrimeFieldElement * a, PrimeFieldElement * b, PrimeFieldElement * p);

// Modular subtraction of two elements, with specified modulus
//PrimeFieldElement * SubMod(PrimeFieldElement * a, PrimeFieldElement * b, PrimeFieldElement * p);

// Fast modulo reduction for p = 2^192 − 2^64 − 1
//PrimeFieldElement * FastReductionFIPSp192(PrimeFieldElement * a, PrimeFieldElement * p);

#endif // CPRIMEFIELFARITHMETICINT_H