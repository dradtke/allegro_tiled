/*
 * This addon adds Tiled map support to the Allegro game library.
 * Copyright (c) 2012 Damien Radtke - www.damienradtke.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3.0 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * For more information, visit http://www.gnu.org/copyleft
 */

#include <allegro5/tiled.h>
#include "internal.h"

static const char  table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const int   BASE64_INPUT_SIZE = 57;

int isbase64(char c)
{
	return c && strchr(table, c) != NULL;
}

static char value(char c)
{
	const char *p = strchr(table, c);
	if(p) {
		return p-table;
	} else {
		return 0;
	}
}

/*
 * Decode a block of base64-encoded data
 */
int UnBase64(unsigned char *dest, const unsigned char *src, int srclen)
{
	*dest = 0;
	if(*src == 0) 
	{
		return 0;
	}
	unsigned char *p = dest;
	do
	{

		char a = value(src[0]);
		char b = value(src[1]);
		char c = value(src[2]);
		char d = value(src[3]);
		*p++ = (a << 2) | (b >> 4);
		*p++ = (b << 4) | (c >> 2);
		*p++ = (c << 6) | d;
		if(!isbase64(src[1])) 
		{
			p -= 2;
			break;
		} 
		else if(!isbase64(src[2])) 
		{
			p -= 2;
			break;
		} 
		else if(!isbase64(src[3])) 
		{
			p--;
			break;
		}
		src += 4;
		while(*src && (*src == 13 || *src == 10)) src++;
	}
	while(srclen-= 4);
	*p = 0;
	return p-dest;
}

/*
 * Decompresses data from a map file
 * Taken from zlib's zpipe.c example, with slight modifications
 * TODO: make sure this works with both zlib and gzip, using the compression parameter if necessary
 */
int decompress(char *src, FILE *dest, char *compression)
{
    int ret;
    unsigned have, len;
    z_stream strm;
    unsigned char in[CHUNK];
	unsigned char debased[CHUNK];
    unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, 32+MAX_WBITS);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
		len = strlen(src);
		// if nothing's there, break out of the loop
        if (len == 0)
            break;

		// read at most CHUNK bytes
		if (len > CHUNK)
			len = CHUNK;

		strncpy((char *)in, src, len);
		strm.avail_in = UnBase64(debased, in, len);
        strm.next_in = debased;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}
