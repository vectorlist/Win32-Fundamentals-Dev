#include <windows.h>

#include <stdio.h>

#define THREADCOUNT 5
// Global variable
DWORD dwTlsIndex;
// Prototype
void ErrorExit(LPCSTR);

void CommonFunc(void)
{
	LPVOID lpvData;
	// Retrieve a data pointer for the current thread's TLS
	lpvData = TlsGetValue(dwTlsIndex);
	if ((lpvData == 0) && (GetLastError() != ERROR_SUCCESS))
		ErrorExit("TlsGetValue() error");
	else
		printf("TlsGetValue() is OK!\n");
	// Use the data stored for the current thread
	printf("common: Current thread Id is %d : Its data pointer, lpvData = %lx\n", GetCurrentThreadId(), lpvData);
	Sleep(15000);
}



DWORD WINAPI ThreadFunc(void)
{
	LPVOID lpvData;
	// Initialize the TLS index for this thread
	// Allocates the specified number of bytes from the heap
	lpvData = (LPVOID)LocalAlloc(LPTR, 256);
	if (lpvData == NULL)
		printf("LocalAlloc() failed, error %d\n", GetLastError());
	else
		printf("LHeap memory has been successfully allocated!\n");
	if (!TlsSetValue(dwTlsIndex, lpvData))
		ErrorExit("TlsSetValue() error");
	else
		printf("TlsSetValue() is OK lol!\n");
	printf("nThread Id %d: lpvData = %lx\n, GetCurrentThreadId()", lpvData);
	CommonFunc();
	// Release the dynamic memory before the thread returns
	lpvData = TlsGetValue(dwTlsIndex);
	if (lpvData != 0)
		LocalFree((HLOCAL)lpvData);

	else
		printf("lpvData already freed!\n");
	return 0;
}

int wmain(int argc, WCHAR **argv)
{
	DWORD IDThread;
	HANDLE hThread[THREADCOUNT];
	int i;
	DWORD Ret;
	// Allocate a TLS index
	if ((dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES)
		ErrorExit("TlsAlloc() failed");
	else
		printf("TlsAlloc() is OK!\n");

	// Create multiple threads
	for (i = 0; i < THREADCOUNT; i++)
	{
		hThread[i] = CreateThread(NULL, // default security attributes
			0,                           // use default stack size
			(LPTHREAD_START_ROUTINE)ThreadFunc, // thread function
			NULL,                    // no thread function argument
			0,                       // use default creation flags
			&IDThread);              // returns thread identifier
		// Check the return value for success.
		if (hThread[i] == NULL)
			ErrorExit("CreateThread() error\n");
		else
			printf("CreateThread() #%u is OK.Thread ID is %u\n", i, IDThread);
	}
	for (i = 0; i < THREADCOUNT; i++)

	{
		Ret = WaitForSingleObject(hThread[i], INFINITE);
		printf("The WaitForSingleObject() return value is 0X%.8x\n", Ret);
	}
	if (TlsFree(dwTlsIndex) != 0)
		printf("The TLS index, dwTlsIndex was released!\n");

	else
		printf("Failed to released TLS index, dwTlsIndex, error %d\n", GetLastError());
	return 0;

}



void ErrorExit(LPCSTR lpszMessage){
	fprintf_s(stderr, "%s\n", lpszMessage);
	ExitProcess(0);

}