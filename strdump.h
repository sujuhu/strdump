#ifndef LIB_STRDUMP_T_
#define LIB_STRDUMP_T_

#include "typedef.h"
#include <slist.h>

#define INVALID_DUMP_BUFFER  (int)0

#define CHARSET_ASCII   1
#define CHARSET_GB2312  2
#define CHARSET_UNICODE 3
#define CHARSET_UTF8    4


typedef struct _string_t
{
    int codeset;
    unsigned char* start;
    int  len;
}string_t;

typedef struct _item_t
{
    string_t    data;
    snode_t     node;
}item_t;

int open_dump_buffer(unsigned char* buffer, int size);

string_t* dump_string_first(int fd);

string_t* dump_string_next(int fd);

void close_dump_buffer(int fd);

#endif  // LIB_STRDUMP_T_