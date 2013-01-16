#ifndef _ITEM_HEADER_H_
#define _ITEM_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#include "strdump.h"
	
typedef struct _item_t
{
    string_t    data;
}item_t;

#ifdef __cplusplus
};
#endif

#endif