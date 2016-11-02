#ifndef FISHOS_SOUND_H
#define FISHOS_SOUND_H

#include "types.h"

// 设置蜂鸣
void beep(int frequency, int milliseconds);
// 发声
void playNote(char *note, int milliseconds);

// song
void song();

#endif
