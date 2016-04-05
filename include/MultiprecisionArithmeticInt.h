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

/*!
* \file MultiprecisionArithmetic.h
* Header file for multiple precision positive integer arithmetic functions.
*/

#ifndef MULTIPRECISIONARITHMETICINT_H
#define MULTIPRECISIONARITHMETICINT_H

#include "Globals.h"

//! Generic multiple precision number structure
struct _mpnumber
{
	chunk * data;
	unsigned int size;
};

//! Generic multiple precision number type
typedef struct _mpnumber mpnumber;

//! Multiple precision addition algorithm
//! 
void MPAddition(mpnumber * sum, mpnumber * a, mpnumber * b);

#endif // MULTIPRECISIONARITHMETICINT_H