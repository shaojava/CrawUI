#ifndef MainWnd_h__
#define MainWnd_h__
#pragma once

#include "../../Common/Include/UiLib/Utils/IWindowBase.h"
#include "UIAnimateTab.h"
#include "UIMltPrsChrom.h"
#include "AboutWnd.h"
#include "MachineView.h"
#include "UIFrameImage.h"

class CWallPaperUI : public CContainerUI
{
public:
	CWallPaperUI(CPaintManagerUI *pManager){m_pManager = pManager;}
	~CWallPaperUI(){}
public:
	LPCTSTR GetClass() const {return _T("test");}
	LPVOID GetInterface(LPCTSTR pstrName) {return _T("WallPaper");};
};

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
	CControlUI * CreateControl(LPCTSTR pstrClass,CPaintManagerUI *pManager);
private:
	CButtonUI *m_pToolTipTest;
	CButtonUI *m_pReload;
	CToolTipsWnd* pToolTiWnd;

	bool m_bPause;

	CButtonUI *m_pBtn1;
	CButtonUI *m_pBtn2;
	CButtonUI *m_pBtn3;
	CButtonUI *m_pBtn4;
	CButtonUI *m_pBtn5;
	CButtonUI *m_pBtnNewChrome;
	CMaskControlUI *m_pMask;
	CAnimateTabUI *m_pAnimate;

	CMltPrsChromUI *m_pChrome;

	CHorizontalLayoutUI *m_pItme1;
	CHorizontalLayoutUI *m_pItme2;
	CHorizontalLayoutUI *m_pItme3;
	CHorizontalLayoutUI *m_pItme4;
	CHorizontalLayoutUI *m_pItme5;
	CHorizontalLayoutUI *m_pItme6;

	CListUI* m_pList;

	CMachineTree *m_pMachineView;

	CEditUI *m_pEdit;

	CComboUI *m_pCombo;
};

#endif // MainWnd_h__
