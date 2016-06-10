/* input.h
 *
 * Copyright (C) 2007 Pascal Giard
 * Copyright (C) 2016 Felipe Izzo
 *
 * Author: Pascal Giard <evilynux@gmail.com>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "MMU.h"
#include "types.h"

#define ADD_KEY(keypad,key) ( (keypad) |= (key) )
#define RM_KEY(keypad,key) ( (keypad) &= ~(key) )
#define KEYMASK_(k)	(1 << (k))
#define JOY_AXIS_(k)    (((k)+1) << 8)

#define NB_KEYS		14
#define DSK_NONE		0
#define DSK_A			1
#define DSK_B			2
#define DSK_SELECT		3
#define DSK_START		4
#define DSK_RIGHT		5
#define DSK_LEFT		6
#define DSK_UP			7
#define DSK_DOWN		8
#define DSK_R			9
#define DSK_L			10
#define DSK_X			11
#define DSK_Y			12
#define DSK_DEBUG		13
#define DSK_BOOST		14

int process_ctrls_events( u16 *keypad );

#endif /* INPUT_H */