#define no_init_all
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

#include <GLContextEx.h>
#include <iostream>
//PFNGLVIEWPORTPROC glViewport = nullptr;

int main(int args, char* argv[])
{
	int a = 0;
	InitOpenGL(&a);
	printf("%s, name : %s\n", __PRETTY_FUNCTION__, __FILENAME__);
	wglSwapIntervalEXT(0);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}