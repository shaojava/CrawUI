#include "StdAfx.h"
#include "UIToolTips.h"

namespace UiLib
{
	CToolTipsWnd::CToolTipsWnd(HWND hParent)
	{
		m_hParent = hParent;
		m_pOwner = NULL;
		m_pToolTips = NULL;
		m_szSkinFilePath.Empty();
		m_ptMouse.x = m_ptMouse.y = 0;
		m_WndSize.cx = m_WndSize.cy = 50;
	}

	CToolTipsWnd::~CToolTipsWnd()
	{

	}

	CToolTipsWnd * CToolTipsWnd::CreateToolTipsWnd(HWND hWnd)
	{
		CToolTipsWnd *pToolTipsWnd = new CToolTipsWnd(hWnd);
		return pToolTipsWnd;
	}


	CDuiString CToolTipsWnd::GetSkinFile()
	{
		if(!m_szSkinFilePath.IsEmpty())
			return m_szSkinFilePath;
		else
			return CDuiString(_T("ToolTips.xml"));
	}

	CDuiString CToolTipsWnd::GetSkinFolder()
	{
		return CDuiString(CPaintManagerUI::GetInstancePath()+_T("Skins\\Common\\"));
	}

	LPCTSTR CToolTipsWnd::GetWindowClassName() const
	{
		return _T("ToolTipsWnd");
	}

	LRESULT CToolTipsWnd::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		if(uMsg == WM_KEYDOWN)
		{
			if(wParam == VK_ESCAPE)
			{
				Close();
				return 0;
			}
		}
		return IWindowBase::HandleMessage(uMsg,wParam,lParam);
	}

	void CToolTipsWnd::Init()
	{
		m_pToolTips = static_cast<CToolTipsUI *>(pm.FindSubControlByClass(NULL,_T("ToolTipsUI")));
		if(m_pToolTips)
		{
			if(m_pOwner)
				m_pToolTips->SetTipString(m_pOwner->GetToolTip());
		}
		MONITORINFO oMonitor = {}; 
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
		int nStartX = m_ptMouse.x + 10;
		int nStartY = m_ptMouse.y + 10;
		int nWidht = m_WndSize.cx;
		int nHeight = m_WndSize.cy;

		if(nStartX + nWidht > rcWork.GetWidth())
			nStartX = rcWork.GetWidth() -4;
		if(nStartY +nHeight > rcWork.GetHeight())
			nStartY = rcWork.GetHeight() -4;
		MoveWindow(m_hWnd,nStartX,nStartY,nWidht,nHeight,FALSE);
		IWindowBase::Init();
	}

	void CToolTipsWnd::Notify( TNotifyUI& msg )
	{
		IWindowBase::Notify(msg);
	}

	void CToolTipsWnd::OnFinalMessage( HWND hWnd )
	{
		IWindowBase::OnFinalMessage(hWnd);
		delete this;
	}

	void CToolTipsWnd::SetSkinFile( CDuiString szSkinFilePath )
	{
		m_szSkinFilePath = szSkinFilePath;
	}

	void CToolTipsWnd::Attach( CControlUI *pOwner,SIZE szWndSize,POINT ptMouse )
	{
		m_WndSize.cx = szWndSize.cx;
		m_WndSize.cy = szWndSize.cy;
		m_ptMouse.x = ptMouse.x;
		m_ptMouse.y = ptMouse.y;
		m_pOwner = pOwner;
		if(!IsWindow(m_hWnd))
		{
			Create((m_pOwner == NULL) ? m_hParent : m_pOwner->GetManager()->GetPaintWindow(), NULL, UI_WNDSTYLE_DIALOG, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, CDuiRect());
		}
		else
		{
			/*Ö±½ÓÇÐ»»*/
		}
		ShowWindow(true,false);
	}

	void CToolTipsWnd::Detach()
	{
		Close();
	}

	void CToolTipsWnd::SetTipString( CDuiString szTipString )
	{
		if(m_pToolTips)
			m_pToolTips->SetTipString(szTipString);
	}

	LRESULT CToolTipsWnd::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled )
	{
		return 1;
	}

	

	/////////////////////////////////////////////////////////////
	////////
	REGIST_DUICLASS(CToolTipsUI);

	CToolTipsUI::CToolTipsUI(void)
	{
		m_pTextLable = new CLabelUI();
		m_pTextLable->SetTextStyle(DT_CENTER);
		
		CContainerUI::Add(m_pTextLable);
	}


	CToolTipsUI::~CToolTipsUI(void)
	{
	}

	void CToolTipsUI::SetTipString( CDuiString szString )
	{
		if(m_pTextLable)
			m_pTextLable->SetText(szString);
	}

	LPCTSTR CToolTipsUI::GetClass() const
	{
		return _T("ToolTipsUI");
	}

	LPVOID CToolTipsUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, _T("ToolTips")) == 0 ) return static_cast<CToolTipsUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	void CToolTipsUI::SetFont( int nIndex )
	{
		if(m_pTextLable)
			m_pTextLable->SetFont(nIndex);
	}

}
