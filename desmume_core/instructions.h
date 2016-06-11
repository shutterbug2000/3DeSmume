/*	Copyright (C) 2006 yopyop
	Copyright (C) 2012-2015 DeSmuME team

	This file is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with the this software.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _INSTRUCIONS_H_
#define _INSTRUCIONS_H_

#include "3ds/types.h"

#ifdef __MINGW32__
#define FASTCALL __attribute__((fastcall))
#define ASMJIT_CALL_CONV kX86FuncConvGccFastCall
#elif defined (__i386__) && !defined(__clang__)
#define FASTCALL __attribute__((regparm(3)))
#define ASMJIT_CALL_CONV kX86FuncConvGccRegParm3
#elif defined(_MSC_VER) || defined(__INTEL_COMPILER)
#define FASTCALL
#define ASMJIT_CALL_CONV kX86FuncConvDefault
#else
#define FASTCALL
#define ASMJIT_CALL_CONV kX86FuncConvDefault
#endif

typedef u32 (FASTCALL* OpFunc)(const u32 i);
extern const OpFunc arm_instructions_set[2][4096];
extern const char* arm_instruction_names[4096];
extern const OpFunc thumb_instructions_set[2][1024];
extern const char* thumb_instruction_names[1024];

#endif
