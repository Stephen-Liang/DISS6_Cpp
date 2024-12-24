#pragma once
#ifndef DISS6CONTROLERRORS_H
#define DISS6CONTROLERRORS_H

#include <exception>
#include "libCommon.h"
#include "string"

enum RetCode
{
    NO_ERROR_CODE = 0x0000000,
    NOT_INITIALIZED = 0x0000001,
    ALREADY_INITIALIZED = 0x0000002,
    CANNOT_LOCATE_DEVICE = 0x0000003,
    OPENING_USBCONNECTION_FAILED = 0x0000004,
    CREATING_SUP_CONNECTION_FAILED = 0x0000005,
    SUP_READ_FAILED = 0x0000006,
    SUP_WRITE_FAILED = 0x0000007,
    UDP_SEND_FAILED = 0x0000008,
    UDP_RECEIVE_FAILED = 0x0000009,
    UDP_LOCATOR_SECRET_MISMATCH = 0x000000A,
    INVALID_IP_ADDRESS = 0x000000B,
    TCP_SEND_FAILED = 0x000000C,
    TCP_NOT_CONNECTED = 0x000000D,
    TCP_RECEIVE_FAILED = 0x000000E,
    TCP_PORT_ALREADY_IN_USE = 0x000000F,
    INVALID_MICS_CARD_INDEX = 0x0000010,
    INVALID_MICS_CARD_ADDRESS = 0x0000011,
    INVALID_DISS_CARD_INDEX = 0x0000012,
    INVALID_MICS_CHANNEL_INDEX = 0x0000013,
    OPERATION_FAILED = 0x0000014,
    TIMEOUT_OCCURRED = 0x0000015,
    SEND_TIMEOUT_OCCURRED = 0x0000016,
    NULLPTR_PARAM_GIVEN = 0x0000017,
    INVALID_PARAM_GIVEN = 0x0000018,
    INVALID_PARAMETER_INDEX = 0x0000019,
    CREATING_DEVICE_FAILED = 0x000001A,
    CREATING_DIIP_CONNECTION_FAILED = 0x000001B,
    CREATING_DSCPMAILER_FAILED = 0x000001C,
    CREATING_DSCP_SENDER_FAILED = 0x000001D,
    CREATING_SCANDATARECEIVER_FAILED = 0x000001E,
    CREATING_SCANDATADRIVER_FAILED = 0x000001F,
    STARTING_SCAN_THREAD_FAILED = 0x0000020,
    SCAN_THREAD_STALLED = 0x0000021,
    EVENT_THREAD_STALLED = 0x0000022,
    CREATING_SCANJOBHANDLER_FAILED = 0x0000023,
    STARTING_EVENT_THREAD_FAILED = 0x0000024,
    CREATING_SCANJOBEVENTDRIVER_FAILED = 0x0000025,
    INVALID_EVENT_HANDLE = 0x0000026,
    CREATING_VIDEODATARECEIVER_FAILED = 0x0000027,
    STARTING_VIDEO_DRIVER_FAILED = 0x0000028,
    VIDEO_THREAD_STALLED = 0x0000029,
    VIDEO_CHANNEL_IN_USE = 0x000002A,
    NO_D6HW_CHANNEL = 0x000002B,
    NO_CHANNEL_SELECTED = 0x000002C,
    TOO_MANY_CHANNELS_SELECTED = 0x000002D,
    UNSUPPORTED_CHANNEL_COMBINATION = 0x000002E,
    INCONSISTENT_COUNTER_CASCADING = 0x000002F,
    INCONSISTENT_GEOMETRY = 0x0000030,
    INVALID_CHANNEL_ID = 0x0000031,
    INVALID_SCAN_ID = 0x0000032,
    INVALID_SCAN_JOB_TYPE = 0x0000033,
    INVALID_RESOLUTION = 0x0000034,
    SCAN_JOB_GONE_AWAY = 0x0000035,
    SCAN_JOB_INEXISTENT = 0x0000036,
    SCAN_JOB_ABANDONED = 0x0000037,
    SCAN_JOB_REJECTED = 0x0000038,
    SCAN_JOB_TRANSMISSION_FAILED = 0x0000039,
    SCAN_JOB_NOT_IN_PREPARED_STATE = 0x000003A
};

class LibException: public std::exception
{
public:
    LibException(const std::string& message = "", RetCode_t code = DC_RET_NO_ERROR)
        : msg(message), hResult(static_cast<int>(code)) {}

    virtual const char* what() const noexcept override {
        return msg.c_str();
    }

    int HResult() const { return hResult;}

private:
    std::string msg;
    int hResult;
};

class NotInitializedException : public LibException {
public:
    NotInitializedException(const std::string& message = "")
        : LibException(message, DC_RET_NOT_INITIALIZED) {}
};

class AlreadyInitializedException : public LibException {
public:
    AlreadyInitializedException(const std::string& message = "")
        : LibException(message, DC_RET_ALREADY_INITIALIZED) {}
};

class DeviceNotFoundException : public LibException {
public:
    DeviceNotFoundException(const std::string& message = "")
        : LibException(message, DC_RET_CANNOT_LOCATE_DEVICE) {}
};

#endif // DISS6CONTROLERRORS_H