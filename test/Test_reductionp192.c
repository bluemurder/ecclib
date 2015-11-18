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

#include "../include/CPrimeFieldArithmeticInt.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

int main()
{
	wprintf(L"CPrimeFieldArithmetic Assignment test");

	field_t field;
	SetField(&field, 192, "FFFFFFFFFFFFFFFFFFFFFFFF99DEF836146BC9B1B4D22831");
	element_t a;
	unsigned int cnumber = 384 / ARCHITECTURE_BITS;
	if (384 % ARCHITECTURE_BITS)
	{
		cnumber++;
	}
	a.data = (chunk_t *)malloc(cnumber * sizeof(chunk_t));
	SetString("FFFFFFFFFFFFFFFFFFFFFFFF99DEF836146BC9B1B4D22845", cnumber, 384, a.data);
	a.data[0];
	a.data[1];
	a.data[2];
	a.data[10];

	element_t reducted;
	SetElement(&reducted, "", &field);
	FastReductionFIPSp192(&reducted, &a, &field);

	reducted.data[0];
	reducted.data[1];
	reducted.data[2];
	reducted.data[3];
	reducted.data[4];
	reducted.data[5];
	return 0;
}