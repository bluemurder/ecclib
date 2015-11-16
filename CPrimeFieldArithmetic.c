/***********************************************************************\
* Prime Field Arithmetic primitives                                     *
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

#include "CPrimeFieldArithmeticInt.h"
#include <malloc.h>
#include <string.h>

//! Create a new prime field element from an hexadecimal string and a field 
//! size. Note that the field size overcomes the string if it is passed with 
//! higher dimension than specified field size.
PrimeFieldElement * NewElement(char * hexString, size_t fieldBitSize)
{
	size_t hexStringLen = strlen(hexString);
	PrimeFieldElement * element = (PrimeFieldElement *)malloc(sizeof(PrimeFieldElement));
	element->m_fieldBits = fieldBitSize;
	element->m_ChunksNumber = fieldBitSize / ARCHITECTURE_BITS;
	if (fieldBitSize % ARCHITECTURE_BITS)
	{
		element->m_ChunksNumber++;
	}
	element->m_data = (chunk_t *)malloc(element->m_ChunksNumber * sizeof(chunk_t));
	char * stringCursor = hexString + hexStringLen - 1;
	size_t i, j;
	unsigned int maxChunkCharacters = ARCHITECTURE_BITS / 4;
	chunk_t value;
	unsigned char readstring = 1;
	size_t charactersCounter = 0;
	size_t bitsCounter = 0;
	// For every chunk
	for (i = 0; i < element->m_ChunksNumber; i++)
	{
		element->m_data[i] = 0;
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
			if (bitsCounter + 4 > fieldBitSize)
			{
				// Apply proper bitmask
				size_t difference = bitsCounter + 4 - fieldBitSize;
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
			element->m_data[i] += value;
			// If input string with no more characters, exit
			if (charactersCounter + 1 == hexStringLen)
			{
				readstring = 0;
			}
			// Read next string character
			stringCursor--;
		}
	}
	return element;
}

//! Frees memory of specified element
void FreeElement(PrimeFieldElement * element)
{
	if (element != NULL)
	{
		free(element->m_data);
		element->m_data = NULL;
	}
}

PrimeFieldElement * AddMod(PrimeFieldElement * a, PrimeFieldElement * b, PrimeFieldElement * mod)
{
	// Assuming that a, b amd c are not null pointers. Care of caller to ensure this
	// Assuming that a and b present the SAME number of chunks. Care of caller to ensure this
	// Allocate 
	PrimeFieldElement * element = (PrimeFieldElement *)malloc(sizeof(PrimeFieldElement));
	element->m_data = (chunk_t *)malloc(a->m_ChunksNumber * sizeof(chunk_t));
	element->m_ChunksNumber = a->m_ChunksNumber;
	element->m_fieldBits = a->m_fieldBits;
	// Binary sum
	chunk_t carry = 0;
	chunk_t borrow = 0;
	size_t i;
	for (i = 0; i < a->m_ChunksNumber; i++)
	{
		if (carry)
		{
			element->m_data[i] = a->m_data[i] + b->m_data[i] + carry;
			carry = (a->m_data[i] >= element->m_data[i]);
		}
		else
		{
			element->m_data[i] = a->m_data[i] + b->m_data[i];
			carry = (a->m_data[i] > element->m_data[i]);
		}
	}
	// If carry, modulo reduction
	if (carry)
	{
		//borrow = carry;
		size_t maxElementChunk = element->m_ChunksNumber - 1;
		for (i = 0; i < element->m_ChunksNumber; i++)
		{
			//if (i > maxElementChunk)
			//{
			//	borrow = mod->m_data[i] - borrow;
			//}
			//else
			//{
				if (borrow)
				{
					element->m_data[i] = element->m_data[i] - mod->m_data[i] - borrow;
					borrow = (element->m_data[i] <= mod->m_data[i]);
				}
				else
				{
					element->m_data[i] = element->m_data[i] - mod->m_data[i];
					borrow = (element->m_data[i] < mod->m_data[i]);
				}
			//}
		}
	}
	return element;
}