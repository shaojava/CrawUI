#include "StdAfx.h"
#include "UIActiveX.h"

namespace UiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

class CActiveXCtrl;

REGIST_DUICLASS(CActiveXCtrl);
REGIST_DUICLASS(CWebBrowserUI);
/////////////////////////////////////////////////////////////////////////////////////
//
//

class CActiveXWnd : public CWindowWnd
{
public:
    HWND Init(CActiveXCtrl* pOwner, HWND hWndParent);

    LPCTSTR GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    void DoVerb(LONG iVerb);

    LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
    CActiveXCtrl* m_pOwner;
};


/////////////////////////////////////////////////////////////////////////////////////
//
//

class CActiveXEnum : public IEnumUnknown
{
public:
    CActiveXEnum(IUnknown* pUnk) : m_pUnk(pUnk), m_dwRef(1), m_iPos(0)
    {
        m_pUnk->AddRef();
    }
    ~CActiveXEnum()
    {
        m_pUnk->Release();
    }

    LONG m_iPos;
    ULONG m_dwRef;
    IUnknown* m_pUnk;

    STDMETHOD_(ULONG,AddRef)()
    {
        return ++m_dwRef;
    }
    STDMETHOD_(ULONG,Release)()
    {
        LONG lRef = --m_dwRef;
        if( lRef == 0 ) delete this;
        return lRef;
    }
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
    {
        *ppvObject = NULL;
        if( riid == IID_IUnknown ) *ppvObject = static_cast<IEnumUnknown*>(this);
        else if( riid == IID_IEnumUnknown ) *ppvObject = static_cast<IEnumUnknown*>(this);
        if( *ppvObject != NULL ) AddRef();
        return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
    }
    STDMETHOD(Next)(ULONG celt, IUnknown **rgelt, ULONG *pceltFetched)
    {
        if( pceltFetched != NULL ) *pceltFetched = 0;
        if( ++m_iPos > 1 ) return S_FALSE;
        *rgelt = m_pUnk;
        (*rgelt)->AddRef();
        if( pceltFetched != NULL ) *pceltFetched = 1;
        return S_OK;
    }
    STDMETHOD(Skip)(ULONG celt)
    {
        m_iPos += celt;
        return S_OK;
    }
    STDMETHOD(Reset)(void)
    {
        m_iPos = 0;
        return S_OK;
    }
    STDMETHOD(Clone)(IEnumUnknown **ppenum)
    {
        return E_NOTIMPL;
    }
};


/////////////////////////////////////////////////////////////////////////////////////
//
//

class CActiveXFrameWnd : public IOleInPlaceFrame
{
public:
    CActiveXFrameWnd(CActiveXUI* pOwner) : m_dwRef(1), m_pOwner(pOwner), m_pActiveObject(NULL)
    {
    }
    ~CActiveXFrameWnd()
    {
        if( m_pActiveObject != NULL ) m_pActiveObject->Release();
    }

    ULONG m_dwRef;
    CActiveXUI* m_pOwner;
    IOleInPlaceActiveObject* m_pActiveObject;

    // IUnknown
    STDMETHOD_(ULONG,AddRef)()
    {
        return ++m_dwRef;
    }
    STDMETHOD_(ULONG,Release)()
    {
        ULONG lRef = --m_dwRef;
        if( lRef == 0 ) delete this;
        return lRef;
    }
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
    {
        *ppvObject = NULL;
        if( riid == IID_IUnknown ) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
        else if( riid == IID_IOleWindow ) *ppvObject = static_cast<IOleWindow*>(this);
        else if( riid == IID_IOleInPlaceFrame ) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
        else if( riid == IID_IOleInPlaceUIWindow ) *ppvObject = static_cast<IOleInPlaceUIWindow*>(this);
        if( *ppvObject != NULL ) AddRef();
        return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
    }  
    // IOleInPlaceFrameWindow
    STDMETHOD(InsertMenus)(HMENU /*hmenuShared*/, LPOLEMENUGROUPWIDTHS /*lpMenuWidths*/)
    {
        return S_OK;
    }
    STDMETHOD(SetMenu)(HMENU /*hmenuShared*/, HOLEMENU /*holemenu*/, HWND /*hwndActiveObject*/)
    {
        return S_OK;
    }
    STDMETHOD(RemoveMenus)(HMENU /*hmenuShared*/)
    {
        return S_OK;
    }
    STDMETHOD(SetStatusText)(LPCOLESTR /*pszStatusText*/)
    {
        return S_OK;
    }
    STDMETHOD(EnableModeless)(BOOL /*fEnable*/)
    {
        return S_OK;
    }
    STDMETHOD(TranslateAccelerator)(LPMSG /*lpMsg*/, WORD /*wID*/)
    {
        return S_FALSE;
    }
    // IOleWindow
    STDMETHOD(GetWindow)(HWND* phwnd)
    {
        if( m_pOwner == NULL ) return E_UNEXPECTED;
        *phwnd = m_pOwner->GetManager()->GetPaintWindow();
        return S_OK;
    }
    STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/)
    {
        return S_OK;
    }
    // IOleInPlaceUIWindow
    STDMETHOD(GetBorder)(LPRECT /*lprectBorder*/)
    {
        return S_OK;
    }
    STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
    {
        return INPLACE_E_NOTOOLSPACE;
    }
    STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
    {
        return S_OK;
    }
    STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/)
    {
        if( pActiveObject != NULL ) pActiveObject->AddRef();
        if( m_pActiveObject != NULL ) m_pActiveObject->Release();
        m_pActiveObject = pActiveObject;
        return S_OK;
    }
};

