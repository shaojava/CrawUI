#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__

#pragma once
#include "UIRenderCore.h"
#include "../Utils/WaitableHandleMgr.h"
#include "../Animation/UIAnimateManager.h"

namespace UiLib {
/////////////////////////////////////////////////////////////////////////////////////
//
#define WM_EFFECTS		WM_USER+1680
#define WM_RELOADSTYLE	WM_USER+1681
#define WM_MOUSEINTRAYICON	WM_USER+1682
#define WM_MOUSEOUTTRAYICON	WM_USER+1683
class CControlUI;

/////////////////////////////////////////////////////////////////////////////////////
//
static UINT MapKeyState()
{
	UINT uState = 0;
	if( ::GetKeyState(VK_CONTROL) < 0 ) uState |= MK_CONTROL;
	if( ::GetKeyState(VK_RBUTTON) < 0 ) uState |= MK_LBUTTON;
	if( ::GetKeyState(VK_LBUTTON) < 0 ) uState |= MK_RBUTTON;
	if( ::GetKeyState(VK_SHIFT) < 0 ) uState |= MK_SHIFT;
	if( ::GetKeyState(VK_MENU) < 0 ) uState |= MK_ALT;
	return uState;
}

typedef enum EVENTTYPE_UI
{
	UIEVENT__ALL,
    UIEVENT__FIRST,
    UIEVENT__KEYBEGIN,
    UIEVENT_KEYDOWN,
    UIEVENT_KEYUP,
    UIEVENT_CHAR,
    UIEVENT_SYSKEY,
    UIEVENT__KEYEND,
    UIEVENT__MOUSEBEGIN,
    UIEVENT_MOUSEMOVE,
    UIEVENT_MOUSELEAVE,
    UIEVENT_MOUSEENTER,
    UIEVENT_MOUSEHOVER,
    UIEVENT_BUTTONDOWN,
    UIEVENT_BUTTONUP,
    UIEVENT_RBUTTONDOWN,
	UIEVENT_RBUTTONUP,
    UIEVENT_DBLCLICK,
    UIEVENT_CONTEXTMENU,
    UIEVENT_SCROLLWHEEL,
    UIEVENT__MOUSEEND,
    UIEVENT_KILLFOCUS,
    UIEVENT_SETFOCUS,
	UIEVENT_WMMOVE,
    UIEVENT_WINDOWSIZE,
    UIEVENT_SETCURSOR,
    UIEVENT_TIMER,
    UIEVENT_NOTIFY,
	UIEVENT_COMMAND,
	UIEVENT_RELOADSTYLE,
	UIEVENT_TRAYICON,
	UIEVENT_MOUSEINTRAYICON,
	UIEVENT_MOUSEOUTTRAYICON,
	UIEVENT_TEXTCHANGE,
	UIEVENT__LAST,
};

typedef enum
{
	UIFONT__FIRST = 0,
	UIFONT_NORMAL,
	UIFONT_BOLD,
	UIFONT_CAPTION,
	UIFONT_MENU,
	UIFONT_LINK,
	UIFONT_TITLE,
	UIFONT_HEADLINE,
	UIFONT_SUBSCRIPT,
	UIFONT__LAST,
} UITYPE_FONT;

typedef enum
{
	UICOLOR__FIRST = 0,
	UICOLOR_WINDOW_BACKGROUND,
	UICOLOR_WINDOW_TEXT,
	UICOLOR_DIALOG_BACKGROUND,
	UICOLOR_DIALOG_TEXT_NORMAL,
	UICOLOR_DIALOG_TEXT_DARK,
	UICOLOR_MENU_BACKGROUND,
	UICOLOR_MENU_TEXT_NORMAL,
	UICOLOR_MENU_TEXT_HOVER,
	UICOLOR_MENU_TEXT_SELECTED,
	UICOLOR_TEXTCOLOR_NORMAL,
	UICOLOR_TEXTCOLOR_HOVER,
	UICOLOR_TAB_BACKGROUND_NORMAL,
	UICOLOR_TAB_BACKGROUND_SELECTED,
	UICOLOR_TAB_FOLDER_NORMAL,
	UICOLOR_TAB_FOLDER_SELECTED,
	UICOLOR_TAB_BORDER,
	UICOLOR_TAB_TEXT_NORMAL,
	UICOLOR_TAB_TEXT_SELECTED,
	UICOLOR_TAB_TEXT_DISABLED,
	UICOLOR_NAVIGATOR_BACKGROUND,
	UICOLOR_NAVIGATOR_BUTTON_HOVER,
	UICOLOR_NAVIGATOR_BUTTON_PUSHED,
	UICOLOR_NAVIGATOR_BUTTON_SELECTED,
	UICOLOR_NAVIGATOR_BORDER_NORMAL,
	UICOLOR_NAVIGATOR_BORDER_SELECTED,
	UICOLOR_NAVIGATOR_TEXT_NORMAL,
	UICOLOR_NAVIGATOR_TEXT_SELECTED,
	UICOLOR_NAVIGATOR_TEXT_PUSHED,
	UICOLOR_BUTTON_BACKGROUND_NORMAL,
	UICOLOR_BUTTON_BACKGROUND_DISABLED,
	UICOLOR_BUTTON_BACKGROUND_PUSHED,
	UICOLOR_BUTTON_TEXT_NORMAL,
	UICOLOR_BUTTON_TEXT_PUSHED,
	UICOLOR_BUTTON_TEXT_DISABLED,
	UICOLOR_BUTTON_BORDER_LIGHT,
	UICOLOR_BUTTON_BORDER_DARK,
	UICOLOR_BUTTON_BORDER_DISABLED,
	UICOLOR_BUTTON_BORDER_FOCUS,
	UICOLOR_CONTROL_BACKGROUND_NORMAL,
	UICOLOR_CONTROL_BACKGROUND_SELECTED,
	UICOLOR_CONTROL_BACKGROUND_DISABLED,
	UICOLOR_CONTROL_BACKGROUND_READONLY,
	UICOLOR_CONTROL_BACKGROUND_HOVER,
	UICOLOR_CONTROL_BACKGROUND_SORTED,
	UICOLOR_CONTROL_BACKGROUND_EXPANDED,
	UICOLOR_CONTROL_BORDER_NORMAL,
	UICOLOR_CONTROL_BORDER_SELECTED,
	UICOLOR_CONTROL_BORDER_DISABLED,
	UICOLOR_CONTROL_TEXT_NORMAL,
	UICOLOR_CONTROL_TEXT_SELECTED,
	UICOLOR_CONTROL_TEXT_DISABLED,
	UICOLOR_CONTROL_TEXT_READONLY,
	UICOLOR_TOOL_BACKGROUND_NORMAL,
	UICOLOR_TOOL_BACKGROUND_DISABLED,
	UICOLOR_TOOL_BACKGROUND_HOVER,
	UICOLOR_TOOL_BACKGROUND_PUSHED,
	UICOLOR_TOOL_BORDER_NORMAL,
	UICOLOR_TOOL_BORDER_DISABLED,
	UICOLOR_TOOL_BORDER_HOVER,
	UICOLOR_TOOL_BORDER_PUSHED,
	UICOLOR_EDIT_BACKGROUND_NORMAL,
	UICOLOR_EDIT_BACKGROUND_HOVER,
	UICOLOR_EDIT_BACKGROUND_DISABLED,
	UICOLOR_EDIT_BACKGROUND_READONLY,
	UICOLOR_EDIT_TEXT_NORMAL,
	UICOLOR_EDIT_TEXT_DISABLED,
	UICOLOR_EDIT_TEXT_READONLY,
	UICOLOR_TITLE_BACKGROUND,
	UICOLOR_TITLE_TEXT,
	UICOLOR_TITLE_BORDER_LIGHT,
	UICOLOR_TITLE_BORDER_DARK,
	UICOLOR_HEADER_BACKGROUND,
	UICOLOR_HEADER_BORDER,
	UICOLOR_HEADER_SEPARATOR,
	UICOLOR_HEADER_TEXT,
	UICOLOR_TASK_BACKGROUND,
	UICOLOR_TASK_CAPTION,
	UICOLOR_TASK_BORDER,
	UICOLOR_TASK_TEXT,
	UICOLOR_LINK_TEXT_HOVER,
	UICOLOR_LINK_TEXT_NORMAL,
	UICOLOR_STANDARD_BLACK,
	UICOLOR_STANDARD_YELLOW,
	UICOLOR_STANDARD_RED,
	UICOLOR_STANDARD_GREY,
	UICOLOR_STANDARD_LIGHTGREY,
	UICOLOR_STANDARD_WHITE,
	UICOLOR__LAST,
	UICOLOR__INVALID,
} UITYPE_COLOR;
/////////////////////////////////////////////////////////////////////////////////////
//

// Flags for CControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004
#define UIFLAG_WANTCAPTURE   0x00000008

// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_TOP_FIRST     0x00000008
#define UIFIND_ME_FIRST      0x80000000

// Flags for the CDialogLayout stretching
#define UISTRETCH_NEWGROUP   0x00000001
#define UISTRETCH_NEWLINE    0x00000002
#define UISTRETCH_MOVE_X     0x00000004
#define UISTRETCH_MOVE_Y     0x00000008
#define UISTRETCH_SIZE_X     0x00000010
#define UISTRETCH_SIZE_Y     0x00000020

// Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_READONLY     0x00000020
#define UISTATE_CAPTURED     0x00000040



/////////////////////////////////////////////////////////////////////////////////////
//
#ifdef UILIB_D3D
typedef struct tagEffectAge
{
	bool		m_bEnableEffect;
	int			m_iZoom;
	DWORD		m_dFillingBK;
	int			m_iOffectX;
	int			m_iOffectY;
	int			m_iAlpha;
	float		m_fRotation;
	int			m_iNeedTimer;
}TEffectAge;
#endif

typedef struct tagTProperty
{
	union unValue
	{
		RECT		rcValue;
		INT			iValue;
		UINT		uValue;
		LONG		lValue;
		DWORD		dwValue;
		SIZE		siValue;
	};
	
	UINT			uInterval;
	UINT			uTimer;
	UINT			uDelay;
	bool			bLoop;
	bool			bReverse;
	bool			bAutoStart;
	CDuiString		sImageLeft;
	CDuiString		sImageRight;
	CDuiString		sName;
	CDuiString		sType;
	CDuiString		sStartValue;
	CDuiString		sEndValue;
	unValue			nStartValue;
	unValue			nEndValue;
	unValue			nDiffValue;

	INT GetDiffINT(INT _StartValue,INT _EndValue,int _CurFrame){
		if(_CurFrame == 1 && nDiffValue.iValue != _StartValue)
			nDiffValue.iValue = max(_StartValue,_EndValue) - min(_StartValue,_EndValue);
		
		return nDiffValue.iValue;
	}

	INT CalDiffInt(INT _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		if(_CurFrame == 1 && _IsStartNone)
			nStartValue.iValue = _SrcValue;
		if(_CurFrame == 1 && _IsEndNone)
			nEndValue.iValue = _SrcValue;

		int iPartValue	= GetDiffINT(nStartValue.iValue,nEndValue.iValue,_CurFrame) / _TotalFrame;
		int iCurValue;
		if(_CurFrame == _TotalFrame)
			iCurValue = nEndValue.iValue;
		else
			iCurValue = nStartValue.iValue < nEndValue.iValue?(_CurFrame * iPartValue + nStartValue.iValue):(nStartValue.iValue - _CurFrame * iPartValue);
		
		return iCurValue;
	}

	UINT GetDiffUINT(UINT _StartValue,UINT _EndValue,int _CurFrame){
		if(_CurFrame == 1 && nDiffValue.iValue != _StartValue)
			nDiffValue.uValue = max(_StartValue,_EndValue) - min(_StartValue,_EndValue);

		return nDiffValue.uValue;
	}

	UINT CalDiffUint(UINT _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		if(_CurFrame == 1 && _IsStartNone)
			nStartValue.uValue = _SrcValue;
		if(_CurFrame == 1 && _IsEndNone)
			nEndValue.uValue = _SrcValue;

		int iPartValue	= GetDiffUINT(nStartValue.uValue,nEndValue.uValue,_CurFrame) / _TotalFrame;
		int iCurValue;
		if(_CurFrame == _TotalFrame)
			iCurValue = nEndValue.iValue;
		else
			iCurValue = nStartValue.iValue < nEndValue.iValue?(_CurFrame * iPartValue + nStartValue.iValue):(nStartValue.iValue - _CurFrame * iPartValue);

		return iCurValue;
	}

	LONG GetDiffLONG(LONG _StartValue,LONG _EndValue,int _CurFrame){
		if(_CurFrame == 1 && nDiffValue.iValue != _StartValue)
			nDiffValue.lValue = max(_StartValue,_EndValue) - min(_StartValue,_EndValue);

		return nDiffValue.lValue;
	}

	LONG CalDiffLong(LONG _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		if(_CurFrame == 1 && _IsStartNone)
		nStartValue.lValue = _SrcValue;
		if(_CurFrame == 1 && _IsEndNone)
			nEndValue.lValue = _SrcValue;

		int iPartValue	= GetDiffLONG(nStartValue.lValue,nEndValue.lValue,_CurFrame) / _TotalFrame;
		int iCurValue;
		if(_CurFrame == _TotalFrame)
			iCurValue = nEndValue.iValue;
		else
			iCurValue = nStartValue.iValue < nEndValue.iValue?(_CurFrame * iPartValue + nStartValue.iValue):(nStartValue.iValue - _CurFrame * iPartValue);

		return iCurValue;
	}

	RECT GetDiffRect(RECT _StartValue,RECT _EndValue,int _CurFrame){
		if(_CurFrame == 1 && (nDiffValue.rcValue.left != _StartValue.left || nDiffValue.rcValue.right != _StartValue.right || nDiffValue.rcValue.top != _StartValue.top || nDiffValue.rcValue.bottom != _StartValue.bottom)){
			nDiffValue.rcValue.left		= max(_StartValue.left,_EndValue.left) - min(_StartValue.left,_EndValue.left);
			nDiffValue.rcValue.right	= max(_StartValue.right,_EndValue.right) - min(_StartValue.right,_EndValue.right);
			nDiffValue.rcValue.top		= max(_StartValue.top,_EndValue.top) - min(_StartValue.top,_EndValue.top);
			nDiffValue.rcValue.bottom	= max(_StartValue.bottom,_EndValue.bottom) - min(_StartValue.bottom,_EndValue.bottom);
		}

		return nDiffValue.rcValue;
	}

	RECT CalDiffRect(RECT _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		if(_CurFrame == 1 && _IsStartNone)
			nStartValue.rcValue = _SrcValue;
		if(_CurFrame == 1 && _IsEndNone)
			nEndValue.rcValue = _SrcValue;

		RECT nDiffRect	= GetDiffRect(nStartValue.rcValue,nEndValue.rcValue,_CurFrame);
		RECT nCurRect;
		if(_CurFrame == _TotalFrame){
			nCurRect	= nEndValue.rcValue;
		}
		else{
			nCurRect.left	= nStartValue.iValue < nEndValue.iValue?(nDiffRect.left / _TotalFrame * _CurFrame + nStartValue.rcValue.left):(nStartValue.rcValue.left - nDiffRect.left / _TotalFrame * _CurFrame);
			nCurRect.right	= nStartValue.iValue < nEndValue.iValue?(nDiffRect.right / _TotalFrame * _CurFrame + nStartValue.rcValue.right):(nStartValue.rcValue.right - nDiffRect.right / _TotalFrame * _CurFrame);
			nCurRect.top	= nStartValue.iValue < nEndValue.iValue?(nDiffRect.top / _TotalFrame * _CurFrame + nStartValue.rcValue.top):(nStartValue.rcValue.top - nDiffRect.top / _TotalFrame * _CurFrame);
			nCurRect.bottom	= nStartValue.iValue < nEndValue.iValue?(nDiffRect.bottom / _TotalFrame * _CurFrame + nStartValue.rcValue.bottom):(nStartValue.rcValue.bottom - nDiffRect.bottom / _TotalFrame * _CurFrame);
		}
		return nCurRect;
	}

	DWORD GetDiffColor(DWORD _StartValue,DWORD _EndValue,int _CurFrame){
		if(_CurFrame == 1 && nDiffValue.dwValue != _StartValue){
			Gdiplus::Color nStartColor(_StartValue);
			Gdiplus::Color nEndColor(_EndValue);

			int iDiffValueA = max(nEndColor.GetA(),nStartColor.GetA()) - min(nEndColor.GetA(),nStartColor.GetA());
			int iDiffValueR = max(nEndColor.GetR(),nStartColor.GetR()) - min(nEndColor.GetR(),nStartColor.GetR());
			int iDiffValueG = max(nEndColor.GetG(),nStartColor.GetG()) - min(nEndColor.GetG(),nStartColor.GetG());
			int iDiffValueB = max(nEndColor.GetB(),nStartColor.GetB()) - min(nEndColor.GetB(),nStartColor.GetB());

			Gdiplus::Color nRetColor(iDiffValueA,iDiffValueR,iDiffValueG,iDiffValueB);

			nDiffValue.dwValue = nRetColor.GetValue();
		}
		return nDiffValue.dwValue;
	}

	DWORD CalCurColor(DWORD _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		if(_CurFrame == 1 && _IsStartNone)
		nStartValue.dwValue = _SrcValue;
		if(_CurFrame == 1 && _IsEndNone)
			nEndValue.dwValue = _SrcValue;

		Gdiplus::Color nStartColor(nStartValue.dwValue);
		Gdiplus::Color nDiffColor(GetDiffColor(nStartValue.dwValue,nEndValue.dwValue,_CurFrame));
		DWORD nCurColor;
		if(_CurFrame == _TotalFrame){
			nCurColor = Gdiplus::Color(nEndValue.dwValue).GetValue();
		}
		else{
			int iPartValueA = nStartValue.iValue < nEndValue.iValue?(nDiffColor.GetA() / _TotalFrame * _CurFrame + nStartColor.GetA()):(nStartColor.GetA() - nDiffColor.GetA() / _TotalFrame * _CurFrame);
			int iPartValueR = nStartValue.iValue < nEndValue.iValue?(nDiffColor.GetR() / _TotalFrame * _CurFrame + nStartColor.GetR()):(nStartColor.GetR() - nDiffColor.GetR() / _TotalFrame * _CurFrame);
			int iPartValueG = nStartValue.iValue < nEndValue.iValue?(nDiffColor.GetG() / _TotalFrame * _CurFrame + nStartColor.GetG()):(nStartColor.GetG() - nDiffColor.GetG() / _TotalFrame * _CurFrame);
			int iPartValueB = nStartValue.iValue < nEndValue.iValue?(nDiffColor.GetB() / _TotalFrame * _CurFrame + nStartColor.GetB()):(nStartColor.GetB() - nDiffColor.GetB() / _TotalFrame * _CurFrame);
			nCurColor = Gdiplus::Color(iPartValueA,iPartValueR,iPartValueG,iPartValueB).GetValue();
		}
		return nCurColor;
	}

	SIZE GetDiffSize(SIZE _StartValue,SIZE _EndValue,int _CurFrame){
		if(_CurFrame == 1 && (nDiffValue.siValue.cx != _StartValue.cx || nDiffValue.siValue.cy != _StartValue.cy)){
			nDiffValue.siValue.cx	= max(_StartValue.cx,_EndValue.cx) - min(_StartValue.cx,_EndValue.cx);
			nDiffValue.siValue.cy	= max(_StartValue.cy,_EndValue.cy) - min(_StartValue.cy,_EndValue.cy);
		}

		return nDiffValue.siValue;
	}

	SIZE CalCurSize(SIZE _SrcValue,int _TotalFrame,int _CurFrame,bool _IsStartNone,bool _IsEndNone){
		if(_CurFrame == 1 && _IsStartNone)
			nStartValue.siValue = _SrcValue;
		if(_CurFrame == 1 && _IsEndNone)
			nEndValue.siValue = _SrcValue;

		SIZE nDiffSize = GetDiffSize(nStartValue.siValue,nEndValue.siValue,_CurFrame);
		SIZE nCurSize;
		if(_CurFrame == _TotalFrame){
			nCurSize = nEndValue.siValue;
		}
		else{
			nCurSize.cx	= nStartValue.iValue < nEndValue.iValue?(nDiffSize.cx / _TotalFrame * _CurFrame + nDiffSize.cx):(nDiffSize.cx - nDiffSize.cx / _TotalFrame * _CurFrame);
			nCurSize.cy	= nStartValue.iValue < nEndValue.iValue?(nDiffSize.cy / _TotalFrame * _CurFrame + nDiffSize.cy):(nDiffSize.cy - nDiffSize.cy / _TotalFrame * _CurFrame);
		}

		return nCurSize;
	}

	bool IsStartNull(){
		return _tcscmp(_T("none"),sStartValue.GetData()) == 0;
	}

	bool IsEndNull(){
		return _tcscmp(_T("none"),sEndValue.GetData()) == 0;
	}

	tagTProperty():uInterval(0),uDelay(0),uTimer(0),bLoop(false),bReverse(false){
		::ZeroMemory((void*)&nStartValue, sizeof(unValue));
		::ZeroMemory((void*)&nEndValue, sizeof(unValue));
	}

}TProperty;

typedef struct tagTActionGroup
{
	UINT		uDefaultInterval;
	UINT		uDefaultTimer;
	bool		bDefaultLoop;
	bool		bDefaultAutoStart;
	bool		bDefaultReverse;
	INT			iEventValue;
	CDuiString	sMsgValue;
	CDuiString	sMsgType;
	CDuiString	sName;
	TStdPtrArray<TProperty*> mPropertys;

	tagTActionGroup(void){
		uDefaultInterval	= 0;
		bDefaultLoop		= false;
		bDefaultReverse		= false;
		iEventValue			= 0;
	}

	~tagTActionGroup(void){
		for(int i = 0;i < mPropertys.GetSize();i++){
			TProperty* pTp = mPropertys.GetAt(i);
			if(!pTp)
				continue;

			delete pTp;
			pTp = NULL;
			mPropertys.Remove(i);
		}
		mPropertys.Empty();
	}
}TAGroup;

// Structure for notifications from the system
// to the control implementation.
typedef struct tagTEventUI
{
    int Type;
	bool bHandle;
    CControlUI* pSender;
    DWORD dwTimestamp;
    POINT ptMouse;
    TCHAR chKey;
    WORD wKeyState;
    WPARAM wParam;
    LPARAM lParam;
} TEventUI;

// Structure for relative position to the parent
typedef struct tagTRelativePosUI
{
	bool bRelative;
	SIZE szParent;
	int nMoveXPercent;
	int nMoveYPercent;
	int nZoomXPercent;
	int nZoomYPercent;
}TRelativePosUI;

// Listener interface
class INotifyUI
{
public:
	virtual void Notify(TNotifyUI& msg) = 0;
};

// MessageFilter interface
class IMessageFilterUI
{
public:
    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};

class ITranslateAccelerator
{
public:
	virtual LRESULT TranslateAccelerator(MSG *pMsg) = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//
typedef CControlUI* (*LPCREATECONTROL)(LPCTSTR pstrType);


class UILIB_API CPaintManagerUI
{
public:
    CPaintManagerUI();
    ~CPaintManagerUI();
public:
    void Init(HWND hWnd,RenderWay _RenderWay = SKIA_RENDER);
	RenderWay GetRenderWay();
	void SetOwner(LPVOID pOwner);
	CWindowWnd *GetOwner();
    void NeedUpdate();
    void Invalidate(RECT& rcItem);

