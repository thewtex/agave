#include "HardwareWidget.h"
//#include "RenderThread.h"
#include "glad/include/glad/glad.h"
#include "CudaUtilities.h"
#include "Logging.h"

#include <boost/lexical_cast.hpp>

#include <iomanip>

std::string toStr(float f, int dec) {
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(dec);
	ss << f;
	std::string s = ss.str();
	return s;
}

void DeviceSelector::EnumerateDevices(void)
{
	int NoDevices = 0;

	HandleCudaError(cudaGetDeviceCount(&NoDevices), "no. Cuda capable devices");

	for (int DeviceID = 0; DeviceID < NoDevices; DeviceID++)
	{
		aicsCudaDevice CudaDevice;

		cudaDeviceProp DeviceProperties;

		HandleCudaError(cudaGetDeviceProperties(&DeviceProperties, DeviceID));

		CudaDevice.m_ID					= DeviceID;
		CudaDevice.m_Name				= std::string(DeviceProperties.name);
		CudaDevice.m_Capability			= boost::lexical_cast<std::string>(DeviceProperties.major) + "." + boost::lexical_cast<std::string>(DeviceProperties.minor);
		CudaDevice.m_GlobalMemory		= boost::lexical_cast<std::string>((float)DeviceProperties.totalGlobalMem / powf(1024.0f, 2.0f)) + "MB";
		CudaDevice.m_NoMultiProcessors	= boost::lexical_cast<std::string>(DeviceProperties.multiProcessorCount);
		CudaDevice.m_GpuClockSpeed		= toStr(DeviceProperties.clockRate * 1e-6f, 2) + "GHz";
		CudaDevice.m_MemoryClockRate    = toStr(DeviceProperties.memoryClockRate * 1e-6f, 2) + "GHz";
		CudaDevice.m_RegistersPerBlock  = boost::lexical_cast<std::string>(DeviceProperties.regsPerBlock);

		AddDevice(CudaDevice);
	}
}

void DeviceSelector::AddDevice(const aicsCudaDevice& Device)
{
	listOfPairs.push_back(Device);
}

bool DeviceSelector::GetDevice(const int& DeviceID, aicsCudaDevice& CudaDevice)
{
	for (int i = 0; i < listOfPairs.size(); i++)
	{
		if (listOfPairs[i].m_ID == DeviceID)
		{
			CudaDevice = listOfPairs[i];
			return true;
		}
	}

	return false;
}

DeviceSelector::DeviceSelector()
{
	int DriverVersion = 0, RuntimeVersion = 0; 

	HandleCudaError(cudaDriverGetVersion(&DriverVersion));
	HandleCudaError(cudaRuntimeGetVersion(&RuntimeVersion));

	std::string DriverVersionString		= boost::lexical_cast<std::string>(DriverVersion / 1000) + "." + boost::lexical_cast<std::string>(DriverVersion % 100);
	std::string RuntimeVersionString	= boost::lexical_cast<std::string>(RuntimeVersion / 1000) + "." + boost::lexical_cast<std::string>(RuntimeVersion % 100);

	LOG_INFO << "CUDA Driver Version: " << DriverVersionString;
	LOG_INFO << "CUDA Runtime Version: " << RuntimeVersionString;

	//std::string VersionInfo;

	//VersionInfo += "CUDA Driver Version: " + DriverVersionString;
	//VersionInfo += ", CUDA Runtime Version: " + RuntimeVersionString;

	EnumerateDevices();

	OnOptimalDevice();
}

void DeviceSelector::OnOptimalDevice(void)
{
	const int MaxGigaFlopsDeviceID = GetMaxGigaFlopsDeviceID();

	_selectedDevice = MaxGigaFlopsDeviceID;
	OnCudaDeviceChanged();
}

void DeviceSelector::OnCudaDeviceChanged(void)
{
	aicsCudaDevice CudaDevice;

	if (GetDevice(_selectedDevice, CudaDevice))
	{
		if (SetCudaDevice(CudaDevice.m_ID))
		{
			_selectedDeviceID = CudaDevice.m_ID;
			LOG_INFO << "Cuda device changed to " << CudaDevice.m_Name;
		}
	}
}