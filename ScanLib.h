#pragma once
#ifndef SCANLIB_H
#define SCANLIB_H

#include "DISS6ControlTypes.h"
#include "CommonTools.h"
#include "ScanJob.h"
#include <Windows.h>
#include <string>
#include <map>
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>

class ScanLib
{
public:
    ScanLib(Utils::ConnectionType type = Utils::ConnectionType::USB);

    //NormalParams normalParams;//Normal Scan 
    //ReduceParams reduceParams;//Reduce Scan
    //ChannelOptions channelSelect;
    //void InitScanParams();
    //void InitReduceScanParams();

    void SetReduceMode(ushort StartX, ushort StartY, ushort ReduceWidth, ushort ReduceHeight, ushort FullImageWidth, ushort FullImageHeight);//reduce scan parameters
    void SetScanMode(Utils::ScanningMode mode);
    void SetDualChannelMode(int CH1Index, int CH2Index);
    void SelectDetectorChannel(bool IsEtOn = true, bool IsInLensOn = false, bool IsBsdOn = false, bool IsStemOn = false);  //选取探测器
    void RefreshImagingSettings(int width, int height, int dwell, int avgMode, int avgNum);  //只有线平均，加了平均数量 
    void SelectChannel(FastChannelSelection fastChSelection, bool* channelSelection);
    void SetScanVoltage(double scanVoltage);
    void SetCurrentChannel(int channelIndex);

    double GetScanRotationAngle();
    double GetBrightnessByChannel(int channelIndex);
    double GetContrastByChannel(int channelIndex);

    void Start(int imagingMode);
    void StartDiss6();
    void StopScan();

    bool _isChangeSize = false;
    bool _isRunning = false;//Scan is running
    bool _isInit = false;//ScanLibLocalFunctions
    bool _isStopImmediately = false;
    bool _isReducePosChanged = false;

    int _currentChannel = 1;
    int *DetectorIndex = new int[4];

    // Reduce scan Params   Reduce 扫描模式参数. Scanlib 和 dvScangenerator 的参数设置本质一致，实际上考虑如何，将两个变量设置为一个。省去变量
    ushort _startX, _startY, _rscanWidth, _rscanHeight, _fullWidth, _fullHeight;//reduce ROI区域尺寸， 整体尺寸， ROI X， ROI Y

private:
    void InitChDetPair();
    void InitDiss6Param();
    void InitializeDISS6(Utils::ConnectionType type);

    std::vector<ChannelInfo_t> GetChannelSection();

    //Handle Data
    void HandlePixelToSend(uint pixelOffset, uint pixelCount);

    void HandleDataAvailable();
    void HandleScanJobRunning();//初始化指针，只执行一次
    void HandleDataTimeout();
    void HandleScanJobFinished();

    void SetupCallbacks();
    void ImagingProc();

    void Continuous();
    void Grab();
    void SubScan();
    void Spot();

    ScanJob* job = nullptr;

    std::map<std::string, std::string> channelDetPairs;
    std::map<std::string, bool> detSelectedPairs;

    int linesPerPacket, packetsPerFrame;
    int imagingState = 0;  //ScanState: 0-Idle, 1-Continuous, 2-Grabing, 3-SubScaning

    // Scan Setting Params  (Size, Dwell Time, AvgMode, AvgNum, FrameCount, Brightness, Contrast, ScanRotation Angle， Reduce , Dual Channel)
    int _imgwidth, _imgheight, _dwelltime, _avgMode, _avgNum;   // avgMode: 0-NoAveraging, 1-Line, 2-Frame, 3-SubScan, -1-unknown
    TimeScale _frameFlyBackTime = TimeScale::US10;

    ushort _frameCount;  //0为持续扫描，及continuous模式， 1为Grab模式, 设置帧数，扫描发生器到达这个帧数后停止
    double _scanRotate, _scanRotationOffsetAngle, _skewAngle;
    const ushort dacOffset = 6560;

    // 探测器Mix 相关设置
    bool _mixFlag = false;
    std::string _mixIndexA = "", _mixIndexB = "";
    double _mixRate = 0;
    int _scanMode = 0;  // 0:正常扫描   1:Split Mode   2:Dual Channel   3:Dual Mag    4:Quad Mode   5:Reduce,   6:Mix Channel

    // Dual Channel 相关设置
    int _dualSig1 = 1, _dualSig2 = 1;
    const int N = 16388; // 4(current starting line)  + 16KB buffer    每次上传buffer容量
    float brightness_A1, brightness_A2, brightness_A3, brightness_A4;
    float contrast_A1, contrast_A2, contrast_A3, contrast_A4;

    //真正获取数据参数
    int _ImageBufferSize = 1024 * 1024;//动态修改. 使用1024进行试验
    bool _Shutdown = false;
    bool _BitmapUpdateNeeded = false;
    //byte** _FrameBuffers = nullptr; 实际上二者是一个东西 保留frameBuffer即可
    void** _MarshalledFramePtrs = nullptr;
    int _currentPixelIndex = 0, _packetsSendNum = 0;//现在获取到的像素个数，已发送的包数

    std::mutex mtx;                       // 用于线程锁的互斥量

    int _channelCount = 0;
    bool _channelsA[4] = { false, false, false, false };
    bool _channelsB[4] = { false, false, false, false };
};

#endif // !SCANLIB_H

#pragma region MMF参数
//static const int MMFOffset = 12;//前3个字节存整数，scanmode, buffersize, offset.

//static HANDLE hMapFile;// 打开内存映射文件
//static LPVOID pBuf;
//static const wchar_t* mmfMapName;

//static HANDLE hMapFile_Reduce;
//static LPVOID pBuf_Reduce;
//static const wchar_t* mmfMapName_Reduce;

//static HANDLE hMutex;
//static const wchar_t* mmfMutexName;

//static HANDLE hMutex_Reduce;
//static const wchar_t* mmfMutexName_Reduce;

//static int InitMMFParams();
//static int UninitMMFParams();

//static int InitMMFReduceParams();
//static int UninitMMFReduceParams();
#pragma endregion