    HDC GetPaintDC() const;
    HWND GetPaintWindow() const;
    HWND GetTooltipWindow() const;

    POINT GetMousePos() const;
    SIZE GetClientSize() const;
    SIZE GetInitSize();
    void SetInitSize(int cx, int cy);
    RECT& GetSizeBox();
    void SetSizeBox(RECT& rcSizeBox);
    RECT& GetCaptionRect();
    void SetCaptionRect(RECT& rcCaption);
    SIZE GetRoundCorner() const;
    void SetRoundCorner(int cx, int cy);
    SIZE GetMinInfo() const;
    void SetMinInfo(int cx, int cy);
    SIZE GetMaxInfo() const;
    void SetMaxInfo(int cx, int cy);
	int GetTransparent() const;
    void SetTransparent(int nOpacity);
    void SetBackgroundTransparent(bool bTrans);
	void SetUnLayerdWndShadow(bool bShadow);
	bool IsWndShadow();
	void SetWndGlassEffect(bool bGlass);
	bool IsWndGlass(){return m_bUserGlass;}
    bool IsShowUpdateRect() const;
    void SetShowUpdateRect(bool show);

    static HINSTANCE GetInstance();
    static CDuiString GetInstancePath();
    static CDuiString GetCurrentPath();
    static HINSTANCE GetResourceDll();
    static const CDuiString& GetResourcePath();
    static const CDuiString& GetResourceZip();
    static bool IsCachedResourceZip();
    static HANDLE GetResourceZipHandle();
    static void SetInstance(HINSTANCE hInst);
    static void SetCurrentPath(LPCTSTR pStrPath);
    static void SetResourceDll(HINSTANCE hInst);
    static void SetResourcePath(LPCTSTR pStrPath);
	static void SetResourceZip(LPCTSTR resID,LPVOID pVoid, unsigned int len);
    static void SetResourceZip(LPCTSTR pstrZip, bool bCachedResourceZip = false);
    static void GetHSL(short* H, short* S, short* L);
    static void SetHSL(bool bUseHSL, short H, short S, short L); // H:0~360, S:0~200, L:0~200 
    static void ReloadSkin();
    static bool LoadPlugin(LPCTSTR pstrModuleName);
    static CStdPtrArray* GetPlugins();

