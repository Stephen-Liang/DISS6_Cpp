#include "ScanLib.h"

ScanLib::ScanLib(Utils::ConnectionType type)
{
    InitializeDISS6(type);
}

void ScanLib::InitializeDISS6(Utils::ConnectionType type)
{
    try
    {
        switch (type)
        {
            case Utils::ConnectionType::USB:
                DISS6Control::DISS6InitUSB();
                break;
            case Utils::ConnectionType::TCP:
                break;
            default:
                break;
        }

        DISS6Control::Job::AbortAll(0);
        InitDiss6Param();
		_isInit = true;
    }
    catch (const std::exception& ex)
    {
        Logger::GetInstance() << "Error: " << ex.what() << std::endl;
		_isInit = false;
    }

    InitChDetPair(); //初始化每个信道对应的探测器
}

void ScanLib::InitChDetPair()
{
    channelDetPairs["ET"] = "A1";
    channelDetPairs["InLens"] = "A2";
    channelDetPairs["BSD"] = "A3";
    channelDetPairs["STEM"] = "A4";

    detSelectedPairs["A1"] = true;
    detSelectedPairs["A2"] = false;
    detSelectedPairs["A3"] = false;
    detSelectedPairs["A4"] = false;
}

void ScanLib::InitDiss6Param()
{
    _scanRotate = DISS6Control::Scan::Rotation::GetAngle();//获取旋转角度

    //亮度初始化
    float brightnessMaxValue = 1.252f, brightnessMinValue = -1.25f, nowValue;
    DISS6Control::Adjustment::FastA::GetInputOffsetRange(0, nowValue, brightnessMinValue, brightnessMaxValue);
    float brightnessValueRange = brightnessMaxValue - brightnessMinValue;

    brightness_A1 = (int)((nowValue - brightnessMinValue) / brightnessValueRange * 1000);
    DISS6Control::Adjustment::FastA::GetInputOffsetRange(1, nowValue, brightnessMinValue, brightnessMaxValue);
    brightness_A2 = (int)((nowValue - brightnessMinValue) / brightnessValueRange * 1000);
    DISS6Control::Adjustment::FastA::GetInputOffsetRange(2, nowValue, brightnessMinValue, brightnessMaxValue); 
    brightness_A3 = (int)((nowValue - brightnessMinValue) / brightnessValueRange * 1000);
    DISS6Control::Adjustment::FastA::GetInputOffsetRange(3, nowValue, brightnessMinValue, brightnessMaxValue); 
    brightness_A4 = (int)((nowValue - brightnessMinValue) / brightnessValueRange * 1000);

    //对比度初始化
    float contrastMaxValue = 26, contrastMinValue = -22;
    DISS6Control::Adjustment::FastA::GetGainRange(0, nowValue, contrastMinValue, contrastMaxValue);
    float contrastValueRange = contrastMaxValue - contrastMinValue;

    contrast_A1 = (int)((nowValue - contrastMinValue) / contrastValueRange * 1000);
    DISS6Control::Adjustment::FastA::GetGainRange(1, nowValue, contrastMinValue, contrastMaxValue);
    contrast_A2 = (int)((nowValue - contrastMinValue) / contrastValueRange * 1000);
    DISS6Control::Adjustment::FastA::GetGainRange(2, nowValue, contrastMinValue, contrastMaxValue);
    contrast_A3 = (int)((nowValue - contrastMinValue) / contrastValueRange * 1000);
    DISS6Control::Adjustment::FastA::GetGainRange(3, nowValue, contrastMinValue, contrastMaxValue);
    contrast_A4 = (int)((nowValue - contrastMinValue) / contrastValueRange * 1000);
}

void ScanLib::SetDualChannelMode(int CH1Index, int CH2Index)
{
	SetScanMode(Utils::ScanningMode::DualChannel);

	_dualSig1 = CH1Index;
	_dualSig2 = CH2Index;
}

