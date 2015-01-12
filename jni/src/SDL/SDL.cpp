// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2004 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "AutoBuild.h"

#include "SDL.h"
#include "SDL_Image.h"
#include "GBA.h"
#include "agbprint.h"
#include "Flash.h"
#include "Port.h"
#include "debugger.h"
#include "RTC.h"
#include "Sound.h"
#include "Text.h"
#include "unzip.h"
#include "Util.h"
#include "gb/GB.h"
#include "gb/gbGlobals.h"
#include <jni.h>
#include <cheats.h>
#ifndef WIN32
# include <unistd.h>
# define GETCWD getcwd
#else // WIN32
# include <direct.h>
# define GETCWD _getcwd
#endif // WIN32

#ifndef __GNUC__
# define HAVE_DECL_GETOPT 0
# define __STDC__ 1
# include "getopt.h"
#else // ! __GNUC__
# define HAVE_DECL_GETOPT 1
# include "getopt.h"
#endif // ! __GNUC__

#include <string>

#ifdef MMX
extern "C" bool cpu_mmx;
#endif
extern bool soundEcho;
extern bool soundLowPass;
extern bool soundReverse;
extern int Init_2xSaI(u32);
extern void _2xSaI(u8*,u32,u8*,u8*,u32,int,int);
extern void _2xSaI32(u8*,u32,u8*,u8*,u32,int,int);  
extern void Super2xSaI(u8*,u32,u8*,u8*,u32,int,int);
extern void Super2xSaI32(u8*,u32,u8*,u8*,u32,int,int);
extern void SuperEagle(u8*,u32,u8*,u8*,u32,int,int);
extern void SuperEagle32(u8*,u32,u8*,u8*,u32,int,int);  
extern void Pixelate(u8*,u32,u8*,u8*,u32,int,int);
extern void Pixelate32(u8*,u32,u8*,u8*,u32,int,int);
extern void MotionBlur(u8*,u32,u8*,u8*,u32,int,int);
extern void MotionBlur32(u8*,u32,u8*,u8*,u32,int,int);
extern void AdMame2x(u8*,u32,u8*,u8*,u32,int,int);
extern void AdMame2x32(u8*,u32,u8*,u8*,u32,int,int);
extern void Simple2x(u8*,u32,u8*,u8*,u32,int,int);
extern void Simple2x32(u8*,u32,u8*,u8*,u32,int,int);
extern void Bilinear(u8*,u32,u8*,u8*,u32,int,int);
extern void Bilinear32(u8*,u32,u8*,u8*,u32,int,int);
extern void BilinearPlus(u8*,u32,u8*,u8*,u32,int,int);
extern void BilinearPlus32(u8*,u32,u8*,u8*,u32,int,int);
extern void Scanlines(u8*,u32,u8*,u8*,u32,int,int);
extern void Scanlines32(u8*,u32,u8*,u8*,u32,int,int);
extern void ScanlinesTV(u8*,u32,u8*,u8*,u32,int,int);
extern void ScanlinesTV32(u8*,u32,u8*,u8*,u32,int,int);
extern void hq2x(u8*,u32,u8*,u8*,u32,int,int);
extern void hq2x32(u8*,u32,u8*,u8*,u32,int,int);
extern void lq2x(u8*,u32,u8*,u8*,u32,int,int);
extern void lq2x32(u8*,u32,u8*,u8*,u32,int,int);

extern void SmartIB(u8*,u32,int,int);
extern void SmartIB32(u8*,u32,int,int);
extern void MotionBlurIB(u8*,u32,int,int);
extern void MotionBlurIB32(u8*,u32,int,int);

void Init_Overlay(SDL_Surface *surface, int overlaytype);
void Quit_Overlay(void);
//void Draw_Overlay(SDL_Surface *surface, int size);

extern void remoteInit();
extern void remoteCleanUp();
extern void remoteStubMain();
extern void remoteStubSignal(int,int);
extern void remoteOutput(char *, u32);
extern void remoteSetProtocol(int);
extern void remoteSetPort(int);
extern void debuggerOutput(char *, u32);

extern void CPUUpdateRenderBuffers(bool);

struct EmulatedSystem emulator = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  false,
  0
};

SDL_Window *window = NULL;
SDL_Surface *surface = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_Texture* directionTexture = NULL;
SDL_Texture* upTexture = NULL;
SDL_Texture* downTexture = NULL;
SDL_Texture* leftTexture = NULL;
SDL_Texture* rightTexture = NULL;
SDL_Texture* leftupTexture = NULL;
SDL_Texture* rightupTexture = NULL;
SDL_Texture* leftdownTexture = NULL;
SDL_Texture* rightdownTexture = NULL;
SDL_Texture* abTexture = NULL;
SDL_Texture* ab2Texture = NULL;
SDL_Texture* aTexture = NULL;
SDL_Texture* bTexture = NULL;
SDL_Texture* startTexture = NULL;
SDL_Texture* start2Texture = NULL;
SDL_Texture* selectTexture = NULL;
SDL_Texture* select2Texture = NULL;
SDL_Texture* left2Texture = NULL;
SDL_Texture* left2_2Texture = NULL;
SDL_Texture* right2Texture = NULL;
SDL_Texture* right2_2Texture = NULL;
SDL_Texture* saveTexture = NULL;
SDL_Texture* save2Texture = NULL;
SDL_Texture* loadTexture = NULL;
SDL_Texture* load2Texture = NULL;
SDL_Texture* fastforwardTexture = NULL;
SDL_Texture* fastforward2Texture = NULL;
SDL_Texture* resetTexture = NULL;
SDL_Texture* reset2Texture = NULL;
SDL_Texture* cheatTexture = NULL;
SDL_Texture* cheat2Texture = NULL;
bool autoLoad = true;
extern jclass mActivityClass;
float density = 1.0f;
int windowWidth = 0;
int windowHeight = 0;
int directionX = 0;
int directionY = 0;
int directionHalfSize = 0;
float l30, l60, l120, l150;
int directionFingerId = -1;
int abFingerId = -1;
int startFingerId = -1;
int selectFingerId = -1;
int left2FingerId = -1;
int right2FingerId = -1;
bool touchFromB = false;
int loadFingerId = -1;
int saveFingerId = -1;
int cheatFingerId = -1;
int fastforwardFingerId = -1;
int resetFingerId = -1;
bool userWantReset = false;
enum DIRECTION_STATUS
{
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	LEFTUP,
	RIGHTUP,
	LEFTDOWN,
	RIGHTDOWN
};

DIRECTION_STATUS directionStatus = NONE;

//SDL_Surface *surface = NULL;
//SDL_Overlay *overlay = NULL;
SDL_Rect overlay_rect;

int systemSpeed = 0;
int systemRedShift = 0;
int systemBlueShift = 0;
int systemGreenShift = 0;
int systemColorDepth = 0;
int systemDebug = 0;
int systemVerbose = 0;
int systemFrameSkip = 0;
int systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;

int srcPitch = 0;
int srcWidth = 0;
int srcHeight = 0;
int destWidth = 0;
int destHeight = 0;

int sensorX = 2047;
int sensorY = 2047;

int filter = 0;
u8 *delta = NULL;

int sdlPrintUsage = 0;
int disableMMX = 0;

int cartridgeType = 3;
int sizeOption = 0;
int captureFormat = 0;

int pauseWhenInactive = 0;
int active = 1;
int emulating = 0;
int RGB_LOW_BITS_MASK=0x821;
u32 systemColorMap32[0x10000];
u16 systemColorMap16[0x10000];
u16 systemGbPalette[24];
void (*filterFunction)(u8*,u32,u8*,u8*,u32,int,int) = NULL;
void (*ifbFunction)(u8*,u32,int,int) = NULL;
int ifbType = 0;
char filename[2048];
char ipsname[2048];
char biosFileName[2048];
char captureDir[2048];
char saveDir[2048];
char batteryDir[2048];

static char *rewindMemory = NULL;
static int rewindPos = 0;
static int rewindTopPos = 0;
static int rewindCounter = 0;
static int rewindCount = 0;
static bool rewindSaveNeeded = false;
static int rewindTimer = 0;

#define REWIND_SIZE 400000

#define _stricmp strcasecmp

bool sdlButtons[4][12] = {
  { false, false, false, false, false, false, 
    false, false, false, false, false, false },
  { false, false, false, false, false, false,
    false, false, false, false, false, false },
  { false, false, false, false, false, false,
    false, false, false, false, false, false },
  { false, false, false, false, false, false,
    false, false, false, false, false, false }
};

bool sdlMotionButtons[4] = { false, false, false, false };

int sdlNumDevices = 0;
SDL_Joystick **sdlDevices = NULL;

bool wasPaused = false;
int autoFrameSkip = 0;
int frameskipadjust = 0;
int showRenderedFrames = 0;
int renderedFrames = 0;

int throttle = 0;
u32 throttleLastTime = 0;
u32 autoFrameSkipLastTime = 0;

int showSpeed = 1;
int showSpeedTransparent = 1;
bool disableStatusMessages = false;
bool paused = false;
bool pauseNextFrame = false;
bool debugger = false;
bool debuggerStub = false;
int fullscreen = 0;
bool systemSoundOn = false;
bool yuv = false;
int yuvType = 0;
bool removeIntros = false;
int sdlFlashSize = 1;
int sdlAutoIPS = 1;
int sdlRtcEnable = 1;
int sdlAgbPrint = 0;

int sdlDefaultJoypad = 0;

extern void debuggerSignal(int,int);

void (*dbgMain)() = debuggerMain;
void (*dbgSignal)(int,int) = debuggerSignal;
void (*dbgOutput)(char *, u32) = debuggerOutput;

int  mouseCounter = 0;
int autoFire = 0;
bool autoFireToggle = false;

bool screenMessage = false;
char screenMessageBuffer[21];
u32  screenMessageTime = 0;

SDL_cond *cond = NULL;
SDL_mutex *mutex = NULL;
u8 sdlBuffer[4096];
int sdlSoundLen = 0;

char *arg0;

#ifndef C_CORE
u8 sdlStretcher[16384];
int sdlStretcherPos;
#else
void (*sdlStretcher)(u8 *, u8*) = NULL;
#endif

enum {
  KEY_LEFT, KEY_RIGHT,
  KEY_UP, KEY_DOWN,
  KEY_BUTTON_A, KEY_BUTTON_B,
  KEY_BUTTON_START, KEY_BUTTON_SELECT,
  KEY_BUTTON_L, KEY_BUTTON_R,
  KEY_BUTTON_SPEED, KEY_BUTTON_CAPTURE
};

