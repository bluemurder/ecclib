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
	wprintf(L"CPrimeFieldArithmetic fast modular reduction test");

	field_t field;
	SetField(&field, 224, "ffffffffffffffffffffffffffffffff000000000000000000000001");
	element_t a;
	unsigned int cnumber = 448 / ARCHITECTURE_BITS;
	if (448 % ARCHITECTURE_BITS)
	{
		cnumber++;
	}
	a.data = (chunk_t *)malloc(cnumber * sizeof(chunk_t));
	SetString("836599dda83f4f262f648033be340c2049385d9533ee3162d7219f0a86f3babdd10c85d6056a7ee5174a55cadbee486efd307da2334aea7b", cnumber, 448, a.data);
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
	//SetString("ffffffffffffffffffffffffffffffff000000000000100000000002", cnumber, 448, a.data);
	char * hexdump = GetString(cnumber, 448, a.data);

	element_t reduced;
	SetElement(&reduced, "", &field);
	FastReductionFIPSp224(&reduced, &a, &field);

	char * hexdump2 = GetString(field.chunksNumber, field.bits, reduced.data);

	element_t true_red;
	SetElement(&true_red, "c1c22d00f56b2e22b2337f3a5f9c68e70f5050fc2102fd192cc4cb3e", &field);

	if (Equals(&reduced, &true_red, &field))
	{
		return 1;
	}
	return 0;
}
