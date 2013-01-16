#pragma warning( disable:4996 )
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include <slist.h>
#include "strdump.h"
#include "item.h"
#include "strdump1.h"
#define MAX_STR_LEN         512
#define MIN_STR_LEN         4

inline long is_mbs_end( uint8_t* pc )
{
    if( 0 == *pc || 0xA == *pc || 0xD == *pc )
        return true;
    return false;
};

inline long is_mbs_asc( uint8_t* pc ) 
{
    if( 0x20 <= *pc && 0x7F > *pc ) return true; 
    if( '\t' == *pc ) return true;
    return false;
};

inline long is_mbs_chs_s(uint8_t* pchs) 
{
#ifndef __CALCER_USE__
    if( *pchs > 0xA0 && *pchs < 0xF7 && *(pchs+1) > 0xA0 && *(pchs+1) < 0xFF )
        return true;
    return false;
#else
    return false;
#endif
};

inline long is_mbs_chs_r(uint8_t* pche, uint8_t* prngs ) 
{
#ifndef __CALCER_USE__
    if( pche < prngs+1 ) return false;
    if( *(pche) > 0xA0 && *(pche) < 0xFF && *(pche-1) > 0xA0 && *(pche-1) < 0xF7 )
        return true;
    return false;
#else
    return false;
#endif
};

inline long is_uni_end(wchar_t* pc) 
{
    if( 0 == *pc || 0xA == *pc || 0xD == *pc )
        return true;
    return false;
};

inline long is_uni_asc(wchar_t* pc) 
{
    if( 0x20 <= *pc && 0x7F > *pc ) return true;
    if( '\t' == *pc ) return true;
    return false;
};

inline long is_uni_chs( wchar_t* pc ) 
{
    if( *pc > 0x4e00 && *pc < 0x9fbf )
        return true;
    return false;
};

// 返回可能的串结束点
uint8_t* find_mbs_range_end(uint8_t* start, uint8_t* end)
{
    uint8_t* ep = start;
    while( ep < end && !is_mbs_end(ep)) 
        ep++;
    // 串必须有结束符号
    return ep;
}

// 返回去空格和制表符的串结束点
uint8_t*  trim_mbs_end( uint8_t* str_ep, uint8_t* rng_sp )
{
    while( str_ep > rng_sp ) {
        uint8_t* ep = str_ep-1;
        if( 0x20 == *ep || '\t' == *ep ) 
            str_ep --;
        else
            return str_ep;
    }
    return NULL;
}

// 返回去空格和制表符的串结束点
wchar_t* trim_unis_end(wchar_t* str_ep, wchar_t* end)
{
    while( str_ep > end ) {
        wchar_t* ep = str_ep - 1;
        if( 0x20 == *ep || '\t' == *ep ) 
            str_ep --;
        else
            return str_ep;
    }
    return NULL;
}


// 返回可能的串结束点
wchar_t* find_unis_range_end(wchar_t* start, uint8_t* end)
{
    wchar_t* ep = start;
    while(((uint8_t*)ep+1) < end && !is_uni_end(ep)) 
        ep++;
    // 串必须有结束符号
    return (wchar_t*)ep;
}

// 从串结束点向前扫描找到一个有效的串开头
wchar_t* find_unis(wchar_t* str_ep, wchar_t* rng_sp)
{
    int cc = 0;
    int  mxcc = (int)(str_ep - rng_sp);
    if( mxcc > MAX_STR_LEN ) 
        mxcc = MAX_STR_LEN;
    wchar_t* sp = str_ep - 1;
    do {
        if( is_uni_asc(sp) ) {
            cc ++;
            sp --;
        } else if( is_uni_chs( sp ) ) {
            cc ++;
            sp --;
        } else {
            break;
        }
    }while( cc < mxcc );
    if( cc < MIN_STR_LEN ) 
        return NULL;
    return sp + 1;  
}

// 返回去空格和制表符的串开头
wchar_t* trim_unis_start( wchar_t* str_sp, int cc )
{
    while( cc && ( 0x20 == *str_sp || '\t' == *str_sp ) ) {
        cc --;
        str_sp ++;
    }
    if( cc < MIN_STR_LEN ) return NULL;
    return str_sp;    
}