u16 joypad[4][12] = {
  { SDLK_LEFT,  SDLK_RIGHT,
    SDLK_UP,    SDLK_DOWN,
    SDLK_z,     SDLK_x,
    SDLK_RETURN,SDLK_BACKSPACE,
    SDLK_a,     SDLK_s,
    SDLK_SPACE, SDLK_F12
  },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

u16 defaultJoypad[12] = {
  SDLK_LEFT,  SDLK_RIGHT,
  SDLK_UP,    SDLK_DOWN,
  SDLK_z,     SDLK_x,
  SDLK_RETURN,SDLK_BACKSPACE,
  SDLK_a,     SDLK_s,
  SDLK_SPACE, SDLK_F12
};

u16 motion[4] = {
  SDLK_KP_4, SDLK_KP_6, SDLK_KP_8, SDLK_KP_2
};

u16 defaultMotion[4] = {
  SDLK_KP_4, SDLK_KP_6, SDLK_KP_8, SDLK_KP_2
};

struct option sdlOptions[] = {
  { "agb-print", no_argument, &sdlAgbPrint, 1 },
  { "auto-frameskip", no_argument, &autoFrameSkip, 1 },  
  { "bios", required_argument, 0, 'b' },
  { "config", required_argument, 0, 'c' },
  { "debug", no_argument, 0, 'd' },
  { "filter", required_argument, 0, 'f' },
  { "filter-normal", no_argument, &filter, 0 },
  { "filter-tv-mode", no_argument, &filter, 1 },
  { "filter-2xsai", no_argument, &filter, 2 },
  { "filter-super-2xsai", no_argument, &filter, 3 },
  { "filter-super-eagle", no_argument, &filter, 4 },
  { "filter-pixelate", no_argument, &filter, 5 },
  { "filter-motion-blur", no_argument, &filter, 6 },
  { "filter-advmame", no_argument, &filter, 7 },
  { "filter-simple2x", no_argument, &filter, 8 },
  { "filter-bilinear", no_argument, &filter, 9 },
  { "filter-bilinear+", no_argument, &filter, 10 },
  { "filter-scanlines", no_argument, &filter, 11 },
  { "filter-hq2x", no_argument, &filter, 12 },
  { "filter-lq2x", no_argument, &filter, 13 },
  { "flash-size", required_argument, 0, 'S' },
  { "flash-64k", no_argument, &sdlFlashSize, 0 },
  { "flash-128k", no_argument, &sdlFlashSize, 1 },
  { "frameskip", required_argument, 0, 's' },
  { "fullscreen", no_argument, &fullscreen, 1 },
  { "gdb", required_argument, 0, 'G' },
  { "help", no_argument, &sdlPrintUsage, 1 },
  { "ifb-none", no_argument, &ifbType, 0 },
  { "ifb-motion-blur", no_argument, &ifbType, 1 },
  { "ifb-smart", no_argument, &ifbType, 2 },
  { "ips", required_argument, 0, 'i' },
  { "no-agb-print", no_argument, &sdlAgbPrint, 0 },
  { "no-auto-frameskip", no_argument, &autoFrameSkip, 0 },
  { "no-debug", no_argument, 0, 'N' },
  { "no-ips", no_argument, &sdlAutoIPS, 0 },
  { "no-mmx", no_argument, &disableMMX, 1 },
  { "no-pause-when-inactive", no_argument, &pauseWhenInactive, 0 },
  { "no-rtc", no_argument, &sdlRtcEnable, 0 },
  { "no-show-speed", no_argument, &showSpeed, 0 },
  { "no-throttle", no_argument, &throttle, 0 },
  { "pause-when-inactive", no_argument, &pauseWhenInactive, 1 },
  { "profile", optional_argument, 0, 'p' },
  { "rtc", no_argument, &sdlRtcEnable, 1 },
  { "save-type", required_argument, 0, 't' },
  { "save-auto", no_argument, &cpuSaveType, 0 },
  { "save-eeprom", no_argument, &cpuSaveType, 1 },
  { "save-sram", no_argument, &cpuSaveType, 2 },
  { "save-flash", no_argument, &cpuSaveType, 3 },
  { "save-sensor", no_argument, &cpuSaveType, 4 },
  { "save-none", no_argument, &cpuSaveType, 5 },
  { "show-speed-normal", no_argument, &showSpeed, 1 },
  { "show-speed-detailed", no_argument, &showSpeed, 2 },
  { "throttle", required_argument, 0, 'T' },
  { "verbose", required_argument, 0, 'v' },  
  { "video-1x", no_argument, &sizeOption, 0 },
  { "video-2x", no_argument, &sizeOption, 1 },
  { "video-3x", no_argument, &sizeOption, 2 },
  { "video-4x", no_argument, &sizeOption, 3 },
  { "yuv", required_argument, 0, 'Y' },
  { NULL, no_argument, NULL, 0 }
};

extern bool CPUIsGBAImage(char *);
extern bool gbIsGameboyRom(char *);

#ifndef C_CORE
#define SDL_LONG(val) \
  *((u32 *)&sdlStretcher[sdlStretcherPos]) = val;\
  sdlStretcherPos+=4;

#define SDL_AND_EAX(val) \
  sdlStretcher[sdlStretcherPos++] = 0x25;\
  SDL_LONG(val);

#define SDL_AND_EBX(val) \
  sdlStretcher[sdlStretcherPos++] = 0x81;\
  sdlStretcher[sdlStretcherPos++] = 0xe3;\
  SDL_LONG(val);

#define SDL_OR_EAX_EBX \
  sdlStretcher[sdlStretcherPos++] = 0x09;\
  sdlStretcher[sdlStretcherPos++] = 0xd8;

#define SDL_LOADL_EBX \
  sdlStretcher[sdlStretcherPos++] = 0x8b;\
  sdlStretcher[sdlStretcherPos++] = 0x1f;

#define SDL_LOADW \
  sdlStretcher[sdlStretcherPos++] = 0x66;\
  sdlStretcher[sdlStretcherPos++] = 0x8b;\
  sdlStretcher[sdlStretcherPos++] = 0x06;\
  sdlStretcher[sdlStretcherPos++] = 0x83;\
  sdlStretcher[sdlStretcherPos++] = 0xc6;\
  sdlStretcher[sdlStretcherPos++] = 0x02;  

#define SDL_LOADL \
  sdlStretcher[sdlStretcherPos++] = 0x8b;\
  sdlStretcher[sdlStretcherPos++] = 0x06;\
  sdlStretcher[sdlStretcherPos++] = 0x83;\
  sdlStretcher[sdlStretcherPos++] = 0xc6;\
  sdlStretcher[sdlStretcherPos++] = 0x04;  

#define SDL_LOADL2 \
  sdlStretcher[sdlStretcherPos++] = 0x8b;\
  sdlStretcher[sdlStretcherPos++] = 0x06;\
  sdlStretcher[sdlStretcherPos++] = 0x83;\
  sdlStretcher[sdlStretcherPos++] = 0xc6;\
  sdlStretcher[sdlStretcherPos++] = 0x03;  

#define SDL_STOREW \
  sdlStretcher[sdlStretcherPos++] = 0x66;\
  sdlStretcher[sdlStretcherPos++] = 0x89;\
  sdlStretcher[sdlStretcherPos++] = 0x07;\
  sdlStretcher[sdlStretcherPos++] = 0x83;\
  sdlStretcher[sdlStretcherPos++] = 0xc7;\
  sdlStretcher[sdlStretcherPos++] = 0x02;  

#define SDL_STOREL \
  sdlStretcher[sdlStretcherPos++] = 0x89;\
  sdlStretcher[sdlStretcherPos++] = 0x07;\
  sdlStretcher[sdlStretcherPos++] = 0x83;\
  sdlStretcher[sdlStretcherPos++] = 0xc7;\
  sdlStretcher[sdlStretcherPos++] = 0x04;  

#define SDL_STOREL2 \
  sdlStretcher[sdlStretcherPos++] = 0x89;\
  sdlStretcher[sdlStretcherPos++] = 0x07;\
  sdlStretcher[sdlStretcherPos++] = 0x83;\
  sdlStretcher[sdlStretcherPos++] = 0xc7;\
  sdlStretcher[sdlStretcherPos++] = 0x03;  

#define SDL_RET \
  sdlStretcher[sdlStretcherPos++] = 0xc3;

#define SDL_PUSH_EAX \
  sdlStretcher[sdlStretcherPos++] = 0x50;

#define SDL_PUSH_ECX \
  sdlStretcher[sdlStretcherPos++] = 0x51;

#define SDL_PUSH_EBX \
  sdlStretcher[sdlStretcherPos++] = 0x53;

#define SDL_PUSH_ESI \
  sdlStretcher[sdlStretcherPos++] = 0x56;

#define SDL_PUSH_EDI \
  sdlStretcher[sdlStretcherPos++] = 0x57;

#define SDL_POP_EAX \
  sdlStretcher[sdlStretcherPos++] = 0x58;

#define SDL_POP_ECX \
  sdlStretcher[sdlStretcherPos++] = 0x59;

#define SDL_POP_EBX \
  sdlStretcher[sdlStretcherPos++] = 0x5b;

#define SDL_POP_ESI \
  sdlStretcher[sdlStretcherPos++] = 0x5e;

#define SDL_POP_EDI \
  sdlStretcher[sdlStretcherPos++] = 0x5f;

#define SDL_MOV_ECX(val) \
  sdlStretcher[sdlStretcherPos++] = 0xb9;\
  SDL_LONG(val);

#define SDL_REP_MOVSB \
  sdlStretcher[sdlStretcherPos++] = 0xf3;\
  sdlStretcher[sdlStretcherPos++] = 0xa4;

#define SDL_REP_MOVSW \
  sdlStretcher[sdlStretcherPos++] = 0xf3;\
  sdlStretcher[sdlStretcherPos++] = 0x66;\
  sdlStretcher[sdlStretcherPos++] = 0xa5;

#define SDL_REP_MOVSL \
  sdlStretcher[sdlStretcherPos++] = 0xf3;\
  sdlStretcher[sdlStretcherPos++] = 0xa5;

void sdlMakeStretcher(int width)
{
  sdlStretcherPos = 0;
  switch(systemColorDepth) {
  case 16:
    if(sizeOption) {
      SDL_PUSH_EAX;
      SDL_PUSH_ESI;
      SDL_PUSH_EDI;
      for(int i = 0; i < width; i++) {
        SDL_LOADW;
        SDL_STOREW;
        SDL_STOREW;
        if(sizeOption > 1) {
          SDL_STOREW;
        }
        if(sizeOption > 2) {
          SDL_STOREW;
        }
      }
      SDL_POP_EDI;
      SDL_POP_ESI;
      SDL_POP_EAX;
      SDL_RET;
    } else {
      SDL_PUSH_ESI;
      SDL_PUSH_EDI;
      SDL_PUSH_ECX;
      SDL_MOV_ECX(width);
      SDL_REP_MOVSW;
      SDL_POP_ECX;
      SDL_POP_EDI;
      SDL_POP_ESI;
      SDL_RET;
    }
    break;
  case 24:
    if(sizeOption) {
      SDL_PUSH_EAX;
      SDL_PUSH_ESI;
      SDL_PUSH_EDI;
      int w = width - 1;
      for(int i = 0; i < w; i++) {
        SDL_LOADL2;
        SDL_STOREL2;
        SDL_STOREL2;
        if(sizeOption > 1) {
          SDL_STOREL2;
        }
        if(sizeOption > 2) {
          SDL_STOREL2;
        }
      }
      // need to write the last one
      SDL_LOADL2;
      SDL_STOREL2;
      if(sizeOption > 1) {
        SDL_STOREL2;
      }
      if(sizeOption > 2) {
        SDL_STOREL2;
      }
      SDL_AND_EAX(0x00ffffff);
      SDL_PUSH_EBX;
      SDL_LOADL_EBX;
      SDL_AND_EBX(0xff000000);
      SDL_OR_EAX_EBX;
      SDL_POP_EBX;
      SDL_STOREL2;
      SDL_POP_EDI;
      SDL_POP_ESI;
      SDL_POP_EAX;
      SDL_RET;
    } else {
      SDL_PUSH_ESI;
      SDL_PUSH_EDI;
      SDL_PUSH_ECX;
      SDL_MOV_ECX(3*width);
      SDL_REP_MOVSB;
      SDL_POP_ECX;
      SDL_POP_EDI;
      SDL_POP_ESI;
      SDL_RET;
    }
    break;
  case 32:
    if(sizeOption) {
      SDL_PUSH_EAX;
      SDL_PUSH_ESI;
      SDL_PUSH_EDI;
      for(int i = 0; i < width; i++) {
        SDL_LOADL;
        SDL_STOREL;
        SDL_STOREL;
        if(sizeOption > 1) {
          SDL_STOREL;
        }
        if(sizeOption > 2) {
          SDL_STOREL;
        }
      }
      SDL_POP_EDI;
      SDL_POP_ESI;
      SDL_POP_EAX;
      SDL_RET;
    } else {
      SDL_PUSH_ESI;
      SDL_PUSH_EDI;
      SDL_PUSH_ECX;
      SDL_MOV_ECX(width);
      SDL_REP_MOVSL;
      SDL_POP_ECX;
      SDL_POP_EDI;
      SDL_POP_ESI;
      SDL_RET;
    }
    break;
  }
}

#ifdef _MSC_VER
#define SDL_CALL_STRETCHER \
  {\
    __asm mov eax, stretcher\
    __asm mov edi, dest\
    __asm mov esi, src\
    __asm call eax\
  }
#else
#define SDL_CALL_STRETCHER \
        asm volatile("call *%%eax"::"a" (stretcher),"S" (src),"D" (dest))
#endif
#else
#define SDL_CALL_STRETCHER \
       sdlStretcher(src, dest)

void sdlStretch16x1(u8 *src, u8 *dest)
{
  u16 *s = (u16 *)src;
  u16 *d = (u16 *)dest;
  for(int i = 0; i < srcWidth; i++)
    *d++ = *s++;
}

void sdlStretch16x2(u8 *src, u8 *dest)
{
  u16 *s = (u16 *)src;
  u16 *d = (u16 *)dest;
  for(int i = 0; i < srcWidth; i++) {
    *d++ = *s;
    *d++ = *s++;
  }
}

void sdlStretch16x3(u8 *src, u8 *dest)
{
  u16 *s = (u16 *)src;
  u16 *d = (u16 *)dest;
  for(int i = 0; i < srcWidth; i++) {
    *d++ = *s;
    *d++ = *s;
    *d++ = *s++;
  }
}

void sdlStretch16x4(u8 *src, u8 *dest)
{
  u16 *s = (u16 *)src;
  u16 *d = (u16 *)dest;
  for(int i = 0; i < srcWidth; i++) {
    *d++ = *s;
    *d++ = *s;
    *d++ = *s;
    *d++ = *s++;
  }
}

void (*sdlStretcher16[4])(u8 *, u8 *) = {
  sdlStretch16x1,
  sdlStretch16x2,
  sdlStretch16x3,
  sdlStretch16x4
};

void sdlStretch32x1(u8 *src, u8 *dest)
{
  u32 *s = (u32 *)src;
  u32 *d = (u32 *)dest;
  for(int i = 0; i < srcWidth; i++)
    *d++ = *s++;
}

void sdlStretch32x2(u8 *src, u8 *dest)
{
  u32 *s = (u32 *)src;
  u32 *d = (u32 *)dest;
  for(int i = 0; i < srcWidth; i++) {
    *d++ = *s;
    *d++ = *s++;
  }
}

void sdlStretch32x3(u8 *src, u8 *dest)
{
  u32 *s = (u32 *)src;
  u32 *d = (u32 *)dest;
  for(int i = 0; i < srcWidth; i++) {
    *d++ = *s;
    *d++ = *s;
    *d++ = *s++;
  }
}

void sdlStretch32x4(u8 *src, u8 *dest)
{
  u32 *s = (u32 *)src;
  u32 *d = (u32 *)dest;
  for(int i = 0; i < srcWidth; i++) {
    *d++ = *s;
    *d++ = *s;
    *d++ = *s;
    *d++ = *s++;
  }
}

void (*sdlStretcher32[4])(u8 *, u8 *) = {
  sdlStretch32x1,
  sdlStretch32x2,
  sdlStretch32x3,
  sdlStretch32x4
};

void sdlStretch24x1(u8 *src, u8 *dest)
{
  u8 *s = src;
  u8 *d = dest;
  for(int i = 0; i < srcWidth; i++) {
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
  }
}

void sdlStretch24x2(u8 *src, u8 *dest)
{
  u8 *s = (u8 *)src;
  u8 *d = (u8 *)dest;
  for(int i = 0; i < srcWidth; i++) {
    *d++ = *s;
    *d++ = *(s+1);
    *d++ = *(s+2);
    s += 3;
    *d++ = *s;
    *d++ = *(s+1);
    *d++ = *(s+2);
    s += 3;
  }
}

void sdlStretch24x3(u8 *src, u8 *dest)
{
  u8 *s = (u8 *)src;
  u8 *d = (u8 *)dest;
  for(int i = 0; i < srcWidth; i++) {
    *d++ = *s;
    *d++ = *(s+1);
    *d++ = *(s+2);
    s += 3;
    *d++ = *s;
    *d++ = *(s+1);
    *d++ = *(s+2);
    s += 3;
    *d++ = *s;
    *d++ = *(s+1);
    *d++ = *(s+2);
    s += 3;
  }
}

void sdlStretch24x4(u8 *src, u8 *dest)
{
  u8 *s = (u8 *)src;
  u8 *d = (u8 *)dest;
  for(int i = 0; i < srcWidth; i++) {
    *d++ = *s;
    *d++ = *(s+1);
    *d++ = *(s+2);
    s += 3;
    *d++ = *s;
    *d++ = *(s+1);
    *d++ = *(s+2);
    s += 3;
    *d++ = *s;
    *d++ = *(s+1);
    *d++ = *(s+2);
    s += 3;
    *d++ = *s;
    *d++ = *(s+1);
    *d++ = *(s+2);
    s += 3;
  }
}

void (*sdlStretcher24[4])(u8 *, u8 *) = {
  sdlStretch24x1,
  sdlStretch24x2,
  sdlStretch24x3,
  sdlStretch24x4
};

#endif

u32 sdlFromHex(char *s)
{
  u32 value;
  sscanf(s, "%x", &value);
  return value;
}

#ifdef __MSC__
#define stat _stat
#define S_IFDIR _S_IFDIR
#endif

void sdlCheckDirectory(char *dir)
{
  struct stat buf;

  int len = strlen(dir);

  char *p = dir + len - 1;

  if(*p == '/' ||
     *p == '\\')
    *p = 0;
  
  if(stat(dir, &buf) == 0) {
    if(!(buf.st_mode & S_IFDIR)) {
      fprintf(stderr, "Error: %s is not a directory\n", dir);
      dir[0] = 0;
    }
  } else {
    fprintf(stderr, "Error: %s does not exist\n", dir);
    dir[0] = 0;
  }
}

char *sdlGetFilename(char *name)
{
  static char filebuffer[2048];

  int len = strlen(name);
  
  char *p = name + len - 1;
  
  while(true) {
    if(*p == '/' ||
       *p == '\\') {
      p++;
      break;
    }
    len--;
    p--;
    if(len == 0)
      break;
  }
  
  if(len == 0)
    strcpy(filebuffer, name);
  else
    strcpy(filebuffer, p);
  return filebuffer;
}

FILE *sdlFindFile(const char *name)
{
	SDL_Log("find file: %s", name);
//  char buffer[4096];
//  char path[2048];
//
//#ifdef WIN32
//#define PATH_SEP ";"
//#define FILE_SEP '\\'
//#define EXE_NAME "VisualBoyAdvance-SDL.exe"
//#else // ! WIN32
//#define PATH_SEP ":"
//#define FILE_SEP '/'
//#define EXE_NAME "VisualBoyAdvance"
//#endif // ! WIN32
//
//  fprintf(stderr, "Searching for file %s\n", name);
//
//  if(GETCWD(buffer, 2048)) {
//    fprintf(stderr, "Searching current directory: %s\n", buffer);
//  }
//
//  FILE *f = fopen(name, "r");
//  if(f != NULL) {
//    return f;
//  }
//
//  char *home = getenv("HOME");
//
//  if(home != NULL) {
//    fprintf(stderr, "Searching home directory: %s\n", home);
//    sprintf(path, "%s%c%s", home, FILE_SEP, name);
//    f = fopen(path, "r");
//    if(f != NULL)
//      return f;
//  }
//
//#ifdef WIN32
//  home = getenv("USERPROFILE");
//  if(home != NULL) {
//    fprintf(stderr, "Searching user profile directory: %s\n", home);
//    sprintf(path, "%s%c%s", home, FILE_SEP, name);
//    f = fopen(path, "r");
//    if(f != NULL)
//      return f;
//  }
//#else // ! WIN32
//    fprintf(stderr, "Searching system config directory: %s\n", SYSCONFDIR);
//    sprintf(path, "%s%c%s", SYSCONFDIR, FILE_SEP, name);
//    f = fopen(path, "r");
//    if(f != NULL)
//      return f;
//#endif // ! WIN32
//
//  if(!strchr(arg0, '/') &&
//     !strchr(arg0, '\\')) {
//    char *path = getenv("PATH");
//
//    if(path != NULL) {
//      fprintf(stderr, "Searching PATH\n");
//      strncpy(buffer, path, 4096);
//      buffer[4095] = 0;
//      char *tok = strtok(buffer, PATH_SEP);
//
//      while(tok) {
//        sprintf(path, "%s%c%s", tok, FILE_SEP, EXE_NAME);
//        f = fopen(path, "r");
//        if(f != NULL) {
//          char path2[2048];
//          fclose(f);
//          sprintf(path2, "%s%c%s", tok, FILE_SEP, name);
//          f = fopen(path2, "r");
//          if(f != NULL) {
//            fprintf(stderr, "Found at %s\n", path2);
//            return f;
//          }
//        }
//        tok = strtok(NULL, PATH_SEP);
//      }
//    }
//  } else {
//    // executable is relative to some directory
//    fprintf(stderr, "Searching executable directory\n");
//    strcpy(buffer, arg0);
//    char *p = strrchr(buffer, FILE_SEP);
//    if(p) {
//      *p = 0;
//      sprintf(path, "%s%c%s", buffer, FILE_SEP, name);
//      f = fopen(path, "r");
//      if(f != NULL)
//        return f;
//    }
//  }
  return NULL;
}

void sdlReadPreferences(FILE *f)
{
  char buffer[2048];
  
  while(1) {
    char *s = fgets(buffer, 2048, f);

    if(s == NULL)
      break;

    char *p  = strchr(s, '#');
    
    if(p)
      *p = 0;
    
    char *token = strtok(s, " \t\n\r=");

    if(!token)
      continue;

    if(strlen(token) == 0)
      continue;

    char *key = token;
    char *value = strtok(NULL, "\t\n\r");

    if(value == NULL) {
      fprintf(stderr, "Empty value for key %s\n", key);
      continue;
    }

    if(!strcmp(key,"Joy0_Left")) {
      joypad[0][KEY_LEFT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_Right")) {
      joypad[0][KEY_RIGHT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_Up")) {
      joypad[0][KEY_UP] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_Down")) {
      joypad[0][KEY_DOWN] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_A")) {
      joypad[0][KEY_BUTTON_A] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_B")) {
      joypad[0][KEY_BUTTON_B] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_L")) {
      joypad[0][KEY_BUTTON_L] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_R")) {
      joypad[0][KEY_BUTTON_R] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_Start")) {
      joypad[0][KEY_BUTTON_START] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_Select")) {
      joypad[0][KEY_BUTTON_SELECT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_Speed")) {
      joypad[0][KEY_BUTTON_SPEED] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_Capture")) {
      joypad[0][KEY_BUTTON_CAPTURE] = sdlFromHex(value);
    } else if(!strcmp(key,"Joy1_Left")) {
      joypad[1][KEY_LEFT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_Right")) {
      joypad[1][KEY_RIGHT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_Up")) {
      joypad[1][KEY_UP] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_Down")) {
      joypad[1][KEY_DOWN] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_A")) {
      joypad[1][KEY_BUTTON_A] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_B")) {
      joypad[1][KEY_BUTTON_B] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_L")) {
      joypad[1][KEY_BUTTON_L] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_R")) {
      joypad[1][KEY_BUTTON_R] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_Start")) {
      joypad[1][KEY_BUTTON_START] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_Select")) {
      joypad[1][KEY_BUTTON_SELECT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_Speed")) {
      joypad[1][KEY_BUTTON_SPEED] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_Capture")) {
      joypad[1][KEY_BUTTON_CAPTURE] = sdlFromHex(value);
    } else if(!strcmp(key,"Joy2_Left")) {
      joypad[2][KEY_LEFT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_Right")) {
      joypad[2][KEY_RIGHT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_Up")) {
      joypad[2][KEY_UP] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_Down")) {
      joypad[2][KEY_DOWN] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_A")) {
      joypad[2][KEY_BUTTON_A] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_B")) {
      joypad[2][KEY_BUTTON_B] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_L")) {
      joypad[2][KEY_BUTTON_L] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_R")) {
      joypad[2][KEY_BUTTON_R] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_Start")) {
      joypad[2][KEY_BUTTON_START] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_Select")) {
      joypad[2][KEY_BUTTON_SELECT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_Speed")) {
      joypad[2][KEY_BUTTON_SPEED] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_Capture")) {
      joypad[2][KEY_BUTTON_CAPTURE] = sdlFromHex(value);
    } else if(!strcmp(key,"Joy4_Left")) {
      joypad[4][KEY_LEFT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_Right")) {
      joypad[4][KEY_RIGHT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_Up")) {
      joypad[4][KEY_UP] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_Down")) {
      joypad[4][KEY_DOWN] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_A")) {
      joypad[4][KEY_BUTTON_A] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_B")) {
      joypad[4][KEY_BUTTON_B] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_L")) {
      joypad[4][KEY_BUTTON_L] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_R")) {
      joypad[4][KEY_BUTTON_R] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_Start")) {
      joypad[4][KEY_BUTTON_START] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_Select")) {
      joypad[4][KEY_BUTTON_SELECT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_Speed")) {
      joypad[4][KEY_BUTTON_SPEED] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_Capture")) {
      joypad[4][KEY_BUTTON_CAPTURE] = sdlFromHex(value);
    } else if(!strcmp(key, "Motion_Left")) {
      motion[KEY_LEFT] = sdlFromHex(value);
    } else if(!strcmp(key, "Motion_Right")) {
      motion[KEY_RIGHT] = sdlFromHex(value);
    } else if(!strcmp(key, "Motion_Up")) {
      motion[KEY_UP] = sdlFromHex(value);
    } else if(!strcmp(key, "Motion_Down")) {
      motion[KEY_DOWN] = sdlFromHex(value);
    } else if(!strcmp(key, "frameSkip")) {
      frameSkip = sdlFromHex(value);
      if(frameSkip < 0 || frameSkip > 9)
        frameSkip = 2;
    } else if(!strcmp(key, "gbFrameSkip")) {
      gbFrameSkip = sdlFromHex(value);
      if(gbFrameSkip < 0 || gbFrameSkip > 9)
        gbFrameSkip = 0;      
    } else if(!strcmp(key, "video")) {
      sizeOption = sdlFromHex(value);
      if(sizeOption < 0 || sizeOption > 3)
        sizeOption = 1;
    } else if(!strcmp(key, "fullScreen")) {
      fullscreen = sdlFromHex(value) ? 1 : 0;
    } else if(!strcmp(key, "useBios")) {
      useBios = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "skipBios")) {
      skipBios = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "biosFile")) {
      strcpy(biosFileName, value);
    } else if(!strcmp(key, "filter")) {
      filter = sdlFromHex(value);
      if(filter < 0 || filter > 13)
        filter = 0;
    } else if(!strcmp(key, "disableStatus")) {
      disableStatusMessages = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "borderOn")) {
      gbBorderOn = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "borderAutomatic")) {
      gbBorderAutomatic = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "emulatorType")) {
      gbEmulatorType = sdlFromHex(value);
      if(gbEmulatorType < 0 || gbEmulatorType > 5)
        gbEmulatorType = 1;
    } else if(!strcmp(key, "colorOption")) {
      gbColorOption = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "captureDir")) {
      sdlCheckDirectory(value);
      strcpy(captureDir, value);
    } else if(!strcmp(key, "saveDir")) {
      sdlCheckDirectory(value);
      strcpy(saveDir, value);
    } else if(!strcmp(key, "batteryDir")) {
      sdlCheckDirectory(value);
      strcpy(batteryDir, value);
    } else if(!strcmp(key, "captureFormat")) {
      captureFormat = sdlFromHex(value);
    } else if(!strcmp(key, "soundQuality")) {
      soundQuality = sdlFromHex(value);
      switch(soundQuality) {
      case 1:
      case 2:
      case 4:
        break;
      default:
        fprintf(stderr, "Unknown sound quality %d. Defaulting to 22Khz\n", 
                soundQuality);
        soundQuality = 2;
        break;
      }
    } else if(!strcmp(key, "soundOff")) {
      soundOffFlag = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "soundEnable")) {
      int res = sdlFromHex(value) & 0x30f;
      soundEnable(res);
      soundDisable(~res);
    } else if(!strcmp(key, "soundEcho")) {
      soundEcho = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "soundLowPass")) {
      soundLowPass = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "soundReverse")) {
      soundReverse = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "soundVolume")) {
      soundVolume = sdlFromHex(value);
      if(soundVolume < 0 || soundVolume > 3)
        soundVolume = 0;
    } else if(!strcmp(key, "removeIntros")) {
      removeIntros = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "saveType")) {
      cpuSaveType = sdlFromHex(value);
      if(cpuSaveType < 0 || cpuSaveType > 5)
        cpuSaveType = 0;
    } else if(!strcmp(key, "flashSize")) {
      sdlFlashSize = sdlFromHex(value);
      if(sdlFlashSize != 0 && sdlFlashSize != 1)
        sdlFlashSize = 0;
    } else if(!strcmp(key, "ifbType")) {
      ifbType = sdlFromHex(value);
      if(ifbType < 0 || ifbType > 2)
        ifbType = 0;
    } else if(!strcmp(key, "showSpeed")) {
      showSpeed = sdlFromHex(value);
      if(showSpeed < 0 || showSpeed > 2)
        showSpeed = 1;
    } else if(!strcmp(key, "showSpeedTransparent")) {
      showSpeedTransparent = sdlFromHex(value);
    } else if(!strcmp(key, "autoFrameSkip")) {
      autoFrameSkip = sdlFromHex(value);
    } else if(!strcmp(key, "throttle")) {
      throttle = sdlFromHex(value);
      if(throttle != 0 && (throttle < 5 || throttle > 1000))
        throttle = 0;
    } else if(!strcmp(key, "disableMMX")) {
#ifdef MMX
      cpu_mmx = sdlFromHex(value) ? false : true;
#endif
    } else if(!strcmp(key, "pauseWhenInactive")) {
      pauseWhenInactive = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "agbPrint")) {
      sdlAgbPrint = sdlFromHex(value);
    } else if(!strcmp(key, "rtcEnabled")) {
      sdlRtcEnable = sdlFromHex(value);
    } else if(!strcmp(key, "rewindTimer")) {
      rewindTimer = sdlFromHex(value);
      if(rewindTimer < 0 || rewindTimer > 600)
        rewindTimer = 0;
      rewindTimer *= 6;  // convert value to 10 frames multiple
    } else if(!strcmp(key, "enhancedDetection")) {
      cpuEnhancedDetection = sdlFromHex(value) ? true : false;
    } else {
      fprintf(stderr, "Unknown configuration key %s\n", key);
    }
  }
}

void sdlReadPreferences()
{
  FILE *f = sdlFindFile("VisualBoyAdvance.cfg");

  if(f == NULL) {
    fprintf(stderr, "Configuration file NOT FOUND (using defaults)\n");
    return;
  } else
    fprintf(stderr, "Reading configuration file.\n");

  sdlReadPreferences(f);

  fclose(f);
}

static void sdlApplyPerImagePreferences()
{
  FILE *f = sdlFindFile("vba-over.ini");
  if(!f) {
    fprintf(stderr, "vba-over.ini NOT FOUND (using emulator settings)\n");
    return;
  } else
    fprintf(stderr, "Reading vba-over.ini\n");

  char buffer[7];
  buffer[0] = '[';
  buffer[1] = rom[0xac];
  buffer[2] = rom[0xad];
  buffer[3] = rom[0xae];
  buffer[4] = rom[0xaf];
  buffer[5] = ']';
  buffer[6] = 0;

  char readBuffer[2048];

  bool found = false;
  
  while(1) {
    char *s = fgets(readBuffer, 2048, f);

    if(s == NULL)
      break;

    char *p  = strchr(s, ';');
    
    if(p)
      *p = 0;
    
    char *token = strtok(s, " \t\n\r=");

    if(!token)
      continue;
    if(strlen(token) == 0)
      continue;

    if(!strcmp(token, buffer)) {
      found = true;
      break;
    }
  }

  if(found) {
    while(1) {
      char *s = fgets(readBuffer, 2048, f);

      if(s == NULL)
        break;

      char *p = strchr(s, ';');
      if(p)
        *p = 0;

      char *token = strtok(s, " \t\n\r=");
      if(!token)
        continue;
      if(strlen(token) == 0)
        continue;

      if(token[0] == '[') // starting another image settings
        break;
      char *value = strtok(NULL, "\t\n\r=");
      if(value == NULL)
        continue;
      
      if(!strcmp(token, "rtcEnabled"))
        rtcEnable(atoi(value) == 0 ? false : true);
      else if(!strcmp(token, "flashSize")) {
        int size = atoi(value);
        if(size == 0x10000 || size == 0x20000)
          flashSetSize(size);
      } else if(!strcmp(token, "saveType")) {
        int save = atoi(value);
        if(save >= 0 && save <= 5)
          cpuSaveType = save;
      }
    }
  }
  fclose(f);
}

static int sdlCalculateShift(u32 mask)
{
  int m = 0;
  
  while(mask) {
    m++;
    mask >>= 1;
  }

  return m-5;
}

static int sdlCalculateMaskWidth(u32 mask)
{
  int m = 0;
  int mask2 = mask;

  while(mask2) {
    m++;
    mask2 >>= 1;
  }

  int m2 = 0;
  mask2 = mask;
  while(!(mask2 & 1)) {
    m2++;
    mask2 >>= 1;
  }

  return m - m2;
}

void sdlWriteState(int num)
{
  char stateName[2048];

  if(saveDir[0])
    sprintf(stateName, "%s/%s%d.sgm", saveDir, sdlGetFilename(filename),
            num+1);
  else
    sprintf(stateName,"%s%d.sgm", filename, num+1);
  if(emulator.emuWriteState)
    emulator.emuWriteState(stateName);
  sprintf(stateName, "Wrote state %d", num+1);
  systemScreenMessage(stateName);
}

void sdlReadState(int num)
{
  char stateName[2048];

  if(saveDir[0])
    sprintf(stateName, "%s/%s%d.sgm", saveDir, sdlGetFilename(filename),
            num+1);
  else
    sprintf(stateName,"%s%d.sgm", filename, num+1);

  if(emulator.emuReadState)
    emulator.emuReadState(stateName);

  sprintf(stateName, "Loaded state %d", num+1);
  systemScreenMessage(stateName);
}

void sdlWriteBattery()
{
  char buffer[1048];

  if(batteryDir[0])
    sprintf(buffer, "%s/%s.sav", batteryDir, sdlGetFilename(filename));
  else  
    sprintf(buffer, "%s.sav", filename);

  emulator.emuWriteBattery(buffer);

  systemScreenMessage("Wrote battery");
}

void sdlReadBattery()
{
  char buffer[1048];
  
  if(batteryDir[0])
    sprintf(buffer, "%s/%s.sav", batteryDir, sdlGetFilename(filename));
  else 
    sprintf(buffer, "%s.sav", filename);
  
  bool res = false;

  res = emulator.emuReadBattery(buffer);

  if(res)
    systemScreenMessage("Loaded battery");
}

#define MOD_KEYS    (KMOD_CTRL|KMOD_SHIFT|KMOD_ALT/*|KMOD_META*/)
#define MOD_NOCTRL  (KMOD_SHIFT|KMOD_ALT/*|KMOD_META*/)
#define MOD_NOALT   (KMOD_CTRL|KMOD_SHIFT/*|KMOD_META*/)
#define MOD_NOSHIFT (KMOD_CTRL|KMOD_ALT/*|KMOD_META*/)

void sdlUpdateKey(int key, bool down)
{
  int i;
  for(int j = 0; j < 4; j++)
  {
    for(i = 0 ; i < 12; i++)
    {
      if((joypad[j][i] & 0xf000) == 0)
      {
        if(key == joypad[j][i])
        {
          sdlButtons[j][i] = down;
        }
      }
    }
  }
  for(i = 0 ; i < 4; i++) {
    if((motion[i] & 0xf000) == 0) {
      if(key == motion[i])
        sdlMotionButtons[i] = down;
    }
  }
}

void sdlUpdateJoyButton(int which,
                        int button,
                        bool pressed)
{
  int i;
  for(int j = 0; j < 4; j++) {
    for(i = 0; i < 12; i++) {
      int dev = (joypad[j][i] >> 12);
      int b = joypad[j][i] & 0xfff;
      if(dev) {
        dev--;
        
        if((dev == which) && (b >= 128) && (b == (button+128))) {
          sdlButtons[j][i] = pressed;
        }
      }
    }
  }
  for(i = 0; i < 4; i++) {
    int dev = (motion[i] >> 12);
    int b = motion[i] & 0xfff;
    if(dev) {
      dev--;

      if((dev == which) && (b >= 128) && (b == (button+128))) {
        sdlMotionButtons[i] = pressed;
      }
    }
  }  
}

void sdlUpdateJoyHat(int which,
                     int hat,
                     int value)
{
  int i;
  for(int j = 0; j < 4; j++) {
    for(i = 0; i < 12; i++) {
      int dev = (joypad[j][i] >> 12);
      int a = joypad[j][i] & 0xfff;
      if(dev) {
        dev--;
        
        if((dev == which) && (a>=32) && (a < 48) && (((a&15)>>2) == hat)) {
          int dir = a & 3;
          int v = 0;
          switch(dir) {
          case 0:
            v = value & SDL_HAT_UP;
            break;
          case 1:
            v = value & SDL_HAT_DOWN;
            break;
          case 2:
            v = value & SDL_HAT_RIGHT;
            break;
          case 3:
            v = value & SDL_HAT_LEFT;
            break;
          }
          sdlButtons[j][i] = (v ? true : false);
        }
      }
    }
  }
  for(i = 0; i < 4; i++) {
    int dev = (motion[i] >> 12);
    int a = motion[i] & 0xfff;
    if(dev) {
      dev--;

      if((dev == which) && (a>=32) && (a < 48) && (((a&15)>>2) == hat)) {
        int dir = a & 3;
        int v = 0;
        switch(dir) {
        case 0:
          v = value & SDL_HAT_UP;
          break;
        case 1:
          v = value & SDL_HAT_DOWN;
          break;
        case 2:
          v = value & SDL_HAT_RIGHT;
          break;
        case 3:
          v = value & SDL_HAT_LEFT;
          break;
        }
        sdlMotionButtons[i] = (v ? true : false);
      }
    }
  }      
}

void sdlUpdateJoyAxis(int which,
                      int axis,
                      int value)
{
  int i;
  for(int j = 0; j < 4; j++) {
    for(i = 0; i < 12; i++) {
      int dev = (joypad[j][i] >> 12);
      int a = joypad[j][i] & 0xfff;
      if(dev) {
        dev--;
        
        if((dev == which) && (a < 32) && ((a>>1) == axis)) {
          sdlButtons[j][i] = (a & 1) ? (value > 16384) : (value < -16384);
        }
      }
    }
  }
  for(i = 0; i < 4; i++) {
    int dev = (motion[i] >> 12);
    int a = motion[i] & 0xfff;
    if(dev) {
      dev--;

      if((dev == which) && (a < 32) && ((a>>1) == axis)) {
        sdlMotionButtons[i] = (a & 1) ? (value > 16384) : (value < -16384);
      }
    }
  }  
}

bool sdlCheckJoyKey(int key)
{
  int dev = (key >> 12) - 1;
  int what = key & 0xfff;

  if(what >= 128) {
    // joystick button
    int button = what - 128;

    if(button >= SDL_JoystickNumButtons(sdlDevices[dev]))
      return false;
  } else if (what < 0x20) {
    // joystick axis    
    what >>= 1;
    if(what >= SDL_JoystickNumAxes(sdlDevices[dev]))
      return false;
  } else if (what < 0x30) {
    // joystick hat
    what = (what & 15);
    what >>= 2;
    if(what >= SDL_JoystickNumHats(sdlDevices[dev]))
      return false;
  }

  // no problem found
  return true;
}

void sdlCheckKeys()
{
  sdlNumDevices = SDL_NumJoysticks();

  if(sdlNumDevices)
    sdlDevices = (SDL_Joystick **)calloc(1,sdlNumDevices *
                                         sizeof(SDL_Joystick **));
  int i;

  bool usesJoy = false;

  for(int j = 0; j < 4; j++) {
    for(i = 0; i < 12; i++) {
      int dev = joypad[j][i] >> 12;
      if(dev) {
        dev--;
        bool ok = false;
        
        if(sdlDevices) {
          if(dev < sdlNumDevices) {
            if(sdlDevices[dev] == NULL) {
              sdlDevices[dev] = SDL_JoystickOpen(dev);
            }
            
            ok = sdlCheckJoyKey(joypad[j][i]);
          } else
            ok = false;
        }
        
        if(!ok)
          joypad[j][i] = defaultJoypad[i];
        else
          usesJoy = true;
      }
    }
  }

  for(i = 0; i < 4; i++) {
    int dev = motion[i] >> 12;
    if(dev) {
      dev--;
      bool ok = false;
      
      if(sdlDevices) {
        if(dev < sdlNumDevices) {
          if(sdlDevices[dev] == NULL) {
            sdlDevices[dev] = SDL_JoystickOpen(dev);
          }
          
          ok = sdlCheckJoyKey(motion[i]);
        } else
          ok = false;
      }
      
      if(!ok)
        motion[i] = defaultMotion[i];
      else
        usesJoy = true;
    }
  }

  if(usesJoy)
    SDL_JoystickEventState(SDL_ENABLE);
}

int squareLength(int x1, int y1, int x2, int y2)
{
	int dx = x2 - x1;
	int dy = y2 - y1;
	return dx * dx + dy * dy;
}

void sdlPollEvents()
{
  SDL_Event event;
  while(SDL_PollEvent(&event)) {

    switch(event.type) {
    case SDL_QUIT:
      emulating = 0;
      break;
//    case SDL_ACTIVEEVENT:
//      if(pauseWhenInactive && (event.active.state & SDL_APPINPUTFOCUS)) {
//        active = event.active.gain;
//        if(active) {
//          if(!paused) {
//            if(emulating)
//              soundResume();
//          }
//        } else {
//          wasPaused = true;
//          if(pauseWhenInactive) {
//            if(emulating)
//              soundPause();
//          }
//
//          memset(delta,255,sizeof(delta));
//        }
//      }
//      break;
	  case SDL_WINDOWEVENT:
			if((event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) || (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST))
			{
			  if(event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
				  active = 1;
			  else
				  active = 0;
			  if(active) {
				if(!paused) {
				  if(emulating)
				  {
					//soundResume();
					  if(autoLoad)
					  {
						if(emulator.emuReadState)
						{
							std::string strAutoSaveState = SDL_GetRomFile();
							strAutoSaveState += ".sgm";
							emulator.emuReadState(strAutoSaveState.c_str());
						}
					  }
				  }
				}
			  } else {
				wasPaused = true;
				  if(emulating)
				  {
					if(emulator.emuWriteState)
					{
						std::string strAutoSaveState = SDL_GetRomFile();
						strAutoSaveState += ".sgm";
						emulator.emuWriteState(strAutoSaveState.c_str());
						autoLoad = true;
					}
					//soundPause();
				  }

				memset(delta,255,sizeof(delta));
			  }
			}
		break;
    case SDL_FINGERUP:
    	if(directionFingerId == event.tfinger.fingerId)
    	{
    		directionFingerId = -1;
    		directionStatus = NONE;
			sdlButtons[0][0] = false;
			sdlButtons[0][1] = false;
			sdlButtons[0][2] = false;
			sdlButtons[0][3] = false;
    	}
    	else if(abFingerId == event.tfinger.fingerId)
    	{
    		abFingerId = -1;
    		sdlButtons[0][4] = false;
    		sdlButtons[0][5] = false;
    	}
    	else if(startFingerId == event.tfinger.fingerId)
    	{
    		startFingerId = -1;
    		sdlButtons[0][6] = false; // start up
    	}
    	else if(selectFingerId == event.tfinger.fingerId)
    	{
    		selectFingerId = -1;
    		sdlButtons[0][7] = false;
    	}
    	else if(left2FingerId == event.tfinger.fingerId)
    	{
    		left2FingerId = -1;
    		sdlButtons[0][8] = false;
    	}
    	else if(right2FingerId == event.tfinger.fingerId)
    	{
    		right2FingerId = -1;
    		sdlButtons[0][9] = false;
    	}
    	else if(loadFingerId == event.tfinger.fingerId)
		{
    		loadFingerId = -1;
    		JNIEnv* pEnv = (JNIEnv*)::SDL_AndroidGetJNIEnv();
			jmethodID jmid = pEnv->GetStaticMethodID(mActivityClass, "StartLoadStateActivity", "()V");
			pEnv->CallStaticVoidMethod(mActivityClass, jmid);
		}
    	else if(saveFingerId == event.tfinger.fingerId)
		{
    		saveFingerId = -1;
    		JNIEnv* pEnv = (JNIEnv*)::SDL_AndroidGetJNIEnv();
    		jmethodID jmid = pEnv->GetStaticMethodID(mActivityClass, "StartSaveStateActivity", "()V");
    		pEnv->CallStaticVoidMethod(mActivityClass, jmid);
		}
    	else if(cheatFingerId == event.tfinger.fingerId)
    	{
    		cheatFingerId = -1;
    		JNIEnv* pEnv = (JNIEnv*)::SDL_AndroidGetJNIEnv();
			jmethodID jmid = pEnv->GetStaticMethodID(mActivityClass, "StartGamesharkActivity", "()V");
			pEnv->CallStaticVoidMethod(mActivityClass, jmid);
    	}
//    	else if(fastforwardFingerId == event.tfinger.fingerId)
//    	{
//    		//sdlButtons[0][10] = false;
//    		fastforwardFingerId = -1;
//    	}
    	else if(resetFingerId == event.tfinger.fingerId)
    	{
    		resetFingerId = -1;
    		SDL_Log("gdsgsdg");
    		JNIEnv* pEnv = (JNIEnv*)::SDL_AndroidGetJNIEnv();
			jmethodID jmid = pEnv->GetStaticMethodID(mActivityClass, "showResetConfirmDialog", "()V");
			pEnv->CallStaticVoidMethod(mActivityClass, jmid);
    	}
    	break;
    case SDL_FINGERDOWN:
    case SDL_FINGERMOTION:
      {
    	  int x = event.tfinger.x * windowWidth;
    	  int y = event.tfinger.y * windowHeight;

    	  if(event.type == SDL_FINGERDOWN && startFingerId == -1)
		  {
			  // Start button
			  int sx = windowWidth - 80 * density;
			  int sy = windowHeight - 160 * density;
			  int sw = 80 * density;
			  int sh = 40 * density;
			  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
			  {
				  startFingerId = event.tfinger.fingerId;
				  sdlButtons[0][6] = true;
			  }
		  }

    	  if(event.type == SDL_FINGERDOWN && selectFingerId == -1)
		  {
			  int sx = windowWidth - 160 * density;
			  int sy = windowHeight - 160 * density;
			  int sw = 80 * density;
			  int sh = 40 * density;
			  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
			  {
				  selectFingerId = event.tfinger.fingerId;
				  sdlButtons[0][7] = true;
			  }
		  }

    	  if(event.type == SDL_FINGERDOWN && loadFingerId == -1)
    	  {
    		  int sx = 0;
    		  int sy = 0;
    		  int sw = 80 * density;
    		  int sh = 40 * density;
    		  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
			  {
    			  loadFingerId = event.tfinger.fingerId;
			  }
    	  }

    	  if(event.type == SDL_FINGERDOWN && saveFingerId == -1)
    	  {
    		  int sx = windowWidth - 80 * density;
    		  int sy = 0;
    		  int sw = 80 * density;
    		  int sh = 40 * density;
    		  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
			  {
    			  saveFingerId = event.tfinger.fingerId;
			  }
    	  }

    	  if(event.type == SDL_FINGERDOWN && cheatFingerId == -1)
    	  {
    		  int sx = windowWidth - 80 * density;
    		  int sy = 40 * density;
    		  int sw = 80 * density;
    		  int sh = 40 * density;
    		  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
			  {
    			  cheatFingerId = event.tfinger.fingerId;
			  }
    	  }

    	  if(event.type == SDL_FINGERDOWN && left2FingerId == -1)
    	  {
    		  int sx = 0;
			  int sy = windowHeight - 240 * density;
			  int sw = 80 * density;
			  int sh = 40 * density;
			  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
			  {
				  left2FingerId = event.tfinger.fingerId;
				  sdlButtons[0][8] = true;
			  }
    	  }

    	  if(event.type == SDL_FINGERDOWN && fastforwardFingerId == -1)
    	  {
    		  int sx = 80 * density;
			  int sy = windowHeight - 240 * density;
			  int sw = 80 * density;
			  int sh = 40 * density;
			  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
			  {
//				  fastforwardFingerId = event.tfinger.fingerId;
				  sdlButtons[0][10] = !sdlButtons[0][10];
			  }

//			  sx = windowWidth - 160 * density;
//			  int sy = windowHeight - 120 * density;
//			  int sw = 80 * density;
//			  int sh = 40 * density;
//			  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
//			  {
//				  fastforwardFingerId = event.tfinger.fingerId;
//				  sdlButtons[0][10] = true;
//			  }
    	  }

    	  if(event.type == SDL_FINGERDOWN && right2FingerId == -1)
    	  {
    		  int sx = windowWidth - 80 * density;
			  int sy = windowHeight - 120 * density;
			  int sw = 80 * density;
			  int sh = 40 * density;
			  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
			  {
				  right2FingerId = event.tfinger.fingerId;
				  sdlButtons[0][9] = true;
			  }
    	  }

    	  if(event.type == SDL_FINGERDOWN && resetFingerId == -1)
    	  {
    		  int sx = (windowWidth - 80 * density) / 2;
			  int sy = 0;
			  int sw = 80 * density;
			  int sh = 40 * density;
			  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
			  {
				  resetFingerId = event.tfinger.fingerId;
			  }
    	  }

    	  if(event.type == SDL_FINGERDOWN && abFingerId == -1)
    	  {
    		  {
    			  // A button
    			  int sx = windowWidth - 65 * density;
    			  int sy = windowHeight - 80 * density;
    			  int sw = 65 * density;
    			  int sh = 80 * density;
    			  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
				  {
    				  sdlButtons[0][4] = true;
					  abFingerId = event.tfinger.fingerId;
					  touchFromB = false;
				  }
    		  }
    		  {
    			  // B button
    			  int sx = windowWidth - 160 * density;
    			  int sy = windowHeight - 80 * density;
    			  int sw = 65 * density;
    			  int sh = 80 * density;
    			  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
				  {
    				  sdlButtons[0][5] = true;
					  abFingerId = event.tfinger.fingerId;
					  touchFromB = true;
				  }
    		  }
    		  {
    			  // AB button
    			  int sx = windowWidth - (160 - 65) * density;
    			  int sy = windowHeight - 80 * density;
    			  int sw = 30 * density;
    			  int sh = 80 * density;
    			  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
				  {
    				  sdlButtons[0][4] = true;
    				  sdlButtons[0][5] = true;
					  abFingerId = event.tfinger.fingerId;
					  touchFromB = false;
				  }
    		  }
    	  }
    	  else if(event.type == SDL_FINGERMOTION && event.tfinger.fingerId == abFingerId)
    	  {
			  {
				  // AB button
				  int sx = windowWidth - (160 - 65) * density;
				  int sy = windowHeight - 80 * density;
				  int sw = 30 * density;
				  int sh = 80 * density;
				  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
				  {
					  sdlButtons[0][4] = true;
					  sdlButtons[0][5] = true;
					  return;
				  }
			  }
    		  {
				  // A button
				  int sx = windowWidth - 65 * density;
				  int sy = windowHeight - 80 * density;
				  int sw = 65 * density;
				  int sh = 80 * density;
				  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
				  {
					  sdlButtons[0][4] = true;
					  abFingerId = event.tfinger.fingerId;
				  }
				  else
				  {
					  if(touchFromB)
					  {
						  sdlButtons[0][4] = false;
					  }
				  }
    		  }
			  {
				  // B button
				  int sx = windowWidth - 160 * density;
				  int sy = windowHeight - 80 * density;
				  int sw = 65 * density;
				  int sh = 80 * density;
				  if(x > sx && x < sx + sw && y > sy && y < sy + sh)
				  {
					  sdlButtons[0][5] = true;
					  abFingerId = event.tfinger.fingerId;
				  }
				  else
				  {
					  if(!touchFromB)
					  {
						  sdlButtons[0][5] = false;
					  }
				  }
			  }

    	  }

    	  if(directionFingerId == -1 || directionFingerId == event.tfinger.fingerId)
    	  {
			  int l = squareLength(x, y, directionX, directionY);
			  if(l >= (directionHalfSize / 3) * (directionHalfSize / 3) && l <= directionHalfSize * directionHalfSize)
			  {
				  directionFingerId = event.tfinger.fingerId;
				  float vx, vy;
				  vx = x - directionX;
				  vy = y - directionY;
				  float t = sqrt(l);
				  vx /= t;
				  vy /= t;
				  vx *= directionHalfSize;
				  vy *= directionHalfSize;
				  int tx = directionX + vx;
				  int ty = directionY + vy;

				sdlButtons[0][0] = false; //left
				sdlButtons[0][1] = false; //right
				sdlButtons[0][2] = false; //up
				sdlButtons[0][3] = false; //down

				  if(vy > 0)
				  {
					  l = squareLength(tx, ty, directionX + directionHalfSize, directionY);
					  if(l < l30)
					  {
						  directionStatus = RIGHT;
						  sdlButtons[0][1] = true;
					  }
					  else if(l < l60)
					  {
						  directionStatus = RIGHTDOWN;
						  sdlButtons[0][1] = true;
						  sdlButtons[0][3] = true;
					  }
					  else if(l < l120)
					  {
						  directionStatus = DOWN;
						  sdlButtons[0][3] = true;
					  }
					  else if(l < l150)
					  {
						  directionStatus = LEFTDOWN;
						  sdlButtons[0][0] = true;
						  sdlButtons[0][3] = true;
					  }
					  else
					  {
						  directionStatus = LEFT;
						  sdlButtons[0][0] = true;
					  }
				  }
				  else
				  {
					  l = squareLength(tx, ty, directionX + directionHalfSize, directionY);
					  if(l < l30)
					  {
						  directionStatus = RIGHT;
						  sdlButtons[0][1] = true;
					  }
					  else if(l < l60)
					  {
						  directionStatus = RIGHTUP;
						  sdlButtons[0][1] = true;
						  sdlButtons[0][2] = true;
					  }
					  else if(l < l120)
					  {
						  directionStatus = UP;
						  sdlButtons[0][2] = true;
					  }
					  else if(l < l150)
					  {
						  directionStatus = LEFTUP;
						  sdlButtons[0][0] = true;
						  sdlButtons[0][2] = true;
					  }
					  else
					  {
						  directionStatus = LEFT;
						  sdlButtons[0][0] = true;
					  }
				  }
			  }
			  else
			  {
				  directionFingerId = -1;
				directionStatus = NONE;
				sdlButtons[0][0] = false;
				sdlButtons[0][1] = false;
				sdlButtons[0][2] = false;
				sdlButtons[0][3] = false;
			  }
    	  }
      }

      break;
    case SDL_JOYHATMOTION:
      sdlUpdateJoyHat(event.jhat.which,
                      event.jhat.hat,
                      event.jhat.value);
      break;
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
      sdlUpdateJoyButton(event.jbutton.which,
                         event.jbutton.button,
                         event.jbutton.state == SDL_PRESSED);
      break;
    case SDL_JOYAXISMOTION:
      sdlUpdateJoyAxis(event.jaxis.which,
                       event.jaxis.axis,
                       event.jaxis.value);
      break;
    case SDL_KEYDOWN:
      sdlUpdateKey(event.key.keysym.sym, true);
      break;
    case SDL_KEYUP:

    	if(event.key.keysym.scancode == 270)
    		emulating = false;

      switch(event.key.keysym.sym) {
      case SDLK_r:
        if(!(event.key.keysym.mod & MOD_NOCTRL) &&
           (event.key.keysym.mod & KMOD_CTRL)) {
          if(emulating) {
            emulator.emuReset();

            systemScreenMessage("Reset");
          }
        }
        break;
      case SDLK_b:
        if(!(event.key.keysym.mod & MOD_NOCTRL) &&
           (event.key.keysym.mod & KMOD_CTRL)) {
          if(emulating && emulator.emuReadMemState && rewindMemory 
             && rewindCount) {
            rewindPos = --rewindPos & 7;
            emulator.emuReadMemState(&rewindMemory[REWIND_SIZE*rewindPos], 
                                     REWIND_SIZE);
            rewindCount--;
            rewindCounter = 0;
            systemScreenMessage("Rewind");
          }
        }
        break;
      case SDLK_p:
        if(!(event.key.keysym.mod & MOD_NOCTRL) &&
           (event.key.keysym.mod & KMOD_CTRL)) {
          paused = !paused;
          SDL_PauseAudio(paused);
          if(paused)
            wasPaused = true;
        }
        break;
      case SDLK_ESCAPE:
        emulating = 0;
        break;
//      case SDLK_f:
//        if(!(event.key.keysym.mod & MOD_NOCTRL) &&
//           (event.key.keysym.mod & KMOD_CTRL)) {
//          int flags = 0;
//          fullscreen = !fullscreen;
//          if(fullscreen)
//            flags |= SDL_FULLSCREEN;
//          SDL_SetVideoMode(destWidth, destHeight, systemColorDepth, flags);
//          //          if(SDL_WM_ToggleFullScreen(surface))
//          //            fullscreen = !fullscreen;
//        }
//        break;
      case SDLK_F11:
        if(dbgMain != debuggerMain) {
          if(armState) {
            armNextPC -= 4;
            reg[15].I -= 4;
          } else {
            armNextPC -= 2;
            reg[15].I -= 2;
          }
        }
        debugger = true;
        break;
      case SDLK_F1:
      case SDLK_F2:
      case SDLK_F3:
      case SDLK_F4:
      case SDLK_F5:
      case SDLK_F6:
      case SDLK_F7:
      case SDLK_F8:
      case SDLK_F9:
      case SDLK_F10:
        if(!(event.key.keysym.mod & MOD_NOSHIFT) &&
           (event.key.keysym.mod & KMOD_SHIFT)) {
          sdlWriteState(event.key.keysym.sym-SDLK_F1);
        } else if(!(event.key.keysym.mod & MOD_KEYS)) {
          sdlReadState(event.key.keysym.sym-SDLK_F1);
        }
        break;
      case SDLK_1:
      case SDLK_2:
      case SDLK_3:
      case SDLK_4:
        if(!(event.key.keysym.mod & MOD_NOALT) &&
           (event.key.keysym.mod & KMOD_ALT)) {
          char *disableMessages[4] = 
            { "autofire A disabled",
              "autofire B disabled",
              "autofire R disabled",
              "autofire L disabled"};
          char *enableMessages[4] = 
            { "autofire A",
              "autofire B",
              "autofire R",
              "autofire L"};
          int mask = 1 << (event.key.keysym.sym - SDLK_1);
    if(event.key.keysym.sym > SDLK_2)
      mask <<= 6;
          if(autoFire & mask) {
            autoFire &= ~mask;
            systemScreenMessage(disableMessages[event.key.keysym.sym - SDLK_1]);
          } else {
            autoFire |= mask;
            systemScreenMessage(enableMessages[event.key.keysym.sym - SDLK_1]);
          }
        } if(!(event.key.keysym.mod & MOD_NOCTRL) &&
             (event.key.keysym.mod & KMOD_CTRL)) {
          int mask = 0x0100 << (event.key.keysym.sym - SDLK_1);
          layerSettings ^= mask;
          layerEnable = DISPCNT & layerSettings;
          CPUUpdateRenderBuffers(false);
        }
        break;
      case SDLK_5:
      case SDLK_6:
      case SDLK_7:
      case SDLK_8:
        if(!(event.key.keysym.mod & MOD_NOCTRL) &&
           (event.key.keysym.mod & KMOD_CTRL)) {
          int mask = 0x0100 << (event.key.keysym.sym - SDLK_1);
          layerSettings ^= mask;
          layerEnable = DISPCNT & layerSettings;
        }
        break;
      case SDLK_n:
        if(!(event.key.keysym.mod & MOD_NOCTRL) &&
           (event.key.keysym.mod & KMOD_CTRL)) {
          if(paused)
            paused = false;
          pauseNextFrame = true;
        }
        break;
      default:
        break;
      }
      sdlUpdateKey(event.key.keysym.sym, false);
      break;
    }
  }
}

extern "C"
{
	void Java_org_libsdl_app_SDLActivity_saveGameState(JNIEnv* env, jclass obj, jstring strStateFile);
	void Java_org_libsdl_app_SDLActivity_loadGameState(JNIEnv* env, jclass obj, jstring strStateFile);
	void Java_org_libsdl_app_SDLActivity_turnOffAutoLoad(JNIEnv* env, jclass obj);
	void Java_org_libsdl_app_SDLActivity_setUserWantReset(JNIEnv* env, jclass obj);
	//void Java_org_libsdl_app_SDLActivity_loadGamesharkFile(JNIEnv* env, jclass obj, jstring strFile);
	int Java_org_libsdl_app_SDLActivity_getCheatsNumber(JNIEnv* env, jclass obj);
	jstring Java_org_libsdl_app_SDLActivity_getCheatCodeString(JNIEnv* env, jclass obj, int i);
	jstring Java_org_libsdl_app_SDLActivity_getCheatDesc(JNIEnv* env, jclass obj, int i);
	bool Java_org_libsdl_app_SDLActivity_getCheatEnabled(JNIEnv* env, jclass obj, int i);
	void Java_org_libsdl_app_SDLActivity_addCheat(JNIEnv* env, jclass obj, jstring jstrDesc, jstring jstrCode);
	void Java_org_libsdl_app_SDLActivity_deleteCheat(JNIEnv* env, jclass obj, int i);
};

void Java_org_libsdl_app_SDLActivity_deleteCheat(JNIEnv* env, jclass obj, int i)
{
	::cheatsDelete(i, false);
}

void Java_org_libsdl_app_SDLActivity_addCheat(JNIEnv* env, jclass obj, jstring jstrDesc, jstring jstrCode)
{
	const char* strCode = env->GetStringUTFChars(jstrCode, 0);
	const char* strDesc = env->GetStringUTFChars(jstrDesc, 0);
	cheatsAddCBACode(strCode, strDesc);
}

int Java_org_libsdl_app_SDLActivity_getCheatsNumber(JNIEnv* env, jclass obj)
{
	return cheatsNumber;
}

jstring Java_org_libsdl_app_SDLActivity_getCheatCodeString(JNIEnv* env, jclass obj, int i)
{
	return env->NewStringUTF(cheatsList[i].codestring);
}

jstring Java_org_libsdl_app_SDLActivity_getCheatDesc(JNIEnv* env, jclass obj, int i)
{
	return env->NewStringUTF(cheatsList[i].desc);
}

bool Java_org_libsdl_app_SDLActivity_getCheatEnabled(JNIEnv* env, jclass obj, int i)
{
	return cheatsList[i].enabled;
}

void Java_org_libsdl_app_SDLActivity_setUserWantReset(JNIEnv* env, jclass obj)
{
	userWantReset = true;
}

void Java_org_libsdl_app_SDLActivity_turnOffAutoLoad(JNIEnv* env, jclass obj)
{
	autoLoad = false;
}

void Java_org_libsdl_app_SDLActivity_saveGameState(JNIEnv* env, jclass obj, jstring jstrStateFile)
{
	const char* strStateFile = env->GetStringUTFChars(jstrStateFile, NULL);
	SDL_Log(strStateFile);

	if(emulator.emuWriteState)
	{
	    emulator.emuWriteState(strStateFile);
	    char buffer[1024];
	    strcpy(buffer, strStateFile);
	    strcat(buffer, ".png");
	    emulator.emuWritePNG(buffer);
	}
	int n = strlen(strStateFile);
	char c = strStateFile[n - 5];
	std::string strMessage("Saved to slot ");
	strMessage += c;
	systemScreenMessage(strMessage.c_str());
}

void Java_org_libsdl_app_SDLActivity_loadGameState(JNIEnv* env, jclass obj, jstring jstrStateFile)
{
	const char* strStateFile = env->GetStringUTFChars(jstrStateFile, NULL);
	SDL_Log(strStateFile);

	if(emulator.emuReadState)
	    emulator.emuReadState(strStateFile);
	int n = strlen(strStateFile);
	char c = strStateFile[n - 5];
	std::string strMessage("Loaded slot ");
	strMessage += c;
	systemScreenMessage(strMessage.c_str());
}

#include <fstream>

//void Java_org_libsdl_app_SDLActivity_loadGamesharkFile(JNIEnv* env, jclass obj, jstring jstrFile)
//{
//	const char* strFile = env->GetStringUTFChars(jstrFile, NULL);
//	std::ifstream fin(strFile);
//	SDL_Log(strFile);
//	std::string s;
//	if(fin)
//	{
//		char* temp;
//		int nFileLength = 0;
//		fin.seekg(0, fin.end);
//		nFileLength = fin.tellg();
//		fin.seekg(0, fin.beg);
//		temp = new char[nFileLength + 1];
//		fin.read(temp, nFileLength);
//		temp[nFileLength] = 0;
//		s = temp;
//		SDL_Log("entire file:%s", s.c_str());
//		delete[] temp;
//		while(s != "")
//		{
//			int k = s.find('\n');
//			std::string s1 = s.substr(0, s.find('\n'));
//			s = s.substr(s1.length() + 1);
//
//			int n = s1.rfind(';');
//			std::string strMessage = s1.substr(0, n);
//			std::string strCode = s1.substr(n + 1);
//			if(strCode != "")
//			{
//				while(((int)strCode.find(' ')) >= 0)
//					strCode = strCode.substr(0, strCode.find(' ')) + strCode.substr(strCode.find(' ') + 1);
//
//				{
//					if(strCode.length() == 16)
//					{
//						cheatsAddGSACode(strCode.c_str(), strMessage.c_str(), false);
//						SDL_Log("add gsa code:%s", strCode.c_str());
//					}
//					else if(strCode.length() == 12)
//					{
//						strCode = strCode.substr(0, 8) + " " + strCode.substr(8);
//						cheatsAddCBACode(strCode.c_str(), strMessage.c_str());
//						SDL_Log("add cba code:%s", strCode.c_str());
//					}
//				}
//			}
//			SDL_Log("code: %s", strCode.c_str());
//		}
//
//		::cheatsSaveCheatList("/sdcard/testcheat.txt");
//	}
//}

void usage(char *cmd)
{
  printf("%s [option ...] file\n", cmd);
  printf("\
\n\
Options:\n\
  -1, --video-1x               1x\n\
  -2, --video-2x               2x\n\
  -3, --video-3x               3x\n\
  -4, --video-4x               4x\n\
  -F, --fullscreen             Full screen\n\
  -G, --gdb=PROTOCOL           GNU Remote Stub mode:\n\
                                tcp      - use TCP at port 55555\n\
                                tcp:PORT - use TCP at port PORT\n\
                                pipe     - use pipe transport\n\
  -N, --no-debug               Don't parse debug information\n\
  -S, --flash-size=SIZE        Set the Flash size\n\
      --flash-64k               0 -  64K Flash\n\
      --flash-128k              1 - 128K Flash\n\
  -T, --throttle=THROTTLE      Set the desired throttle (5...1000)\n\
  -Y, --yuv=TYPE               Use YUV overlay for drawing:\n\
                                0 - YV12\n\
                                1 - UYVY\n\
                                2 - YVYU\n\
                                3 - YUY2\n\
                                4 - IYUV\n\
  -b, --bios=BIOS              Use given bios file\n\
  -c, --config=FILE            Read the given configuration file\n\
  -d, --debug                  Enter debugger\n\
  -f, --filter=FILTER          Select filter:\n\
      --filter-normal            0 - normal mode\n\
      --filter-tv-mode           1 - TV Mode\n\
      --filter-2xsai             2 - 2xSaI\n\
      --filter-super-2xsai       3 - Super 2xSaI\n\
      --filter-super-eagle       4 - Super Eagle\n\
      --filter-pixelate          5 - Pixelate\n\
      --filter-motion-blur       6 - Motion Blur\n\
      --filter-advmame           7 - AdvanceMAME Scale2x\n\
      --filter-simple2x          8 - Simple2x\n\
      --filter-bilinear          9 - Bilinear\n\
      --filter-bilinear+        10 - Bilinear Plus\n\
      --filter-scanlines        11 - Scanlines\n\
      --filter-hq2x             12 - hq2x\n\
      --filter-lq2x             13 - lq2x\n\
  -h, --help                   Print this help\n\
  -i, --ips=PATCH              Apply given IPS patch\n\
  -p, --profile=[HERTZ]        Enable profiling\n\
  -s, --frameskip=FRAMESKIP    Set frame skip (0...9)\n\
");
  printf("\
  -t, --save-type=TYPE         Set the available save type\n\
      --save-auto               0 - Automatic (EEPROM, SRAM, FLASH)\n\
      --save-eeprom             1 - EEPROM\n\
      --save-sram               2 - SRAM\n\
      --save-flash              3 - FLASH\n\
      --save-sensor             4 - EEPROM+Sensor\n\
      --save-none               5 - NONE\n\
  -v, --verbose=VERBOSE        Set verbose logging (trace.log)\n\
                                  1 - SWI\n\
                                  2 - Unaligned memory access\n\
                                  4 - Illegal memory write\n\
                                  8 - Illegal memory read\n\
                                 16 - DMA 0\n\
                                 32 - DMA 1\n\
                                 64 - DMA 2\n\
                                128 - DMA 3\n\
                                256 - Undefined instruction\n\
                                512 - AGBPrint messages\n\
\n\
Long options only:\n\
      --agb-print              Enable AGBPrint support\n\
      --auto-frameskip         Enable auto frameskipping\n\
      --ifb-none               No interframe blending\n\
      --ifb-motion-blur        Interframe motion blur\n\
      --ifb-smart              Smart interframe blending\n\
      --no-agb-print           Disable AGBPrint support\n\
      --no-auto-frameskip      Disable auto frameskipping\n\
      --no-ips                 Do not apply IPS patch\n\
      --no-mmx                 Disable MMX support\n\
      --no-pause-when-inactive Don't pause when inactive\n\
      --no-rtc                 Disable RTC support\n\
      --no-show-speed          Don't show emulation speed\n\
      --no-throttle            Disable thrrotle\n\
      --pause-when-inactive    Pause when inactive\n\
      --rtc                    Enable RTC support\n\
      --show-speed-normal      Show emulation speed\n\
      --show-speed-detailed    Show detailed speed data\n\
");
}

int main(int argc, char **argv)
{
	  density = SDL_GetDensity();
	  directionHalfSize = 100 * density;
	  int x1, y1;
	  int x2, y2;
	  x1 = directionHalfSize * cos(0);
	  y1 = directionHalfSize * sin(0);
	  x2 = directionHalfSize * cos(30.0f / 180.0f * M_PI);
	  y2 = directionHalfSize * sin(30.0f / 180.0f * M_PI);
	  l30 = squareLength(x1, y1, x2, y2);
	  x2 = directionHalfSize * cos(60.0f / 180.0f * M_PI);
	  y2 = directionHalfSize * sin(60.0f / 180.0f * M_PI);
	  l60 = squareLength(x1, y1, x2, y2);
	  x2 = directionHalfSize * cos(120.0f / 180.0f * M_PI);
	  y2 = directionHalfSize * sin(120.0f / 180.0f * M_PI);
	  l120 = squareLength(x1, y1, x2, y2);
	  x2 = directionHalfSize * cos(150.0f / 180.0f * M_PI);
	  y2 = directionHalfSize * sin(150.0f / 180.0f * M_PI);
	  l150 = squareLength(x1, y1, x2, y2);

  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "VisualBoyAdvance version %s [SDL]\n", VERSION);
  argc = 2;
  char* hackargv[] = {"SDL_app", SDL_GetRomFile()};
  argv = hackargv;

  arg0 = argv[0];
  
  captureDir[0] = 0;
  saveDir[0] = 0;
  batteryDir[0] = 0;
  ipsname[0] = 0;
  
  int op = -1;

  frameSkip = 2;
  gbBorderOn = 0;

  parseDebug = true;

  sdlReadPreferences();

  sdlPrintUsage = 0;
  
  while((op = getopt_long(argc,
                          argv,
                          "FNT:Y:G:D:b:c:df:hi:p::s:t:v:1234",
                          sdlOptions,
                          NULL)) != -1) {
    switch(op) {
    case 0:
      // long option already processed by getopt_long
      break;
    case 'b':
      useBios = true;
      if(optarg == NULL) {
        fprintf(stderr, "Missing BIOS file name\n");
        exit(-1);
      }
      strcpy(biosFileName, optarg);
      break;
    case 'c':
      {
        if(optarg == NULL) {
          fprintf(stderr, "Missing config file name\n");
          exit(-1);
        }
        FILE *f = fopen(optarg, "r");
        if(f == NULL) {
          fprintf(stderr, "File not found %s\n", optarg);
          exit(-1);
        }
        sdlReadPreferences(f);
        fclose(f);
      }
      break;
    case 'd':
      debugger = true;
      break;
    case 'h':
      sdlPrintUsage = 1;
      break;
    case 'i':
      if(optarg == NULL) {
        fprintf(stderr, "Missing IPS name\n");
        exit(-1);
        strcpy(ipsname, optarg);
      }
      break;
//    case 'Y':
//      yuv = true;
//      if(optarg) {
//        yuvType = atoi(optarg);
//        switch(yuvType) {
//        case 0:
////          yuvType = SDL_YV12_OVERLAY;
//          break;
//        case 1:
////          yuvType = SDL_UYVY_OVERLAY;
//          break;
//        case 2:
////          yuvType = SDL_YVYU_OVERLAY;
//          break;
//        case 3:
////          yuvType = SDL_YUY2_OVERLAY;
//          break;
//        case 4:
////          yuvType = SDL_IYUV_OVERLAY;
//          break;
//        default:
////          yuvType = SDL_YV12_OVERLAY;
//        }
//      } else
////        yuvType = SDL_YV12_OVERLAY;
//      break;
    case 'G':
      dbgMain = remoteStubMain;
      dbgSignal = remoteStubSignal;
      dbgOutput = remoteOutput;
      debugger = true;
      debuggerStub = true;
      if(optarg) {
        char *s = optarg;
        if(strncmp(s,"tcp:", 4) == 0) {
          s+=4;
          int port = atoi(s);
          remoteSetProtocol(0);
          remoteSetPort(port);
        } else if(strcmp(s,"tcp") == 0) {
          remoteSetProtocol(0);
        } else if(strcmp(s, "pipe") == 0) {
          remoteSetProtocol(1);
        } else {
          fprintf(stderr, "Unknown protocol %s\n", s);
          exit(-1);
        }
      } else {
        remoteSetProtocol(0);
      }
      break;
    case 'N':
      parseDebug = false;
      break;
    case 'D':
      if(optarg) {
        systemDebug = atoi(optarg);
      } else {
        systemDebug = 1;
      }
      break;
    case 'F':
      fullscreen = 1;
      mouseCounter = 120;
      break;
    case 'f':
      if(optarg) {
        filter = atoi(optarg);
      } else {
        filter = 0;
      }
      break;
    case 'p':
#ifdef PROFILING
      if(optarg) {
        cpuEnableProfiling(atoi(optarg));
      } else
        cpuEnableProfiling(100);
#endif
      break;
    case 'S':
      sdlFlashSize = atoi(optarg);
      if(sdlFlashSize < 0 || sdlFlashSize > 1)
        sdlFlashSize = 0;
      break;
    case 's':
      if(optarg) {
        int a = atoi(optarg);
        if(a >= 0 && a <= 9) {
          gbFrameSkip = a;
          frameSkip = a;
        }
      } else {
        frameSkip = 2;
        gbFrameSkip = 0;
      }
      break;
    case 't':
      if(optarg) {
        int a = atoi(optarg);
        if(a < 0 || a > 5)
          a = 0;
        cpuSaveType = a;
      }
      break;
    case 'T':
      if(optarg) {
        int t = atoi(optarg);
        if(t < 5 || t > 1000)
          t = 0;
        throttle = t;
      }
      break;
    case 'v':
      if(optarg) {
        systemVerbose = atoi(optarg);
      } else 
        systemVerbose = 0;
      break;
    case '1':
      sizeOption = 0;
      break;
    case '2':
      sizeOption = 1;
      break;
    case '3':
      sizeOption = 2;
      break;
    case '4':
      sizeOption = 3;
      break;
    case '?':
      sdlPrintUsage = 1;
      break;
    }
  }

  if(sdlPrintUsage) {
    usage(argv[0]);
    exit(-1);
  }

#ifdef MMX
  if(disableMMX)
    cpu_mmx = 0;
#endif

  if(rewindTimer)
    rewindMemory = (char *)malloc(8*REWIND_SIZE);

//  if(sdlFlashSize == 0)
//    flashSetSize(0x10000);
//  else
    flashSetSize(0x20000);

  rtcEnable(sdlRtcEnable ? true : false);
  agbPrintEnable(sdlAgbPrint ? true : false);
  
  if(!debuggerStub) {
    if(optind >= argc) {
      systemMessage(0,"Missing image name");
      usage(argv[0]);
      exit(-1);
    }
  }

  if(filter) {
    sizeOption = 1;
  }

  for(int i = 0; i < 24;) {
    systemGbPalette[i++] = (0x1f) | (0x1f << 5) | (0x1f << 10);
    systemGbPalette[i++] = (0x15) | (0x15 << 5) | (0x15 << 10);
    systemGbPalette[i++] = (0x0c) | (0x0c << 5) | (0x0c << 10);
    systemGbPalette[i++] = 0;
  }

  systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;

  if(optind < argc) {
    char *szFile = argv[optind];

    utilGetBaseName(szFile, filename);
    char *p = strrchr(filename, '.');

    if(p)
      *p = 0;

    if(ipsname[0] == 0)
      sprintf(ipsname, "%s.ips", filename);
    
    bool failed = false;

    IMAGE_TYPE type = utilFindType(szFile);

    if(type == IMAGE_UNKNOWN) {
      systemMessage(0, "Unknown file type %s", szFile);
      exit(-1);
    }
    cartridgeType = (int)type;
    
    if(type == IMAGE_GB) {
      failed = !gbLoadRom(szFile);
      if(!failed) {
        cartridgeType = 1;
        emulator = GBSystem;
        if(sdlAutoIPS) {
          int size = gbRomSize;
          utilApplyIPS(ipsname, &gbRom, &size);
          if(size != gbRomSize) {
            extern bool gbUpdateSizes();
            gbUpdateSizes();
            gbReset();
          }
        }
      }
    } else if(type == IMAGE_GBA) {
      int size = CPULoadRom(szFile);
      failed = (size == 0);
      if(!failed) {
        //        if(cpuEnhancedDetection && cpuSaveType == 0) {
        //          utilGBAFindSave(rom, size);
        //        }

        sdlApplyPerImagePreferences();
        
        cartridgeType = 0;
        emulator = GBASystem;

        /* disabled due to problems
        if(removeIntros && rom != NULL) {
          WRITE32LE(&rom[0], 0xea00002e);
        }
        */
        
        CPUInit(biosFileName, useBios);
        CPUReset();
        if(sdlAutoIPS) {
          int size = 0x2000000;
          utilApplyIPS(ipsname, &rom, &size);
          if(size != 0x2000000) {
            CPUReset();
          }
        }
      }
    }
    
    if(failed) {
      systemMessage(0, "Failed to load file %s", szFile);
      exit(-1);
    }
  } else {
    cartridgeType = 0;
    strcpy(filename, "gnu_stub");
    rom = (u8 *)malloc(0x2000000);
    workRAM = (u8 *)calloc(1, 0x40000);
    bios = (u8 *)calloc(1,0x4000);
    internalRAM = (u8 *)calloc(1,0x8000);
    paletteRAM = (u8 *)calloc(1,0x400);
    vram = (u8 *)calloc(1, 0x20000);
    oam = (u8 *)calloc(1, 0x400);
    pix = (u8 *)calloc(1, 4 * 240 * 160);
    ioMem = (u8 *)calloc(1, 0x400);

    emulator = GBASystem;
    
    CPUInit(biosFileName, useBios);
    CPUReset();
  }
  
  sdlReadBattery();
  
  if(debuggerStub) 
    remoteInit();
  
  int flags = SDL_INIT_VIDEO|SDL_INIT_AUDIO|
    SDL_INIT_TIMER|SDL_INIT_NOPARACHUTE;

  if(soundOffFlag)
    flags ^= SDL_INIT_AUDIO;
  
  if(SDL_Init(flags)) {
    systemMessage(0, "Failed to init SDL: %s", SDL_GetError());
    exit(-1);
  }

  if(SDL_InitSubSystem(SDL_INIT_JOYSTICK)) {
    systemMessage(0, "Failed to init joystick support: %s", SDL_GetError());
  }
  
  sdlCheckKeys();
  
  if(cartridgeType == 0) {
    srcWidth = 240;
    srcHeight = 160;
    systemFrameSkip = frameSkip;
  } else if (cartridgeType == 1) {
    if(gbBorderOn) {
      srcWidth = 256;
      srcHeight = 224;
      gbBorderLineSkip = 256;
      gbBorderColumnSkip = 48;
      gbBorderRowSkip = 40;
    } else {      
      srcWidth = 160;
      srcHeight = 144;
      gbBorderLineSkip = 160;
      gbBorderColumnSkip = 0;
      gbBorderRowSkip = 0;
    }
    systemFrameSkip = gbFrameSkip;
  } else {
    srcWidth = 320;
    srcHeight = 240;
  }
  
  destWidth = (sizeOption+1)*srcWidth;
  destHeight = (sizeOption+1)*srcHeight;
  
//  surface = SDL_SetVideoMode(destWidth, destHeight, 16,
//                             SDL_ANYFORMAT|SDL_HWSURFACE|SDL_DOUBLEBUF|
//                             (fullscreen ? SDL_FULLSCREEN : 0));

  window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, destWidth, destHeight, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

    surface = SDL_CreateRGBSurface(0, destWidth, destHeight, 16, 0xf800, 0x7e0, 0x1f, 0);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STREAMING, destWidth, destHeight);
  
    SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "linear");

  if(surface == NULL) {
    systemMessage(0, "Failed to set video mode");
    SDL_Quit();
    exit(-1);
  }
  
  SDL_GetWindowSize(window, &windowWidth, &windowHeight);

  int imgFlags = IMG_INIT_PNG;
  if( !( IMG_Init( imgFlags ) & imgFlags ) )
  {
	  return 0;
  }

  //directionSurface = IMG_Load( "direction.png" );
  directionTexture = ::IMG_LoadTexture(renderer, "direction.png");
  ::SDL_SetTextureAlphaMod(directionTexture, 127);
  upTexture = ::IMG_LoadTexture(renderer, "up.png");
  ::SDL_SetTextureAlphaMod(upTexture, 127);
  downTexture = ::IMG_LoadTexture(renderer, "down.png");
  ::SDL_SetTextureAlphaMod(downTexture, 127);
  leftTexture = ::IMG_LoadTexture(renderer, "left.png");
  ::SDL_SetTextureAlphaMod(leftTexture, 127);
  rightTexture = ::IMG_LoadTexture(renderer, "right.png"); SDL_SetTextureAlphaMod(rightTexture, 127);
  leftupTexture = ::IMG_LoadTexture(renderer, "leftup.png"); SDL_SetTextureAlphaMod(leftupTexture, 127);
  rightupTexture = ::IMG_LoadTexture(renderer, "rightup.png"); SDL_SetTextureAlphaMod(rightupTexture, 127);
  leftdownTexture = ::IMG_LoadTexture(renderer, "leftdown.png"); SDL_SetTextureAlphaMod(leftdownTexture, 127);
  rightdownTexture = ::IMG_LoadTexture(renderer, "rightdown.png"); SDL_SetTextureAlphaMod(rightdownTexture, 127);
  abTexture = ::IMG_LoadTexture(renderer, "AB.png");SDL_SetTextureAlphaMod(abTexture, 127);
  ab2Texture = IMG_LoadTexture(renderer, "AB2.png"); SDL_SetTextureAlphaMod(ab2Texture, 127);
  aTexture = ::IMG_LoadTexture(renderer, "A2.png"); SDL_SetTextureAlphaMod(aTexture, 127);
  bTexture = ::IMG_LoadTexture(renderer, "B2.png"); SDL_SetTextureAlphaMod(bTexture, 127);
  startTexture = IMG_LoadTexture(renderer, "start.png"); SDL_SetTextureAlphaMod(startTexture, 127);
  start2Texture = IMG_LoadTexture(renderer, "start2.png"); SDL_SetTextureAlphaMod(start2Texture, 127);
  selectTexture = IMG_LoadTexture(renderer, "select.png"); SDL_SetTextureAlphaMod(selectTexture, 127);
  select2Texture = IMG_LoadTexture(renderer, "select2.png"); SDL_SetTextureAlphaMod(select2Texture, 127);
  left2Texture = IMG_LoadTexture(renderer, "left2.png"); SDL_SetTextureAlphaMod(left2Texture, 127);
  left2_2Texture = IMG_LoadTexture(renderer, "left2_2.png"); SDL_SetTextureAlphaMod(left2_2Texture, 127);
  right2Texture = IMG_LoadTexture(renderer, "right2.png"); SDL_SetTextureAlphaMod(right2Texture, 127);
  right2_2Texture = IMG_LoadTexture(renderer, "right2_2.png"); SDL_SetTextureAlphaMod(right2_2Texture, 127);
  saveTexture = IMG_LoadTexture(renderer, "save.png"); SDL_SetTextureAlphaMod(saveTexture, 127);
  save2Texture = IMG_LoadTexture(renderer, "save2.png"); SDL_SetTextureAlphaMod(save2Texture, 127);
  loadTexture = IMG_LoadTexture(renderer, "load.png"); SDL_SetTextureAlphaMod(loadTexture, 127);
  load2Texture = IMG_LoadTexture(renderer, "load2.png"); SDL_SetTextureAlphaMod(load2Texture, 127);
  fastforwardTexture = IMG_LoadTexture(renderer, "fastforward.png"); SDL_SetTextureAlphaMod(fastforwardTexture, 127);
  fastforward2Texture = IMG_LoadTexture(renderer, "fastforward2.png"); SDL_SetTextureAlphaMod(fastforward2Texture, 127);
  resetTexture = IMG_LoadTexture(renderer, "reset.png"); SDL_SetTextureAlphaMod(resetTexture, 127);
  reset2Texture = IMG_LoadTexture(renderer, "reset2.png"); SDL_SetTextureAlphaMod(reset2Texture, 127);
  cheatTexture = IMG_LoadTexture(renderer, "cheat.png"); SDL_SetTextureAlphaMod(cheatTexture, 127);
  cheat2Texture = IMG_LoadTexture(renderer, "cheat2.png"); SDL_SetTextureAlphaMod(cheat2Texture, 127);

  systemRedShift = sdlCalculateShift(surface->format->Rmask);
  systemGreenShift = sdlCalculateShift(surface->format->Gmask);
  systemBlueShift = sdlCalculateShift(surface->format->Bmask);
  
  systemColorDepth = surface->format->BitsPerPixel;
  if(systemColorDepth == 15)
    systemColorDepth = 16;

  if(yuv) {
    Init_Overlay(surface, yuvType);
    systemColorDepth = 32;
    systemRedShift = 3;
    systemGreenShift = 11;
    systemBlueShift =  19;
  }
  
  if(systemColorDepth != 16 && systemColorDepth != 24 &&
     systemColorDepth != 32) {
    fprintf(stderr,"Unsupported color depth '%d'.\nOnly 16, 24 and 32 bit color depths are supported\n", systemColorDepth);
    exit(-1);
  }

