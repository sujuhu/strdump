#ifndef LIB_STRDUMP_T_
#define LIB_STRDUMP_T_


#define INVALID_DUMP_BUFFER  (int)0

#define CHARSET_ASCII   1	/*ASCII编码字符串*/
#define CHARSET_GB2312  2	/*GB2312编码字符串*/
#define CHARSET_UNICODE 3	/*UNICODE编码字符串*/
#define CHARSET_UTF8    4	/*UTF8编码字符串*/

typedef struct _string_t
{
    int codeset;			/*字符串编码*/
    unsigned int offset;	/*字符串在文件中的偏移*/
    int  len;				/*字符串长度*/
}string_t;

/* 
 	打开字符串dump
	@buffer:  内容
	@size:    内容长度
	返回:  句柄， 如果失败， 返回 INVALID_DUMP_BUFFER
 */
int open_dump_buffer(unsigned char* buffer, int size);

/*
	枚举字符串
 */
string_t* dump_string_first(int fd);

string_t* dump_string_next(int fd);

/*
	关闭字符串dump句柄
 */
void close_dump_buffer(int fd);

#endif  // LIB_STRDUMP_T_