#ifndef MainWnd_h__
#define MainWnd_h__
#pragma once

#include "../../Common/Include/UiLib/Utils/IWindowBase.h"

typedef bool (*_ChangeWindowMessageFilter)(unsigned int,bool);

class CMainWnd : public IWindowBase
{
public:
	CMainWnd(void);
	~CMainWnd(void);
public:
	CDuiString GetSkinFile();
	CDuiString GetSkinFolder();
	UILIB_RESOURCETYPE GetResourceType() const;
	LPCTSTR GetResourceID() const;
	char * GetResourcePwd() const;
	LPCTSTR GetWindowClassName() const ;
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void Init();
	void Notify(TNotifyUI& msg);
	void OnFinalMessage(HWND hWnd);
protected:
	void OnPrepare();
	bool OnBtnEnCrypt(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnBtnDeCrypt(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
protected:
	BOOL AllowMeesageForVistaAbove(UINT uMessageID, BOOL bAllow);
	void OnDropFiles(HDROP hDropInfo);
private:
	CLabelUI *m_pLblFileName;
	CEditUI *m_pEdtPwd;
	CButtonUI *m_pBtnEnCrypt;
	CButtonUI *m_pBtnDeCrypt;
private:
	CString m_szFilePath;
};

#endif // MainWnd_h__
