#include "GLContextEx.h"
//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
#include <iostream>
bool InitOpenGL(void* hwnd) {
	{
		HWND dummy = CreateWindow("static", NULL, NULL, 0, 0, 0, 0, NULL, 0, 0, 0);
		HDC dc = GetDC(dummy);

		PIXELFORMATDESCRIPTOR pfd{ 0 };
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 24;

		int pixelFormat = ChoosePixelFormat(dc, &pfd);
		int res = DescribePixelFormat(dc, pixelFormat, sizeof(pfd), &pfd);

		SetPixelFormat(dc, pixelFormat, &pfd);

		HGLRC rc = wglCreateContext(dc);
		wglMakeCurrent(dc, rc);

		//PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB =
		//	(PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");

		//const char* list = wglGetExtensionsStringARB(dc);
		auto list = wglGetExtensionsStringARB(dc);

		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
		PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
		LPSTR tok = (LPSTR)list;
		while ((tok = strtok(tok, " ")) != NULL) {
			if (strcmp(tok, "WGL_ARB_pixel_format") == 0)
				wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress(
					"wglChoosePixelFormatARB");
			if (strcmp(tok, "WGL_ARB_create_context") == 0)
				wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress(
					"wglCreateContextAttribsARB");
			printf("%s\n", tok);
			tok = NULL;
		}

	}
	//ASSERT(0 && "failes");
	ASSERT_LOG(0, "failed");
	return false;
}