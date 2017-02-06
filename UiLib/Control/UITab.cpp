#include "stdafx.h"
#include "UITab.h"

namespace UiLib
{
	REGIST_DUICLASS(CTabItemUI);
	REGIST_DUICLASS(CTabBodyUI);
	REGIST_DUICLASS(CTabUI);

	//////////////////////////////////////////////////////////////////////////
	CTabUI::CTabUI() : m_iCurSel(-1)
	{
		m_uLBtnState = UISTATE_DISABLED;
		m_uRBtnState = 0;
		m_nSpeed = 10;
		m_nBtnInterval = 5;
		m_pTabList = new CTabBodyUI();
		CHorizontalLayoutUI::Add(m_pTabList);
	}

	LPCTSTR CTabUI::GetClass() const
	{
		return _T("TabUI");
	}

	LPVOID CTabUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("TabUI")) == 0 ) return static_cast<CTabUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	CControlUI* CTabUI::GetItemAt(int iIndex) const
	{
		return m_pTabList->GetItemAt(iIndex);
	}

	int	CTabUI::GetItemIndex(CControlUI* pControl) const
	{
		return m_pTabList->GetItemIndex(pControl);
	}

	int CTabUI::GetCount() const
	{
		return m_pTabList->GetCount();
	}

	bool CTabUI::Add(CControlUI* pControl)
	{
		bool ret = false;
		if (_tcscmp(pControl->GetClass(), DUI_CTR_TABBODY) == 0)
		{
			ret = CHorizontalLayoutUI::Add(pControl);
		}
		else
		{
			ret = m_pTabList->Add(pControl);
		}
		if( !ret ) return ret;

		if(m_iCurSel == -1 && pControl->IsVisible())
		{
			m_iCurSel = m_pTabList->GetItemIndex(pControl);
		}

		return ret;
	}

	bool CTabUI::AddAt(CControlUI* pControl, int iIndex)
	{
		bool ret = false;
		if (m_pTabList)
		{
			bool ret = m_pTabList->AddAt(pControl, iIndex);
			if( !ret ) return ret;

			if(m_iCurSel == -1 && pControl->IsVisible())
			{
				m_iCurSel = m_pTabList->GetItemIndex(pControl);
			}
			else if( m_iCurSel != -1 && iIndex <= m_iCurSel )
			{
				m_iCurSel += 1;
			}
		}
		return ret;
	}

	bool CTabUI::Remove(CControlUI* pControl)
	{
		if( pControl == NULL) return false;
		bool ret = false;
		if (m_pTabList)
		{
			int index = GetItemIndex(pControl);
			ret = m_pTabList->Remove(pControl);
			if( !ret ) return false;

			if( m_iCurSel == index)
			{
				if( GetCount() > 0 )
				{
					m_iCurSel=0;
					GetItemAt(m_iCurSel)->SetVisible(true);
				}
				else
					m_iCurSel=-1;
				NeedParentUpdate();
			}
			else if( m_iCurSel > index )
			{
				m_iCurSel -= 1;
			}
		}
		return ret;
	}

	void CTabUI::RemoveAll(bool bUpdate)
	{
		m_iCurSel = -1;
		m_pTabList->RemoveAll(bUpdate);
		if(bUpdate)
			NeedParentUpdate();
	}

	int CTabUI::GetCurSel() const
	{
		return m_iCurSel;
	}

	bool CTabUI::SelectItem(int iIndex)
	{
		if( iIndex < 0 || iIndex >= GetCount() )
		{
			CTabItemUI* pControl = (CTabItemUI*)GetItemAt(m_iCurSel);
			if (pControl)
			{
				m_iCurSel = -1;
				pControl->Selected(false);
			}
			return true;
		}
		if( iIndex == m_iCurSel ) return true;

		int iOldSel = m_iCurSel;
		m_iCurSel = iIndex;
		for( int it = 0; it < GetCount(); it++ )
		{
			if( it == iIndex ) {
				CTabItemUI* pControl = (CTabItemUI*)GetItemAt(it);
				if (pControl)
				{
					pControl->Selected(true);
				}
			}
		}
		NeedParentUpdate();
		return true;
	}

	bool CTabUI::SelectItem( CControlUI* pControl )
	{
		int iIndex= GetItemIndex(pControl);
		if (iIndex==-1)
			return false;
		else
			return SelectItem(iIndex);
	}

	void CTabUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("selectedid")) == 0 ) SelectItem(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("leftnormalimage")) == 0) SetLNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("leftforeimage")) == 0) SetLForeImage(pstrValue);
		else if( _tcscmp(pstrName, _T("lefthotimage")) == 0) SetLHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("lefthotforeimage")) == 0) SetLHotForeImage(pstrValue);
		else if( _tcscmp(pstrName, _T("leftpushedimage")) == 0) SetLPushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("leftpushedforeimage")) == 0) SetLPushedForeImage(pstrValue);
		else if( _tcscmp(pstrName, _T("leftfocusedimage")) == 0) SetLFocusedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("leftdisabledimage")) == 0) SetLDisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("rightnormalimage")) == 0) SetRNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("rightforeimage")) == 0) SetRForeImage(pstrValue);
		else if( _tcscmp(pstrName, _T("righthotimage")) == 0) SetRHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("righthotforeimage")) == 0) SetRHotForeImage(pstrValue);
		else if( _tcscmp(pstrName, _T("rightpushedimage")) == 0) SetRPushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("rightpushedforeimage")) == 0) SetRPushedForeImage(pstrValue);
		else if( _tcscmp(pstrName, _T("rightfoucsedimage")) == 0) SetRFocusedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("rightdisabledimage")) == 0) SetRDisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("btninterval")) == 0) SetBtnInterval(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("interval")) == 0) SetInterval(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("scrollspeed")) == 0) SetScrollSpeed(_ttoi(pstrValue));
		return CContainerUI::SetAttribute(pstrName, pstrValue);
	}
 
	void CTabUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		m_pTabList->SetPos(rc);

		int nLength = GetItemsLength();
		int nWidth = rc.right - rc.left;
		if (nLength > nWidth)
		{
			int nBtnWidth = 0;
			if (!m_sLNormalImage.IsEmpty())
			{
				TCHAR szName[MAX_PATH] = {0};
				GetImageFileName(m_sLNormalImage, szName);
				if (szName)
				{
					TImageInfo* pInfo = m_pManager->GetRenderCore()->LoadImage(szName);
					if (pInfo)
					{
						nBtnWidth = pInfo->nX / 4;
					}
				}
			}
			m_rcLBtn.left = rc.left;
			m_rcLBtn.right = rc.left + nBtnWidth + m_nBtnInterval;
			m_rcLBtn.top = rc.top;
			m_rcLBtn.bottom = rc.bottom;

			m_rcRBtn.left = nWidth - nBtnWidth;
			m_rcRBtn.right = nWidth;
			m_rcRBtn.top = rc.top;
			m_rcRBtn.bottom = rc.bottom;

			m_uLBtnState = UISTATE_DISABLED;
			m_uRBtnState = 0;

			RECT rcContainer = {m_rcLBtn.right, m_rcRBtn.top, m_rcRBtn.left - m_nBtnInterval, m_rcRBtn.bottom};
			m_pTabList->SetPos(rcContainer);
		}
		else
		{
			if (m_pTabList)
			{
				RECT rcBtn = {0};
				m_rcLBtn = rcBtn;
				m_rcRBtn = rcBtn;
				m_pTabList->SetPos(rc);
			}
		}
	}

	//得到图像文件文件名
	void CTabUI::GetImageFileName(LPCTSTR pstrName, TCHAR* pstrRes)
	{
		CDuiString sItem;
		CDuiString sValue;
		LPCTSTR sImageName = NULL;
		LPCTSTR pStrImage = pstrName;
		while( *pStrImage != _T('\0') ) {
			sItem.Empty();
			sValue.Empty();
			while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
			while( *pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' ') ) {
				LPTSTR pstrTemp = ::CharNext(pStrImage);
				while( pStrImage < pstrTemp) {
					sItem += *pStrImage++;
				}
			}
			while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
			if( *pStrImage++ != _T('=') ) break;
			while( *pStrImage > _T('\0') && *pStrImage <= _T(' ') ) pStrImage = ::CharNext(pStrImage);
			if( *pStrImage++ != _T('\'') ) break;
			while( *pStrImage != _T('\0') && *pStrImage != _T('\'') ) {
				LPTSTR pstrTemp = ::CharNext(pStrImage);
				while( pStrImage < pstrTemp) {
					sValue += *pStrImage++;
				}
			}
			if( *pStrImage++ != _T('\'') ) break;
			if( !sValue.IsEmpty() ) {
				if( sItem == _T("file") || sItem == _T("res") ) {
					wcscpy(pstrRes, sValue.GetData());
					break;
				}
			}
		}
	}

	void CTabUI::PaintBkColor()
	{
		//画Tab项
		CContainerUI::PaintBkColor();
		////画按钮
		//DrawLButton(hDC);
		//DrawRButton(hDC);
	}

	void CTabUI::PaintBkImage()
	{
		//画Tab项
		CContainerUI::PaintBkImage();
		////画按钮
		DrawLButtonImage();
		DrawRButtonImage();
	}

	//绘制左按钮
	void CTabUI::DrawLButton()
	{
		CUIRenderCore *pRenderCore = NULL;
		if(m_pManager)
			pRenderCore = m_pManager->GetRenderCore();
		if(!pRenderCore) return;
		if (m_uLBtnState & UISTATE_DISABLED)
			pRenderCore->DrawColor(m_rcLBtn, 0xFF000000);
		else if (m_uLBtnState & UISTATE_PUSHED)
			pRenderCore->DrawColor(m_rcLBtn, 0xFF00FF00);
		else if (m_uLBtnState & UISTATE_HOT)
			pRenderCore->DrawColor(m_rcLBtn, 0xFF0000FF);
		else
			pRenderCore->DrawColor(m_rcLBtn, 0xFFFF0000);
	}

	//绘制右按钮
	void CTabUI::DrawRButton()
	{
		CUIRenderCore *pRenderCore = NULL;
		if(m_pManager)
			pRenderCore = m_pManager->GetRenderCore();
		if(!pRenderCore) return;
		if (m_uRBtnState & UISTATE_DISABLED)
			pRenderCore->DrawColor(m_rcRBtn, 0xFF000000);
		else if (m_uRBtnState & UISTATE_PUSHED)
			pRenderCore->DrawColor(m_rcRBtn, 0xFF00FF00);
		else if (m_uRBtnState & UISTATE_HOT)
			pRenderCore->DrawColor(m_rcRBtn, 0xFF0000FF);
		else
			pRenderCore->DrawColor(m_rcRBtn, 0xFFFF0000);
	}

	//绘制左按钮图像
	void CTabUI::DrawLButtonImage()
	{
		CUIRenderCore *pRenderCore = NULL;
		if(m_pManager)
			pRenderCore = m_pManager->GetRenderCore();
		if(!pRenderCore) return;

		if (m_uLBtnState & UISTATE_DISABLED)
		{
			if (!m_sLDisabledImage.IsEmpty())
			{
				pRenderCore->DrawImageString(m_pManager, m_rcLBtn, m_rcLBtn, m_sLDisabledImage, NULL);
			}
		}
		else if (m_uLBtnState & UISTATE_PUSHED)
		{
			if (!m_sLPushedImage.IsEmpty())
			{
				pRenderCore->DrawImageString(m_pManager, m_rcLBtn, m_rcLBtn, m_sLPushedImage, NULL);
				if (!m_sLPushedForeImage)
				{
					pRenderCore->DrawImageString(m_pManager, m_rcLBtn, m_rcLBtn, m_sLPushedForeImage, NULL);
				}
			}
		}
		else if (m_uLBtnState & UISTATE_HOT)
		{
			if (!m_sLHotImage.IsEmpty())
			{
				pRenderCore->DrawImageString(m_pManager, m_rcLBtn, m_rcLBtn, m_sLHotImage, NULL);
				if (!m_sLHotForeImage)
				{
					pRenderCore->DrawImageString(m_pManager, m_rcLBtn, m_rcLBtn, m_sLHotForeImage, NULL);
				}
			}
		}
		else if (m_uLBtnState & UISTATE_FOCUSED)
		{
			if (!m_sLFocusedImage.IsEmpty())
			{
				pRenderCore->DrawImageString(m_pManager, m_rcLBtn, m_rcLBtn, m_sLFocusedImage, NULL);
				if (!m_sLFocusedImage)
				{
					pRenderCore->DrawImageString(m_pManager, m_rcLBtn, m_rcLBtn, m_sLHotForeImage, NULL);
				}
			}
		}
		else
		{
			if (!m_sLNormalImage.IsEmpty())
			{
				pRenderCore->DrawImageString(m_pManager, m_rcLBtn, m_rcLBtn, m_sLNormalImage, NULL);
				if (!m_sLForeImage)
				{
					pRenderCore->DrawImageString(m_pManager, m_rcLBtn, m_rcLBtn, m_sLForeImage, NULL);
				}
			}
		}
	}

	//绘制右按钮图像
	void CTabUI::DrawRButtonImage()
	{
		CUIRenderCore *pRenderCore = NULL;
		if(m_pManager)
			pRenderCore = m_pManager->GetRenderCore();
		if(!pRenderCore) return;

		if (m_uRBtnState & UISTATE_DISABLED)
		{
			if (!m_sRDisabledImage.IsEmpty())
			{
				pRenderCore->DrawImageString(m_pManager, m_rcRBtn, m_rcRBtn, m_sRDisabledImage, NULL);
			}
		}
		else if (m_uRBtnState & UISTATE_PUSHED)
		{
			if (!m_sRPushedImage.IsEmpty())
			{
				pRenderCore->DrawImageString(m_pManager, m_rcRBtn, m_rcRBtn, m_sRPushedImage, NULL);
				if (!m_sRPushedForeImage)
				{
					pRenderCore->DrawImageString(m_pManager, m_rcRBtn, m_rcRBtn, m_sRPushedForeImage, NULL);
				}
			}
		}
		else if (m_uRBtnState & UISTATE_HOT)
		{
			if (!m_sRHotImage.IsEmpty())
			{
				pRenderCore->DrawImageString(m_pManager, m_rcRBtn, m_rcRBtn, m_sRHotImage, NULL);
				if (!m_sRHotForeImage)
				{
					pRenderCore->DrawImageString(m_pManager, m_rcRBtn, m_rcRBtn, m_sRHotForeImage, NULL);
				}
			}
		}
		else if (m_uRBtnState & UISTATE_FOCUSED)
		{
			if (!m_sRFocusedImage.IsEmpty())
			{
				pRenderCore->DrawImageString(m_pManager, m_rcRBtn, m_rcRBtn, m_sRFocusedImage, NULL);
				if (!m_sRFocusedImage)
				{
					pRenderCore->DrawImageString(m_pManager, m_rcRBtn, m_rcRBtn, m_sRHotForeImage, NULL);
				}
			}
		}
		else
		{
			if (!m_sRNormalImage.IsEmpty())
			{
				pRenderCore->DrawImageString(m_pManager, m_rcRBtn, m_rcRBtn, m_sRNormalImage, NULL);
				if (!m_sRForeImage)
				{
					pRenderCore->DrawImageString(m_pManager, m_rcRBtn, m_rcRBtn, m_sRForeImage, NULL);
				}
			}
		}
	}

	void CTabUI::DoEvent(TEventUI& event)
	{
		if(event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK)
		{
			if (::PtInRect(&m_rcLBtn, event.ptMouse))
			{
				if ((m_uLBtnState & UISTATE_DISABLED) == 0)
				{
					m_uLBtnState |= UISTATE_PUSHED | UISTATE_CAPTURED;
					m_pManager->SetTimer(this,1000,50);
					Invalidate();
				}
			}
			else if (::PtInRect(&m_rcRBtn, event.ptMouse))
			{
				if ((m_uRBtnState & UISTATE_DISABLED) == 0)
				{
					m_uRBtnState |= UISTATE_PUSHED | UISTATE_CAPTURED;
					Invalidate();
					m_pManager->SetTimer(this,1000,50);
				}
			}
		}
		if (event.Type == UIEVENT_BUTTONUP)
		{
			if( (m_uLBtnState & UISTATE_CAPTURED) != 0 ) 
			{
				if( ::PtInRect(&m_rcLBtn, event.ptMouse) )
				{
					m_uLBtnState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
					m_pManager->KillTimer(this, 1000);
					Invalidate();
				}
			}
			else if( (m_uRBtnState & UISTATE_CAPTURED) != 0 ) 
			{
				if( ::PtInRect(&m_rcRBtn, event.ptMouse) )
				{
					m_uRBtnState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
					Invalidate();
					m_pManager->KillTimer(this, 1000);
				}
			}
		}
		if (event.Type == UIEVENT_MOUSEENTER)
		{
			if( IsEnabled() ) {
				if (::PtInRect(&m_rcLBtn, event.ptMouse))
				{
					m_uLBtnState |= UISTATE_HOT;
					Invalidate();
				}
				else if (::PtInRect(&m_rcRBtn, event.ptMouse))
				{
					m_uRBtnState |= UISTATE_HOT;
					Invalidate();
				}
			}
		}
		if (event.Type == UIEVENT_MOUSELEAVE)
		{
			if( IsEnabled() ) {
				if (m_uLBtnState & UISTATE_HOT)
				{
					m_uLBtnState &= ~UISTATE_HOT;
				}
				if (m_uRBtnState & UISTATE_HOT)
				{
					m_uRBtnState &= ~UISTATE_HOT;
				}
				Invalidate();
			}
		}
		if (event.Type == UIEVENT_TIMER)
		{
			if (m_uLBtnState & UISTATE_PUSHED)
			{
				ScrollLeft();
				if (m_uRBtnState & UISTATE_DISABLED)
				{
					m_uRBtnState = 0;
					Invalidate();
				}
			}
			if (m_uRBtnState & UISTATE_PUSHED)
			{
				ScrollRight();
				if (m_uLBtnState & UISTATE_DISABLED)
				{
					m_uLBtnState = 0;
					Invalidate();
				}
			}
		}
		if (event.Type == UIEVENT_WINDOWSIZE)
		{
			RECT rc = GetPos();
			SetPos(rc);
		}
		if (event.Type == UIEVENT_KEYDOWN)
		{
			int nRes = GetItemVisible(2);
			nRes = -1;
		}
	}

	//得到所有项目的长度
	int CTabUI::GetItemsLength()
	{
		CControlUI* pControl = NULL;
		int nLength = 0;
		for (int i = 0; i < GetCount(); i++)
		{
			pControl = GetItemAt(i);
			if (pControl)
			{
				nLength += pControl->GetWidth();
			}
		}
		return nLength;
	}

	//是否显示左侧按钮
	BOOL CTabUI::IsShowLBtn()
	{
		return TRUE;
	}

	//是否显示右侧按钮
	BOOL CTabUI::IsShowRBtn()
	{
		return TRUE;
	}

	//左滚动
	void CTabUI::ScrollLeft()
	{
		int i = 0;
		int nCount = GetCount();
		int nSpeed = m_nSpeed;
		CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(0));;
		if (pControl)
		{
			RECT rc = pControl->GetPos();
			int nTmp = rc.left + nSpeed;
			if (nTmp >= m_rcLBtn.right)
			{
				nSpeed = m_rcLBtn.right - rc.left;
			}
		}
		for (int i = nCount - 1; i >= 0; i--)
		{
			pControl = static_cast<CControlUI*>(GetItemAt(i));
			if (pControl)
			{
				RECT rc = pControl->GetPos();
				rc.left += nSpeed;
				rc.right += nSpeed;
				pControl->SetPos(rc);
				if (i == 0)
				{
					if (rc.left == m_rcLBtn.right)
					{
						m_uLBtnState = 0;
						m_uLBtnState |= UISTATE_DISABLED;
						Invalidate();
					}
				}
			}
		}
	}

	//右滚动
	void CTabUI::ScrollRight()
	{
		int i = 0;
		int nCount = GetCount();
		int nSpeed = m_nSpeed;
		CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(nCount - 1));;
		if (pControl)
		{
			RECT rc = pControl->GetPos();
			int nTmp = rc.right - nSpeed;
			if (nTmp <= m_rcRBtn.left - m_nBtnInterval)
			{
				nSpeed = rc.right - m_rcRBtn.left + m_nBtnInterval;
			}
		}
		for (int i = 0; i < nCount; i++)
		{
			pControl = static_cast<CControlUI*>(GetItemAt(i));
			if (pControl)
			{
				RECT rc = pControl->GetPos();
				rc.left -= nSpeed;
				rc.right -= nSpeed;
				pControl->SetPos(rc);
				if (i == nCount - 1)
				{
					if (rc.right == m_rcRBtn.left - m_nBtnInterval)
					{
						m_uRBtnState = 0;
						m_uRBtnState |= UISTATE_DISABLED;
						Invalidate();
					}
				}
			}
		}
	}

	//左侧按钮图像
	LPCTSTR CTabUI::GetLNoramlImage()
	{
		return m_sLNormalImage;
	}

	void CTabUI::SetLNormalImage(LPCTSTR pstrName)
	{
		m_sLNormalImage = pstrName;
		Invalidate();
	}

	LPCTSTR CTabUI::GetLForeImage()
	{
		return m_sLForeImage;
	}

	void CTabUI::SetLForeImage(LPCTSTR pstrName)
	{
		m_sLForeImage = pstrName;
	}

	LPCTSTR CTabUI::GetLHotImage()
	{
		return m_sLHotImage;
	}

	void CTabUI::SetLHotImage(LPCTSTR pstrName)
	{
		m_sLHotImage = pstrName;
		Invalidate();
	}

	LPCTSTR CTabUI::GetLHotForeImage()
	{
		return m_sLHotForeImage;
	}

	void CTabUI::SetLHotForeImage(LPCTSTR pstrName)
	{
		m_sLHotForeImage = pstrName;
		Invalidate();
	}

	LPCTSTR CTabUI::GetLPushedImage()
	{
		return m_sLPushedImage;
	}

	void CTabUI::SetLPushedImage(LPCTSTR pstrName)
	{
		m_sLPushedImage = pstrName;
		Invalidate();
	}

	LPCTSTR CTabUI::GetLPushedForeImage()
	{
		return m_sLPushedForeImage;
	}

	void CTabUI::SetLPushedForeImage(LPCTSTR pstrName)
	{
		m_sLPushedForeImage = pstrName;
		Invalidate();
	}

	LPCTSTR CTabUI::GetLFocusedImage()
	{
		return m_sLFocusedImage;
	}

	void CTabUI::SetLFocusedImage(LPCTSTR pstrName)
	{
		m_sLFocusedImage = pstrName;
		Invalidate();
	}

	LPCTSTR CTabUI::GetLDisabledImage()
	{
		return m_sLDisabledImage;
	}

	void CTabUI::SetLDisabledImage(LPCTSTR pstrName)
	{
		m_sLDisabledImage = pstrName;
		Invalidate();
	}

	//右侧按钮图像
	LPCTSTR CTabUI::GetRNoramlImage()
	{
		return m_sRNormalImage;
	}

	void CTabUI::SetRNormalImage(LPCTSTR pstrName)
	{
		m_sRNormalImage = pstrName;
		Invalidate();
	}

	LPCTSTR CTabUI::GetRForeImage()
	{
		return m_sRForeImage;
	}

	void CTabUI::SetRForeImage(LPCTSTR pstrName)
	{
		m_sRForeImage = pstrName;
	}

	LPCTSTR CTabUI::GetRHotImage()
	{
		return m_sRHotImage;
	}

	void CTabUI::SetRHotImage(LPCTSTR pstrName)
	{
		m_sRHotImage = pstrName;
		Invalidate();
	}

	LPCTSTR CTabUI::GetRHotForeImage()
	{
		return m_sRHotForeImage;
	}

	void CTabUI::SetRHotForeImage(LPCTSTR pstrName)
	{
		m_sRHotForeImage = pstrName;
		Invalidate();
	}

	LPCTSTR CTabUI::GetRPushedImage()
	{
		return m_sRPushedImage;
	}

	void CTabUI::SetRPushedImage(LPCTSTR pstrName)
	{
		m_sRPushedImage = pstrName;
		Invalidate();
	}

	LPCTSTR CTabUI::GetRPushedForeImage()
	{
		return m_sRPushedForeImage;
	}

	void CTabUI::SetRPushedForeImage(LPCTSTR pstrName)
	{
		m_sRPushedForeImage = pstrName;
		Invalidate();
	}

	LPCTSTR CTabUI::GetRFocusedImage()
	{
		return m_sRFocusedImage;
	}

	void CTabUI::SetRFocusedImage(LPCTSTR pstrName)
	{
		m_sRFocusedImage = pstrName;
		Invalidate();
	}

	LPCTSTR CTabUI::GetRDisabledImage()
	{
		return m_sRDisabledImage;
	}

	void CTabUI::SetRDisabledImage(LPCTSTR pstrName)
	{
		m_sRDisabledImage = pstrName;
		Invalidate();
	}

	//按钮间隔
	int CTabUI::GetBtnInterval()
	{
		return m_nBtnInterval;
	}

	void CTabUI::SetBtnInterval(int nInterval)
	{
		m_nBtnInterval = nInterval;
		SetPos(GetPos());
		Invalidate();
	}

	//Tab间隔
	int CTabUI::GetInterval()
	{
		CControlUI* pControl = GetItemAt(0);
		if (pControl)
		{
			RECT rc = pControl->GetPadding();
			return rc.left;
		}
		return 0;
	}

	void CTabUI::SetInterval(int nInterval)
	{
		CControlUI* pControl = NULL;
		RECT rc = {nInterval, 0, 0, 0};
		for (int i = 1; i < GetCount(); i++)
		{
			pControl = GetItemAt(i);
			if (pControl)
			{
				pControl->SetPadding(rc);
			}
		}
		//SetPos(GetPos());
	}

	//滚动速度
	int CTabUI::GetScrollSpeed()
	{
		return m_nSpeed;
	}

	void CTabUI::SetScrollSpeed(int nSpeed)
	{
		m_nSpeed = nSpeed;
	}

	//得到Tab项相关的URL
	CDuiString CTabUI::GetItemUrl(int nIndex)
	{
		CDuiString strUrl;
		CTabItemUI* pTabItem = (CTabItemUI*)GetItemAt(nIndex);
		if (pTabItem)
		{
			strUrl = pTabItem->GetUrl();
		}
		return strUrl;
	}

	//设置Tab项相关的URL
	void CTabUI::SetItemUrl(int nIndex, LPCTSTR lpszUrl)
	{
		CTabItemUI* pTabItem = (CTabItemUI*)GetItemAt(nIndex);
		if (pTabItem)
		{
			pTabItem->SetUrl(lpszUrl);
		}
	}

	//得到Tab项相关的Text
	CDuiString CTabUI::GetItemText(int nIndex)
	{
		CDuiString strText;
		CTabItemUI* pTabItem = (CTabItemUI*)GetItemAt(nIndex);
		if (pTabItem)
		{
			strText = pTabItem->GetText();
		}
		return strText;
	}

	//设置Tab项相关的Text
	void CTabUI::SetItemText(int nIndex, LPCTSTR lpszText)
	{
		CTabItemUI* pTabItem = (CTabItemUI*)GetItemAt(nIndex);
		if (pTabItem)
		{
			pTabItem->SetText(lpszText);
		}
	}

	//得到Tab项相关的ForeImage
	LPCTSTR CTabUI::GetItemForeImage(int nIndex)
	{
		CTabItemUI* pTabItem = (CTabItemUI*)GetItemAt(nIndex);
		if (pTabItem)
		{
			return pTabItem->GetForeImage();
		}
		return NULL;
	}

	//设置Tab项相关的ForeImage
	void CTabUI::SetItemForeImage(int nIndex, LPCTSTR pstrName)
	{
		CTabItemUI* pTabItem = (CTabItemUI*)GetItemAt(nIndex);
		if (pTabItem)
		{
			pTabItem->SetForeImage(pstrName);
		}
	}

	//得到某项的可见状态
	int CTabUI::GetItemVisible(int nIndex)
	{
		if (m_pTabList)
		{
			CControlUI* pControl = GetItemAt(nIndex);
			if (pControl)
			{
				RECT rcContainer = m_pTabList->GetPos();
				RECT rcDst = {0};
				RECT rc = pControl->GetPos();
				if (::IntersectRect(&rcDst, &rc, &rcContainer))
				{
					if (EqualRect(&rcDst,&rc))
					{
						if (rc.left == rcContainer.left)
						{
							return 2;
						}
						if (rc.right == rcContainer.right)
						{
							return 3;
						}
						return 0;
					}
					else
					{
						return 1;
					}
				}
				else
				{
					return 4;
				}
			}
		}
		return -1;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	CTabBodyUI::CTabBodyUI()
	{

	}

	LPCTSTR CTabBodyUI::GetClass() const
	{
		return _T("TabBodyUI");
	}

	LPVOID CTabBodyUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_TABBODY) == 0)	return this;
		return CHorizontalLayoutUI::GetInterface(pstrName);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//

	CTabItemUI::CTabItemUI()
	{
		//SetSelectedBkColor(0xFFD3E7F3);
		//SetBkColor(0xFF1E83BB);
	}

	LPCTSTR CTabItemUI::GetClass() const
	{
		return _T("TabItemUI");
	}

	LPVOID CTabItemUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, DUI_CTR_TABITEM) == 0)	return this;
		return CControlUI::GetInterface(pstrName);
	}

	void CTabItemUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcscmp(pstrName, _T("url")) == 0)	return SetUrl(pstrValue);
		COptionUI::SetAttribute(pstrName, pstrValue);
	}

	void CTabItemUI::Selected(bool bSelected)
	{
		if (bSelected)
		{
			CTabBodyUI* pTabBody = (CTabBodyUI*)GetParent();
			if (pTabBody)
			{
				CTabUI* pTabUI = (CTabUI*)pTabBody->GetParent();
				if (pTabUI)
				{
					pTabUI->SelectItem(this);
				}
			}
		}
		COptionUI::Selected(bSelected);
	}
}