//将main中的参数传递到scanlib中
void ScanLib::SetReduceMode(ushort StartX, ushort StartY, ushort ReduceWidth, ushort ReduceHeight, ushort FullImageWidth, ushort FullImageHeight)
{
	//Tools::ClearDataQueue();

	_startX = StartX;
	_startY = StartY;

	_rscanWidth = ReduceWidth;
	_rscanHeight = ReduceHeight;

	_fullWidth = FullImageWidth;
	_fullHeight = FullImageHeight;

	StartDiss6();
}

void ScanLib::SetScanMode(Utils::ScanningMode mode)
{
    _scanMode = (int)mode;//设置扫描模式
    _isReducePosChanged = false;

    switch (mode)
    {
        case Utils::ScanningMode::Normal:
            break;
        case Utils::ScanningMode::SplitMode:
            SelectDetectorChannel(true, true);
            break;
        case Utils::ScanningMode::DualChannel:
            break;
        case Utils::ScanningMode::DualMag:
            break;
        case Utils::ScanningMode::QaudMode:
            SelectDetectorChannel(true, true, true, true);
            break;
        case Utils::ScanningMode::Reduce:
            break;
        case Utils::ScanningMode::Mix:
            break;
        default:
            break;
    }
}

void ScanLib::SelectDetectorChannel(bool IsEtOn, bool IsInLensOn, bool IsBsdOn, bool IsStemOn)
{
    detSelectedPairs[channelDetPairs["ET"]] = IsEtOn;
    detSelectedPairs[channelDetPairs["InLens"]] = IsInLensOn;
    detSelectedPairs[channelDetPairs["BSD"]] = IsBsdOn;
    detSelectedPairs[channelDetPairs["STEM"]] = IsStemOn;
}

//从device 设置参数到 scanlib
void ScanLib::RefreshImagingSettings(int width, int height, int dwell, int avgMode, int avgNum)
{
    _imgwidth = width;
    _imgheight = height;
    _dwelltime = dwell;
    _avgMode = avgMode;
    _avgNum = avgNum;

    linesPerPacket = (N - 4) / _imgwidth;
    packetsPerFrame = width / linesPerPacket;

    if (_isRunning) StartDiss6();
}

//void ScanLib::InitScanParams()
//{
//    normalParams.imageWidth = std::stoi(Tools::parameterMap["imageWidth"]);
//    normalParams.imageHeight = std::stoi(Tools::parameterMap["imageHeight"]);
//    normalParams.acqTime = std::stoi(Tools::parameterMap["acqTime"]);
//
//    normalParams.lineStartDelay = std::stoi(Tools::parameterMap["lineStartDelay"]);
//    normalParams.flyBackCount = std::stoi(Tools::parameterMap["flyBackCount"]);
//    normalParams.lineFlyBackTime = std::stoi(Tools::parameterMap["lineFlyBackTime"]);
//    normalParams.frameFlyBackTime = std::stoi(Tools::parameterMap["frameFlyBackTime"]);
//
//    normalParams.frameCount = std::stoi(Tools::parameterMap["frameCount"]);
//    normalParams.fastAOffsetBright = std::stod(Tools::parameterMap["fastAOffsetBright"]);
//    normalParams.fastAGainContrast = std::stod(Tools::parameterMap["fastAGainContrast"]);
//
//    normalParams.isAverageEnable = Tools::parameterMap["isAverageEnable"] == "True" ? true : false;
//    normalParams.lineAvgCount = std::stoi(Tools::parameterMap["lineAvgCount"]);
//    normalParams.frameAvgCount = std::stoi(Tools::parameterMap["frameAvgCount"]);
//    normalParams.rotationAngle = std::stod(Tools::parameterMap["rotationAngle"]);
//}
//
//void ScanLib::InitReduceScanParams()
//{
//    reduceParams.reduceStartX = std::stoi(Tools::parameterMap["reduceStartX"]);
//    reduceParams.reduceStartY = std::stoi(Tools::parameterMap["reduceStartY"]);
//    reduceParams.reduceWidth = std::stoi(Tools::parameterMap["reduceWidth"]);
//	reduceParams.reduceHeight = std::stoi(Tools::parameterMap["reduceHeight"]);
//    reduceParams.reduceFullWidth = std::stoi(Tools::parameterMap["reduceFullWidth"]);
//	reduceParams.reduceFullHeight = std::stoi(Tools::parameterMap["reduceFullHeight"]);
//}

