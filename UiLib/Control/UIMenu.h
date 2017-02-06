#ifndef __UIMENU_H__
#define __UIMENU_H__

#pragma once

namespace UiLib {

struct ContextMenuParam
{
	WPARAM  wParam;
	HWND    hWnd;
};

enum MenuAlignment
{
	eMenuAlignment_Left   = 1,
	eMenuAlignment_Top    = 1 << 2,
	eMenuAlignment_Right  = 1 << 3,
	eMenuAlignment_Bottom = 1 << 4,
};

typedef class ObserverImpl<BOOL, ContextMenuParam> ContextMenuObserver;
typedef class ReceiverImpl<BOOL, ContextMenuParam> ContextMenuReceiver;

extern ContextMenuObserver s_context_menu_observer;

class CMenuElementUI;
class CMenuWnd;
class CListUI;
class UILIB_API CMenuUI : public CListUI
{
public:
	CMenuUI();
	~CMenuUI();

public:
    LPCTSTR GetClass() const;
    LPVOID  GetInterface(LPCTSTR pstrName);

	virtual void DoEvent(TEventUI& event);

	void SetBirthElement(CMenuElementUI * pControl);
	void SetBelongMenuWnd(CMenuWnd *pBelongWnd);
	CMenuWnd* GetBelongMenuWnd();
	CMenuElementUI *GetBirthElement();
	void DoInit();
    virtual bool Add(CControlUI* pControl);
    virtual bool AddAt(CControlUI* pControl, int iIndex);

    virtual int  GetItemIndex(CControlUI* pControl) const;
    virtual bool SetItemIndex(CControlUI* pControl, int iIndex);
    virtual bool Remove(CControlUI* pControl);
	SIZE EstimateNeedSize();
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
protected:
	CMenuElementUI *m_pBirthElement;
	CMenuWnd *m_pBelongWnd;
};

class CMenuElementUI;
class UILIB_API CMenuWnd : public CWindowWnd, public ContextMenuReceiver,public INotifyUI
{
public:
	CMenuWnd(HWND hParent = NULL);
	static CMenuWnd * CMenuWnd::CreateDuiMenu(HWND hWnd);
public:
    void    Init(CMenuElementUI* pOwner, LPCTSTR xml, LPCTSTR pSkinType, POINT point);
    void    Init(CMenuElementUI* pOwner, UINT idxml, LPCTSTR pSkinType, POINT point);

    LPCTSTR GetWindowClassName() const;
    void    OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void Notify(TNotifyUI& msg);

	BOOL    Receive(ContextMenuParam param);

	void AddDisableItmes(char* strItems);
	bool CheckDisableItems(LPCTSTR szItemLocation);
	void SetNotifier(INotifyUI *pNotifier);
	INotifyUI *GetNotifier();
	void SetCurLocation(CDuiString szLocation);
public:
    LRESULT OnCreate(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle);
    LRESULT OnClose(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle);
    LRESULT OnKeyDown(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle);
    LRESULT OnKillFocus(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle);
    LRESULT OnRButtonUp(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle);
    LRESULT OnRButtonDown(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle);
    LRESULT OnContextMenu(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle);
    LRESULT OnRButtonDbClick(UINT uMsg,WPARAM wParam,LPARAM lParam,BOOL& bHandle);

public:
	HWND            m_hParent;
	POINT           m_BasedPoint;
	CDuiString      m_xml;
	CDuiString      m_sType;
	CDuiString      m_szCurLocation;
    CPaintManagerUI m_pm;
    CMenuElementUI *    m_pOwner;
    CMenuUI*        m_pLayout;
	INotifyUI *m_pNotifier;
	std::vector<string> m_vDisableItems;
};

class CListContainerElementUI;
class UILIB_API CMenuElementUI : public CListContainerElementUI
{
	friend class CMenuWnd;
public:
    CMenuElementUI();
	~CMenuElementUI();

public:
    LPCTSTR GetClass() const;
    LPVOID  GetInterface(LPCTSTR pstrName);

    void DoPaint(const RECT& rcPaint);

	void DrawItemText(const RECT& rcItem);
	void DrawIcon();

	bool Activate();

	void DoEvent(TEventUI& event);

	CMenuWnd* GetMenuWnd();

	bool Add(CControlUI* pControl);

	void CreateMenuWnd();

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void SetLocation(CDuiString szLocation);
	CDuiString GetLocation();

	void SetNormalImage(LPCTSTR pStrImage);
	void SetHotImage(LPCTSTR pStrImage);
protected:
	CMenuWnd* m_pWindow;
	CDuiString m_szLocation;
	CDuiImage m_sNormalImage;
	CDuiImage m_sHotImage;
};

} // namespace DuiLib

#endif // __UIMENU_H__
