/***runtime_settings.h
* 
* Created: 2025-10-21
*/
#pragma once
#include <stdbool.h>

#define CONNECTION_CODE_LEN			(4)

#define POINTER_STYLE_RED			(0)
#define POINTER_STYLE_GREEN			(1)
#define POINTER_STYLE_BLUE			(2)
#define POINTER_STYLE_SPOTLIGHT		(3)

struct runtime_settings {
	int pointer_style; // UNUSED
	float pointer_speed; // UNUSED
	float mouse_speed;
	char connection_code[CONNECTION_CODE_LEN + 1];
};

typedef struct runtime_settings runtime_settings;
