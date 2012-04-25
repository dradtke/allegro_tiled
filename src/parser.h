#include <string.h>
#include "global.h"
#include "map.h"
#include "util.h"
#include "xml.h"
#include "zlib.h"

#ifndef MAP_PARSER_H
#define MAP_PARSER_H

map_data *parse_map(const char *filename);

#endif
