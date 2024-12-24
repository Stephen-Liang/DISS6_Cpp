#include <iostream>
#include "DISS6.h"
#include "DISS6BasicFunc.hpp"
#include "DISS6NativeMethods.hpp"
#include <zmq.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <fcntl.h> // for _O_U16TEXT
#include <io.h>    // for _setmode
#include "HAL.h"
#include "CommonTools.h"
#include "ScanLib.h"

// 全局变量
ScanLib* scan = new ScanLib();//WPF Server写法 main --> dvScanGenerator

//初始化字符串。一次初始化，全局通用 From dvScanGenerator.cs 对应device层
const std::string pnImageSize = "ImageSizeLsv", pnScanAvgMode = "ScanAvgModeLsv", pnAverages = "AveragesLsv", pnDwell = "DwellLsv", pnScanState = "ScanStateLsv", pnBrightness = "BrightnessLsv", pnContrast = "ContrastLsv", pnSignalMix = "SignalMixLsv", pnStopMode = "StopModeLsv", pnRotation = "RotationLsv", pnCalibrationAngle = "CalibrationAngleLsv", pnDualChannelBrightness = "DualChannelBrightnessLsv", pnDualChannelContrast = "DualChannelContrastLsv", pnDetector = "DetectorLsv", pnDetector1 = "Detector1Lsv", pnDetector2 = "Detector2Lsv", pnDetector3 = "Detector3Lsv", pnConnectionState = "ConnectionState", pnScanMode = "ScanModeLsv", pnSetMixMode = "SetMixMode";

const std::string pnStartContinuous = "StartContinuous", pnStopNow = "StopNow", pnReduceScan = "ReduceScan", pnIsRunning = "isRunning";
ScanManager& managerIns = ScanManager::GetInstance();
ScanState& stateIns = ScanState::GetInstance();
HAL& hal = HAL::GetInstance();

//MessageQueue parameterQueue;//直接从UI上给参数
CommandQueue commandQueue;//command 队列
std::atomic<bool> running(true);

#pragma region 逻辑调用函数
//计算帧循环时间
double CalcCycleTimeForFrame(int LineFlyBackTime = 100, int LineStartDelay = 20000)
{
	double ctTime = 0.0;
	long cycleTime = 0;

	int avgMode = managerIns._scanAvgModeLsv;
	int DwellTime = managerIns._dwellLsv;
	int imageWidth = managerIns.GetImageSize(managerIns._imageSizeLsv);
	int avgNums = managerIns._averagesLsv;

	if (avgMode != 1)
		cycleTime = DwellTime * imageWidth / 1000 * imageWidth + LineFlyBackTime * imageWidth / 1000 * imageWidth + LineStartDelay * imageWidth / 1000;
	else
	{
		cycleTime = (DwellTime * imageWidth * avgNums) / 1000 * imageWidth
			+ LineFlyBackTime * avgNums * imageWidth / 1000 * imageWidth +
			LineStartDelay * imageWidth / 1000;
	}
	ctTime = (double)cycleTime / 1000.0;  // to ms
	return ctTime;
}
#pragma endregion

//发送回复消息
void sendReply(zmq::socket_t& socket, const std::string& message)
{
	// 使用字符串的实际长度来创建消息，避免多余的 "\0"
	zmq::message_t reply(message.size());
	memcpy(reply.data(), message.data(), message.size());
	socket.send(reply, zmq::send_flags::none);
}

