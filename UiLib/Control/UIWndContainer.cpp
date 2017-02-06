#include "StdAfx.h"
#include "UIWndContainer.h"

namespace UiLib
{
	REGIST_DUICLASS(CWndContainerUI);

	CWndContainerUI::CWndContainerUI(void)
	{
		m_hWndAttached = NULL;
		m_pWindow = NULL;
	}

	CWndContainerUI::~CWndContainerUI(void)
	{

	}

	void CWndContainerUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if( _tcscmp(pstrName, _T("pos")) == 0 ) {
			RECT rcPos = { 0 };
			LPTSTR pstr = NULL;
			rcPos.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcPos.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcPos.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SIZE szXY = {rcPos.left >= 0 ? rcPos.left : rcPos.right, rcPos.top >= 0 ? rcPos.top : rcPos.bottom};
			SetFixedXY(szXY);
			SetFixedWidth(rcPos.right - rcPos.left);
			SetFixedHeight(rcPos.bottom - rcPos.top);
		}
		else if( _tcscmp(pstrName, _T("relativepos")) == 0 ) {
			SIZE szMove,szZoom;
			LPTSTR pstr = NULL;
			szMove.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szMove.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			szZoom.cx = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			szZoom.cy = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr); 
			SetRelativePos(szMove,szZoom);
		}
		else if( _tcscmp(pstrName, _T("padding")) == 0 ) {
			RECT rcPadding = { 0 };
			LPTSTR pstr = NULL;
			rcPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetPadding(rcPadding);
		}
		else if( _tcscmp(pstrName, _T("leftbordersize")) == 0 ) SetLeftBorderSize(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("topbordersize")) == 0 ) SetTopBorderSize(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("rightbordersize")) == 0 ) SetRightBorderSize(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("bottombordersize")) == 0 ) SetBottomBorderSize(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("width")) == 0 ) SetFixedWidth(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("height")) == 0 ) SetFixedHeight(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("minwidth")) == 0 ) SetMinWidth(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("minheight")) == 0 ) SetMinHeight(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("maxwidth")) == 0 ) SetMaxWidth(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("maxheight")) == 0 ) SetMaxHeight(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("name")) == 0 ) SetName(pstrValue);
		else if( _tcscmp(pstrName, _T("float")) == 0 ) SetFloat(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("visible")) == 0 ) SetVisible(_tcscmp(pstrValue, _T("true")) == 0);
		if(m_pWindow)
			m_pWindow->SetAttribute(pstrName,pstrValue);
	}

	void CWndContainerUI::Event( TEventUI& event )
	{
		if(event.Type == UIEVENT_WINDOWSIZE || event.Type == UIEVENT_WMMOVE)
		{
			RECT rc = m_rcItem;
			RECT rcParent;
			GetWindowRect(m_pManager->GetPaintWindow(),&rcParent);
			::SetWindowPos(m_hWndAttached,m_pManager->GetPaintWindow(), rc.left+rcParent.left, rc.top+rcParent.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
		}
		CControlUI::Event(event);
	}

	BOOL CWndContainerUI::Attach( CWindowWnd *pWindow )
	{
		if(!pWindow) return FALSE;
		m_pWindow = pWindow;
		m_hWndAttached = pWindow->GetHWND();
		SetPos(m_rcItem);
		return TRUE;
	}

	BOOL CWndContainerUI::Attach( HWND hWndNew )
	{
		if (! ::IsWindow(hWndNew))
		{
			return FALSE;
		}

		m_hWndAttached = hWndNew;
		SetPos(m_rcItem);
		return TRUE;
	}

	HWND CWndContainerUI::Detach()
	{
		HWND hWnd = m_hWndAttached;
		m_hWndAttached = NULL;
		return hWnd;
	}

	void CWndContainerUI::SetVisible( bool bVisible /*= true*/ )
	{
		__super::SetVisible(bVisible);
		::ShowWindow(m_hWndAttached, bVisible);
	}

	void CWndContainerUI::SetInternVisible( bool bVisible /*= true*/ )
	{
		__super::SetInternVisible(bVisible);
		::ShowWindow(m_hWndAttached, bVisible);
	}

	void CWndContainerUI::SetPos( RECT rc )
	{
		__super::SetPos(rc);
		if(m_hWndAttached)
		{
			RECT rcParent;
			if(!IsWindowVisible(m_hWndAttached))
				::ShowWindow(m_hWndAttached,SW_SHOW);
			GetWindowRect(m_pManager->GetPaintWindow(),&rcParent);
			::SetWindowPos(m_hWndAttached,m_pManager->GetPaintWindow(), rc.left+rcParent.left, rc.top+rcParent.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
		}
	}

	HWND CWndContainerUI::GetHWND()
	{
		return m_hWndAttached;
	}

}