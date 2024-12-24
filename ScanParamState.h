#pragma once
#ifndef SCANMAIN_H
#define SCANMAIN_H

#include <mutex>
#include <queue>
#include <stdexcept>

//Utils 计算prescan参数等
class Utils
{
public:
    static enum ConnectionType
    {
        USB = 0,
        TCP = 1
    };

    static enum ScanningMode
    {
        Normal,
        SplitMode,
        DualChannel,
        DualMag, //暂时屏蔽DualMag
        QaudMode = 4,
        Reduce = 5,
        Mix = 6,
    };

    //计算prescan reduce scan
    static void CalcPreScanForReduceMode(double DwellTime,
        ushort ImageWidth,
        ushort ImageHeight,
        ushort StartX,
        ushort StartY,
        ushort& ReduceDacOffsetX,
        ushort& ReduceDacOffsetY,
        ushort& DacStepX,
        ushort& DacStepY,
        ushort& PreScanX,
        ushort& PreScanY);

    static ushort dacOffset;       //prescan 6560个 DAC step
    static ushort lineStartDelay;    //行扫描延迟 20us

    //计算prescan normal scan
    static void CalcPreScanArgs(double DwellTime,
        ushort ImageWidth,
        ushort ImageHeight,
        ushort& DacStepX,
        ushort& DacStepY,
        ushort& PreScanStepCountX,
        ushort& PreScanStepCountY,
        ushort DacOffset = 6560,
        ushort LineStartDelay = 20);
};

//扫描参数放到这里。放到全局变量也不合适。调试时暂时没有用到的，暂时不写。
//整个程序，保留三个参数位置。1.manager/state单例. 2.scanlib设置使用.所有Diss6库中的函数，都调用scanlib的参数 3.hal字典中保留
class ScanManager
{
public:
    // 获取单例实例
    static ScanManager& GetInstance() {
        static ScanManager instance; // 懒惰初始化
        return instance;
    }

    // 删除拷贝构造和赋值运算符，防止复制
    ScanManager(const ScanManager&) = delete;
    ScanManager& operator=(const ScanManager&) = delete;

    int GetImageSize(int imageSize)
    {
        switch (imageSize)
        {
        case 0:
            return 256;
        case 1:
            return 512;
        case 2:
            return 1024;
        case 3:
            return 2048;
        case 4:
            return 4096;
        case 5:
            return 8192;
        case 6:
            return 16384;
        default:
            return 1024;

        }
    }

#pragma region width, height, dwelltime,...
    int _reduceWidth = 300, _reduceHeight = 300, _startX = 0, _startY = 0, _imageWidth = 1024, _imageHeight = 1024;
    int _scanAvgModeLsv = 0, _averagesLsv = 1, _dwellLsv = 400;//实际操作的参数.. 发给扫描发生器设置.. fullImageWidth针对reduceScan

    int _selectDetLsv = 1, _selectDet1Lsv = 1, _selectDet2Lsv = 1, _selectDet3Lsv = 1;// 0-CCD, 1--ET--A1, 2--IN LENS--A2, 3--BSD--A3, 4--STEM--A4
    double _scanRotateLsv = 0;

    int _imageSizeLsv = 2, _brightnessLsv = 500, _contrastLsv = 500;

    int _currentSelectedChannel = -1;

    const double _scanVoltage = 3.5;
#pragma endregion

#pragma region 扫描参数
    int _targetScanState = 0, _scanStateLsv = 0;//状态量，当前扫描状态
    //_targetScanState 0:stop 1:NormalScan 5:ReduceScan
    Utils::ScanningMode _scanMode = Utils::ScanningMode::Normal;
    bool _isStopScan = false;//标志位，是否停止扫描。true,停止；false，不停止
#pragma endregion

#pragma region 探测器混合
    bool _isMixOn = false;
#pragma endregion


private:
    ScanManager() {};
};

class ScanState//所有扫描状态相关都放到这里
{
public:
    // 获取单例实例
    static ScanState& GetInstance() {
        static ScanState ins;
        return ins;
    }

    // 删除拷贝构造和赋值运算符，防止复制
    ScanState(const ScanState&) = delete;
    ScanState& operator=(const ScanState&) = delete;

private:
    ScanState() {};
};

//// 线程安全的消息队列
//class MessageQueue {
//private:
//    std::queue<std::string> queue;
//    std::mutex mutex;
//    std::condition_variable cond;
//
//public:
//    void push(const std::string& msg) {
//        std::unique_lock<std::mutex> lock(mutex);
//        queue.push(msg);
//        cond.notify_one();
//    }
//
//    std::string pop() {
//        std::unique_lock<std::mutex> lock(mutex);
//        cond.wait(lock, [this] { return !queue.empty(); });
//        std::string msg = queue.front();
//        queue.pop();
//        return msg;
//    }
//};

#endif // !SCANMAIN_H