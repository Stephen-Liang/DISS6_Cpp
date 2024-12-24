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

// ȫ�ֱ���
ScanLib* scan = new ScanLib();//WPF Serverд�� main --> dvScanGenerator

//��ʼ���ַ�����һ�γ�ʼ����ȫ��ͨ�� From dvScanGenerator.cs ��Ӧdevice��
const std::string pnImageSize = "ImageSizeLsv", pnScanAvgMode = "ScanAvgModeLsv", pnAverages = "AveragesLsv", pnDwell = "DwellLsv", pnScanState = "ScanStateLsv", pnBrightness = "BrightnessLsv", pnContrast = "ContrastLsv", pnSignalMix = "SignalMixLsv", pnStopMode = "StopModeLsv", pnRotation = "RotationLsv", pnCalibrationAngle = "CalibrationAngleLsv", pnDualChannelBrightness = "DualChannelBrightnessLsv", pnDualChannelContrast = "DualChannelContrastLsv", pnDetector = "DetectorLsv", pnDetector1 = "Detector1Lsv", pnDetector2 = "Detector2Lsv", pnDetector3 = "Detector3Lsv", pnConnectionState = "ConnectionState", pnScanMode = "ScanModeLsv", pnSetMixMode = "SetMixMode";

const std::string pnStartContinuous = "StartContinuous", pnStopNow = "StopNow", pnReduceScan = "ReduceScan", pnIsRunning = "isRunning";
ScanManager& managerIns = ScanManager::GetInstance();
ScanState& stateIns = ScanState::GetInstance();
HAL& hal = HAL::GetInstance();

//MessageQueue parameterQueue;//ֱ�Ӵ�UI�ϸ�����
CommandQueue commandQueue;//command ����
std::atomic<bool> running(true);

#pragma region �߼����ú���
//����֡ѭ��ʱ��
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

//���ͻظ���Ϣ
void sendReply(zmq::socket_t& socket, const std::string& message)
{
	// ʹ���ַ�����ʵ�ʳ�����������Ϣ���������� "\0"
	zmq::message_t reply(message.size());
	memcpy(reply.data(), message.data(), message.size());
	socket.send(reply, zmq::send_flags::none);
}

