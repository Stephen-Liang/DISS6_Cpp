#pragma once
#ifndef DISS6NATIVEMETHODS_HPP
#define DISS6NATIVEMETHODS_HPP

#include "libCommon.h"
#include "DISS6.h"
#include <string>
#include <iostream>

typedef int BOOL;
typedef void* HANDLE;

#pragma comment(lib, "DISS6Control64.dll")

namespace NativeMethods
{
	extern "C" __declspec(dllimport) RetCode_t _stdcall InitUSB(_In_ const char* pSerialNumber);
	extern "C" __declspec(dllimport) RetCode_t _stdcall InitTCP(_In_ const char* pIPAddress, _In_ uint16_t portSUP, _In_ uint16_t portDSCP, _In_ uint16_t portDIIP, _In_ uint16_t portDIDP, _In_ uint16_t portDVDP);

	// Basic scan job functions

	extern "C" __declspec(dllimport) RetCode_t _stdcall CreateImageScanJob(_In_ uint32_t channelCount, _In_ const ChannelInfo_t * channelConfig, _Out_ uint16_t & ScanId);
	extern "C" __declspec(dllimport) RetCode_t _stdcall CreatePixelMapScanJob(_In_ uint32_t channelCount, _In_ const ChannelInfo_t * channelConfig, _Out_ uint16_t & ScanId);

	// Functions for setting and getting general scan job parameters

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetFrameCount(_In_ uint16_t scanId, _In_ uint16_t frameCount);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetFrameCount(_In_ uint16_t scanId, _Out_ uint16_t & frameCount);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetFrameAveragingCount(_In_ uint16_t scanId, _In_ uint16_t frameAveragingCount);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetFrameAveragingCount(_In_ uint16_t scanId, _Out_ uint16_t & frameAveragingCount);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetAcquisitionTime(_In_ uint16_t scanId, _In_ uint32_t acquisitionTimeNs);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetAcquisitionTime(_In_ uint16_t scanId, _Out_ uint32_t & acquisitionTime);

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetBufferAveragingEnabled(_In_ uint16_t scanId, _In_ BOOL enabled);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetBufferAveragingEnabled(_In_ uint16_t scanId, _Out_ BOOL & enabled);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetKeepInternalScanEnabled(_In_ uint16_t scanId, _In_ BOOL enabled);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetKeepInternalScanEnabled(_In_ uint16_t scanId, _Out_ BOOL & enabled);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetAccumulateCounterValuesEnabled(_In_ uint16_t scanId, _In_ BOOL enabled);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetAccumulateCounterValuesEnabled(_In_ uint16_t scanId, _Out_ BOOL & enabled);

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetHardwareTriggerEnabled(_In_ uint16_t scanId, _In_ BOOL pixelTrigger, _In_ BOOL lineTrigger, _In_ BOOL frameTrigger);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetHardwareTriggerEnabled(_In_ uint16_t scanId, _Out_ BOOL & pixelTrigger, _Out_ BOOL & lineTrigger, _Out_ BOOL & frameTrigger);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetMainsTriggerEnabled(_In_ uint16_t scanId, _In_ BOOL pixelTrigger, _In_ BOOL lineTrigger, _In_ BOOL frameTrigger);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetMainsTriggerEnabled(_In_ uint16_t scanId, _Out_ BOOL & pixelTrigger, _Out_ BOOL & lineTrigger, _Out_ BOOL & frameTrigger);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetSoftwareTriggerEnabled(_In_ uint16_t scanId, _In_ BOOL pixelTrigger, _In_ BOOL lineTrigger, _In_ BOOL frameTrigger);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetSoftwareTriggerEnabled(_In_ uint16_t scanId, _Out_ BOOL & pixelTrigger, _Out_ BOOL & lineTrigger, _Out_ BOOL & frameTrigger);

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanMode(_In_ uint16_t scanId, _In_ uint8_t scanMode, _In_ uint16_t valueX, _In_ uint16_t valueY, _In_ uint16_t subPixelCountX, _In_ uint16_t _In_ subPixelCountY, _In_ uint32_t subPixelTime);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanMode(_In_ uint16_t scanId, _Out_ uint8_t & scanMode, _Out_ uint16_t & valueX, _Out_ uint16_t & valueY, _Out_ uint16_t & subPixelCountX, _Out_ uint16_t & subPixelCountY, _Out_ uint32_t & subPixelTime);

