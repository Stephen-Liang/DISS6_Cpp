#include "ScanJob.h"

ScanJob::ScanJob(const std::vector<ChannelInfo_t>& Channels)
{
    _Channels = Channels;//std::move
    CreateHandle(_Channels);
    RegisterEvents();
}

ScanJob::~ScanJob()
{
    CloseHandle(JobStartedEvent);
    CloseHandle(DataReadyEvent);
    CloseHandle(JobFinishedEvent);
    CloseHandle(JobAbortedEvent);

    StopControlThread();
}

void ScanJob::RegisterEvents()
{
    DISS6Control::Job::RegisterJobStartedEvent(_Handle, JobStartedEvent);
    DISS6Control::Job::RegisterDataReadyEvent(_Handle, DataReadyEvent);
    DISS6Control::Job::RegisterJobFinishedEvent(_Handle, JobFinishedEvent);
    DISS6Control::Job::RegisterJobAbortedEvent(_Handle, JobAbortedEvent);
}

void ScanJob::SetImageGeometry(ushort ImageWidth, ushort ImageHeight, ushort DacOffsetX, ushort DacOffsetY, ushort DacIncrementX, ushort DacIncrementY, ushort PreScanStepCountX, ushort PreScanStepCountY)
{
    _ImageWidth = ImageWidth;
    _ImageHeight = ImageHeight;
    DISS6Control::Job::SetImageGeometry(_Handle, ImageWidth, ImageHeight, DacOffsetX, DacOffsetY, DacIncrementX, DacIncrementY, PreScanStepCountX, PreScanStepCountY);
}

void ScanJob::SetBeamReturnTiming(ushort StepCount, TimeScale LineStepTime, TimeScale FrameStepTime)
{
    DISS6Control::Job::SetBeamReturnTiming(_Handle, StepCount, LineStepTime, FrameStepTime);
}

void ScanJob::CreateHandle(const std::vector<ChannelInfo_t>& Channels)
{
    _Handle = DISS6Control::Job::CreateImageScan(Channels);   //返回ScanID
    std::cout << "Init ok. CreateImageScan = " << _Handle << std::endl;
    Logger::GetInstance() << "Init ok. CreateImageScan = " << _Handle << std::endl;
}

void ScanJob::Start(AbortCode Code)
{
    DISS6Control::Job::Start(_Handle, Code);
    StartControlThread();
}

void ScanJob::StopControlThread()
{
    SetEvent(JobAbortedEvent);
    SetEvent(JobStartedEvent);
    SetEvent(JobFinishedEvent);
    SetEvent(DataReadyEvent);

    if (controlThread.joinable())
    {
        controlThread.join();
    }
}

void ScanJob::StartControlThread()
{
    //Creat a Thread.
    if (!controlThread.joinable())
    {
        controlThread = std::thread(&ScanJob::ControlTaskProc, this);//启动线程
    }
}

void ScanJob::ControlTaskProc()
{
    DataTaskReadyEvent = CreateEvent(nullptr, false, false, nullptr);
    HANDLE events[] =
    {
        TaskCancelEvent,//Cancel Tasks 永远不会触发 WAIT_OBJECT_0

        JobStartedEvent,//Start Tasks WAIT_OBJECT_0 + 1
        DataTaskReadyEvent,
        JobFinishedEvent,
        JobAbortedEvent
    };

    while (true)
    {
        DWORD waitResult = WaitForMultipleObjects(5, events, FALSE, INFINITE);

        switch (waitResult)
        {
        case WAIT_OBJECT_0://TaskCancel
            std::cout << "ControlTaskProc WAIT_OBJECT_0" << std::endl;
            break;
        case WAIT_OBJECT_0 + 1://Scan Job Starteds
            std::cout << "ControlTaskProc WAIT_OBJECT_0 + 1 " << std::endl;
            if (!dataTaskActive)
            {
                dataTaskThread = std::thread(&ScanJob::DataTaskProc, this);
                //dataTaskThread.join();
                dataTaskActive = true;
            }
            break;
        case WAIT_OBJECT_0 + 2:
            std::cout << "ControlTaskProc WAIT_OBJECT_0 + 2" << std::endl;
            //sTools::ClearDataQueue();//触发一次，从NormalScan-->ReduceScan 清空数据队列
            OnScanJobRunning();
            break;

        case WAIT_OBJECT_0 + 3:
            //Finished
            std::cout << "ControlTaskProc WAIT_OBJECT_0 + 3" << std::endl;
            Tools::ClearDataQueue();//结束后清空数据，重新new一个job时也会重新触发

            bool wasAborted;
            try
            {
                wasAborted = DISS6Control::Job::GetResult(_Handle) == AbortCode::Immediately ? true : false;
            }
            catch (...)
            {
                wasAborted = true;
            }

            if (wasAborted == false)
            {
                OnScanJobDataAvailable();
            }
            
            break;

        case WAIT_OBJECT_0 + 4:

            std::cout << "ControlTaskProc WAIT_OBJECT_0 + 4" << std::endl;
            OnScanJobFinished(true);
            break;

        default:
            break;
        }
    }
}