#pragma region switch-case������������ɨ�跢��������
//Normal Scan
void StartContinuous() {
	scan->SetScanMode(Utils::ScanningMode::Normal);
	managerIns._targetScanState = 1;

	if (managerIns._targetScanState != managerIns._scanStateLsv)
	{
		managerIns._scanStateLsv = managerIns._targetScanState;
		hal.Update(pnScanState, managerIns._scanStateLsv);

		scan->RefreshImagingSettings(managerIns._imageWidth, managerIns._imageHeight, managerIns._dwellLsv, managerIns._scanAvgModeLsv, managerIns._averagesLsv);

		managerIns._isStopScan = false;//δֹͣɨ��

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
	// ����Ĭ������
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

	managerIns._isStopScan = true;//ͣɨ��־λ
	hal.Update(pnIsRunning, 0);
}

void ChangeImgSize(int targetSize)
{
	managerIns._imageSizeLsv = targetSize;
	managerIns._imageWidth = managerIns.GetImageSize(targetSize);
	managerIns._imageHeight = managerIns._imageWidth;
	scan->_isChangeSize = true;

	double val = CalcCycleTimeForFrame();
	//��Ӷ�ʱ��������ʱû��
	if (managerIns._isStopScan && !scan->_isStopImmediately)
	{
		scan->_isStopImmediately = true;
		scan->StopScan();
		scan->_isStopImmediately = false;

		scan->RefreshImagingSettings(managerIns._imageWidth, managerIns._imageHeight, managerIns._dwellLsv, managerIns._scanAvgModeLsv, managerIns._averagesLsv);
		scan->Start(2);//���ʣ�����ģʽд2��grabģʽ��û�е���
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

#pragma region Thread ����ͨ�ź���
// ������Ϣ���� command ����component�����. ����component��ķֲ𣬵���component��update��Ҫ�ˣ�ֻ����device���update
void CommandThread()
{
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_REP);
	socket.bind(IPConfig::ipCmd);

	while (running) {
		zmq::message_t request;

		// ʹ�� try_recv �������߳���û����ϢʱҲ���˳�
		if (socket.recv(request, zmq::recv_flags::dontwait)) {

			Command::argNames.clear();
			Command::parameters.clear();

			Command cmd = Command::Deserialize(request.to_string());
			Command::UpdateArgNames();
			
			sendReply(socket, "From scan server: cmd.DeviceID = " + cmd.DeviceID + " cmd.CommandName = " + cmd.CommandName);
			Logger::GetInstance() << "From scan server: cmd.DeviceID = " + cmd.DeviceID + " cmd.CommandName = " + cmd.CommandName << std::endl;

			//������ִ��component��ķֲ����.ScanGenerator.cs 83�� ComponentComProc 
			Command* dvCmd = new Command();//���¸�device �·�ָ��
			if (cmd.CommandName == "SetParam")
			{
				std::string cmdName = cmd.argNames[0];//��ȡ set �Ĳ���
				dvCmd->DeviceID = "Scan";
				dvCmd->CommandName = cmdName;

				if (cmdName == pnImageSize || cmdName == pnScanAvgMode || cmdName == pnAverages || cmdName == pnDwell || cmdName == pnBrightness || cmdName == pnContrast )
				{
					dvCmd->SetCommandArg(cmdName, cmd.GetCommandArg(cmdName, 0));
				}
				else if (cmdName == pnDetector || cmdName == pnDetector1 || cmdName == pnDetector2 || cmdName == pnDetector3)
				{
					dvCmd->SetCommandArg(cmdName, cmd.GetCommandArg(cmdName, 0));
					//SetChamberLEDLevelWhenDetChanged(1, cmd.GetCommandArg(cmdName, 0)); //TODO ����ccd���
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
		//���Ӻͽ���
		Command cmd = commandQueue.pop();

		if (cmd.CommandName == pnStartContinuous) StartContinuous();
		else if (cmd.CommandName == pnStopNow) StopNow();
		else if (cmd.CommandName == pnReduceScan) ReduceScan();
		else if (cmd.CommandName == pnDetector)
		{
			auto targetDet = cmd.GetCommandArg(cmd.CommandName, 0);//�漰����ȡ��Ӧgetֵ�������һ��д��if�����
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

#pragma region ����ӳ��
		//// �������ӳ��
		//std::map<std::string, std::function<void()>> commandHandlers;
		//commandHandlers["StartContinuous"] = StartContinuous;
		//commandHandlers["StopNow"] = StopNow;  // ʾ���������������
		//commandHandlers["ReduceScan"] = ReduceScan;
		// 
		//// �������ִ����Ӧ�Ĵ�����
		//auto it = commandHandlers.find(cmd.CommandName);
		//if (it != commandHandlers.end()) {
		//	it->second();  // ���ö�Ӧ�Ĵ�����
		//}
		//else {
		//	DefaultHandler();  // Ĭ�ϴ���
		//}
#pragma endregion
	}
}

//// �޸Ĳ�������ʼ������²���
//void ParameterThread() {
//	zmq::context_t context(1);
//	zmq::socket_t socket(context, ZMQ_REP);
//	socket.bind(IPConfig::ipParams);
//
//	while (running) 
//	{
//		zmq::message_t request;
//
//		// ʹ�� try_recv �������߳���û����ϢʱҲ���˳�
//		if (socket.recv(request, zmq::recv_flags::dontwait)) {
//
//			std::string msg = request.to_string();
//
//			//std::cout << "From Params ======= Received: " << msg << std::endl;
//			Logger::GetInstance() << "From Params ======= Received: " << msg << std::endl;
//
//			if (msg.find("#Header:InitParameters") != std::string::npos)
//			{
//				//�ֽ�ͳ�ʼ��
//				Tools::DeserializeParameters(msg);
//				scan->InitScanParams();//���ʣ���ʼ������NormalParams�ṹ���С�����֮�����Ҫ�޸ĵ������ط�����ȫ��ʹ�� HAL update���ԣ�����һ����������
//				sendReply(socket, "Received:InitParams");
//			}
//			else if (msg.find("#Header:ReduceParamters") != std::string::npos)
//			{
//				//С��ɨ��
//				Tools::DeserializeParameters(msg);
//				scan->InitReduceScanParams();//���ʣ���ʼ������NormalParams�ṹ���С�����֮�����Ҫ�޸ĵ������ط�����ȫ��ʹ�� HAL update���ԣ�����һ����������
//				sendReply(socket, "Received:ReduceScan");
//			}
//			else
//			{
//				sendReply(socket, "Received: None");
//			}
//			//����˫ͨ����˫�Ŵ���ͨ������ͨ��reducescan......
//		}
//		else
//		{
//
//		}
//	}
//}
	
void ImageDataThread() {
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_PUSH);  // ZMQ_PUSH ���ڷ���ͼ������
	socket.bind(IPConfig::ipImage);      // ����һ���˿ڣ�����ͼ�����ݴ���

	while (running) {
		// �������Ƿ�Ϊ��
		if (!Tools::dataQueue.empty())
		{
			// �Ӷ�����ȡ������
			auto dataToSend = Tools::dataQueue.front();

			// ���� ZMQ ��Ϣ������
			zmq::message_t imgMessage(dataToSend.data(), dataToSend.size());
			socket.send(imgMessage, zmq::send_flags::none);

			Tools::dataQueue.pop(); // ������Ϻ����
		}
		else
		{
			// ����Ϊ��ʱ���ȴ����ݵ���
			//std::this_thread::sleep_for(std::chrono::milliseconds(10)); // �ɵ��ڵȴ�ʱ��
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

	hal.Update(pnImageSize, managerIns._imageSizeLsv);//�൱��update width & height
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

	//TODO �����¼��Ĺҽ���ô����
	//HAL.Instance.DoubleParamChanged += Instance_DoubleParamChanged;     
	//HAL.Instance.IntParamChanged += Instance_IntParamChanged;
}
#pragma endregion


// ������
int main()
{
	InitializeDevice();

	std::thread commandThread(CommandThread);//ͨ���̣߳�����ָ��. ���
	//std::thread paramThread(ParameterThread);//���ͺͽ��������߳�
	std::thread imageThread(ImageDataThread);//����ͼ��
	std::thread deviceThread(DeviceProcThread);//����

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

#pragma region MMF��������
	//HANDLE hMapFile_Reduce;
	//LPVOID pBuf_Reduce;
	//const wchar_t* mmfMapName_Reduce = L"MMFFILE_REDUCE";

	//ScanLib::InitMMFParams();
	//ScanLib::InitMMFReduceParams();

	//ScanLib::UninitMMFParams();
	//ScanLib::UninitMMFReduceParams();
#pragma endregion