	// Functions for setting and getting image scan parameters

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetImageGeometry(_In_ uint16_t scanId, _In_ uint16_t imageWidth, _In_ uint16_t imageHeight, _In_ uint16_t dacOffsetX, _In_ uint16_t dacOffsetY, _In_ uint16_t dacIncrementX, _In_ uint16_t dacIncrementY, _In_ uint16_t preScanStepCountX, _In_ uint16_t preScanStepCountY);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetImageGeometry(_In_ uint16_t scanId, _Out_ uint16_t & imageWidth, _Out_ uint16_t & imageHeight, _Out_ uint16_t & dacOffsetX, _Out_ uint16_t & dacOffsetY, _Out_ uint16_t & dacIncrementX, _Out_ uint16_t & dacIncrementY, _Out_ uint16_t & preScanStepCountX, _Out_ uint16_t & preScanStepCountY);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetLineAveragingCount(_In_ uint16_t scanId, _In_ uint16_t lineAveragingCount);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetLineAveragingCount(_In_ uint16_t scanId, _Out_ uint16_t & lineAveragingCount);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetLineStartDelay(_In_ uint16_t scanId, _In_ TimeScale_t lineStartDelay);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetLineStartDelay(_In_ uint16_t scanId, _Out_ TimeScale_t & lineStartDelay);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetBeamReturnTiming(_In_ uint16_t scanId, _In_ uint16_t stepCount, _In_ TimeScale_t lineStepTime, _In_ TimeScale_t frameStepTime);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetBeamReturnTiming(_In_ uint16_t scanId, _Out_ uint16_t & stepCount, _Out_ TimeScale_t & lineStepTime, _Out_ TimeScale_t & frameStepTime);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetSettlingTime(_In_ uint16_t scanId, _In_ TimeScale_t settlingTime);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetSettlingTime(_In_ uint16_t scanId, _Out_ TimeScale_t & settlingTime);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetHoldTime(_In_ uint16_t scanId, _In_ TimeScale_t holdTime);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetHoldTime(_In_ uint16_t scanId, _Out_ TimeScale_t & holdTime);

	// Functions for setting and getting pixel map parameters

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetPixelMapValues(_In_ uint16_t scanId, _In_ uint32_t pixelCount, _In_ const uint64_t * values);

	// Event setting functions

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetEventDataReady(_In_ uint16_t scanId, _In_ HANDLE eventDataReady);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetEventScanJobStarted(_In_ uint16_t scanId, _In_ HANDLE eventScanJobStarted);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetEventScanJobFinished(_In_ uint16_t scanId, _In_ HANDLE eventScanJobFinished);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetEventScanJobAborted(_In_ uint16_t scanId, _In_ HANDLE eventScanJobAborted);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetEventConnectionLost(_In_ HANDLE eventConnectionLost);

	// General scan job control functions

	extern "C" __declspec(dllimport) RetCode_t _stdcall StartScanJob(_In_ uint16_t scanId, uint8_t abortCodeForActiveScan);
	extern "C" __declspec(dllimport) RetCode_t _stdcall StopScanJob(_In_ uint16_t scanId, _In_ uint8_t abortCode);
	extern "C" __declspec(dllimport) RetCode_t _stdcall RemoveScanJob(_In_ uint16_t scanId);
	extern "C" __declspec(dllimport) RetCode_t _stdcall PrepareScanJob(_In_ uint16_t scanId);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanState(_In_ uint16_t scanId, _Out_ uint8_t & state);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetChannelCount(_In_ uint16_t scanId, _Out_ uint32_t & channelCount);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetChannelInfo(_In_ uint16_t scanId, _In_ uint32_t channelIdx, _Out_ ChannelInfo_t & channelInfo);
	extern "C" __declspec(dllimport) RetCode_t _stdcall ReadChannelData(_In_ uint16_t scanId, _In_ void** pDestination, _Inout_ uint32_t * pPixelCount, _In_ uint32_t flags, _Out_ uint32_t * pFrameIndex, _Out_ uint32_t * pPixelOffset, _Out_ uint32_t * pChannelStatus);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanResult(_In_ uint16_t scanId, _Out_ uint8_t & result);

	// Common scan job functions, apply for all types of scan jobs

	extern "C" __declspec(dllimport) RetCode_t _stdcall RemoveCompletedScans(void);
	extern "C" __declspec(dllimport) RetCode_t _stdcall RemoveAllScans(void);
	extern "C" __declspec(dllimport) RetCode_t _stdcall AbortAllScans(uint8_t abortCondition);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanProgress(_Out_ uint32_t & progress);

	// Pause and resume functions

	extern "C" __declspec(dllimport) RetCode_t _stdcall Pause(void);
	extern "C" __declspec(dllimport) RetCode_t _stdcall Resume(void);
	extern "C" __declspec(dllimport) RetCode_t _stdcall IsPaused(_Out_ BOOL & isPaused);

	// Software trigger

