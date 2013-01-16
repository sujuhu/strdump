#pragma warning( disable:4996 )
#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>
#include <memory.h>
#include "typedef.h"
#include <queue.h>
#include "rarechar.h"
#include "strdump2.h"
#include "strdump.h"
#include "item.h"

bool is_ascii(char ch)
{
    if (ch < 0x20 || ch > 0x7d)
        return false;

    if (ch == 0x21 || ch == 0x24)
        return false;
    return true;
}

bool is_alpha(char ch)
{
    if ( (ch > 'a' && ch < 'z')
        || (ch > 'A' && ch < 'Z') 
        || (ch > '0' && ch < '9')
        || ch == '.'
        || ch == ' ')
        return true;
    return false;
}

bool is_valid_ascii_string(char* str, int len)
{
    bool exist_vowel = false;       //是否存在元音字符
    bool all_upper = true;
    bool last_not_alpha = false;
    for (int i=0; i<len; i++) {
        if (str[i] == 'a' || str[i] == 'A' 
            || str[i] == 'e' || str[i] == 'E'
            || str[i] == 'i' || str[i] == 'I'
            || str[i] == 'o' || str[i] == 'O'
            || str[i] == 'u' || str[i] == 'U') {
            exist_vowel = true;
        }

        if (str[i] < 'A' || str[i] > 'Z') {
            all_upper = false;
        }

        
        // if (!is_alpha(str[i])) {
        //         //连续出现两个符号
        //     if (last_not_alpha) {
        //         return false;
        //     } else {
        //         last_not_alpha = true;
        //     }
        // } else {
        //     last_not_alpha = false;
        // }
    }

    if (!exist_vowel) {
        //不存在元音字母
        //是否都是大写
        if (all_upper)
            return true;
        else
            return false;
    } else {
        //存在元音字母
        return true;
    }
}

#define MIN_ASCII_SIZE   8
#define MAX_ASCII_SIZE   128
bool is_ascii_string(char* str, int* len)
{
    *len = 0;
    for(int i= 0; i < MAX_ASCII_SIZE; i++) {
        if (!is_ascii(str[i])) {
            if (i <= MIN_ASCII_SIZE) {
                return false;
            } else {
                *len = i;
                if (!is_valid_ascii_string(str, i)) {
                    return false;
                }
                return true;
            }
        }
    }

    *len = MAX_ASCII_SIZE;
    if (!is_valid_ascii_string(str, MAX_ASCII_SIZE)) {
        return false;
    }
    return true;
}

#define CHARBetween(CH_,Low_,Hi_)  (((CH_)>=(Low_)) && ((CH_)<=(Hi_)))
#define CHARBetween(CH_,Low_,Hi_)  (((CH_)>=(Low_)) && ((CH_)<=(Hi_)))

bool    is_gb2312(unsigned char* pBuf)
{   
#define GB2312ZONENum  9
  const unsigned char GB2312Tbl[GB2312ZONENum][10]={
    {0xA1,0xA1,0xFE,0},
    {0xA2,0xB1,0xE2,0xE5,0xEE,0xF1,0xFC,0},
    {0xA3,0xA1,0xFE,0},
    {0xA4,0xA1,0xF3,0},
    {0xA5,0xA1,0xF6,0},
    {0xA6,0xA1,0xB8,0xC1,0xD8,0},
    {0xA7,0xA1,0xC1,0xD1,0xF1,0},
    {0xA8,0xA1,0xBA,0xC5,0xE9,0},
    {0xA9,0xA4,0xEF,0}
  };

  int  Zone,hi,low;
  if ((pBuf[0] < 0xA1)|| (pBuf[0] > 0xF7)) return false;
 
  hi = GB2312ZONENum-1;
  low = 0;
  while (low <= hi)
  {
    Zone = (hi+low+1)/2;
    if (pBuf[0] == GB2312Tbl[Zone][0])
    {
      for (low=1;;low+=2)
      {
        if (GB2312Tbl[Zone][low] == 0) return false;
        if (CHARBetween(pBuf[1],GB2312Tbl[Zone][low],GB2312Tbl[Zone][low+1])) return true;
      }
    }
    if (low == hi) break;
    if (pBuf[0] < GB2312Tbl[Zone][0]) hi= Zone-1;
    else low = Zone+1;
  }
  if ((pBuf[0] == 0xD7) && CHARBetween(pBuf[1],0xFA,0xFE)) return false;
  if (CHARBetween(pBuf[0],0xB0,0xF7) && CHARBetween(pBuf[1],0xA1,0xFE)) return true;
  return false;
}

bool    is_gbk(unsigned char* pBuf)
{
  if ((pBuf[0]<0x81)||(pBuf[0]==0xFF)) return false;
  if ((pBuf[1]<0x40)||(pBuf[1]==0xFF)) return false;
  if ((pBuf[1]==0x7F)) return false;
  return true;
}


