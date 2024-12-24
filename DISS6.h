#pragma once
#ifndef DISS6_H
#define DISS6_H
#include <cstdint>

// scan job abort codes
#define ABORT_SCAN_NONE						0
#define ABORT_SCAN_AFTER_FRAME				1
#define ABORT_SCAN_IMMEDIATELY				3

// scan mode
#define SCAN_MODE_NORMAL					0
#define SCAN_MODE_CHOPPED					1
#define SCAN_MODE_WOBBLE_A					2
#define SCAN_MODE_WOBBLE_B					3
#define SCAN_MODE_SUBPIXEL					4
#define SCAN_MODE_SUBPIXEL_REVOLVING		5

// time scale
#define TIME_SCALE_NONE						0
#define TIME_SCALE_10NS						1
#define TIME_SCALE_20NS						2
#define TIME_SCALE_50NS						3
#define TIME_SCALE_100NS					4
#define TIME_SCALE_200NS					5
#define TIME_SCALE_500NS					6
#define TIME_SCALE_1US						7
#define TIME_SCALE_2US						8
#define TIME_SCALE_5US						9
#define TIME_SCALE_10US						10
#define TIME_SCALE_20US						11
#define TIME_SCALE_50US						12
#define TIME_SCALE_100US					13
#define TIME_SCALE_200US					14
#define TIME_SCALE_500US					15
#define TIME_SCALE_1MS						16
#define TIME_SCALE_2MS						17
#define TIME_SCALE_5MS						18
#define TIME_SCALE_10MS						19
#define TIME_SCALE_20MS						20
#define TIME_SCALE_50MS						21
#define TIME_SCALE_100MS					22
#define TIME_SCALE_200MS					23
#define TIME_SCALE_500MS					24
#define TIME_SCALE_1S						25
#define TIME_SCALE_2S						26
#define TIME_SCALE_5S						27
#define TIME_SCALE_10S						28

typedef uint8_t TimeScale_t;

// channel source
#define CHANNEL_SOURCE_NONE					0
#define CHANNEL_SOURCE_A_FAST_A				1
#define CHANNEL_SOURCE_A_FAST_B				2
#define CHANNEL_SOURCE_A_SLOW				3
#define CHANNEL_SOURCE_A_LIA				4
#define CHANNEL_SOURCE_COUNTER				5
#define CHANNEL_SOURCE_ECL_COUNTER			6

// channel selection SLOW/MICS
#define CHANNEL_SEL_SLOW_DIRECT				0
// SUM selection, only for MICS (optional)
#define CHANNEL_SEL_SLOW_SUM				1	
// MIX selection, only with MICS (optional)
#define CHANNEL_SEL_SLOW_MIX				2	
// AUX selection, only with MICS (optional)
#define CHANNEL_SEL_SLOW_AUX				3	

// channel data type
#define CHANNEL_DATATYPE_INVALID			0
// 8 bit data, cannnot be mixed with other data types                                                     
#define CHANNEL_DATATYPE_U8					1
// 16 bit data (default), cannnot be mixed with 8 bit data                                              
#define CHANNEL_DATATYPE_U16				2
// 32 bit daten, only for counter channels (two 16 bit counter channels will be cascaded)
#define CHANNEL_DATATYPE_U32				3

// channel id/info structure
#pragma pack(push,1)
typedef struct
{
	uint8_t source;
	uint8_t index;
} ChannelId_struct_t;
#pragma pack(pop)

typedef union
{
	uint16_t value;
	ChannelId_struct_t part;
} ChannelId_t;

#pragma pack(push,1)
typedef struct
{
	ChannelId_t id;
	uint8_t selection;
	uint8_t datatype;
} ChannelInfo_struct_t;
#pragma pack(pop)

typedef union
{
	uint32_t value;
	ChannelInfo_struct_t part;
} ChannelInfo_t;

// flags/status for ReadChannelData
#define READ_FLAG_USE_PIXEL_OFFSET			0x0001

#define READ_STATUS_NONE					0x00000000
#define READ_STATUS_BUFFER_EMPTY			0x00000001
#define READ_STATUS_BUFFER_OVERFLOW			0x00000002
#define READ_STATUS_OFFSET_MISMATCH			0x00000004
#define READ_STATUS_FRAME_END				0x00000010
#define READ_STATUS_USB_ERROR				0x80000000
#define READ_STATUS_DATA_LOSS				(READ_STATUS_BUFFER_OVERFLOW | READ_STATUS_OFFSET_MISMATCH)

// LIA clock mode
#define LIA_CLOCKMODE_REFERENCE 			0
#define LIA_CLOCKMODE_PIXEL 				1
#define LIA_CLOCKMODE_LINE 					2
#define LIA_CLOCKMODE_FRAME 				3

// scan job state
#define SCAN_JOB_STATE_INEXISTENT			0
#define SCAN_JOB_STATE_PREPARED				1
#define SCAN_JOB_STATE_TRANSMISSION_STARTED	2
#define SCAN_JOB_STATE_IN_TRANSMISSION		3
#define SCAN_JOB_STATE_TRANSMISSION_FAILED	4
#define SCAN_JOB_STATE_SENT					5
#define SCAN_JOB_STATE_RUNNING				6
#define SCAN_JOB_STATE_REJECTED				7
#define SCAN_JOB_STATE_FINISHED				8
#define SCAN_JOB_STATE_ABANDONED			9

// DVI mode
#define DVI_MODE_DVI						0
#define DVI_MODE_HDMI						1

// DVI resolution
#define DVI_RESOLUTION_640X480				0
#define DVI_RESOLUTION_800X600				1
#define DVI_RESOLUTION_1024X768				2
#define DVI_RESOLUTION_1280X720				3
#define DVI_RESOLUTION_1360X768				4
#define DVI_RESOLUTION_1600X900				5
#define DVI_RESOLUTION_1920X1080			6
#define DVI_RESOLUTION_1920X1200			7

#endif