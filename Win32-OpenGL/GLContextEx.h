#pragma once

#pragma comment(lib, "opengl32")
#define no_init_all
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#if defined(_WIN32) && !defined(APIENTRY)
#define APIENTRY __stdcall            
#endif
#if defined(_WIN32) && !defined(WINGDIAPI)
#define WINGDIAPI __declspec(dllimport)   
#endif

#define ASSERT(x) do { if(!(x)) DebugBreak();} while(0)

#include <GL\GL.h>
#include <GL\glcorearb.h>
#define WGL_WGLEXT_PROTOTYPES
#include <GL\glext.h>	
#include <GL\wglext.h>

//the include source must be at same level
//Include Path
//..	GL
//..	KHR

//struct HWND__;
//typedef HWND *HWND__;

#define __FILENAME__ (strrchr(__FILE__,'\\')+1)

#undef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCSIG__

#define ASSERT_LOG(x, xx) \
do { \
	if(!x) {\
	char buf[64]; \
	sprintf(buf, "file : %s\nline : %d\n%s", __FILENAME__, __LINE__, xx); \
	MessageBox(NULL, buf, 0, MB_OK); \
	}	\
}while(0)

bool InitOpenGL(void* hwnd);