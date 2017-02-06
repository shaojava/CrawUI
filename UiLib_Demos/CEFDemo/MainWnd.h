#ifndef MainWnd_h__
#define MainWnd_h__
#pragma once

#include "../../Common/Include/UiLib/Utils/IWindowBase.h"

class CMainWnd : public IWindowBase
{
public:
	CMainWnd(void);
	~CMainWnd(void);
public:
	CDuiString GetSkinFile();
	CDuiString GetSkinFolder();
	LPCTSTR GetWindowClassName() const ;
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Init();
	void Notify(TNotifyUI& msg);
	void OnFinalMessage(HWND hWnd);
protected:

};

#endif // MainWnd_h__
