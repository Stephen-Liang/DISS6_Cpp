#pragma once
#ifndef COMMONTOOLS_H
#define COMMONTOOLS_H

#include <string>
#include <sstream>
#include <mutex>
#include <map>
#include <queue>
#include "DISS6ControlTypes.h"
#include <windows.h>
#include <atomic>
#include <ctime>
#include <fstream>
#include <string>
#include <zmq.hpp>
#include <string_view>
#include <unordered_map>
#include "Logger.h"
#include "ScanParamState.h"
#include "HAL.h"

//定义三个ip 一个收取cmd 一个获取参数，后续集成到UI中可取消掉，一个用作图像数据通信
class IPConfig
{
public:
    static std::string ipCmd;
    //static std::string ipParams;
    static std::string ipImage;
};

//获取扫描参数和图像数据入队出队
class Tools {
public:
    //static void DeserializeParameters(const std::string& data);//反序列化具体扫描参数
    static void EnqueueData(const byte* buf, int scanMode, int pixelOffset, int pixelCount);//图像数据入队
    static void ClearDataQueue();//清空图像数据

    //static std::map<std::string, std::string> parameterMap;
    static std::queue<std::vector<byte>> dataQueue;
};

////Normal Scan 参数 可扩展
//class NormalParams
//{
//public:
//    ushort imageWidth = 1024;
//    ushort imageHeight = 1024;
//    int acqTime = 100;
//
//    int lineStartDelay = 20;
//    int flyBackCount = 512;
//    int lineFlyBackTime = 10;
//    int frameFlyBackTime = 10;
//
//    int frameCount = 0;
//    double fastAOffsetBright = 0.0;
//    double fastAGainContrast = 0.0;
//
//    bool isAverageEnable = false;
//    ushort lineAvgCount = 0;
//    ushort frameAvgCount = 0;
//    double rotationAngle = 0.0;
//
//    const ushort dacOffset = 6560;
//    int avgMode = 0;
//    int avgNum = 0;
//    Utils::ScanningMode scanmode = Utils::ScanningMode::Normal;
//};
////Reduce Scan 参数 可扩展
//class ReduceParams
//{
//public:
//    ushort reduceStartX = 361;
//    ushort reduceStartY = 361;
//    ushort reduceWidth = 300;
//    ushort reduceHeight = 300;
//    ushort reduceFullWidth = 1024;
//    ushort reduceFullHeight = 1024;
//};
////通道选项
//class ChannelOptions
//{
//public:
//    bool fastA1 = false;
//    bool fastA2 = false;
//    bool fastA3 = false;
//    bool fastA4 = false;
//
//    bool fastB1 = false;
//    bool fastB2 = false;
//    bool fastB3 = false;
//    bool fastB4 = false;
//
//    bool slow1 = false;
//    bool slow2 = false;
//    bool slow3 = false;
//    bool slow4 = false;
//    bool slow5 = false;
//    bool slow6 = false;
//    bool slow7 = false;
//    bool slow8 = false;
//    bool slow9 = false;
//    bool slow10 = false;
//    bool slow11 = false;
//    bool slow12 = false;
//    bool slow13 = false;
//    bool slow14 = false;
//    bool slow15 = false;
//    bool slow16 = false;
//};

//Command 定义类型
struct ParamValue {
    int DataType;
    std::string ValueString;
    bool ValueBool;
    int ValueInt;
    double ValueDouble;
    std::string date;
    double ts;
};

//Command class 配合 UI使用
class Command {
public:
    std::string DeviceID = "Scan";
    std::string CommandName = "";
    static std::unordered_map<std::string, ParamValue> parameters;
    static std::vector<std::string> argNames;

    Command() {};

    Command(std::string deviceId, std::string cmdName)
    {
        this->DeviceID = deviceId;
        this->CommandName = cmdName;
    }

    static std::string ReadStringWithLength(std::istringstream& stream)
    {
        // Read the length of the string
        uint8_t length;
        stream.read(reinterpret_cast<char*>(&length), sizeof(length));

        // Read the string with the specified length
        std::string result(length, '\0');
        stream.read(&result[0], length);
        return result;
    }