// int is_gbk(unsigned short ch)
// {
//     int r=0;
//     int iHead = ch & 0xff;
//     int iTail = (ch >> 16) & 0xff;

//     if ((iHead>=0x81 && iHead<=0xfe &&
//     (iTail>=0x40 && iTail<=0x7e || iTail>=0x80 && iTail<=0xfe)) ||
//     (iHead>=0xa1 && iHead<=0xf7 && iTail>=0xa1 && iTail<=0xfe) ||
//     (iHead>=0xa1 && iHead<=0xf9 &&
//     (iTail>=0x40 && iTail<=0x7e || iTail>=0xa1 && iTail<=0xfe)))
//     {
//         return true;
//     }
//     return false;
// }

int gbk_to_unicode(
    uint8_t* in_buf, size_t in_len, 
    uint8_t* out_buf, size_t out_len)
{
    memset(out_buf, 0, out_len);
    iconv_t fd = iconv_open("UCS-2-INTERNAL","GBK");
    if (fd == (iconv_t)-1) {
        return 0;
    }
    char* in = (char*)in_buf;
    char* out = (char*)out_buf;
    size_t out_left = out_len;
    int ret = iconv(fd, &in, &in_len, &out, &out_left);
    if (-1 == ret) {
        iconv_close(fd);
        return 0;
    }

    iconv_close(fd);
    return out_len - out_left;
}

bool is_valid_gb2312(unsigned short ch)
{
    unsigned short unicode = 0;
    int ret = gbk_to_unicode((uint8_t*)&ch, 2, (uint8_t*)&unicode, 2);
    if (ret == -1) {
        return false;
    }

    if (unicode < 0x4E00 || unicode > 0x9FA5) {
        return false;
    }

    if (is_rare_unicode((unsigned char*)&unicode)) {
        return false;
    }

    return true;
}

#define MAX_GB2312_SIZE  64
#define MIN_GB2312_SIZE   4
int is_gb2312_string(unsigned short* str, int* len)
{
    *len = 0;
    for(int i= 0; i < MAX_GB2312_SIZE; i++) {
        if (!is_gb2312((unsigned char*)&str[i]) 
            || !is_valid_gb2312(str[i])) {
            if (i <= MIN_GB2312_SIZE) {
                return false;
            } else {
                *len = i * 2;
                return true;
            }
        }
    }

    *len = MAX_GB2312_SIZE *2;
    return true;
}

bool is_unicode_chinese(unsigned short utf)
{
    if ((utf >= 0x0020 && utf <= 0x007E)
    || (utf >= 0x4E00 && utf <= 0x9FA5) )
        return true;
    return false;
}

#define MAX_UNICODE_SIZE  64
#define MIN_UNICODE_SIZE   4
bool is_unicode_string(unsigned short* str, int* len)
{
    *len = 0;
    for(int i= 0; i < MAX_UNICODE_SIZE; i++) {
        if (is_unicode_chinese(str[i]) && !is_rare_unicode((uint8_t*)&str[i])) {
            continue;
        } else {
            if (i <= MIN_UNICODE_SIZE) {
                return false;
            } else {
                *len = i * 2;
                return true;
            }
        }
    }

    *len = MAX_UNICODE_SIZE *2;
    return true;
}

bool is_utf8(char* str,int* utf8_bytes) 
{ 
    int nBytes = 0;
    unsigned char chr= str[0]; 
    if (chr < 0x80) {
        //ASCII码
        if (!is_ascii(chr)) {
            *utf8_bytes = 0;
            return false;
        }
        *utf8_bytes = 1;
    } else { 
        //不是ASCII码,应该是多字节符,计算字节数 
        if(chr>=0xFC&&chr<=0xFD) 
            nBytes=6; 
        else if(chr>=0xF8) 
            nBytes=5; 
        else if(chr>=0xF0) 
            nBytes=4; 
        else if(chr>=0xE0) 
            nBytes=3; 
        else if(chr>=0xC0) 
            nBytes=2; 
        else { 
            *utf8_bytes  = 0;
            return false; 
        }

        *utf8_bytes = nBytes;
        nBytes--; 

        for(int i=0; i<nBytes; i++) { 
            //多字节符的非首字节,应为 10xxxxxx 
            if( (str[1+i] & 0xC0) != 0x80 ) { 
                *utf8_bytes  = 0;
                return false; 
            } 
            nBytes--; 
        }
    } 

    return true;

}

int utf8_to_unicode(
    uint8_t* in_buf, size_t in_len, 
    uint8_t* out_buf, size_t out_len)
{
    memset(out_buf, 0, out_len);
    iconv_t fd = iconv_open("UCS-2-INTERNAL","UTF-8");
    if (fd == (iconv_t)-1) {
        return 0;
    }
    char* in = (char*)in_buf;
    char* out = (char*)out_buf;
    size_t out_left = out_len;
    int ret = iconv(fd, &in, &in_len, &out, &out_left);
    if (-1 == ret) {
        iconv_close(fd);
        return 0;
    }

    iconv_close(fd);
    return out_len - out_left;
}

