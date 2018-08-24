#pragma once

// -------------------------------------------------------------------------
#define SIZEOF_STATIC_ARRAY(a)		(sizeof(a) / sizeof(a[0]))
#define SAFE_FREE(p)				{ delete p; p = nullptr; }
#define SAFE_FREE_ARRAY(p)			{ delete[] p; p = nullptr; }

#define SET_BIT(item, bit) (item) |= (1 << (bit))
#define CLEAR_BIT(item, bit) (item) &= ~(1 << (bit))
#define IS_BIT_SET(item, bit) (((item) & (1 << (bit))) != 0)

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;

