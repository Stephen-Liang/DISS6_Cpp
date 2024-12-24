#pragma once
#ifndef DISS6BASICFUNC_H
#define DISS6BASICFUNC_H

#include "libCommon.h"
#include "DISS6ControlErrors.hpp"
#include "DISS6ControlTypes.h"
#include "DISS6NativeMethods.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <exception>
#include <vector>

class DISS6Control
{
public:
    static void DISS6InitUSB(const char* serialNum = "")
    {
        CheckRetCode(NativeMethods::InitUSB(serialNum));
        std::cout << "Init OK" << std::endl;
    }

    static void CheckRetCode(RetCode_t code)
    {
        if (code == DC_RET_NO_ERROR)
        {
            return;
        }

        std::wstring ErrMsg = LastErrorMessage();

        std::ostringstream oss;
        switch (code) {
        case DC_RET_NOT_INITIALIZED:
            oss << "DISS6Control library is not initialized (";// << static_cast<unsigned int>(code) << ")" << ErrMsg;
            throw new NotInitializedException(oss.str());
            break;
        case DC_RET_ALREADY_INITIALIZED:
            oss << "DISS6Control library is already initialized (";// << static_cast<unsigned int>(code) << ")" << ErrMsg;
            throw new NotInitializedException(oss.str());
            break;
        case DC_RET_CANNOT_LOCATE_DEVICE:
            oss << "DISS6 device not found (";// << static_cast<unsigned int>(code) << ")" << ErrMsg;
            throw new DeviceNotFoundException(oss.str());
            break;
        default:
            oss << "DISS6Control error (";// << static_cast<unsigned int>(code) << ")" << ErrMsg;
            throw new LibException(oss.str(), code);
            break;
        }
    }

    static std::wstring LastErrorMessage()
    {
        std::vector<wchar_t> buffer(128);
        //std::string errorMessage(128, "\n"); // 创建一个具有初始大小的字符串
        // 调用函数并传递字符串的引用和期望的长度
        uint length = NativeMethods::GetLastErrorMessage(buffer.data(), buffer.size());

        // 如果返回的长度小于初始大小，调整字符串大小
        if (length > 0 && length <= buffer.size()) {
            // 返回错误消息字符串
            return std::wstring(buffer.data(), length);
        }
        else {
            // 错误消息长度为 0 或超出预期，返回空字符串
            return std::wstring();
        }
    }

    class Job
    {
    public:
        static uint16_t CreateImageScan(const std::vector<ChannelInfo_t>& Channels)
        {
            uint16_t ScanJobHandler;

            CheckRetCode(NativeMethods::CreateImageScanJob((uint32_t)Channels.size(), Channels.data(), ScanJobHandler));

            return ScanJobHandler;
        }

        static void SetFrameCount(ushort ScanJobHandle, ushort Value)
        {
            CheckRetCode(NativeMethods::SetFrameCount(ScanJobHandle, Value));
        }

        static ushort GetFrameCount(ushort ScanJobHandle)
        {
            ushort value;
            CheckRetCode(NativeMethods::GetFrameCount(ScanJobHandle, value));
            return value;
        }

        static void SetFrameAveragingCount(ushort ScanJobHandle, ushort Value)
        {
            CheckRetCode(NativeMethods::SetFrameAveragingCount(ScanJobHandle, Value));
        }

        static ushort GetFrameAveragingCount(ushort ScanJobHandle)
        {
            ushort value;
            CheckRetCode(NativeMethods::GetFrameAveragingCount(ScanJobHandle, value));
            return value;
        }

        static void SetAcquisitionTime(ushort ScanJobHandle, uint Value)
        {
            CheckRetCode(NativeMethods::SetAcquisitionTime(ScanJobHandle, Value));
        }

        static uint GetAcquisitionTime(ushort ScanJobHandle)
        {
            uint value;
            CheckRetCode(NativeMethods::GetAcquisitionTime(ScanJobHandle, value));
            return value;
        }

        static void SetBufferAveragingEnabled(ushort ScanJobHandle, bool Enabled)
        {
            CheckRetCode(NativeMethods::SetBufferAveragingEnabled(ScanJobHandle, Enabled ? -1 : 0));
        }

        static bool GetBufferAveragingEnabled(ushort ScanJobHandle)
        {
            int enabled;
            CheckRetCode(NativeMethods::GetBufferAveragingEnabled(ScanJobHandle, enabled));
            return enabled != 0 ? true : false;
        }

        static void SetScanMode(ushort ScanJobHandle, ScanMode ScanMode, ushort ValueX, ushort ValueY, ushort SubPixelCountX, ushort SubPixelCountY, uint SubPixelTime)
        {
            CheckRetCode(NativeMethods::SetScanMode(ScanJobHandle, ScanMode, ValueX, ValueY, SubPixelCountX, SubPixelCountY, SubPixelTime));
        }

