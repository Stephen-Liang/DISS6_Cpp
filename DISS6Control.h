#pragma once
#ifndef DISS6CONTROL_H
#define DISS6CONTROL_H
// DISS6Control Library Interface Functions

#include "libCommon.h"
#include "DISS6.h"
#include <minwindef.h>

// Force C-style name mangling
#ifdef __cplusplus 
extern "C" {
#endif

// Library initialization functions

DC_API RetCode_t DC_CALLCONV InitUSB(_In_ const char* pSerialNumber);
DC_API RetCode_t DC_CALLCONV InitTCP(_In_ const char* pIPAddress, _In_ uint16_t portSUP, _In_ uint16_t portDSCP, _In_ uint16_t portDIIP, _In_ uint16_t portDIDP, _In_ uint16_t portDVDP);

// Basic scan job functions

DC_API RetCode_t DC_CALLCONV CreateImageScanJob(_In_ uint32_t channelCount, _In_ const ChannelInfo_t* channelConfig, _Out_ uint16_t& ScanId);
DC_API RetCode_t DC_CALLCONV CreatePixelMapScanJob(_In_ uint32_t channelCount, _In_ const ChannelInfo_t* channelConfig, _Out_ uint16_t& ScanId);

// Functions for setting and getting general scan job parameters

DC_API RetCode_t DC_CALLCONV SetFrameCount(_In_ uint16_t scanId, _In_ uint16_t frameCount);
DC_API RetCode_t DC_CALLCONV GetFrameCount(_In_ uint16_t scanId, _Out_ uint16_t& frameCount);
DC_API RetCode_t DC_CALLCONV SetFrameAveragingCount(_In_ uint16_t scanId, _In_ uint16_t frameAveragingCount);
DC_API RetCode_t DC_CALLCONV GetFrameAveragingCount(_In_ uint16_t scanId, _Out_ uint16_t& frameAveragingCount);
DC_API RetCode_t DC_CALLCONV SetAcquisitionTime(_In_ uint16_t scanId, _In_ uint32_t acquisitionTimeNs);
DC_API RetCode_t DC_CALLCONV GetAcquisitionTime(_In_ uint16_t scanId, _Out_ uint32_t& acquisitionTime);

DC_API RetCode_t DC_CALLCONV SetBufferAveragingEnabled(_In_ uint16_t scanId, _In_ BOOL enabled);
DC_API RetCode_t DC_CALLCONV GetBufferAveragingEnabled(_In_ uint16_t scanId, _Out_ BOOL& enabled);
DC_API RetCode_t DC_CALLCONV SetKeepInternalScanEnabled(_In_ uint16_t scanId, _In_ BOOL enabled);
DC_API RetCode_t DC_CALLCONV GetKeepInternalScanEnabled(_In_ uint16_t scanId, _Out_ BOOL& enabled);
DC_API RetCode_t DC_CALLCONV SetAccumulateCounterValuesEnabled(_In_ uint16_t scanId, _In_ BOOL enabled);
DC_API RetCode_t DC_CALLCONV GetAccumulateCounterValuesEnabled(_In_ uint16_t scanId, _Out_ BOOL& enabled);

DC_API RetCode_t DC_CALLCONV SetHardwareTriggerEnabled(_In_ uint16_t scanId, _In_ BOOL pixelTrigger, _In_ BOOL lineTrigger, _In_ BOOL frameTrigger);
DC_API RetCode_t DC_CALLCONV GetHardwareTriggerEnabled(_In_ uint16_t scanId, _Out_ BOOL& pixelTrigger, _Out_ BOOL& lineTrigger, _Out_ BOOL& frameTrigger);
DC_API RetCode_t DC_CALLCONV SetMainsTriggerEnabled(_In_ uint16_t scanId, _In_ BOOL pixelTrigger, _In_ BOOL lineTrigger, _In_ BOOL frameTrigger);
DC_API RetCode_t DC_CALLCONV GetMainsTriggerEnabled(_In_ uint16_t scanId, _Out_ BOOL& pixelTrigger, _Out_ BOOL& lineTrigger, _Out_ BOOL& frameTrigger);
DC_API RetCode_t DC_CALLCONV SetSoftwareTriggerEnabled(_In_ uint16_t scanId, _In_ BOOL pixelTrigger, _In_ BOOL lineTrigger, _In_ BOOL frameTrigger);
DC_API RetCode_t DC_CALLCONV GetSoftwareTriggerEnabled(_In_ uint16_t scanId, _Out_ BOOL& pixelTrigger, _Out_ BOOL& lineTrigger, _Out_ BOOL& frameTrigger);

DC_API RetCode_t DC_CALLCONV SetScanMode(_In_ uint16_t scanId, _In_ uint8_t scanMode, _In_ uint16_t valueX, _In_ uint16_t valueY, _In_ uint16_t subPixelCountX, _In_ uint16_t _In_ subPixelCountY, _In_ uint32_t subPixelTime);
DC_API RetCode_t DC_CALLCONV GetScanMode(_In_ uint16_t scanId, _Out_ uint8_t& scanMode, _Out_ uint16_t& valueX, _Out_ uint16_t& valueY, _Out_ uint16_t& subPixelCountX, _Out_ uint16_t& subPixelCountY, _Out_ uint32_t& subPixelTime);

// Functions for setting and getting image scan parameters

DC_API RetCode_t DC_CALLCONV SetImageGeometry(_In_ uint16_t scanId, _In_ uint16_t imageWidth, _In_ uint16_t imageHeight, _In_ uint16_t dacOffsetX, _In_ uint16_t dacOffsetY, _In_ uint16_t dacIncrementX, _In_ uint16_t dacIncrementY, _In_ uint16_t preScanStepCountX, _In_ uint16_t preScanStepCountY);
DC_API RetCode_t DC_CALLCONV GetImageGeometry(_In_ uint16_t scanId, _Out_ uint16_t& imageWidth, _Out_ uint16_t& imageHeight, _Out_ uint16_t& dacOffsetX, _Out_ uint16_t& dacOffsetY, _Out_ uint16_t& dacIncrementX, _Out_ uint16_t& dacIncrementY, _Out_ uint16_t& preScanStepCountX, _Out_ uint16_t& preScanStepCountY);
DC_API RetCode_t DC_CALLCONV SetLineAveragingCount(_In_ uint16_t scanId, _In_ uint16_t lineAveragingCount);
DC_API RetCode_t DC_CALLCONV GetLineAveragingCount(_In_ uint16_t scanId, _Out_ uint16_t& lineAveragingCount);
DC_API RetCode_t DC_CALLCONV SetLineStartDelay(_In_ uint16_t scanId, _In_ TimeScale_t lineStartDelay);
DC_API RetCode_t DC_CALLCONV GetLineStartDelay(_In_ uint16_t scanId, _Out_ TimeScale_t& lineStartDelay);
DC_API RetCode_t DC_CALLCONV SetBeamReturnTiming(_In_ uint16_t scanId, _In_ uint16_t stepCount, _In_ TimeScale_t lineStepTime, _In_ TimeScale_t frameStepTime);
DC_API RetCode_t DC_CALLCONV GetBeamReturnTiming(_In_ uint16_t scanId, _Out_ uint16_t& stepCount, _Out_ TimeScale_t& lineStepTime, _Out_ TimeScale_t& frameStepTime);
DC_API RetCode_t DC_CALLCONV SetSettlingTime(_In_ uint16_t scanId, _In_ TimeScale_t settlingTime);
DC_API RetCode_t DC_CALLCONV GetSettlingTime(_In_ uint16_t scanId, _Out_ TimeScale_t& settlingTime);
DC_API RetCode_t DC_CALLCONV SetHoldTime(_In_ uint16_t scanId, _In_ TimeScale_t holdTime);
DC_API RetCode_t DC_CALLCONV GetHoldTime (_In_ uint16_t scanId, _Out_ TimeScale_t& holdTime);

// Functions for setting and getting pixel map parameters

DC_API RetCode_t DC_CALLCONV SetPixelMapValues(_In_ uint16_t scanId, _In_ uint32_t pixelCount, _In_ const uint64_t* values);

// Event setting functions

DC_API RetCode_t DC_CALLCONV SetEventDataReady(_In_ uint16_t scanId, _In_ HANDLE eventDataReady);
DC_API RetCode_t DC_CALLCONV SetEventScanJobStarted(_In_ uint16_t scanId, _In_ HANDLE eventScanJobStarted);
DC_API RetCode_t DC_CALLCONV SetEventScanJobFinished(_In_ uint16_t scanId, _In_ HANDLE eventScanJobFinished);
DC_API RetCode_t DC_CALLCONV SetEventScanJobAborted(_In_ uint16_t scanId, _In_ HANDLE eventScanJobAborted);
DC_API RetCode_t DC_CALLCONV SetEventConnectionLost(_In_ HANDLE eventConnectionLost);

// General scan job control functions

DC_API RetCode_t DC_CALLCONV StartScanJob(_In_ uint16_t scanId, uint8_t abortCodeForActiveScan);
DC_API RetCode_t DC_CALLCONV StopScanJob(_In_ uint16_t scanId, _In_ uint8_t abortCode); 
DC_API RetCode_t DC_CALLCONV RemoveScanJob(_In_ uint16_t scanId);
DC_API RetCode_t DC_CALLCONV PrepareScanJob(_In_ uint16_t scanId);
DC_API RetCode_t DC_CALLCONV GetScanState(_In_ uint16_t scanId, _Out_ uint8_t& state);  
DC_API RetCode_t DC_CALLCONV GetChannelCount(_In_ uint16_t scanId, _Out_ uint32_t& channelCount);
DC_API RetCode_t DC_CALLCONV GetChannelInfo(_In_ uint16_t scanId, _In_ uint32_t channelIdx, _Out_ ChannelInfo_t& channelInfo);
DC_API RetCode_t DC_CALLCONV ReadChannelData(_In_ uint16_t scanId, _In_ void** pDestination, _Inout_ uint32_t* pPixelCount, _In_ uint32_t flags, _Out_ uint32_t* pFrameIndex, _Out_ uint32_t* pPixelOffset, _Out_ uint32_t* pChannelStatus);
DC_API RetCode_t DC_CALLCONV GetScanResult(_In_ uint16_t scanId, _Out_ uint8_t& result);

// Common scan job functions, apply for all types of scan jobs

DC_API RetCode_t DC_CALLCONV RemoveCompletedScans(void);
DC_API RetCode_t DC_CALLCONV RemoveAllScans(void);
DC_API RetCode_t DC_CALLCONV AbortAllScans(uint8_t abortCondition);
DC_API RetCode_t DC_CALLCONV GetScanProgress(_Out_ uint32_t& progress);

// Pause and resume functions

DC_API RetCode_t DC_CALLCONV Pause(void);
DC_API RetCode_t DC_CALLCONV Resume(void);
DC_API RetCode_t DC_CALLCONV IsPaused(_Out_ BOOL& isPaused);

// Software trigger

DC_API RetCode_t DC_CALLCONV TriggerNextPixel(_In_ uint16_t scanId);
DC_API RetCode_t DC_CALLCONV TriggerNextLine(_In_ uint16_t scanId);
DC_API RetCode_t DC_CALLCONV TriggerNextFrame(_In_ uint16_t scanId);

// Support functions

DC_API uint16_t DC_CALLCONV GetAPIVersion(void);
DC_API RetCode_t DC_CALLCONV GetFirmwareRevision(_Out_ uint16_t& pVersion);
DC_API RetCode_t DC_CALLCONV GetHardwareRevision(_Out_ uint8_t& pVersion);
DC_API RetCode_t DC_CALLCONV GetHardwareOptions(_Out_ uint32_t& pOptionFlags);
DC_API RetCode_t DC_CALLCONV GetDeviceSerialNumber(_Out_ char* pSerialNumber, _Inout_ uint32_t& nLength);
DC_API uint32_t DC_CALLCONV GetLastErrorMessage(_Out_ wchar_t* pMessage, _In_ uint32_t nLength);

// TCP/IP configuration functions

DC_API RetCode_t DC_CALLCONV SetTCPEnabled(_In_ BOOL bSet);
DC_API RetCode_t DC_CALLCONV GetTCPEnabled(_Out_ BOOL& isSet);
DC_API RetCode_t DC_CALLCONV SetDHCPEnabled(_In_ BOOL bSet);
DC_API RetCode_t DC_CALLCONV GetDHCPEnabled(_Out_ BOOL& isSet);
DC_API RetCode_t DC_CALLCONV SetIPAddress(_In_ const char* pIPv4Address);
DC_API RetCode_t DC_CALLCONV GetIPAddress(_Out_ char* pIPv4Address, _Inout_ uint32_t& nLength);
DC_API RetCode_t DC_CALLCONV SetIPNetMask(_In_ const char* pIPv4NetMask);
DC_API RetCode_t DC_CALLCONV GetIPNetMask(_Out_ char* pIPv4NetMask, _Inout_ uint32_t& nLength);
DC_API RetCode_t DC_CALLCONV SetTCPPortNumbers(_In_ uint16_t portSUP, _In_ uint16_t portDSCP, _In_ uint16_t portDIIP, _In_ uint16_t portDIDP, _In_ uint16_t portDVDP);
DC_API RetCode_t DC_CALLCONV GetTCPPortNumbers(_Out_ uint16_t& portSUP, _Out_ uint16_t& portDSCP, _Out_ uint16_t& portDIIP, _Out_ uint16_t& portDIDP, _Out_ uint16_t& portDVDP);

// Idle scan setup functions

DC_API RetCode_t DC_CALLCONV SetIdleScanTimeout(_In_ uint16_t timeOut);
DC_API RetCode_t DC_CALLCONV GetIdleScanTimeout(_Out_ uint16_t& timeOut);
DC_API RetCode_t DC_CALLCONV SetIdleScanChannel(_In_ ChannelInfo_t channel);
DC_API RetCode_t DC_CALLCONV GetIdleScanChannel(_Out_ ChannelInfo_t& channel);

// Miscellaneous setup functions

DC_API RetCode_t DC_CALLCONV SetScanOffPosition(_In_ uint16_t DACPosX, _In_ uint16_t DACPosY);
DC_API RetCode_t DC_CALLCONV GetScanOffPosition(_Out_ uint16_t& DACPosX, _Out_ uint16_t& DACPosY);
DC_API RetCode_t DC_CALLCONV SetADCDelay(_In_ uint16_t ADCDelay);
DC_API RetCode_t DC_CALLCONV GetADCDelay(_Out_ uint16_t& ADCDelay);
DC_API RetCode_t DC_CALLCONV SetExtScanDelay(_In_ TimeScale_t delay);
DC_API RetCode_t DC_CALLCONV GetExtScanDelay(_Out_ TimeScale_t& delay);
DC_API RetCode_t DC_CALLCONV SetExtBlankDelay(_In_ TimeScale_t delay);
DC_API RetCode_t DC_CALLCONV GetExtBlankDelay(_Out_ TimeScale_t& delay);
DC_API RetCode_t DC_CALLCONV SetTimeConstantAutomatic(_In_ BOOL bSet); 
DC_API RetCode_t DC_CALLCONV GetTimeConstantAutomatic(_Out_ BOOL& bSet); 

// Adjustment functions

DC_API RetCode_t DC_CALLCONV SaveDeviceSettings(void);
DC_API RetCode_t DC_CALLCONV LoadDeviceSettings(void);
DC_API RetCode_t DC_CALLCONV GetScanPolarity(_Out_ BOOL& inverted_x, _Out_ BOOL& inverted_y);
DC_API RetCode_t DC_CALLCONV SetScanPolarity(_In_ BOOL inverted_x, _In_ BOOL inverted_y);
DC_API RetCode_t DC_CALLCONV GetScanGainXRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
DC_API RetCode_t DC_CALLCONV SetScanGainX(_In_ float value);
DC_API RetCode_t DC_CALLCONV GetScanGainYRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
DC_API RetCode_t DC_CALLCONV SetScanGainY(_In_ float value);
DC_API RetCode_t DC_CALLCONV GetScanOffsetXRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
DC_API RetCode_t DC_CALLCONV SetScanOffsetX(_In_ float value);
DC_API RetCode_t DC_CALLCONV GetScanOffsetYRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
DC_API RetCode_t DC_CALLCONV SetScanOffsetY(_In_ float value);
DC_API RetCode_t DC_CALLCONV GetFastAGainRange(_In_ uint8_t index, _Out_ float& value, _Out_ float& min, _Out_ float& max);
DC_API RetCode_t DC_CALLCONV SetFastAGain(_In_ uint8_t index, _In_ float value);
DC_API RetCode_t DC_CALLCONV GetFastBGainRange(_In_ uint8_t index, _Out_ float& value, _Out_ float& min, _Out_ float& max);
DC_API RetCode_t DC_CALLCONV SetFastBGain(_In_ uint8_t index, _In_ float value);
DC_API RetCode_t DC_CALLCONV GetFastAInputOffsetRange(_In_ uint8_t index, _Out_ float& value, _Out_ float& min, _Out_ float& max);
DC_API RetCode_t DC_CALLCONV SetFastAInputOffset(_In_ uint8_t index, _In_ float value);
DC_API RetCode_t DC_CALLCONV GetFastBInputOffsetRange(_In_ uint8_t index, _Out_ float& value, _Out_ float& min, _Out_ float& max);
DC_API RetCode_t DC_CALLCONV SetFastBInputOffset(_In_ uint8_t index, _In_ float value);
DC_API RetCode_t DC_CALLCONV GetVideoGainRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
DC_API RetCode_t DC_CALLCONV SetVideoGain(_In_ float value);
DC_API RetCode_t DC_CALLCONV GetVideoInputOffsetRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
DC_API RetCode_t DC_CALLCONV SetVideoInputOffset(_In_ float value);
DC_API RetCode_t DC_CALLCONV GetScanMagXRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
DC_API RetCode_t DC_CALLCONV SetScanMagX(_In_ float value);
DC_API RetCode_t DC_CALLCONV GetScanMagYRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
DC_API RetCode_t DC_CALLCONV SetScanMagY(_In_ float value);
DC_API RetCode_t DC_CALLCONV GetScanShiftXRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
DC_API RetCode_t DC_CALLCONV SetScanShiftX(_In_ float value);
DC_API RetCode_t DC_CALLCONV GetScanShiftYRange(_Out_ float& value, _Out_ float& min, _Out_ float& max);
DC_API RetCode_t DC_CALLCONV SetScanShiftY(_In_ float value);

// Scan Transformation functions

DC_API RetCode_t DC_CALLCONV GetScanTransformationEnabled(_Out_ BOOL& enabled);
DC_API RetCode_t DC_CALLCONV SetScanTransformationEnabled(_In_ BOOL enabled);
DC_API RetCode_t DC_CALLCONV GetScanTransformationFlags(_Out_ uint8_t& value);
DC_API RetCode_t DC_CALLCONV SetScanTransformationFlags(_In_ uint8_t value);
DC_API RetCode_t DC_CALLCONV GetScanRotationEnabled(_Out_ BOOL& enabled);
DC_API RetCode_t DC_CALLCONV SetScanRotationEnabled(_In_ BOOL enabled);
DC_API RetCode_t DC_CALLCONV GetScanRotationAngle(_Out_ float& value);
DC_API RetCode_t DC_CALLCONV SetScanRotationAngle(_In_ float value);
DC_API RetCode_t DC_CALLCONV GetScanRotationAngleOffset(_Out_ float& value);
DC_API RetCode_t DC_CALLCONV SetScanRotationAngleOffset(_In_ float value);
DC_API RetCode_t DC_CALLCONV GetScanSkewAngle(_Out_ float& value);
DC_API RetCode_t DC_CALLCONV SetScanSkewAngle(_In_ float value);
DC_API RetCode_t DC_CALLCONV GetScanTiltEnabled(_Out_ BOOL& enabled);
DC_API RetCode_t DC_CALLCONV SetScanTiltEnabled(_In_ BOOL enabled);
DC_API RetCode_t DC_CALLCONV GetScanTiltAngle(_Out_ float& value);
DC_API RetCode_t DC_CALLCONV SetScanTiltAngle(_In_ float value);

// Video functions 
// Video refers to the signal of a chamber camera that is sent to the client, i.e. this library
// Video can also be shown on the HDMI monitor

DC_API RetCode_t DC_CALLCONV SetVideoEnabled(_In_ BOOL bSet);
DC_API RetCode_t DC_CALLCONV GetVideoEnabled(_Out_ BOOL& bSet);
DC_API RetCode_t DC_CALLCONV SetVideoResolution(_In_ uint16_t columnCount, _In_ uint16_t rowCount);
DC_API RetCode_t DC_CALLCONV GetVideoResolution(_Out_ uint16_t& columnCount, _Out_ uint16_t& rowCount);
DC_API RetCode_t DC_CALLCONV SetVideoDeinterlacingMode(_In_ uint8_t mode);
DC_API RetCode_t DC_CALLCONV GetVideoDeinterlacingMode(_Out_ uint8_t& mode);
DC_API RetCode_t DC_CALLCONV SetVideoAcqTime(_In_ uint32_t timeNs);
DC_API RetCode_t DC_CALLCONV GetVideoAcqTime(_Out_ uint32_t& time);
DC_API RetCode_t DC_CALLCONV SetEventVideoFrameReady(_In_ HANDLE eventVideoFrameReady);
DC_API RetCode_t DC_CALLCONV GetVideoFrame(_Out_ uint8_t* pBufferStart);
DC_API RetCode_t DC_CALLCONV SetVideoHOffset(_In_ uint16_t value);
DC_API RetCode_t DC_CALLCONV GetVideoHOffset(_Out_ uint16_t& value);
DC_API RetCode_t DC_CALLCONV SetVideoVOffset(_In_ uint16_t value);
DC_API RetCode_t DC_CALLCONV GetVideoVOffset(_Out_ uint16_t& value);

// DVI functions

DC_API RetCode_t DC_CALLCONV SetDVIEnabled(_In_ BOOL bSet);
DC_API RetCode_t DC_CALLCONV GetDVIEnabled(_Out_ BOOL& isSet);
DC_API RetCode_t DC_CALLCONV SetDVISource(_In_ uint8_t channel_source, _In_ uint8_t channel_index);
DC_API RetCode_t DC_CALLCONV GetDVISource(_Out_ uint8_t& channel_source, _Out_ uint8_t& channel_index);
DC_API RetCode_t DC_CALLCONV SetDVIMode(_In_ uint8_t /* HDMIMode_t */ mode);
DC_API RetCode_t DC_CALLCONV GetDVIMode(_Out_ uint8_t& /* HDMIMode_t */ mode);
DC_API RetCode_t DC_CALLCONV SetDVIModeLine(_In_ uint8_t index, _In_ const char* pText);
DC_API RetCode_t DC_CALLCONV SetDVIResolution(_In_ uint8_t resolution);
DC_API RetCode_t DC_CALLCONV GetDVIResolution(_Out_ uint8_t& resolution);
DC_API RetCode_t DC_CALLCONV SetDVIText(_In_ uint8_t index, _In_ const char* pText);
DC_API RetCode_t DC_CALLCONV GetDVIText(_In_ uint8_t index, _Out_ char* pText, _Inout_ uint32_t& nLength);
DC_API RetCode_t DC_CALLCONV SetDVIDacStepSize(_In_ float size);
DC_API RetCode_t DC_CALLCONV GetDVIDacStepSize(_Out_ float& size);

// Clock signal functions

DC_API RetCode_t DC_CALLCONV SetClockInvertMask(_In_ uint8_t mask);
DC_API RetCode_t DC_CALLCONV GetClockInvertMask(_Out_ uint8_t& mask);
DC_API RetCode_t DC_CALLCONV SetPixelClockLength(_In_ TimeScale_t clockLength);
DC_API RetCode_t DC_CALLCONV GetPixelClockLength(_Out_ TimeScale_t& clockLength);
DC_API RetCode_t DC_CALLCONV SetLineClockLength(_In_ TimeScale_t clockLength);
DC_API RetCode_t DC_CALLCONV GetLineClockLength(_Out_ TimeScale_t& clockLength);
DC_API RetCode_t DC_CALLCONV SetFrameClockLength(_In_ TimeScale_t clockLength);
DC_API RetCode_t DC_CALLCONV GetFrameClockLength(_Out_ TimeScale_t& clockLength);

DC_API RetCode_t DC_CALLCONV SetSyncEdgeInvertMask(_In_ uint16_t mask);
DC_API RetCode_t DC_CALLCONV GetSyncEdgeInvertMask(_Out_ uint16_t& mask);

DC_API RetCode_t DC_CALLCONV SetSyncDebounce(_In_ uint8_t value);
DC_API RetCode_t DC_CALLCONV GetSyncDebounce(_Out_ uint8_t& value);

// Hardware Pause/Resume Enabled

DC_API RetCode_t DC_CALLCONV SetHardwarePauseResumeEnabled(_In_ BOOL enabled);
DC_API RetCode_t DC_CALLCONV GetHardwarePauseResumeEnabled(_Out_ BOOL& enabled);

// Lock-In Amplifier Functions

DC_API RetCode_t DC_CALLCONV SetLIAEnabled(_In_ BOOL bSet);
DC_API RetCode_t DC_CALLCONV GetLIAEnabled(_Out_ BOOL& pIsSet);
DC_API RetCode_t DC_CALLCONV SetLIADigitalGain(_In_ uint8_t gain);
DC_API RetCode_t DC_CALLCONV GetLIADigitalGain(_Out_ uint8_t& gain);
DC_API RetCode_t DC_CALLCONV SetLIAClockMode(_In_ uint8_t clockMode);
DC_API RetCode_t DC_CALLCONV GetLIAClockMode(_Out_ uint8_t& clockMode);
DC_API RetCode_t DC_CALLCONV SetLIAReferenceFrequency(_In_ uint32_t frequency);
DC_API RetCode_t DC_CALLCONV GetLIAReferenceFrequency(_Out_ uint32_t& frequency);

#ifdef __cplusplus 
}
#endif

#endif