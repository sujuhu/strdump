#ifndef LIB_STRDUMP_T_
#define LIB_STRDUMP_T_

#define INVALID_DUMP_BUFFER  (int)0

typedef struct _string_t
{
    unsigned char* start;
    int  len;
}string_t;

int open_dump_buffer(unsigned char* buffer, int size);

string_t* dump_string_first(int fd);

string_t* dump_string_next(string_t* it);

void close_dump_buffer(int fd);

#endif  // LIB_STRDUMP_T_