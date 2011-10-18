#include "map-data-decompresser.h"

/*
 * Decompresses data from a map file
 * I keep getting a Z_DATA_ERROR from inflate() =/
 */
int decompress(unsigned char *data, char *encoding) {
	int ret;
	unsigned have;
	z_stream strm;
	unsigned char out[CHUNK];
	FILE *dest;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.next_in = Z_NULL;
	strm.next_out = Z_NULL;
	strm.avail_in = 0;
	strm.avail_out = 0;

	// initialize w/ support for zlib and gzip
	ret = inflateInit2(&strm, MAX_WBITS+32);
	if (ret != Z_OK) {
		printf("Failed to initialize\n");
		return ret;
	}

	dest = fopen("/home/damien/Desktop/data.txt", "w");

	do {
		strm.avail_in = strlen(data);
		strm.next_in = data;
		strm.avail_out = CHUNK;
		strm.next_out = out;

		ret = inflate(&strm, Z_NO_FLUSH);
		assert(ret != Z_STREAM_ERROR);

		switch (ret) {
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				fclose(dest);
				return ret;
		}

		printf("decompressed, saving to file\n");

		have = CHUNK - strm.avail_out;
		if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
			(void)inflateEnd(&strm);
			fclose(dest);
			return ret;
		}
	} while (strm.avail_out != 0);

	return ret;
}
