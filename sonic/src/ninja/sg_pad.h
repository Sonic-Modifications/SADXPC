#pragma once

#include <sg_xpt.h>

// Port ids
#define PDD_PORT_A0    0
#define PDD_PORT_A1    1
#define PDD_PORT_A2    2
#define PDD_PORT_A3    3
#define PDD_PORT_A4    4
#define PDD_PORT_A5    5
#define PDD_PORT_B0    6
#define PDD_PORT_B1    7
#define PDD_PORT_B2    8
#define PDD_PORT_B3    9
#define PDD_PORT_B4   10
#define PDD_PORT_B5   11
#define PDD_PORT_C0   12
#define PDD_PORT_C1   13
#define PDD_PORT_C2   14
#define PDD_PORT_C3   15
#define PDD_PORT_C4   16
#define PDD_PORT_C5   17
#define PDD_PORT_D0   18
#define PDD_PORT_D1   19
#define PDD_PORT_D2   20
#define PDD_PORT_D3   21
#define PDD_PORT_D4   22
#define PDD_PORT_D5   23

// Device ids
#define PDD_DEV_NODEVICE       0x00000000
#define PDD_DEV_CONTROLLER     0x000f06fe
#define PDD_DEV_KEYBOARD       0x01020000

// Pad capabilities
#define PDD_DEV_SUPPORT_AY2   (1 << 21)
#define PDD_DEV_SUPPORT_AX2   (1 << 20)
#define PDD_DEV_SUPPORT_AY1   (1 << 19)
#define PDD_DEV_SUPPORT_AX1   (1 << 18)
#define PDD_DEV_SUPPORT_AL    (1 << 17)
#define PDD_DEV_SUPPORT_AR    (1 << 16)
#define PDD_DEV_SUPPORT_KRB   (1 << 15)
#define PDD_DEV_SUPPORT_KLB   (1 << 14)
#define PDD_DEV_SUPPORT_KDB   (1 << 13)
#define PDD_DEV_SUPPORT_KUB   (1 << 12)
#define PDD_DEV_SUPPORT_TD    (1 << 11)
#define PDD_DEV_SUPPORT_TX    (1 << 10)
#define PDD_DEV_SUPPORT_TY    (1 <<  9)
#define PDD_DEV_SUPPORT_TZ    (1 <<  8)
#define PDD_DEV_SUPPORT_KR    (1 <<  7)
#define PDD_DEV_SUPPORT_KL    (1 <<  6)
#define PDD_DEV_SUPPORT_KD    (1 <<  5)
#define PDD_DEV_SUPPORT_KU    (1 <<  4)
#define PDD_DEV_SUPPORT_ST    (1 <<  3)
#define PDD_DEV_SUPPORT_TA    (1 <<  2)
#define PDD_DEV_SUPPORT_TB    (1 <<  1)
#define PDD_DEV_SUPPORT_TC    (1 <<  0)

// Pad buttons
#define PDD_DGT_TL    (1 << 17)
#define PDD_DGT_TR    (1 << 16)
#define PDD_DGT_KRB   (1 << 15)
#define PDD_DGT_KLB   (1 << 14)
#define PDD_DGT_KDB   (1 << 13)
#define PDD_DGT_KUB   (1 << 12)
#define PDD_DGT_TD    (1 << 11)
#define PDD_DGT_TX    (1 << 10)
#define PDD_DGT_TY    (1 <<  9)
#define PDD_DGT_TZ    (1 <<  8)
#define PDD_DGT_KR    (1 <<  7)
#define PDD_DGT_KL    (1 <<  6)
#define PDD_DGT_KD    (1 <<  5)
#define PDD_DGT_KU    (1 <<  4)
#define PDD_DGT_ST    (1 <<  3)
#define PDD_DGT_TA    (1 <<  2)
#define PDD_DGT_TB    (1 <<  1)
#define PDD_DGT_TC    (1 <<  0)

// Pad error codes
#define PDD_ERR_OK       0x00000000
#define PDD_ERR_RETRY    0xff0000fc
#define PDD_ERR_GENERIC  0xff000000

// Pad logical modes
#define PDD_PLOGIC_ACTIVE    0
#define PDD_PLOGIC_NEGATIVE  1

// Maple modes(?)
#define PDD_MODE_TYPE_A    1
#define PDD_MODE_TYPE_B    0

// Device type
#define PDD_DEVTYPE_CONTROLLER   (1 << 0)
#define PDD_DEVTYPE_STORAGE      (1 << 1)
#define PDD_DEVTYPE_LCD          (1 << 2)
#define PDD_DEVTYPE_TIMER        (1 << 3)
#define PDD_DEVTYPE_SOUNDINPUT   (1 << 4)
#define PDD_DEVTYPE_ARGUN        (1 << 5)
#define PDD_DEVTYPE_KEYBOARD     (1 << 6)

// Device area code
#define PDD_DEVAREA_USA          (1 << 0)
#define PDD_DEVAREA_JAPAN        (1 << 1)
#define PDD_DEVAREA_ASIA         (1 << 2)
#define PDD_DEVAREA_EUROPE       (1 << 3)
#define PDD_DEVAREA_RESERVED1    (1 << 4)
#define PDD_DEVAREA_RESERVED2    (1 << 5)
#define PDD_DEVAREA_RESERVED3    (1 << 6)
#define PDD_DEVAREA_RESERVED4    (1 << 7)

// Connector direction
#define PDD_CONDIR_TOPSIDE       0
#define PDD_CONDIR_BOTTOMSIDE    1
#define PDD_CONDIR_LEFTSIDE      2
#define PDD_CONDIR_RIGHTSIDE     3

// Pad globals
struct PDS_PERIPHERALINFO
{
	Uint32 type;
	Uint32 reserved[3];
	Uint8 is_root;
	Uint8 area_code;
	Uint8 connector_dir[2];
	char product_name[32];
	char license[64];
	Uint16 stdby_pow;
	Uint16 max_pow;
};

struct PDS_PERIPHERAL
{
	Uint32 id;
	Uint32 support;
	Uint32 on;
	Uint32 off;
	Uint32 press;
	Uint32 release;
	Uint16 r;
	Uint16 l;
	Sint16 x1;
	Sint16 y1;
	Sint16 x2;
	Sint16 y2;
	char* name;
	void* extend;
	Uint32 old;
	PDS_PERIPHERALINFO* info;
};

// Pad functions
void pdInitPeripheral(Uint32 logic, void *recvbuf, void *sendbuf);