    bool UseParentResource(CPaintManagerUI* pm);
    CPaintManagerUI* GetParentResource() const;

	void SetUseMutiLanguage(bool bUse = true){m_bUserMultiLanguage = bUse;}
	bool IsUserMutiLanguage(){return m_bUserMultiLanguage;}

    DWORD GetDefaultDisabledColor() const;
    void SetDefaultDisabledColor(DWORD dwColor);
    DWORD GetDefaultFontColor() const;
    void SetDefaultFontColor(DWORD dwColor);
    DWORD GetDefaultLinkFontColor() const;
    void SetDefaultLinkFontColor(DWORD dwColor);
    DWORD GetDefaultLinkHoverFontColor() const;
    void SetDefaultLinkHoverFontColor(DWORD dwColor);
    DWORD GetDefaultSelectedBkColor() const;
    void SetDefaultSelectedBkColor(DWORD dwColor);
	DWORD GetCustomFontCount() const;

	bool FindFont(HFONT hFont);
	bool FindFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);

    TFontInfo* GetDefaultFontInfo();
    void SetDefaultFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    TFontInfo* AddFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    TFontInfo* AddFontAt(int index, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    int GetFontIndex(HFONT hFont);
	int GetFontIndex(TFontInfo *pFontInfoFd);
    int GetFontIndex(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    bool RemoveFont(HFONT hFont);
	bool RemoveFont(TFontInfo *pFontInfoRm);
    bool RemoveFontAt(int index);
    void RemoveAllFonts();
	void AddFontInfo(TFontInfo* pFontInfo);
	TFontInfo* GetFont(int index);
	TFontInfo* GetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    TFontInfo* GetFontInfo(int index);
    TFontInfo* GetFontInfo(HFONT hFont);

	const TImageInfo* GetImage(LPCTSTR bitmap);
    const TImageInfo* GetImage(CDuiImage& bitmap);
    const TImageInfo* GetImageEx(const CDuiImage& bitmap, LPCTSTR type = NULL, DWORD mask = 0);
	const TImageInfo* AddImage(CDuiImage& bitmap, LPCTSTR type = NULL, DWORD mask = 0);
    const TImageInfo* AddImage(CDuiImage& bitmap, HBITMAP hBitmap, int nWidth, int nHeight, bool bAlpha);
	bool RemoveImage(LPCTSTR bitmap);
    bool RemoveImage(CDuiImage& bitmap);
    void RemoveAllImages();
    void ReloadAllImages();

    void AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList);
    LPCTSTR GetDefaultAttributeList(LPCTSTR pStrControlName) const;
    bool RemoveDefaultAttributeList(LPCTSTR pStrControlName);
    const CStdStringPtrMap& GetDefaultAttribultes() const;
    void RemoveAllDefaultAttributeList();

    bool AttachDialog(CControlUI* pControl);
    bool InitControls(CControlUI* pControl, CControlUI* pParent = NULL);
    void ReapObjects(CControlUI* pControl);

    bool AddOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
    CStdPtrArray* GetOptionGroup(LPCTSTR pStrGroupName);
    void RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
    void RemoveAllOptionGroups();

#ifdef UILIB_D3D
	bool AddAnimationJob(const CDxAnimationUI& _DxAnimationUI);
	bool AddEffectsStyle(LPCTSTR pStrStyleName,LPCTSTR pStrStyleValue);
	CDuiString GetEffectsStyle(LPCTSTR pStrStyleName);
	const CStdStringPtrMap& GetEffectsStyles() const;
	bool RemoveEffectStyle(LPCTSTR pStrStyleName);
	void RemoveAllEffectStyle();
#endif

	bool AddControlStyle(LPCTSTR pStrStyleName,LPCTSTR pStrKey,LPCTSTR pStrVal,LPCTSTR pStylesName = NULL);
	bool AddControlStyle(LPCTSTR pStrStyleName,CStdStringPtrMap* _StyleMap,LPCTSTR pStylesName = NULL);
	bool SetControlStyle(LPCTSTR pStrStyleName,LPCTSTR pStrKey,LPCTSTR pStrVal,LPCTSTR pStylesName = NULL);
	bool SetControlStyle(LPCTSTR pStrStyleName,CStdStringPtrMap* _StyleMap,LPCTSTR pStylesName = NULL);
	CDuiString GetControlStyle(LPCTSTR pStrStyleName,LPCTSTR pStrKey,LPCTSTR pStylesName = NULL);
	CStdStringPtrMap* GetControlsStyles(LPCTSTR pStylesName) ;
	CStdStringPtrMap* GetControlStyles(LPCTSTR pStrStyleName,LPCTSTR pStylesName = NULL) ;
	bool RemoveControlStyle(LPCTSTR pStrStyleName,LPCTSTR pStrKey = NULL,LPCTSTR pStylesName = NULL);
	void RemoveAllControlStyle(LPCTSTR pStrStyleName = NULL,LPCTSTR pStylesName = NULL);
	bool SetCurStyles(LPCTSTR pStylesName = NULL,bool _NowUpdate = true);
	bool SetCurStyles(int _iStyleIndex = 0,bool _NowUpdate = true);
	UINT GetStylesCount();
	CDuiString GetCurStylesName();
	bool RemoveStyles(LPCTSTR pStylesName);
	void RemoveAllStyles();

	bool AddPropertyAction(LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop = false,bool bAutoStart = true);
	bool AddPropertyAction(TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop = false,bool bAutoStart = true);
	bool SetPropertyAction(TAGroup& tAGroup,TProperty* tProperty,bool bAutoCreate = false);
	bool SetPropertyAction(LPCTSTR pAGroupName,TProperty* tProperty,bool bAutoCreate = false);
	bool SetPropertyAction(LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop = false,bool bAutoStart = true,bool bAutoCreate = false);
	bool SetPropertyAction(TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop = false,bool bAutoStart = true,bool bAutoCreate = false);
	TProperty* GetPropertyAction(LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType) const;
	TProperty* GetPropertyAction(TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType) const;
	TProperty* GetPropertyAction(LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType,int& iIndex) const;
	TProperty* GetPropertyAction(TAGroup& tAGroup,LPCTSTR pPropertyName,LPCTSTR pType,int& iIndex) const;
	bool RemovePropertyAction(LPCTSTR pAGroupName,LPCTSTR pPropertyName,LPCTSTR pType);
	bool SetPropertyActionParse(TProperty& nTProperty,LPCTSTR pPropertyName,LPCTSTR pType,LPCTSTR pStartValue,LPCTSTR pEndValue,int iInterval,int iTimer,int iDelay,bool bRevers,bool bLoop = false,bool bAutoStart = true);
	int HasPropertyMsgType(TAGroup& tAGroup,LPCTSTR pType);
	int HasPropertyMsgType(LPCTSTR pAGroupName,LPCTSTR pType);

	bool AddActionScriptGroup(LPCTSTR pAGroupName,LPCTSTR pNotifyName,int iDefaultInterval = 0,int iDefaultTimer = 500,bool bDefaultReverse = false,bool bDefaultLoop = false,bool bDefaultAutoStart = true);
	bool AddActionScriptGroup(LPCTSTR pAGroupName,EVENTTYPE_UI pEventType,int iDefaultInterval = 0,int iDefaultTimer = 500,bool bDefaultReverse = false,bool bDefaultLoop = false,bool bDefaultAutoStart = true);
	bool SetActionScriptGroup(TAGroup** tAGroup,bool bMergerProperty = true);
	bool SetActionScriptGroup(LPCTSTR pAGroupName,LPCTSTR pNotifyName,int iDefaultInterval = 0,int iDefaultTimer = 500,bool bDefaultReverse = false,bool bDefaultLoop = false,bool bDefaultAutoStart = true,bool bMergerProperty = false);
	bool SetActionScriptGroup(LPCTSTR pAGroupName,EVENTTYPE_UI pEventType,int iDefaultInterval = 0,int iDefaultTimer = 500,bool bDefaultReverse = false,bool bDefaultLoop = false,bool bDefaultAutoStart = true,bool bMergerProperty = true);
	TAGroup* GetActionScriptGroup(LPCTSTR pAGroupName) const;
	bool RemoveActionScriptGroup(LPCTSTR pAGroupName);
	void RemoveActionScriptGroupAll();
	int HasActionScriptGroup(TAGroup& tAGroup,LPCTSTR pMsgType = NULL);
	int HasActionScriptGroup(TAGroup& tAGroup,int pEventValue = 0);
	int HasActionScriptGroup(LPCTSTR pAGroupName,LPCTSTR pMsgType = NULL);
	int HasActionScriptGroup(LPCTSTR pAGroupName,int pEventValue = 0);

	CUIAnimateManager *GetAnimateManager(){return m_pAnimateManager;}
	bool AddStoryBoard(CStoryBoard *pStoryBoard){if(m_pAnimateManager)return m_pAnimateManager->AddStoryboard(pStoryBoard);return false;}
	bool AddStoryBoardBlock(CStoryBoard *pStoryBoard){if(m_pAnimateManager)return m_pAnimateManager->AddStoryboardBlock(pStoryBoard); return false;}
	void RemoveStoryBoard(CStoryBoard *pStoryBoard){if(m_pAnimateManager)m_pAnimateManager->RemoveStoryboard(pStoryBoard);}

    CControlUI* GetFocus() const;
    void SetFocus(CControlUI* pControl);
    void SetFocusNeeded(CControlUI* pControl);

	void AddStringRess(CDuiString szID,CDuiString szText);
	CDuiString GetStringFromID(CDuiString szID);
	void ClearStringRess();

	bool SetDefaultFocus(CControlUI *pControl);
    bool SetNextTabControl(bool bForward = true);

    bool SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse);
    bool KillTimer(CControlUI* pControl, UINT nTimerID);
    void KillTimer(CControlUI* pControl);
    void RemoveAllTimers();