bool is_valid_utf8(char* ch, int char_len)
{
    if(char_len != 1 && char_len != 3) {
        return false;
    }

    if (char_len == 1) {
        return true;
    } else {
        unsigned short gbk = 0;
        int ret = utf8_to_unicode((uint8_t*)ch, char_len, (uint8_t*)&gbk, 2);
        if (ret != 2)
            return false;

        if (gbk < 0x4E00 || gbk > 0x9FA5) {
            return false;
        }

        if (is_rare_unicode((unsigned char*)&gbk)) {
            return false;
        }

        return true;
    }
}

#define MAX_UTF8_SIZE  64
#define MIN_UTF8_SIZE   4

bool is_utf8_string(char* str, int* len)
{   
    *len = 0;
    for(int i= 0; i < MAX_UTF8_SIZE; i++) {
        int char_len = 0;
        if (!is_utf8(str, &char_len) 
            || !is_valid_utf8(str, char_len)) {
            if (i <= MIN_UTF8_SIZE || i == *len) {
                //字符数量过少
                //字符数量和字节数量一样， 说明是ASCII编码
                return false;
            } else {
                return true;
            }
        } else {
            str += char_len;
            *len += char_len;
        }
    }

    return true;
}

void add_string_list(int charset, uint32_t offset, int len, queue_t* strlist)
{
    item_t* mbs = (item_t*)malloc(sizeof(item_t));
    if (mbs == NULL) {
        return;
    }
    memset(mbs, 0, sizeof(item_t));

    mbs->data.codeset = charset;
    mbs->data.offset = (unsigned int)offset ;
    mbs->data.len = len;
    queue_put(strlist, mbs);   
}

bool dump_string2(char* buffer, int size, queue_t* strlist)
{
    int cnt_ascii = 0;
    int cnt_unicode = 0;
    int cnt_gb2312 = 0;
    int cnt_utf8 = 0;
    char* content = buffer;
    int last_charset = 0;
    char* head = NULL;
    int   len = 0;
    while (content < (buffer + size - 128)) {
        int current_charset = 0;
        int str_len = 0;
        if ((content - buffer) == 0x5D0) {
            printf("debug mode\n");
        }

        uint32_t offset = content - buffer;
        if (is_ascii_string(content, &str_len)) {
            add_string_list(CHARSET_ASCII, offset, str_len, strlist);
            cnt_ascii ++;
            content += str_len;
        } else if (is_unicode_string((unsigned short*)content, &str_len)) {
            add_string_list(CHARSET_UNICODE, offset, str_len, strlist);
            cnt_unicode ++;
            content += str_len;
        } else if (is_gb2312_string((unsigned short*)content, &str_len)) {
            add_string_list(CHARSET_GB2312, offset, str_len, strlist);
            cnt_gb2312 ++;
            content += str_len;
        } else if (is_utf8_string(content, &str_len)) {
            add_string_list(CHARSET_UTF8, offset, str_len, strlist);
            cnt_utf8 ++;
            content += str_len;
        } else {
            //没有检测到字符
            content ++;
        }

        // if (current_charset != 0 ){
        //     if (current_charset == last_charset) {
        //         //字符串延伸
        //         len += char_len;
        //     } else {
        //         if (last_charset == 0) {
        //             //当前没有字符串
        //           last_charset = current_charset;
        //           head = content;
        //           len = char_len;
        //         } else {
        //             if (len >= 4) {
        //                 //当前有字符串
        //                 printf("type: %d, offset:%08x, len:%d\n", 
        //                     last_charset, head - buf, len);
        //                 count++;
        //             }
        //             last_charset = current_charset;
        //             head = content;
        //             len = char_len;
        //         }
        //     }
        // } else {
        //     //没有检测到字符串
        //    if (last_charset == 0) {
        //         //当前没有字符串
        //     } else {
        //         //当前有字符串
        //         if (len >= 4) {
        //             printf("type: %d, offset:%08x, len:%d\n", 
        //                 last_charset, head - buf, len);
        //             count++;
        //         }
        //         last_charset = 0;
        //         head = NULL;
        //         len = 0;
        //     }
        // }

        // content += char_len;
    }
    printf("ASCII :%d\n", cnt_ascii);
    printf("UNICODE :%d\n", cnt_unicode);
    printf("GB2312 :%d\n", cnt_gb2312);
    printf("UTF8 :%d\n", cnt_utf8);
    printf("Count:%d\n", cnt_ascii + cnt_unicode + cnt_gb2312 + cnt_utf8);
    return true;
}