#include "CommonTools.h"

std::string IPConfig::ipCmd = "tcp://localhost:9827";
//std::string IPConfig::ipParams = "tcp://localhost:9828";
std::string IPConfig::ipImage = "tcp://localhost:9829";

//std::map<std::string, std::string> Tools::parameterMap;
std::queue<std::vector<byte>> Tools::dataQueue;

std::unordered_map<std::string, ParamValue> Command::parameters;
std::vector<std::string> Command::argNames;

////�����л������������ǲ���
//void Tools::DeserializeParameters(const std::string& data)
//{
//	std::istringstream stream(data);
//	std::string token;
//
//	while (std::getline(stream, token, '#')) {
//		if (token.empty()) continue; // Skip empty tokens
//
//		size_t pos = token.find(':');
//		if (pos != std::string::npos) {
//			std::string key = token.substr(0, pos);
//			std::string value = token.substr(pos + 1);
//
//			//parameterMap.insert({ key, value });
//			parameterMap[key] = value;
//		}
//	}
//}

//ͼ����ӡ�ͼ�����ʱ������scanmode��pixeloffset, pixelCount
void Tools::EnqueueData(const byte* buf, int scanMode, int pixelOffset, int pixelCount)
{
	std::vector<byte> data(sizeof(int)*3 + pixelCount);

	// ����ƫ��������������
	uint32_t* pScanMode = reinterpret_cast<uint32_t*>(&data[0]);
	uint32_t* pOffset = reinterpret_cast<uint32_t*>(&data[4]);
	uint32_t* pCount = reinterpret_cast<uint32_t*>(&data[8]);

	*pScanMode = static_cast<uint32_t>(scanMode);
	*pOffset = static_cast<uint32_t>(pixelOffset);
	*pCount = static_cast<uint32_t>(pixelCount);

	// ����ͼ�����ݵ�������
	memcpy(&data[12], buf + pixelOffset, pixelCount); // �ӵ� 8 �ֽڿ�ʼ����ͼ������

	// ���������������͵�����
	Tools::dataQueue.push(data);

	//std::vector<byte> data(buf + pixelOffset, buf + pixelOffset + pixelCount);
	//Tools::dataQueue.push(data);
}

//������ж����е�ͼ������
void Tools::ClearDataQueue()
{
	while (!Tools::dataQueue.empty()) {
		Tools::dataQueue.pop();
	}
}

ushort Utils::dacOffset = 6560;		//prescan 6560�� DAC step
ushort Utils::lineStartDelay = 20;		//��ɨ���ӳ� 20us

//ReduceScan����prescan Ԥɨ��������г���һ����
void Utils::CalcPreScanForReduceMode(double DwellTime,
	ushort ImageWidth,
	ushort ImageHeight,
	ushort StartX,
	ushort StartY,
	ushort& ReduceDacOffsetX,
	ushort& ReduceDacOffsetY,
	ushort& DacStepX,
	ushort& DacStepY,
	ushort& PreScanX,
	ushort& PreScanY)
{
	double lineflyBackStepCount = (double)ImageWidth;
	double lineFlyBackStepTime = 100; // 10ns

	//double DacScanRange = 65536 - Utils::dacOffset * 2;
	double DacScanRange = 52416.0;
	ReduceDacOffsetX = (ushort)(((double)StartX / (double)ImageWidth) * DacScanRange + dacOffset);
	ReduceDacOffsetY = (ushort)(((double)StartY / (double)ImageHeight) * DacScanRange + dacOffset);

	CalcPreScanArgs((double)DwellTime, ImageWidth, ImageHeight, DacStepX, DacStepY, PreScanX, PreScanY);
}

/// <summary>
/// Caclulate the Pre Scan Args : DacStepX, DacStepY, PreScanStepCountX, PreScanStepY
/// </summary>
/// <param name="DwellTime">Dwell Time in NS </param>
/// <param name="ImageWidth">Image Pixel Size</param>
/// <param name="DacStepX">Output : Dac Steps For One Pixel For X</param>
/// <param name="DacStepY">Output : Dac Steps For One Pixel For Y</param>
/// <param name="PreScanStepCountX"> Output : PreScan Pixels For X</param>
/// <param name="PreScanStepCountY">Output : PreScan Pixels For Y</param>
/// <param name="DacOffset"> Default Value = 6560</param>
/// <param name="LineStartDelay">Default Value = 20us</param>
//���� PreScan �������г���һ���� 
void Utils::CalcPreScanArgs(double DwellTime,
	ushort ImageWidth,
	ushort ImageHeight,
	ushort& DacStepX,
	ushort& DacStepY,
	ushort& PreScanStepCountX,
	ushort& PreScanStepCountY,
	ushort DacOffset,
	ushort LineStartDelay)
{
	ushort lineflyBackStepCount = ImageWidth;
	ushort lineFlyBackStepTime = 100; // 10ns ,��Ҫת����us

	double w = ImageWidth;
	double h = ImageHeight;

	ushort offsetX = DacOffset;
	ushort offsetY = DacOffset;

	ushort DacRangeX = (ushort)(65536 - offsetX * 2);  //52416 steps for image scan , 4096 for prescan
	ushort DacRangeY = (ushort)(65536 - offsetY * 2);

	DacStepX = (ushort)(DacRangeX / w);
	DacStepY = (ushort)(DacRangeY / h);

	ushort maxPreScanPixelsX = (ushort)(offsetX / DacStepX);   //���prescan������
	ushort maxPreScanPixelsY = (ushort)(offsetY / DacStepY);   //���prescan������


	//���� Prescan X
	double preScanFormulaX = DacStepX / (DwellTime / 1000);
	if (preScanFormulaX >= 1)
		PreScanStepCountX = maxPreScanPixelsX;
	else
		PreScanStepCountX = (ushort)(maxPreScanPixelsX * preScanFormulaX);


	//����Prescan Y
	double lineTime = LineStartDelay + DwellTime / 1000 * PreScanStepCountX + DwellTime * (w + PreScanStepCountX) / 1000 + lineFlyBackStepTime * lineflyBackStepCount / 1000;

	double preScanFormulaY = DacStepY / lineTime;
	if (preScanFormulaY >= 1)
		PreScanStepCountY = maxPreScanPixelsY;
	else
		PreScanStepCountY = (ushort)(maxPreScanPixelsY * preScanFormulaY);
}