// UiLib_Demos.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MainWnd.h"


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	
	CPaintManagerUI::SetInstance(hInstance);

	GdiplusStartupInput   gdiplusStartupInput;
	ULONG_PTR             gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;
	LoadLibrary(_T("d3d9.dll"));
	CMainWnd* pFrame = new CMainWnd();
	if(pFrame == NULL)
		return 0;
	pFrame->Create(NULL,_T("UiLib Demos"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 900, 600);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);
	CPaintManagerUI::MessageLoop();
	::CoUninitialize();
	GdiplusShutdown(gdiplusToken);
	return 0;
}