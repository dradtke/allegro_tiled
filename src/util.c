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
 *
 *                               ---
 *
 * Miscellaneous utility methods.
 */

#include "util.h"

/*
 * Trims whitespace from before and after a string.
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