#ifndef C_CORE
  sdlMakeStretcher(srcWidth);
#else
  switch(systemColorDepth) {
  case 16:
    sdlStretcher = sdlStretcher16[sizeOption];
    break;
  case 24:
    sdlStretcher = sdlStretcher24[sizeOption];
    break;
  case 32:
    sdlStretcher = sdlStretcher32[sizeOption];
    break;
  default:
    fprintf(stderr, "Unsupported resolution: %d\n", systemColorDepth);
    exit(-1);
  }
#endif

  fprintf(stderr,"Color depth: %d\n", systemColorDepth);
  
  if(systemColorDepth == 16) {
    if(sdlCalculateMaskWidth(surface->format->Gmask) == 6) {
      Init_2xSaI(565);
      RGB_LOW_BITS_MASK = 0x821;
    } else {
      Init_2xSaI(555);
      RGB_LOW_BITS_MASK = 0x421;      
    }
    if(cartridgeType == 2) {
      for(int i = 0; i < 0x10000; i++) {
        systemColorMap16[i] = (((i >> 1) & 0x1f) << systemBlueShift) |
          (((i & 0x7c0) >> 6) << systemGreenShift) |
          (((i & 0xf800) >> 11) << systemRedShift);  
      }      
    } else {
      for(int i = 0; i < 0x10000; i++) {
        systemColorMap16[i] = ((i & 0x1f) << systemRedShift) |
          (((i & 0x3e0) >> 5) << systemGreenShift) |
          (((i & 0x7c00) >> 10) << systemBlueShift);  
      }
    }
    srcPitch = srcWidth * 2+4;
  } else {
    if(systemColorDepth != 32)
      filterFunction = NULL;
    RGB_LOW_BITS_MASK = 0x010101;
    if(systemColorDepth == 32) {
      Init_2xSaI(32);
    }
    for(int i = 0; i < 0x10000; i++) {
      systemColorMap32[i] = ((i & 0x1f) << systemRedShift) |
        (((i & 0x3e0) >> 5) << systemGreenShift) |
        (((i & 0x7c00) >> 10) << systemBlueShift);  
    }
    if(systemColorDepth == 32)
      srcPitch = srcWidth*4 + 4;
    else
      srcPitch = srcWidth*3;
  }

  if(systemColorDepth != 32) {
    switch(filter) {
    case 0:
      filterFunction = NULL;
      break;
    case 1:
      filterFunction = ScanlinesTV;
      break;
    case 2:
      filterFunction = _2xSaI;
      break;
    case 3:
      filterFunction = Super2xSaI;
      break;
    case 4:
      filterFunction = SuperEagle;
      break;
    case 5:
      filterFunction = Pixelate;
      break;
    case 6:
      filterFunction = MotionBlur;
      break;
    case 7:
      filterFunction = AdMame2x;
      break;
    case 8:
      filterFunction = Simple2x;
      break;
    case 9:
      filterFunction = Bilinear;
      break;
    case 10:
      filterFunction = BilinearPlus;
      break;
    case 11:
      filterFunction = Scanlines;
      break;
    case 12:
      filterFunction = hq2x;
      break;
    case 13:
      filterFunction = lq2x;
      break;
    default:
      filterFunction = NULL;
      break;
    }
  } else {
    switch(filter) {
    case 0:
      filterFunction = NULL;
      break;
    case 1:
      filterFunction = ScanlinesTV32;
      break;
    case 2:
      filterFunction = _2xSaI32;
      break;
    case 3:
      filterFunction = Super2xSaI32;
      break;
    case 4:
      filterFunction = SuperEagle32;
      break;
    case 5:
      filterFunction = Pixelate32;
      break;
    case 6:
      filterFunction = MotionBlur32;
      break;
    case 7:
      filterFunction = AdMame2x32;
      break;
    case 8:
      filterFunction = Simple2x32;
      break;
    case 9:
      filterFunction = Bilinear32;
      break;
    case 10:
      filterFunction = BilinearPlus32;
      break;
    case 11:
      filterFunction = Scanlines32;
      break;
    case 12:
      filterFunction = hq2x32;
      break;
    case 13:
      filterFunction = lq2x32;
      break;
    default:
      filterFunction = NULL;
      break;
    }
  }
  
  if(systemColorDepth == 16) {
    switch(ifbType) {
    case 0:
    default:
      ifbFunction = NULL;
      break;
    case 1:
      ifbFunction = MotionBlurIB;
      break;
    case 2:
      ifbFunction = SmartIB;
      break;
    }
  } else if(systemColorDepth == 32) {
    switch(ifbType) {
    case 0:
    default:
      ifbFunction = NULL;
      break;
    case 1:
      ifbFunction = MotionBlurIB32;
      break;
    case 2:
      ifbFunction = SmartIB32;
      break;
    }
  } else
    ifbFunction = NULL;

  if(delta == NULL) {
    delta = (u8*)malloc(322*242*4);
    memset(delta, 255, 322*242*4);
  }
  
  emulating = 1;
  renderedFrames = 0;

  if(!soundOffFlag)
    soundInit();

  autoFrameSkipLastTime = throttleLastTime = systemGetClock();
  
