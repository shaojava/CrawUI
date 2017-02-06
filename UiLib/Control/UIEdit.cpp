#include "StdAfx.h"
#include "UIEdit.h"
#include "../Utils/DuiSystemInfo.h"
#include <regex>
#include <atlstr.h>

namespace UiLib
{
	REGIST_DUICLASS(CEditUI);

	class CEditWnd : public CWindowWnd
	{
	public:
		CEditWnd();

		void Init(CEditUI* pOwner,BOOL bPop);
		RECT CalPos();
		void SetForbidWords(CDuiString szForbidWords);
		void SetPwdMode(bool bPwd);
		LPTSTR GetWindowText();
		LPCTSTR GetWindowClassName() const;
		LPCTSTR GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		void UpdatePos(bool bPop);
		void BlockCopy(bool bBlock = true);
		void BlockRBtnDown(bool bBlock = true);
		
	protected:
		CEditUI* m_pOwner;
		HBRUSH m_hBkBrush;
		CDuiString m_szForbidWords;
		bool m_bFloatOnly;
		bool m_bInit;
		bool m_bBlockCopy;
		bool m_bBlockRBtnDown;
	};

	void CEditWnd::BlockCopy(bool bBlock)
	{
		m_bBlockCopy = bBlock;
	}

	void CEditWnd::BlockRBtnDown(bool bBlock)
	{
		m_bBlockRBtnDown = bBlock;
	}
	CEditWnd::CEditWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false)
	{
		m_bBlockCopy = false;
		m_bFloatOnly = false;
		m_bBlockRBtnDown = false;
	}

	void CEditWnd::Init(CEditUI* pOwner,BOOL bPop)
	{
		if(!pOwner) return;
		m_pOwner = pOwner;
		RECT rcPos = CalPos();
		UINT uStyle;
		OSVERSIONINFO os;
		CDuiSystemInfo::GetSystemInfo(os);
		if(bPop)
		{
			if( m_pOwner->IsPasswordMode() && os.dwMajorVersion == 5 && os.dwMinorVersion == 2)
				uStyle = WS_POPUP | WS_CHILD | ES_AUTOHSCROLL;
			else
				uStyle = WS_POPUP | WS_CHILD | ES_AUTOHSCROLL | ES_MULTILINE;

			RECT rcWnd={0};
			::GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWnd);
			rcPos.left += rcWnd.left;
			rcPos.right += rcWnd.left;
			rcPos.top += rcWnd.top;
			rcPos.bottom += rcWnd.top;
		}
		else
		{
			if( m_pOwner->IsPasswordMode() && os.dwMajorVersion == 5 && os.dwMinorVersion == 2 )
				uStyle = WS_CHILD | ES_AUTOHSCROLL;
				
			else
				uStyle = WS_CHILD | ES_AUTOHSCROLL | ES_MULTILINE;
		}

		Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);

		HFONT hFont=NULL;
		int iFontIndex=m_pOwner->GetFont();
		if (iFontIndex!=-1)
			hFont=m_pOwner->GetManager()->GetFont(iFontIndex)->hFont;
		if (hFont==NULL)
			hFont=m_pOwner->GetManager()->GetDefaultFontInfo()->hFont;
		SetWindowFont(m_hWnd, hFont, TRUE);
		if(m_pOwner)
		{
			RECT rcEdit = {0,0,rcPos.right-rcPos.left,rcPos.bottom-rcPos.top};
			LONG lEditHeight = m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->tm.tmHeight;
			if(lEditHeight < rcEdit.bottom)
			{
				int nV = (rcEdit.bottom-lEditHeight)/2;
				rcEdit.top += nV;
			}
			RECT rcTextPaddint = m_pOwner->GetTextPadding();

			if(rcTextPaddint.left)
				rcEdit.left = 1+rcTextPaddint.left;
			else
				rcEdit.left = 1;

			rcEdit.right -= rcTextPaddint.right;
			rcEdit.bottom -= rcTextPaddint.bottom;
			Edit_SetRectNoPaint(m_hWnd,&rcEdit);
		}

		Edit_LimitText(m_hWnd, m_pOwner->GetMaxChar());
		if( m_pOwner->IsPasswordMode() ) Edit_SetPasswordChar(m_hWnd, m_pOwner->GetPasswordChar());
		Edit_SetText(m_hWnd, m_pOwner->GetText());
		Edit_SetModify(m_hWnd, FALSE);
		SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
		Edit_Enable(m_hWnd, m_pOwner->IsEnabled() == true);
		Edit_SetReadOnly(m_hWnd, m_pOwner->IsReadOnly() == true);
		//Styls
		LONG styleValue = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
		styleValue |= pOwner->GetWindowStyls();
		::SetWindowLongPtr(GetHWND(), GWL_STYLE, styleValue);
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		::SetFocus(m_hWnd);
		if(m_pOwner->IsFloatOnly())
			m_bFloatOnly = true;
		m_szForbidWords = m_pOwner->GetForbidWords();
		BlockCopy(m_pOwner->IsCopyBlock());
		BlockRBtnDown(m_pOwner->isRButtonBlock());
		m_bInit = true;
		m_pOwner->GetManager()->SendNotify(m_pOwner, _T("eidtinit"));
	}
	void CEditWnd::SetForbidWords(CDuiString szForbidWords)
	{
		m_szForbidWords = szForbidWords;
	}

	RECT CEditWnd::CalPos()
	{
		CDuiRect rcPos = m_pOwner->GetPos();
		RECT rcInset = m_pOwner->GetEditInset();
		rcPos.left += rcInset.left+1;
		rcPos.top += rcInset.top+1;
		rcPos.right -= rcInset.right+1;
		rcPos.bottom -= rcInset.bottom+1;
		return rcPos;
	}

	LPCTSTR CEditWnd::GetWindowClassName() const
	{
		return _T("EditWnd");
	}

	LPTSTR CEditWnd::GetWindowText()
	{
		if(m_hWnd)
		{
			int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
			LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
			ASSERT(pstr);
			if( pstr == NULL ) return 0;
			::GetWindowText(m_hWnd, pstr, cchLen);
			return pstr;
		}
		return NULL;
	}

	LPCTSTR CEditWnd::GetSuperClassName() const
	{
		return WC_EDIT;
	}

	void CEditWnd::OnFinalMessage(HWND /*hWnd*/)
	{
		m_pOwner->Invalidate();
		// Clear reference and die
		if( m_hBkBrush != NULL )
		{
			::DeleteObject(m_hBkBrush);
			m_hBkBrush = NULL;
		}
		m_pOwner->m_pWindow = NULL;
		m_hWnd = NULL;
		delete this;
	}

	std::string Unicode2StdString( LPCTSTR szString )
	{
		if(!szString) return "";
		int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
		int nChar = WideCharToMultiByte(codepage, 0, szString, -1, 0, 0, 0, 0);
		int nSize = nChar * sizeof(char) + 1;
		char* pchBuffer = new char[nSize];

		nChar = WideCharToMultiByte(codepage, 0, szString, -1, pchBuffer, nSize, 0, 0);
		if( nChar == 0 )
		{
			delete[] pchBuffer;
			return NULL;
		}
		pchBuffer[nChar]= 0;
		std::string szRes(pchBuffer);
		delete[] pchBuffer;
		pchBuffer = NULL;
		return szRes;
	}

	LRESULT CEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;

		if(uMsg == WM_RBUTTONDOWN)
		{
			if(m_bBlockRBtnDown)
			{
				bHandled = TRUE;
				return 0;
			}
			bHandled = FALSE;
		}
		else if( uMsg == WM_KILLFOCUS )
		{
			DWORD dwStyle = GetWindowLongPtr(m_hWnd,GWL_STYLE);
			if((dwStyle & WS_POPUP) == 0)
 				lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
		}
		else if( uMsg == OCM_COMMAND ) {
			if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE ) lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
			else if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE ) {
				RECT rcClient;
				::GetClientRect(m_hWnd, &rcClient);
				::InvalidateRect(m_hWnd, &rcClient, FALSE);
			}
		}
		else if(uMsg == WM_PASTE)
		{
			if(m_bBlockCopy)
				return FALSE;
			else
				bHandled = FALSE;
		}
		else if( uMsg == WM_KEYDOWN ) {
			TEventUI event = { 0 };
			event.Type = UIEVENT_KEYDOWN;
			event.chKey = (TCHAR)wParam;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			m_pOwner->Event(event);

			if (TCHAR(wParam) == VK_RETURN )
			{
				m_pOwner->GetManager()->HandleIDOKEvent();
				m_pOwner->GetManager()->SendNotify(m_pOwner, _T("editreturn"));
			}
			else
				bHandled = FALSE;
		}
		else if(uMsg == WM_CHAR)
		{
			CDuiString szRegex = m_pOwner->GetRegex();
			if(!szRegex.IsEmpty() && wParam != 8 && !(GetKeyState(VK_CONTROL) & 0x8000))
			{
				int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
				std::string strRex = Unicode2StdString(szRegex);
				const std::regex pattern(strRex.c_str());

				CDuiString szInputed; 
				if(m_pOwner) szInputed = m_pOwner->GetText();
				int nS,nE;
				::SendMessage(m_hWnd,EM_GETSEL,(WPARAM)(&nS),(LPARAM)(&nE));
				
				string szMatch = Unicode2StdString(szInputed);

				string szFront = "";
				string szTail = "";
				int nInputSize = szInputed.GetLength();
				if(nE-nS < nInputSize)
				{
					if(nS > 0)
						szFront = szMatch.substr(0,nS);
					if(nE < nInputSize)
						szTail = szMatch.substr(nE,std::string::npos);
				}

				TCHAR * tMatchs = new TCHAR[2];
				tMatchs[0] = wParam;
				tMatchs[1] = 0;
				int nChar = WideCharToMultiByte(codepage, 0, tMatchs, -1, 0, 0, 0, 0);
				int nSize = nChar * sizeof(char) + 1;
				char* pchBuffer = new char[nSize];
				nChar = WideCharToMultiByte(codepage, 0, tMatchs, -1, pchBuffer, nSize, 0, 0);
				pchBuffer[nChar] = 0;
				SAFE_DELETE(tMatchs);
				string strInput = pchBuffer;
				szMatch = szFront+strInput+szTail;
				SAFE_DELETE(pchBuffer);
				bool match = std::regex_match(szMatch.c_str(), pattern);
				if(!match)
					return 0;
				
			}

			if(m_bFloatOnly && wParam != '-' && wParam != 8 && wParam != '.' && (wParam > '9'|| wParam < '0'))
				return 0;
			if(!m_szForbidWords.IsEmpty())
			{
				if(m_szForbidWords.Find((TCHAR)wParam) >= 0)
				{
					m_pOwner->GetManager()->SendNotify(m_pOwner, _T("forbidwords"));
					return 0;
				}
			}
			TEventUI event = { 0 };
			event.Type = UIEVENT_CHAR;
			event.chKey = (TCHAR)wParam;
			event.wKeyState = MapKeyState();
			event.dwTimestamp = ::GetTickCount();
			event.pSender = m_pOwner;
			m_pOwner->Event(event);
			if(event.bHandle)
				return lRes;
			bHandled = FALSE;
		}
		else if( uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC ) 
		{
			::SetBkMode((HDC)wParam, TRANSPARENT);
			DWORD dwTextColor = m_pOwner->GetTextColor();
			::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
			if( m_hBkBrush == NULL )
			{
				DWORD clrColor = !m_pOwner->IsReadOnly()?m_pOwner->GetNativeEditBkColor():m_pOwner->GetDisabledBkColor();
				if(clrColor != 0)
					m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
				else 
					return (LRESULT)::HBRUSH(GetStockObject(HOLLOW_BRUSH));
			}
			return (LRESULT)m_hBkBrush;
		}
		else bHandled = FALSE;
		if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT CEditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		m_pOwner->m_pWindow = NULL;
		PostMessage(WM_CLOSE);
		return lRes;
	}

	LRESULT CEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if( !m_bInit ) return 0;
		if( m_pOwner == NULL ) return 0;
		// Copy text back
		int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
		LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
		ASSERT(pstr);
		if( pstr == NULL ) return 0;
		::GetWindowText(m_hWnd, pstr, cchLen);
		m_pOwner->m_sText = pstr;
		TEventUI event = { 0 };
		event.Type = UIEVENT_TEXTCHANGE;
		event.pSender = m_pOwner;
		event.dwTimestamp = ::GetTickCount();
		m_pOwner->Event(event);
		m_pOwner->GetManager()->SendNotify(m_pOwner, _T("textchanged"));
		return 0;
	}

	void CEditWnd::SetPwdMode( bool bPwd )
	{
		if(m_hWnd&&bPwd)
			Edit_SetPasswordChar(m_hWnd, m_pOwner->GetPasswordChar());
	}

	void CEditWnd::UpdatePos( bool bPop )
	{
		RECT rcPos = CalPos();
		if(bPop)
		{
			RECT rcWnd={0};
			::GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWnd);
			rcPos.left += rcWnd.left;
			rcPos.right += rcWnd.left;
			rcPos.top += rcWnd.top;
			rcPos.bottom += rcWnd.top;
		}
		::SetWindowPos(m_hWnd,m_pOwner->GetManager()->GetPaintWindow(),rcPos.left,rcPos.top,rcPos.right-rcPos.left,rcPos.bottom-rcPos.top,SWP_NOZORDER);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CEditUI::CEditUI() : m_pWindow(NULL), m_uMaxChar(-1), m_bReadOnly(false),m_bFloatOnly(false),
		m_bPasswordMode(false), m_cPasswordChar(_T('*')), m_uButtonState(0), 
		m_dwEditbkColor(0xFFFFFFFF), m_iWindowStyls(0),m_sTipValueColor(0xFFBAC0C5)
	{
		SetBorderSize(1);
		SetBorderColor(0xFFBAC0C5);
		SetTextPadding(CDuiRect(4, 3, 4, 3));
		SetBkColor(0xFFFFFFFF);
		m_rcEditInset.left = m_rcEditInset.right = m_rcEditInset.top = m_rcEditInset.bottom = 0;
		m_bBlockCopy = false;
		m_bBlockRButtonDown = false;
		m_bNeedTab = true;
		m_bNeedCursor = true;
	}

	CEditUI::~CEditUI()
	{
		//添加析构函数,来关闭窗口否则在消息循环中,还会像这个没关闭的窗口发送消息
		//而这个窗口在处理一些消息时使用到了父类,父类已经被析构了,所以窗口只是在OnKillFocus时Destroy显然会崩溃;
		//要注意的是,必须SendMessage立刻关闭,PostMessage会直接返回完成析构,而m_pWindow要在消息队列到WM_CLOSE时才关闭,也会调用到已经被析构完的父类造成崩溃;
		if(m_pWindow&&IsWindow(m_pWindow->GetHWND()))
			m_pWindow->SendMessage(WM_CLOSE);
	}

	LPCTSTR CEditUI::GetClass() const
	{
		return _T("EditUI");
	}

	LPVOID CEditUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("Edit")) == 0 ) return static_cast<CEditUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	UINT CEditUI::GetControlFlags() const
	{
		if( !IsEnabled() ) return 0;

		return IsWantCursor() | IsWantTab();
	}

	void CEditUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}
		if( event.Type == UIEVENT_TIMER && event.pSender == this && m_pWindow )
			return OnTimer(event.wParam );
		if( event.Type == UIEVENT_SETCURSOR && IsEnabled() )
		{
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			return;
		}
		if( event.Type == UIEVENT_WINDOWSIZE )
		{
			if( m_pWindow != NULL ) m_pManager->SetFocusNeeded(this);
		}
		if(event.Type == UIEVENT_TEXTCHANGE)
		{
		}
		if( event.Type == UIEVENT_SCROLLWHEEL )
		{
			if( m_pWindow != NULL ) return;
		}
		if( event.Type == UIEVENT_SETFOCUS && IsEnabled() ) 
		{
			if( m_pWindow ) return;
			m_pWindow = new CEditWnd();
			ASSERT(m_pWindow);
			m_pWindow->Init(this,m_pManager->GetRenderCore()->IsWindowLayered());
			SetSel(0,-1);
			Invalidate();
		}
		if( event.Type == UIEVENT_KILLFOCUS && IsEnabled() ) 
		{
			m_bFocused = false;
			if(m_RegularCheckStr.GetLength() > 0)
			{
				if(!MatchRegular(true)){
					GetManager()->SendNotify(this,_T("OnEditRegex"),IDNO);
					event.Type = UIEVENT_SETFOCUS;
					DoEvent(event);
					return;
				}
				else
					GetManager()->SendNotify(this,_T("OnEditRegex"),IDYES);
			}
			if(m_pManager)
				m_pManager->SendNotify(this,_T("EditDestory"));
			SetInternVisible(false);
			Invalidate();
			return;
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) 
		{
			if( IsEnabled() ) 
			{
				RECT rcEdit = m_rcItem;
				rcEdit.left += m_rcEditInset.left;
				rcEdit.right -= m_rcEditInset.right;
				rcEdit.top += m_rcEditInset.top;
				rcEdit.bottom -= m_rcEditInset.bottom;
				if( PtInRect(&rcEdit, event.ptMouse) )
				{
					GetManager()->ReleaseCapture();
					if( m_pWindow == NULL )
					{
						m_pWindow = new CEditWnd();
						ASSERT(m_pWindow);
						m_pWindow->Init(this,m_pManager->GetRenderCore()->IsWindowLayered());

						
						int nSize = GetWindowTextLength(*m_pWindow);
						if( nSize == 0 )
							nSize = 1;

						Edit_SetSel(*m_pWindow, 0, nSize);
						
					}
					else if( m_pWindow != NULL )
					{
						int nSize = GetWindowTextLength(*m_pWindow);
						if( nSize == 0 )
							nSize = 1;
						Edit_SetSel(*m_pWindow, 0, nSize);
					}
				}
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE ) 
		{
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP ) 
		{
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		CLabelUI::DoEvent(event);
	}

	void CEditUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	void CEditUI::SetText(LPCTSTR pstrText)
	{
		m_sText = pstrText;
		if(m_pWindow)
			Edit_SetText(*m_pWindow, m_sText);
		else
			CLabelUI::SetText(pstrText);
		Invalidate();

	}

	CDuiString CEditUI::GetText() const
	{
		return CLabelUI::GetText();
	}

	LPTSTR CEditUI::GetWindowText()
	{
		if(m_pWindow)
			return m_pWindow->GetWindowText();
		return NULL;
	}

	void CEditUI::SetMaxChar(int uMax)
	{
		m_uMaxChar = uMax;
		if( m_pWindow != NULL ) Edit_LimitText(*m_pWindow, m_uMaxChar);
	}

	int CEditUI::GetMaxChar()
	{
		return m_uMaxChar;
	}

	void CEditUI::SetReadOnly(bool bReadOnly)
	{
		if( m_bReadOnly == bReadOnly ) return;

		m_bReadOnly = bReadOnly;
		if( m_pWindow != NULL ) Edit_SetReadOnly(*m_pWindow, m_bReadOnly);
		Invalidate();
	}

	bool CEditUI::IsReadOnly() const
	{
		return m_bReadOnly;
	}

	void CEditUI::SetNumberOnly(bool bNumberOnly)
	{
		if( bNumberOnly )
		{
			m_iWindowStyls |= ES_NUMBER;
		}
		else
		{
			m_iWindowStyls |= ~ES_NUMBER;
		}
	}

	void CEditUI::SetFloatOnly(bool bFloatOnly)
	{
		m_bFloatOnly = bFloatOnly;
	}

	bool CEditUI::IsFloatOnly() const
	{
		return m_bFloatOnly;
	}

	bool CEditUI::IsNumberOnly() const
	{
		return m_iWindowStyls&ES_NUMBER ? true:false;
	}

	int CEditUI::GetWindowStyls() const 
	{
		return m_iWindowStyls;
	}

	void CEditUI::SetPasswordMode(bool bPasswordMode)
	{
		if( m_bPasswordMode == bPasswordMode ) return;
		m_bPasswordMode = bPasswordMode;
		if(m_pWindow)
			m_pWindow->SetPwdMode(m_bPasswordMode);
		Invalidate();
	}

	bool CEditUI::IsPasswordMode() const
	{
		return m_bPasswordMode;
	}

	void CEditUI::SetPasswordChar(TCHAR cPasswordChar)
	{
		if( m_cPasswordChar == cPasswordChar ) return;
		m_cPasswordChar = cPasswordChar;
		if( m_pWindow != NULL ) Edit_SetPasswordChar(*m_pWindow, m_cPasswordChar);
		Invalidate();
	}

	TCHAR CEditUI::GetPasswordChar() const
	{
		return m_cPasswordChar;
	}

	LPCTSTR CEditUI::GetNormalImage()
	{
		return m_sNormalImage;
	}

	void CEditUI::SetNormalImage(LPCTSTR pStrImage)
	{
		m_sNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CEditUI::GetHotImage()
	{
		return m_sHotImage;
	}

	void CEditUI::SetHotImage(LPCTSTR pStrImage)
	{
		m_sHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CEditUI::GetFocusedImage()
	{
		return m_sFocusedImage;
	}

	void CEditUI::SetFocusedImage(LPCTSTR pStrImage)
	{
		m_sFocusedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CEditUI::GetDisabledImage()
	{
		return m_sDisabledImage;
	}

	void CEditUI::SetDisabledImage(LPCTSTR pStrImage)
	{
		m_sDisabledImage = pStrImage;
		Invalidate();
	}

	void CEditUI::SetNativeEditBkColor(DWORD dwBkColor)
	{
		m_dwEditbkColor = dwBkColor;
		Invalidate();
	}

	DWORD CEditUI::GetNativeEditBkColor() const
	{
		return m_dwEditbkColor;
	}

	int CEditUI::GetSel(int &nStart,int& nEnd)
	{
		if(m_pWindow)
			return SendMessage(*m_pWindow,EM_GETSEL,(WPARAM)(&nStart),(LPARAM)(&nEnd));
		return -1;
	}
	
	void CEditUI::SetSel(long nStartChar, long nEndChar)
	{
		if( m_pWindow != NULL ) Edit_SetSel(*m_pWindow, nStartChar,nEndChar);
	}

	void CEditUI::SetSelAll()
	{
		SetSel(0,-1);
	}

	void CEditUI::SetReplaceSel(LPCTSTR lpszReplace)
	{
		if( m_pWindow != NULL ) Edit_ReplaceSel(*m_pWindow, lpszReplace);
	}

	bool CEditUI::MatchRegular(bool isShowMsg/* = true*/)
	{
		try
		{
			if(!m_RegularCheckStr.GetLength())
				return true;
			
			try
			{
#ifndef _UNICODE
				wchar_t* mSrcRegularCheck = new wchar_t[_tclen(GetRegularCheck())*2+1]();
				wsprintfW(mSrcRegularCheck,L"%s",GetRegularCheck());
				std::tr1::wregex regExpress(mSrcRegularCheck);

				wchar_t* mSrcVal = new wchar_t[GetText().GetLength()*2+1]();
				wsprintfW(mSrcVal,L"%s",GetText().GetData());
				std::wstring mSrcText = mSrcVal;

				delete mSrcRegularCheck;
				mSrcRegularCheck = NULL;
				delete mSrcVal;
				mSrcVal = NULL;

				if(!regex_match(mSrcText,regExpress))
					goto MatchFailed;
				else
					return true;
#else
				char* mSrcRegularCheck = new char[strlen(CW2A(GetRegularCheck()))*2+1]();
				sprintf(mSrcRegularCheck,"%s",CW2A(GetRegularCheck()));
				std::tr1::regex regExpress(mSrcRegularCheck);

				char* mSrcVal = new char[GetText().GetLength()*2+1]();
				sprintf(mSrcVal,"%s",CW2A(GetText().GetData()));
				std::string mSrcText = mSrcVal;

				delete mSrcRegularCheck;
				mSrcRegularCheck = NULL;
				delete mSrcVal;
				mSrcVal = NULL;

				if(!regex_match(mSrcText,regExpress))
					goto MatchFailed;
				else
					return true;
#endif
			}
			catch(...)
			{
				goto MatchFailed;
			}

		}
		catch (...)
		{
			throw "CEditUI::MatchRegular";
		}
MatchFailed:
		{
			if(m_RegularTipStr.GetLength() > 0 && isShowMsg)
				MessageBox(GetManager()->GetPaintWindow(),m_RegularTipStr.GetData(),NULL,MB_OK);

			SetText(m_RegluarSrcText.GetData());
			return false;
		}
	}


	void CEditUI::SetRegularCheck( LPCTSTR pRegularCheckStr )
	{
		try
		{
			m_RegularCheckStr = pRegularCheckStr;
			Invalidate();
		}
		catch(...)
		{
			throw "CEditUI::SetRegularCheck";
		}
	}

	LPCTSTR CEditUI::GetRegularCheck()
	{
		try
		{
			return m_RegularCheckStr;
		}
		catch(...)
		{
			throw "CEditUI::GetRegularCheck";
		}
	}

	void CEditUI::SetRegularTip( LPCTSTR pRegularTipStr )
	{
		try
		{
			m_RegularTipStr = pRegularTipStr;
			Invalidate();
		}
		catch(...)
		{
			throw "CEditUI::SetRegularTip";
		}
	}

	LPCTSTR CEditUI::GetRegularTip()
	{
		try
		{
			return m_RegularTipStr;
		}
		catch(...)
		{
			CTextUI p;
			throw "CEditUI::GetRegularTip";
		}
	}

	void CEditUI::SetMatchCase( bool bMatchCase /*= false*/ )
	{
		try
		{
			m_bMatchCase = bMatchCase;
		}
		catch(...)
		{
			throw "CEditUI::SetMatchCase";
		}
	}

	bool CEditUI::GetMatchCase()
	{
		try
		{
			return m_bMatchCase;
		}
		catch(...)
		{
			throw "CEditUI::GetMatchCase";
		}
	}

	void CEditUI::SetTipValue( LPCTSTR pStrTipValue )
	{
		try
		{
			m_sSrcTipValue	= pStrTipValue;
		}
		catch(...)
		{
			throw "CEditUI::SetTipValue";
		}
	}

	void CEditUI::SetTipValueColor( LPCTSTR pStrColor )
	{
		try
		{
			if( *pStrColor == _T('#')) pStrColor = ::CharNext(pStrColor);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pStrColor, &pstr, 16);

			m_sTipValueColor = clrColor;
		}
		catch(...)
		{
			throw "CEditUI::SetTipValueColor";
		}
	}

	DWORD CEditUI::GetTipValueColor()
	{
		try
		{
			return m_sTipValueColor;
		}
		catch(...)
		{
			throw "CEditUI::GetTipValueColor";
		}
	}

	UiLib::CDuiString CEditUI::GetTipValue()
	{
		try
		{
			return m_sSrcTipValue;
		}
		catch(...)
		{
			throw "CEditUI::GetTipValue";
		}
	}

	LPCTSTR CEditUI::GetSrcTipValue()
	{
		try
		{
			return m_sSrcTipValue.GetData();
		}
		catch(...)
		{
			throw "CEditUI::GetSrcTipValue";
		}
	}

	void CEditUI::SetPos(RECT rc)
	{
		if(m_pWindow&&IsWindow(m_pWindow->GetHWND()))
		{
			CControlUI::SetPos(rc);
			m_pWindow->UpdatePos(m_pManager->GetRenderCore()->IsWindowLayered());
		}
		else
			CControlUI::SetPos(rc);
	}

	void CEditUI::SetVisible(bool bVisible)
	{
		CControlUI::SetVisible(bVisible);
		if( !IsVisible() && m_pWindow != NULL ) m_pManager->SetFocus(NULL);
	}

	void CEditUI::SetInternVisible(bool bVisible)
	{
		if( !IsVisible() && m_pWindow != NULL ) m_pManager->SetFocus(NULL);
	}

	SIZE CEditUI::EstimateSize(SIZE szAvailable)
	{
		if( m_cxyFixed.cy == 0 ) return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 6);
		return CControlUI::EstimateSize(szAvailable);
	}

	void CEditUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("editinset")) == 0 ) {
			LPTSTR pstr = NULL;
			m_rcEditInset.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			m_rcEditInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			m_rcEditInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			m_rcEditInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		}
		else if( _tcscmp(pstrName, _T("readonly")) == 0 ) SetReadOnly(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("floatonly")) == 0 ) SetFloatOnly(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("numberonly")) == 0 ) SetNumberOnly(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("password")) == 0 ) SetPasswordMode(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("maxchar")) == 0 ) SetMaxChar(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("disabledimage")) == 0 ) SetDisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("tipvalue")) == 0 ) SetTipValue(pstrValue);
		else if( _tcscmp(pstrName, _T("tipvaluecolor")) == 0 ) SetTipValueColor(pstrValue);
		else if( _tcscmp(pstrName, _T("enabletimer")) == 0) SetEnableTimer(_tcscmp(pstrValue,_T("true")) == 0);
		else if( _tcscmp(pstrName, _T("timerdelay")) == 0) SetTimerDelay(_tcstoul(pstrValue,NULL,10));
		else if( _tcscmp(pstrName, _T("regularcheck")) == 0) SetRegularCheck(pstrValue);
		else if( _tcscmp(pstrName, _T("regulartip")) == 0) SetRegularTip(pstrValue);
		else if (_tcscmp(pstrName, _T("tipimage")) == 0)m_sTipImage = pstrValue;
		else if( _tcscmp(pstrName, _T("editregex")) == 0 )m_szEditRegex = pstrValue;
		else if( _tcscmp(pstrName, _T("nativebkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetNativeEditBkColor(clrColor);
		}
		else CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CEditUI::PaintBkColor()
	{
		if(!IsEnabled() || IsReadOnly())
			m_pManager->GetRenderCore()->DrawColor(m_rcItem,GetAdjustColor(m_dwDisabledBkColor));
		else 
			CLabelUI::PaintBkColor();
	}

	void CEditUI::PaintStatusImage()
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;


		if( (m_uButtonState & UISTATE_DISABLED) != 0 ) 
		{
			if( !m_sDisabledImage.IsEmpty() ) 
			{
				if( !DrawImage(m_sDisabledImage) ) 
					m_sDisabledImage.Empty();
				else if( !m_sTipImage.IsEmpty() ) 
				{
					if( !DrawImage(m_sTipImage) ) m_sTipImage.Empty();
					return;
				}
			}
		}
		else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
			if( !m_sFocusedImage.IsEmpty() ) {
				if( !DrawImage(m_sFocusedImage) ) 
					m_sFocusedImage.Empty();
				else if( !m_sTipImage.IsEmpty() ) 
				{
					if( !DrawImage(m_sTipImage) ) m_sTipImage.Empty();
					return;
				}
			}
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( !m_sHotImage.IsEmpty() ) {
				if( !DrawImage(m_sHotImage) ) 
					m_sHotImage.Empty();
				else  if( !m_sTipImage.IsEmpty() ) 
				{
					if( !DrawImage(m_sTipImage) ) m_sTipImage.Empty();
					return;
				}
			}
		}

		if( !m_sNormalImage.IsEmpty() ) {
			if( !DrawImage(m_sNormalImage) ) 
				m_sNormalImage.Empty();
			else  if( !m_sTipImage.IsEmpty() ) 
			{
				if( !DrawImage(m_sTipImage) ) m_sTipImage.Empty();
				return;
			}
		}
		
	}

	void CEditUI::PaintText()
	{
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		DWORD mCurTextColor = m_dwTextColor;
		CDuiString sText = m_sText;
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();


		if(!m_pWindow&&m_sText.IsEmpty()&&!m_sSrcTipValue.IsEmpty())
		{
			mCurTextColor = m_sTipValueColor;
			sText = m_sSrcTipValue;
		}
		else
		{
			if( m_bPasswordMode ) 
			{
				sText.Empty();
				LPCTSTR p = m_sText.GetData();
				while( *p != _T('\0') ) {
					sText += m_cPasswordChar;
					p = ::CharNext(p);
				}
			}
		}

		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left+m_rcEditInset.left;
		rc.right -= m_rcTextPadding.right+m_rcEditInset.right;
		rc.top += m_rcTextPadding.top+m_rcEditInset.top;
		rc.bottom -= m_rcTextPadding.bottom+m_rcEditInset.bottom;
		if( IsEnabled() ) {
			m_pManager->GetRenderCore()->DrawText(m_pManager, rc, sText, mCurTextColor, \
				m_iFont, DT_VCENTER | DT_SINGLELINE | m_uTextStyle,m_bTextGlow);
		}
		else {
			m_pManager->GetRenderCore()->DrawText(m_pManager, rc, sText, m_dwDisabledTextColor, \
				m_iFont, DT_VCENTER | DT_SINGLELINE | m_uTextStyle,m_bTextGlow);
		}
	}

	void CEditUI::SetEnableTimer( bool bEnableTime )
	{
		try
		{
			m_bEnableTime = bEnableTime;
		}
		catch(...)
		{
			throw "CEditUI::SetEnableTimer";
		}
	}
	void CEditUI::SetForbidWords(CDuiString szForbidWords)
	{
		m_szForbidWords = szForbidWords;
	}
	CDuiString CEditUI::GetForbidWords()
	{
		return m_szForbidWords;
	}

	bool CEditUI::GetEnableTimer()
	{
		try
		{
			return m_bEnableTime;
		}
		catch(...)
		{
			throw "CEditUI::GetEnableTimer";
		}
	}

	void CEditUI::SetTimerDelay( UINT nDelay )
	{
		try
		{
			m_uDelay = nDelay;

			if(!m_bEnableTime)
				return;

			GetManager()->KillTimer(this);
			GetManager()->SetTimer(this,1650,m_uDelay);
		}
		catch(...)
		{
			throw "CEditUI::SetTimerDelay";
		}
	}

	void CEditUI::OnTimer( UINT iTimerID )
	{
		if(_tcscmp(m_sCheckVal.GetData(),GetText().GetData()) != 0)
		{
			m_sCheckVal = GetText();
			GetManager()->SendNotify(this,_T("OnEditTimer"));
		}
	}

	UINT CEditUI::GetTimerDelay()
	{
		return m_uDelay;
	}

	UiLib::CDuiString CEditUI::GetRegex()
	{
		return m_szEditRegex;
	}

	void CEditUI::SetRegex( CDuiString szRegex )
	{
		m_szEditRegex = szRegex;
	}

	RECT CEditUI::GetEditInset()
	{
		return m_rcEditInset;
	}

}
