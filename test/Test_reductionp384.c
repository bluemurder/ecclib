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

#define FF_BITS 384
#define DFF_BITS 768

int main()
{
	wprintf(L"CPrimeFieldArithmetic fast modular reduction test");

	pfproperties field;
	InitFieldProperties(&field, FF_BITS, "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000ffffffff");
	pfelement a;
	unsigned int cnumber = DFF_BITS / ARCHITECTURE_BITS;
	if (DFF_BITS % ARCHITECTURE_BITS)
	{
		cnumber++;
	}
	a.data = (chunk *)malloc(cnumber * sizeof(chunk));
	SetString(&a, "82af13387880826f0a344d0eeeee6eff37e2ff2687673a686d1c1fe97f99004d32a4f04a3c55657c2a8242690914b3816623934182b88efa3ea8485b467ff8748feae38826db5749bed8c7712f6b0e83c8350ee9b924a44ad27139ef5d521507", cnumber, DFF_BITS);
	a.data[0];
	a.data[1];
	a.data[2];
	a.data[3];
	a.data[4];
	a.data[5];
	a.data[6];
	a.data[7];
	a.data[8];
	a.data[9];
	a.data[10];
	a.data[11];
	a.data[12];
	a.data[13];
	a.data[14];
	char * hexdump = GetString(cnumber, DFF_BITS, &a);

	pfelement reduced;
	InitElement(&reduced, "", &field);
	FastReductionFIPSp384(&reduced, &a, &field);

	char * hexdump2 = GetString(field.chunksNumber, field.bits, &reduced);

	pfelement true_red;
	InitElement(&true_red, "97239230b04efde94e7180bcea4088847553ac172dff706f9117f417266f798bc58c73a043728d88001fef7edcda715e", &field);

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
