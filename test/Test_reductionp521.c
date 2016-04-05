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

#define FF_BITS 521
#define DFF_BITS 1042

int main()
{
	wprintf(L"CPrimeFieldArithmetic fast modular reduction test");

	pfproperties field;
	InitFieldProperties(&field, FF_BITS, "1ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
	pfelement a;
	unsigned int cnumber = DFF_BITS / ARCHITECTURE_BITS;
	if (DFF_BITS % ARCHITECTURE_BITS)
	{
		cnumber++;
	}
	a.data = (chunk *)malloc(cnumber * sizeof(chunk));
	SetString(&a, "33da86a55a040bd8d495be225c7fa77c031805877e2578a8421cc2f2f187e7706bdb958a9ed0bb77f4f50e8bcc3b62c72c1a5259e418568c2e92333d4f2711949f29cf1aa2413b1d663fcb636f23f6daaf574772928905379b92f5830a5b01d86d6a3a17946856eedc52fc454e32d10b1fa26919c7b84491b842be2050f6e6e16e527", cnumber, DFF_BITS);
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
	FastReductionFIPSp521(&reduced, &a, &field);

	char * hexdump2 = GetString(field.chunksNumber, field.bits, &reduced);

	pfelement true_red;
	InitElement(&true_red, "3bc5df4ee3d2352aa1642805236ae6d58d375564817f3efb403e483d3c5cc25a027e259b7bf4c984d76cc9418eebc83387642f4aa50bcfe5a0739ef9e7a6a38a21", &field);
	char * hexdump3 = GetString(field.chunksNumber, field.bits, &true_red);

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