void ScanLib::Start(int imagingMode)
{
	_isRunning = true;

	// 线程锁，防止并发修改 imagingState
	{
		std::lock_guard<std::mutex> lock(mtx);
		imagingState = imagingMode;  // 设置状态
	}

	// 启动新的线程来执行 ImagingProc
	std::thread imagingThread([this]() { ImagingProc(); });
	imagingThread.detach();  // 分离线程，类似 Task.Run
}

void ScanLib::ImagingProc()
{
	if (_isRunning)
	{
		switch (imagingState)
		{
			case 1:
				Continuous();
				break;
			case 2:
				Grab();
				break;
			case 3:
				SubScan();
				break;
			case 4:
				Spot();
				break;
		}
		//Thread.Sleep(1000);
	}
}

void ScanLib::Continuous()
{
	_frameCount = 0;

	StartDiss6();
}

void ScanLib::Grab()
{

}

void ScanLib::SubScan()
{

}

void ScanLib::Spot()
{

}

std::vector<ChannelInfo_t> ScanLib::GetChannelSection()
{
	std::vector<ChannelInfo_t> channels;

	ChannelInfo_t* item = new ChannelInfo_t();
	item->part.datatype = CHANNEL_DATATYPE_U8;
	item->part.selection = CHANNEL_SEL_SLOW_DIRECT;
	item->part.id.part.source = CHANNEL_SOURCE_A_FAST_A;

	//Fast A
	if (_channelsA[0])
	{
		item->part.id.part.index = 0;
		channels.push_back(*item);
	}

	if (_channelsA[1])
	{
		item->part.id.part.index = 1;
		channels.push_back(*item);
	}

	if (_channelsA[2])
	{
		item->part.id.part.index = 2;
		channels.push_back(*item);
	}

	if (_channelsA[3])
	{
		item->part.id.part.index = 3;
		channels.push_back(*item);
	}

	//Fast B
	item->part.id.part.source = CHANNEL_SOURCE_A_FAST_B;
	if (_channelsB[0])
	{
		item->part.id.part.index = 0;
		channels.push_back(*item);
	}

	if (_channelsB[1])
	{
		item->part.id.part.index = 1;
		channels.push_back(*item);
	}

	if (_channelsB[2])
	{
		item->part.id.part.index = 2;
		channels.push_back(*item);
	}

	if (_channelsB[3])
	{
		item->part.id.part.index = 3;
		channels.push_back(*item);
	}

#pragma region SLOW Channel
	////Slow
	//item->part.id.part.source = CHANNEL_SOURCE_A_SLOW;
	//if (Tools::parameterMap["slow1"] == "True")
	//{
	//	item->part.id.part.index = 0;
	//	channels.push_back(*item);
	//}

	//if (Tools::parameterMap["slow2"] == "True")
	//{
	//	item->part.id.part.index = 1;
	//	channels.push_back(*item);
	//}

	//if (Tools::parameterMap["slow3"] == "True")
	//{
	//	item->part.id.part.index = 2;
	//	channels.push_back(*item);
	//}

	//if (Tools::parameterMap["slow4"] == "True")
	//{
	//	item->part.id.part.index = 3;
	//	channels.push_back(*item);
	//}

	//if (Tools::parameterMap["slow5"] == "True")
	//{
	//	item->part.id.part.index = 4;
	//	channels.push_back(*item);
	//}

	//if (Tools::parameterMap["slow6"] == "True")
	//{
	//	item->part.id.part.index = 5;
	//	channels.push_back(*item);
	//}

	//if (Tools::parameterMap["slow7"] == "True")
	//{
	//	item->part.id.part.index = 6;
	//	channels.push_back(*item);
	//}

	//if (Tools::parameterMap["slow8"] == "True")
	//{
	//	item->part.id.part.index = 7;
	//	channels.push_back(*item);
	//}

	//if (Tools::parameterMap["slow9"] == "True")
	//{
	//	item->part.id.part.index = 8;
	//	channels.push_back(*item);
	//}

	//if (Tools::parameterMap["slow10"] == "True")
	//{
	//	item->part.id.part.index = 9;
	//	channels.push_back(*item);
	//}

	//if (Tools::parameterMap["slow11"] == "True")
	//{
	//	item->part.id.part.index = 10;
	//	channels.push_back(*item);
	//}

	//if (Tools::parameterMap["slow12"] == "True")
	//{
	//	item->part.id.part.index = 11;
	//	channels.push_back(*item);
	//}

	//if (Tools::parameterMap["slow13"] == "True")
	//{
	//	item->part.id.part.index = 12;
	//	channels.push_back(*item);
	//}

	//if (Tools::parameterMap["slow14"] == "True")
	//{
	//	item->part.id.part.index = 13;
	//	channels.push_back(*item);
	//}

	//if (Tools::parameterMap["slow15"] == "True")
	//{
	//	item->part.id.part.index = 14;
	//	channels.push_back(*item);
	//}

	//if (Tools::parameterMap["slow16"] == "True")
	//{
	//	item->part.id.part.index = 15;
	//	channels.push_back(*item);
	//}
#pragma endregion
	return channels;
}

