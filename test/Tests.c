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

//! \file Tests.c Main program to test library methods.

#include "../include/PrimeFieldArithmeticInt.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

unsigned int testSumSub()
{
	///////////////////////////////////////////////////////////////////////////
	wprintf(L"Sum and subtraction over prime field test...");

	pfproperties field;
	InitFieldProperties(&field, 45, "1fffffffffff");
	pfelement a;
	InitElementByString(&a, "009000000000000000000ff01", &field);
	pfelement b;
	InitElementByString(&b, "1ffffffffffe", &field);
	pfelement c;
	InitElementByString(&c, "0", &field);
	pfelement desired;
	PFAddition(&c, &a, &b, &field);

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

	InitElementByString(&desired, "00000000ff00", &field);
	if (Equals(&c, &desired, &field) != 1)
	{
		wprintf(L"\nSum over prime field test failed..\n");
		return 0;
	}

	pfelement d;
	InitElementByString(&d, "0", &field);
	PFSubtraction(&d, &a, &b, &field);

	d.data[0];
	d.data[1];
	d.data[2];
	d.data[3];
	d.data[4];
	d.data[5];
	d.data[6];
	d.data[7];

	// Desired d = 0x000000000000ff02
	FreeElement(&desired);
	InitElementByString(&desired, "00000000ff02", &field);
	if (Equals(&d, &desired, &field) != 1)
	{
		wprintf(L"\nSubtraction over prime field test failed..\n");
		return 0;
	}

	FreeElement(&desired);
	FreeFieldProperties(&field);
	FreeElement(&a);
	FreeElement(&b);
	FreeElement(&c);
	FreeElement(&d);

	wprintf(L"PASSED\n");
	return 1;
}

unsigned int testAssignment()
{
	///////////////////////////////////////////////////////////////////////////
	wprintf(L"PrimeFieldArithmetic Assignment test...");

	pfproperties field;
	InitFieldProperties(&field, 45, "1fffffffffff");
	pfelement n;
	InitElementByString(&n, "009000000000000000000000012", &field);
	n.data[0];
	n.data[1];
	n.data[2];
	n.data[3];
	n.data[4];
	n.data[5];
	n.data[6];
	n.data[7];

	pfelement desired;
	InitElementByString(&desired, "0000000000000000012", &field);
	if (Equals(&n, &desired, &field) != 1)
	{
		wprintf(L"\nAssignment over prime field test failed..\n");
		return 0;
	}
	FreeElement(&desired);
	FreeFieldProperties(&field);
	FreeElement(&n);

	wprintf(L"PASSED\n");
	return 1;
}

unsigned int testRedp192()
{
	wprintf(L"p192 fast modular reduction test...");

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

	unsigned int retval = 0;
	if (Equals(&reduced, &true_red, &field))
	{
		retval = 1; // success
		wprintf(L"PASSED\n");
	}
	else
	{
		wprintf(L"failed\n");
	}

	FreeElement(&a);
	FreeElement(&reduced);
	FreeElement(&true_red);
	FreeFieldProperties(&field);

	return retval;
}

unsigned int testRedp224()
{
	wprintf(L"p224 fast modular reduction test...");

	pfproperties field;
	InitFieldProperties(&field, 224, "ffffffffffffffffffffffffffffffff000000000000000000000001");
	pfelement a;
	unsigned int cnumber = 448 / ARCHITECTURE_BITS;
	if (448 % ARCHITECTURE_BITS)
	{
		cnumber++;
	}
	a.data = (chunk *)malloc(cnumber * sizeof(chunk));
	SetString(&a, "836599dda83f4f262f648033be340c2049385d9533ee3162d7219f0a86f3babdd10c85d6056a7ee5174a55cadbee486efd307da2334aea7b", cnumber, 448);
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
	//SetString(&a, "ffffffffffffffffffffffffffffffff000000000000100000000002", cnumber, 448);
	char * hexdump = GetString(cnumber, 448, &a);

	pfelement reduced;
	InitElementByString(&reduced, "", &field);
	FastReductionFIPSp224(&reduced, &a, &field);

	char * hexdump2 = GetString(field.chunksNumber, field.bits, &reduced);

	pfelement true_red;
	InitElementByString(&true_red, "c1c22d00f56b2e22b2337f3a5f9c68e70f5050fc2102fd192cc4cb3e", &field);

	unsigned int retval = 0;
	if (Equals(&reduced, &true_red, &field))
	{
		retval = 1; // success
		wprintf(L"PASSED\n");
	}
	else
	{
		wprintf(L"failed\n");
	}

	FreeElement(&a);
	FreeElement(&reduced);
	FreeElement(&true_red);
	FreeFieldProperties(&field);

	return retval;
}