//  SDL_WM_SetCaption("VisualBoyAdvance", NULL);

	if(emulator.emuReadState)
	{
		std::string strAutoSaveState = SDL_GetRomFile();
		strAutoSaveState += ".sgm";
		emulator.emuReadState(strAutoSaveState.c_str());
	}

	::cheatsDeleteAll(false);
	//cheatsLoadCheatList("/sdcard/a.clt");


  while(emulating) {
    if(!paused && active) {
      if(debugger && emulator.emuHasDebugger)
        dbgMain();
      else {

    	if(userWantReset)
    	{
    		emulator.emuReset();
    		userWantReset = false;
    	}

        emulator.emuMain(emulator.emuCount);
        if(rewindSaveNeeded && rewindMemory && emulator.emuWriteMemState) {
          rewindCount++;
          if(rewindCount > 8)
            rewindCount = 8;
          if(emulator.emuWriteMemState &&
             emulator.emuWriteMemState(&rewindMemory[rewindPos*REWIND_SIZE], 
                                       REWIND_SIZE)) {
            rewindPos = ++rewindPos & 7;
            if(rewindCount == 8)
              rewindTopPos = ++rewindTopPos & 7;
          }
        }

        rewindSaveNeeded = false;
      }
    } else {
      SDL_Delay(500);
    }
    sdlPollEvents();
    if(mouseCounter) {
      mouseCounter--;
      if(mouseCounter == 0)
        SDL_ShowCursor(SDL_DISABLE);
    }
  }

	if(emulator.emuWriteState)
	{
		std::string strAutoSaveState = SDL_GetRomFile();
		strAutoSaveState += ".sgm";
		emulator.emuWriteState(strAutoSaveState.c_str());
	}
  
  emulating = 0;
  fprintf(stderr,"Shutting down\n");
  remoteCleanUp();
  soundShutdown();

  if(gbRom != NULL || rom != NULL) {
    sdlWriteBattery();
    emulator.emuCleanUp();
  }

  if(delta) {
    free(delta);
    delta = NULL;
  }
  
  SDL_Quit();
  ::SDL_FinishActivity();
  return 0;
}

