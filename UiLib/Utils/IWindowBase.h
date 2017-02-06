#pragma once

#include <uxtheme.h>
#include "WndShadow.h"


using namespace UiLib;

	typedef struct _DWM_BLURBEHIND {
		DWORD dwFlags;
		BOOL  fEnable;
		HRGN  hRgnBlur;
		BOOL  fTransitionOnMaximized;
} DWM_BLURBEHIND, *PDWM_BLURBEHIND;
#define DWM_BB_ENABLE					0x00000001
#define DWM_BB_BLURREGION				0x00000002
#define DWM_BB_TRANSITIONONMAXIMIZED	0x00000004

enum UILIB_RESOURCETYPE
{
	UILIB_FILE=1,				// 来自磁盘文件
	UILIB_ZIP,						// 来自磁盘zip压缩包
	UILIB_RESOURCE,			// 来自资源
	UILIB_ZIPRESOURCE,	// 来自资源的zip压缩包
};

class UILIB_API IWindowBase 
	: public CWindowWnd
	, public CNotifyPump
	, public INotifyUI
	, public IMessageFilterUI
	, public IDialogBuilderCallback
{
public:
	IWindowBase(LPCTSTR _ZipSkin = NULL);
	virtual ~IWindowBase();

	virtual void OnFinalMessage(HWND hWnd);

	virtual void Init();

	virtual LPCTSTR GetWindowClassName() const;

	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual CControlUI* CreateControl(LPCTSTR pstrClass, CPaintManagerUI* pManager);

	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);   
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void Notify(TNotifyUI& msg);

public:
	CPaintManagerUI* GetPaintManager();
	virtual BOOL IsInStaticControl(CControlUI *pControl);
	void StartUnLayeredShadow();
protected:
	void ChooseLanuage();
	virtual CDuiString GetMLStringsPath();
	virtual CDuiString GetSkinFolder();
	virtual CDuiString GetSkinFile() = 0;
	virtual UINT GetClassStyle() const;
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual char *GetResourcePwd() const;
	virtual LPCTSTR GetResourceID() const;
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
	void StartGlassEffect(BOOL bOn = TRUE);
public:
	CButtonUI*		mpCloseBtn;
	CButtonUI*		mpMaxBtn;
	CButtonUI*		mpRestoreBtn;
	CButtonUI*		mpMinBtn;
protected:
	CWndShadow * m_pWndShadow;
	CDuiString		ZipSkinPath;
	CPaintManagerUI pm;
	LPBYTE m_lpResourceZIPBuffer;
	HMODULE m_hDwmAPI;
	HRESULT (WINAPI *DwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS *pMarInset);
	HRESULT (WINAPI *DwmIsCompositionEnabled)(BOOL *pfEnabled);
	HRESULT (WINAPI *DwmEnableBlurBehindWindow)(HWND hWnd, const DWM_BLURBEHIND *pBlurBehind);
};
