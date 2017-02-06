#include "stdafx.h"
#include "UIVerticalLayout.h"

namespace UiLib
{
	REGIST_DUICLASS(CVerticalLayoutUI);

	CVerticalLayoutUI::CVerticalLayoutUI() : m_iSepHeight(0), m_uButtonState(0), m_bImmMode(false)
	{
		ptLastMouse.x = ptLastMouse.y = 0;
		m_rcNewPos.left = m_rcNewPos.right = m_rcNewPos.top = m_rcNewPos.bottom = 0;
		m_bNeedCursor = true;
	}

	LPCTSTR CVerticalLayoutUI::GetClass() const
	{
		return _T("VerticalLayoutUI");
	}

	LPVOID CVerticalLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("VerticalLayout")) == 0 ) return static_cast<CVerticalLayoutUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	UINT CVerticalLayoutUI::GetControlFlags() const
	{
		if( IsEnabled() && m_iSepHeight != 0 ) return IsWantCursor();
		else return 0;
	}

	SIZE CVerticalLayoutUI::EstimateNeedSize()
	{
		if(m_items.GetSize() == 0)
			return EstimateSize(m_cxyFixed);

		RECT rc = m_rcItem;
		rc.top += m_rcInset.top;
		rc.bottom -= m_rcInset.bottom;

		int cxFixed = 0;
		int cyFixed = 0;
		int nEstimateNum = 0;
		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			SIZE sz = pControl->EstimateSize(m_cxyFixed);
			if( sz.cx > 0)
			{
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				if(sz.cx > cxFixed)
					cxFixed = sz.cx;
			}
			if(sz.cy > 0)
			{
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				cyFixed += sz.cy;
			}
			cyFixed += pControl->GetPadding().top + pControl->GetPadding().bottom;
			nEstimateNum++;
		}
		cyFixed += (nEstimateNum - 1) * m_iChildPadding;
		SIZE szAvailable = {-1,-1};
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

	void CVerticalLayoutUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		rc = m_rcItem;

		// Adjust for inset;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		if( m_items.GetSize() == 0) 
		{
			ProcessScrollBar(rc, 0, 0);
			return;
		}

		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

		if( m_pHorizontalScrollBar) 
		{
			szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();
			if(m_pHorizontalScrollBar->IsTakePlace())
			{
				rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
				szAvailable.cy -= m_pHorizontalScrollBar->GetFixedHeight();
			}
		}

		int nAdjustables = 0;
		int cyFixed = 0;
		int nEstimateNum = 0;
		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			if( sz.cy == -1 )
				nAdjustables++;
			else
			{
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				cyFixed+=sz.cy;
			}
			cyFixed += pControl->GetPadding().top + pControl->GetPadding().bottom;
			nEstimateNum++;
		}
        //Cacle and fix child Padding Sum;
        //Add it to Item;
		if(nEstimateNum > 1)
			cyFixed += (nEstimateNum - 1) * m_iChildPadding;

		if(cyFixed > szAvailable.cy)
		{
			if(m_pVerticalScrollBar&&m_pVerticalScrollBar->IsVisible()&&m_pVerticalScrollBar->IsShowTakePlace())
			{
				rc.right -= m_pVerticalScrollBar->GetFixedWidth();
				szAvailable.cx -= m_pVerticalScrollBar->GetFixedWidth();
			}
		}
	
		// Place elements
		int cyNeeded = 0;
		int cxNeeded = 0;
		int cyExpand = 0;
		int nCurItemCxNeeded = 0;
        //if there are some Items' cy unset; cale cyExpand[average](get the average of the left height)
		if( nAdjustables > 0 ) cyExpand = MAX(0, (szAvailable.cy - cyFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;
		int iPosY = rc.top;
		if(m_pVerticalScrollBar)
			iPosY -= m_pVerticalScrollBar->GetScrollPos();
		int iPosX = rc.left;
		if( m_pHorizontalScrollBar) 
			iPosX -= m_pHorizontalScrollBar->GetScrollPos();

		int iAdjustable = 0;
		int cyFixedRemaining = cyFixed;

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
			if( sz.cy == -1 )
			{
				iAdjustable++;
				sz.cy = cyExpand;
				if( iAdjustable == nAdjustables )
					sz.cy = MAX(0, szRemaining.cy - cyFixedRemaining);
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
			}
			else
			{
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				cyFixedRemaining -= sz.cy;
			}
			cyFixedRemaining -= rcPadding.top + rcPadding.bottom;
			
			if( sz.cx == -1 ) sz.cx = szAvailable.cx - rcPadding.left - rcPadding.right;
			if( sz.cx < 0 ) sz.cx = 0;
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

			RECT rcCtrl = { iPosX + rcPadding.left, iPosY + rcPadding.top, iPosX + rcPadding.left + sz.cx, iPosY + sz.cy + rcPadding.top};
			pControl->SetPos(rcCtrl);

			iPosY += sz.cy + m_iChildPadding + rcPadding.top + rcPadding.bottom;
			cyNeeded += sz.cy + rcPadding.top + rcPadding.bottom;

			nCurItemCxNeeded = sz.cx + rcPadding.left+rcPadding.right;
			if(nCurItemCxNeeded > cxNeeded)
				cxNeeded = nCurItemCxNeeded;
			szRemaining.cy -= sz.cy + rcPadding.top + rcPadding.bottom;
		}
		cyNeeded += (nEstimateNum - 1) * m_iChildPadding;
		ProcessScrollBar(rc, cxNeeded, cyNeeded);
	}

	void CVerticalLayoutUI::DoPostPaint(const RECT& rcPaint)
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode ) 
		{
			RECT rcSeparator = GetThumbRect(true);
			m_pManager->GetRenderCore()->DrawColor(rcSeparator,0xAA000000);
		}
	}

	void CVerticalLayoutUI::SetSepHeight(int iHeight)
	{
		m_iSepHeight = iHeight;
	}

	int CVerticalLayoutUI::GetSepHeight() const
	{
		return m_iSepHeight;
	}

	void CVerticalLayoutUI::SetSepImmMode(bool bImmediately)
	{
		if( m_bImmMode == bImmediately ) return;
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode && m_pManager != NULL ) {
			m_pManager->RemovePostPaint(this);
		}

		m_bImmMode = bImmediately;
	}

	bool CVerticalLayoutUI::IsSepImmMode() const
	{
		return m_bImmMode;
	}

	void CVerticalLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("sepheight")) == 0 ) SetSepHeight(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("sepimm")) == 0 ) SetSepImmMode(_tcscmp(pstrValue, _T("true")) == 0);
		else CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CVerticalLayoutUI::DoEvent(TEventUI& event)
	{
		if( m_iSepHeight != 0 ) {
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
					LONG cy = event.ptMouse.y - ptLastMouse.y;
					ptLastMouse = event.ptMouse;
					RECT rc = m_rcNewPos;
					if( m_iSepHeight >= 0 ) {
						if( cy > 0 && event.ptMouse.y < m_rcNewPos.bottom + m_iSepHeight ) return;
						if( cy < 0 && event.ptMouse.y > m_rcNewPos.bottom ) return;
						rc.bottom += cy;
						if( rc.bottom - rc.top <= GetMinHeight() ) {
							if( m_rcNewPos.bottom - m_rcNewPos.top <= GetMinHeight() ) return;
							rc.bottom = rc.top + GetMinHeight();
						}
						if( rc.bottom - rc.top >= GetMaxHeight() ) {
							if( m_rcNewPos.bottom - m_rcNewPos.top >= GetMaxHeight() ) return;
							rc.bottom = rc.top + GetMaxHeight();
						}
					}
					else {
						if( cy > 0 && event.ptMouse.y < m_rcNewPos.top ) return;
						if( cy < 0 && event.ptMouse.y > m_rcNewPos.top + m_iSepHeight ) return;
						rc.top += cy;
						if( rc.bottom - rc.top <= GetMinHeight() ) {
							if( m_rcNewPos.bottom - m_rcNewPos.top <= GetMinHeight() ) return;
							rc.top = rc.bottom - GetMinHeight();
						}
						if( rc.bottom - rc.top >= GetMaxHeight() ) {
							if( m_rcNewPos.bottom - m_rcNewPos.top >= GetMaxHeight() ) return;
							rc.top = rc.bottom - GetMaxHeight();
						}
					}

					CDuiRect rcInvalidate = GetThumbRect(true);
					m_rcNewPos = rc;
					m_cxyFixed.cy = m_rcNewPos.bottom - m_rcNewPos.top;

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
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS)));
					return;
				}
			}
		}
		CContainerUI::DoEvent(event);
	}

	RECT CVerticalLayoutUI::GetThumbRect(bool bUseNew) const
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 && bUseNew) {
			if( m_iSepHeight >= 0 ) 
				return CDuiRect(m_rcNewPos.left, MAX(m_rcNewPos.bottom - m_iSepHeight, m_rcNewPos.top), 
				m_rcNewPos.right, m_rcNewPos.bottom);
			else 
				return CDuiRect(m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.right, 
				MIN(m_rcNewPos.top - m_iSepHeight, m_rcNewPos.bottom));
		}
		else {
			if( m_iSepHeight >= 0 ) 
				return CDuiRect(m_rcItem.left, MAX(m_rcItem.bottom - m_iSepHeight, m_rcItem.top), m_rcItem.right, 
				m_rcItem.bottom);
			else 
				return CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.right, 
				MIN(m_rcItem.top - m_iSepHeight, m_rcItem.bottom));

		}
	}
}