void ScanLib::StartDiss6()
{
	std::vector<ChannelInfo_t> channels = GetChannelSection();
	//if (job != nullptr)
	//{
	//	DISS6Control::Job::Remove(job->_Handle);
	//	job = nullptr;
	//}
	job = new ScanJob(channels);
	this->SetupCallbacks();

	try
	{
		ushort flybackCount = 512;

		ushort dacStepX = 0, dacStepY = 0, preScanX = 0, preScanY = 0;
		if (_scanMode == (int)Utils::ScanningMode::Reduce)//Reduce Scan mode
		{
			ushort reduceDacOffsetX, reduceDacOffsetY, reducePreScanX, reducePreScanY;
			Utils::CalcPreScanForReduceMode((double)_dwelltime, _fullWidth, _fullHeight, _startX, _startY, reduceDacOffsetX, reduceDacOffsetY, dacStepX, dacStepY, preScanX, preScanY);
			job->SetImageGeometry(_rscanWidth, _rscanHeight, reduceDacOffsetX, reduceDacOffsetY, dacStepX, dacStepY, preScanX, preScanY);
		}
		else if (_scanMode == (int)Utils::ScanningMode::SplitMode)
		{
			ushort imgW = _imgwidth / 2;
			Utils::CalcPreScanArgs((double)_dwelltime, imgW, _imgheight, dacStepX, dacStepY, preScanX, preScanY);
			job->SetImageGeometry(imgW, _imgheight, dacOffset, dacOffset, dacStepX / 2, dacStepY, preScanX, preScanY);
		}
		else if (_scanMode == (int)Utils::ScanningMode::DualMag)
		{
			ushort imgW = _imgwidth / 2;
			Utils::CalcPreScanArgs((double)_dwelltime, _imgwidth, _imgheight, dacStepX, dacStepY, preScanX, preScanY);
			job->SetImageGeometry(imgW, _imgheight, dacOffset, dacOffset, dacStepX, dacStepY, preScanX, preScanY);
		}
		else//Normal Scan Settings
		{
			Utils::CalcPreScanArgs((double)_dwelltime, _imgwidth, _imgheight, dacStepX, dacStepY, preScanX, preScanY);
			job->SetImageGeometry(_imgwidth, _imgheight, dacOffset, dacOffset, dacStepX, dacStepY, preScanX, preScanY);
		}

		if (_scanMode == (int)Utils::ScanningMode::SplitMode || _scanMode == (int)Utils::ScanningMode::DualMag)
		{
			job->SetBeamReturnTiming(_imgwidth / 2, TimeScale::NS100, _frameFlyBackTime);
		}
		else
		{
			job->SetBeamReturnTiming(_imgwidth, TimeScale::NS200, _frameFlyBackTime);
		}
	
		if (_avgMode == 0 || _avgMode == 3)//不做平均
		{
			DISS6Control::Job::SetBufferAveragingEnabled(job->_Handle, false);
		}
		else if (_avgMode == 1)//线平均
		{
			DISS6Control::Job::SetBufferAveragingEnabled(job->_Handle, true);
			DISS6Control::Job::SetLineAveragingCount(job->_Handle, _avgNum);
		}
		else if (_avgMode == 2)//帧平均
		{
			DISS6Control::Job::SetBufferAveragingEnabled(job->_Handle, true);
			DISS6Control::Job::SetFrameAveragingCount(job->_Handle, _avgNum);
		}

		//job->FrameCount = 1;
		job->FrameCount = _frameCount;
		job->setFrameCount(job->FrameCount);//设置一帧图 or 像素点平均

		job->LineStartDelay = TimeScale::US50;
		job->setLineStartDelay(job->LineStartDelay);

		job->AcquisitionTime = (uint)(this->_dwelltime / 10);
		job->setAcquisitionTime(job->AcquisitionTime);

		job->Start(AbortCode::Immediately);
		
		_isRunning = true;
	}
	catch (...)
	{
		std::cout << "Start Scan Error! Exception!" << std::endl;
	}
}