        static void SetImageGeometry(ushort ScanJobHandle, ushort ImageWidth, ushort ImageHeight, ushort DacOffsetX, ushort DacOffsetY, ushort DacIncrementX, ushort DacIncrementY, ushort PreScanStepCountX, ushort PreScanStepCountY)
        {
            CheckRetCode(NativeMethods::SetImageGeometry(ScanJobHandle, ImageWidth, ImageHeight, DacOffsetX, DacOffsetY, DacIncrementX, DacIncrementY, PreScanStepCountX, PreScanStepCountY));
        }

        static void SetLineAveragingCount(ushort ScanJobHandle, ushort Value)
        {
            CheckRetCode(NativeMethods::SetLineAveragingCount(ScanJobHandle, Value));
        }

        static void SetLineStartDelay(ushort ScanJobHandle, TimeScale Value)
        {
            CheckRetCode(NativeMethods::SetLineStartDelay(ScanJobHandle, Value));
        }

        static TimeScale_t& GetLineStartDelay(ushort ScanJobHandle)
        {
            TimeScale_t value;
            CheckRetCode(NativeMethods::GetLineStartDelay(ScanJobHandle, value));
            return value;
        }

        static void SetBeamReturnTiming(ushort ScanJobHandle, ushort StepCount, TimeScale LineStepTime, TimeScale FrameStepTime)
        {
            CheckRetCode(NativeMethods::SetBeamReturnTiming(ScanJobHandle, StepCount, LineStepTime, FrameStepTime));
        }

        static void SetSettlingTime(ushort ScanJobHandle, TimeScale Value)
        {
            CheckRetCode(NativeMethods::SetSettlingTime(ScanJobHandle, Value));
        }

        static TimeScale_t& GetSettlingTime(ushort ScanJobHandle)
        {
            TimeScale_t value;
            CheckRetCode(NativeMethods::GetSettlingTime(ScanJobHandle, value));
            return value;
        }

        static void SetHoldTime(ushort ScanJobHandle, TimeScale Value)
        {
            CheckRetCode(NativeMethods::SetHoldTime(ScanJobHandle, Value));
        }

        static TimeScale_t& GetHoldTime(ushort ScanJobHandle)
        {
            TimeScale_t value;
            CheckRetCode(NativeMethods::GetHoldTime(ScanJobHandle, value));
            return value;
        }

        //Event setting functions
        static void RegisterDataReadyEvent(ushort ScanJobHandle, HANDLE Event)
        {
            CheckRetCode(NativeMethods::SetEventDataReady(ScanJobHandle, Event));
        }

        static void RegisterJobStartedEvent(ushort ScanJobHandle, HANDLE Event)
        {
            CheckRetCode(NativeMethods::SetEventScanJobStarted(ScanJobHandle, Event));
        }

        static void RegisterJobFinishedEvent(ushort ScanJobHandle, HANDLE Event)
        {
            CheckRetCode(NativeMethods::SetEventScanJobFinished(ScanJobHandle, Event));
        }

        static void RegisterJobAbortedEvent(ushort ScanJobHandle, HANDLE Event)
        {
            CheckRetCode(NativeMethods::SetEventScanJobAborted(ScanJobHandle, Event));
        }

        static void Start(ushort ScanJobHandle, AbortCode AbortCodeForActiveScan)
        {
            CheckRetCode(NativeMethods::StartScanJob(ScanJobHandle, AbortCodeForActiveScan));
        }

        static void Stop(ushort ScanJobHandle, AbortCode AbortCode)
        {
            CheckRetCode(NativeMethods::StopScanJob(ScanJobHandle, AbortCode));
        }

        static void Remove(ushort ScanJobHandle)
        {
            CheckRetCode(NativeMethods::RemoveScanJob(ScanJobHandle));
        }

        static void Prepare(ushort ScanJobHandle)
        {
            CheckRetCode(NativeMethods::PrepareScanJob(ScanJobHandle));
        }

        static uint ReadChannelData(uint16_t ScanJobHandle, void** ChannelBuffers, uint32_t* PixelCount, uint Flags, uint32_t* FrameIndex, uint32_t* PixelOffset, uint32_t* ChannelStatus)
        {
            CheckRetCode(NativeMethods::ReadChannelData(ScanJobHandle, ChannelBuffers, PixelCount, Flags, FrameIndex, PixelOffset, ChannelStatus));
            return *PixelCount;
        }

        static AbortCode GetResult(ushort ScanJobHandle)
        {
            /*AbortCode value = AbortCode::None;*/
            uint8_t value;
            CheckRetCode(NativeMethods::GetScanResult(ScanJobHandle, value));
            return static_cast<AbortCode>(value);
        }

        static void AbortAll(byte AbortCondition)
        {
            CheckRetCode(NativeMethods::AbortAllScans(AbortCondition));
        }
    };

    class Adjustment
    {
    public:
        static void SaveSettings()
        {
            CheckRetCode(NativeMethods::SaveDeviceSettings());
        }
        
        static void LoadSettings()
        {
            CheckRetCode(NativeMethods::LoadDeviceSettings());
        }

        class Scan
        {
        public:
            static void SetOffPosition(ushort DACPosX, ushort DACPosY)
            {
                CheckRetCode(NativeMethods::SetScanOffPosition(DACPosX, DACPosY));
            }

