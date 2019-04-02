#include "Mars_Process_Controller.h"

Mars_Process_Controller::Mars_Process_Controller()
{
	_ProcessHandle = NULL;
}

void Mars_Process_Controller::SetDirectory(WCHAR *Directory)
{
	if (Directory == NULL)
		return;
}

bool Mars_Process_Controller::StartController(WCHAR *ProcessName)
{
	PdhOpenQuery(NULL, NULL, &_CpuQuery);
	PdhAddCounter(_CpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &_CpuTotal);
	PdhCollectQueryData(_CpuQuery);
	if (!wcscpy_s(_pFileName, 100, ProcessName))
		return true;
	return false;
}

bool Mars_Process_Controller::ResetProcess(WCHAR *ProcessName)
{
	if (!wcscpy_s(_pFileName, 100, ProcessName))
		return true;
	return false;
}

bool Mars_Process_Controller::StartProcess()
{
	STARTUPINFO StartInfo;
	PROCESS_INFORMATION ProcessInfo;
	memset(&StartInfo, 0, sizeof(StartInfo));
	StartInfo.cb = sizeof(StartInfo);
	if (!CreateProcessW(NULL, _pFileName, NULL, NULL, TRUE, 0, NULL, NULL, &StartInfo, &ProcessInfo))
	{
		printf("Process Start fail!!!\n\n");
		return false;
	}
	_ProcessID = ProcessInfo.dwProcessId;
	_ProcessHandle = ProcessInfo.hProcess;
	return true;
}

bool Mars_Process_Controller::GetCpuStatus(double *CpuUsage)
{
	PDH_FMT_COUNTERVALUE CounterVal;
	if (PdhCollectQueryData(_CpuQuery) != ERROR_SUCCESS)
		return false;
	if (PdhGetFormattedCounterValue(_CpuTotal, PDH_FMT_DOUBLE, NULL, &CounterVal) != ERROR_SUCCESS)
		return false;
	*CpuUsage = CounterVal.doubleValue;
	return true;
}

bool Mars_Process_Controller::GetMemoryStatus(__int64 *Virtual, __int64 *Nonpaged)
{
	PROCESS_MEMORY_COUNTERS_EX MemoryInfo;
	if (!GetProcessMemoryInfo(_ProcessHandle, (PROCESS_MEMORY_COUNTERS*)&MemoryInfo, sizeof(MemoryInfo)))
		return false;
	*Virtual = MemoryInfo.PrivateUsage;
	*Nonpaged = MemoryInfo.QuotaNonPagedPoolUsage;
	return true;
}

bool Mars_Process_Controller::GetProcessIsLive()
{
	DWORD Stat;
	GetExitCodeProcess(_ProcessHandle, &Stat);
	if (Stat == STILL_ACTIVE)
		return true;
	return false;
}

bool Mars_Process_Controller::ProcessForcedKill()
{
	if (!TerminateProcess(_ProcessHandle, 10))
		return false;
	CloseHandle(_ProcessHandle);
	return true;
}

bool Mars_Process_Controller::ProcessRestart()
{
	STARTUPINFO StartInfo;
	PROCESS_INFORMATION ProcessInfo;
	if (!TerminateProcess(_ProcessHandle, 10))
		return false;
	CloseHandle(_ProcessHandle);
	memset(&StartInfo, 0, sizeof(StartInfo));
	StartInfo.cb = sizeof(StartInfo);
	if (!CreateProcessW(NULL, _pFileName, NULL, NULL, TRUE, 0, NULL, NULL, &StartInfo, &ProcessInfo))
	{
		printf("Process Start fail!!!\n\n");
		return false;
	}
	_ProcessID = ProcessInfo.dwProcessId;
	_ProcessHandle = ProcessInfo.hProcess;
	return true;
}

bool Mars_Process_Controller::CheckProcessVersion(WCHAR *Comp)
{
	WCHAR Temp[100];
	for (int i = 0; i < 100; i++)
	{
		if (_pFileName[i] == L'@')
		{
			Temp[i] = L'\0';
			break;
		}
		Temp[i] = _pFileName[i];
	}
	if (!lstrcmp(Temp, Comp))
		return true;
	return false;
}



int main()
{
	Mars_Process_Controller Pro;
	double Cpu;
	__int64 Memory;
	__int64 Nonpage;
	Pro.StartController(L"notepad");
	Pro.StartProcess();
	while (1)
	{
		Pro.GetCpuStatus(&Cpu);
		Pro.GetMemoryStatus(&Memory, &Nonpage);
		if(Pro.GetProcessIsLive())
			printf("CPU : %.02f %% MEMORY : %lld KB NON : %lld KB\n", Cpu, Memory / 1024, Nonpage / 1024);
		Sleep(1000);
	}
}