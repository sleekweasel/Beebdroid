/*
 * main.h
 *
 *
 * Written by Reuben Scratton, based on original code by Tom Walker
 *
 */
#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <time.h>

typedef signed int fixed;

typedef struct BITMAP {
	int width;
	int height;
	int bpp;
	int stride;
	unsigned char* pixels;
} BITMAP;

BITMAP *create_bitmap(int width, int height);

#define RED(x) (x&0xff)
#define GREEN(x) ((x&0xff00)>>8)
#define BLUE(x) ((x&0xff0000)>>16)



#define  LOG_TAG    "beebdroid"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


void blit_to_screen(int source_x, int source_y, int width, int height);


#define FLAG_C 1
#define FLAG_Z 2
#define FLAG_I 4
#define FLAG_D 8
#define FLAG_V 0x40
#define FLAG_N 0x80

typedef struct M6502_struct M6502;

typedef int (*FN)(M6502*);
extern FN fns[];

struct M6502_struct {
	uint8_t* mem; // +0
	uint16_t pc;  // +4
	uint8_t a;    // +6
	uint8_t x;    // +7
	uint8_t y;    // +8
	uint8_t s;    // +9
	uint8_t p;    // +10
	uint8_t unused; //
	int interrupt; // +12
	int nmi;	  // +16
	int takeint;  // +20
	int cycles;   // +24
	uint16_t pc_trigger_hit; //+28
	uint16_t pc_triggers[4]; //+30
	int cycles2;	// +38
	void (*log_cpu_S)(M6502*); // +42 for PIC!
	uint8_t (*readmem_ex_S)(uint16_t addr); // +46 for PIC!
	void (*writemem_ex_S)(uint16_t addr, uint8_t val16); // +50 for PIC!
	void (*adc_bcd_S)(M6502*, uint8_t); // +54 for PIC!
	void (*sbc_bcd_S)(M6502*, uint8_t); // +58 for PIC!
	void (*log_undef_opcode_S)(M6502*); // +62 for PIC!
	void (*do_poll_S)(M6502*, int c); // +64 for PIC!
	void (*readword_ex_S)(uint16_t); // +68 for PIC!
	void (**fns_C)(M6502*); // +72F

};

extern M6502* the_cpu;

extern void log_cpu_C(M6502*); // +42 for PIC!
extern uint8_t readmem_ex(uint16_t addr); // +46 for PIC!
extern void writemem_ex(uint16_t addr, uint8_t val16);
extern void adc_bcd_C(M6502*, uint8_t temp); // +54 for PIC!
extern void sbc_bcd_C(M6502*, uint8_t temp); // +58 for PIC!
extern void log_undef_opcode_C(M6502*); // +62 for PIC!
extern void do_poll_C(M6502*, int c); // +64 for PIC!
extern uint16_t readword_ex(uint16_t addr); // +68 for PIC!

extern uint8_t *roms;
extern int swram[16];
extern int motorspin;
extern int fdctime;
extern int motoron;
extern int disctime;
extern int frames;

typedef struct VIA
{
        uint8_t ora,orb,ira,irb;
        uint8_t ddra,ddrb;
        uint8_t sr;
        uint32_t t1l,t2l;
        int t1c,t2c;
        uint8_t acr,pcr,ifr,ier;
        int t1hit,t2hit;
        uint8_t porta,portb;
        int ca1,ca2;
} VIA;
extern VIA sysvia,uservia;

extern int scrsize;

void (*fdccallback)();
void (*fdcdata)(uint8_t dat);
void (*fdcspindown)();
void (*fdcfinishread)();
void (*fdcnotfound)();
void (*fdcdatacrcerror)();
void (*fdcheadercrcerror)();
void (*fdcwriteprotect)();
int  (*fdcgetdata)(int last);

extern int writeprot[2],fwriteprot[2];

void initvideo();
void closevideo();
void makemode7chars();
void pollvideo(int clocks);
uint8_t readcrtc(uint16_t addr);
void writecrtc(uint16_t addr, uint8_t val);
extern uint8_t crtc[32];
extern int crtci;
extern int hc,vc,sc;
extern uint16_t ma;


void reset6502();
void exec6502(M6502* cpu);

void writesysvia(uint16_t addr, uint8_t val);
uint8_t readsysvia(uint16_t addr);
void resetsysvia();
void checkkeys();
void clearkeys();

void writeuservia(uint16_t addr, uint8_t val);
uint8_t readuservia(uint16_t addr);
void resetuservia();

void ssd_reset();
void ssd_load(int drive, unsigned char *img, int cb);
void ssd_close(int drive);
void dsd_load(int drive, unsigned char *img, int cb);
void ssd_seek(int drive, int track);
void ssd_readsector(int drive, int sector, int track, int side, int density);
void ssd_writesector(int drive, int sector, int track, int side, int density);
void ssd_readaddress(int drive, int sector, int side, int density);
void ssd_format(int drive, int sector, int side, int density);
void ssd_poll();

void disc_reset();
void disc_poll();
void disc_seek(int drive, int track);
void disc_readsector(int drive, int sector, int track, int side, int density);
void disc_writesector(int drive, int sector, int track, int side, int density);
void disc_readaddress(int drive, int track, int side, int density);
void disc_format(int drive, int track, int side, int density);

void reset8271();
uint8_t read8271(uint16_t addr);
void write8271(uint16_t addr, uint8_t val);

void writecrtc(uint16_t addr, uint8_t val);
void writeula(uint16_t addr, uint8_t val);
extern uint8_t bakpal[16];
extern uint8_t ulactrl;


struct
{
        void (*seek)(int drive, int track);
        void (*readsector)(int drive, int sector, int track, int side, int density);
        void (*writesector)(int drive, int sector, int track, int side, int density);
        void (*readaddress)(int drive, int track, int side, int density);
        void (*format)(int drive, int track, int side, int density);
        void (*poll)();
} drives[2];

int curdrive;


extern int adcconvert;
extern int joybutton[2];

void initsound();

void initadc();
uint8_t readadc(uint16_t addr);
void writeadc(uint16_t addr, uint8_t val);

extern int defaultwriteprot;

extern int idecallback;
extern int vidchange;
extern int autoboot;
extern int ideenable;

void loaddisc(int drive, int loader, unsigned char* discimg, int cb);
void logvols();
void dumpregs();
void closedisc(int drive);
void updatesystimers();
void updateusertimers();

#endif
