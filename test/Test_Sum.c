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
	wprintf(L"CPrimeFieldArithmetic modular sum and modular subtraction  test");

	pfproperties field;
	InitFieldProperties(&field, 45, "1fffffffffff");
	pfelement a;
	InitElementByString(&a, "009000000000000000000ff01", &field);
	pfelement b;
	InitElementByString(&b, "1ffffffffffe", &field);
	pfelement c;
	InitElementByString(&c, "0", &field);
	Addition(&c, &a, &b, &field);

	b.data[0];
	b.data[1];
	b.data[2];
	b.data[3];
	b.data[4];
	b.data[5];

	c.data[0];
	c.data[1];
	c.data[2];
	c.data[3];
	c.data[4];
	c.data[5];
	c.data[6];
	c.data[7];

	// Desired c = 0x000000000000ff00

	pfelement d;
	InitElementByString(&d, "0", &field);
	Subtraction(&d, &a, &b, &field);

	d.data[0];
	d.data[1];
	d.data[2];
	d.data[3];
	d.data[4];
	d.data[5];
	d.data[6];
	d.data[7];

	// Desired d = 0x000000000000ff02

	FreeFieldProperties(&field);
	FreeElement(&a);
	FreeElement(&b);
	FreeElement(&c);
	FreeElement(&d);

	return 0;
}
