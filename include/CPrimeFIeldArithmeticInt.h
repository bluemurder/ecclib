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
*
* Some references for following algorithms:
* [1] D. Hankerson, S. Vanstone, and A.J. Menezes. Guide to Elliptic Curve
* Cryptography. Springer Professional Computing. Springer, 2004.
* [2] W. Hasenplaugh, G. Gaubatz, V. Gopal. Fast Modular Reduction.
* Proceedings of ARITH'07, 2007.
* [3] A.J. Menezes, P.C. van Oorschot, S.A. Vanstone. Handbook of Applied 
* Cryptography. Taylor & Francis, 2010.
* [4] D.E. Knuth, The Art of Computer Programming – Seminumerical Algorithms,
* volume 2, Addison-Wesley, Reading, Massachusetts, 2nd edition, 1981.
* [5] C. Koc, High-speed RSA implementation, Technical Report, RSA 
* Laboratories, 1994.
* [6] K. Geddes, S. Czapor, and G. Labahn, Algorithms for Computer Algebra, 
* Kluwer Academic Publishers, Boston, 1992.
* 
*
* Notes:
* From [3] some notes on best algorithms for multiprecsion arithmetic are 
* available. In general for modulo reduction, Montgomery reduction and barrett
* reduction are comparable. Concerning multiplication, the Karatsuba-Ofman 
* algorithm is considered a valid solution to improve efficiency of the 
* classical multiplication. For a more detailed treatment of the Karatsuba-
* Ofman algorithm, see Knuth [4], Koc [5], and Geddes, Czapor, and Labahn
* [6]. A detailed treatment of methods for performing modular multiplication
* can be found in Knuth [4].
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
	unsigned int muReady;		//!< 1 when mu is computed, 0 otherwise
};

typedef struct _field_t field_t;

unsigned int GreaterOrEqual(element_t * a, element_t * b, field_t * field);

unsigned int Equals(element_t * a, element_t * b, field_t * field);

//! Writes the hexdecimal value from hexString to data, strictly following 
//! the specified bitsize
//! \param hexString The hexdecimal value to write (MSB letf)
//! \param chunksNumber The number of chunks already allocated
//! \param bitSize The size in bits of the underlying field
//! \param data The vector of chunks to fill
void SetString(
	char * hexString, 
	unsigned int chunksNumber,
	unsigned int bitSize, 
	chunk_t * data);

//! Give a string containing the hexadecimal representation of the number 
//! stored in element. This is not an efficient implementation, use only for 
//! debug purposes.
//! \param chunksNumber The number of chunks of given data array
//! \param bitSize The size in bits of the number to print
//! \param data The multiprecision number data
//! \returns The hexadecimal representation string, remember to free such
//! memory
char * GetString(unsigned int chunksNumber, unsigned int bitSize, chunk_t * data);

//! Sets the specified bit size and characteristics in the field object
//! \param field The field to fill (must point to already allocated memory)
//! \param fieldBitSize The field bis size
//! \param characteristics Hex string describing the field characteristics
void SetField(field_t * field, unsigned int fieldBitSize, char * characteristics);

//! Sets the specified value expressed via a hex string in the field element 
//! object. Needs the presence of a field_t object storing field bitsize, 
//! chunks number and characteristics.
//! \param element The target element to write
//! \param hexString The string value to write
//! \param field The object storing field data
void SetElement(element_t * element, char * hexString, field_t * field);

//! Erase the memory of internal members of element
void FreeElement(element_t * element);

//! Erase the memory of internal members of field
void FreeField(field_t * field);

//! Addition of a and b over a prime field. Let p the field characteristics, 
//! the method evaluates a+b(mod p)
//! Algorithm 2.7 [1]
void Addition(element_t * sum, element_t * a, element_t * b, field_t * field);

//! Subtraction of b from a over a prime field. Let p the field 
//! characteristics, the method evaluates a-b(mod p)
//! Algorithm 2.8 [1]
void Subtraction(element_t * sub, element_t * a, element_t * b, field_t * field);

//! General modulo reduction
//! Algorithm 2.14 [1]
//! http://cacr.uwaterloo.ca/hac/about/chap14.pdf
void BarrettReduction(element_t * red, element_t a, field_t * field);

//! Modified Barrett modulo reduction [2]
void ModifiedBarretReduction(element_t * red, element_t a, field_t * field);

//! Fast modulo reduction for p = 2^192 − 2^64 − 1 (FIPS 186-4 prime)
//! Algorithm 2.27 [1]
void FastReductionFIPSp192(element_t * red, element_t * a, field_t * field);

//! Fast modulo reduction for p = 2^224 − 2^96 + 1 (FIPS 186-4 prime)
//! Algorithm 2.28 [1]
void FastReductionFIPSp224(element_t * red, element_t * a, field_t * field);

//! Fast modulo reduction for p = 2^256 - 2^224 + 2^192 + 2^96 - 1 
//! (FIPS 186-4 prime)
//! Algorithm 2.29 [1]
void FastReductionFIPSp256(element_t * red, element_t * a, field_t * field);

//! Fast modulo reduction for p = 2^384 - 2^128 - 2^96 + 2^32 - 1 
//! (FIPS 186-4 prime)
//! Algorithm 2.30 [1]
void FastReductionFIPSp384(element_t * red, element_t * a, field_t * field);

//! Fast modulo reduction for p = 2^521 - 1 (FIPS 186-4 prime)
//! Algorithm 2.31 [1]
void FastReductionFIPSp521(element_t * red, element_t * a, field_t * field);

#endif // CPRIMEFIELFARITHMETICINT_H
