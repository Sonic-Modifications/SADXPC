#pragma once

#include <sg_xpt.h>

// Ninja types
typedef Sint32 Angle;

typedef Float NJS_MATRIX[16];

typedef Float NJS_FOG_TABLE[128];

// Screen resolution
#define NJD_DSP_NTSC		(0x0)
#define NJD_DSP_VGA			(0x1)
#define NJD_DSP_PAL			(0x2)

#define NJD_DSP_NONINTER 	(0x00)
#define NJD_DSP_INTER    	(0x04)
#define NJD_DSP_PSINTER  	(0x08)

#define NJD_DSP_WIDTH_320	(0x000)
#define NJD_DSP_WIDTH_640	(0x010)

#define NJD_DSP_HEIGHT_240	(0x000)
#define NJD_DSP_HEIGHT_480	(0x020)

#define NJD_DSP_320x240		(NJD_DSP_WIDTH_320|NJD_DSP_HEIGHT_240)
#define NJD_DSP_640x240		(NJD_DSP_WIDTH_640|NJD_DSP_HEIGHT_240)
#define NJD_DSP_320x480		(NJD_DSP_WIDTH_320|NJD_DSP_HEIGHT_480)
#define NJD_DSP_640x480		(NJD_DSP_WIDTH_640|NJD_DSP_HEIGHT_480)

#define NJD_DSP_ANTI	(0x00010000)	

#define	NJD_RESOLUTION_VGA					(NJD_DSP_VGA)
#define	NJD_RESOLUTION_320x240_NTSCNI		(NJD_DSP_NTSC|NJD_DSP_NONINTER|NJD_DSP_320x240)
#define	NJD_RESOLUTION_320x240_NTSCI		(NJD_DSP_NTSC|NJD_DSP_INTER   |NJD_DSP_320x240)
#define	NJD_RESOLUTION_640x240_NTSCNI		(NJD_DSP_NTSC|NJD_DSP_NONINTER|NJD_DSP_640x240)
#define	NJD_RESOLUTION_640x240_NTSCI		(NJD_DSP_NTSC|NJD_DSP_INTER   |NJD_DSP_640x240)
#define	NJD_RESOLUTION_320x480_NTSCNI_FF 	(NJD_DSP_NTSC|NJD_DSP_NONINTER|NJD_DSP_320x480)
#define	NJD_RESOLUTION_320x480_NTSCNI		(NJD_DSP_NTSC|NJD_DSP_PSINTER |NJD_DSP_320x480)
#define	NJD_RESOLUTION_320x480_NTSCI		(NJD_DSP_NTSC|NJD_DSP_INTER   |NJD_DSP_320x480)
#define	NJD_RESOLUTION_640x480_NTSCNI_FF 	(NJD_DSP_NTSC|NJD_DSP_NONINTER|NJD_DSP_640x480)
#define NJD_RESOLUTION_640x480_NTSCNI	 	(NJD_DSP_NTSC|NJD_DSP_PSINTER |NJD_DSP_640x480)
#define	NJD_RESOLUTION_640x480_NTSCI		(NJD_DSP_NTSC|NJD_DSP_INTER   |NJD_DSP_640x480)
#define	NJD_RESOLUTION_320x240_PALNI		(NJD_DSP_PAL |NJD_DSP_NONINTER|NJD_DSP_320x240)
#define	NJD_RESOLUTION_320x240_PALI			(NJD_DSP_PAL |NJD_DSP_INTER   |NJD_DSP_320x240)
#define	NJD_RESOLUTION_640x240_PALNI		(NJD_DSP_PAL |NJD_DSP_NONINTER|NJD_DSP_640x240)
#define	NJD_RESOLUTION_640x240_PALI			(NJD_DSP_PAL |NJD_DSP_INTER   |NJD_DSP_640x240)
#define	NJD_RESOLUTION_320x480_PALNI_FF		(NJD_DSP_PAL |NJD_DSP_NONINTER|NJD_DSP_320x480)
#define	NJD_RESOLUTION_320x480_PALNI		(NJD_DSP_PAL |NJD_DSP_PSINTER |NJD_DSP_320x480)
#define	NJD_RESOLUTION_320x480_PALI			(NJD_DSP_PAL |NJD_DSP_INTER   |NJD_DSP_320x480)
#define	NJD_RESOLUTION_640x480_PALNI_FF		(NJD_DSP_PAL |NJD_DSP_NONINTER|NJD_DSP_640x480)
#define	NJD_RESOLUTION_640x480_PALNI		(NJD_DSP_PAL |NJD_DSP_PSINTER |NJD_DSP_640x480)
#define	NJD_RESOLUTION_640x480_PALI			(NJD_DSP_PAL |NJD_DSP_INTER   |NJD_DSP_640x480)

