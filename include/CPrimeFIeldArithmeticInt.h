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

/*!
* \file CPrimeFieldArithmetic.h
* Header file for prime field arithmetic functions.
* 
* Present library is designed to allow deploying on 8, 16, 32 and 64 bit
* platforms. User have to define globals in "PrimeFieldGlobals.h"
* accordingly.
*
* First steps to get a working environment:
* 1) Define a field_t object, it will be unique for operating on a certain
* field. 
* 2) Call SetField() to set parameters and allocate needed memory.
* 3) Define some element_t that will represent elements belonging to the 
* previously defined field.
* 4) Call SetElement() to set element values and allocate memory.
* 5) Now perform operations among field elements.
* 6) Call FreeElement() on every element to free memory.
* 7) Call FreeField() to free field memory.
*
* Warning: no null pointer deferencing check is performed on current
* library functions. It is responsibility of the developer to provide valid
* data to present code.
*/

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
	unsigned int muReady;		//!< Set to 1 when mu is already computed, 0 otherwise
};

typedef struct _field_t field_t;

unsigned int GreaterOrEqual(element_t * a, element_t * b, field_t * field);

unsigned int Equals(element_t * a, element_t * b, field_t * field);

void SetString(char * hexString, unsigned int chunksNumber, unsigned int bitSize, chunk_t * data);

char * GetString(unsigned int chunksNumber, unsigned int bitSize, chunk_t * data);

void SetField(field_t * field, unsigned int fieldBitSize, char * characteristics);

void SetElement(element_t * element, char * hexString, field_t * field);

void FreeElement(element_t * element);

void FreeField(field_t * field);

void Addition(element_t * sum, element_t * a, element_t * b, field_t * field);

void Subtraction(element_t * sub, element_t * a, element_t * b, field_t * field);

void BarrettReduction(element_t * red, element_t a, field_t * field);

void ModifiedBarretReduction(element_t * red, element_t a, field_t * field);

void FastReductionFIPSp192(element_t * red, element_t * a, field_t * field);

void FastReductionFIPSp224(element_t * red, element_t * a, field_t * field);

void FastReductionFIPSp256(element_t * red, element_t * a, field_t * field);

void FastReductionFIPSp384(element_t * red, element_t * a, field_t * field);

void FastReductionFIPSp521(element_t * red, element_t * a, field_t * field);

#endif // CPRIMEFIELFARITHMETICINT_H
