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

	bool OnStartTrayAminCtrlClick(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnMsgBtnMouseEnter(TEventUI* pTEventUI,LPARAM lParam,WPARAM wParam);
	bool OnMsgBtnMouseLeave(TEventUI* pTEventUI,LPARAM lParam,WPARAM wParam);
	bool OnMsgBtnClick(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnEffectsBtnClick(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnAddNodeBtnClick(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnAddAtNodeBtnClick(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnRemoveNodeBtnClick(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnCreateChartViewBtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnAddChartDataABtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnAddChartDataBBtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnAddChartDataCBtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnLegendTopBtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnLegendRightBtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnLegendBottomBtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnDelChartDataBtn(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);

	void OnDuiTimerA(IDuiTimer* pTimer);
	void OnDuiTimerB(IDuiTimer* pTimer,CButtonUI* pControl);
	void OnDuiTimerC(IDuiTimer* pTimer,HWND hWnd,CMainWnd* lParam,WPARAM wParam);
	bool OnDuiTimerD(TEventUI* pTEventUI);
private:
	int				mChartDataPos;
	CLabelUI*		pTestLabel;
	CEditUI*		pTestEdit;
	CButtonUI*		pEffectsDemo;
	CRichEditWnd *  m_pRichEditWnd;
	CChartViewUI*	pChartView;
	CHorizontalLayoutUI* pAnimWnd;
	HMENU			hMenu;
};

#endif // MainWnd_h__
