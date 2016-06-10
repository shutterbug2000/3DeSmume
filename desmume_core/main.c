/* main.c - this file is part of DeSmuME
*
* Copyright (C) 2006,2007 DeSmuME Team
* Copyright (C) 2007 Pascal Giard (evilynux)
* Copyright (C) 2009 Yoshihiro (DsonPSP)
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
#include <stdio.h>
#include "MMU.h"
#include "NDSSystem.h"
#include "cflash.h"
#include "debug.h"
//#include "sndsdl.h"
//#include "ctrlssdl.h"
#include "render3D.h"
#include "FrontEnd.h"
#include <3ds.h>

volatile BOOL execute = FALSE;

#define NUM_FRAMES_TO_TIME 15

#define FPS_LIMITER_FRAME_PERIOD 8

#define BGR5A1_RGB5A1(src)  (((src & 0xF800) >> 10) | ((src & 0x3E) << 10) | ((src & 0x7C) >> 5) (src & 0x1))

unsigned int ABGR1555toRGBA8(unsigned short c)
{
    const unsigned int a = c&0x8000, b = c&0x7C00, g = c&0x03E0, r = c&0x1F;
    const unsigned int rgb = (r << 27) | (g << 14) | (b << 1);
    return ((a * 0xFF) >> 15) | rgb | ((rgb >> 5) & 0x07070700);
}

static u16 keypad;

u32 fps_timing = 0;
u32 fps_frame_counter = 0;
u32 fps_previous_time = 0;
u32 fps_temp_time;
u32 opengl_2d = 0;

/* SoundInterface_struct *SNDCoreList[] = {
&SNDDummy,
&SNDFile,
&SNDSDL,
NULL
}; */

GPU3DInterface *core3DList[] = {
	&gpu3DNull
};

int savetype=MC_TYPE_AUTODETECT;
u32 savesize=1;


const char * save_type_names[] = {
	"Autodetect",
	"EEPROM 4kbit",
	"EEPROM 64kbit",
	"EEPROM 512kbit",
	"FRAM 256kbit",
	"FLASH 2mbit",
	"FLASH 4mbit",
	NULL
};

int cycles;

touchPosition touch;

void DSonpspExec()
{  

	process_ctrls_events(&keypad);

	update_keypad(keypad);     /* Update keypad */

	if(hidKeysHeld() & KEY_TOUCH){
		hidTouchRead(&touch);
		NDS_setTouchPos(touch.px,touch.py);
	}

	else if(hidKeysUp() & KEY_TOUCH){
		NDS_releasTouch();
	}

	NDS_exec(FALSE);

	if ( enable_sound) {
		SPU_Emulate();
	}

}


int main(int argc, char **argv)
{
	struct armcpu_memory_iface *arm9_memio = &arm9_base_memory_iface;
	struct armcpu_memory_iface *arm7_memio = &arm7_base_memory_iface;
	struct armcpu_ctrl_iface *arm9_ctrl_iface;
	struct armcpu_ctrl_iface *arm7_ctrl_iface;
	char rom_filename[256];

	osSetSpeedupEnable(true);

	gfxSetDoubleBuffering(GFX_TOP, false);
	gfxSetDoubleBuffering(GFX_BOTTOM, false);
	gfxSet3D(false);

	gfxInit(GSP_RGBA8_OES,GSP_RGBA8_OES,false);

	gfxSwapBuffersGpu();
	gspWaitForVBlank();

	cflash_disk_image_file = NULL;


/* the firmware settings */
	struct NDS_fw_config_data fw_config;

	NDS_FillDefaultFirmwareConfigData(&fw_config); 

	NDS_Init( arm9_memio, &arm9_ctrl_iface,
		arm7_memio, &arm7_ctrl_iface);


/* Create the dummy firmware */
	NDS_CreateDummyFirmware( &fw_config);



	mmu_select_savetype(savetype, &savetype, &savesize);

	if (NDS_LoadROM( "/game.nds"  , MC_TYPE_AUTODETECT, 1, cflash_disk_image_file) < 0) {
		printf("Error loading game.nds\n");
	}

	execute = TRUE;


	DSonpspExec();

	u16 width;
	u16 height;

	u32 *tfb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
	u32 *bfb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);


	while(aptMainLoop()) {

		DSonpspExec();

		u16 * src = (u16*)GPU_screen;
		int x,y;

		for(x=0; x<256; x++){
    		for(y=0; y<192;y++){
        		tfb[((x*240) + (239 -y))] = ABGR1555toRGBA8(src[( y * 256 ) + x]);
        		bfb[((x*240) + (239 -y))] = ABGR1555toRGBA8(src[( (y + 192) * 256 ) + x]);
    		}
		}

		if ( enable_sound) {
			SPU_Emulate();
		}


    }

	return 0;
}