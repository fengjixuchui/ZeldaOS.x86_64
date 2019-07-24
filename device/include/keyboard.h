/*
 * Copyright (c) 2019 Jie Zheng
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <lib64/include/type.h>

void
keyboard_init(void);

uint8_t
try_retrieve_scancode(uint8_t * scancode);

#endif

