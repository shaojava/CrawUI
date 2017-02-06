#include "stdafx.h"
#include "UIHorizontalLayout.h"

namespace UiLib
{
	REGIST_DUICLASS(CHorizontalLayoutUI);

	CHorizontalLayoutUI::CHorizontalLayoutUI() : m_iSepWidth(0), m_uButtonState(0), m_bImmMode(false)
	{
		ptLastMouse.x = ptLastMouse.y = 0;
		m_rcNewPos.left = m_rcNewPos.right = m_rcNewPos.bottom = m_rcNewPos.top = 0;
		m_bNeedCursor = true;
	}

	LPCTSTR CHorizontalLayoutUI::GetClass() const
	{
		return _T("HorizontalLayoutUI");
	}

	LPVOID CHorizontalLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("HorizontalLayout")) == 0 ) return static_cast<CHorizontalLayoutUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	UINT CHorizontalLayoutUI::GetControlFlags() const
	{
		if( IsEnabled() && m_iSepWidth != 0 ) return IsWantCursor();
		else return 0;
	}

	SIZE CHorizontalLayoutUI::EstimateNeedSize()
	{
		//计算这个容器所需要的最小尺寸;
		//如果没有子控件则直接是自己的EstimateSize;
		if(m_items.GetSize() == 0)
			return EstimateSize(m_cxyFixed);
		
		RECT rc = m_rcItem;
		rc.left += m_rcInset.left;
		rc.right -= m_rcInset.right;

		int cxFixed = 0;
		int cyFixed = 0;
		int nEstimateNum = 0;
		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			//估算每个控件的大小,所需要的大小是拓展大小与实际大小的和;
			SIZE sz = pControl->EstimateSize(m_cxyFixed);
			//如果返回-1,不添加到Fixed大小里面去,有尺寸才加;
			if( sz.cx > 0)
			{
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				cxFixed += sz.cx;
			}
			//因为是水平容器只管理水平坐标,垂直坐标需要的大小则是所有子控件中最长的那个;
			if( sz.cy > 0)
			{
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				if(sz.cy > cyFixed)
					cyFixed = sz.cy;
			}
			cxFixed += pControl->GetPadding().left + pControl->GetPadding().right;
			nEstimateNum++;
		}
		cxFixed += (nEstimateNum - 1) * m_iChildPadding;
		SIZE szAvailable = {-1,-1};
		//最后算出需要的尺寸同Container一样,小于父布局本身大小则采用本身;
		if(cxFixed > 0)
		{
			szAvailable.cx = cxFixed;
			if(cxFixed < m_cxyFixed.cx)
				szAvailable.cx = m_cxyFixed.cx;
		}
		else
			szAvailable.cx = m_cxyFixed.cx;

		if(cyFixed > 0)
		{
			szAvailable.cy = cyFixed;
			if(cyFixed < m_cxyFixed.cy)
				szAvailable.cy = m_cxyFixed.cy;
		}
		else
			szAvailable.cy = m_cxyFixed.cy;

		return szAvailable;
	}

	void CHorizontalLayoutUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		rc = m_rcItem;

		// Adjust for inset
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		if( m_items.GetSize() == 0) 
		{
			ProcessScrollBar(rc, 0, 0);
			return;
		}

		// Determine the width of elements that are sizeable
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
 		if(m_pVerticalScrollBar) 
		{
			szAvailable.cy += m_pVerticalScrollBar->GetScrollRange();
			if(m_pVerticalScrollBar->IsTakePlace())
			{
				rc.right -= m_pVerticalScrollBar->GetFixedWidth();
				szAvailable.cx -= m_pVerticalScrollBar->GetFixedWidth();
			}
		}

		int nAdjustables = 0;
		int cxFixed = 0;
		int nEstimateNum = 0;
		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			//这里会估算子控件大小,然后最后管理其他自适应控件;
			//因为NeedUpdate的时候 如果有sizegrow则已经计算出了该控件应该的大小;
			SIZE sz = pControl->EstimateSize(szAvailable);
			if( sz.cx == -1 )
				nAdjustables++;
			else 
			{
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				cxFixed+=sz.cx;
			}
			cxFixed += pControl->GetPadding().left + pControl->GetPadding().right;
			nEstimateNum++;
		}
		if(nEstimateNum > 1)
			cxFixed += (nEstimateNum - 1) * m_iChildPadding;
		if(cxFixed > szAvailable.cx)
		{
			if(m_pHorizontalScrollBar&&m_pHorizontalScrollBar->IsVisible()&&m_pHorizontalScrollBar->IsShowTakePlace())
			{
				rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
				szAvailable.cy -= m_pHorizontalScrollBar->GetFixedHeight();
			}
		}

		int cxExpand = 0;
		int cxNeeded = 0;
		int cyNeeded = 0;
		int nCurItemCyNeeded = 0;
		if( nAdjustables > 0 ) cxExpand = MAX(0, (szAvailable.cx - cxFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosX = rc.left;
		if( m_pHorizontalScrollBar)
			iPosX -= m_pHorizontalScrollBar->GetScrollPos();
		int iPosY = rc.top;
		if( m_pVerticalScrollBar)
			iPosY -= m_pVerticalScrollBar->GetScrollPos();

		int iAdjustable = 0;
		int cxFixedRemaining = cxFixed;

		for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) 
			{
				SetFloatPos(it2);
				continue;
			}
			RECT rcPadding = pControl->GetPadding();
			SIZE sz = pControl->EstimateSize(szRemaining);
			
			if( sz.cx == -1 )
			{
				sz.cx = cxExpand;
				iAdjustable++;
				if( iAdjustable == nAdjustables )
					sz.cx = MAX(0, szRemaining.cx - cxFixedRemaining);
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
			}
			else
			{
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				cxFixedRemaining -= sz.cx;
			}
			cxFixedRemaining -= rcPadding.left + rcPadding.right;
		
			if( sz.cy == -1 ) sz.cy = rc.bottom - rc.top - rcPadding.top - rcPadding.bottom;
			if( sz.cy < 0 ) sz.cy = 0;
			if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
			if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();

			RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + sz.cx + rcPadding.left, iPosY + rcPadding.top + sz.cy};
			pControl->SetPos(rcCtrl);
			iPosX += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right;
			cxNeeded += sz.cx + rcPadding.left + rcPadding.right;
			nCurItemCyNeeded = sz.cy + rcPadding.top+rcPadding.bottom;
			if(nCurItemCyNeeded > cyNeeded)
				cyNeeded = nCurItemCyNeeded;
			szRemaining.cx -= sz.cx + rcPadding.left + rcPadding.right;
		}
		cxNeeded += (nEstimateNum - 1) * m_iChildPadding;

		ProcessScrollBar(rc, cxNeeded, cyNeeded);
	}

	void CHorizontalLayoutUI::DoPostPaint(const RECT& rcPaint)
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode ) 
		{
			RECT rcSeparator = GetThumbRect(true);
			m_pManager->GetRenderCore()->DrawColor(rcSeparator,0xAA000000);
		}
	}

	void CHorizontalLayoutUI::SetSepWidth(int iWidth)
	{
		m_iSepWidth = iWidth;
	}

	int CHorizontalLayoutUI::GetSepWidth() const
	{
		return m_iSepWidth;
	}

	void CHorizontalLayoutUI::SetSepImmMode(bool bImmediately)
	{
		if( m_bImmMode == bImmediately ) return;
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode && m_pManager != NULL ) {
			m_pManager->RemovePostPaint(this);
		}

		m_bImmMode = bImmediately;
	}

	bool CHorizontalLayoutUI::IsSepImmMode() const
	{
		return m_bImmMode;
	}

	void CHorizontalLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("sepwidth")) == 0 ) SetSepWidth(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("sepimm")) == 0 ) SetSepImmMode(_tcscmp(pstrValue, _T("true")) == 0);
		else CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CHorizontalLayoutUI::DoEvent(TEventUI& event)
	{
		if( m_iSepWidth != 0 ) {
			if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() )
			{
				RECT rcSeparator = GetThumbRect(false);
				if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
					m_uButtonState |= UISTATE_CAPTURED;
					ptLastMouse = event.ptMouse;
					m_rcNewPos = m_rcItem;
					if( !m_bImmMode && m_pManager ) m_pManager->AddPostPaint(this);
					return;
				}
			}
			if( event.Type == UIEVENT_BUTTONUP )
			{
				if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
					m_uButtonState &= ~UISTATE_CAPTURED;
					m_rcItem = m_rcNewPos;
					if( !m_bImmMode && m_pManager ) m_pManager->RemovePostPaint(this);
					NeedParentUpdate();
					return;
				}
			}
			if( event.Type == UIEVENT_MOUSEMOVE )
			{
				if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
					LONG cx = event.ptMouse.x - ptLastMouse.x;
					ptLastMouse = event.ptMouse;
					RECT rc = m_rcNewPos;
					if( m_iSepWidth >= 0 ) {
						if( cx > 0 && event.ptMouse.x < m_rcNewPos.right - m_iSepWidth ) return;
						if( cx < 0 && event.ptMouse.x > m_rcNewPos.right ) return;
						rc.right += cx;
						if( rc.right - rc.left <= GetMinWidth() ) {
							if( m_rcNewPos.right - m_rcNewPos.left <= GetMinWidth() ) return;
							rc.right = rc.left + GetMinWidth();
						}
						if( rc.right - rc.left >= GetMaxWidth() ) {
							if( m_rcNewPos.right - m_rcNewPos.left >= GetMaxWidth() ) return;
							rc.right = rc.left + GetMaxWidth();
						}
					}
					else {
						if( cx > 0 && event.ptMouse.x < m_rcNewPos.left ) return;
						if( cx < 0 && event.ptMouse.x > m_rcNewPos.left - m_iSepWidth ) return;
						rc.left += cx;
						if( rc.right - rc.left <= GetMinWidth() ) {
							if( m_rcNewPos.right - m_rcNewPos.left <= GetMinWidth() ) return;
							rc.left = rc.right - GetMinWidth();
						}
						if( rc.right - rc.left >= GetMaxWidth() ) {
							if( m_rcNewPos.right - m_rcNewPos.left >= GetMaxWidth() ) return;
							rc.left = rc.right - GetMaxWidth();
						}
					}

					CDuiRect rcInvalidate = GetThumbRect(true);
					m_rcNewPos = rc;
					m_cxyFixed.cx = m_rcNewPos.right - m_rcNewPos.left;

					if( m_bImmMode ) {
						m_rcItem = m_rcNewPos;
						NeedParentUpdate();
					}
					else {
						rcInvalidate.Join(GetThumbRect(true));
						rcInvalidate.Join(GetThumbRect(false));
						if( m_pManager ) m_pManager->Invalidate(rcInvalidate);
					}
					return;
				}
			}
			if( event.Type == UIEVENT_SETCURSOR )
			{
				RECT rcSeparator = GetThumbRect(false);
				if( IsEnabled() && ::PtInRect(&rcSeparator, event.ptMouse) ) {
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
					return;
				}
			}
		}
		CContainerUI::DoEvent(event);
	}

	RECT CHorizontalLayoutUI::GetThumbRect(bool bUseNew) const
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && bUseNew) {
			if( m_iSepWidth >= 0 ) return CDuiRect(m_rcNewPos.right - m_iSepWidth, m_rcNewPos.top, m_rcNewPos.right, m_rcNewPos.bottom);
			else return CDuiRect(m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.left - m_iSepWidth, m_rcNewPos.bottom);
		}
		else {
			if( m_iSepWidth >= 0 ) return CDuiRect(m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
			else return CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom);
		}
	}
}
