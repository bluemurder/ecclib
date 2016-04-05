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

#define FF_BITS 256
#define DFF_BITS 512

int main()
{
	wprintf(L"CPrimeFieldArithmetic fast modular reduction test");

	pfproperties field;
	InitFieldProperties(&field, FF_BITS, "ffffffff00000001000000000000000000000000ffffffffffffffffffffffff");
	pfelement a;
	unsigned int cnumber = DFF_BITS / ARCHITECTURE_BITS;
	if (DFF_BITS % ARCHITECTURE_BITS)
	{
		cnumber++;
	}
	a.data = (chunk *)malloc(cnumber * sizeof(chunk));
	SetString(&a, "90a4cfe7612836599dda83f4f262f648033be340c2049385d9533ee3162d7219f0a86f3babdd10c85d6056a7ee5174a55cadbee486efd307da2334aea7b", cnumber, DFF_BITS);
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
	//SetString(&a, "efffffff00000002000000000000000000000001ffffffffffffffffffffffff", cnumber, 448);
	char * hexdump = GetString(cnumber, DFF_BITS, &a);

	pfelement reduced;
	InitElementByString(&reduced, "", &field);
	FastReductionFIPSp256(&reduced, &a, &field);

	char * hexdump2 = GetString(field.chunksNumber, field.bits, &reduced);

	pfelement true_red;
	InitElementByString(&true_red, "f1d27633742e39f0843c226ca2465d49ccd0f65af922bbc88bf97f3708699a28", &field);

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
