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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>

// Define here the target architecture bit number
#define ARCHITECTURE_BITS 32

#if ARCHITECTURE_BITS == 8
typedef uint8_t chunk;
#elif ARCHITECTURE_BITS == 16
typedef uint16_t chunk;
#elif ARCHITECTURE_BITS == 64
typedef uint64_t chunk;
#else
typedef uint32_t chunk;
#endif

#endif // GLOBALS_H
