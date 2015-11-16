/************************************************************************\
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


PrimeFieldElement * NewElement(char * hexString, size_t fieldBitSize)
{
	size_t hexStringLen = strlen(hexString);
	PrimeFieldElement * element = (PrimeFieldElement *)malloc(sizeof(PrimeFieldElement));

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
		for (j = 0; (j < maxChunkCharacters && readstring == 1); j++, charactersCounter++, bitsCounter+=4)
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
			// Shift when reading higher chunk bits
			value = value << (4 * j);
			// Sum to current chunk integer
			element->m_data[i] += value;
			// If 
			if (charactersCounter == hexStringLen-1)
			{
				readstring = 0;
			}
			else if (bitsCounter > fieldBitSize)
			{
				readstring = 0;
			}
			else
			{
				// Read next string character
				stringCursor--;
			}
		}
	}
	return element;
}

void FreeElement(PrimeFieldElement * element)
{

}