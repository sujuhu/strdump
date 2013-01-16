//#define _DEBUG_PRINT
#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include "typedef.h"
#include <queue.h>
#include "strdump.h"
#include "strdump1.h"
#include "strdump2.h"
#include "rarechar.h"

typedef struct _dump_t
{
    queue_t     strlist;
    uint8_t*    buffer;
    size_t      size;
}dump_t;


int open_dump_buffer(unsigned char* buffer, int size)
{
	if( buffer == NULL || size <= 0 ) 
		return INVALID_DUMP_BUFFER;

    if (!init_rare_table()){
        return INVALID_DUMP_BUFFER;
    }

    dump_t* dump = (dump_t*)malloc(sizeof(dump_t));
    if (dump == NULL) {
        return INVALID_DUMP_BUFFER;
    }
    memset(dump, 0, sizeof(dump_t));
    queue_init(&dump->strlist, 40960);
    dump->buffer = buffer;
    dump->size = size;

    //dump_string1(buffer, size, &dump->strlist);
    dump_string2((char*)buffer, size, &dump->strlist);
	return (intptr_t)dump;
}

string_t* dump_string_first(int fd)
{
  if (fd == INVALID_DUMP_BUFFER){
    errno = EINVAL;
    return NULL;
  }

  dump_t* dump = (dump_t*)(intptr_t)fd;
  return (string_t*)queue_get(&dump->strlist); 
}

string_t* dump_string_next(int fd)
{
    return dump_string_first(fd);
}

void close_dump_buffer(int fd)
{
    if (fd == INVALID_DUMP_BUFFER) {
        return;
    }

    dump_t* dump = (dump_t*)fd;
    fifo_close(&dump->strlist);
    free(dump);
    dump = NULL;
}


/*
bool internal_on_discover_string(ulong_t raw_ofs, uchar_t* str, ulong_t cc )
{
    crc32_calc_t crc32_calc;
    ulong_t crc = crc32_calc.do_hash( (uchar_t*)str, cc );
    uchar_t tmp = 0;
    // 串存在则直接返回
	if( m_crc_str_map.find( crc ) ) 
		return true;
	memcpy( m_tmp_mbs, str, cc );
	m_tmp_mbs[cc] = 0;
	// 要是不符合要求，则返回
	if( !on_discover_string( raw_ofs,(char*)m_tmp_mbs,cc,crc) ) 
		return false;
	// 将串的CRC放入MAP
	m_crc_str_map.insert( crc, tmp );
	// 计数加1
	m_dumpedc ++;
    // 返回
    return true;
}

bool on_discover_string(ulong_t raw_ofs, char* str, ulong_t cc, ulong_t crc )
{
    return true;
}
*/