void ScanLib::HandleDataAvailable()
{
	uint pixelsPerFrame = _ImageBufferSize;

	uint pixelCount = 0;
	uint frameIndex = 0;
	uint pixelOffset = 0;
	uint status = 0;

	while ((status & ReadChannelDataStatus::BufferEmpty) == 0)
	{
		//std::cout << "==========HandleDataAvailable==========" << std::endl;
		if (_MarshalledFramePtrs == nullptr)
		{
			std::cout << "Error: _MarshalledFramePtrs[0] = NULL" << std::endl;
			return;
		}

		//pixelsPerFrame = sizeof(_FrameBuffers[0]) / sizeof(_FrameBuffers[0][0]);
		//Logger::GetInstance() << "pixelsPerFrame = " << pixelsPerFrame << std::endl;
		pixelCount = job->ReadChannelData(_MarshalledFramePtrs, pixelsPerFrame, frameIndex, pixelOffset, status);

		//std::cout << "===============================" << std::endl;
		// Logger::GetInstance() << "PixelCount = " << pixelCount << " ,frameIndex = " << frameIndex << " ,PixOffset = " << pixelOffset << std::endl;

		if (pixelCount == 0) return;

		if ((status & ReadChannelDataStatus::BufferOverflow) != 0)
		{
			std::cout << "Buffer overflow" << std::endl;
			break;
		}

		if ((status & ReadChannelDataStatus::OffsetMismatch) != 0)
		{
			std::cout << "Offset mismatch" << std::endl; // fast A offset.
			break;
		}

		if (_Shutdown == false)
		{
			HandlePixelToSend(pixelOffset, pixelCount);
			if ((status & ReadChannelDataStatus::FrameEnd) != 0)
			{
				////将八位数据转化为十六位数据代码
				//uint16_t* buffer16bit = new uint16_t[job->_ImageWidth * job->_ImageHeight];

				//// 转换数据 - 将8位数据扩展到16位
				//char* sourceData = static_cast<char*>(_MarshalledFramePtrs[0]); // 先转换为char*
				//for (int i = 0; i < job->_ImageWidth * job->_ImageHeight; i++) {
				//	// 将8位值扩展到16位
				//	buffer16bit[i] = static_cast<uint16_t>(static_cast<unsigned char>(sourceData[i])) << 8;
				//}

				//RetCode_t rc = WriteTiffFile("FastA1.tif", job->_ImageWidth, job->_ImageHeight, buffer16bit);
				//if (rc != DC_RET_NO_ERROR)
				//	std::cout << "Creating file FastA1.tif failed (" << rc << ")!\n";

				// 直接使用 8 位数据的指针
				//uint8_t* sourceData = static_cast<uint8_t*>(_MarshalledFramePtrs[0]); // 转换为 char*

				//WritePGMImage("output.pgm", sourceData, job->_ImageWidth, job->_ImageHeight);

				/*std::cout << "status & ReadChannelDataStatus::FrameEnd" << std::endl;*/
				break;
			}
		}
	};
}

