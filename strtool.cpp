#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "strdump.h"

void usage()
{
	printf("strtool <sample_file>\n");
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
	for(; str != NULL; str = dump_string_next(str)) {
		if (str->codeset == CODESET_UTF8) {
			printf("UTF8:%*ls\n", str->len, str->start);
		} else if(str->codeset = CODESET_ASCII) {
			printf("ASCII:%*s\n", str->len, str->start);
		} else {
			continue;
		}
	}

	close_dump_buffer(fd);

	fclose(fp);
}