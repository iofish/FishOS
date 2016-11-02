// 2016年04月28日15:19:28
#ifndef FISHOS_KEYBOARD_H_
#define FISHOS_KEYBOARD_H_

#include "common.h"

void init_keyboard();

char get_char_from_keyboard();

int isSpecialKey(uint8_t keyPressChar);

#endif //FISHOS_KEYBOARD_H_