void systemMessage(int num, const char *msg, ...)
{
  char buffer[2048];
  va_list valist;
  
  va_start(valist, msg);
  vsprintf(buffer, msg, valist);
  
  fprintf(stderr, "%s\n", buffer);
  va_end(valist);
}

void systemDrawScreen()
{
  renderedFrames++;
  
//  if(yuv) {
//    Draw_Overlay(surface, sizeOption+1);
//    return;
//  }
  
  SDL_LockSurface(surface);

  if(screenMessage) {
    if(cartridgeType == 1 && gbBorderOn) {
      gbSgbRenderBorder();
    }
    if(((systemGetClock() - screenMessageTime) < 3000) &&
       !disableStatusMessages) {
      drawText(pix, srcPitch, 10, srcHeight - 20,
               screenMessageBuffer); 
    } else {
      screenMessage = false;
    }
  }

  if(ifbFunction) {
    if(systemColorDepth == 16)
      ifbFunction(pix+destWidth+4, destWidth+4, srcWidth, srcHeight);
    else
      ifbFunction(pix+destWidth*2+4, destWidth*2+4, srcWidth, srcHeight);
  }
  
  if(filterFunction) {
    if(systemColorDepth == 16)
      filterFunction(pix+destWidth+4,destWidth+4, delta,
                     (u8*)surface->pixels,surface->pitch,
                     srcWidth,
                     srcHeight);
    else
      filterFunction(pix+destWidth*2+4,
                     destWidth*2+4,
                     delta,
                     (u8*)surface->pixels,
                     surface->pitch,
                     srcWidth,
                     srcHeight);
  } else {
    int destPitch = surface->pitch;
    u8 *src = pix;
    u8 *dest = (u8*)surface->pixels;
    int i;
    u32 *stretcher = (u32 *)sdlStretcher;
    if(systemColorDepth == 16)
      src += srcPitch;
    int option = sizeOption;
    if(yuv)
      option = 0;
    switch(sizeOption) {
    case 0:
      for(i = 0; i < srcHeight; i++) {
        SDL_CALL_STRETCHER;
        src += srcPitch;
        dest += destPitch;
      }
      break;
    case 1:
      for(i = 0; i < srcHeight; i++) {
        SDL_CALL_STRETCHER;     
        dest += destPitch;
        SDL_CALL_STRETCHER;
        src += srcPitch;
        dest += destPitch;
      }
      break;
    case 2:
      for(i = 0; i < srcHeight; i++) {
        SDL_CALL_STRETCHER;
        dest += destPitch;
        SDL_CALL_STRETCHER;
        dest += destPitch;
        SDL_CALL_STRETCHER;
        src += srcPitch;
        dest += destPitch;
      }
      break;
    case 3:
      for(i = 0; i < srcHeight; i++) {
        SDL_CALL_STRETCHER;
        dest += destPitch;
        SDL_CALL_STRETCHER;
        dest += destPitch;
        SDL_CALL_STRETCHER;
        dest += destPitch;
        SDL_CALL_STRETCHER;
        src += srcPitch;
        dest += destPitch;
      }
      break;
    }
  }

  if(showSpeed && fullscreen) {
    char buffer[50];
    if(showSpeed == 1)
      sprintf(buffer, "%d%%", systemSpeed);
    else
      sprintf(buffer, "%3d%%(%d, %d fps)", systemSpeed,
              systemFrameSkip,
              showRenderedFrames);
    if(showSpeedTransparent)
      drawTextTransp((u8*)surface->pixels,
                     surface->pitch,
                     10,
                     surface->h-20,
                     buffer);
    else
      drawText((u8*)surface->pixels,
               surface->pitch,
               10,
               surface->h-20,
               buffer);        
  }  

  SDL_UnlockSurface(surface);
  //  SDL_UpdateRect(surface, 0, 0, destWidth, destHeight);
//  SDL_Flip(surface);
  SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
  //SDL_UpdateTexture(texture, NULL, directionSurface->pixels, directionSurface->pitch);
  SDL_RenderClear(renderer);
  SDL_GetWindowSize(window, &windowWidth, &windowHeight);
  int width = windowWidth;
  int height = width * 160 / 240;
  if(height > windowHeight)
  {
	  height = windowHeight;
	  width = height * 240 / 160;
  }
  SDL_Rect rect = {(windowWidth - width) / 2, 0, width, height};
  SDL_RenderCopy(renderer, texture, NULL, &rect);

  {
	  int newDirectionHalfSize = 100 * density;
	  if((newDirectionHalfSize * 2) > (windowWidth - 160 * density))
	  {
		  newDirectionHalfSize = (windowWidth - 160 * density) / 2;
	  }
	  if(newDirectionHalfSize != directionHalfSize)
	  {
		  directionHalfSize = newDirectionHalfSize;
		  int x1, y1;
		  int x2, y2;
		  x1 = directionHalfSize * cos(0);
		  y1 = directionHalfSize * sin(0);
		  x2 = directionHalfSize * cos(30.0f / 180.0f * M_PI);
		  y2 = directionHalfSize * sin(30.0f / 180.0f * M_PI);
		  l30 = squareLength(x1, y1, x2, y2);
		  x2 = directionHalfSize * cos(60.0f / 180.0f * M_PI);
		  y2 = directionHalfSize * sin(60.0f / 180.0f * M_PI);
		  l60 = squareLength(x1, y1, x2, y2);
		  x2 = directionHalfSize * cos(120.0f / 180.0f * M_PI);
		  y2 = directionHalfSize * sin(120.0f / 180.0f * M_PI);
		  l120 = squareLength(x1, y1, x2, y2);
		  x2 = directionHalfSize * cos(150.0f / 180.0f * M_PI);
		  y2 = directionHalfSize * sin(150.0f / 180.0f * M_PI);
		  l150 = squareLength(x1, y1, x2, y2);
	  }
  }

  directionX = directionHalfSize;
  directionY = windowHeight - directionHalfSize;

  rect.x = directionX - directionHalfSize;
  rect.y = directionY - directionHalfSize;
  rect.w = directionHalfSize * 2;
  rect.h = directionHalfSize * 2;
  switch(directionStatus)
  {
  case UP:
	  SDL_RenderCopy(renderer, upTexture, NULL, &rect);
	  break;
  case DOWN:
	  SDL_RenderCopy(renderer, downTexture, NULL, &rect);
	  break;
  case LEFT:
  	  SDL_RenderCopy(renderer, leftTexture, NULL, &rect);
  	  break;
  case RIGHT:
  	  SDL_RenderCopy(renderer, rightTexture, NULL, &rect);
  	  break;
  case LEFTUP:
  	  SDL_RenderCopy(renderer, leftupTexture, NULL, &rect);
  	  break;
  case RIGHTUP:
  	  SDL_RenderCopy(renderer, rightupTexture, NULL, &rect);
  	  break;
  case LEFTDOWN:
  	  SDL_RenderCopy(renderer, leftdownTexture, NULL, &rect);
  	  break;
  case RIGHTDOWN:
  	  SDL_RenderCopy(renderer, rightdownTexture, NULL, &rect);
  	  break;
  default:
	  SDL_RenderCopy(renderer, directionTexture, NULL, &rect);
	  break;
  }

  rect.x = windowWidth - 160 * density;
  rect.y = windowHeight - 80 * density;
  rect.w = 160 * density;
  rect.h = 80 * density;
  if(sdlButtons[0][4] && sdlButtons[0][5])
	  SDL_RenderCopy(renderer, ab2Texture, NULL, &rect);
  else if(sdlButtons[0][4])
	  SDL_RenderCopy(renderer, aTexture, NULL, &rect);
  else if(sdlButtons[0][5])
	  SDL_RenderCopy(renderer, bTexture, NULL, &rect);
  else
	  SDL_RenderCopy(renderer, abTexture, NULL, &rect);

  rect.x = windowWidth - 80 * density;
  rect.y = windowHeight - 160 * density;
  rect.w = 80 * density;
  rect.h = 40 * density;
  if(sdlButtons[0][6])
	  SDL_RenderCopy(renderer, start2Texture, NULL, &rect);
  else
	  SDL_RenderCopy(renderer, startTexture, NULL, &rect);

  rect.x = windowWidth - 160 * density;
  rect.y = windowHeight - 160 * density;
  rect.w = 80 * density;
  rect.h = 40 * density;
  if(sdlButtons[0][7])
	  SDL_RenderCopy(renderer, select2Texture, NULL, &rect);
  else
	  SDL_RenderCopy(renderer, selectTexture, NULL, &rect);

  rect.x = 0;
  rect.y = windowHeight - 240 * density;
  rect.w = 80 * density;
  rect.h = 40 * density;
  if(sdlButtons[0][8])
	  SDL_RenderCopy(renderer, left2_2Texture, NULL, &rect);
  else
	  SDL_RenderCopy(renderer, left2Texture, NULL, &rect);

  rect.x = windowWidth - 80 * density;
  rect.y = windowHeight - 120 * density;
  rect.w = 80 * density;
  rect.h = 40 * density;
  if(sdlButtons[0][9])
	  SDL_RenderCopy(renderer, right2_2Texture, NULL, &rect);
  else
	  SDL_RenderCopy(renderer, right2Texture, NULL, &rect);

  rect.x = 0;
  rect.y = 0;
  rect.w = 80 * density;
  rect.h = 40 * density;
  if(loadFingerId != -1)
	  SDL_RenderCopy(renderer, load2Texture, NULL, &rect);
  else
	  SDL_RenderCopy(renderer, loadTexture, NULL, &rect);

  rect.x = windowWidth - 80 * density;
  rect.y = 0;
  rect.w = 80 * density;
  rect.h = 40 * density;
  if(saveFingerId != -1)
	  SDL_RenderCopy(renderer, save2Texture, NULL, &rect);
  else
	  SDL_RenderCopy(renderer, saveTexture, NULL, &rect);

  rect.y = 40 * density;
  if(cheatFingerId != -1)
	  SDL_RenderCopy(renderer, cheat2Texture, NULL, &rect);
  else
	  SDL_RenderCopy(renderer, cheatTexture, NULL, &rect);

    rect.x = 80 * density;
    rect.y = windowHeight - 240 * density;
    rect.w = 80 * density;
    rect.h = 40 * density;
    if(sdlButtons[0][10])
  	  SDL_RenderCopy(renderer, fastforward2Texture, NULL, &rect);
    else
  	  SDL_RenderCopy(renderer, fastforwardTexture, NULL, &rect);

    rect.x = (windowWidth - 80 * density) / 2;
	rect.y = 0;
	rect.w = 80 * density;
	rect.h = 40 * density;
	if(resetFingerId != -1)
	  SDL_RenderCopy(renderer, reset2Texture, NULL, &rect);
	else
	  SDL_RenderCopy(renderer, resetTexture, NULL, &rect);

  SDL_RenderPresent(renderer);
}

