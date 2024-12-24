#pragma once
#include <thread>
#include <vector>
#include "BlockingQueue.h"
#include <shared_mutex>
#include <unordered_map>
#include <optional>

// 自定义简单的 Optional 实现
template <typename T>
class Optional {
private:
    T value_;
    bool has_value_ = false;

public:
    Optional() = default;

    Optional(const T& value) : value_(value), has_value_(true) {}

    bool has_value() const { return has_value_; }

    T value() const {
        if (!has_value_)
            throw std::runtime_error("No value present");
        return value_;
    }

    T value_or(const T& default_value) const {
        return has_value_ ? value_ : default_value;
    }

    explicit operator bool() const { return has_value_; }

    T* operator->() {
        if (!has_value_)
            throw std::runtime_error("No value present");
        return &value_;
    }

    const T* operator->() const {
        if (!has_value_)
            throw std::runtime_error("No value present");
        return &value_;
    }
};

//所有扫描状态相关都放到这里
class HAL
{
public:
    // 获取单例实例
    static HAL& GetInstance() {
        static HAL ins;
        return ins;
    }

    // 删除拷贝构造和赋值运算符，防止复制
    HAL(const HAL&) = delete;
    HAL& operator=(const HAL&) = delete;

#pragma region Set/Get unordered_map Params
    // 整数参数
    void SetIntParam(const std::string& key, int value) {
        std::lock_guard<std::mutex> lock(int_mutex);
        intParams[key] = value;
    }

    int GetIntParam(const std::string& key) const {
        std::lock_guard<std::mutex> lock(int_mutex);
        auto it = intParams.find(key);
        return (it != intParams.end()) ? it->second : 0;
    }

    //Optional<int> GetIntParam(const std::string& key) const {
    //    std::lock_guard<std::mutex> lock(int_mutex);
    //    auto it = intParams.find(key);
    //    return (it != intParams.end()) ? Optional<int>(it->second) : Optional<int>();
    //}

    //bool TryGetIntParam(const std::string& key, int& value) const {
    //    std::lock_guard<std::mutex> lock(int_mutex);
    //    auto it = intParams.find(key);
    //    if (it != intParams.end()) {
    //        value = it->second;
    //        return true;
    //    }
    //    return false;
    //}

    // 双精度浮点数参数
    void SetDoubleParam(const std::string& key, double value) {
        std::lock_guard<std::mutex> lock(double_mutex);
        doubleParams[key] = value;
    }

    Optional<double> GetDoubleParam(const std::string& key) const {
        std::lock_guard<std::mutex> lock(double_mutex);
        auto it = doubleParams.find(key);
        return (it != doubleParams.end()) ? Optional<double>(it->second) : Optional<double>();
    }

    // 布尔参数
    void SetBoolParam(const std::string& key, bool value) {
        std::lock_guard<std::mutex> lock(bool_mutex);
        boolParams[key] = value;
    }

    Optional<bool> GetBoolParam(const std::string& key) const {
        std::lock_guard<std::mutex> lock(bool_mutex);
        auto it = boolParams.find(key);
        return (it != boolParams.end()) ? Optional<bool>(it->second) : Optional<bool>();
    }

    // 字符串参数
    void SetStringParam(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(string_mutex);
        stringParams[key] = value;
    }

    Optional<std::string> GetStringParam(const std::string& key) const {
        std::lock_guard<std::mutex> lock(string_mutex);
        auto it = stringParams.find(key);
        return (it != stringParams.end()) ? Optional<std::string>(it->second) : Optional<std::string>();
    }