void ScanJob::DataTaskProc()
{
    const int DataTimeOutInterval = 100;

    HANDLE events[] = { TaskCancelEvent, Semaphore };

    //std::cout << "DataTaskProc trigger" << std::endl;

    //DWORD waitResult = WaitForMultipleObjects(2, events, FALSE, INFINITE);
    //std::cout << "DataTaskProc waitResult = " << waitResult << std::endl;//output = 1

    if (WaitForMultipleObjects(2, events, FALSE, INFINITE) == 0)
        return;

    // Semaphore 被触发
    SetEvent(DataTaskReadyEvent); // 通知控制线程数据准备就绪? 没使用到。
    //events[1] = GUISetEvent; //GUI准备就绪

    bool gotData = false;
    auto lastUpdateTime = std::chrono::system_clock::now();
    events[1] = DataReadyEvent;

    while (true)
    {
        gotData = false;

        try
        {
            DWORD waitResult = WaitForMultipleObjects(2, events, FALSE, DataTimeOutInterval);
            //std::cout << "while (true) DataTaskProc waitResult = " << waitResult << std::endl; //output = 258

            if (waitResult == 0) {
                return; 
            }

            gotData = true;
            OnScanJobDataAvailable(); // 处理数据，触发事件

            auto timeNow = std::chrono::system_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - lastUpdateTime).count();
            if (elapsed >= DataTimeOutInterval)
            {
                OnDataTimeout(); // 数据超时
                lastUpdateTime = timeNow;
            }
        }
        catch (...)
        {
            // 超时或其他错误
            if (gotData) {
                OnDataTimeout(); // 数据超时
                lastUpdateTime = std::chrono::system_clock::now();
            }
        }
    }
}

void ScanJob::OnScanJobDataAvailable()
{
    if (m_dataAvailableCallback) {
        m_dataAvailableCallback();
    }
}

void ScanJob::OnScanJobRunning()
{
    if (m_scanJobRunningCallback) {
        m_scanJobRunningCallback();
    }
}

void ScanJob::OnScanJobFinished(bool aborted)
{
    if (m_scanJobFinishedCallback){
        m_scanJobFinishedCallback();
    }
}

void ScanJob::OnDataTimeout()
{
    if (m_scanJobTimeOutCallback) {
        m_scanJobTimeOutCallback();
    }
}

void ScanJob::SetDataAvailableCallback(VoidFuncCallback callback) {
    m_dataAvailableCallback = callback;
}

void ScanJob::SetScanJobRunningCallback(VoidFuncCallback callback)
{
    m_scanJobRunningCallback = callback;
}

void ScanJob::SetScanJobFinishedCallback(VoidFuncCallback callback)
{
    m_scanJobFinishedCallback = callback;
}

void ScanJob::SetScanJobTimeOutCallback(VoidFuncCallback callback)
{
    m_scanJobTimeOutCallback = callback;
}

void ScanJob::Dispose(bool disposing)
{
    if (!disposedValue)
    {
        StopThreads();
    }

    if (disposing)
    {

    }

    if (_Handle != 0)
    {
        NativeMethods::RemoveScanJob(_Handle);
    }

    disposedValue = true;
}

void ScanJob::StopThreads()
{

}