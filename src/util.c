#include <allegro5/tiled.h>
#include "internal.h"

/*
 * Trims whitespace from before and after a string
 */
char *trim(char *str)
{
	char *end;

	while (isspace(*str)) str++;
	if (*str == 0) return str;

	end = str + strlen(str) - 1;
	while (end > str && isspace(*end)) end--;

	*(end+1) = '\0';
	return str;
}

/*
 * Returns a copy of a string
 * Used to keep XML data that would otherwise be freed
 */
char *copy(const char *src)
{
	int len = strlen(src);
	char *result = (char *)malloc(sizeof(char) * (len+1));
	strcpy(result, src);
	return result;
}

