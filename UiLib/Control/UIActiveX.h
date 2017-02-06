#ifndef __UIACTIVEX_H__
#define __UIACTIVEX_H__

#pragma once

struct IOleObject;

namespace UiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class CActiveXCtrl;

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CActiveXUI : public CControlUI, public IMessageFilterUI
{
    friend class CActiveXCtrl;
public:
    CActiveXUI();
    virtual ~CActiveXUI();

    LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

    HWND GetHostWindow() const;

    bool IsDelayCreate() const;
    void SetDelayCreate(bool bDelayCreate = true);

    bool CreateControl(const CLSID clsid);
    bool CreateControl(LPCTSTR pstrCLSID);
    HRESULT GetControl(const IID iid, LPVOID* ppRet);
	CLSID GetClisd() const;
    CDuiString GetModuleName() const;
    void SetModuleName(LPCTSTR pstrText);

    void SetVisible(bool bVisible = true);
    void SetInternVisible(bool bVisible = true);
    void SetPos(RECT rc);
    void DoPaint(const RECT& rcPaint);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:
    virtual void ReleaseControl();
    virtual bool DoCreateControl();

protected:
    CLSID m_clsid;
    CDuiString m_sModuleName;
    bool m_bCreated;
    bool m_bDelayCreate;
    IOleObject* m_pUnk;
    CActiveXCtrl* m_pControl;
    HWND m_hwndHost;
};

class UILIB_API CActiveXCtrl :
	public IOleClientSite,
	public IOleInPlaceSiteWindowless,
	public IOleControlSite,
	public IObjectWithSite,
	public IOleContainer
{
	friend class CActiveXUI;
	friend class CActiveXWnd;
public:
	CActiveXCtrl();
	~CActiveXCtrl();

	// IUnknown
	STDMETHOD_(ULONG,AddRef)();
	STDMETHOD_(ULONG,Release)();
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);

	// IObjectWithSite
	STDMETHOD(SetSite)(IUnknown *pUnkSite);
	STDMETHOD(GetSite)(REFIID riid, LPVOID* ppvSite);

	// IOleClientSite
	STDMETHOD(SaveObject)(void);       
	STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk);
	STDMETHOD(GetContainer)(IOleContainer** ppContainer);        
	STDMETHOD(ShowObject)(void);        
	STDMETHOD(OnShowWindow)(BOOL fShow);        
	STDMETHOD(RequestNewObjectLayout)(void);

	// IOleInPlaceSiteWindowless
	STDMETHOD(CanWindowlessActivate)(void);
	STDMETHOD(GetCapture)(void);
	STDMETHOD(SetCapture)(BOOL fCapture);
	STDMETHOD(GetFocus)(void);
	STDMETHOD(SetFocus)(BOOL fFocus);
	STDMETHOD(GetDC)(LPCRECT pRect, DWORD grfFlags, HDC* phDC);
	STDMETHOD(ReleaseDC)(HDC hDC);
	STDMETHOD(InvalidateRect)(LPCRECT pRect, BOOL fErase);
	STDMETHOD(InvalidateRgn)(HRGN hRGN, BOOL fErase);
	STDMETHOD(ScrollRect)(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip);
	STDMETHOD(AdjustRect)(LPRECT prc);
	STDMETHOD(OnDefWindowMessage)(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);

	// IOleInPlaceSiteEx
	STDMETHOD(OnInPlaceActivateEx)(BOOL *pfNoRedraw, DWORD dwFlags);        
	STDMETHOD(OnInPlaceDeactivateEx)(BOOL fNoRedraw);       
	STDMETHOD(RequestUIActivate)(void);

	// IOleInPlaceSite
	STDMETHOD(CanInPlaceActivate)(void);       
	STDMETHOD(OnInPlaceActivate)(void);        
	STDMETHOD(OnUIActivate)(void);
	STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
	STDMETHOD(Scroll)(SIZE scrollExtant);
	STDMETHOD(OnUIDeactivate)(BOOL fUndoable);
	STDMETHOD(OnInPlaceDeactivate)(void);
	STDMETHOD(DiscardUndoState)( void);
	STDMETHOD(DeactivateAndUndo)( void);
	STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);

	// IOleWindow
	STDMETHOD(GetWindow)(HWND* phwnd);
	STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

	// IOleControlSite
	STDMETHOD(OnControlInfoChanged)(void);      
	STDMETHOD(LockInPlaceActive)(BOOL fLock);       
	STDMETHOD(GetExtendedControl)(IDispatch** ppDisp);        
	STDMETHOD(TransformCoords)(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags);       
	STDMETHOD(TranslateAccelerator)(MSG* pMsg, DWORD grfModifiers);
	STDMETHOD(OnFocus)(BOOL fGotFocus);
	STDMETHOD(ShowPropertyFrame)(void);

	// IOleContainer
	STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown** ppenum);
	STDMETHOD(LockContainer)(BOOL fLock);

	// IParseDisplayName
	STDMETHOD(ParseDisplayName)(IBindCtx* pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut);

protected:
	HRESULT CreateActiveXWnd();
public:
	bool	_GetWindowless(){return m_bWindowless;};
	bool	_GetCaptured(){return m_bCaptured;};
	bool	_GetInPlaceActive(){return m_bInPlaceActive;};
	IOleInPlaceObjectWindowless* _GetInPlaceObjectWindowless(){return m_pInPlaceObject;};
	IViewObject* _GetViewObject(){return m_pViewObject;};

protected:
	LONG m_dwRef;
	CActiveXUI* m_pOwner;
	CActiveXWnd* m_pWindow;
	IUnknown* m_pUnkSite;
	IViewObject* m_pViewObject;
	IOleInPlaceObjectWindowless* m_pInPlaceObject;
	bool m_bLocked;
	bool m_bFocused;
	bool m_bCaptured;
	bool m_bUIActivated;
	bool m_bInPlaceActive;
	bool m_bWindowless;
};

} // namespace DuiLib

#endif // __UIACTIVEX_H__