bool systemReadJoypads()
{
  return true;
}

u32 systemReadJoypad(int which)
{
  if(which < 0 || which > 3)
    which = sdlDefaultJoypad;
  
  u32 res = 0;
  
  if(sdlButtons[which][KEY_BUTTON_A])
    res |= 1;
  if(sdlButtons[which][KEY_BUTTON_B])
    res |= 2;
  if(sdlButtons[which][KEY_BUTTON_SELECT])
    res |= 4;
  if(sdlButtons[which][KEY_BUTTON_START])
    res |= 8;
  if(sdlButtons[which][KEY_RIGHT])
    res |= 16;
  if(sdlButtons[which][KEY_LEFT])
    res |= 32;
  if(sdlButtons[which][KEY_UP])
    res |= 64;
  if(sdlButtons[which][KEY_DOWN])
    res |= 128;
  if(sdlButtons[which][KEY_BUTTON_R])
    res |= 256;
  if(sdlButtons[which][KEY_BUTTON_L])
    res |= 512;

  // disallow L+R or U+D of being pressed at the same time
  if((res & 48) == 48)
    res &= ~16;
  if((res & 192) == 192)
    res &= ~128;

  if(sdlButtons[which][KEY_BUTTON_SPEED])
    res |= 1024;
  if(sdlButtons[which][KEY_BUTTON_CAPTURE])
    res |= 2048;

  if(autoFire) {
    res &= (~autoFire);
    if(autoFireToggle)
      res |= autoFire;
    autoFireToggle = !autoFireToggle;
  }
  
  return res;
}

