#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <vector>

std::vector<long> offsets;
std::vector<long> opcodes;

long get_offsets(unsigned char *data) {
	long i = 0;
	if (data[3] == 0) {
		i = data[4] << 8 | data[5];
	}

	if (data[3] > 0 && data[3] < 10) {
		i = data[3] << 16 | data[4] << 8 | data[5];
	}

	return i;
}
void find_load_str_opcode(unsigned char *data, long size)
{
	long i;
	data += 0x30;
	size -= 0x30;
	size -= 0x10;


	for (i = 0; i < size; i++) {
		if (data[i] == 0x00 && data[i + 1] == 0x50 && data[i + 2] == 0x00) {

			if (data[3] == 0 && data[4] == 0) {
				i += 6; continue;
			}
			long f = get_offsets(&data[i]);
			opcodes.push_back(i);
			offsets.push_back(f);
			i += 6;
		}
	}
}

int main(int argc, char*argv[]) {
	FILE *fp_in;
	FILE *fp_out;
	long f_inlen;
	char filename[256];
	unsigned char *data;

	if (argc < 2) {
		printf("bin2text.exe <input>\n");
		return 0;
	}

	fp_in = fopen("11_01_100C.BIN", "rb");
	fseek(fp_in, 0, SEEK_END);
	f_inlen = ftell(fp_in);
	fseek(fp_in, 0, SEEK_SET);

	if (f_inlen <= 0x2d) {
		printf("invalid file\n");
		return 0;
	}

	data = (unsigned char*)malloc(f_inlen);
	fread(data, f_inlen, 1, fp_in);

	if (strncmp((char*)data, "CSTS", 4) != 0) {
		printf("invalid file\n");
		return 0;
	}

	strcpy(filename, argv[1]);

	if (strrchr(filename, '.') == NULL) {
		printf("invalid file name\n");
		return 0;
	}

	*strrchr(filename, '.') = 0;
	strcat(filename, ".txt");

	find_load_str_opcode(data, f_inlen);
	if (offsets.size() == 0) {
		return 0;
	}
	fp_out = fopen(filename, "wb");

	for (int i = 0; i < offsets.size(); i++) {
		if (offsets[i] >= f_inlen) {
			continue;
		}

		if (!(data[offsets[i]] & 0xE0)) {
			continue;
		}

		fprintf(fp_out, "<%08X>\r\n//", opcodes[i]);
		fprintf(fp_out, "%s", &data[offsets[i]]);
		fprintf(fp_out, "\r\n");
		fprintf(fp_out, "%s", &data[offsets[i]]);
		fprintf(fp_out, "\r\n\r\n");
	}

	fclose(fp_out);


	return 0;
}