bool dump_unis(uint8_t* rbof, uint8_t* reof, slist_t* list)
{
    wchar_t* fp = (wchar_t*)rbof;
    wchar_t* ep = NULL;
    while((uint8_t*)fp < reof) {
        //先找到宽字符字符串的末尾
        wchar_t* ep = find_unis_range_end(fp, reof);
        if (!ep) 
            //未找到末尾
            break;

        //删除字符串末尾的空格
        wchar_t* tep = trim_unis_end(ep, fp);
        if( NULL == tep ) {
            fp = ep + 1;
            continue;
        }

        //找到宽字符串的首部
        wchar_t* sp = find_unis( tep, fp );
        if( NULL == sp ) {
            fp = ep + 1;
            continue;
        }
        int cc = tep - sp; 

        //删除宽字符串开头的空格
        wchar_t* tsp = trim_unis_start(sp, cc);
        if (tsp == NULL) {
            fp = ep + 1;
            continue;
        }

        cc = tep - tsp;
        item_t* wcs = (item_t*)malloc(sizeof(item_t));
        if (wcs == NULL) 
            return false;
        memset(wcs, 0, sizeof(item_t));
        wcs->data.codeset = CHARSET_UTF8;
        wcs->data.start = (unsigned char*)tsp;
        wcs->data.len = cc;
        slist_add(list, &wcs->node);
        // printf("add string\n");
        fp = ep + 1;
        /*
        bool tst_unis = true;
        ulong_t wp = 0, tmp_mb_cc = 0;
        for( ; wp <= cc; wp ++ ) {
            int skip = wctomb( m_tmp_mbs + tmp_mb_cc, sp[wp] );
            if( skip < 0 ) { 
                tst_unis = false;
                break; 
            }
            tmp_mb_cc += skip;
        }

        if( tst_unis ) {
            ulong_t raw_ofs = ofsbase + ((uchar_t*)sp - rbof);
            if( !internal_on_discover_string( raw_ofs, (uchar_t*)m_tmp_mbs, tmp_mb_cc ) )
                return false;
        }
        */
    }
    return true;
}


// 从串结束点向前扫描找到一个有效的串开头
uint8_t*  find_mbs(uint8_t* str_end, uint8_t* range_start)
{
    int cc = 0;
    //uchar_t* rngs = rng_sp;
    //计算出搜索范围
    int  mxcc = (int)(str_end - range_start);
    if( mxcc > MAX_STR_LEN ) 
        //如果搜索范围超出了字符串的最大长度
        //则将搜索范围限制在最大长度
        mxcc = MAX_STR_LEN;
    uint8_t* sp = str_end - 1;
    do {
        if(is_mbs_asc(sp)) {
            //如果是可识别字符， 继续向前搜索
            cc ++;
            sp --;
        } else if(is_mbs_chs_r(sp, str_end-mxcc)) {
            //如果是可识别中文汉字， 继续向前搜索
            cc += 2;
            sp -= 2;
        } else {
            //如果都不能识别， 则中断退出
            break;
        }
    }while( cc < mxcc );

    //如果实际找到的字符串长度小于最小长度， 则该字符串无效
    if( cc < MIN_STR_LEN ) 
        return NULL;

    return sp + 1;
}

// 返回去空格和制表符的串开头
uint8_t*  trim_mbs_start( uint8_t* str_sp, int cc )
{
    while( cc && ( 0x20 == *str_sp || '\t' == *str_sp ) ) {
        cc --;
        str_sp ++;
    }
    if( cc < MIN_STR_LEN ) return NULL;
    return str_sp;
}

bool dump_mbs(uint8_t* rbof, uint8_t* reof, slist_t* list)
{
    uint8_t* fp = rbof;
    while( fp < reof ) {
        //查找字符串结束字符
        uint8_t* ep = find_mbs_range_end(fp, reof); 
        if( !ep ) 
            //找不到结束字符了， 就直接退出
            return true;
    
        //找到结束字符
        //删除空格
        uint8_t* tep = trim_mbs_end(ep, fp);
        if( tep ) {
            int cc = 0;
            //找到字符串开始位置
            uint8_t* sp = find_mbs( tep, fp);
            int len = tep - sp;
            if( sp ) {
                //找到字符串开始位置， 还需要删除空格字符
                uint8_t* tsp = trim_mbs_start(sp, len);
                len = tep - tsp;
                if (len >= MIN_STR_LEN && len <= MAX_STR_LEN) {
                    item_t* mbs = (item_t*)malloc(sizeof(item_t));
                    if (mbs == NULL) {
                        return false;
                    }
                    memset(mbs, 0, sizeof(item_t));
                    mbs->data.codeset = CHARSET_ASCII;
                    mbs->data.start = tsp;
                    mbs->data.len = len;
                    slist_add(list, &mbs->node);
                }

                // printf("add string\n");
                /*
                if( sp ) {
                    ulong_t raw_ofs = ofsbase + (sp - rbof);
                    if( !internal_on_discover_string( raw_ofs, sp, cc ) )
                        return false;
                }
                */
            }
        }
        fp = ep + 1;
    }
    return true;
}

bool dump_string1(unsigned char* buffer, int size, slist_t* strlist)
{
	uint8_t* end = buffer + size;
	bool r = dump_mbs(buffer, end, strlist);
    if(r) 
        r = dump_unis(buffer, end, strlist);
	if(r) 
        r = dump_unis(buffer+1, end-1, strlist);

   	snode_t* str = NULL;
   	int count = 0;
	for(str = strlist->first; str != NULL; str = str->next) {
		count ++;
	}

	printf("Count :%d\n", count);
    return true;
}