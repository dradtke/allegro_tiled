#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "global.h"

#ifndef UTIL_H
#define UTIL_H

char *trim(char *str);
char *copy(const char *str);

void debug(const char *format, ...);

#endif
