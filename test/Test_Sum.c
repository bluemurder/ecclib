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
	PrimeFieldElement * mod = NewElement("10e", 9);
	mod->m_data[0];
	mod->m_data[1];

	PrimeFieldElement * a = NewElement("ff", 8);
	a->m_data[0];

	PrimeFieldElement * b = NewElement("ff", 8);
	b->m_data[0];

	PrimeFieldElement * sum = AddMod(a,b,mod);
	sum->m_data[0];
	sum->m_data[1];

	PrimeFieldElement * c = NewElement("fe", 9);
	c->m_data[0];

	PrimeFieldElement * d = NewElement("ff", 9);
	d->m_data[0];

	PrimeFieldElement * sub = SubMod(c, d, mod);
	sub->m_data[0];
	sub->m_data[1];

	wprintf(L"Hello");
}