unsigned int testRedp256()
{
	wprintf(L"p256 fast modular reduction test...");

	pfproperties field;
	InitFieldProperties(&field, 256, "ffffffff00000001000000000000000000000000ffffffffffffffffffffffff");
	pfelement a;
	unsigned int cnumber = 512 / ARCHITECTURE_BITS;
	if (512 % ARCHITECTURE_BITS)
	{
		cnumber++;
	}
	a.data = (chunk *)malloc(cnumber * sizeof(chunk));
	SetString(&a, "90a4cfe7612836599dda83f4f262f648033be340c2049385d9533ee3162d7219f0a86f3babdd10c85d6056a7ee5174a55cadbee486efd307da2334aea7b", cnumber, 512);
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
	char * hexdump = GetString(cnumber, 512, &a);

	pfelement reduced;
	InitElementByString(&reduced, "", &field);
	FastReductionFIPSp256(&reduced, &a, &field);

	char * hexdump2 = GetString(field.chunksNumber, field.bits, &reduced);

	pfelement true_red;
	InitElementByString(&true_red, "f1d27633742e39f0843c226ca2465d49ccd0f65af922bbc88bf97f3708699a28", &field);

	unsigned int retval = 0;
	if (Equals(&reduced, &true_red, &field))
	{
		retval = 1; // success
		wprintf(L"PASSED\n");
	}
	else
	{
		wprintf(L"failed\n");
	}

	FreeElement(&a);
	FreeElement(&reduced);
	FreeElement(&true_red);
	FreeFieldProperties(&field);

	return retval;
}

unsigned int testRedp384()
{
	wprintf(L"p384 fast modular reduction test...");

	pfproperties field;
	InitFieldProperties(&field, 384, "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000ffffffff");
	pfelement a;
	unsigned int cnumber = 768 / ARCHITECTURE_BITS;
	if (768 % ARCHITECTURE_BITS)
	{
		cnumber++;
	}
	a.data = (chunk *)malloc(cnumber * sizeof(chunk));
	SetString(&a, "82af13387880826f0a344d0eeeee6eff37e2ff2687673a686d1c1fe97f99004d32a4f04a3c55657c2a8242690914b3816623934182b88efa3ea8485b467ff8748feae38826db5749bed8c7712f6b0e83c8350ee9b924a44ad27139ef5d521507", cnumber, 768);
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
	char * hexdump = GetString(cnumber, 768, &a);

	pfelement reduced;
	InitElementByString(&reduced, "", &field);
	FastReductionFIPSp384(&reduced, &a, &field);

	char * hexdump2 = GetString(field.chunksNumber, field.bits, &reduced);

	pfelement true_red;
	InitElementByString(&true_red, "97239230b04efde94e7180bcea4088847553ac172dff706f9117f417266f798bc58c73a043728d88001fef7edcda715e", &field);

	unsigned int retval = 0;
	if (Equals(&reduced, &true_red, &field))
	{
		retval = 1; // success
		wprintf(L"PASSED\n");
	}
	else
	{
		wprintf(L"failed\n");
	}

	FreeElement(&a);
	FreeElement(&reduced);
	FreeElement(&true_red);
	FreeFieldProperties(&field);

	return retval;
}

unsigned int testRedp521()
{
	wprintf(L"p521 fast modular reduction test...");

	pfproperties field;
	InitFieldProperties(&field, 521, "1ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
	pfelement a;
	unsigned int cnumber = 1042 / ARCHITECTURE_BITS;
	if (1042 % ARCHITECTURE_BITS)
	{
		cnumber++;
	}
	a.data = (chunk *)malloc(cnumber * sizeof(chunk));
	SetString(&a, "33da86a55a040bd8d495be225c7fa77c031805877e2578a8421cc2f2f187e7706bdb958a9ed0bb77f4f50e8bcc3b62c72c1a5259e418568c2e92333d4f2711949f29cf1aa2413b1d663fcb636f23f6daaf574772928905379b92f5830a5b01d86d6a3a17946856eedc52fc454e32d10b1fa26919c7b84491b842be2050f6e6e16e527", cnumber, 1042);
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
	char * hexdump = GetString(cnumber, 1042, &a);

	pfelement reduced;
	InitElementByString(&reduced, "", &field);
	FastReductionFIPSp521(&reduced, &a, &field);

	char * hexdump2 = GetString(field.chunksNumber, field.bits, &reduced);

	pfelement true_red;
	InitElementByString(&true_red, "3bc5df4ee3d2352aa1642805236ae6d58d375564817f3efb403e483d3c5cc25a027e259b7bf4c984d76cc9418eebc83387642f4aa50bcfe5a0739ef9e7a6a38a21", &field);
	char * hexdump3 = GetString(field.chunksNumber, field.bits, &true_red);

	unsigned int retval = 0;
	if (Equals(&reduced, &true_red, &field))
	{
		retval = 1; // success
		wprintf(L"PASSED\n");
	}
	else
	{
		wprintf(L"failed\n");
	}

	FreeElement(&a);
	FreeElement(&reduced);
	FreeElement(&true_red);
	FreeFieldProperties(&field);

	return retval;
}

//! Main routine
//! \return 0 if some tests failed, 1 if all ok
int main()
{
	unsigned int result = 1;
	result = result && testSumSub();
	result = result && testAssignment();
	result = result && testRedp192();
	result = result && testRedp224();
	result = result && testRedp256();
	result = result && testRedp384();
	result = result && testRedp521();

	return result;
}