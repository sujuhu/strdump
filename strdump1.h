#ifndef _STRDUMP1_HEADER_H_
#define _STRDUMP1_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif
#include <slist.h>


bool dump_string1(unsigned char* buffer, int size, slist_t* strlist);


#ifdef __cplusplus
};
#endif

#endif