#define	NJD_RESOLUTION_VGA_ANTI					(NJD_RESOLUTION_VGA				 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_320x240_NTSCNI_ANTI		(NJD_RESOLUTION_320x240_NTSCNI	 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_320x240_NTSCI_ANTI		(NJD_RESOLUTION_320x240_NTSCI	 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_640x240_NTSCNI_ANTI		(NJD_RESOLUTION_640x240_NTSCNI	 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_640x240_NTSCI_ANTI		(NJD_RESOLUTION_640x240_NTSCI	 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_320x480_NTSCNI_FF_ANTI 	(NJD_RESOLUTION_320x480_NTSCNI_FF|NJD_DSP_ANTI)
#define	NJD_RESOLUTION_320x480_NTSCNI_ANTI		(NJD_RESOLUTION_320x480_NTSCNI	 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_320x480_NTSCI_ANTI		(NJD_RESOLUTION_320x480_NTSCI	 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_640x480_NTSCNI_FF_ANTI 	(NJD_RESOLUTION_640x480_NTSCNI_FF|NJD_DSP_ANTI)
#define NJD_RESOLUTION_640x480_NTSCNI_ANTI	 	(NJD_RESOLUTION_640x480_NTSCNI	 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_640x480_NTSCI_ANTI		(NJD_RESOLUTION_640x480_NTSCI	 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_320x240_PALNI_ANTI		(NJD_RESOLUTION_320x240_PALNI	 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_320x240_PALI_ANTI		(NJD_RESOLUTION_320x240_PALI	 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_640x240_PALNI_ANTI		(NJD_RESOLUTION_640x240_PALNI	 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_640x240_PALI_ANTI		(NJD_RESOLUTION_640x240_PALI	 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_320x480_PALNI_FF_ANTI	(NJD_RESOLUTION_320x480_PALNI_FF |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_320x480_PALNI_ANTI		(NJD_RESOLUTION_320x480_PALNI	 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_320x480_PALI_ANTI		(NJD_RESOLUTION_320x480_PALI	 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_640x480_PALNI_FF_ANTI	(NJD_RESOLUTION_640x480_PALNI_FF |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_640x480_PALNI_ANTI		(NJD_RESOLUTION_640x480_PALNI	 |NJD_DSP_ANTI)
#define	NJD_RESOLUTION_640x480_PALI_ANTI		(NJD_RESOLUTION_640x480_PALI	 |NJD_DSP_ANTI)

#define NJD_VIDEO_MASK	(0x00000003)
#define NJD_DSP_MASK	(0x00000FFF)
#define NJD_WIDTH_MASK 	(NJD_DSP_WIDTH_640)
#define NJD_HEIGHT_MASK	(NJD_DSP_HEIGHT_480)

// Frame buffer mode
#define	NJD_FRAMEBUFFER_MODE_RGB565				0
#define	NJD_FRAMEBUFFER_MODE_RGB555				1
#define	NJD_FRAMEBUFFER_MODE_ARGB1555			3
#define	NJD_FRAMEBUFFER_MODE_RGB888				4
#define	NJD_FRAMEBUFFER_MODE_ARGB8888			5