	void AddWndContainer(CControlUI *pControl);
	void RemoveWndContainer(CControlUI *pControl);
	void RemoveAllWndContainer();

    void SetCapture(CControlUI* pControl = NULL);
    void ReleaseCapture();
    bool IsCaptured();
	CControlUI* GetCapture() const;

	void EventAllControl(TEventUI& event,CControlUI* pControl = NULL);
    bool AddNotifier(INotifyUI* pControl);
    bool RemoveNotifier(INotifyUI* pControl);
    void SendNotify(TNotifyUI& Msg, bool bAsync = false);
    void SendNotify(CControlUI* pControl, LPCTSTR pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

    bool AddPreMessageFilter(IMessageFilterUI* pFilter);
    bool RemovePreMessageFilter(IMessageFilterUI* pFilter);

    bool AddMessageFilter(IMessageFilterUI* pFilter);
    bool RemoveMessageFilter(IMessageFilterUI* pFilter);

    int GetPostPaintCount() const;
    bool AddPostPaint(CControlUI* pControl);
    bool RemovePostPaint(CControlUI* pControl);
    bool SetPostPaintIndex(CControlUI* pControl, int iIndex);

	void AddIDOKEvent(const CDelegateBase &d);
	bool HandleIDOKEvent();

    void AddDelayedCleanup(CControlUI* pControl);

	bool AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool TranslateAccelerator(LPMSG pMsg);

    CControlUI* GetRoot() const;
    CControlUI* FindControl(POINT pt) const;
    CControlUI* FindControl(LPCTSTR pstrName) const;
    CControlUI* FindSubControlByPoint(CControlUI* pParent, POINT pt) const;
    CControlUI* FindSubControlByName(CControlUI* pParent, LPCTSTR pstrName) const;
    CControlUI* FindSubControlByClass(CControlUI* pParent, LPCTSTR pstrClass, int iIndex = 0);
    CStdPtrArray* FindSubControlsByClass(CControlUI* pParent, LPCTSTR pstrClass);
    CStdPtrArray* GetSubControlsByClass();

    static void MessageLoop();
    static bool TranslateMessage(const LPMSG pMsg);
	static void Term();

    bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
    bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	void UsedVirtualWnd(bool bUsed);

	HPEN GetThemePen(UITYPE_COLOR Index) const;
	HFONT GetThemeFont(UITYPE_FONT Index) const;
	HBRUSH GetThemeBrush(UITYPE_COLOR Index) const;
	COLORREF GetThemeColor(UITYPE_COLOR Index) const;
	HICON GetThemeIcon(int Index, int cxySize) const;
	const TEXTMETRIC& GetThemeFontInfo(UITYPE_FONT Index) const;
	bool GetThemeColorPair(UITYPE_COLOR Index, COLORREF& clr1, COLORREF& clr2) const;

	CEventSource& GetEventSource();
	CDuiTrayIcon& GetTrayObject();

	CUIRenderCore *GetRenderCore();
private:
    static CControlUI* CALLBACK __FindControlFromNameHash(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromCount(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromPoint(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromTab(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromShortcut(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromUpdate(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromName(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromClass(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlsFromClass(CControlUI* pThis, LPVOID pData);

private:
	RenderWay m_RenderWay;
	CWindowWnd *m_pOwnerWnd;
    HWND m_hWndPaint;
	int m_nOpacity;
    HWND m_hwndTooltip;
    TOOLINFO m_ToolTip;
	//
	CDuiTrayIcon mDuiTray;
    //
    CControlUI* m_pRoot;
    CControlUI* m_pFocus;
	CControlUI* m_pDefaultFocus;
	CControlUI* m_pCapture;
    CControlUI* m_pEventEnter;
    CControlUI* m_pEventClick;
    CControlUI* m_pEventKey;
    //
    POINT m_ptLastMousePos;
    SIZE m_szMinWindow;
    SIZE m_szMaxWindow;
    SIZE m_szInitWindowSize;
    RECT m_rcSizeBox;
    SIZE m_szRoundCorner;
    RECT m_rcCaption;
    UINT m_uTimerID;
    bool m_bFirstLayout;
    bool m_bUpdateNeeded;
    bool m_bFocusNeeded;
    bool m_bMouseTracking;
    bool m_bMouseCapture;
	bool m_bUsedVirtualWnd;
	bool m_bShowUpdateRect;
	bool m_bUserGlass;
	bool m_bWndShadow;
	bool m_bUserMultiLanguage;
	//
	CDuiString m_sCurStylesName;
    //
	CStdPtrArray m_aWndContainers;
    CStdPtrArray m_aNotifiers;
    CStdPtrArray m_aTimers;
    CStdPtrArray m_aPreMessageFilters;
    CStdPtrArray m_aMessageFilters;
    CStdPtrArray m_aPostPaintControls;
    CStdPtrArray m_aDelayedCleanup;
    CStdPtrArray m_aAsyncNotify;
    CStdPtrArray m_aFoundControls;
	StringResMap m_StringRess;
    CStdStringPtrMap m_mNameHash;
    CStdStringPtrMap m_mOptionGroup;
	CStdStringPtrMap m_mStyles;
	CStdStringPtrMap m_mEffectsStyle;
	TStdStringPtrMap<TAGroup*>	m_mActionScript;
    //
    CPaintManagerUI* m_pParentResourcePM;
    DWORD m_dwDefaultDisabledColor;
    DWORD m_dwDefaultFontColor;
    DWORD m_dwDefaultLinkFontColor;
    DWORD m_dwDefaultLinkHoverFontColor;
    DWORD m_dwDefaultSelectedBkColor;
    CStdPtrArray m_aCustomFonts;

    CStdStringPtrMap m_mImageHash;
    CStdStringPtrMap m_DefaultAttrHash;
	//
	CEventSource m_aCustomEvents;
	CEventSource m_aIDOKEvents;
    //
    static HINSTANCE m_hInstance;
    static HINSTANCE m_hResourceInstance;
    static CDuiString m_pStrResourcePath;
    static CDuiString m_pStrResourceZip;
    static bool m_bCachedResourceZip;
    static HANDLE m_hResourceZip;
    static short m_H;
    static short m_S;
    static short m_L;
    static CStdPtrArray m_aPreMessages;
	static CStdPtrArray m_aPlugins;

	//rendercore
	CUIRenderCore *m_pRenderCore;
	
	CUIAnimateManager *m_pAnimateManager;
public:
	static CDuiString m_pStrDefaultFontName;
	//WaitableHandles
	static CWaitableHandleMgr m_WaitForHandlesMgr;
	CStdPtrArray m_aTranslateAccelerator;
};

} // namespace UiLib

#endif // __UIMANAGER_H__
