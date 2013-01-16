#ifndef _STRDUMP2_HEADER_H_
#define _STRDUMP2_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

bool dump_string2(char* buffer, int size, queue_t* strlist);


#ifdef __cplusplus
};
#endif

#endif