/////////////////////////////////////////////////////////////////////////////////////
//

CActiveXCtrl::CActiveXCtrl() : 
m_dwRef(1), 
m_pOwner(NULL), 
m_pWindow(NULL),
m_pUnkSite(NULL), 
m_pViewObject(NULL),
m_pInPlaceObject(NULL),
m_bLocked(false), 
m_bFocused(false),
m_bCaptured(false),
m_bWindowless(true),
m_bUIActivated(false),
m_bInPlaceActive(false)
{
}

CActiveXCtrl::~CActiveXCtrl()
{
    if( m_pWindow != NULL ) {
        ::DestroyWindow(*m_pWindow);
        delete m_pWindow;
    }
    if( m_pUnkSite != NULL ) m_pUnkSite->Release();
    if( m_pViewObject != NULL ) m_pViewObject->Release();
    if( m_pInPlaceObject != NULL ) m_pInPlaceObject->Release();
}

STDMETHODIMP CActiveXCtrl::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
    *ppvObject = NULL;
    if( riid == IID_IUnknown )                       *ppvObject = static_cast<IOleWindow*>(this);
    else if( riid == IID_IOleClientSite )            *ppvObject = static_cast<IOleClientSite*>(this);
    else if( riid == IID_IOleInPlaceSiteWindowless ) *ppvObject = static_cast<IOleInPlaceSiteWindowless*>(this);
    else if( riid == IID_IOleInPlaceSiteEx )         *ppvObject = static_cast<IOleInPlaceSiteEx*>(this);
    else if( riid == IID_IOleInPlaceSite )           *ppvObject = static_cast<IOleInPlaceSite*>(this);
    else if( riid == IID_IOleWindow )                *ppvObject = static_cast<IOleWindow*>(this);
    else if( riid == IID_IOleControlSite )           *ppvObject = static_cast<IOleControlSite*>(this);
    else if( riid == IID_IOleContainer )             *ppvObject = static_cast<IOleContainer*>(this);
    else if( riid == IID_IObjectWithSite )           *ppvObject = static_cast<IObjectWithSite*>(this);
    if( *ppvObject != NULL ) AddRef();
    return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}

STDMETHODIMP_(ULONG) CActiveXCtrl::AddRef()
{
    return ++m_dwRef;
}

STDMETHODIMP_(ULONG) CActiveXCtrl::Release()
{
    LONG lRef = --m_dwRef;
    if( lRef == 0 ) delete this;
    return lRef;
}

