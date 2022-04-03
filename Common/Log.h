#pragma once
#define no_init_all
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <CommCtrl.h>

#define NUMCHAR(x) (sizeof(x)/ sizeof(x)[0])

namespace LOG
{
	LPCSTR ClassStyle(UINT stlye);
	LPCSTR WndMessage(UINT msg);
	void LogWndMessage(UINT msg, UINT filterMsg);
	void LogCtrlCustomDrawStage(LPARAM lp);
	void LogLastError();
};