            static void GetOffPosition(ushort& DACPosX, ushort& DACPosY)
            {
                CheckRetCode(NativeMethods::GetScanOffPosition(DACPosX, DACPosY));
            }

            static void SetPolarity(bool InvertedX, bool InvertedY)
            {
                CheckRetCode(NativeMethods::SetScanPolarity(InvertedX ? -1 : 0, InvertedY ? -1 : 0));
            }

            static void GetPolarity(bool& InvertedX, bool& InvertedY)
            {
                //bool invX, invY;
                //CheckRetCode(NativeMethods::GetScanPolarity(&invX, &invY));
                //InvertedX = invX != 0 ? true : false;
                //InvertedY = invY != 0 ? true : false;
            }

            static void SetGainX(float Value)
            {
                CheckRetCode(NativeMethods::SetScanGainX(Value));
            }

            static void GetGainXRange(float& Value, float& Min, float& Max)
            {
                CheckRetCode(NativeMethods::GetScanGainXRange(Value, Min, Max));
            }

            static void SetGainY(float Value)
            {
                CheckRetCode(NativeMethods::SetScanGainY(Value));
            }

            static void GetGainYRange(float& Value, float& Min, float& Max)
            {
                CheckRetCode(NativeMethods::GetScanGainYRange(Value, Min, Max));
            }

            static void SetOffsetX(float Value)
            {
                CheckRetCode(NativeMethods::SetScanOffsetX(Value));
            }

            static void GetOffsetXRange(float& Value, float& Min, float& Max)
            {
                CheckRetCode(NativeMethods::GetScanOffsetXRange(Value, Min, Max));
            }

            static void SetOffsetY(float Value)
            {
                CheckRetCode(NativeMethods::SetScanOffsetY(Value));
            }

            static void GetOffsetYRange(float& Value, float& Min, float& Max)
            {
                CheckRetCode(NativeMethods::GetScanOffsetYRange(Value, Min, Max));
            }
        };

        class FastA
        {
        public:
            static void SetGain(byte Index, float Value)
            {
                CheckRetCode(NativeMethods::SetFastAGain(Index, Value));
            }

            static void GetGainRange(byte Index, float& Value, float& Min, float& Max)
            {
                CheckRetCode(NativeMethods::GetFastAGainRange(Index, Value, Min, Max));
            }

            static void SetInputOffset(byte Index, float Value)
            {
                CheckRetCode(NativeMethods::SetFastAInputOffset(Index, Value));
            }

            static void GetInputOffsetRange(byte Index, float& Value, float& Min, float& Max)
            {
                CheckRetCode(NativeMethods::GetFastAInputOffsetRange(Index, Value, Min, Max));
            }
        };

        class FastB
        {
            static void SetGain(byte Index, float Value)
            {

            }

            static void GetGainRange(byte Index, float& Value, float& Min, float& Max)
            {

            }

            static void SetInputOffset(byte Index, float Value)
            {

            }

            static void GetInputOffsetRange(byte Index, float& Value, float& Min, float& Max)
            {

            }
        };

        class Video
        {
            static void SetGain(byte Index, float Value)
            {

            }

            static void GetGainRange(byte Index, float& Value, float& Min, float& Max)
            {

            }

            static void SetInputOffset(byte Index, float Value)
            {

            }

            static void GetInputOffsetRange(byte Index, float& Value, float& Min, float& Max)
            {

            }
        };
    
        ushort ADCDelay;

        TimeScale ExtScanDelay;

        TimeScale ExtBlankDelay;

        bool TimeConstantAutomatic;

    };

    class Scan
    {
    public:
        class Rotation
        {
        public:
            // 静态方法来启用/禁用旋转功能
            static void SetEnabled(bool value) {
                CheckRetCode(NativeMethods::SetScanRotationEnabled(value ? -1 : 0));
            }

            static bool GetEnabled() {
                int enabled;
                CheckRetCode(NativeMethods::GetScanRotationEnabled(enabled));
                return enabled != 0;
            }

            // 静态方法设置/获取角度
            static void SetAngle(float value) {
                CheckRetCode(NativeMethods::SetScanRotationAngle(value));
            }

            static float GetAngle() {
                float value;
                CheckRetCode(NativeMethods::GetScanRotationAngle(value));
                return value;
            }

            // 静态方法设置/获取角度偏移
            static void SetAngleOffset(float value) {
                CheckRetCode(NativeMethods::SetScanRotationAngleOffset(value));
            }

            static float GetAngleOffset() {
                float value;
                CheckRetCode(NativeMethods::GetScanRotationAngleOffset(value));
                return value;
            }
        };
    
        class Skew
        {
        public:
            // 静态方法设置角度
            static void SetAngle(float value) {
                CheckRetCode(NativeMethods::SetScanSkewAngle(value));
            }

            // 静态方法获取角度
            static float GetAngle() {
                float value;
                CheckRetCode(NativeMethods::GetScanSkewAngle(value));
                return value;
            }
        };
    };
};

#endif