#include <string.h>
#include <ctype.h>

#include "listener.h"

int index_of(const char* str, char ch) {
	char* p = strchr(str, ch);
	if (p) {
		return (int)(p - str);
	}
	return -1;
}