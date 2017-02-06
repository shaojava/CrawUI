#pragma once

#define FLOAT_TO_INT(f,i) __asm \
	{ \
	__asm fld   f \
	__asm fistp i \
	}