	extern "C" __declspec(dllimport) RetCode_t _stdcall TriggerNextPixel(_In_ uint16_t scanId);
	extern "C" __declspec(dllimport) RetCode_t _stdcall TriggerNextLine(_In_ uint16_t scanId);
	extern "C" __declspec(dllimport) RetCode_t _stdcall TriggerNextFrame(_In_ uint16_t scanId);

	// Support functions

	extern "C" __declspec(dllimport) uint16_t _stdcall GetAPIVersion(void);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetFirmwareRevision(_Out_ uint16_t & pVersion);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetHardwareRevision(_Out_ uint8_t & pVersion);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetHardwareOptions(_Out_ uint32_t & pOptionFlags);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetDeviceSerialNumber(_Out_ char* pSerialNumber, _Inout_ uint32_t & nLength);
	extern "C" __declspec(dllimport) uint32_t _stdcall GetLastErrorMessage(_Out_ wchar_t* pMessage, _In_ uint32_t nLength);

	// TCP/IP configuration functions

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetTCPEnabled(_In_ BOOL bSet);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetTCPEnabled(_Out_ BOOL & isSet);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetDHCPEnabled(_In_ BOOL bSet);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetDHCPEnabled(_Out_ BOOL & isSet);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetIPAddress(_In_ const char* pIPv4Address);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetIPAddress(_Out_ char* pIPv4Address, _Inout_ uint32_t & nLength);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetIPNetMask(_In_ const char* pIPv4NetMask);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetIPNetMask(_Out_ char* pIPv4NetMask, _Inout_ uint32_t & nLength);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetTCPPortNumbers(_In_ uint16_t portSUP, _In_ uint16_t portDSCP, _In_ uint16_t portDIIP, _In_ uint16_t portDIDP, _In_ uint16_t portDVDP);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetTCPPortNumbers(_Out_ uint16_t & portSUP, _Out_ uint16_t & portDSCP, _Out_ uint16_t & portDIIP, _Out_ uint16_t & portDIDP, _Out_ uint16_t & portDVDP);

	// Idle scan setup functions

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetIdleScanTimeout(_In_ uint16_t timeOut);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetIdleScanTimeout(_Out_ uint16_t & timeOut);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetIdleScanChannel(_In_ ChannelInfo_t channel);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetIdleScanChannel(_Out_ ChannelInfo_t & channel);

	// Miscellaneous setup functions

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanOffPosition(_In_ uint16_t DACPosX, _In_ uint16_t DACPosY);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanOffPosition(_Out_ uint16_t & DACPosX, _Out_ uint16_t & DACPosY);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetADCDelay(_In_ uint16_t ADCDelay);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetADCDelay(_Out_ uint16_t & ADCDelay);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetExtScanDelay(_In_ TimeScale_t delay);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetExtScanDelay(_Out_ TimeScale_t & delay);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetExtBlankDelay(_In_ TimeScale_t delay);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetExtBlankDelay(_Out_ TimeScale_t & delay);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetTimeConstantAutomatic(_In_ BOOL bSet);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetTimeConstantAutomatic(_Out_ BOOL & bSet);

	// Adjustment functions

