#ifndef _RARECHAR_HEADER_H_
#define _RARECHAR_HEADER_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

bool init_rare_table();

bool is_rare_unicode(unsigned char* buf);

bool is_rare_gb2312(unsigned char* buf);

bool is_rare_utf8(unsigned char* buf, int char_len);

#ifdef __cplusplus
};
#endif

#endif