void systemSetTitle(const char *title)
{
//  SDL_WM_SetCaption(title, NULL);
}

void systemShowSpeed(int speed)
{
  systemSpeed = speed;

  showRenderedFrames = renderedFrames;
  renderedFrames = 0;  

  if(!fullscreen && showSpeed) {
    char buffer[80];
    if(showSpeed == 1)
      sprintf(buffer, "VisualBoyAdvance-%3d%%", systemSpeed);
    else
      sprintf(buffer, "VisualBoyAdvance-%3d%%(%d, %d fps)", systemSpeed,
              systemFrameSkip,
              showRenderedFrames);

    systemSetTitle(buffer);
  }
}

void systemFrame()
{
}

void system10Frames(int rate)
{
  u32 time = systemGetClock();  
  if(!wasPaused && autoFrameSkip && !throttle) {
    u32 diff = time - autoFrameSkipLastTime;
    int speed = 100;

    if(diff)
      speed = (1000000/rate)/diff;
    
    if(speed >= 98) {
      frameskipadjust++;

      if(frameskipadjust >= 3) {
        frameskipadjust=0;
        if(systemFrameSkip > 0)
          systemFrameSkip--;
      }
    } else {
      if(speed  < 80)
        frameskipadjust -= (90 - speed)/5;
      else if(systemFrameSkip < 9)
        frameskipadjust--;

      if(frameskipadjust <= -2) {
        frameskipadjust += 2;
        if(systemFrameSkip < 9)
          systemFrameSkip++;
      }
    }    
  }
  if(!wasPaused && throttle) {
    if(!speedup) {
      u32 diff = time - throttleLastTime;
      
      int target = (1000000/(rate*throttle));
      int d = (target - diff);
      
      if(d > 0) {
        SDL_Delay(d);
      }
    }
    throttleLastTime = systemGetClock();
  }
  if(rewindMemory) {
    if(++rewindCounter >= rewindTimer) {
      rewindSaveNeeded = true;
      rewindCounter = 0;
    }
  }

  if(systemSaveUpdateCounter) {
    if(--systemSaveUpdateCounter <= SYSTEM_SAVE_NOT_UPDATED) {
      sdlWriteBattery();
      systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
    }
  }

  wasPaused = false;
  autoFrameSkipLastTime = time;
}

