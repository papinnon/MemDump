// MemDump.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <vector>
#include <stdio.h>

using namespace std;

bool traverseProcesses(std::map<std::string, int>& _nameID)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		std::cout << "CreateToolhelp32Snapshot Error!" << std::endl;;
		return false;
	}

	BOOL bResult = Process32First(hProcessSnap, &pe32);

	int num(0);

	while (bResult)
	{
		wstring ws = pe32.szExeFile;
		std::string name(ws.begin(), ws.end());
		//std::string name = pe32.szExeFile;

		int id = pe32.th32ProcessID;

		//std::cout << "[" << ++num << "] : " << "Process Name:"
		//<< name << "  " << "ProcessID:" << id << std::endl;

		_nameID.insert(std::pair<string, int>(name, id)); //×Öµä´æ´¢
		bResult = Process32Next(hProcessSnap, &pe32);
	}

	CloseHandle(hProcessSnap);

	return true;
}


void memdump(DWORD pid, LPCVOID src, LPVOID dest, SIZE_T n_to_read)
{
	HANDLE phdl;
	DWORD n_read;

	phdl = OpenProcess(
		PROCESS_ALL_ACCESS,
		FALSE,
		pid);
	if (phdl == NULL)
	{
		std::cout << "Failed Open Process\n";
		exit(1);
	}
	n_read = ReadProcessMemory(
		phdl,
		src,
		dest,
		n_to_read,
		nullptr);
	if (n_read == 0)
	{
		std::cout << "Failed Read Process";
		exit(1);
	}
	CloseHandle(phdl);
}
int _tmain(int argc, _TCHAR* argv[])
{
	std::map<std::string, int> _nameID;
	std::string target;
	DWORD targetpid;
	PBYTE start, end;
	if (!traverseProcesses(_nameID))
	{
		std::cout << "Error\n";
		exit(1);
	}
	std::cout << "Target name :\n";
	std::cin >> target;
	std::vector<std::pair<std::string, int>> list;
	for (std::map<std::string, int>::iterator it = _nameID.begin(); it != _nameID.end(); ++it)
	{
		size_t pos = it->first.find(target, 0);
		if (pos == std::string::npos)
		{
			continue;
		}
		list.push_back(*it);
	}
	for (unsigned int i = 0; i < list.size(); ++i)
	{
		std::cout << list[i].first << "  pid:" << list[i].second;
	}
	std::cout << "\npid,start,end([start,end))\n";
	scanf_s("%u,%p,%p", &targetpid, &start, &end);
	SIZE_T &&cnt = end - start;
	std::cout << cnt;
	int shit;
	std::cin >> shit;
	PBYTE buffer = new BYTE[cnt];
	memdump(targetpid, start, buffer, cnt);
	HANDLE hFile = CreateFile(L"./TMP_DUMPDATA",                // name of the write
		GENERIC_WRITE,          // open for writing
		0,                      // do not share
		NULL,                   // default security
		CREATE_NEW,             // create new file only
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template

	WriteFile(
		hFile,           // open file handle
		buffer,      // start of data to write
		cnt,  // number of bytes to write
		nullptr, // number of bytes that were written
		NULL);            // no overlapped structure
	CloseHandle(hFile);
	delete[] buffer;

	return 0;
}