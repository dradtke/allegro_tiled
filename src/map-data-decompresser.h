#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <zlib.h>

#ifndef MAP_DATA_DECOMPRESSER_H
#define MAP_DATA_DECOMPRESSER_H

// Hack to make it work on Windows
// ???: Is this actually needed?
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 16384

int decompress(unsigned char *data, char *encoding);

#endif