void ScanLib::HandleScanJobRunning()
{
	//初始化通道数，初始化获取数据内存大小
	std::cout << "********** HandleScanJobRunning **********" << std::endl;
	try
	{
		_ImageBufferSize = static_cast<int>(job->_ImageWidth * job->_ImageHeight);
		int channelCount = job->_Channels.size();

		/*std::cout << "ChannelCount = " << channelCount << std::endl;*/

		//_FrameBuffers = new byte*[channelCount];
		if (_MarshalledFramePtrs)
		{
			for (int i = 0; i < channelCount; i++)
			{
				delete[] static_cast<char*>(_MarshalledFramePtrs[i]);
			}
			delete[] _MarshalledFramePtrs;
			_MarshalledFramePtrs = nullptr;
		}

		_MarshalledFramePtrs = new void* [channelCount];
		for (int i = 0; i < channelCount; i++)
		{
			//_FrameBuffers[i] = new byte[_ImageBufferSize];
			_MarshalledFramePtrs[i] = new char[_ImageBufferSize];
		}
	}
	catch (...) {
		std::cout << "Handle ScanJob Running Failed!" << std::endl;
	}
}

void ScanLib::HandleDataTimeout()
{
	_BitmapUpdateNeeded = true;
}

void ScanLib::HandleScanJobFinished()
{
	std::cout << "=======Finish=======" << std::endl;
	job->Dispose(true);
}

void ScanLib::SetupCallbacks()
{
	// 假设 job 是 Manager 类的一个成员
	job->SetDataAvailableCallback(std::bind(&ScanLib::HandleDataAvailable, this));
	job->SetScanJobRunningCallback(std::bind(&ScanLib::HandleScanJobRunning, this));
	job->SetScanJobFinishedCallback(std::bind(&ScanLib::HandleScanJobFinished, this));
	job->SetScanJobTimeOutCallback(std::bind(&ScanLib::HandleDataTimeout, this));
}

void ScanLib::StopScan()
{
	if (job != nullptr && _isStopImmediately)
	{
		DISS6Control::Job::Stop(job->_Handle, AbortCode::Immediately);
	}
	else if (job != nullptr && !_isStopImmediately)
	{
		DISS6Control::Job::Stop(job->_Handle, AbortCode::AfterFrame);
	}

	_isRunning = false;
}

void ScanLib::HandlePixelToSend(uint pixelOffset, uint pixelCount)
{
	if (pixelCount == 0)
	{
		return;
	}

	if (!_isRunning && _isStopImmediately) return;


	int detCount = 1;// Normal Scan / Reduce Scan

	if (pixelOffset + pixelCount > _ImageBufferSize)
	{
		std::cout << "Memory overflow. copy to bitmap" << std::endl;
		return;
	}
	//std::cout << "HandlePixelToSend: \t" << DetectorIndex[0] - 1 << std::endl;
	/*Logger::GetInstance() << "HandlePixelToSend: \t" << DetectorIndex[0] - 1 << std::endl;*/

	byte* buf = static_cast<byte*>(_MarshalledFramePtrs[DetectorIndex[0] - 1]);
	Tools::EnqueueData(buf, _scanMode, pixelOffset, pixelCount);
	//Logger::GetInstance() << "Enqueue Data: \t offset = " << pixelOffset << " ,count = " << pixelCount << std::endl;
}

