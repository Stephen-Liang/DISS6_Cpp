#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include <ctime>
#include <direct.h>  // ����WindowsĿ¼����
#include <io.h>     // �����ļ�����
#include <minwinbase.h>
#include "DISS6NativeMethods.hpp"
#include <fileapi.h>
#include <handleapi.h>
#include <timezoneapi.h>

class Logger {
public:
    // ɾ����������͸�ֵ�������ȷ������
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // ��̬������ȡ����
    static Logger& GetInstance() {
        static Logger instance;
        return instance;
    }

    // ����std::cout���������ģ��
    template<typename T>
    Logger& operator<<(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        log_stream_ << value;
        return *this;
    }

    // ����endl����
    Logger& operator<<(std::ostream& (*manip)(std::ostream&)) {
        std::lock_guard<std::mutex> lock(mutex_);
        log_stream_ << manip;

        // �����endl����ִ��ʵ��д���ļ�
        if (manip == static_cast<std::ostream & (*)(std::ostream&)>(std::endl)) {
            flushLogToFile();
        }
        return *this;
    }

    // �쳣��¼����
    void logException(const std::exception& e, const std::string& context = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        log_stream_ << "[EXCEPTION] "
            << (context.empty() ? "" : context + ": ")
            << e.what() << std::endl;
        flushLogToFile();
    }

private:
    Logger() : log_stream_() {
        createLogDirectoryIfNotExists();
        cleanupOldLogs();
    }

    void createLogDirectoryIfNotExists() {
        // Windows�´���Ŀ¼
        _mkdir("logs");
    }

    void cleanupOldLogs() {
        std::string findPath = "logs\\*.*";
        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA(findPath.c_str(), &findData);

        if (hFind == INVALID_HANDLE_VALUE) {
            return;
        }

        time_t now = time(nullptr);
        do {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                std::string fullPath = "logs\\" + std::string(findData.cFileName);

                // ��ȡ�ļ��޸�ʱ��
                HANDLE hFile = CreateFileA(
                    fullPath.c_str(),
                    GENERIC_READ,
                    FILE_SHARE_READ,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL
                );

                if (hFile != INVALID_HANDLE_VALUE) {
                    FILETIME ftCreate, ftAccess, ftWrite;
                    if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
                        SYSTEMTIME stUTC;
                        FileTimeToSystemTime(&ftWrite, &stUTC);

                        struct tm fileTime = { 0 };
                        fileTime.tm_year = stUTC.wYear - 1900;
                        fileTime.tm_mon = stUTC.wMonth - 1;
                        fileTime.tm_mday = stUTC.wDay;
                        fileTime.tm_hour = stUTC.wHour;
                        fileTime.tm_min = stUTC.wMinute;
                        fileTime.tm_sec = stUTC.wSecond;

                        time_t fileTimet = mktime(&fileTime);

                        // ɾ������һ�ܵ���־
                        if (now - fileTimet > 7 * 24 * 60 * 60) {
                            DeleteFileA(fullPath.c_str());
                        }
                    }
                    CloseHandle(hFile);
                }
            }
        } while (FindNextFileA(hFind, &findData) != 0);

        FindClose(hFind);
    }

    void flushLogToFile() {
        // ���ɰ�������������־�ļ�
        //std::time_t now = std::time(nullptr);
        //std::tm* now_tm = std::localtime(&now);

        time_t now = time(nullptr);
        struct tm now_tm;
        localtime_s(&now_tm, &now);

        char filename[256];
        // ʹ�� & ��ȡ now_tm �ĵ�ַ����ȷ��ת��Ϊ const struct tm*
        std::strftime(filename, sizeof(filename), "logs/log_%Y-%m-%d.txt", &now_tm);

        std::ofstream log_file(filename, std::ios::app);
        if (log_file.is_open()) {
            // ���ʱ���
            char timestamp[64];
            // ͬ��ʹ�� & ��ȡ now_tm �ĵ�ַ
            std::strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S] ", &now_tm);
            log_file << timestamp << log_stream_.str();
            log_file.close();
        }

        // ����ַ�����
        log_stream_.str("");
        log_stream_.clear();
    }

    std::stringstream log_stream_;
    std::mutex mutex_;
};

#endif // LOGGER_H