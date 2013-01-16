#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <iconv.h>
#include "typedef.h"
#include "strdump.h"

void usage()
{
	printf("strtool <sample_file>\n");
}

char  buffer[2048] = {0};

char* utf8_to_gbk(uint8_t* in_buf, size_t in_len)
{
	memset(buffer, 0, sizeof(buffer));
	iconv_t fd = iconv_open("GBK","UTF-8");
	if (fd == (iconv_t)-1) {
		printf("iconv_open failed, %d", errno);
		return NULL;
	}
	char* in = (char*)in_buf;
	char* out = (char*)buffer;
	size_t out_len = sizeof(buffer) - 1;
	if (-1 == iconv(fd, &in, &in_len, &out, &out_len)) {
		printf("utf8_to_gbk failed, %d", errno);
		iconv_close(fd);
		return NULL;
	}

	iconv_close(fd);
	return buffer;
}

char* unicode_to_gbk(uint8_t* in_buf, size_t in_len)
{
	memset(buffer, 0, sizeof(buffer));
	iconv_t fd = iconv_open("GBK","UCS-2-INTERNAL");
	if (fd == (iconv_t)-1) {
		return NULL;
	}
	char* in = (char*)in_buf;
	char* out = (char*)buffer;
	size_t out_len = sizeof(buffer) - 1;
	if (-1 == iconv(fd, &in, &in_len, &out, &out_len)) {
		iconv_close(fd);
		return NULL;
	}

	iconv_close(fd);
	return buffer;	
}

char* gb2312_to_gbk(uint8_t* in_buf, size_t in_len)
{
	memset(buffer, 0, sizeof(buffer));
	iconv_t fd = iconv_open("GBK","GB2312");
	if (fd == (iconv_t)-1) {
		return NULL;
	}
	char* in = (char*)in_buf;
	char* out = (char*)buffer;
	size_t out_len = sizeof(buffer) - 1;
	if (-1 == iconv(fd, &in, &in_len, &out, &out_len)) {
		iconv_close(fd);
		return NULL;
	}

	iconv_close(fd);
	return buffer;		
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		usage();
		exit(-1);
	}
	char* sample_file = argv[1];
	FILE* fp = fopen(sample_file, "rb");
	if (fp == NULL) {
		printf("open file failed\n");
		exit(-1);
	}
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* buffer = (char*)malloc(size);
	memset(buffer, 0, size);
	fread(buffer, 1, size, fp);
	int fd = open_dump_buffer((unsigned char*)buffer, size);
	if (fd == INVALID_DUMP_BUFFER) {
		printf("open_dump_buffer failed");
		exit(-1);
	}

	string_t* str = dump_string_first(fd);
	for(; str != NULL; str = dump_string_next(fd)) {
		int offset = int(str->start - (unsigned char*)buffer);
		int len = str->len;
		if (str->codeset == CHARSET_UTF8) {
			//UTF转GB2312
			char* gbk = utf8_to_gbk(str->start, str->len);
			if (gbk == NULL) {
				printf("UTF8: 0x%08X %d ########\n", offset, len);
			} else {
				printf("UTF8: 0x%08X %d %s\n", offset, len, gbk);	
			}
		} else if(str->codeset == CHARSET_ASCII) {
			printf("ASCII: 0x%08X %d %.*s\n", offset, len,  str->len, str->start );
		} else if(str->codeset == CHARSET_UNICODE) {
			char* gbk = unicode_to_gbk(str->start, str->len);
			if (gbk == NULL) {
				printf("UNICODE: 0x%08X %d ########\n", offset, len);
			} else {
				printf("UNICODE: 0x%08X %d %s\n", offset, len, gbk);
			}
		} else if(str->codeset == CHARSET_GB2312) {
			char* gbk = gb2312_to_gbk(str->start, str->len);
			if (gbk == NULL) {
				printf("GB2312: 0x%08X %d ########\n", offset, len);
			} else {
				printf("GB2312: 0x%08X %d %s\n", offset, len, gbk);
			}
		} else {
			printf("UNKNOWN CHARSET\n");
			continue;
		}
	}

	close_dump_buffer(fd);

	fclose(fp);
}