void ScanLib::SelectChannel(FastChannelSelection fastChSelection, bool* channelSelection)
{
	_channelCount = 0;
	int index = 0;
	switch (fastChSelection)
	{
	case FAST_A://inlens se2  bsd  stem ccd 
		index = 0;
		_channelsA[index] = channelSelection[index++];
		_channelsA[index] = channelSelection[index++];
		_channelsA[index] = channelSelection[index++];
		_channelsA[index] = channelSelection[index++];
		break;
	case FAST_B:
		_channelsB[index] = channelSelection[index++];
		_channelsB[index] = channelSelection[index++];
		_channelsB[index] = channelSelection[index++];
		_channelsB[index] = channelSelection[index++];
		break;
	default:
		break;
	}

	for (int i = 0; i < 4; i++)//i < channelSelection.size()
	{
		if (channelSelection[i])
		{
			_channelCount += 1;
		}
	}

}

void ScanLib::SetScanVoltage(double scanVoltage)
{
	if (scanVoltage >= 3.5 && scanVoltage <= 12)
	{
		DISS6Control::Adjustment::Scan::SetGainX(scanVoltage);
	}
}

void ScanLib::SetCurrentChannel(int channelIndex)
{
	_currentChannel = channelIndex;
}

double ScanLib::GetScanRotationAngle()
{
	return _scanRotate;
}

double ScanLib::GetBrightnessByChannel(int channelIndex)
{
	switch (channelIndex)
	{
	case 1:
	{
		return brightness_A1;
	}
	break;
	case 2:
	{
		return brightness_A2;
	}
	break;
	case 3:
	{
		return brightness_A3;
	}
	break;
	case 4:
	{
		return brightness_A4;
	}
	break;
	}
	return 0;
}

double ScanLib::GetContrastByChannel(int channelIndex)
{
	switch (channelIndex)
	{
	case 1:
	{
		return contrast_A1;
	}
	break;
	case 2:
	{
		return contrast_A2;
	}
	break;
	case 3:
	{
		return contrast_A3;
	}
	break;
	case 4:
	{
		return contrast_A4;
	}
	break;
	}
	return 0;
}




#pragma region MMF注释代码

//
//int* pHeaderData = nullptr;
//byte* pMMFData = nullptr;

//// 根据扫描模式选择合适的缓冲区
//if (scanMode == static_cast<int>(Utils::ScanningMode::Normal)) {
//	pHeaderData = static_cast<int*>(pBuf);
//	pMMFData = static_cast<byte*>(pBuf);
//}
//else if (scanMode == static_cast<int>(Utils::ScanningMode::Reduce)) {
//	pHeaderData = static_cast<int*>(pBuf_Reduce);
//	pMMFData = static_cast<byte*>(pBuf_Reduce);
//}

//if (pHeaderData && pMMFData) {
//	// 填充头部数据
//	readflag = 1;
//	pHeaderData[0] = scanMode;      // 记录扫描模式
//	pHeaderData[1] = pixelCount;    // 像素数量
//	pHeaderData[2] = pixelOffset;   // 像素偏移
//	pHeaderData[3] = readflag;

//	logger.LogPacket(pixelCount, pixelOffset);
//	//std::cout << "MMF CPP Count = " << pixelCount << " ,CPP Offset = " << pixelOffset << std::endl;

//	// 复制图像数据到内存映射文件缓冲区
//	memcpy(pMMFData + MMFOffset, buf + pixelOffset, pixelCount);

//	//WaitForSingleObject(mmfMutex, INFINITE);
//	
//	// 入队操作
//	Tools::EnqueueData(buf, pixelOffset, pixelCount);

//	//// 从队列中取出数据进行 memcpy
//	//if (!Tools::dataQueue.empty()) {
//	//	auto dataToSend = Tools::dataQueue.front();
//	//	memcpy(pMMFData + MMFOffset, dataToSend.data(), dataToSend.size());
//	//	Tools::dataQueue.pop(); // 处理完毕后出队
//	//}
//	//ReleaseMutex(mmfMutex);
//}