STDMETHODIMP CActiveXCtrl::SetSite(IUnknown *pUnkSite)
{
    if( m_pUnkSite != NULL ) {
        m_pUnkSite->Release();
        m_pUnkSite = NULL;
    }
    if( pUnkSite != NULL ) {
        m_pUnkSite = pUnkSite;
        m_pUnkSite->AddRef();
    }
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetSite(REFIID riid, LPVOID* ppvSite)
{
    if( ppvSite == NULL ) return E_POINTER;
    *ppvSite = NULL;
    if( m_pUnkSite == NULL ) return E_FAIL;
    return m_pUnkSite->QueryInterface(riid, ppvSite);
}

STDMETHODIMP CActiveXCtrl::SaveObject(void)
{
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk)
{
    if( ppmk != NULL ) *ppmk = NULL;
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetContainer(IOleContainer** ppContainer)
{
    if( ppContainer == NULL ) return E_POINTER;
    *ppContainer = NULL;
    HRESULT Hr = E_NOTIMPL;
    if( m_pUnkSite != NULL ) Hr = m_pUnkSite->QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
    if( FAILED(Hr) ) Hr = QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
    return Hr;
}

STDMETHODIMP CActiveXCtrl::ShowObject(void)
{
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    HDC hDC = ::GetDC(m_pOwner->m_hwndHost);
    if( hDC == NULL ) return E_FAIL;
	// add by wangshj
	RECT rcPadding = m_pOwner->GetPadding();
	RECT rc = m_pOwner->m_rcItem;;
	rc.left += rcPadding.left;
	rc.top += rcPadding.top;
	rc.right -= rcPadding.right;
	rc.bottom -= rcPadding.bottom;

    if( m_pViewObject != NULL ) m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC, (RECTL*) &rc, (RECTL*) &rc, NULL, NULL);
    ::ReleaseDC(m_pOwner->m_hwndHost, hDC);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnShowWindow(BOOL fShow)
{
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::RequestNewObjectLayout(void)
{
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::CanWindowlessActivate(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::CanWindowlessActivate"));
    return S_OK;  // Yes, we can!!
}

STDMETHODIMP CActiveXCtrl::GetCapture(void)
{
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    return m_bCaptured ? S_OK : S_FALSE;
}

STDMETHODIMP CActiveXCtrl::SetCapture(BOOL fCapture)
{
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    m_bCaptured = (fCapture == TRUE);
    if( fCapture ) ::SetCapture(m_pOwner->m_hwndHost); else ::ReleaseCapture();
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetFocus(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::GetFocus"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    return m_bFocused ? S_OK : S_FALSE;
}

STDMETHODIMP CActiveXCtrl::SetFocus(BOOL fFocus)
{
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( fFocus ) m_pOwner->SetFocus();
    m_bFocused = (fFocus == TRUE);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetDC(LPCRECT pRect, DWORD grfFlags, HDC* phDC)
{
    if( phDC == NULL ) return E_POINTER;
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    *phDC = ::GetDC(m_pOwner->m_hwndHost);
    if( (grfFlags & OLEDC_PAINTBKGND) != 0 ) {
        CDuiRect rcItem = m_pOwner->GetPos();
        if( !m_bWindowless ) rcItem.ResetOffset();
        ::FillRect(*phDC, &rcItem, (HBRUSH) (COLOR_WINDOW + 1));
    }
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ReleaseDC(HDC hDC)
{
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    ::ReleaseDC(m_pOwner->m_hwndHost, hDC);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::InvalidateRect(LPCRECT pRect, BOOL fErase)
{
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( m_pOwner->m_hwndHost == NULL ) return E_FAIL;
    return ::InvalidateRect(m_pOwner->m_hwndHost, pRect, fErase) ? S_OK : E_FAIL;
}

STDMETHODIMP CActiveXCtrl::InvalidateRgn(HRGN hRGN, BOOL fErase)
{
    DUITRACE(_T("AX: CActiveXCtrl::InvalidateRgn"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    return ::InvalidateRgn(m_pOwner->m_hwndHost, hRGN, fErase) ? S_OK : E_FAIL;
}

STDMETHODIMP CActiveXCtrl::ScrollRect(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip)
{
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::AdjustRect(LPRECT prc)
{
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnDefWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult)
{
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    *plResult = ::DefWindowProc(m_pOwner->m_hwndHost, msg, wParam, lParam);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceActivateEx(BOOL* pfNoRedraw, DWORD dwFlags)        
{
    ASSERT(m_pInPlaceObject==NULL);
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( m_pOwner->m_pUnk == NULL ) return E_UNEXPECTED;
    ::OleLockRunning(m_pOwner->m_pUnk, TRUE, FALSE);
    HWND hWndFrame = m_pOwner->GetManager()->GetPaintWindow();
    HRESULT Hr = E_FAIL;
    if( (dwFlags & ACTIVATE_WINDOWLESS) != 0 ) {
        m_bWindowless = true;
        Hr = m_pOwner->m_pUnk->QueryInterface(IID_IOleInPlaceObjectWindowless, (LPVOID*) &m_pInPlaceObject);
        m_pOwner->m_hwndHost = hWndFrame;
        m_pOwner->GetManager()->AddMessageFilter(m_pOwner);
    }
    if( FAILED(Hr) ) {
        m_bWindowless = false;
        Hr = CreateActiveXWnd();
        if( FAILED(Hr) ) return Hr;
        Hr = m_pOwner->m_pUnk->QueryInterface(IID_IOleInPlaceObject, (LPVOID*) &m_pInPlaceObject);
    }
	// add by wangshj
	RECT rcPadding = m_pOwner->GetPadding();
	RECT rc = m_pOwner->m_rcItem;;
	rc.left += rcPadding.left;
	rc.top += rcPadding.top;
	rc.right -= rcPadding.right;
	rc.bottom -= rcPadding.bottom;

    if( m_pInPlaceObject != NULL ) {
        CDuiRect rcItem = rc;//m_pOwner->m_rcItem;
        if( !m_bWindowless ) rcItem.ResetOffset();
        m_pInPlaceObject->SetObjectRects(&rcItem, &rcItem);
    }
    m_bInPlaceActive = SUCCEEDED(Hr);
    return Hr;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceDeactivateEx(BOOL fNoRedraw)       
{
    m_bInPlaceActive = false;
    if( m_pInPlaceObject != NULL ) {
        m_pInPlaceObject->Release();
        m_pInPlaceObject = NULL;
    }
    if( m_pWindow != NULL ) {
        ::DestroyWindow(*m_pWindow);
        delete m_pWindow;
        m_pWindow = NULL;
    }
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::RequestUIActivate(void)
{
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::CanInPlaceActivate(void)       
{
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceActivate(void)
{
    BOOL bDummy = FALSE;
    return OnInPlaceActivateEx(&bDummy, 0);
}

STDMETHODIMP CActiveXCtrl::OnUIActivate(void)
{
    m_bUIActivated = true;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetWindowContext(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	// delete by wangshj
//     if( ppDoc == NULL ) return E_POINTER;
//     if( ppFrame == NULL ) return E_POINTER;
//     if( lprcPosRect == NULL ) return E_POINTER;
//     if( lprcClipRect == NULL ) return E_POINTER;
// 	if (m_pWindow)
// 	{
// 		::GetClientRect(m_pWindow->GetHWND(),lprcPosRect);
// 		::GetClientRect(m_pWindow->GetHWND(),lprcClipRect);
// 	}
//     *ppFrame = new CActiveXFrameWnd(m_pOwner);
//     *ppDoc = NULL;
//     ACCEL ac = { 0 };
//     HACCEL hac = ::CreateAcceleratorTable(&ac, 1);
//     lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
//     lpFrameInfo->fMDIApp = FALSE;
//     lpFrameInfo->hwndFrame = m_pOwner->GetManager()->GetPaintWindow();
//     lpFrameInfo->haccel = hac;
//     lpFrameInfo->cAccelEntries = 1;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::Scroll(SIZE scrollExtant)
{
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::OnUIDeactivate(BOOL fUndoable)
{
    m_bUIActivated = false;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceDeactivate(void)
{
    return OnInPlaceDeactivateEx(TRUE);
}

STDMETHODIMP CActiveXCtrl::DiscardUndoState(void)
{
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::DeactivateAndUndo(void)
{
    DUITRACE(_T("AX: CActiveXCtrl::DeactivateAndUndo"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::OnPosRectChange(LPCRECT lprcPosRect)
{
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetWindow(HWND* phwnd)
{
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( m_pOwner->m_hwndHost == NULL ) CreateActiveXWnd();
    if( m_pOwner->m_hwndHost == NULL ) return E_FAIL;
    *phwnd = m_pOwner->m_hwndHost;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ContextSensitiveHelp(BOOL fEnterMode)
{
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnControlInfoChanged(void)      
{
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::LockInPlaceActive(BOOL fLock)       
{
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetExtendedControl(IDispatch** ppDisp)        
{
    if( ppDisp == NULL ) return E_POINTER;   
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( m_pOwner->m_pUnk == NULL ) return E_UNEXPECTED;
    return m_pOwner->m_pUnk->QueryInterface(IID_IDispatch, (LPVOID*) ppDisp);
}

STDMETHODIMP CActiveXCtrl::TransformCoords(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags)       
{
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::TranslateAccelerator(MSG *pMsg, DWORD grfModifiers)
{
    return S_FALSE;
}

STDMETHODIMP CActiveXCtrl::OnFocus(BOOL fGotFocus)
{
    m_bFocused = (fGotFocus == TRUE);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ShowPropertyFrame(void)
{
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::EnumObjects(DWORD grfFlags, IEnumUnknown** ppenum)
{
    if( ppenum == NULL ) return E_POINTER;
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    *ppenum = new CActiveXEnum(m_pOwner->m_pUnk);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::LockContainer(BOOL fLock)
{
    m_bLocked = fLock != FALSE;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ParseDisplayName(IBindCtx *pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut)
{
    return E_NOTIMPL;
}

HRESULT CActiveXCtrl::CreateActiveXWnd()
{
    if( m_pWindow != NULL ) return S_OK;
    m_pWindow = new CActiveXWnd;
    if( m_pWindow == NULL ) return E_OUTOFMEMORY;
    m_pOwner->m_hwndHost = m_pWindow->Init(this, m_pOwner->GetManager()->GetPaintWindow());
    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

HWND CActiveXWnd::Init(CActiveXCtrl* pOwner, HWND hWndParent)
{
    m_pOwner = pOwner;
    UINT uStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    Create(hWndParent, _T("UIActiveX"), uStyle, 0L, 0,0,0,0, NULL);
    return m_hWnd;
}

LPCTSTR CActiveXWnd::GetWindowClassName() const
{
    return _T("ActiveXWnd");
}

void CActiveXWnd::OnFinalMessage(HWND hWnd)
{
    //delete this; // 这里不需要清理，CActiveXUI会清理的
}

void CActiveXWnd::DoVerb(LONG iVerb)
{
    if( m_pOwner == NULL ) return;
    if( m_pOwner->m_pOwner == NULL ) return;
    IOleObject* pUnk = NULL;
    m_pOwner->m_pOwner->GetControl(IID_IOleObject, (LPVOID*) &pUnk);
    if( pUnk == NULL ) return;
    CSafeRelease<IOleObject> RefOleObject = pUnk;
    IOleClientSite* pOleClientSite = NULL;
    m_pOwner->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
    CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;
    pUnk->DoVerb(iVerb, NULL, pOleClientSite, 0, m_hWnd, &m_pOwner->m_pOwner->GetPos());
}

LRESULT CActiveXWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes=0;
    BOOL bHandled = TRUE;
    switch( uMsg ) {
    case WM_PAINT:         lRes = OnPaint(uMsg, wParam, lParam, bHandled); break;
    case WM_SETFOCUS:      lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
    case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
    case WM_ERASEBKGND:    lRes = OnEraseBkgnd(uMsg, wParam, lParam, bHandled); break;
    case WM_MOUSEACTIVATE: lRes = OnMouseActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEWHEEL: break;
    default:
        bHandled = FALSE;
    }
    if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    return lRes;
}

LRESULT CActiveXWnd::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if( m_pOwner->m_pViewObject == NULL ) bHandled = FALSE;
    return 1;
}

LRESULT CActiveXWnd::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    IOleObject* pUnk = NULL;
    m_pOwner->m_pOwner->GetControl(IID_IOleObject, (LPVOID*) &pUnk);
    if( pUnk == NULL ) return 0;
    CSafeRelease<IOleObject> RefOleObject = pUnk;
    DWORD dwMiscStatus = 0;
    pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
    if( (dwMiscStatus & OLEMISC_NOUIACTIVATE) != 0 ) return 0;
    if( !m_pOwner->m_bInPlaceActive ) DoVerb(OLEIVERB_INPLACEACTIVATE);
    bHandled = FALSE;
    return 0;
}

LRESULT CActiveXWnd::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    m_pOwner->m_bFocused = true;
    if( !m_pOwner->m_bUIActivated ) DoVerb(OLEIVERB_UIACTIVATE);
    return 0;
}

LRESULT CActiveXWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    m_pOwner->m_bFocused = false;
    return 0;
}

LRESULT CActiveXWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    PAINTSTRUCT ps = { 0 };
    ::BeginPaint(m_hWnd, &ps);
    ::EndPaint(m_hWnd, &ps);
    return 1;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CActiveXUI::CActiveXUI() : m_pUnk(NULL), m_pControl(NULL), m_hwndHost(NULL), m_bCreated(false), m_bDelayCreate(true)
{
    m_clsid = IID_NULL;
}

CActiveXUI::~CActiveXUI()
{
    ReleaseControl();
}

LPCTSTR CActiveXUI::GetClass() const
{
    return _T("ActiveXUI");
}

LPVOID CActiveXUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("ActiveX")) == 0 ) return static_cast<CActiveXUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

HWND CActiveXUI::GetHostWindow() const
{
    return m_hwndHost;
}

static void PixelToHiMetric(const SIZEL* lpSizeInPix, LPSIZEL lpSizeInHiMetric)
{
#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)
    int nPixelsPerInchX;    // Pixels per logical inch along width
    int nPixelsPerInchY;    // Pixels per logical inch along height
    HDC hDCScreen = ::GetDC(NULL);
    nPixelsPerInchX = ::GetDeviceCaps(hDCScreen, LOGPIXELSX);
    nPixelsPerInchY = ::GetDeviceCaps(hDCScreen, LOGPIXELSY);
    ::ReleaseDC(NULL, hDCScreen);
    lpSizeInHiMetric->cx = MAP_PIX_TO_LOGHIM(lpSizeInPix->cx, nPixelsPerInchX);
    lpSizeInHiMetric->cy = MAP_PIX_TO_LOGHIM(lpSizeInPix->cy, nPixelsPerInchY);
}

void CActiveXUI::SetVisible(bool bVisible)
{
    CControlUI::SetVisible(bVisible);
    if( m_hwndHost != NULL && !m_pControl->m_bWindowless ) 
        ::ShowWindow(m_hwndHost, IsVisible() ? SW_SHOW : SW_HIDE);
}

void CActiveXUI::SetInternVisible(bool bVisible)
{
    CControlUI::SetInternVisible(bVisible);
    if( m_hwndHost != NULL && !m_pControl->m_bWindowless ) 
        ::ShowWindow(m_hwndHost, IsVisible() ? SW_SHOW : SW_HIDE);
}

void CActiveXUI::SetPos(RECT rc)
{
    CControlUI::SetPos(rc);

    if( !m_bCreated ) DoCreateControl();

    if( m_pUnk == NULL ) return;
    if( m_pControl == NULL ) return;

    SIZEL hmSize = { 0 };
    SIZEL pxSize = { 0 };
    pxSize.cx = m_rcItem.right - m_rcItem.left;
    pxSize.cy = m_rcItem.bottom - m_rcItem.top;
    PixelToHiMetric(&pxSize, &hmSize);

    if( m_pUnk != NULL ) {
        m_pUnk->SetExtent(DVASPECT_CONTENT, &hmSize);
    }
	// add by wangshj
	RECT rcPadding = GetPadding();
	rc.left += rcPadding.left;
	rc.top += rcPadding.top;
	rc.right -= rcPadding.right;
	rc.bottom -= rcPadding.bottom;

    if( m_pControl->m_pInPlaceObject != NULL ) {
        CDuiRect rcItem = rc;//m_rcItem;
        if( !m_pControl->m_bWindowless ) rcItem.ResetOffset();
        m_pControl->m_pInPlaceObject->SetObjectRects(&rc, &rc);
    }
    if( !m_pControl->m_bWindowless ) {
        ASSERT(m_pControl->m_pWindow);
        ::MoveWindow(*m_pControl->m_pWindow, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
    }
}

void CActiveXUI::DoPaint(const RECT& rcPaint)
{
    if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

    if( m_pControl != NULL && m_pControl->m_bWindowless && m_pControl->m_pViewObject != NULL )
    {
		HDC hDC = m_pManager->GetRenderCore()->GetRenderHDC();
		RECT rcPadding = GetPadding();
		RECT rc = m_rcItem;;
		rc.left += rcPadding.left;
		rc.top += rcPadding.top;
		rc.right -= rcPadding.right;
		rc.bottom -= rcPadding.bottom;
        m_pControl->m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC, (RECTL*) &rc, (RECTL*) &rc, NULL, NULL); 
		m_pManager->GetRenderCore()->FreeRenderHDC();
    }
}

void CActiveXUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("clsid")) == 0 ) CreateControl(pstrValue);
    else if( _tcscmp(pstrName, _T("modulename")) == 0 ) SetModuleName(pstrValue);
    else if( _tcscmp(pstrName, _T("delaycreate")) == 0 ) SetDelayCreate(_tcscmp(pstrValue, _T("true")) == 0);
    else CControlUI::SetAttribute(pstrName, pstrValue);
}

LRESULT CActiveXUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    if( m_pControl == NULL ) return 0;
    ASSERT(m_pControl->m_bWindowless);
    if( !m_pControl->m_bInPlaceActive ) return 0;
    if( m_pControl->m_pInPlaceObject == NULL ) return 0;
    if( !IsMouseEnabled() && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ) return 0;
    bool bWasHandled = true;
    if( (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR ) {
        // Mouse message only go when captured or inside rect
        DWORD dwHitResult = m_pControl->m_bCaptured ? HITRESULT_HIT : HITRESULT_OUTSIDE;
        if( dwHitResult == HITRESULT_OUTSIDE && m_pControl->m_pViewObject != NULL ) {
            IViewObjectEx* pViewEx = NULL;
            m_pControl->m_pViewObject->QueryInterface(IID_IViewObjectEx, (LPVOID*) &pViewEx);
            if( pViewEx != NULL ) {
                POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                pViewEx->QueryHitPoint(DVASPECT_CONTENT, &m_rcItem, ptMouse, 0, &dwHitResult);
                pViewEx->Release();
            }
        }
        if( dwHitResult != HITRESULT_HIT ) return 0;
        if( uMsg == WM_SETCURSOR ) bWasHandled = false;
		if (uMsg == WM_LBUTTONDOWN)  bWasHandled = false;
		if (uMsg == WM_LBUTTONUP)  bWasHandled = false;
		if (uMsg == WM_MOUSEMOVE)  bWasHandled = false;
    }
    else if( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST ) {
        // Keyboard messages just go when we have focus
        if( !IsFocused() ) return 0;
    }
    else {
        switch( uMsg ) {
        case WM_HELP:
        case WM_CONTEXTMENU:
            bWasHandled = false;
            break;
        default:
            return 0;
        }
    }
    LRESULT lResult = 0;
    HRESULT Hr = m_pControl->m_pInPlaceObject->OnWindowMessage(uMsg, wParam, lParam, &lResult);
    if( Hr == S_OK ) bHandled = bWasHandled;
    return lResult;
}

bool CActiveXUI::IsDelayCreate() const
{
    return m_bDelayCreate;
}

void CActiveXUI::SetDelayCreate(bool bDelayCreate)
{
    if( m_bDelayCreate == bDelayCreate ) return;
    if( bDelayCreate == false ) {
        if( m_bCreated == false && m_clsid != IID_NULL ) DoCreateControl();
    }
    m_bDelayCreate = bDelayCreate;
}

bool CActiveXUI::CreateControl(LPCTSTR pstrCLSID)
{
    CLSID clsid = { 0 };
    OLECHAR szCLSID[100] = { 0 };
#ifndef _UNICODE
    ::MultiByteToWideChar(::GetACP(), 0, pstrCLSID, -1, szCLSID, lengthof(szCLSID) - 1);
#else
    _tcsncpy(szCLSID, pstrCLSID, lengthof(szCLSID) - 1);
#endif
    if( pstrCLSID[0] == '{' ) ::CLSIDFromString(szCLSID, &clsid);
    else ::CLSIDFromProgID(szCLSID, &clsid);
    return CreateControl(clsid);
}

bool CActiveXUI::CreateControl(const CLSID clsid)
{
    ASSERT(clsid!=IID_NULL);
    if( clsid == IID_NULL ) return false;
    m_bCreated = false;
    m_clsid = clsid;
    if( !m_bDelayCreate ) DoCreateControl();
    return true;
}

void CActiveXUI::ReleaseControl()
{
    m_hwndHost = NULL;
    if( m_pUnk != NULL ) 
	{
		IObjectWithSite* pSite = NULL;
		m_pUnk->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);
		if( pSite != NULL ) {
			pSite->SetSite(NULL);
			pSite->Release();
		}
// 		m_pUnk->Close(OLECLOSE_NOSAVE);
// 		m_pUnk->SetClientSite(NULL);
		m_pUnk->Release(); 
		m_pUnk = NULL;
	}
    if( m_pControl != NULL ) {
        m_pControl->m_pOwner = NULL;
        m_pControl->Release();
		m_pControl = NULL;
    }
    m_pManager->RemoveMessageFilter(this);
}

typedef HRESULT (__stdcall *DllGetClassObjectFunc)(REFCLSID rclsid, REFIID riid, LPVOID* ppv); 

bool CActiveXUI::DoCreateControl()
{
    ReleaseControl();
    // At this point we'll create the ActiveX control
    m_bCreated = true;
    IOleControl* pOleControl = NULL;

    HRESULT Hr = -1;
    if( !m_sModuleName.IsEmpty() ) {
        HMODULE hModule = ::LoadLibrary((LPCTSTR)m_sModuleName);
        if( hModule != NULL ) {
            IClassFactory* aClassFactory = NULL;
            DllGetClassObjectFunc aDllGetClassObjectFunc = (DllGetClassObjectFunc)::GetProcAddress(hModule, "DllGetClassObject");
            Hr = aDllGetClassObjectFunc(m_clsid, IID_IClassFactory, (LPVOID*)&aClassFactory);
            if( SUCCEEDED(Hr) ) {
                Hr = aClassFactory->CreateInstance(NULL, IID_IOleObject, (LPVOID*)&pOleControl);
            }
            aClassFactory->Release();
        }
    }
    if( FAILED(Hr) ) {
        Hr = ::CoCreateInstance(m_clsid, NULL, CLSCTX_ALL, IID_IOleControl, (LPVOID*)&pOleControl);
    }
    ASSERT(SUCCEEDED(Hr));
    if( FAILED(Hr) ) return false;
    pOleControl->QueryInterface(IID_IOleObject, (LPVOID*) &m_pUnk);
    pOleControl->Release();
    if( m_pUnk == NULL ) return false;
    // Create the host too
    m_pControl = new CActiveXCtrl();
    m_pControl->m_pOwner = this;
    // More control creation stuff
    DWORD dwMiscStatus = 0;
    m_pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
    IOleClientSite* pOleClientSite = NULL;
    m_pControl->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
    CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;
    // Initialize control
    if( (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) != 0 ) m_pUnk->SetClientSite(pOleClientSite);
    IPersistStreamInit* pPersistStreamInit = NULL;
    m_pUnk->QueryInterface(IID_IPersistStreamInit, (LPVOID*) &pPersistStreamInit);
    if( pPersistStreamInit != NULL ) {
        Hr = pPersistStreamInit->InitNew();
        pPersistStreamInit->Release();
    }
    if( FAILED(Hr) ) return false;
    if( (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) == 0 ) m_pUnk->SetClientSite(pOleClientSite);
    // Grab the view...
    Hr = m_pUnk->QueryInterface(IID_IViewObjectEx, (LPVOID*) &m_pControl->m_pViewObject);
    if( FAILED(Hr) ) Hr = m_pUnk->QueryInterface(IID_IViewObject2, (LPVOID*) &m_pControl->m_pViewObject);
    if( FAILED(Hr) ) Hr = m_pUnk->QueryInterface(IID_IViewObject, (LPVOID*) &m_pControl->m_pViewObject);
    // Activate and done...
    m_pUnk->SetHostNames(OLESTR("UIActiveX"), NULL);
    if( m_pManager != NULL ) m_pManager->SendNotify((CControlUI*)this, _T("showactivex"), 0, 0, false);
    if( (dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME) == 0 ) {
        Hr = m_pUnk->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pOleClientSite, 0, m_pManager->GetPaintWindow(), &m_rcItem);
        //::RedrawWindow(m_pManager->GetPaintWindow(), &m_rcItem, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_FRAME);
    }
    IObjectWithSite* pSite = NULL;
    m_pUnk->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);
    if( pSite != NULL ) {
        pSite->SetSite(static_cast<IOleClientSite*>(m_pControl));
        pSite->Release();
    }
    return SUCCEEDED(Hr);
}

HRESULT CActiveXUI::GetControl(const IID iid, LPVOID* ppRet)
{
    ASSERT(ppRet!=NULL);
    ASSERT(*ppRet==NULL);
    if( ppRet == NULL ) return E_POINTER;
    if( m_pUnk == NULL ) return E_PENDING;
    return m_pUnk->QueryInterface(iid, (LPVOID*) ppRet);
}

CLSID CActiveXUI::GetClisd() const
{
	return m_clsid;
}

CDuiString CActiveXUI::GetModuleName() const
{
    return m_sModuleName;
}

void CActiveXUI::SetModuleName(LPCTSTR pstrText)
{
    m_sModuleName = pstrText;
}

} // namespace DuiLib