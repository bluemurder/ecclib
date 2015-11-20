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
	element_t mu;				//!< Barrett reduction precomputed term
};

typedef struct _field_t field_t;

unsigned int GreaterOrEqual(element_t * a, element_t * b, field_t * field);

unsigned int Equals(element_t * a, element_t * b, field_t * field);

void SetString(char * hexString, unsigned int chunksNumber, unsigned int bitSize, chunk_t * data);

char * GetString(unsigned int chunksNumber, unsigned int bitSize, chunk_t * data);

void SetField(field_t * field, unsigned int fieldBitSize, char * characteristics);

void SetElement(element_t * element, char * hexString, field_t * field);

void FreeElement(element_t * element);

void Addition(element_t * sum, element_t * a, element_t * b, field_t * field);

void Subtraction(element_t * sub, element_t * a, element_t * b, field_t * field);

void FastReductionFIPSp192(element_t * red, element_t * a, field_t * field);

void FastReductionFIPSp224(element_t * red, element_t * a, field_t * field);

#endif // CPRIMEFIELFARITHMETICINT_H