void systemScreenCapture(int a)
{
  char buffer[2048];

  if(captureFormat) {
    if(captureDir[0])
      sprintf(buffer, "%s/%s%02d.bmp", captureDir, sdlGetFilename(filename), a);
    else
      sprintf(buffer, "%s%02d.bmp", filename, a);

    emulator.emuWriteBMP(buffer);
  } else {
    if(captureDir[0])
      sprintf(buffer, "%s/%s%02d.png", captureDir, sdlGetFilename(filename), a);
    else
      sprintf(buffer, "%s%02d.png", filename, a);
    emulator.emuWritePNG(buffer);
  }

  systemScreenMessage("Screen capture");
}

void soundCallback(void *,u8 *stream,int len)
{
  if(!emulating)
    return;
  SDL_mutexP(mutex);
  //  printf("Locked mutex\n");
  if(!speedup && !throttle) {
    while(sdlSoundLen < 2048*2) {
      if(emulating)
        SDL_CondWait(cond, mutex);
      else 
        break;
    }
  }
  if(emulating) {
    //  printf("Copying data\n");
    memcpy(stream, sdlBuffer, len);
  }
  sdlSoundLen = 0;
  if(mutex)
    SDL_mutexV(mutex);
}

void systemWriteDataToSoundBuffer()
{
  if(SDL_GetAudioStatus() != SDL_AUDIO_PLAYING)
    SDL_PauseAudio(0);
  bool cont = true;
  while(cont && !speedup && !throttle) {
    SDL_mutexP(mutex);
    //    printf("Waiting for len < 2048 (speed up %d)\n", speedup);
    if(sdlSoundLen < 2048*2)
      cont = false;
    SDL_mutexV(mutex);
  }

  int len = soundBufferLen;
  int copied = 0;
  if((sdlSoundLen+len) >= 2048*2) {
    //    printf("Case 1\n");
    memcpy(&sdlBuffer[sdlSoundLen],soundFinalWave, 2048*2-sdlSoundLen);
    copied = 2048*2 - sdlSoundLen;
    sdlSoundLen = 2048*2;
    SDL_CondSignal(cond);
    cont = true;
    if(!speedup && !throttle) {
      while(cont) {
        SDL_mutexP(mutex);
        if(sdlSoundLen < 2048*2)
          cont = false;
        SDL_mutexV(mutex);
      }
      memcpy(&sdlBuffer[0],&(((u8 *)soundFinalWave)[copied]),
             soundBufferLen-copied);
      sdlSoundLen = soundBufferLen-copied;
    } else {
      memcpy(&sdlBuffer[0], &(((u8 *)soundFinalWave)[copied]), 
soundBufferLen);
    }
  } else {
    //    printf("case 2\n");
    memcpy(&sdlBuffer[sdlSoundLen], soundFinalWave, soundBufferLen);
    sdlSoundLen += soundBufferLen;
  }
}

bool systemSoundInit()
{
  SDL_AudioSpec audio;

  switch(soundQuality) {
  case 1:
    audio.freq = 44100;
    soundBufferLen = 1470*2;
    break;
  case 2:
    audio.freq = 22050;
    soundBufferLen = 736*2;
    break;
  case 4:
    audio.freq = 11025;
    soundBufferLen = 368*2;
    break;
  }
  audio.format=AUDIO_S16SYS;
  audio.channels = 2;
  audio.samples = 1024;
  audio.callback = soundCallback;
  audio.userdata = NULL;
  if(SDL_OpenAudio(&audio, NULL)) {
    fprintf(stderr,"Failed to open audio: %s\n", SDL_GetError());
    return false;
  }
  soundBufferTotalLen = soundBufferLen*10;
  cond = SDL_CreateCond();
  mutex = SDL_CreateMutex();
  sdlSoundLen = 0;
  systemSoundOn = true;
  return true;
}

void systemSoundShutdown()
{
  SDL_mutexP(mutex);
  SDL_CondSignal(cond);
  SDL_mutexV(mutex);
  SDL_DestroyCond(cond);
  cond = NULL;
  SDL_DestroyMutex(mutex);
  mutex = NULL;
  SDL_CloseAudio();
}

void systemSoundPause()
{
  SDL_PauseAudio(1);
}

void systemSoundResume()
{
  SDL_PauseAudio(0);
}

void systemSoundReset()
{
}

u32 systemGetClock()
{
  return SDL_GetTicks();
}

void systemUpdateMotionSensor()
{
  if(sdlMotionButtons[KEY_LEFT]) {
    sensorX += 3;
    if(sensorX > 2197)
      sensorX = 2197;
    if(sensorX < 2047)
      sensorX = 2057;
  } else if(sdlMotionButtons[KEY_RIGHT]) {
    sensorX -= 3;
    if(sensorX < 1897)
      sensorX = 1897;
    if(sensorX > 2047)
      sensorX = 2037;
  } else if(sensorX > 2047) {
    sensorX -= 2;
    if(sensorX < 2047)
      sensorX = 2047;
  } else {
    sensorX += 2;
    if(sensorX > 2047)
      sensorX = 2047;
  }

  if(sdlMotionButtons[KEY_UP]) {
    sensorY += 3;
    if(sensorY > 2197)
      sensorY = 2197;
    if(sensorY < 2047)
      sensorY = 2057;
  } else if(sdlMotionButtons[KEY_DOWN]) {
    sensorY -= 3;
    if(sensorY < 1897)
      sensorY = 1897;
    if(sensorY > 2047)
      sensorY = 2037;
  } else if(sensorY > 2047) {
    sensorY -= 2;
    if(sensorY < 2047)
      sensorY = 2047;
  } else {
    sensorY += 2;
    if(sensorY > 2047)
      sensorY = 2047;
  }    
}

int systemGetSensorX()
{
  return sensorX;
}

int systemGetSensorY()
{
  return sensorY;
}

void systemGbPrint(u8 *data,int pages,int feed,int palette, int contrast)
{
}

void systemScreenMessage(const char *msg)
{
  screenMessage = true;
  screenMessageTime = systemGetClock();
  if(strlen(msg) > 20) {
    strncpy(screenMessageBuffer, msg, 20);
    screenMessageBuffer[20] = 0;
  } else
    strcpy(screenMessageBuffer, msg);  
}

bool systemCanChangeSoundQuality()
{
  return false;
}

bool systemPauseOnFrame()
{
  if(pauseNextFrame) {
    paused = true;
    pauseNextFrame = false;
    return true;
  }
  return false;
}

// Code donated by Niels Wagenaar (BoycottAdvance)

// GBA screensize.
#define GBA_WIDTH   240
#define GBA_HEIGHT  160

void Init_Overlay(SDL_Surface *gbascreen, int overlaytype)
{
  
//  overlay = SDL_CreateYUVOverlay( GBA_WIDTH,
//                                  GBA_HEIGHT,
//                                  overlaytype, gbascreen);
//  fprintf(stderr, "Created %dx%dx%d %s %s overlay\n",
//          overlay->w,overlay->h,overlay->planes,
//          overlay->hw_overlay?"hardware":"software",
//          overlay->format==SDL_YV12_OVERLAY?"YV12":
//          overlay->format==SDL_IYUV_OVERLAY?"IYUV":
//          overlay->format==SDL_YUY2_OVERLAY?"YUY2":
//          overlay->format==SDL_UYVY_OVERLAY?"UYVY":
//          overlay->format==SDL_YVYU_OVERLAY?"YVYU":
//          "Unknown");
}

void Quit_Overlay(void)
{
  
//  SDL_FreeYUVOverlay(overlay);
}

/* NOTE: These RGB conversion functions are not intended for speed,
   only as examples.
*/
inline void RGBtoYUV(Uint8 *rgb, int *yuv)
{
  yuv[0] = (int)((0.257 * rgb[0]) + (0.504 * rgb[1]) + (0.098 * rgb[2]) + 16);
  yuv[1] = (int)(128 - (0.148 * rgb[0]) - (0.291 * rgb[1]) + (0.439 * rgb[2]));
  yuv[2] = (int)(128 + (0.439 * rgb[0]) - (0.368 * rgb[1]) - (0.071 * rgb[2]));
}

//inline void ConvertRGBtoYV12(SDL_Overlay *o)
//{
//  int x,y;
//  int yuv[3];
//  Uint8 *p,*op[3];
//
//  SDL_LockYUVOverlay(o);
//
//  /* Black initialization */
//  /*
//    memset(o->pixels[0],0,o->pitches[0]*o->h);
//    memset(o->pixels[1],128,o->pitches[1]*((o->h+1)/2));
//    memset(o->pixels[2],128,o->pitches[2]*((o->h+1)/2));
//  */
//
//  /* Convert */
//  for(y=0; y<160 && y<o->h; y++) {
//    p=(Uint8 *)pix+srcPitch*y;
//    op[0]=o->pixels[0]+o->pitches[0]*y;
//    op[1]=o->pixels[1]+o->pitches[1]*(y/2);
//    op[2]=o->pixels[2]+o->pitches[2]*(y/2);
//    for(x=0; x<240 && x<o->w; x++) {
//      RGBtoYUV(p,yuv);
//      *(op[0]++)=yuv[0];
//      if(x%2==0 && y%2==0) {
//        *(op[1]++)=yuv[2];
//        *(op[2]++)=yuv[1];
//      }
//      p+=4;//s->format->BytesPerPixel;
//    }
//  }
//
//  SDL_UnlockYUVOverlay(o);
//}
//
//inline void ConvertRGBtoIYUV(SDL_Overlay *o)
//{
//  int x,y;
//  int yuv[3];
//  Uint8 *p,*op[3];
//
//  SDL_LockYUVOverlay(o);
//
//  /* Black initialization */
//  /*
//    memset(o->pixels[0],0,o->pitches[0]*o->h);
//    memset(o->pixels[1],128,o->pitches[1]*((o->h+1)/2));
//    memset(o->pixels[2],128,o->pitches[2]*((o->h+1)/2));
//  */
//
//  /* Convert */
//  for(y=0; y<160 && y<o->h; y++) {
//    p=(Uint8 *)pix+srcPitch*y;
//    op[0]=o->pixels[0]+o->pitches[0]*y;
//    op[1]=o->pixels[1]+o->pitches[1]*(y/2);
//    op[2]=o->pixels[2]+o->pitches[2]*(y/2);
//    for(x=0; x<240 && x<o->w; x++) {
//      RGBtoYUV(p,yuv);
//      *(op[0]++)=yuv[0];
//      if(x%2==0 && y%2==0) {
//        *(op[1]++)=yuv[1];
//        *(op[2]++)=yuv[2];
//      }
//      p+=4; //s->format->BytesPerPixel;
//    }
//  }
//
//  SDL_UnlockYUVOverlay(o);
//}
//
//inline void ConvertRGBtoUYVY(SDL_Overlay *o)
//{
//  int x,y;
//  int yuv[3];
//  Uint8 *p,*op;
//
//  SDL_LockYUVOverlay(o);
//
//  for(y=0; y<160 && y<o->h; y++) {
//    p=(Uint8 *)pix+srcPitch*y;
//    op=o->pixels[0]+o->pitches[0]*y;
//    for(x=0; x<240 && x<o->w; x++) {
//      RGBtoYUV(p,yuv);
//      if(x%2==0) {
//        *(op++)=yuv[1];
//        *(op++)=yuv[0];
//        *(op++)=yuv[2];
//      } else
//        *(op++)=yuv[0];
//
//      p+=4; //s->format->BytesPerPixel;
//    }
//  }
//
//  SDL_UnlockYUVOverlay(o);
//}
//
//inline void ConvertRGBtoYVYU(SDL_Overlay *o)
//{
//  int x,y;
//  int yuv[3];
//  Uint8 *p,*op;
//
//  SDL_LockYUVOverlay(o);
//
//  for(y=0; y<160 && y<o->h; y++) {
//    p=(Uint8 *)pix+srcPitch*y;
//    op=o->pixels[0]+o->pitches[0]*y;
//    for(x=0; x<240 && x<o->w; x++) {
//      RGBtoYUV(p,yuv);
//      if(x%2==0) {
//        *(op++)=yuv[0];
//        *(op++)=yuv[2];
//        op[1]=yuv[1];
//      } else {
//        *op=yuv[0];
//        op+=2;
//      }
//
//      p+=4; //s->format->BytesPerPixel;
//    }
//  }
//
//  SDL_UnlockYUVOverlay(o);
//}
//
//inline void ConvertRGBtoYUY2(SDL_Overlay *o)
//{
//  int x,y;
//  int yuv[3];
//  Uint8 *p,*op;
//
//  SDL_LockYUVOverlay(o);
//
//  for(y=0; y<160 && y<o->h; y++) {
//    p=(Uint8 *)pix+srcPitch*y;
//    op=o->pixels[0]+o->pitches[0]*y;
//    for(x=0; x<240 && x<o->w; x++) {
//      RGBtoYUV(p,yuv);
//      if(x%2==0) {
//        *(op++)=yuv[0];
//        *(op++)=yuv[1];
//        op[1]=yuv[2];
//      } else {
//        *op=yuv[0];
//        op+=2;
//      }
//
//      p+=4; //s->format->BytesPerPixel;
//    }
//  }
//
//  SDL_UnlockYUVOverlay(o);
//}
//
//inline void Convert32bit(SDL_Surface *display)
//{
//  switch(overlay->format) {
//  case SDL_YV12_OVERLAY:
//    ConvertRGBtoYV12(overlay);
//    break;
//  case SDL_UYVY_OVERLAY:
//    ConvertRGBtoUYVY(overlay);
//    break;
//  case SDL_YVYU_OVERLAY:
//    ConvertRGBtoYVYU(overlay);
//    break;
//  case SDL_YUY2_OVERLAY:
//    ConvertRGBtoYUY2(overlay);
//    break;
//  case SDL_IYUV_OVERLAY:
//    ConvertRGBtoIYUV(overlay);
//    break;
//  default:
//    fprintf(stderr, "cannot convert RGB picture to obtained YUV format!\n");
//    exit(1);
//    break;
//  }
//
//}
//
//
//inline void Draw_Overlay(SDL_Surface *display, int size)
//{
//  SDL_LockYUVOverlay(overlay);
//
//  Convert32bit(display);
//
//  overlay_rect.x = 0;
//  overlay_rect.y = 0;
//  overlay_rect.w = GBA_WIDTH  * size;
//  overlay_rect.h = GBA_HEIGHT * size;
//
//  SDL_DisplayYUVOverlay(overlay, &overlay_rect);
//  SDL_UnlockYUVOverlay(overlay);
//}

void systemGbBorderOn()
{
  srcWidth = 256;
  srcHeight = 224;
  gbBorderLineSkip = 256;
  gbBorderColumnSkip = 48;
  gbBorderRowSkip = 40;

  destWidth = (sizeOption+1)*srcWidth;
  destHeight = (sizeOption+1)*srcHeight;
  
//  surface = SDL_SetVideoMode(destWidth, destHeight, 16,
//                             SDL_ANYFORMAT|SDL_HWSURFACE|SDL_DOUBLEBUF|
//                             (fullscreen ? SDL_FULLSCREEN : 0));
#ifndef C_CORE
  sdlMakeStretcher(srcWidth);
#else
  switch(systemColorDepth) {
  case 16:
    sdlStretcher = sdlStretcher16[sizeOption];
    break;
  case 24:
    sdlStretcher = sdlStretcher24[sizeOption];
    break;
  case 32:
    sdlStretcher = sdlStretcher32[sizeOption];
    break;
  default:
    fprintf(stderr, "Unsupported resolution: %d\n", systemColorDepth);
    exit(-1);
  }
#endif

  if(systemColorDepth == 16) {
    if(sdlCalculateMaskWidth(surface->format->Gmask) == 6) {
      Init_2xSaI(565);
      RGB_LOW_BITS_MASK = 0x821;
    } else {
      Init_2xSaI(555);
      RGB_LOW_BITS_MASK = 0x421;      
    }
    if(cartridgeType == 2) {
      for(int i = 0; i < 0x10000; i++) {
        systemColorMap16[i] = (((i >> 1) & 0x1f) << systemBlueShift) |
          (((i & 0x7c0) >> 6) << systemGreenShift) |
          (((i & 0xf800) >> 11) << systemRedShift);  
      }      
    } else {
      for(int i = 0; i < 0x10000; i++) {
        systemColorMap16[i] = ((i & 0x1f) << systemRedShift) |
          (((i & 0x3e0) >> 5) << systemGreenShift) |
          (((i & 0x7c00) >> 10) << systemBlueShift);  
      }
    }
    srcPitch = srcWidth * 2+4;
  } else {
    if(systemColorDepth != 32)
      filterFunction = NULL;
    RGB_LOW_BITS_MASK = 0x010101;
    if(systemColorDepth == 32) {
      Init_2xSaI(32);
    }
    for(int i = 0; i < 0x10000; i++) {
      systemColorMap32[i] = ((i & 0x1f) << systemRedShift) |
        (((i & 0x3e0) >> 5) << systemGreenShift) |
        (((i & 0x7c00) >> 10) << systemBlueShift);  
    }
    if(systemColorDepth == 32)
      srcPitch = srcWidth*4 + 4;
    else
      srcPitch = srcWidth*3;
  }
}
