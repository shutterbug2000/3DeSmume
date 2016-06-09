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
/* #include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspvfpu.h> */
#include <stdio.h>
#include "MMU.h"
#include "NDSSystem.h"
#include "cflash.h"
#include "debug.h"
//#include "sndsdl.h"
//#include "ctrlssdl.h"
#include "render3D.h"
#include "gdbstub.h"
#include "FrontEnd.h"
#include <3ds.h>
#include <sf2d.h>
#include <3ds/types.h>

#define WIDTH 400
#define HEIGHT 240
#define BPP 32
#define DEPTH 32

volatile BOOL execute = FALSE;

static float nds_screen_size_ratio = 1.0f;

#define NUM_FRAMES_TO_TIME 15

#define FPS_LIMITER_FRAME_PERIOD 8

//static SDL_Surface *surface;

/* Flags to pass to SDL_SetVideoMode */
static int sdl_videoFlags = 0;
static int sdl_quit = 0;
static u16 keypad;
  
u8 *GPU_vram[512*192*4];
u8 *GPU_mergeA[256*192*4];
u8 *GPU_mergeB[256*192*4];

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

void DSonpspExec()
{  


	NDS_exec(FALSE);

	if ( enable_sound) {
		SPU_Emulate();
	}

}


int main(int argc, char **argv)
{
  int f;
  struct armcpu_memory_iface *arm9_memio = &arm9_base_memory_iface;
  struct armcpu_memory_iface *arm7_memio = &arm7_base_memory_iface;
  struct armcpu_ctrl_iface *arm9_ctrl_iface;
  struct armcpu_ctrl_iface *arm7_ctrl_iface;
  char rom_filename[256];
  
	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);
  
      u32 *pixmem32;
    u32 colour;  
	
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
  
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));
  
	sf2d_texture *texture = sf2d_create_texture(256, 192, TEXFMT_RGB5A1, SF2D_PLACE_RAM);
  
  while(aptMainLoop()) {
  	
	DSonpspExec();
	
	u16 *src, *dst,*dstA,*dstB;
     src = (u16*)GPU_screen;
	 dstA = (u16*)GPU_mergeA;
	 dstB = (u16*)GPU_mergeB;
     dst = (u16*)GPU_vram;
	 int x,y,i;
   
	for(i=0; i < 256*192*4; i++)
	 { 
	 dstA[i] = src[i]; //Screen buffer to top screen buffer
	 dstB[i] = src[(256*192*4)+i]; //Screen buffer to bottom screen buffer
	 }
	
	void* texturedata = texture->data;
	u16* texturedata16 = (u16*)texturedata;
 	for(x=0; x < (256*192*4); x++){
		texturedata16[x] = (((dstA[x] << 10) | (dstA[x+1] << 5) | 0x1));
		
	}
	sf2d_texture_tile32(texture);
	
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
			sf2d_draw_texture_rotate(texture, 400/2, 240/2, 0);
		sf2d_end_frame();

	
	sf2d_swapbuffers();

	if ( enable_sound) {
		SPU_Emulate();
	}

}

  return 0;
}