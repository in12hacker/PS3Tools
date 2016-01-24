#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>


struct TEXHDR
{
	char signature[8];
	unsigned long chunk_length;
	unsigned char unknown1[4];
	unsigned long image_length;
	unsigned long image_width;
	unsigned long image_height;
};

unsigned long se32(unsigned long i) {
	return ((i & 0xFF000000) >> 24) | ((i & 0xFF0000) >> 8) | ((i & 0xFF00) << 8) | ((i & 0xFF) << 24);
}



int main(int argc, char*argv[]) {
	FILE *fp_in;
	FILE *fp_out;
	TEXHDR hdr;
	unsigned char *data;
	char filename[256];

	if (argc < 2) {
		printf("tex2png.exe <input>\n");
		return 0;
	}


	fp_in = fopen(argv[1], "rb");
	if (!fp_in) {
		printf("file not found\n");
		return 0;
	}

	fread(&hdr, sizeof(hdr), 1, fp_in);

	if (strncmp(hdr.signature, "Texture", 7)) {
		printf("invalid format\n");
		return 1;
	}

	hdr.image_length = se32(hdr.image_length);
	hdr.image_width = se32(hdr.image_width);
	hdr.image_height = se32(hdr.image_height);

	strcpy(filename, argv[1]);

	if (strrchr(filename, '.') == NULL) {
		printf("invalid file name\n");
		return 0;
	}

	
	*strrchr(filename, '.') = 0;
	strcat(filename, ".png");

	fp_out = fopen(filename, "wb");
	data = new unsigned char[hdr.image_length];

	fread(data, hdr.image_length, 1, fp_in);
	fwrite(data, hdr.image_length, 1, fp_out);
	fclose(fp_out);



	


	return 0;
}