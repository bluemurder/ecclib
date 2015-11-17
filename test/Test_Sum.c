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
	SetField(&field, 45, "1fffffffffff");
	element_t a;
	SetElement(&a, "0090000000000000000000001", &field);
	element_t b;
	SetElement(&b, "1ffffffffffe", &field);
	element_t c;
	SetElement(&c, "0", &field);
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

	element_t d;
	SetElement(&d, "0", &field);
	Subtraction(&d, &a, &b, &field);

	d.data[0];
	d.data[1];
	d.data[2];
	d.data[3];
	d.data[4];
	d.data[5];
	d.data[6];
	d.data[7];


	//PrimeFieldElement * mod = NewElement("10e", 9);
	//mod->m_data[0];
	//mod->m_data[1];

	//PrimeFieldElement * a = NewElement("ff", 8);
	//a->m_data[0];

	//PrimeFieldElement * b = NewElement("ff", 8);
	//b->m_data[0];

	//PrimeFieldElement * sum = AddMod(a,b,mod);
	//sum->m_data[0];
	//sum->m_data[1];

	//PrimeFieldElement * c = NewElement("fe", 9);
	//c->m_data[0];

	//PrimeFieldElement * d = NewElement("ff", 9);
	//d->m_data[0];

	//PrimeFieldElement * sub = SubMod(c, d, mod);
	//sub->m_data[0];
	//sub->m_data[1];



	return 0;
}