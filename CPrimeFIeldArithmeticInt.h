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

#ifndef CPRIMEFIELFARITHMETICINT_H
#define CPRIMEFIELFARITHMETICINT_H

#include "PrimeFieldGlobals.h"
#include "PrimeFieldElement.h"

// Create new prime field element
PrimeFieldElement * NewElement(char * hexString, unsigned int fieldBitSize);

// Create new prime field element
void FreeElement(PrimeFieldElement * element);

// Modular addition of two elements, with specified modulus
PrimeFieldElement * AddMod(PrimeFieldElement * a, PrimeFieldElement * b, PrimeFieldElement * p);

// Modular subtraction of two elements, with specified modulus
PrimeFieldElement * SubMod(PrimeFieldElement * a, PrimeFieldElement * b, PrimeFieldElement * p);

#endif // CPRIMEFIELFARITHMETICINT_H