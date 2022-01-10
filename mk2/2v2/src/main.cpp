#include <iostream>
#include <windows.h>
#include <string>
#include <random>
#include <stdio.h>
#include <tchar.h>

#define SIZE 5
#define BUFFER_SIZE 4
#define BUF_SIZE 2048
#define MAPNAME "Global\\arosintoarsntoarsetnras"

HANDLE mutex, empty, full;
HANDLE hMapFile;
HANDLE hFile;
TCHAR myVariable[260] = TEXT("fmtest.txt");

void createFileMapping(char* fileMappingName) {
	hMapFile = CreateFileMappingA(
		hFile,
		NULL,                   
		PAGE_READWRITE,         
		0,             
		BUF_SIZE,				
		fileMappingName
	);
}

int random_(int lo, int hi) {
	std::random_device rd;
	std::default_random_engine g(rd());
	std::uniform_int_distribution<> d(lo, hi);
	return d(g);
}

std::string generateData() {
	std::string data;
	for (size_t i = 0; i < 5; i++) {
		int generatedNumber = random_(100, 200);
		data += std::to_string(generatedNumber) + " ";
	} 
	return data;
}

void putData(std::string data) {	
	auto pBuf = MapViewOfFile(
		hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		BUF_SIZE
	);
	int size = data.size();
	CopyMemory(pBuf, data.c_str(), size);
}

std::string consume() {
	auto pBuf = MapViewOfFile(
		hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		BUF_SIZE
	);
	std::string data = reinterpret_cast<LPTSTR>(pBuf);
	CopyMemory((PVOID)pBuf, 0, 0);
	return data;
}

DWORD __stdcall producer() {
	while (true) {
		WaitForSingleObject(empty, INFINITE);
		WaitForSingleObject(mutex, INFINITE);
            auto data = generateData();
            std::cout << "produced data: " << data << std::endl;
            putData(data);
		ReleaseSemaphore(mutex, 1, NULL);
		ReleaseSemaphore(full, 1, NULL);
	}
}

DWORD __stdcall consumer() {
	while (true) {
		WaitForSingleObject(full, INFINITE);
		WaitForSingleObject(mutex, INFINITE);
    		std::string data = consume();
    		std::cout << "consumed: " << data << std::endl;
		ReleaseSemaphore(mutex, 1, NULL);
		ReleaseSemaphore(empty, 1, NULL);
	}
}

void initSemaphores(std::string mutexName, std::string emptyName, std::string fullName) {
	mutex = CreateSemaphore(NULL, 1, 1, mutexName.c_str());
	empty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, emptyName.c_str());
	full = CreateSemaphore(NULL, 0, BUFFER_SIZE, fullName.c_str());
}

int main(int argc, char** argv) {
	char fileMappingName[] = "Global\\arosintoarsntoarsetnras";
	if (argc <= 1) {
		hFile = CreateFileA(
			myVariable,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);

		initSemaphores("mutex", "empty", "full");
		createFileMapping(fileMappingName);

		STARTUPINFO info;
		PROCESS_INFORMATION pi;
		ZeroMemory(&info, sizeof(info));
		info.cb = sizeof(info);
		ZeroMemory(&pi, sizeof(pi));

		std::string programPath = argv[0];
		std::string argumentsConsumer = std::string(programPath + " " + fileMappingName + " " + "consumer" + " mutex empty full");
		std::string argumentsProducer = std::string(programPath + " " + fileMappingName + " " + "producer" + " mutex empty full");

		CreateProcess(NULL, strdup(argumentsConsumer.c_str()), NULL, NULL,
			false,
			CREATE_NEW_CONSOLE,
			NULL, NULL, &info, &pi);

		CreateProcess(NULL, strdup(argumentsProducer.c_str()), NULL, NULL,
			false,
			CREATE_NEW_CONSOLE,
			NULL, NULL, &info, &pi);
	}
	else {
		auto fileMappingName = argv[1];
		std::string processType     = argv[2];
		std::string mutexName       = argv[3];
		std::string emptyName       = argv[4];
		std::string fullName        = argv[5];

		initSemaphores(mutexName, emptyName, fullName);
		createFileMapping(fileMappingName);

		if (processType == "consumer") {
			producer();
		}
		else if (processType == "producer") {
			consumer();
		}
	}
	getchar();
}