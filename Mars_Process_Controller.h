#pragma once
#include <Windows.h>
#include <iostream>
#include <Psapi.h>
#include <Pdh.h>

#pragma comment(lib,"Pdh.lib")
class Mars_Process_Controller
{
public:
	//init
	Mars_Process_Controller();
	bool StartController(WCHAR *ProcessName);
	void SetDirectory(WCHAR *Directory);
	//Control process
	bool ResetProcess(WCHAR *ProcessName);
	bool StartProcess();
	bool ProcessForcedKill();
	bool ProcessRestart();
	//Process watcher
	bool GetCpuStatus(double *CpuUsage);
	bool GetMemoryStatus(__int64 *Virtual, __int64 *Nonpaged);
	bool GetProcessIsLive();
	//Check Process version
	bool CheckProcessVersion(WCHAR *Comp);
	bool ProcessPatch();

private:
	WCHAR _pFileName[100];
	WCHAR *_pDirectoryName;
	HANDLE _ProcessHandle;
	DWORD _ProcessID;
	PDH_HQUERY _CpuQuery;
	PDH_HCOUNTER _CpuTotal; 
};