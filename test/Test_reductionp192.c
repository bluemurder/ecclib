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

#include "../include/PrimeFieldArithmeticInt.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

int main()
{
	wprintf(L"CPrimeFieldArithmetic fast modular reduction test");

	pfproperties field;
	InitFieldProperties(&field, 192, "fffffffffffffffffffffffffffffffeffffffffffffffff");
	pfelement a;
	unsigned int cnumber = 384 / ARCHITECTURE_BITS;
	if (384 % ARCHITECTURE_BITS)
	{
		cnumber++;
	}
	a.data = (chunk *)malloc(cnumber * sizeof(chunk));
	SetString(&a, "f32825fc7b46e221f33861bad98abfaa7a919b3169eb3fc87f43e82788d59eb78f09ed1b06e4fcc5bb85e55acf4", cnumber, 384);
	char * hexdump = GetString(cnumber, 384, &a);
	a.data[0];
	a.data[1];
	a.data[2];
	a.data[10];

	pfelement reduced;
	InitElementByString(&reduced, "", &field);
	FastReductionFIPSp192(&reduced, &a, &field);

	char * hexdump2 = GetString(field.chunksNumber, field.bits, &reduced);

	reduced.data[0];
	reduced.data[1];
	reduced.data[2];
	reduced.data[3];
	reduced.data[4];
	reduced.data[5];

	pfelement true_red;
	InitElementByString(&true_red, "221eb65a46fdedda74084effc1003c59fbc71293fa688b47", &field);

	int retval = 0;
	if (Equals(&reduced, &true_red, &field))
	{
		retval = 1; // success
	}

	FreeElement(&a);
	FreeElement(&reduced);
	FreeElement(&true_red);
	FreeFieldProperties(&field);

	return retval;
}