    static Command Deserialize(const std::string& data)
    {
        Command cmd;
        std::istringstream stream(data);

        // Read DeviceID
        cmd.DeviceID = ReadStringWithLength(stream);

        // Read CommandName
        cmd.CommandName = ReadStringWithLength(stream);

        // Read parameter count
        int parameterCount;
        stream.read(reinterpret_cast<char*>(&parameterCount), sizeof(parameterCount));

        for (int i = 0; i < parameterCount; ++i)
        {
            // Read key
            std::string key = ReadStringWithLength(stream);

            // Read DataType
            byte dataType;
            stream.read(reinterpret_cast<char*>(&dataType), sizeof(dataType));

            ParamValue param;
            param.DataType = dataType;

            // Read value based on DataType
            switch (dataType)
            {
                case 0: {
                    stream.read(reinterpret_cast<char*>(&param.ValueString), sizeof(param.ValueString));
                    HAL::GetInstance().Update(key, param.ValueString);
                    break;
                }
                case 1: {
                    stream.read(reinterpret_cast<char*>(&param.ValueBool), sizeof(param.ValueBool));
                    HAL::GetInstance().Update(key, param.ValueBool);
                    break;
                }
                case 2: {
                    stream.read(reinterpret_cast<char*>(&param.ValueInt), sizeof(param.ValueInt));
                    HAL::GetInstance().Update(key, param.ValueInt);
                    break;
                }
                case 3: {
                    stream.read(reinterpret_cast<char*>(&param.ValueDouble), sizeof(param.ValueDouble));
                    HAL::GetInstance().Update(key, param.ValueDouble);
                    break;
                }
                case 4: {
                    param.date = ReadStringWithLength(stream);
                    break;
                }
                case 5: {
                    stream.read(reinterpret_cast<char*>(&param.ts), sizeof(param.ts));
                    break;
                }
                default:
                    throw std::runtime_error("Unknown DataType: " + std::to_string(dataType));
            }

            Command::parameters[key] = param;
        }

        return cmd;
    }

    static void UpdateArgNames()
    {
        argNames.reserve(parameters.size());

        for (const auto& pair : parameters) {
            argNames.push_back(pair.first);
        }
    }

    //GetCommandArgs
    void ClearCommandArgs()
    {
        parameters.clear();
    }

    std::string GetCommandArg(const std::string& argName, const std::string& defaultValue)
    {
        auto it = parameters.find(argName);

        if (it != parameters.end())
        {
            return parameters[argName].ValueString;; // Assuming a GetValueString() method exists
        }
        else
        {
            return defaultValue;
        }
    }

    bool GetCommandArg(const std::string& argName, bool defaultValue)
    {
        auto it = parameters.find(argName);

        if (it != parameters.end())
        {
            //CmdParamValue pv = parameters[argName];
            return parameters[argName].ValueBool;
        }
        else
            return defaultValue;
    }

    int GetCommandArg(const std::string& argName, int defaultValue)
    {
        auto it = parameters.find(argName);

        if (it != parameters.end())
        {
            return parameters[argName].ValueInt;
        }
        else
            return defaultValue;
    }
    
    double GetCommandArg(const std::string& argName, double defaultValue)
    {
        auto it = parameters.find(argName);

        if (it != parameters.end())
        {
            return parameters[argName].ValueDouble;
        }
        else
            return defaultValue;
    }

    //SetCommandArgs
    void SetCommandArg(const std::string& argName, ParamValue pv)
    {
        parameters[argName] = pv;
    }

    void SetCommandArg(const std::string& argName, std::string value)
    {
        ParamValue pv;
        pv.DataType = 0;
        pv.ValueString = value;

        parameters[argName] = pv;
    }
    
    void SetCommandArg(const std::string& argName, bool value)
    {
        ParamValue pv;
        pv.DataType = 1;
        pv.ValueBool = value;

        parameters[argName] = pv;
    }
    
    void SetCommandArg(const std::string& argName, int value)
    {
        ParamValue pv;
        pv.DataType = 2;
        pv.ValueInt = value;

        parameters[argName] = pv;
    }
    
    void SetCommandArg(const std::string& argName, double value)
    {
        ParamValue pv;
        pv.DataType = 3;
        pv.ValueDouble = value;

        parameters[argName] = pv;
    }
};

// 线程安全的消息队列
class CommandQueue {
private:
    std::queue<Command> queue;
    std::mutex mutex;
    std::condition_variable cond;

public:
    // 入队操作
    void push(Command& cmd) {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(cmd);
        cond.notify_one(); // 通知有新元素加入队列
    }

    // 出队操作
    Command pop() {
        std::unique_lock<std::mutex> lock(mutex);
        cond.wait(lock, [this] { return !queue.empty(); }); // 等待直到队列不为空
        Command cmd = queue.front();
        queue.pop();
        return cmd;
    }

    // 判断队列是否为空
    bool empty() {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.empty();
    }
};

#endif // !COMMONTOOLS_H
