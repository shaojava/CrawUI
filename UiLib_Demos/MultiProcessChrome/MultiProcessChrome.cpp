// UiLib_Demos.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MainWnd.h"


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	CChromeUI::InitWebkit(true);
	//EnableMemLeakCheck();
	CPaintManagerUI::SetInstance(hInstance);

	GdiplusStartupInput   gdiplusStartupInput;
	ULONG_PTR             gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;


#ifdef UILIB_D3D
	if( ::LoadLibrary(_T("d3d9.dll")) == NULL ) 
		::MessageBox(NULL, _T("加载 d3d9.dll 失败，一些特效可能无法显示！"), _T("信息提示"),MB_OK|MB_ICONWARNING);
#endif

	LPWSTR *szArglist = NULL;
	int nArgs = 0;
	int i = 0;
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	HWND hWndParent = NULL;
	if(szArglist)
		hWndParent = (HWND)_tcstoul(CString(szArglist[1]), NULL, 16);
	CString szRect(szArglist[0]);
	int rc[4] = {0};
	for(int i = 0 ; i < 4 ; i++)
	{
		int nPos = szRect.Find(',');
		CString szTmp = _T("");
		if(nPos != -1)
		{
			szTmp = szRect.Left(nPos);
			szRect = szRect.Right(szRect.GetLength()-nPos-1);
		}
		if(i == 3)
			szTmp = szRect;
		rc[i] = _wtoi(szTmp);
	}

	CMainWnd* pFrame = new CMainWnd();
	if(pFrame == NULL)
		return 0;
	pFrame->Create(NULL,_T("UiLib Demos"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 0, 0);
	if(hWndParent)
	{
		SetParent(pFrame->GetHWND(),hWndParent);
		pFrame->SetParent(hWndParent);
		MoveWindow(pFrame->GetHWND(),rc[0],rc[1],rc[2],rc[3],FALSE);
	}

	::ShowWindow(*pFrame, SW_SHOW);
	CPaintManagerUI::MessageLoop();
	::CoUninitialize();
	GdiplusShutdown(gdiplusToken);
	CChromeUI::ShutdownWebkit();
	//_CrtDumpMemoryLeaks();
	return 0;
}