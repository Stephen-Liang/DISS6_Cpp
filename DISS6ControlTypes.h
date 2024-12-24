#pragma once
#ifndef DISS6CONTROLTYPES_H
#define DISS6CONTROLTYPES_H

#include "DISS6.h"

using byte = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;

enum AbortCode : byte
{
    None = 0,
    AfterFrame = 1,
    Immediately = 3
};

enum ScanMode : byte
{
    Normal = 0,
    Chopped = 1,
    WobbleA = 2,
    WobbleB = 3,
    SubPixel = 4,
    SubPixelRevolving = 5
};

enum TimeScale : byte
{
    TimeScaleNone = 0,
    NS10 = 1,
    NS20 = 2,
    NS50 = 3,
    NS100 = 4,
    NS200 = 5,
    NS500 = 6,
    US1 = 7,
    US2 = 8,
    US5 = 9,
    US10 = 10,
    US20 = 11,
    US50 = 12,
    US100 = 13,
    US200 = 14,
    US500 = 15,
    MS1 = 16,
    MS2 = 17,
    MS5 = 18,
    MS10 = 19,
    MS20 = 20,
    MS50 = 21,
    MS100 = 22,
    MS200 = 23,
    MS500 = 24,
    S1 = 25,
    S2 = 26,
    S5 = 27,
    S10 = 28
};

enum ChannelSource : byte
{
    ChannelSourceNone = 0,
    FastA = 1,
    FastB = 2,
    Slow = 3,
    LIA = 4,
    Counter = 5,
    ECLCounter = 6
};

enum ChannelSelection : byte
{
    Direct = 0,
    Sum = 1,
    Mix = 2,
    AUX = 3
};

enum ChannelDataType : byte
{
    Invalid = 0,
    U8 = 1,
    U16 = 2,
    U32 = 3
};

struct ChannelId
{
    ChannelSource source;

    byte index;

    ushort value;
};

struct ChannelInfo
{
    ChannelId id;

    ChannelSelection selection;

    ChannelDataType datatype;

    uint value;
};

enum LIAClockMode : byte
{
    Reference = 0,
    Pixel = 1,
    Line = 2,
    Frame = 3
};

enum ScanJobState : byte
{
    Inexistent = 0,
    Prepared = 1,
    TransmissionStarted = 2,
    InTransmission = 3,
    TransmissionFailed = 4,
    Sent = 5,
    Running = 6,
    Rejected = 7,
    Finished = 8,
    Abandoned = 9
};

enum VideoDeinterlacingMode : byte
{
    Odd = 0,
    Even = 1,
    Weave = 2,
    Bobbing = 3,
    OddFieldInterpolate = 4,
    OddFieldInterpolate6 = 5,
    OddFieldInterpolate6W = 6
};

enum DVIMode : byte
{
    DVI = 0,
    HDMI = 1
};

enum DVIResolution : byte
{
    R640x480 = 0,
    R800x600 = 1,
    R1024X768 = 2,
    R1280X720 = 3,
    R1360X768 = 4,
    R1600X900 = 5,
    R1920X1080 = 6,
    R1920X1200 = 7
};

class ReadChannelDataFlags
{
public:
    static const uint UsePixelOffset = 0x00000001;
};

class ReadChannelDataStatus
{
public:
    static const uint BufferEmpty = 0x00000001;
    static const uint BufferOverflow = 0x00000002;
    static const uint OffsetMismatch = 0x00000004;
    static const uint FrameEnd = 0x00000010;
    static const uint USBError = 0x80000000;
};

class ScanTransformationFlags
{
    const byte InvertX = 0x01;
    const byte InvertY = 0x02;
    const byte NoSkewY = 0x04;
    const byte TiltX = 0x08;
};

#endif // DISS6CONTROLTYPES_H
