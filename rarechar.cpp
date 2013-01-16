#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <iconv.h>
#include "typedef.h"
#include <hash_map.h>
#include "rarechar.h"
#include "raretbl.h"


#define INDEX_SIZE 	256

unsigned short* rare_unicode[INDEX_SIZE] = 
{
/*		00	01	02	03	04	05	06	07	08	09	0A	0B	0C	0D	0E	0F*/
/*00*/  0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0, 
/*10*/  0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
/*20*/  0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
/*30*/  0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
/*40*/  0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	_4E,_4F,
/*50*/  _50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_5A,_5B,_5C,_5D,_5E,_5F,
/*60*/  _60,_61,_62,_63,_64,_65,_66,_67,_68,_69,_6A,_6B,_6C,_6D,_6E,_6F,
/*70*/	_70,_71,_72,_73,_74,_75,_76,_77,_78,_79,_7A,_7B,_7C,_7D,_7E,_7F,
/*80*/  _80,_81,_82,_83,_84,_85,_86,_87,_88,_89,_8A,_8B,_8C,_8D,_8E,_8F,
/*90*/  _90,_91,_92,_93,_94,_95,_96,_97,_98,_99,_9A,_9B,_9C,_9D,_9E,_9F,
/*A0*/  0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
/*B0*/  0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
/*C0*/  0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
/*D0*/  0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
/*E0*/  0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
/*F0*/  0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
};

hash_map_t rare_gb2312;

hash_map_t rare_utf8;

#define UTF8_MAX_WIDTH	6
#define UTF8_CHS_WIDTH 	3


int unicode_to_gb2312(
	uint8_t* in_buf, size_t in_len, 
	uint8_t* out_buf, size_t out_len)
{
	memset(out_buf, 0, out_len);
	iconv_t fd = iconv_open("GBK","UCS-2-INTERNAL");
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


int unicode_to_utf8(
	uint8_t* in_buf, size_t in_len, 
	uint8_t* out_buf, size_t out_len)
{
	memset(out_buf, 0, out_len);
	iconv_t fd = iconv_open("UTF-8","UCS-2-INTERNAL");
	if (fd == (iconv_t)-1) {
		return -1;
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
	
bool is_rare_unicode(unsigned char* buf)
{
	unsigned short ch = *(unsigned short*)buf;
	int idx = buf[1];
	unsigned short* table = rare_unicode[idx];
	if (table == 0) {
		return false;
	}

	for (int i=0; table[i]!=0; i++) {
		if (ch == table[i])
			return true;
	}

	return false;
}

typedef struct _gb2312_node
{
	unsigned short code;
	hash_map_node_t 		node;
}gb2312_node_t;

bool init_rare_gb2312()
{
	hash_map_init(&rare_gb2312);

	int count = 0;
	int fail = 0;
	for (int i =0; i<INDEX_SIZE; i++) {
		unsigned short* table = rare_unicode[i];
		if (table == NULL) {
			continue;
		}

		for(int j =0; table[j] !=0; j++) {
			count ++;
			uint8_t buffer[8] = {0};
	    	int len = unicode_to_gb2312((uint8_t*)&table[j], 2, buffer, 8);
	    	if (len != 2) {
	    		fail ++;
	    		continue;
	    	}

			gb2312_node_t* gb2312 = (gb2312_node_t*)malloc(sizeof(gb2312_node_t));
			if (gb2312 == NULL) {
				return false;
			}
			memset(gb2312, 0, sizeof(gb2312_node_t));
			gb2312->code = *(unsigned short*)buffer;
			hash_map_insert(&rare_gb2312, gb2312->code, &gb2312->node);
		}
	}

	return true;
}

bool is_rare_gb2312(unsigned char* buf)
{
	unsigned short ch = *(unsigned short*)buf;
	hash_map_node_t* node = hash_map_find(&rare_gb2312, ch);
	return node != NULL;
}

typedef struct _utf8_node
{
	uint8_t code[6];
	hash_map_node_t 	node;
}utf8_node_t;

bool init_rare_utf8()
{
	hash_map_init(&rare_utf8);

	for (int i=0; i<INDEX_SIZE; i++) {
		unsigned short* table = rare_unicode[i];
		if (table == NULL) {
			continue;
		}

		for(int j =0; table[j] !=0; j++) {
			uint8_t buffer[UTF8_MAX_WIDTH] = {0};
	    	int len = unicode_to_utf8((uint8_t*)&table[j], 2, 
	    		buffer, UTF8_MAX_WIDTH);
	    	if (len != UTF8_CHS_WIDTH) {
	    		continue;
	    	}

			utf8_node_t* utf8 = (utf8_node_t*)malloc(sizeof(utf8_node_t));
			if (utf8 == NULL) {
				return false;
			}
			memset(utf8, 0, sizeof(utf8_node_t));
			memcpy(utf8->code, buffer, 6);
			hash_map_insert(&rare_utf8, *(uint32_t*)utf8->code, &utf8->node);
		}
	}	

	return true;
}

bool is_rare_utf8(unsigned char* buf, int char_len)
{
	if (char_len != UTF8_CHS_WIDTH) {
		return false;
	}

	uint32_t key = 0;
	memcpy(&key, buf, (char_len >= 4? 3: char_len));
	hash_map_node_t* node = hash_map_find(&rare_utf8, key);
	return node != NULL;
}

bool init_rare_table()
{
	// if (!init_rare_gb2312()) {
	// 	return false;
	// }

	// if (!init_rare_utf8()) {
	// 	return false;
	// }
    return true;
}
