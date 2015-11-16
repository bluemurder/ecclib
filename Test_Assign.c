/************************************************************************\
* Prime Field Arithmetic primitives                                     *
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

#include "CPrimeFieldArithmeticInt.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

int main()
{
	PrimeFieldElement * n = NewElement("ccccccfccccccccc", 37);
	n->m_data[0];
	n->m_data[1];
	n->m_data[2];
	n->m_data[3];
	n->m_data[4];

	PrimeFieldElement * m = NewElement("ffffffff", 13);
	m->m_data[0];
	m->m_data[1];
	wprintf(L"Hello");
}