//const wchar_t* ScanLib::mmfMapName = L"MMFFILE";
//HANDLE ScanLib::hMapFile = NULL;
//LPVOID ScanLib::pBuf = NULL;
//
//int ScanLib::InitMMFParams()
//{
//	int mmfBufferSize = static_cast<int>(1024 * 1024 + MMFOffset);
//	hMapFile = CreateFileMapping(
//		INVALID_HANDLE_VALUE,
//		NULL,
//		PAGE_READWRITE,
//		0,
//		mmfBufferSize,
//		mmfMapName
//	);
//	if (hMapFile == NULL) {
//		std::cerr << "Could not create file mapping object: " << GetLastError() << std::endl;
//		return 1;
//	}
//
//	// 映射内存
//	pBuf = MapViewOfFile(
//		hMapFile,
//		FILE_MAP_ALL_ACCESS,
//		0,
//		0,
//		mmfBufferSize);
//
//	if (pBuf == NULL) {
//		std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
//		CloseHandle(hMapFile);
//		return 1;
//	}
//
//	return 0;
//}

//int ScanLib::UninitMMFParams()
//{
//	// 清理
//	UnmapViewOfFile(pBuf);
//	CloseHandle(hMapFile);
//	//CloseHandle(hSyncEvent);
//
//	return 0;
//}
//
//const wchar_t* ScanLib::mmfMapName_Reduce = L"MMFFILE_REDUCE";
//HANDLE ScanLib::hMapFile_Reduce = NULL;
//LPVOID ScanLib::pBuf_Reduce = NULL;

//int ScanLib::InitMMFReduceParams()
//{
//	int mmfBufferSize = static_cast<int>(1024 * 1024 + MMFOffset);
//	ScanLib::hMapFile_Reduce = CreateFileMapping(
//		INVALID_HANDLE_VALUE,
//		NULL,
//		PAGE_READWRITE,
//		0,
//		mmfBufferSize,
//		ScanLib::mmfMapName_Reduce
//	);
//	if (ScanLib::hMapFile_Reduce == NULL) {
//		std::cerr << "Could not create file mapping object Reduce: " << GetLastError() << std::endl;
//		return 1;
//	}
//
//	// 映射内存
//	ScanLib::pBuf_Reduce = MapViewOfFile(
//		hMapFile_Reduce,
//		FILE_MAP_ALL_ACCESS,
//		0,
//		0,
//		mmfBufferSize);
//
//	if (ScanLib::pBuf_Reduce == NULL) {
//		std::cerr << "Could not map view of file Reduce: " << GetLastError() << std::endl;
//		CloseHandle(ScanLib::hMapFile_Reduce);
//		return 1;
//	}
//
//	return 0;
//}
//
//int ScanLib::UninitMMFReduceParams()
//{
//	// 清理
//	UnmapViewOfFile(pBuf_Reduce);
//	CloseHandle(hMapFile_Reduce);
//
//	return 0;
//}

//int ScanLib::InitMMFReduceParams()
//{
//	int mmfBufferSize = static_cast<int>(reduceParams.reduceFullWidth * reduceParams.reduceFullWidth + MMFOffset);
//	hMapFile_Reduce = CreateFileMapping(
//		INVALID_HANDLE_VALUE,
//		NULL,
//		PAGE_READWRITE,
//		0,
//		mmfBufferSize,
//		mmfMapName_Reduce
//	);
//	if (hMapFile_Reduce == NULL) {
//		std::cerr << "Could not create file mapping object Reduce: " << GetLastError() << std::endl;
//		return 1;
//	}
//
//	// 映射内存
//	pBuf_Reduce = MapViewOfFile(
//		hMapFile_Reduce,
//		FILE_MAP_ALL_ACCESS,
//		0,
//		0,
//		mmfBufferSize);
//
//	if (pBuf_Reduce == NULL) {
//		std::cerr << "Could not map view of file Reduce: " << GetLastError() << std::endl;
//		CloseHandle(hMapFile_Reduce);
//		return 1;
//	}
//
//	return 0;
//}

#pragma endregion