#pragma region switch-case处理函数，调用扫描发生器本身
//Normal Scan
void StartContinuous() {
	scan->SetScanMode(Utils::ScanningMode::Normal);
	managerIns._targetScanState = 1;

	if (managerIns._targetScanState != managerIns._scanStateLsv)
	{
		managerIns._scanStateLsv = managerIns._targetScanState;
		hal.Update(pnScanState, managerIns._scanStateLsv);

		scan->RefreshImagingSettings(managerIns._imageWidth, managerIns._imageHeight, managerIns._dwellLsv, managerIns._scanAvgModeLsv, managerIns._averagesLsv);

		managerIns._isStopScan = false;//未停止扫描

		if (!scan->_isRunning)
		{
			scan->Start(managerIns._targetScanState);
		}

		/*Logger::GetInstance() << "ScanMode = " << (int)managerIns._scanMode << "imageWidth = " << managerIns._imageWidth << "\t imageHeight = " << managerIns._imageHeight << "\t _dwellLsv = " << managerIns._dwellLsv << "\t scanAvgModeLsv = " << managerIns._scanAvgModeLsv << "\t averagesLsv = " << managerIns._averagesLsv << std::endl;*/
	}
}

void ReduceScan()
{
	scan->SetScanMode(Utils::ScanningMode::Reduce);
	managerIns._targetScanState = 5;
	managerIns._scanStateLsv = managerIns._targetScanState;
	hal.Update(pnScanState, managerIns._scanStateLsv);

	scan->_isChangeSize = true;

	managerIns._startX = hal.GetIntParam("rstartX");
	managerIns._startY = hal.GetIntParam("rstartY");
	managerIns._reduceWidth = hal.GetIntParam("reduceWidth");
	managerIns._reduceHeight = hal.GetIntParam("reduceHeight");

	scan->SetReduceMode(managerIns._startX, managerIns._startY, managerIns._reduceWidth, managerIns._reduceHeight, managerIns._imageWidth, managerIns._imageHeight);

	Logger::GetInstance() << "ReduceScan PARAM: \t" << managerIns._startX <<", " << managerIns._startY << ", " << managerIns._reduceWidth << ", " << managerIns._reduceHeight << std::endl;
}

void ChangeDetector(int targetDet)
{
	managerIns._selectDetLsv = targetDet;

	if (managerIns._scanMode == Utils::ScanningMode::DualChannel)
	{
		scan->SetDualChannelMode(managerIns._selectDetLsv, managerIns._selectDet2Lsv);
	}

	scan->DetectorIndex[0] = managerIns._selectDetLsv;

	hal.Update(pnBrightness, (int)scan->GetBrightnessByChannel(targetDet));
	hal.Update(pnContrast, (int)scan->GetContrastByChannel(targetDet));
	hal.Update(pnDetector, targetDet);
}

void DefaultHandler() {
	// 处理默认命令
	std::cout << "Input CommandName" << std::endl;
	Logger::GetInstance() << "Input CommandName" << std::endl;
}

void StopNow()
{
	managerIns._targetScanState = 0;

	std::cout << "Scan Stop Now!" << std::endl;
	Logger::GetInstance() << "Scan Stop Now!" << std::endl;

	if (managerIns._targetScanState != managerIns._scanStateLsv)
	{
		managerIns._scanStateLsv = managerIns._targetScanState;
		scan->StopScan();
		hal.Update(pnScanState, managerIns._targetScanState);
	}

	managerIns._isStopScan = true;//停扫标志位
	hal.Update(pnIsRunning, 0);
}

void ChangeImgSize(int targetSize)
{
	managerIns._imageSizeLsv = targetSize;
	managerIns._imageWidth = managerIns.GetImageSize(targetSize);
	managerIns._imageHeight = managerIns._imageWidth;
	scan->_isChangeSize = true;

	double val = CalcCycleTimeForFrame();
	//添加定时器部分暂时没加
	if (managerIns._isStopScan && !scan->_isStopImmediately)
	{
		scan->_isStopImmediately = true;
		scan->StopScan();
		scan->_isStopImmediately = false;

		scan->RefreshImagingSettings(managerIns._imageWidth, managerIns._imageHeight, managerIns._dwellLsv, managerIns._scanAvgModeLsv, managerIns._averagesLsv);
		scan->Start(2);//提问：成像模式写2？grab模式？没有道理。
	}
	else
	{
		scan->RefreshImagingSettings(managerIns._imageWidth, managerIns._imageHeight, managerIns._dwellLsv, managerIns._scanAvgModeLsv, managerIns._averagesLsv);
	}

	hal.Update(pnImageSize, targetSize);
}