	extern "C" __declspec(dllimport) RetCode_t _stdcall SaveDeviceSettings(void);
	extern "C" __declspec(dllimport) RetCode_t _stdcall LoadDeviceSettings(void);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanPolarity(_Out_ BOOL & inverted_x, _Out_ BOOL & inverted_y);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanPolarity(_In_ BOOL inverted_x, _In_ BOOL inverted_y);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanGainXRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanGainX(_In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanGainYRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanGainY(_In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanOffsetXRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanOffsetX(_In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanOffsetYRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanOffsetY(_In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetFastAGainRange(_In_ uint8_t index, _Out_ float& value, _Out_ float& min, _Out_ float& max);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetFastAGain(_In_ uint8_t index, _In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetFastBGainRange(_In_ uint8_t index, _Out_ float& value, _Out_ float& min, _Out_ float& max);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetFastBGain(_In_ uint8_t index, _In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetFastAInputOffsetRange(_In_ uint8_t index, _Out_ float& value, _Out_ float& min, _Out_ float& max);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetFastAInputOffset(_In_ uint8_t index, _In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetFastBInputOffsetRange(_In_ uint8_t index, _Out_ float& value, _Out_ float& min, _Out_ float& max);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetFastBInputOffset(_In_ uint8_t index, _In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetVideoGainRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetVideoGain(_In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetVideoInputOffsetRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetVideoInputOffset(_In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanMagXRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanMagX(_In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanMagYRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanMagY(_In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanShiftXRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanShiftX(_In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanShiftYRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanShiftY(_In_ float value);

	// Scan Transformation functions

	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanTransformationEnabled(_Out_ BOOL & enabled);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanTransformationEnabled(_In_ BOOL enabled);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanTransformationFlags(_Out_ uint8_t & value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanTransformationFlags(_In_ uint8_t value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanRotationEnabled(_Out_ BOOL & enabled);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanRotationEnabled(_In_ BOOL enabled);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanRotationAngle(_Out_ float& value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanRotationAngle(_In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanRotationAngleOffset(_Out_ float& value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanRotationAngleOffset(_In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanSkewAngle(_Out_ float& value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanSkewAngle(_In_ float value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanTiltEnabled(_Out_ BOOL & enabled);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanTiltEnabled(_In_ BOOL enabled);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetScanTiltAngle(_Out_ float& value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetScanTiltAngle(_In_ float value);

	// Video functions 
	// Video refers to the signal of a chamber camera that is sent to the client, i.e. this library
	// Video can also be shown on the HDMI monitor

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetVideoEnabled(_In_ BOOL bSet);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetVideoEnabled(_Out_ BOOL & bSet);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetVideoResolution(_In_ uint16_t columnCount, _In_ uint16_t rowCount);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetVideoResolution(_Out_ uint16_t & columnCount, _Out_ uint16_t & rowCount);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetVideoDeinterlacingMode(_In_ uint8_t mode);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetVideoDeinterlacingMode(_Out_ uint8_t & mode);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetVideoAcqTime(_In_ uint32_t timeNs);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetVideoAcqTime(_Out_ uint32_t & time);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetEventVideoFrameReady(_In_ HANDLE eventVideoFrameReady);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetVideoFrame(_Out_ uint8_t * pBufferStart);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetVideoHOffset(_In_ uint16_t value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetVideoHOffset(_Out_ uint16_t & value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetVideoVOffset(_In_ uint16_t value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetVideoVOffset(_Out_ uint16_t & value);

	// DVI functions

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetDVIEnabled(_In_ BOOL bSet);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetDVIEnabled(_Out_ BOOL & isSet);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetDVISource(_In_ uint8_t channel_source, _In_ uint8_t channel_index);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetDVISource(_Out_ uint8_t & channel_source, _Out_ uint8_t & channel_index);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetDVIMode(_In_ uint8_t /* HDMIMode_t */ mode);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetDVIMode(_Out_ uint8_t & /* HDMIMode_t */ mode);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetDVIModeLine(_In_ uint8_t index, _In_ const char* pText);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetDVIResolution(_In_ uint8_t resolution);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetDVIResolution(_Out_ uint8_t & resolution);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetDVIText(_In_ uint8_t index, _In_ const char* pText);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetDVIText(_In_ uint8_t index, _Out_ char* pText, _Inout_ uint32_t & nLength);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetDVIDacStepSize(_In_ float size);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetDVIDacStepSize(_Out_ float& size);

	// Clock signal functions

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetClockInvertMask(_In_ uint8_t mask);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetClockInvertMask(_Out_ uint8_t & mask);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetPixelClockLength(_In_ TimeScale_t clockLength);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetPixelClockLength(_Out_ TimeScale_t & clockLength);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetLineClockLength(_In_ TimeScale_t clockLength);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetLineClockLength(_Out_ TimeScale_t & clockLength);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetFrameClockLength(_In_ TimeScale_t clockLength);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetFrameClockLength(_Out_ TimeScale_t & clockLength);

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetSyncEdgeInvertMask(_In_ uint16_t mask);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetSyncEdgeInvertMask(_Out_ uint16_t & mask);

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetSyncDebounce(_In_ uint8_t value);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetSyncDebounce(_Out_ uint8_t & value);

	// Hardware Pause/Resume Enabled

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetHardwarePauseResumeEnabled(_In_ BOOL enabled);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetHardwarePauseResumeEnabled(_Out_ BOOL & enabled);

	// Lock-In Amplifier Functions

	extern "C" __declspec(dllimport) RetCode_t _stdcall SetLIAEnabled(_In_ BOOL bSet);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetLIAEnabled(_Out_ BOOL & pIsSet);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetLIADigitalGain(_In_ uint8_t gain);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetLIADigitalGain(_Out_ uint8_t & gain);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetLIAClockMode(_In_ uint8_t clockMode);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetLIAClockMode(_Out_ uint8_t & clockMode);
	extern "C" __declspec(dllimport) RetCode_t _stdcall SetLIAReferenceFrequency(_In_ uint32_t frequency);
	extern "C" __declspec(dllimport) RetCode_t _stdcall GetLIAReferenceFrequency(_Out_ uint32_t & frequency);
}
#endif // !DISS6NATIVEMETHODS_HPP
