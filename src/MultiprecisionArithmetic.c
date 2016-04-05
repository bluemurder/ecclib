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

void InitNumberByString(mpnumber * number, char * hexString, unsigned int bitSize)
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
	// Note that div and res must be previously allocated:

	// Evaluation of number dimensions
	unsigned int i, m, n;
	for (i = v->size - 1;; i--)
	{
		if (i == 0 || v->data[i] == 0)
		{
			break;
		}
	}
	n = i + 1;
	// Now n is the number of not null chunks of v
	m = n;
	for (i = u->size - 1;; i--)
	{
		if (i == 0 || u->data[i] == 0)
		{
			break;
		}
	}
	m = i + 1;
	// Now m is the index to most significant not null chunk of u
	// Normalize phase (Knuth)
	chunk d = 0;
	d = d - 1;
	d = d / v->data[n - 1];
	mpnumber normalizedu;
	InitNumber(&normalizedu, m);
}

void ShortDivision(mpnumber * div, mpnumber * rem, mpnumber * a, mpnumber * b)
{
	// Short division requires that divisor presents only one data chunk. So 
	// present algorithm takes care of only least significant chunk, ingnoring 
	// possible other ones.
	// Note that div and res must be previously allocated:
	// div has same mpnumber size;
	// rem has size 1
	//unsigned int i,j;
	//chunk partial;
	//for (i = 0,j=a->size-1; i < a->size; i++,j--)
	//{
	//	partial = a->data[j] - 
	//}
}