void ChangeScanMode(int targetScanMode)
{
	if (managerIns._scanMode == Utils::ScanningMode::Reduce && targetScanMode != 5)
	{
		//managerIns._imageHeight = managerIns._fullImageHeight;
		//managerIns._imageWidth = managerIns._fullImageWidth;
	}

	if (managerIns._scanMode != Utils::ScanningMode::SplitMode && targetScanMode == 1)
	{

	}
	else if (managerIns._scanMode != Utils::ScanningMode::QaudMode && targetScanMode == 4)
	{

	}
	else if (managerIns._scanMode != Utils::ScanningMode::DualChannel && targetScanMode == 2)
	{

	}
	else
	{
		scan->DetectorIndex[0] = managerIns._selectDetLsv;
		hal.Update(pnDetector, managerIns._selectDetLsv);
	}

	scan->SetCurrentChannel(managerIns._selectDetLsv);

	hal.Update(pnBrightness, (int)scan->GetBrightnessByChannel(managerIns._selectDetLsv));
	hal.Update(pnContrast, (int)scan->GetContrastByChannel(managerIns._selectDetLsv));
	hal.Update(pnScanMode, targetScanMode);
	scan->SetScanMode((Utils::ScanningMode)targetScanMode);
	Logger::GetInstance() << "void ChangeScanMode(int targetScanMode){}" << "Set Scan Mode Lsv" << targetScanMode << std::endl;
}

void SetMixModeScan()
{
	managerIns._isMixOn = hal.GetBoolParam("IsOn");

}
#pragma endregion

#pragma region Thread 处理通信函数
// 解析消息参数 command 类似component层操作. 类似component层的分拆，但是component的update不要了，只保留device层的update
void CommandThread()
{
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);
	socket.bind(IPConfig::ipCmd);

	while (running) {
		zmq::message_t request;

		// 使用 try_recv 来允许线程在没有消息时也能退出
		if (socket.recv(request, zmq::recv_flags::dontwait)) {

			Command::argNames.clear();
			Command::parameters.clear();

			Command cmd = Command::Deserialize(request.to_string());
			Command::UpdateArgNames();
			
			sendReply(socket, "From scan server: cmd.DeviceID = " + cmd.DeviceID + " cmd.CommandName = " + cmd.CommandName);
			Logger::GetInstance() << "From scan server: cmd.DeviceID = " + cmd.DeviceID + " cmd.CommandName = " + cmd.CommandName << std::endl;

			//在这里执行component层的分拆操作.ScanGenerator.cs 83行 ComponentComProc 
			Command* dvCmd = new Command();//重新给device 下发指令
			if (cmd.CommandName == "SetParam")
			{
				std::string cmdName = cmd.argNames[0];//获取 set 的参数
				dvCmd->DeviceID = "Scan";
				dvCmd->CommandName = cmdName;

				if (cmdName == pnImageSize || cmdName == pnScanAvgMode || cmdName == pnAverages || cmdName == pnDwell || cmdName == pnBrightness || cmdName == pnContrast )
				{
					dvCmd->SetCommandArg(cmdName, cmd.GetCommandArg(cmdName, 0));
				}
				else if (cmdName == pnDetector || cmdName == pnDetector1 || cmdName == pnDetector2 || cmdName == pnDetector3)
				{
					dvCmd->SetCommandArg(cmdName, cmd.GetCommandArg(cmdName, 0));
					//SetChamberLEDLevelWhenDetChanged(1, cmd.GetCommandArg(cmdName, 0)); //TODO 设置ccd相机
				}
				else if (cmdName == pnRotation)
				{
					double angle = cmd.GetCommandArg(cmdName, 0.0);
					if (angle > 180)
					{
						angle -= 360;
					}
					else if (angle < -180)
					{
						angle += 360;
					}
				}
				else if (cmdName == pnReduceScan)
				{

				}
			}
			else
			{
				*dvCmd = cmd;
			}

			commandQueue.push(*dvCmd);
		}
	}
}

