#pragma once
#ifndef SCANJOB_H
#define SCANJOB_H

#include <Windows.h>
#include <thread>
#include <vector>
#include "DISS6.h"
#include "DISS6ControlTypes.h"
#include "DISS6BasicFunc.hpp"
#include "CallBacks.h"
#include "CommonTools.h"

enum FastChannelSelection
{
    FAST_A = 0,
    FAST_B = 1
};

class ScanJob
{
public:
	ScanJob(const std::vector<ChannelInfo_t>& Channels);
    ~ScanJob();
    std::vector<ChannelInfo_t> _Channels;

    ushort FrameCount;
    ushort getFrameCount() const
    {
        return DISS6Control::Job::GetFrameCount(_Handle);
    }
    void setFrameCount(ushort value)
    {
        DISS6Control::Job::SetFrameCount(_Handle, value);
    }

    TimeScale LineStartDelay;
    TimeScale_t& getLineStartDelay() const
    {
        return DISS6Control::Job::GetLineStartDelay(_Handle);
    }
    void setLineStartDelay(TimeScale value)
    {
        DISS6Control::Job::SetLineStartDelay(_Handle, value);
    }

    uint AcquisitionTime;
    uint getAcquisitionTime() const
    {
        return DISS6Control::Job::GetAcquisitionTime(_Handle);
    }
    void setAcquisitionTime(uint value)
    {
        DISS6Control::Job::SetAcquisitionTime(_Handle, value);
    }

    void SetImageGeometry(ushort ImageWidth, ushort ImageHeight, ushort DacOffsetX, ushort DacOffsetY, ushort DacIncrementX, ushort DacIncrementY, ushort PreScanStepCountX, ushort PreScanStepCountY);
    void SetBeamReturnTiming(ushort StepCount, TimeScale LineStepTime, TimeScale FrameStepTime);

    uint ReadChannelData(void** ChannelBuffers, uint& PixelCount, uint& FrameIndex, uint& PixelOffset, uint& ChannelStatus)
    {
        return DISS6Control::Job::ReadChannelData(_Handle, ChannelBuffers, &PixelCount, ReadChannelDataFlags::UsePixelOffset, &FrameIndex, &PixelOffset, &ChannelStatus);
    }

    void GetFastAGainRange(byte Index, float& Value, float& Min, float& Max)
    {
        DISS6Control::Adjustment::FastA::GetGainRange(Index, Value, Min, Max);
    }

    void SetFastAGain(byte Index, float Value)
    {
        DISS6Control::Adjustment::FastA::SetGain(Index, Value);
    }

    void GetFastAInputOffset(byte Index, float& Value, float& Min, float& Max)
    {
        DISS6Control::Adjustment::FastA::GetInputOffsetRange(Index, Value, Min, Max);
    }

    void SetFastAInputOffset(byte Index, float Value)
    {
        DISS6Control::Adjustment::FastA::SetInputOffset(Index, Value);
    }

    void SetGuiFinished()
    {
        //SetEvent(GUISetEvent);
    }

    // Scan Rotation Function
    void GetScanRotationAngle(float& Angle)
    {
        Angle = DISS6Control::Scan::Rotation::GetAngle();
    }

    void SetScanRotationAngle(float Angle)
    {
        DISS6Control::Scan::Rotation::SetAngle(Angle);
    }

    void GetScanRotationOffsetAngle(float& OffsetAngle)
    {
        OffsetAngle = DISS6Control::Scan::Rotation::GetAngleOffset();
    }

    void SetScanRotationOffsetAngle(float OffsetAngle)
    {
        DISS6Control::Scan::Rotation::SetAngleOffset(OffsetAngle);
    }

    void GetSkewAngle(float& Angle)
    {
        Angle = DISS6Control::Scan::Skew::GetAngle();
    }

    void SetSkewAngle(float Angle)
    {
        DISS6Control::Scan::Skew::SetAngle(Angle);
    }

    void SetScanMode(ScanMode ScanMode, ushort ValueX, ushort ValueY, ushort SubPixelCountX, ushort SubPixelCountY, uint SubPixelTime)
    {
        DISS6Control::Job::SetScanMode(_Handle, ScanMode, ValueX, ValueY, SubPixelCountX = 1, SubPixelCountY = 1, SubPixelTime = 10);
    }

    void Start(AbortCode Code);
    void DataTaskProc();
    void StopThreads();
    void OnScanJobDataAvailable();
    void OnDataTimeout();

    void Dispose(bool disposing);

    //设置回调函数
    void SetDataAvailableCallback(VoidFuncCallback callback);
    void SetScanJobRunningCallback(VoidFuncCallback callback);
    void SetScanJobFinishedCallback(VoidFuncCallback callback);
    void SetScanJobTimeOutCallback(VoidFuncCallback callback);

    uint16_t _Handle;
    ushort _ImageWidth;
    ushort _ImageHeight;

    std::thread controlThread;

protected:
    void CreateHandle(const std::vector<ChannelInfo_t>& Channels);
    void RegisterEvents();

private:
    void ControlTaskProc();
    void StartControlThread();
    void StopControlThread();
    void OnScanJobRunning();
    void OnScanJobFinished(bool aborted);

    //Event
    HANDLE JobStartedEvent = CreateEvent(nullptr, false, false, nullptr);//第二个参数false自动复位为无信号状态。第三个参数false表示初始时无信号状态。
    HANDLE DataReadyEvent = CreateEvent(nullptr, false, false, nullptr);
    HANDLE JobFinishedEvent = CreateEvent(nullptr, false, false, nullptr);
    HANDLE JobAbortedEvent = CreateEvent(nullptr, false, false, nullptr);

    HANDLE TaskCancelEvent = CreateEvent(nullptr, false, false, nullptr);
    HANDLE DataTaskReadyEvent = CreateEvent(nullptr, false, false, nullptr);
    HANDLE Semaphore = CreateSemaphore(nullptr, 1, 1, nullptr);

    std::thread dataTaskThread;//模拟datatask
    bool dataTaskActive = false;
    bool disposedValue = false;

    VoidFuncCallback m_dataAvailableCallback;
    VoidFuncCallback m_scanJobRunningCallback;
    VoidFuncCallback m_scanJobFinishedCallback;
    VoidFuncCallback m_scanJobTimeOutCallback;
};

#endif // !SCANJOB_H