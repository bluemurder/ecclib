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

#ifndef PRIMEFIELDELEMENT_H
#define PRIMEFIELDELEMENT_H

#include "PrimeFieldGlobals.h"

struct _primeFieldElement
{
	// Stored number. m_data[0] is the least significant chunk.
	chunk_t * m_data;
	// Size of chunks for present element
	size_t m_ChunksNumber;
	// Bits number
	unsigned int m_fieldBits;
};

typedef struct _primeFieldElement PrimeFieldElement;

#endif // PRIMEFIELD8BITELEMENT_H