    // 时间参数（使用 time_t）
    void SetTimeParam(const std::string& key, time_t value) {
        std::lock_guard<std::mutex> lock(time_mutex);
        timeParams[key] = value;
    }
#pragma endregion

#pragma region Update state data
    bool Update(std::string paramName, int val)
    {
        if (intParams.find(paramName) == intParams.end() || intParams[paramName] != val)
        {
            intParams[paramName] = val;
            OnIntParamChanged(paramName);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Update(std::string paramName, double val)
    {
        if (doubleParams.find(paramName) == doubleParams.end() || doubleParams[paramName] != val)
        {
            doubleParams[paramName] = val;
            OnDoubleParamChanged(paramName);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Update(std::string paramName, std::string val)
    {
        if (stringParams.find(paramName) == stringParams.end() || stringParams[paramName] != val)
        {
            stringParams[paramName] = val;
            OnStringParamChanged(paramName);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Update(std::string paramName, bool val)
    {
        if (boolParams.find(paramName) == boolParams.end() || boolParams[paramName] != val)
        {
            boolParams[paramName] = val;
            OnBoolParamChanged(paramName);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Update(std::string paramName, time_t val)
    {
        if (timeParams.find(paramName) == timeParams.end() || timeParams[paramName] != val)
        {
            timeParams[paramName] = val;
            OnTimeParamChanged(paramName);
            return true;
        }
        else
        {
            return false;
        }
    }
#pragma endregion

private:
    HAL() {
        // 使用 vector 管理线程
        std::vector<std::thread> threads;

        threads.emplace_back(&HAL::IntParamChangedProc, this);
        threads.emplace_back(&HAL::DoubleParamChangedProc, this);
        threads.emplace_back(&HAL::BoolParamChangedProc, this);
        threads.emplace_back(&HAL::StringParamChangedProc, this);
        threads.emplace_back(&HAL::TimeParamChangedProc, this);

        // 分离线程，让它们在后台运行
        for (auto& thread : threads) {
            thread.detach();
        }
    };

    void IntParamChangedProc() {
        // 实现方法
        while (true) {
            std::string paramName = intParamChanged->Dequeue();

        }
    }

    void DoubleParamChangedProc() {
        // 实现方法
        while (true) {

        }
    }

    void BoolParamChangedProc() {
        // 实现方法
        while (true) {

        }
    }

    void StringParamChangedProc() {
        // 实现方法
        while (true) {

        }
    }

    void TimeParamChangedProc() {
        // 实现方法
        while (true) {

        }
    }

#pragma region state store
    BlockingQueue<std::string>* intParamChanged = new BlockingQueue<std::string>();
    BlockingQueue<std::string>* doubleParamChanged = new BlockingQueue<std::string>();
    BlockingQueue<std::string>* boolParamChanged = new BlockingQueue<std::string>();
    BlockingQueue<std::string>* stringParamChanged = new BlockingQueue<std::string>();
    BlockingQueue<std::string>* timeParamChanged = new BlockingQueue<std::string>();

    // Thread-safe maps with shared mutex for better performance
     // 使用标准互斥锁（兼容性更好）
    mutable std::mutex int_mutex;
    std::unordered_map<std::string, int> intParams;

    mutable std::mutex double_mutex;
    std::unordered_map<std::string, double> doubleParams;

    mutable std::mutex bool_mutex;
    std::unordered_map<std::string, bool> boolParams;

    mutable std::mutex string_mutex;
    std::unordered_map<std::string, std::string> stringParams;

    // 使用 time_t 替代 chrono::time_point
    mutable std::mutex time_mutex;
    std::unordered_map<std::string, time_t> timeParams;
#pragma endregion

#pragma region Enqueue
    void OnIntParamChanged(std::string paramName)
    {
        intParamChanged->Enqueue(paramName);
    }

    void OnDoubleParamChanged(std::string paramName)
    {
        doubleParamChanged->Enqueue(paramName);
    }

    void OnBoolParamChanged(std::string paramName)
    {
        boolParamChanged->Enqueue(paramName);
    }

    void OnStringParamChanged(std::string paramName)
    {
        stringParamChanged->Enqueue(paramName);
    }

    void OnTimeParamChanged(std::string paramName)
    {
        timeParamChanged->Enqueue(paramName);
    }
#pragma endregion
};

