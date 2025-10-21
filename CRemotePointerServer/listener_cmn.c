#include <string.h>
#include <ctype.h>

#include "listener.h"

char* str_trim(char* str)
{
	char* end;
	// Trim leading space
	while (isspace((unsigned char)*str)) str++;
	if (*str == 0)  // All spaces?
		return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end)) end--;

	// Write new null terminator
	*(end + 1) = '\0';
	return str;
}

int index_of(const char* str, char ch) {
	char* p = strchr(str, ch);
	if (p) {
		return (int)(p - str);
	}
	return -1;
}