#pragma once
#include "../../Common/Include/UiLib/UIlib.h"
#include "../MultiProcessChrome/MutualDef.h"

class CMltPrsChromUI : public CControlUI
{
public:
	CMltPrsChromUI(void);
	virtual ~CMltPrsChromUI(void);

	BOOL Attach(HWND hWndNew);
	HWND Detach();
	HWND GetHWND();
	void SetUIThreadID(DWORD dwUIThreadID){m_dwUIThreadID = dwUIThreadID;}
	DWORD GetUIThreadID(){return m_dwUIThreadID;}
	DWORD GetProcessID(){return m_dwProcessID;}
	void SetHomePage(LPCTSTR lpszUrl);

	virtual void SetPos(RECT rc);
	virtual void SetInternVisible(bool bVisible = true);
	virtual void SetVisible(bool bVisible = true);

protected:
	void CreateChrome(RECT rcPos);
protected:
	HWND m_hWndAttached;
	DWORD m_dwProcessID;
	DWORD m_dwUIThreadID;
};

