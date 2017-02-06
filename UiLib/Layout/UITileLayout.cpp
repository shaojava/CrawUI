#include "stdafx.h"
#include "UITileLayout.h"

namespace UiLib
{
	REGIST_DUICLASS(CTileLayoutUI);

	CTileLayoutUI::CTileLayoutUI() : m_nColumns(1)
	{
		m_szItem.cx = m_szItem.cy = 0;
		m_bHorAlign = true;
	}

	LPCTSTR CTileLayoutUI::GetClass() const
	{
		return _T("TileLayoutUI");
	}

	LPVOID CTileLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("TileLayout")) == 0 ) return static_cast<CTileLayoutUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	SIZE CTileLayoutUI::GetItemSize() const
	{
		return m_szItem;
	}

	void CTileLayoutUI::SetItemSize(SIZE szItem)
	{
		if( m_szItem.cx != szItem.cx || m_szItem.cy != szItem.cy ) {
			m_szItem = szItem;
			NeedUpdate();
		}
	}

	int CTileLayoutUI::GetColumns() const
	{
		return m_nColumns;
	}

	void CTileLayoutUI::SetColumns(int nCols)
	{
		if( nCols <= 0 ) return;
		m_nColumns = nCols;
		NeedUpdate();
	}

	void CTileLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("itemsize")) == 0 ) {
			SIZE szItem = { 0 };
			LPTSTR pstr = NULL;
			szItem.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szItem.cy = _tcstol(pstr + 1, &pstr, 10);   ASSERT(pstr);     
			SetItemSize(szItem);
		}
		if(_tcscmp(pstrName, _T("tilealign")) == 0)
		{
			if(_tcscmp(pstrValue, _T("vertical")) == 0)
				m_bHorAlign = false;
		}
		else if( _tcscmp(pstrName, _T("columns")) == 0 ) SetColumns(_ttoi(pstrValue));
		else CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CTileLayoutUI::SetPos(RECT rc)
	{
		if(m_bHorAlign)
			AlignHor(rc);
		else
			AlignVer(rc);
		return;

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


		if(m_pVerticalScrollBar&&m_pVerticalScrollBar->IsTakePlace()) 
			rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if( m_pHorizontalScrollBar&&m_pHorizontalScrollBar->IsTakePlace()) 
			rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();


		// Position the elements
		if( m_szItem.cx > 0 ) m_nColumns = (rc.right - rc.left) / m_szItem.cx;
		if( m_nColumns == 0 ) m_nColumns = 1;

		int cyNeeded = 0;
		int cxWidth = (rc.right - rc.left) / m_nColumns;
		if( m_pHorizontalScrollBar) 
			cxWidth = (rc.right - rc.left + m_pHorizontalScrollBar->GetScrollRange() ) / m_nColumns; ;

		int cyHeight = 0;
		int iCount = 0;
		POINT ptTile = { rc.left, rc.top };
		if( m_pVerticalScrollBar)
			ptTile.y -= m_pVerticalScrollBar->GetScrollPos();
		int iPosX = rc.left;
		if( m_pHorizontalScrollBar) 
		{
			iPosX -= m_pHorizontalScrollBar->GetScrollPos();
			ptTile.x = iPosX;
		}
		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {
				SetFloatPos(it1);
				continue;
			}

			// Determine size
			RECT rcTile = { ptTile.x, ptTile.y, ptTile.x + cxWidth, ptTile.y };
			if( (iCount % m_nColumns) == 0 )
			{
				int iIndex = iCount;
				for( int it2 = it1; it2 < m_items.GetSize(); it2++ ) 
				{
					CControlUI* pLineControl = static_cast<CControlUI*>(m_items[it2]);
					if( !pLineControl->IsVisible() ) continue;
					if( pLineControl->IsFloat() ) continue;

					RECT rcPadding = pLineControl->GetPadding();
					SIZE szAvailable = { rcTile.right - rcTile.left - rcPadding.left - rcPadding.right, 9999 };
					if( iIndex == iCount || (iIndex + 1) % m_nColumns == 0 )
						szAvailable.cx -= m_iChildPadding / 2;
					else
						szAvailable.cx -= m_iChildPadding;

					if( szAvailable.cx < pLineControl->GetMinWidth() ) szAvailable.cx = pLineControl->GetMinWidth();
					if( szAvailable.cx > pLineControl->GetMaxWidth() ) szAvailable.cx = pLineControl->GetMaxWidth();

					SIZE szTile = pLineControl->EstimateSize(szAvailable);
					if(szTile.cx == -1)
						szTile.cx = m_szItem.cx;
					if(szTile.cy == -1)
						szTile.cy = m_szItem.cy;
					if( szTile.cx < pLineControl->GetMinWidth() ) szTile.cx = pLineControl->GetMinWidth();
					if( szTile.cx > pLineControl->GetMaxWidth() ) szTile.cx = pLineControl->GetMaxWidth();
					if( szTile.cy < pLineControl->GetMinHeight() ) szTile.cy = pLineControl->GetMinHeight();
					if( szTile.cy > pLineControl->GetMaxHeight() ) szTile.cy = pLineControl->GetMaxHeight();


					//Get The Max Height of this Column;
					cyHeight = MAX(cyHeight, szTile.cy + rcPadding.top + rcPadding.bottom);
					if( (++iIndex % m_nColumns) == 0) break;
				}
			}

			RECT rcPadding = pControl->GetPadding();

			rcTile.left += rcPadding.left + m_iChildPadding / 2;
			rcTile.right -= rcPadding.right + m_iChildPadding / 2;
			if( (iCount % m_nColumns) == 0 ) 
				rcTile.left -= m_iChildPadding / 2;

			if( ( (iCount + 1) % m_nColumns) == 0 ) 
				rcTile.right += m_iChildPadding / 2;

			// Set position
			rcTile.top = ptTile.y + rcPadding.top;
			rcTile.bottom = ptTile.y + cyHeight;

			SIZE szAvailable = { rcTile.right - rcTile.left, rcTile.bottom - rcTile.top };
			SIZE szTile = pControl->EstimateSize(szAvailable);
			if( szTile.cx == -1 ) szTile.cx = szAvailable.cx;
			if( szTile.cy == -1 ) szTile.cy = szAvailable.cy;
			if( szTile.cx < pControl->GetMinWidth() ) szTile.cx = pControl->GetMinWidth();
			if( szTile.cx > pControl->GetMaxWidth() ) szTile.cx = pControl->GetMaxWidth();
			if( szTile.cy < pControl->GetMinHeight() ) szTile.cy = pControl->GetMinHeight();
			if( szTile.cy > pControl->GetMaxHeight() ) szTile.cy = pControl->GetMaxHeight();
			RECT rcPos = {(rcTile.left + rcTile.right - szTile.cx) / 2, (rcTile.top + rcTile.bottom - szTile.cy) / 2,
				(rcTile.left + rcTile.right - szTile.cx) / 2 + szTile.cx, (rcTile.top + rcTile.bottom - szTile.cy) / 2 + szTile.cy};
			pControl->SetPos(rcPos);

			if( (++iCount % m_nColumns) == 0 ) 
			{
				ptTile.x = iPosX;
				ptTile.y += cyHeight + m_iChildPadding;
				cyHeight = 0;
			}
			else 
				ptTile.x += cxWidth;
			cyNeeded = rcTile.bottom - rc.top;
			if( m_pVerticalScrollBar) cyNeeded += m_pVerticalScrollBar->GetScrollPos();
		}

		// Process the scrollbar
		ProcessScrollBar(rc, 0, cyNeeded);
	}

	void CTileLayoutUI::AlignHor( RECT rc )
	{
		CControlUI::SetPos(rc);
		rc = m_rcItem;

		// Adjust for inset
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		if( !m_items.GetSize() && !m_szItem.cx || !m_szItem.cy) 
		{
			ProcessScrollBar(rc, 0, 0);
			return;
		}

		if(m_pVerticalScrollBar&&m_pVerticalScrollBar->IsTakePlace()) 
			rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if( m_pHorizontalScrollBar&&m_pHorizontalScrollBar->IsTakePlace()) 
			rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

		int nTileAbleWidth = rc.right - rc.left;
		if(m_pHorizontalScrollBar)
			nTileAbleWidth += m_pHorizontalScrollBar->GetScrollRange();
		m_nColumns = nTileAbleWidth / m_szItem.cx;

		if(!m_nColumns) m_nColumns = 1;

		int nRemainWidth = nTileAbleWidth-m_szItem.cx*m_nColumns;
		
		int nGap = 0;
		if(m_nColumns > 1)
			nGap = nRemainWidth/(m_nColumns-1);

		POINT ptTile = { rc.left, rc.top };
		if( m_pVerticalScrollBar)
			ptTile.y -= m_pVerticalScrollBar->GetScrollPos();
		if( m_pHorizontalScrollBar) 
			ptTile.x -= m_pHorizontalScrollBar->GetScrollPos();

		RECT rcTile = { ptTile.x, ptTile.y, ptTile.x + m_szItem.cx, ptTile.y+m_szItem.cy };

		int nStartX = ptTile.x;
		int nStartY = ptTile.y;
		int iRow = 0;
		int nColum = 0;
		for( int it = 0; it < m_items.GetSize(); it++ ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {SetFloatPos(it);continue;}

			if(it % m_nColumns == 0)
			{
				nStartX = ptTile.x;
				nStartY = ptTile.y;
				nStartY += iRow * (m_szItem.cy + m_iChildPadding);
				nColum = 0;
				iRow++;
			}
			RECT rcPadding = pControl->GetPadding();
			RECT rcPos = {nColum*(m_szItem.cx+nGap)+nStartX+rcPadding.left,nStartY+rcPadding.top,nColum*(m_szItem.cx+nGap)+nStartX+rcPadding.left+m_szItem.cx,nStartY+rcPadding.top+m_szItem.cy};
			pControl->SetPos(rcPos);
			nColum++;
		}
		int cyNeeded = iRow*m_szItem.cy+(iRow-1)*m_iChildPadding;
		ProcessScrollBar(rc, 0, cyNeeded);
	}

	void CTileLayoutUI::AlignVer( RECT rc )
	{
		CControlUI::SetPos(rc);
		rc = m_rcItem;

		// Adjust for inset
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		if( !m_items.GetSize() && !m_szItem.cx || !m_szItem.cy) 
		{
			ProcessScrollBar(rc, 0, 0);
			return;
		}

		if(m_pVerticalScrollBar&&m_pVerticalScrollBar->IsTakePlace()) 
			rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if( m_pHorizontalScrollBar&&m_pHorizontalScrollBar->IsTakePlace()) 
			rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

		int nTileAbleHeight = rc.bottom - rc.top;
		if(m_pVerticalScrollBar)
			nTileAbleHeight += m_pVerticalScrollBar->GetScrollRange();
		m_nColumns = nTileAbleHeight / m_szItem.cy;

		if(!m_nColumns) m_nColumns = 1;

		int nRemainHeight = nTileAbleHeight-m_szItem.cy*m_nColumns;

		int nGap = 0;
		if(m_nColumns > 1)
			nGap = nRemainHeight/(m_nColumns-1);

		POINT ptTile = { rc.left, rc.top };
		if( m_pVerticalScrollBar)
			ptTile.y -= m_pVerticalScrollBar->GetScrollPos();
		if( m_pHorizontalScrollBar) 
			ptTile.x -= m_pHorizontalScrollBar->GetScrollPos();

		RECT rcTile = { ptTile.x, ptTile.y, ptTile.x + m_szItem.cx, ptTile.y+m_szItem.cy };

		int nStartX = ptTile.x;
		int nStartY = ptTile.y;
		int iRow = 0;
		int nColum = 0;
		for( int it = 0; it < m_items.GetSize(); it++ ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) {SetFloatPos(it);continue;}

			if(it % m_nColumns == 0)
			{
				nStartX = ptTile.x;
				nStartY = ptTile.y;
				nStartX += nColum * (m_szItem.cx + m_iChildPadding);
				iRow = 0;
				nColum++;
			}

			RECT rcPadding = pControl->GetPadding();
			RECT rcPos = {nStartX+rcPadding.left,iRow*(m_szItem.cy+nGap)+nStartY+rcPadding.top,nStartX+rcPadding.left+m_szItem.cx,iRow*(m_szItem.cy+nGap)+nStartY+rcPadding.top+m_szItem.cy};
			pControl->SetPos(rcPos);
			iRow++;
		}
		int cxNeeded = nColum*m_szItem.cx+(nColum-1)*m_iChildPadding;
		ProcessScrollBar(rc, cxNeeded, 0);
	}

}