void DeviceProcThread()
{
	Command dvCmd;

	while (running)
	{
		//出队和解析
		Command cmd = commandQueue.pop();

		if (cmd.CommandName == pnStartContinuous) StartContinuous();
		else if (cmd.CommandName == pnStopNow) StopNow();
		else if (cmd.CommandName == pnReduceScan) ReduceScan();
		else if (cmd.CommandName == pnDetector)
		{
			auto targetDet = cmd.GetCommandArg(cmd.CommandName, 0);//涉及到获取对应get值。因此这一步写到if语句中
			if (targetDet != managerIns._selectDetLsv)
			{
				ChangeDetector(targetDet);
			}
			else
			{
				scan->SelectDetectorChannel(true, true, true, true);
			}
		}
		else if (cmd.CommandName == pnImageSize)
		{
			auto targetSize = cmd.GetCommandArg(cmd.CommandName, -1);
			if (targetSize != managerIns._imageSizeLsv)
			{
				ChangeImgSize(targetSize);
			}
		}
		else if (cmd.CommandName == pnScanMode)
		{
			auto targetScanMode = cmd.GetCommandArg(cmd.CommandName, -1);
			ChangeScanMode(targetScanMode);
		}
		else if (cmd.CommandName == pnStopMode)
		{
			auto targetStopMode = cmd.GetCommandArg(cmd.CommandName, -1);
			if (targetStopMode == 0) scan->_isStopImmediately = true;
			else scan->_isStopImmediately = false;//targetStopMode == 1
		}
		else if (cmd.CommandName == pnDetector1)
		{

		}
		else if (cmd.CommandName == pnDetector2)
		{

		}
		else if (cmd.CommandName == pnDetector3)
		{

		}
		else if (cmd.CommandName == pnSetMixMode) SetMixModeScan();
		else
		{

		}

#pragma region 函数映射
		//// 命令处理函数映射
		//std::map<std::string, std::function<void()>> commandHandlers;
		//commandHandlers["StartContinuous"] = StartContinuous;
		//commandHandlers["StopNow"] = StopNow;  // 示例，其他命令处理函数
		//commandHandlers["ReduceScan"] = ReduceScan;
		// 
		//// 查找命令并执行相应的处理函数
		//auto it = commandHandlers.find(cmd.CommandName);
		//if (it != commandHandlers.end()) {
		//	it->second();  // 调用对应的处理函数
		//}
		//else {
		//	DefaultHandler();  // 默认处理
		//}
#pragma endregion
	}
}

//// 修改参数，初始化后更新参数
//void ParameterThread() {
//	zmq::context_t context(1);
//	zmq::socket_t socket(context, ZMQ_REP);
//	socket.bind(IPConfig::ipParams);
//
//	while (running) 
//	{
//		zmq::message_t request;
//
//		// 使用 try_recv 来允许线程在没有消息时也能退出
//		if (socket.recv(request, zmq::recv_flags::dontwait)) {
//
//			std::string msg = request.to_string();
//
//			//std::cout << "From Params ======= Received: " << msg << std::endl;
//			Logger::GetInstance() << "From Params ======= Received: " << msg << std::endl;
//
//			if (msg.find("#Header:InitParameters") != std::string::npos)
//			{
//				//分解和初始化
//				Tools::DeserializeParameters(msg);
//				scan->InitScanParams();//提问：初始化到了NormalParams结构体中。但是之后可能要修改到其他地方，供全局使用 HAL update可以，定义一个放置所有
//				sendReply(socket, "Received:InitParams");
//			}
//			else if (msg.find("#Header:ReduceParamters") != std::string::npos)
//			{
//				//小框扫描
//				Tools::DeserializeParameters(msg);
//				scan->InitReduceScanParams();//提问：初始化到了NormalParams结构体中。但是之后可能要修改到其他地方，供全局使用 HAL update可以，定义一个放置所有
//				sendReply(socket, "Received:ReduceScan");
//			}
//			else
//			{
//				sendReply(socket, "Received: None");
//			}
//			//还有双通道，双放大，四通道，四通道reducescan......
//		}
//		else
//		{
//
//		}
//	}
//}
	
void ImageDataThread() {
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_PUSH);  // ZMQ_PUSH 用于发送图像数据
	socket.bind(IPConfig::ipImage);      // 绑定另一个端口，用于图像数据传输

	while (running) {
		// 检查队列是否为空
		if (!Tools::dataQueue.empty())
		{
			// 从队列中取出数据
			auto dataToSend = Tools::dataQueue.front();

			// 创建 ZMQ 消息并发送
			zmq::message_t imgMessage(dataToSend.data(), dataToSend.size());
			socket.send(imgMessage, zmq::send_flags::none);

			Tools::dataQueue.pop(); // 处理完毕后出队
		}
		else
		{
			// 队列为空时，等待数据到来
			//std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 可调节等待时间
		}
	}
}
#pragma endregion

#pragma region Initialize
void InitializeDevice()
{
	hal.Update(pnConnectionState, 1);
	hal.Update(pnScanState, managerIns._scanStateLsv);
	hal.Update(pnScanMode, (int)managerIns._scanMode);

	hal.Update(pnImageSize, managerIns._imageSizeLsv);//相当于update width & height
	hal.Update(pnScanAvgMode, managerIns._scanAvgModeLsv);
	hal.Update(pnAverages, managerIns._averagesLsv);
	hal.Update(pnDwell, managerIns._dwellLsv);
	hal.Update(pnDetector, managerIns._selectDetLsv);
	hal.Update(pnBrightness, managerIns._brightnessLsv);
	hal.Update(pnContrast, managerIns._contrastLsv);

	managerIns._imageHeight = managerIns.GetImageSize(managerIns._imageSizeLsv);

	managerIns._imageWidth = managerIns._imageHeight;

	FastChannelSelection fastCH = FastChannelSelection::FAST_A;
	bool channel[4] = { true, true, true, true };

	scan->SelectChannel(fastCH, channel);

	if (scan->_isInit) scan->SetScanVoltage(managerIns._scanVoltage);
	managerIns._scanRotateLsv = scan->GetScanRotationAngle();
	hal.Update(pnRotation, managerIns._scanRotateLsv);
	
	scan->SelectDetectorChannel(true, true, true, true);
	scan->DetectorIndex[0] = 1;
	scan->DetectorIndex[1] = 2;
	scan->DetectorIndex[2] = 3;
	scan->DetectorIndex[3] = 4;

	//TODO 考虑事件的挂接怎么处理
	//HAL.Instance.DoubleParamChanged += Instance_DoubleParamChanged;     
	//HAL.Instance.IntParamChanged += Instance_IntParamChanged;
}
#pragma endregion


// 主函数
int main()
{
	InitializeDevice();

	std::thread commandThread(CommandThread);//通信线程，发送指令. 入队
	//std::thread paramThread(ParameterThread);//发送和解析参数线程
	std::thread imageThread(ImageDataThread);//传输图像
	std::thread deviceThread(DeviceProcThread);//出队

	std::cout << "Server running. Press Enter to exit." << std::endl;
	Logger::GetInstance() << "Server running. Press Enter to exit." << std::endl;

	std::cin.get();

	running = false;

	commandThread.join();
	//paramThread.join();
	imageThread.join();
	deviceThread.join();

	return 0;
}

#pragma region MMF参数代码
	//HANDLE hMapFile_Reduce;
	//LPVOID pBuf_Reduce;
	//const wchar_t* mmfMapName_Reduce = L"MMFFILE_REDUCE";

	//ScanLib::InitMMFParams();
	//ScanLib::InitMMFReduceParams();

	//ScanLib::UninitMMFParams();
	//ScanLib::UninitMMFReduceParams();
#pragma endregion
