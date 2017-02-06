#include "StdAfx.h"

namespace UiLib {

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	REGIST_DUICLASS(CListUI);
	REGIST_DUICLASS(CListHeaderUI);
	REGIST_DUICLASS(CListHeaderItemUI);
	REGIST_DUICLASS(CListLabelElementUI);
	REGIST_DUICLASS(CListTextElementUI);
	REGIST_DUICLASS(CListImageTextElementUI);
	REGIST_DUICLASS(CListContainerElementUI);
	REGIST_DUICLASS(CListContainerItemElementUI);

	CListUI::CListUI() : m_pCallback(NULL), m_bScrollSelect(false), m_iExpandedItem(-1),m_bSingleSel(false)
	{
		m_pList = new CListBodyUI(this);
		m_pHeader = NULL;
		CVerticalLayoutUI::Add(m_pList);

		m_ListInfo.nColumns = 1;
		m_ListInfo.nFont = -1;
		m_ListInfo.uTextStyle = DT_VCENTER; // m_uTextStyle(DT_VCENTER | DT_END_ELLIPSIS)
		m_ListInfo.dwTextColor = 0xFF000000;
		m_ListInfo.dwBkColor = 0;
		m_ListInfo.bAlternateBk = false;
		m_ListInfo.dwSelectedTextColor = 0xFF000000;
		m_ListInfo.dwSelectedBkColor = 0xFFC1E3FF;
		m_ListInfo.dwHotTextColor = 0xFF000000;
		m_ListInfo.dwHotBkColor = 0xFFE9F5FF;
		m_ListInfo.dwDisabledTextColor = 0xFFCCCCCC;
		m_ListInfo.dwDisabledBkColor = 0xFFFFFFFF;
		m_ListInfo.dwLineColor = 0;
		m_ListInfo.bShowHtml = false;
		m_ListInfo.bMultiExpandable = false;
		m_ListInfo.bShowHLine = true;
		m_ListInfo.bShowVLine = true;
		::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
		::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
		::ZeroMemory(&m_ListInfo.szCheckImg, sizeof(m_ListInfo.szCheckImg));
		::ZeroMemory(&m_ListInfo.szIconImg, sizeof(m_ListInfo.szIconImg));

		m_iLastSingleSelect = -1;
		m_bNeedTab = true;
	}

	LPCTSTR CListUI::GetClass() const
	{
		return _T("ListUI");
	}

	UINT CListUI::GetControlFlags() const
	{
		return IsWantTab();
	}

	LPVOID CListUI::GetInterface(LPCTSTR pstrName)
	{
		if(m_pHeader)
			m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);

		if( _tcscmp(pstrName, _T("List")) == 0 ) return static_cast<CListUI*>(this);
		if( _tcscmp(pstrName, _T("IList")) == 0 ) return static_cast<IListUI*>(this);
		if( _tcscmp(pstrName, _T("IListOwner")) == 0 ) return static_cast<IListOwnerUI*>(this);
		return CVerticalLayoutUI::GetInterface(pstrName);
	}

	CControlUI* CListUI::GetItemAt(int iIndex) const
	{
		return m_pList->GetItemAt(iIndex);
	}

	int CListUI::GetItemIndex(CControlUI* pControl) const
	{
		if( pControl->GetInterface(_T("ListHeader")) != NULL ) return CVerticalLayoutUI::GetItemIndex(pControl);
		// We also need to recognize header sub-items
		if( _tcsstr(pControl->GetClass(), _T("ListHeaderItemUI")) != NULL ) return m_pHeader->GetItemIndex(pControl);

		return m_pList->GetItemIndex(pControl);
	}

	bool CListUI::SetItemIndex(CControlUI* pControl, int iIndex)
	{
		if( pControl->GetInterface(_T("ListHeader")) != NULL ) return CVerticalLayoutUI::SetItemIndex(pControl, iIndex);
		// We also need to recognize header sub-items
		if( _tcsstr(pControl->GetClass(), _T("ListHeaderItemUI")) != NULL ) return m_pHeader->SetItemIndex(pControl, iIndex);

		int iOrginIndex = m_pList->GetItemIndex(pControl);
		if( iOrginIndex == -1 ) return false;
		if( iOrginIndex == iIndex ) return true;

		IListItemUI* pSelectedListItem = NULL;
		if( !m_pList->SetItemIndex(pControl, iIndex) ) return false;
		int iMinIndex = min(iOrginIndex, iIndex);
		int iMaxIndex = max(iOrginIndex, iIndex);
		for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
			CControlUI* p = m_pList->GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}
		UnSelectAllItems();
		return true;
	}

	int CListUI::GetCount() const
	{
		return m_pList->GetCount();
	}

	bool CListUI::Add(CControlUI* pControl)
	{
		// Override the Add() method so we can add items specifically to
		// the intended widgets. Headers are assumed to be
		// answer the correct interface so we can add multiple list headers.
		if(!pControl) return false;
		if( pControl->GetInterface(_T("ListHeader")) != NULL ) 
		{
			m_pHeader = static_cast<CListHeaderUI*>(pControl);
			m_pHeader->SetOwner(this);
			m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
			return CVerticalLayoutUI::AddAt(pControl, 0);
		}
		// We also need to recognize header sub-items
		if( _tcsstr(pControl->GetClass(), _T("ListHeaderItemUI")) != NULL ) {
			bool ret = m_pHeader->Add(pControl);
			m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
			return ret;
		}
		// The list items should know about us
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(GetCount());
		}
		return m_pList->Add(pControl);
	}

	bool CListUI::AddAt(CControlUI* pControl, int iIndex)
	{
		// Override the AddAt() method so we can add items specifically to
		// the intended widgets. Headers and are assumed to be
		// answer the correct interface so we can add multiple list headers.
		if( pControl->GetInterface(_T("ListHeader")) != NULL ) {
			if( m_pHeader != pControl && m_pHeader->GetCount() == 0 ) {
				CVerticalLayoutUI::Remove(m_pHeader);
				m_pHeader = static_cast<CListHeaderUI*>(pControl);
				m_pHeader->SetOwner(this);
			}
			m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
			return CVerticalLayoutUI::AddAt(pControl, 0);
		}
		// We also need to recognize header sub-items
		if( _tcsstr(pControl->GetClass(), _T("ListHeaderItemUI")) != NULL ) {
			bool ret = m_pHeader->AddAt(pControl, iIndex);
			m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
			return ret;
		}
		if (!m_pList->AddAt(pControl, iIndex)) return false;

		// The list items should know about us
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(iIndex);
		}

		for(int i = iIndex + 1; i < m_pList->GetCount(); ++i) {
			CControlUI* p = m_pList->GetItemAt(i);
			pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}

		int nSeletedIndexs = m_aSelItems.GetSize();
		if(nSeletedIndexs)
		{
			CStdPtrArray OldSelectedItems = m_aSelItems;
			m_aSelItems.Empty();
			int nSeletedID = -1;
			for(int i = 0 ; i < nSeletedIndexs ; i++)
			{
				nSeletedID = (int)OldSelectedItems.GetAt(i);
				if(nSeletedID >= iIndex)
				{
					nSeletedID++;
					m_aSelItems.Add((LPVOID)nSeletedID);
				}
				else
					m_aSelItems.Add((LPVOID)nSeletedID);
			}
			OldSelectedItems.Empty();
		}
		return true;
	}

	bool CListUI::Remove(CControlUI* pControl)
	{
		if( pControl->GetInterface(_T("ListHeader")) != NULL ) return CVerticalLayoutUI::Remove(pControl);
		// We also need to recognize header sub-items
		if( _tcsstr(pControl->GetClass(), _T("ListHeaderItemUI")) != NULL ) return m_pHeader->Remove(pControl);

		int iIndex = m_pList->GetItemIndex(pControl);
		if (iIndex == -1) return false;

		return RemoveAt(iIndex);
	}

	bool CListUI::RemoveAt(int iIndex)
	{
		if (!m_pList->RemoveAt(iIndex)) return false;

		for(int i = iIndex; i < m_pList->GetCount(); ++i) 
		{
			CControlUI* p = m_pList->GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL )
				pListItem->SetIndex(i);
		}

		int nSeletedIndexs = m_aSelItems.GetSize();
		if(nSeletedIndexs)
		{
			CStdPtrArray OldSelectedItems = m_aSelItems;
			m_aSelItems.Empty();
			int nSeletedID = -1;
			for(int i = 0 ; i < nSeletedIndexs ; i++)
			{
				nSeletedID = (int)OldSelectedItems.GetAt(i);
				if(nSeletedID > iIndex)
				{
					nSeletedID--;
					m_aSelItems.Add((LPVOID)nSeletedID);
				}
				else if(nSeletedID < iIndex)
					m_aSelItems.Add((LPVOID)nSeletedID);
			}
			OldSelectedItems.Empty();
		}

		return true;
	}

	bool CListUI::RemoveItems(int iIndex,int iSel)
	{
		if (!m_pList->RemoveAt(iIndex)) return false;
		for(int i = iIndex; i < m_pList->GetCount(); ++i)
		{
			CControlUI* p = m_pList->GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL )
				pListItem->SetIndex(i);
		}
		m_aSelItems.Remove(m_aSelItems.Find((LPVOID)iSel));
		return true;
	}

	bool CListUI::RemoveSelectItems()
	{
		std::vector<int> vInt;
		for(int i = 0 ; i < m_aSelItems.GetSize() ; i ++)
			vInt.push_back((int)m_aSelItems.GetAt(i));
		sort(vInt.begin(),vInt.end());
		//批量删除时,删一个控件,后面的会顶上,按照选中的删除则需要判断已删除多少次  算出控件顶替后的位置删除;
		for(int i = 0 ; i < vInt.size() ; i++)
			RemoveItems(vInt[i]-i,vInt[i]);
		return true;
	}

	void CListUI::RemoveAll(bool bUpdate)
	{
		m_iExpandedItem = -1;
		m_aSelItems.Empty();
		m_pList->RemoveAll(bUpdate);
	}

	void CListUI::SetPos(RECT rc)
	{
		CVerticalLayoutUI::SetPos(rc);
		if( m_pHeader == NULL ) 
		{
			m_ListInfo.nColumns = 1;
			int iOffset = m_pList->GetScrollPos().cx;
			RECT rcPos = {rc.left,0,rc.right,0};
			if( iOffset > 0 )
			{
				rcPos.left -= iOffset;
				rcPos.right -= iOffset;
			}
			m_ListInfo.rcColumn[0] = rcPos;
			return;
		}

		
		// Determine general list information and the size of header columns
		m_ListInfo.nColumns = MIN(m_pHeader->GetCount(), UILIST_MAX_COLUMNS);
		// The header/columns may or may not be visible at runtime. In either case
		// we should determine the correct dimensions...

		if( !m_pHeader->IsVisible() ) {
			for( int it = 0; it < m_pHeader->GetCount(); it++ ) {
				static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(true);
			}
			m_pHeader->SetPos(CDuiRect(rc.left, 0, rc.right, 0));
		}
		int iOffset = m_pList->GetScrollPos().cx;
		for( int i = 0; i < m_ListInfo.nColumns; i++ ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_pHeader->GetItemAt(i));
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;

			RECT rcPos = pControl->GetPos();
			if( iOffset > 0 )
			{
				rcPos.left -= iOffset;
				rcPos.right -= iOffset;
				pControl->SetPos(rcPos);
			}
			m_ListInfo.rcColumn[i] = pControl->GetPos();
		}
		if( !m_pHeader->IsVisible() ) 
		{
			for( int it = 0; it < m_pHeader->GetCount(); it++ ) 
				static_cast<CControlUI*>(m_pHeader->GetItemAt(it))->SetInternVisible(false);
		}
	}

	int CListUI::GetMinSelItemIndex()
	{
		if (m_aSelItems.GetSize() <= 0)
			return -1;
		int min = (int)m_aSelItems.GetAt(0);
		int index;
		for (int i = 0; i < m_aSelItems.GetSize(); ++i)
		{
			index = (int)m_aSelItems.GetAt(i);
			if (min > index)
				min = index;
		}
		return min;
	}

	int CListUI::GetMaxSelItemIndex()
	{
		if (m_aSelItems.GetSize() <= 0)
			return -1;
		int max = (int)m_aSelItems.GetAt(0);
		int index;
		for (int i = 0; i < m_aSelItems.GetSize(); ++i)
		{
			index = (int)m_aSelItems.GetAt(i);
			if (max < index)
				max = index;
		}
		return max;
	}

	void CListUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CVerticalLayoutUI::DoEvent(event);
			return;
		}
		if(event.Type == UIEVENT_SCROLLWHEEL)
		{
			if(m_pList && (m_pList->GetVerticalScrollBar() || m_pList->GetHorizontalScrollBar()))
				m_pList->DoEvent(event);
		}
		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			m_bFocused = true;
			return;
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			m_bFocused = false;
			return;
		}

		if(event.Type == UIEVENT_KEYDOWN)
		{
			switch( event.chKey ) 
			{
			case VK_UP:
				{
					if (m_aSelItems.GetSize() > 0)
					{					
						int index = GetMinSelItemIndex() - 1;
						UnSelectAllItems();
						index > 0 ? SelectItem(index, true) : SelectItem(0, true);					
					}
				}			
				return;
			case VK_DOWN:
				{
					if (m_aSelItems.GetSize() > 0)
					{					
						int index = GetMaxSelItemIndex() + 1;
						UnSelectAllItems();
						index + 1 > m_pList->GetCount() ? SelectItem(GetCount() - 1, true) : SelectItem(index, true);					
					}
				}
				return;
			case VK_PRIOR:
				PageUp();
				return;
			case VK_NEXT:
				PageDown();
				return;
			case VK_HOME:
				{
					if (m_pList->GetCount() > 0)
						SelectItem(0, true);
				}
				return;
			case VK_END:
				{
					if (m_pList->GetCount() > 0)
						SelectItem(m_pList->GetCount() - 1, true);
				}
				return;        
			case 0x41:
				{
					if (!m_bSingleSel && (GetKeyState(VK_CONTROL) & 0x8000))
						SelectAllItems();
				}
				return;
				break;
			}
		}
		CVerticalLayoutUI::DoEvent(event);
	}

	CListHeaderUI* CListUI::GetHeader() const
	{
		return m_pHeader;
	}

	CContainerUI* CListUI::GetList() const
	{
		return m_pList;
	}

	bool CListUI::GetScrollSelect()
	{
		return m_bScrollSelect;
	}

	void CListUI::SetScrollSelect(bool bScrollSelect)
	{
		m_bScrollSelect = bScrollSelect;
	}

	int CListUI::GetCurSel() const
	{
		if (m_aSelItems.GetSize() <= 0)
		{
			return -1;
		}
		else
		{
			return (int)m_aSelItems.GetAt(0);
		}

		return -1;
	}

	CStdPtrArray CListUI::GetSelectItems()
	{
		return m_aSelItems;
	}

	void CListUI::SetSeleteItems(CStdPtrArray arry)
	{
		m_aSelItems.Empty();
		m_aSelItems = arry;
	}

	int CListUI::GetLastSingleSel() const
	{
		return m_iLastSingleSelect;
	}

	void CListUI::SetLastSingleSel(int iLastSel)
	{
		m_iLastSingleSelect = iLastSel;
	}

	bool CListUI::SelectItem(int iIndex, bool bTakeFocus)
	{
		if( iIndex < 0 ) return false;
		CControlUI* pControl = GetItemAt(iIndex);
		if( pControl == NULL ) return false;
		if( !pControl->IsVisible() ) return false;
		if( !pControl->IsEnabled() ) return false;
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem == NULL ) return false;

		UnSelectAllItems();

		if(m_bSingleSel && m_aSelItems.GetSize() > 0) 
		{
			CControlUI* pControl = GetItemAt((int)m_aSelItems.GetAt(0));
			if( pControl != NULL) 
			{
				IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
				if( pListItem != NULL ) pListItem->Select(false);
			}
		}	

		if( !pListItem->Select(true) ) 
			return false;

		m_aSelItems.Add((LPVOID)iIndex);

		EnsureVisible(iIndex);
		if( bTakeFocus ) pControl->SetFocus();
		if( m_pManager != NULL ) {
			m_pManager->SendNotify(this, _T("itemselect"), iIndex);
		}

		return true;
	}

	TListInfoUI* CListUI::GetListInfo()
	{
		return &m_ListInfo;
	}

	int CListUI::GetChildPadding() const
	{
		return m_pList->GetChildPadding();
	}

	void CListUI::SetChildPadding(int iPadding)
	{
		m_pList->SetChildPadding(iPadding);
	}

	void CListUI::SetItemFont(int index)
	{
		m_ListInfo.nFont = index;
		NeedUpdate();
	}

	void CListUI::SetItemTextStyle(UINT uStyle)
	{
		m_ListInfo.uTextStyle = uStyle;
		NeedUpdate();
	}

	void CListUI::SetItemTextPadding(RECT rc)
	{
		m_ListInfo.rcTextPadding = rc;
		NeedUpdate();
	}

	RECT CListUI::GetItemTextPadding() const
	{
		return m_ListInfo.rcTextPadding;
	}

	void CListUI::SetItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwTextColor = dwTextColor;
		Invalidate();
	}

	void CListUI::SetItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwBkColor = dwBkColor;
		Invalidate();
	}

	void CListUI::SetItemBkImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sBkImage = pStrImage;
		Invalidate();
	}

	void CListUI::SetAlternateBk(bool bAlternateBk)
	{
		m_ListInfo.bAlternateBk = bAlternateBk;
		Invalidate();
	}

	DWORD CListUI::GetItemTextColor() const
	{
		return m_ListInfo.dwTextColor;
	}

	DWORD CListUI::GetItemBkColor() const
	{
		return m_ListInfo.dwBkColor;
	}

	LPCTSTR CListUI::GetItemBkImage() const
	{
		return m_ListInfo.sBkImage;
	}

	bool CListUI::IsAlternateBk() const
	{
		return m_ListInfo.bAlternateBk;
	}

	void CListUI::SetSelectedItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwSelectedTextColor = dwTextColor;
		Invalidate();
	}

	void CListUI::SetSelectedItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwSelectedBkColor = dwBkColor;
		Invalidate();
	}

	void CListUI::SetSelectedItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sSelectedImage = pStrImage;
		Invalidate();
	}

	DWORD CListUI::GetSelectedItemTextColor() const
	{
		return m_ListInfo.dwSelectedTextColor;
	}

	DWORD CListUI::GetSelectedItemBkColor() const
	{
		return m_ListInfo.dwSelectedBkColor;
	}

	LPCTSTR CListUI::GetSelectedItemImage() const
	{
		return m_ListInfo.sSelectedImage;
	}

	void CListUI::SetHotItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwHotTextColor = dwTextColor;
		Invalidate();
	}

	void CListUI::SetHotItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwHotBkColor = dwBkColor;
		Invalidate();
	}

	void CListUI::SetHotItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sHotImage = pStrImage;
		Invalidate();
	}

	DWORD CListUI::GetHotItemTextColor() const
	{
		return m_ListInfo.dwHotTextColor;
	}
	DWORD CListUI::GetHotItemBkColor() const
	{
		return m_ListInfo.dwHotBkColor;
	}

	LPCTSTR CListUI::GetHotItemImage() const
	{
		return m_ListInfo.sHotImage;
	}

	void CListUI::SetDisabledItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwDisabledTextColor = dwTextColor;
		Invalidate();
	}

	void CListUI::SetDisabledItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwDisabledBkColor = dwBkColor;
		Invalidate();
	}

	void CListUI::SetDisabledItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sDisabledImage.SetImage(pStrImage);
		Invalidate();
	}

	DWORD CListUI::GetDisabledItemTextColor() const
	{
		return m_ListInfo.dwDisabledTextColor;
	}

	DWORD CListUI::GetDisabledItemBkColor() const
	{
		return m_ListInfo.dwDisabledBkColor;
	}

	LPCTSTR CListUI::GetDisabledItemImage() const
	{
		return m_ListInfo.sDisabledImage;
	}

	DWORD CListUI::GetItemLineColor() const
	{
		return m_ListInfo.dwLineColor;
	}

	void CListUI::SetItemLineColor(DWORD dwLineColor)
	{
		m_ListInfo.dwLineColor = dwLineColor;
		Invalidate();
	}

	bool CListUI::IsItemShowHtml()
	{
		return m_ListInfo.bShowHtml;
	}

	void CListUI::SetItemShowHtml(bool bShowHtml)
	{
		if( m_ListInfo.bShowHtml == bShowHtml ) return;

		m_ListInfo.bShowHtml = bShowHtml;
		NeedUpdate();
	}

	void CListUI::SetMultiExpanding(bool bMultiExpandable)
	{
		m_ListInfo.bMultiExpandable = bMultiExpandable;
	}

	bool CListUI::ExpandItem(int iIndex, bool bExpand /*= true*/)
	{
		if( m_iExpandedItem >= 0 && !m_ListInfo.bMultiExpandable) {
			CControlUI* pControl = GetItemAt(m_iExpandedItem);
			if( pControl != NULL ) {
				IListItemUI* pItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
				if( pItem != NULL ) pItem->Expand(false);
			}
			m_iExpandedItem = -1;
		}
		if( bExpand ) {
			CControlUI* pControl = GetItemAt(iIndex);
			if( pControl == NULL ) return false;
			if( !pControl->IsVisible() ) return false;
			IListItemUI* pItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pItem == NULL ) return false;
			m_iExpandedItem = iIndex;
			if( !pItem->Expand(true) ) {
				m_iExpandedItem = -1;
				return false;
			}
		}
		NeedUpdate();
		return true;
	}

	int CListUI::GetExpandedItem() const
	{
		return m_iExpandedItem;
	}

	void CListUI::EnsureVisible(int iIndex)
	{
		RECT rcItem = m_pList->GetItemAt(iIndex)->GetPos();
		RECT rcList = m_pList->GetPos();
		RECT rcListInset = m_pList->GetInset();

		rcList.left += rcListInset.left;
		rcList.top += rcListInset.top;
		rcList.right -= rcListInset.right;
		rcList.bottom -= rcListInset.bottom;

		CScrollBarUI* pHorizontalScrollBar = m_pList->GetHorizontalScrollBar();
		if( pHorizontalScrollBar && pHorizontalScrollBar->IsVisible() ) rcList.bottom -= pHorizontalScrollBar->GetFixedHeight();

		int iPos = m_pList->GetScrollPos().cy;
		if( rcItem.top >= rcList.top && rcItem.bottom < rcList.bottom ) return;
		int dx = 0;
		if( rcItem.top < rcList.top ) dx = rcItem.top - rcList.top;
		if( rcItem.bottom > rcList.bottom ) dx = rcItem.bottom - rcList.bottom;
		Scroll(0, dx);
	}

	void CListUI::Scroll(int dx, int dy)
	{
		if( dx == 0 && dy == 0 ) return;
		SIZE sz = m_pList->GetScrollPos();
		m_pList->SetScrollPos(CDuiSize(sz.cx + dx, sz.cy + dy));
	}

	void CListUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("header")) == 0 ) GetHeader()->SetVisible(_tcscmp(pstrValue, _T("hidden")) != 0);
		else if( _tcscmp(pstrName, _T("headerbkimage")) == 0 )
		{
			if(GetHeader())
				GetHeader()->SetBkImage(pstrValue);
		}
		else if( _tcscmp(pstrName, _T("scrollselect")) == 0 ) SetScrollSelect(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("multiexpanding")) == 0 ) SetMultiExpanding(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("multipleitem")) == 0 ) SetMultipleItem(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("showvline")) == 0 ) SetShowVLine(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("showhline")) == 0 ) SetShowHLine(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("itemfont")) == 0 ) m_ListInfo.nFont = _ttoi(pstrValue);
		else if( _tcscmp(pstrName, _T("itemalign")) == 0 ) {
			if( _tcsstr(pstrValue, _T("left")) != NULL ) {
				m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
				m_ListInfo.uTextStyle |= DT_LEFT;
			}
			if( _tcsstr(pstrValue, _T("center")) != NULL ) {
				m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
				m_ListInfo.uTextStyle |= DT_CENTER;
			}
			if( _tcsstr(pstrValue, _T("right")) != NULL ) {
				m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
				m_ListInfo.uTextStyle |= DT_RIGHT;
			}
		}
		else if( _tcscmp(pstrName, _T("itemendellipsis")) == 0 ) {
			if( _tcscmp(pstrValue, _T("true")) == 0 ) m_ListInfo.uTextStyle |= DT_END_ELLIPSIS;
			else m_ListInfo.uTextStyle &= ~DT_END_ELLIPSIS;
		}    
		if( _tcscmp(pstrName, _T("itemtextpadding")) == 0 ) {
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetItemTextPadding(rcTextPadding);
		}
		else if( _tcscmp(pstrName, _T("itemcheckimgsize")) == 0 ) {
			SIZE szCheckImg;
			LPTSTR pstr = NULL;
			szCheckImg.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szCheckImg.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    		
			SetCheckImgSize(szCheckImg);
		}
		else if( _tcscmp(pstrName, _T("itemiconimgsize")) == 0 ) {
			SIZE szIconImg;
			LPTSTR pstr = NULL;
			szIconImg.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szIconImg.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    		
			SetIconImgSize(szIconImg);
		}
		else if( _tcscmp(pstrName, _T("itemtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itembkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemBkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itembkimage")) == 0 ) SetItemBkImage(pstrValue);
		else if( _tcscmp(pstrName, _T("itemaltbk")) == 0 ) SetAlternateBk(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("itemselectedtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedItemTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemselectedbkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedItemBkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemselectedimage")) == 0 ) SetSelectedItemImage(pstrValue);
		else if( _tcscmp(pstrName, _T("itemhottextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetHotItemTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemhotbkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetHotItemBkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemhotimage")) == 0 ) SetHotItemImage(pstrValue);
		else if( _tcscmp(pstrName, _T("itemdisabledtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetDisabledItemTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemdisabledbkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetDisabledItemBkColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemdisabledimage")) == 0 ) SetDisabledItemImage(pstrValue);
		else if( _tcscmp(pstrName, _T("itemlinecolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetItemLineColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("itemshowhtml")) == 0 ) SetItemShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
		else CVerticalLayoutUI::SetAttribute(pstrName, pstrValue);
	}

	IListCallbackUI* CListUI::GetTextCallback() const
	{
		return m_pCallback;
	}

	void CListUI::SetTextCallback(IListCallbackUI* pCallback)
	{
		m_pCallback = pCallback;
	}

	SIZE CListUI::GetScrollPos() const
	{
		return m_pList->GetScrollPos();
	}

	SIZE CListUI::GetScrollRange() const
	{
		return m_pList->GetScrollRange();
	}

	void CListUI::SetScrollPos(SIZE szPos)
	{
		m_pList->SetScrollPos(szPos);
	}

	void CListUI::LineUp()
	{
		m_pList->LineUp();
	}

	void CListUI::LineDown()
	{
		m_pList->LineDown();
	}

	void CListUI::PageUp()
	{
		m_pList->PageUp();
	}

	void CListUI::PageDown()
	{
		m_pList->PageDown();
	}

	void CListUI::HomeUp()
	{
		m_pList->HomeUp();
	}

	void CListUI::EndDown()
	{
		m_pList->EndDown();
	}

	void CListUI::LineLeft()
	{
		m_pList->LineLeft();
	}

	void CListUI::LineRight()
	{
		m_pList->LineRight();
	}

	void CListUI::PageLeft()
	{
		m_pList->PageLeft();
	}

	void CListUI::PageRight()
	{
		m_pList->PageRight();
	}

	void CListUI::HomeLeft()
	{
		m_pList->HomeLeft();
	}

	void CListUI::EndRight()
	{
		m_pList->EndRight();
	}

	void CListUI::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
	{
		if(m_pList)
		m_pList->EnableScrollBar(bEnableVertical, bEnableHorizontal);
	}

	CScrollBarUI* CListUI::GetVerticalScrollBar() const
	{
		return m_pList->GetVerticalScrollBar();
	}

	CScrollBarUI* CListUI::GetHorizontalScrollBar() const
	{
		return m_pList->GetHorizontalScrollBar();
	}
	
	bool CListUI::SelectMultiItem( int iIndex, bool bTakeFocus /*= false*/ )
	{
	
		if (m_bSingleSel)
		{
			return SelectItem(iIndex, bTakeFocus);
		}

		if( iIndex < 0 ) return false;
		CControlUI* pControl = GetItemAt(iIndex);
		if( pControl == NULL ) return false;
		if( !pControl->IsVisible() ) return false;
		if( !pControl->IsEnabled() ) return false;
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem == NULL ) return false;

		if (m_aSelItems.Find((LPVOID)iIndex) >= 0)
			return false;

		if(m_bSingleSel && m_aSelItems.GetSize() > 0) {
			CControlUI* pControl = GetItemAt((int)m_aSelItems.GetAt(0));
			if( pControl != NULL) {
				IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
				if( pListItem != NULL ) pListItem->Select(false);
			}		
		}	

		if( !pListItem->Select(true) ) {		
			return false;
		}

		m_aSelItems.Add((LPVOID)iIndex);

		EnsureVisible(iIndex);
		if( bTakeFocus ) pControl->SetFocus();
		if( m_pManager != NULL ) {
			m_pManager->SendNotify(this, _T("itemselect"), iIndex);
		}

		return true;
	}
	
	void CListUI::SetSingleSelect( bool bSingleSel )
	{
	
		m_bSingleSel = bSingleSel;
		UnSelectAllItems();
	
	}

	bool CListUI::GetSingleSelect() const
	{
	
		return m_bSingleSel;
		
	}
	
	bool CListUI::UnSelectItem( int iIndex )
	{
		if( iIndex < 0 ) return false;
		CControlUI* pControl = GetItemAt(iIndex);
		if( pControl == NULL ) return false;
		if( !pControl->IsVisible() ) return false;
		if( !pControl->IsEnabled() ) return false;
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem == NULL ) return false;

		int aIndex = m_aSelItems.Find((LPVOID)iIndex);
		if (aIndex < 0)
			return false;

		if( !pListItem->Select(false) ) {		
			return false;
		}

		m_aSelItems.Remove(aIndex);

		return true;
	}
	
	void CListUI::SelectAllItems()
	{
		UnSelectAllItems();
		CControlUI* pControl;
		for (int i = 0; i < GetCount(); ++i)
		{
			pControl = GetItemAt(i);
			if(pControl == NULL)
				continue;
			if(!pControl->IsVisible())
				continue;
			if(!pControl->IsEnabled())
				continue;
			IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pListItem == NULL )
				continue;
			if( !pListItem->Select(true) )
				continue;
			m_aSelItems.Add((LPVOID)i);
		}
	}
	
	void CListUI::UnSelectAllItems()
	{
		CControlUI* pControl;
		int itemIndex;
		for (int i = 0; i < m_aSelItems.GetSize(); ++i)
		{
			itemIndex = (int)m_aSelItems.GetAt(i);
			pControl = GetItemAt(itemIndex);
			if(pControl == NULL)
				continue;
			if(!pControl->IsVisible())
				continue;
			if(!pControl->IsEnabled())
				continue;
			IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
			if( pListItem == NULL )
				continue;
			if( !pListItem->Select(false) )
				continue;		
		}
		m_aSelItems.Empty();
	}
	
	int CListUI::GetSelectItemCount() const
	{
		return m_aSelItems.GetSize();
	}
	
	int CListUI::GetNextSelItem( int nItem ) const
	{
		if (m_aSelItems.GetSize() <= 0)
			return -1;

		if (nItem < 0)
		{
			return (int)m_aSelItems.GetAt(0);
		}
		int aIndex = m_aSelItems.Find((LPVOID)nItem);
		if (aIndex < 0)
			return -1;
		if (aIndex + 1 > m_aSelItems.GetSize() - 1)
			return -1;
		return (int)m_aSelItems.GetAt(aIndex + 1);
	}

	void CListUI::SetCheckImgSize( SIZE szCheckImg )
	{
		m_ListInfo.szCheckImg = szCheckImg;
	}

	void CListUI::SetIconImgSize( SIZE szIconImg )
	{
		m_ListInfo.szIconImg = szIconImg;
		
	}
	
	
	void CListUI::SetShowVLine( bool bVLine )
	{
		m_ListInfo.bShowVLine = bVLine;
	}

	
	void CListUI::SetShowHLine( bool bHLine )
	{
		m_ListInfo.bShowHLine = bHLine;
	}

	
	SIZE CListUI::GetCheckImgSize() const
	{
		return m_ListInfo.szCheckImg;
	}

	SIZE CListUI::GetIconImgSize() const
	{
		return m_ListInfo.szIconImg;
		
	}
	
	bool CListUI::IsShowVLine() const
	{
		return m_ListInfo.bShowVLine;
	}

	bool CListUI::IsShowHLine() const
	{
		return m_ListInfo.bShowHLine;
	}

	void CListUI::SortItems(CompareFunc pfnCompare,bool bContainer)
	{
		if (!m_pList||!pfnCompare)
			return ;
		m_pList->SortItems(pfnCompare,bContainer);	
	}

	void CListUI::SetMultipleItem( bool bMultipleable )
	{
		m_bSingleSel = !bMultipleable;
	}
	
	bool CListUI::GetMultipleItem() const
	{
		return !m_bSingleSel;
	}



	/////////////////////////////////////////////////////////////////////////////////////
	//
	//


	CListBodyUI::CListBodyUI(CListUI* pOwner) : m_pOwner(pOwner)
	{
		ASSERT(m_pOwner);
	}

	void CListBodyUI::SetScrollPos(SIZE szPos)
	{
		int cx = 0;
		int cy = 0;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) 
		{
			int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
			m_pVerticalScrollBar->SetScrollPos(szPos.cy);
			cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
		{
			int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
			m_pHorizontalScrollBar->SetScrollPos(szPos.cx);
			cx = m_pHorizontalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if( cx == 0 && cy == 0 ) return;

		RECT rcPos;
		for( int it2 = 0; it2 < m_items.GetSize(); it2++ )
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
			if( !pControl->IsVisible() ) continue;
			if( pControl->IsFloat() ) continue;

			rcPos = pControl->GetPos();
			rcPos.left -= cx;
			rcPos.right -= cx;
			rcPos.top -= cy;
			rcPos.bottom -= cy;
			pControl->SetPos(rcPos);
		}

		Invalidate();

		if( cx != 0 && m_pOwner ) {
			CListHeaderUI* pHeader = m_pOwner->GetHeader();
			if( pHeader == NULL ) return;
			TListInfoUI* pInfo = m_pOwner->GetListInfo();
			pInfo->nColumns = MIN(pHeader->GetCount(), UILIST_MAX_COLUMNS);

			if( !pHeader->IsVisible() ) {
				for( int it = 0; it < pHeader->GetCount(); it++ ) {
					static_cast<CControlUI*>(pHeader->GetItemAt(it))->SetInternVisible(true);
				}
			}
			for( int i = 0; i < pInfo->nColumns; i++ ) {
				CControlUI* pControl = static_cast<CControlUI*>(pHeader->GetItemAt(i));
				if( !pControl->IsVisible() ) continue;
				if( pControl->IsFloat() ) continue;

				RECT rcPos = pControl->GetPos();
				rcPos.left -= cx;
				rcPos.right -= cx;
				pControl->SetPos(rcPos);
				pInfo->rcColumn[i] = pControl->GetPos();
			}
			if( !pHeader->IsVisible() ) {
				for( int it = 0; it < pHeader->GetCount(); it++ ) {
					static_cast<CControlUI*>(pHeader->GetItemAt(it))->SetInternVisible(false);
				}
			}
		}
	}
	

	void CListBodyUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		rc = m_rcItem;

		// Adjust for inset;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		// Determine the minimum size
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
				cyFixed += sz.cy;
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

		int cxNeeded = 0;
		if( m_pOwner ) 
		{
			CListHeaderUI* pHeader = m_pOwner->GetHeader();
			if(pHeader)
			{
				int nWidth = pHeader->GetWidth();
				if( pHeader != NULL && pHeader->GetCount() > 0 ) 
					cxNeeded = MAX(0, pHeader->GetPos().right-pHeader->GetPos().left);
				//由于cxNeeded不是通过 rc.width计算出来的 而是通过Header的大小计算的;
				//cxNeeded如果等于header那么说明 不显示水平滚动条,由于垂直滚动条存在 导致rc.right减去了滚动条宽度;
				//会导致最后ProcessScrollBar时cxNeeded总比rc.widht大一个滚动条宽度而导致水平滚动条不消失;
				if(m_pVerticalScrollBar&&m_pVerticalScrollBar->IsVisible()&&m_pVerticalScrollBar->IsShowTakePlace()&&cxNeeded <= nWidth)
					cxNeeded -= m_pVerticalScrollBar->GetFixedWidth();
			}
			else
				cxNeeded = szAvailable.cx;
		}

		// Place elements
		int cyNeeded = 0;
		int cyExpand = 0;
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
			szRemaining.cy -= sz.cy + rcPadding.top + rcPadding.bottom;
		}
		cyNeeded += (nEstimateNum - 1) * m_iChildPadding;

		ProcessScrollBar(rc,cxNeeded, cyNeeded);
	}

	void CListBodyUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) 
		{
			if( m_pOwner != NULL ) 
				m_pOwner->DoEvent(event);
			else 
				CControlUI::DoEvent(event);
		}
		event.bHandle = true;
		CVerticalLayoutUI::DoEvent(event);
	}


	bool cmp(CListElementUI *p1,CListElementUI *p2)
	{
		CListTextElementUI *t1 = (CListTextElementUI *)p1;
		CListTextElementUI *t2 = (CListTextElementUI *)p2;
		CDuiString sz1(t1->GetText(0));
		CDuiString sz2(t2->GetText(0));
		return  sz1 < sz2;
	}

	//优化:
	//将IsSortAble放到IListItem里面去 不用区分是否是Contianer
	//传出去的比较函数以指针作为参数
	//具体是List里面的哪种控件,ListElement,ListContainerElement或者Edit,或者Lable
	//均由外部对比函数时做interface的RTTI
	//好处:优化此处代码,方便了List添加各种乱七八糟项目时的排序,不限制单一种类排序,不同种类均放在最后加大了自由度
	//坏处:让暴露在外界的对比函数做RTTI需要调用者了解更多库信息
	void CListBodyUI::SortItems(CompareFunc pfnCompare,bool bContainer)
	{
		CStdPtrArray NewSelItems;
		if(bContainer)
		{
			vector<CListContainerElementUI *> VItems;
			vector<CControlUI *> VUnSortItems;
			for(int i = 0 ; i < GetCount() ; i++)
			{
				CControlUI *pControl = GetItemAt(i);
				CListContainerElementUI *pItem = static_cast<CListContainerElementUI *>(pControl->GetInterface(L"ListContainerElement"));
				if(pItem&&pItem->IsSortAble())
					VItems.push_back(pItem);
				else
					VUnSortItems.push_back(pControl);
			}
			sort(VItems.begin(),VItems.end(),pfnCompare);
			vector<CListContainerElementUI *>::iterator VItemsIt;
			int index = 0;
			for(VItemsIt = VItems.begin();VItemsIt!=VItems.end();VItemsIt++)
			{
				CListContainerElementUI *pItem = *VItemsIt;
				SetItemIndex((CControlUI *)pItem,index);
				pItem->SetIndex(index);
				if(pItem->IsSelected())
					NewSelItems.Add((LPVOID)index);
				index++;
			}
			vector<CControlUI *>::iterator VUnSortItemsIt;
			for(VUnSortItemsIt = VUnSortItems.begin();VUnSortItemsIt!=VUnSortItems.end();VUnSortItemsIt++)
			{
				CControlUI *pItem = *VUnSortItemsIt;
				SetItemIndex(pItem,index);
				if(pItem->GetInterface(L"ListContainerElement"))
				{
					((CListContainerElementUI*)pItem)->SetIndex(index);
					if(((CListContainerElementUI*)pItem)->IsSelected())
						NewSelItems.Add((LPVOID)index);
				}
				index++;
			}
		}
		else
		{
			vector<CListElementUI *> VItems;
			vector<CControlUI *> VUnSortItems;
			for(int i = 0 ; i < GetCount() ; i++)
			{
				CControlUI *pControl = GetItemAt(i);
				CListElementUI *pItem = static_cast<CListElementUI *>(pControl->GetInterface(L"ListElement"));
				if(pItem&&pItem->IsSortAble())
					VItems.push_back(pItem);
				else
					VUnSortItems.push_back(pControl);
			}
			sort(VItems.begin(),VItems.end(),pfnCompare);
			vector<CListElementUI *>::iterator VItemsIt;
			int index = 0;
			for(VItemsIt = VItems.begin();VItemsIt!=VItems.end();VItemsIt++)
			{
				CListElementUI *pItem = *VItemsIt;
				SetItemIndex((CControlUI *)pItem,index);
				pItem->SetIndex(index);
				if(pItem->IsSelected())
					NewSelItems.Add((LPVOID)index);
				index++;
			}
			vector<CControlUI *>::iterator VUnSortItemsIt;
			for(VUnSortItemsIt = VUnSortItems.begin();VUnSortItemsIt!=VUnSortItems.end();VUnSortItemsIt++)
			{
				CControlUI *pItem = *VUnSortItemsIt;
				SetItemIndex(pItem,index);
				if(pItem->GetInterface(L"ListElement"))
				{
					((CListElementUI*)pItem)->SetIndex(index);
					if(((CListElementUI*)pItem)->IsSelected())
						NewSelItems.Add((LPVOID)index);
				}
				index++;
			}
		}
		if(NewSelItems.GetSize())
			m_pOwner->SetSeleteItems(NewSelItems);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CListHeaderUI::CListHeaderUI()
	{
		
	}

	LPCTSTR CListHeaderUI::GetClass() const
	{
		return _T("ListHeaderUI");
	}

	LPVOID CListHeaderUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("ListHeader")) == 0 ) return this;
		return CHorizontalLayoutUI::GetInterface(pstrName);
	}

	void CListHeaderUI::SetOwner( CListUI *pOwner )
	{
		m_pOwner = pOwner;
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CListHeaderItemUI::CListHeaderItemUI() : m_bDragable(true), m_uButtonState(0), m_iSepWidth(4),
		m_uTextStyle(DT_VCENTER | DT_CENTER | DT_SINGLELINE), m_dwTextColor(0), m_iFont(-1), m_bShowHtml(false)
	{
		SetTextPadding(CDuiRect(2, 0, 2, 0));
		ptLastMouse.x = ptLastMouse.y = 0;
		SetMinWidth(16);
		m_bNeedCursor = true;
	}

	LPCTSTR CListHeaderItemUI::GetClass() const
	{
		return _T("ListHeaderItemUI");
	}

	LPVOID CListHeaderItemUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("ListHeaderItem")) == 0 ) return this;
		return CContainerUI::GetInterface(pstrName);
	}

	UINT CListHeaderItemUI::GetControlFlags() const
	{
		if( IsEnabled() && m_iSepWidth != 0 ) return IsWantCursor();
		else return 0;
	}

	void CListHeaderItemUI::SetEnabled(bool bEnable)
	{
		CContainerUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	bool CListHeaderItemUI::IsDragable() const
	{
		return m_bDragable;
	}

	void CListHeaderItemUI::SetDragable(bool bDragable)
	{
		m_bDragable = bDragable;
		if ( !m_bDragable ) m_uButtonState &= ~UISTATE_CAPTURED;
	}

	DWORD CListHeaderItemUI::GetSepWidth() const
	{
		return m_iSepWidth;
	}

	void CListHeaderItemUI::SetSepWidth(int iWidth)
	{
		m_iSepWidth = iWidth;
	}

	DWORD CListHeaderItemUI::GetTextStyle() const
	{
		return m_uTextStyle;
	}

	void CListHeaderItemUI::SetTextStyle(UINT uStyle)
	{
		m_uTextStyle = uStyle;
		Invalidate();
	}

	DWORD CListHeaderItemUI::GetTextColor() const
	{
		return m_dwTextColor;
	}


	void CListHeaderItemUI::SetTextColor(DWORD dwTextColor)
	{
		m_dwTextColor = dwTextColor;
	}

	RECT CListHeaderItemUI::GetTextPadding() const
	{
		return m_rcTextPadding;
	}

	void CListHeaderItemUI::SetTextPadding(RECT rc)
	{
		m_rcTextPadding = rc;
		Invalidate();
	}

	void CListHeaderItemUI::SetFont(int index)
	{
		m_iFont = index;
	}

	bool CListHeaderItemUI::IsShowHtml()
	{
		return m_bShowHtml;
	}

	void CListHeaderItemUI::SetShowHtml(bool bShowHtml)
	{
		if( m_bShowHtml == bShowHtml ) return;

		m_bShowHtml = bShowHtml;
		Invalidate();
	}

	LPCTSTR CListHeaderItemUI::GetNormalImage() const
	{
		return m_sNormalImage;
	}

	void CListHeaderItemUI::SetNormalImage(LPCTSTR pStrImage)
	{
		m_sNormalImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CListHeaderItemUI::GetHotImage() const
	{
		return m_sHotImage;
	}

	void CListHeaderItemUI::SetHotImage(LPCTSTR pStrImage)
	{
		m_sHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CListHeaderItemUI::GetPushedImage() const
	{
		return m_sPushedImage;
	}

	void CListHeaderItemUI::SetPushedImage(LPCTSTR pStrImage)
	{
		m_sPushedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CListHeaderItemUI::GetFocusedImage() const
	{
		return m_sFocusedImage;
	}

	void CListHeaderItemUI::SetFocusedImage(LPCTSTR pStrImage)
	{
		m_sFocusedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CListHeaderItemUI::GetSepImage() const
	{
		return m_sSepImage;
	}

	void CListHeaderItemUI::SetSepImage(LPCTSTR pStrImage)
	{
		m_sSepImage = pStrImage;
		Invalidate();
	}

	void CListHeaderItemUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("dragable")) == 0 ) SetDragable(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("sepwidth")) == 0 ) SetSepWidth(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("align")) == 0 ) {
			if( _tcsstr(pstrValue, _T("left")) != NULL ) {
				m_uTextStyle &= ~(DT_CENTER | DT_RIGHT);
				m_uTextStyle |= DT_LEFT;
			}
			if( _tcsstr(pstrValue, _T("center")) != NULL ) {
				m_uTextStyle &= ~(DT_LEFT | DT_RIGHT);
				m_uTextStyle |= DT_CENTER;
			}
			if( _tcsstr(pstrValue, _T("right")) != NULL ) {
				m_uTextStyle &= ~(DT_LEFT | DT_CENTER);
				m_uTextStyle |= DT_RIGHT;
			}
		}
		else if( _tcscmp(pstrName, _T("endellipsis")) == 0 ) {
			if( _tcscmp(pstrValue, _T("true")) == 0 ) m_uTextStyle |= DT_END_ELLIPSIS;
			else m_uTextStyle &= ~DT_END_ELLIPSIS;
		}    
		else if( _tcscmp(pstrName, _T("font")) == 0 ) SetFont(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("textcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetTextColor(clrColor);
		}
		else if( _tcscmp(pstrName, _T("textpadding")) == 0 ) {
			RECT rcTextPadding = { 0 };
			LPTSTR pstr = NULL;
			rcTextPadding.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetTextPadding(rcTextPadding);
		}
		else if( _tcscmp(pstrName, _T("showhtml")) == 0 ) SetShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("normalimage")) == 0 ) SetNormalImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hotimage")) == 0 ) SetHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("pushedimage")) == 0 ) SetPushedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("focusedimage")) == 0 ) SetFocusedImage(pstrValue);
		else if( _tcscmp(pstrName, _T("sepimage")) == 0 ) SetSepImage(pstrValue);
		else CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CListHeaderItemUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
		{
			if( !IsEnabled() ) return;
			RECT rcSeparator = GetThumbRect();
			if (m_iSepWidth>0)
				rcSeparator.left-=4;
			else
				rcSeparator.right+=4;
			if( ::PtInRect(&rcSeparator, event.ptMouse) ) {
				if( m_bDragable ) {
					m_uButtonState |= UISTATE_CAPTURED;
					ptLastMouse = event.ptMouse;
				}
			}
			else {
				m_uButtonState |= UISTATE_PUSHED;
				m_pManager->SendNotify(this, _T("headerclick"));
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 )
				m_pManager->SendNotify(this,_T("headerwidthchanged"));

			if((m_uButtonState & UISTATE_CAPTURED) != 0 ) 
			{
				m_uButtonState &= ~UISTATE_CAPTURED;
				if( GetParent() ) 
					GetParent()->NeedParentUpdate();
			}
			else if( (m_uButtonState & UISTATE_PUSHED) != 0 ) 
			{
				m_uButtonState &= ~UISTATE_PUSHED;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) 
			{
				RECT rc = m_rcItem;
				if( m_iSepWidth > 0 ) 
				{
					rc.right -= ptLastMouse.x - event.ptMouse.x;
					if( rc.right - rc.left > GetMinWidth() ) 
					{
						m_cxyFixed.cx = rc.right - rc.left;
						ptLastMouse = event.ptMouse;
						if( GetParent() ) 
							GetParent()->NeedParentUpdate();
					}
				}
			}
			return;
		}
		if( event.Type == UIEVENT_SETCURSOR )
		{
			RECT rcSeparator = GetThumbRect();
			if (m_iSepWidth>0)
				rcSeparator.left-=4;
			else
				rcSeparator.right+=4;
			if( IsEnabled() && m_bDragable && ::PtInRect(&rcSeparator, event.ptMouse) ) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
				return;
			}
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
		CContainerUI::DoEvent(event);
	}

	SIZE CListHeaderItemUI::EstimateSize(SIZE szAvailable)
	{
		if( m_cxyFixed.cy == 0 ) return CDuiSize(m_cxyFixed.cx, m_pManager->GetDefaultFontInfo()->tm.tmHeight + 14);
		return CControlUI::EstimateSize(szAvailable);
	}

	RECT CListHeaderItemUI::GetThumbRect() const
	{
		if( m_iSepWidth > 0 ) return CDuiRect(m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom);
		else return CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom);
	}

	void CListHeaderItemUI::PaintStatusImage()
	{
		if( IsFocused() ) m_uButtonState |= UISTATE_FOCUSED;
		else m_uButtonState &= ~ UISTATE_FOCUSED;

		if( (m_uButtonState & UISTATE_PUSHED) != 0 ) {
			if( m_sPushedImage.IsEmpty() && !m_sNormalImage.IsEmpty() ) DrawImage(m_sNormalImage);
			if( !DrawImage(m_sPushedImage) ) m_sPushedImage.Empty();
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( m_sHotImage.IsEmpty() && !m_sNormalImage.IsEmpty() ) DrawImage(m_sNormalImage);
			if( !DrawImage(m_sHotImage) ) m_sHotImage.Empty();
		}
		else if( (m_uButtonState & UISTATE_FOCUSED) != 0 ) {
			if( m_sFocusedImage.IsEmpty() && !m_sNormalImage.IsEmpty() ) DrawImage(m_sNormalImage);
			if( !DrawImage(m_sFocusedImage) ) m_sFocusedImage.Empty();
		}
		else {
			if( !m_sNormalImage.IsEmpty() ) {
				if( !DrawImage(m_sNormalImage) ) m_sNormalImage.Empty();
			}
		}

		if( !m_sSepImage.IsEmpty() ) {
			RECT rcThumb = GetThumbRect();
			rcThumb.left -= m_rcItem.left;
			rcThumb.top -= m_rcItem.top;
			rcThumb.right -= m_rcItem.left;
			rcThumb.bottom -= m_rcItem.top;
			if( !DrawImage(m_sSepImage,rcThumb)) m_sSepImage.Empty();
		}
	}

	void CListHeaderItemUI::PaintText()
	{
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();

		RECT rcText = m_rcItem;
		rcText.left += m_rcTextPadding.left;
		rcText.top += m_rcTextPadding.top;
		rcText.right -= m_rcTextPadding.right;
		rcText.bottom -= m_rcTextPadding.bottom;

		if( m_sText.IsEmpty() ) return;
		int nLinks = 0;
		if( m_bShowHtml )
			m_pManager->GetRenderCore()->DrawHtmlText( m_pManager, rcText, m_sText, m_dwTextColor, \
			NULL, NULL, nLinks, DT_SINGLELINE | m_uTextStyle,m_bTextGlow);
		else
			m_pManager->GetRenderCore()->DrawText(m_pManager, rcText, m_sText, m_dwTextColor, \
			m_iFont, DT_SINGLELINE | m_uTextStyle,m_bTextGlow);
	}

	void CListHeaderItemUI::SetPos( RECT rc )
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
			SIZE sz = pControl->EstimateSize(szAvailable);
			if( sz.cx == -1 )
				nAdjustables++;
			else 
			{
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				cxFixed += sz.cx;
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
				iAdjustable++;
				sz.cx = cxExpand;
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
			szRemaining.cx -= sz.cx + rcPadding.left + rcPadding.right;
		}
		cxNeeded += (nEstimateNum - 1) * m_iChildPadding;

		// Process the scrollbar
		ProcessScrollBar(rc, cxNeeded, szAvailable.cy);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CListElementUI::CListElementUI() : 
		m_iIndex(-1),
		m_pOwner(NULL), 
		m_bSelected(false),
		m_uButtonState(0)
	{
		m_bSortAble = true;
		m_bNeedRet = true;
	}

	LPCTSTR CListElementUI::GetClass() const
	{
		return _T("ListElementUI");
	}

	UINT CListElementUI::GetControlFlags() const
	{
		return IsWantReturn();
	}

	LPVOID CListElementUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("ListItem")) == 0 ) return static_cast<IListItemUI*>(this);
		if( _tcscmp(pstrName, _T("ListElement")) == 0 ) return static_cast<CListElementUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	IListOwnerUI* CListElementUI::GetOwner()
	{
		return m_pOwner;
	}

	void CListElementUI::SetOwner(CControlUI* pOwner)
	{
		m_pOwner = static_cast<IListOwnerUI*>(pOwner->GetInterface(_T("IListOwner")));
	}

	void CListElementUI::SetVisible(bool bVisible)
	{
		CControlUI::SetVisible(bVisible);
		if( !IsVisible() && m_bSelected)
		{
			m_bSelected = false;
			if( m_pOwner != NULL ) m_pOwner->SelectItem(-1);
		}
	}

	void CListElementUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	int CListElementUI::GetIndex() const
	{
		return m_iIndex;
	}

	void CListElementUI::SetIndex(int iIndex)
	{
		m_iIndex = iIndex;
	}

	void CListElementUI::Invalidate()
	{
		if( !IsVisible() ) return;

		if( GetParent() ) 
		{
			CContainerUI* pParentContainer = static_cast<CContainerUI*>(GetParent()->GetInterface(_T("Container")));
			if( pParentContainer ) 
			{
				RECT rc = pParentContainer->GetPos();
				RECT rcInset = pParentContainer->GetInset();
				rc.left += rcInset.left;
				rc.top += rcInset.top;
				rc.right -= rcInset.right;
				rc.bottom -= rcInset.bottom;
				CScrollBarUI* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
				if( pVerticalScrollBar && pVerticalScrollBar->IsTakePlace() ) rc.right -= pVerticalScrollBar->GetFixedWidth();
				CScrollBarUI* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
				if( pHorizontalScrollBar && pHorizontalScrollBar->IsTakePlace() ) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

				RECT invalidateRc = m_rcItem;
				if( !::IntersectRect(&invalidateRc, &m_rcItem, &rc)) 
					return;
				CControlUI* pParent = GetParent();
				RECT rcTemp;
				RECT rcParent;
				while( pParent = pParent->GetParent() )
				{
					rcTemp = invalidateRc;
					rcParent = pParent->GetPos();
					if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
						return;
				}
				if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
			}
			else 
				CControlUI::Invalidate();
		}
		else 
			CControlUI::Invalidate();
	}

	bool CListElementUI::Activate()
	{
		if( !CControlUI::Activate() ) return false;
		if( m_pManager != NULL ) m_pManager->SendNotify(this, _T("itemactivate"));
		return true;
	}

	bool CListElementUI::IsSelected() const
	{
		return m_bSelected;
	}

	bool CListElementUI::Select(bool bSelect)
	{
		if( !IsEnabled() ) return false;
		if( bSelect == m_bSelected ) return true;
		m_bSelected = bSelect;
		if( bSelect && m_pOwner != NULL ) m_pOwner->SelectItem(m_iIndex);
		Invalidate();

		return true;
	}

	bool CListElementUI::IsExpanded() const
	{
		return false;
	}

	bool CListElementUI::Expand(bool /*bExpand = true*/)
	{
		return false;
	}

	void CListElementUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}
		else if( event.Type == UIEVENT_DBLCLICK )
		{
			if( IsEnabled() ) {
				Activate();
				Invalidate();
			}
			return;
		}
		else if( event.Type == UIEVENT_KEYDOWN && IsEnabled() )
		{
			if( event.chKey == VK_RETURN ) {
				Activate();
				Invalidate();
				return;
			}
		}
		// An important twist: The list-item will send the event not to its immediate
		// parent but to the "attached" list. A list may actually embed several components
		// in its path to the item, but key-presses etc. needs to go to the actual list.
		if( m_pOwner != NULL ) m_pOwner->DoEvent(event); else CControlUI::DoEvent(event);
	}

	void CListElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("selected")) == 0 ) Select();
		else CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CListElementUI::DrawItemBk(const RECT& rcItem)
	{
		ASSERT(m_pOwner);
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		DWORD iBackColor = 0;
		if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) iBackColor = pInfo->dwBkColor;
		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			iBackColor = pInfo->dwHotBkColor;
		}
		if( IsSelected() ) {
			iBackColor = pInfo->dwSelectedBkColor;
		}
		if( !IsEnabled() ) {
			iBackColor = pInfo->dwDisabledBkColor;
		}
		if ( iBackColor != 0 ) {
			m_pManager->GetRenderCore()->DrawColor(m_rcItem,GetAdjustColor(iBackColor));
		}
		if( !IsEnabled() ) {
			if( !pInfo->sDisabledImage.IsEmpty() ) {
				if( !DrawImage(pInfo->sDisabledImage) ) pInfo->sDisabledImage.Empty();
				else return;
			}
		}
		if( IsSelected() ) {
			if( !pInfo->sSelectedImage.IsEmpty() ) {
				if( !DrawImage(pInfo->sSelectedImage) ) pInfo->sSelectedImage.Empty();
				else return;
			}
		}
		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( !pInfo->sHotImage.IsEmpty() ) {
				if( !DrawImage(pInfo->sHotImage) ) pInfo->sHotImage.Empty();
				else return;
			}
		}

		if( !m_sBkImage.IsEmpty() ) {
			if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) {
				if( !DrawImage(m_sBkImage) ) m_sBkImage.Empty();
			}
		}

		if( m_sBkImage.IsEmpty() ) {
			if( !pInfo->sBkImage.IsEmpty() ) {
				if( !DrawImage(pInfo->sBkImage) ) pInfo->sBkImage.Empty();
				else return;
			}
		}

		if ( pInfo->dwLineColor != 0 && pInfo->bShowHLine ) {
			RECT rcLine = { m_rcItem.left, m_rcItem.bottom, m_rcItem.right, m_rcItem.bottom };
			m_pManager->GetRenderCore()->DrawLine(rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
		}
	}

	void CListElementUI::SetSortAble( bool bSortAble )
	{
		m_bSortAble = bSortAble;
	}

	bool CListElementUI::IsSortAble()
	{
		return m_bSortAble;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CListLabelElementUI::CListLabelElementUI()
	{
	}

	LPCTSTR CListLabelElementUI::GetClass() const
	{
		return _T("ListLabelElementUI");
	}

	LPVOID CListLabelElementUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("ListLabelElement")) == 0 ) return static_cast<CListLabelElementUI*>(this);
		return CListElementUI::GetInterface(pstrName);
	}

	void CListLabelElementUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else CListElementUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
		{
			if( IsEnabled() ) {
				m_pManager->SendNotify(this, _T("itemclick"));
				Select();
				Invalidate();
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
			if( (m_uButtonState & UISTATE_HOT) != 0 ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		CListElementUI::DoEvent(event);
	}

	SIZE CListLabelElementUI::EstimateSize(SIZE szAvailable)
	{
		if( m_pOwner == NULL ) return CDuiSize(0, 0);

		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		SIZE cXY = m_cxyFixed;
		if( cXY.cy == 0 && m_pManager != NULL ) {
			cXY.cy = m_pManager->GetFontInfo(pInfo->nFont)->tm.tmHeight + 8;
			cXY.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
		}

		if( cXY.cx == 0 && m_pManager != NULL ) 
		{
			RECT rcText = { 0, 0, 9999, cXY.cy };
			cXY.cx = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right;        
		}

		return cXY;
	}

	void CListLabelElementUI::DoPaint(const RECT& rcPaint)
	{
		if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
		DrawItemBk(m_rcItem);
		DrawItemText(m_rcItem);
	}

	void CListLabelElementUI::DrawItemText(const RECT& rcItem)
	{
		if( m_sText.IsEmpty() ) return;

		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		DWORD iTextColor = pInfo->dwTextColor;
		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			iTextColor = pInfo->dwHotTextColor;
		}
		if( IsSelected() ) {
			iTextColor = pInfo->dwSelectedTextColor;
		}
		if( !IsEnabled() ) {
			iTextColor = pInfo->dwDisabledTextColor;
		}
		int nLinks = 0;
		RECT rcText = rcItem;
		rcText.left += pInfo->rcTextPadding.left;
		rcText.right -= pInfo->rcTextPadding.right;
		rcText.top += pInfo->rcTextPadding.top;
		rcText.bottom -= pInfo->rcTextPadding.bottom;

		if( pInfo->bShowHtml )
			m_pManager->GetRenderCore()->DrawHtmlText( m_pManager, rcText, m_sText, iTextColor, \
			NULL, NULL, nLinks, DT_SINGLELINE | pInfo->uTextStyle,m_bTextGlow);
		else
			m_pManager->GetRenderCore()->DrawText(m_pManager, rcText, m_sText, iTextColor, \
			pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle,m_bTextGlow);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CListTextElementUI::CListTextElementUI() : m_nLinks(0), m_nHoverLink(-1), m_pOwner(NULL)
	{
		::ZeroMemory(&m_rcLinks, sizeof(m_rcLinks));
		m_bNeedRet = true;
		m_bNeedCursor = true;
	}

	CListTextElementUI::~CListTextElementUI()
	{
		CDuiString* pText;
		for( int it = 0; it < m_aTexts.GetSize(); it++ ) 
		{
			pText = static_cast<CDuiString*>(m_aTexts[it]);
			SAFE_DELETE(pText)
		}
		m_uTextsStyle.clear();
		m_aTexts.Empty();
	}

	LPCTSTR CListTextElementUI::GetClass() const
	{
		return _T("ListTextElementUI");
	}

	LPVOID CListTextElementUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("ListTextElement")) == 0 ) return static_cast<CListTextElementUI*>(this);
		return CListLabelElementUI::GetInterface(pstrName);
	}

	UINT CListTextElementUI::GetControlFlags() const
	{
		return IsWantReturn() | ( (IsEnabled() && m_nLinks > 0) ? IsWantCursor() : 0);
	}

	LPCTSTR CListTextElementUI::GetText(int iIndex) const
	{
		CDuiString* pText = static_cast<CDuiString*>(m_aTexts.GetAt(iIndex));
		if( pText ) return pText->GetData();
		return NULL;
	}

	void CListTextElementUI::SetText(int iIndex, LPCTSTR pstrText,int uTextStyle)
	{
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( m_aTexts.GetSize() < pInfo->nColumns ) 
		{ 
			m_aTexts.Add(NULL);
			m_uTextsStyle.push_back(-1);
		}

		CDuiString* pText = static_cast<CDuiString*>(m_aTexts[iIndex]);
		if( (pText == NULL && pstrText == NULL) || (pText && *pText == pstrText) ) return;

		if ( pText )
			pText->Assign(pstrText);
		else
			m_aTexts.SetAt(iIndex, new CDuiString(pstrText));

		m_uTextsStyle[iIndex] = uTextStyle;
		Invalidate();
	}

	void CListTextElementUI::SetOwner(CControlUI* pOwner)
	{
		CListElementUI::SetOwner(pOwner);
		m_pOwner = static_cast<IListUI*>(pOwner->GetInterface(_T("IList")));
	}

	CDuiString* CListTextElementUI::GetLinkContent(int iIndex)
	{
		if( iIndex >= 0 && iIndex < m_nLinks ) return &m_sLinks[iIndex];
		return NULL;
	}

	void CListTextElementUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else CListLabelElementUI::DoEvent(event);
			return;
		}
		// When you hover over a link
		else if( event.Type == UIEVENT_SETCURSOR ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
					return;
				}
			}      
		}
		else if( event.Type == UIEVENT_BUTTONUP && IsEnabled() ) {
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					m_pManager->SendNotify(this, _T("link"), i);
					return;
				}
			}
		}
		else if( m_nLinks > 0 && event.Type == UIEVENT_MOUSEMOVE ) {
			int nHoverLink = -1;
			for( int i = 0; i < m_nLinks; i++ ) {
				if( ::PtInRect(&m_rcLinks[i], event.ptMouse) ) {
					nHoverLink = i;
					break;
				}
			}

			if(m_nHoverLink != nHoverLink) {
				Invalidate();
				m_nHoverLink = nHoverLink;
			}
		}
		else if( m_nLinks > 0 && event.Type == UIEVENT_MOUSELEAVE ) {
			if(m_nHoverLink != -1) {
				Invalidate();
				m_nHoverLink = -1;
			}
		}
		else if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
		{
			if( IsEnabled() ) {					
				m_pManager->SendNotify(this, _T("itemclick"));						
				if (m_pOwner)
				{
					if (m_pOwner->GetSingleSelect())
					{
						if (!IsSelected())
							m_pOwner->SelectItem(m_iIndex);
					}				
					else
					{
						if ((GetKeyState(VK_CONTROL) & 0x8000))
						{	
							if (IsSelected())
								m_pOwner->UnSelectItem(m_iIndex);
							else
								m_pOwner->SelectMultiItem(m_iIndex);
						}
						else if((GetKeyState(VK_SHIFT) & 0x8000))
						{
							int iShiftDownIndex = m_pOwner->GetLastSingleSel();
							if(iShiftDownIndex != -1)
							{
								m_pOwner->UnSelectAllItems();
								if(m_iIndex < iShiftDownIndex)
								{
									for(int i = m_iIndex ; i <= iShiftDownIndex ; i++)
										m_pOwner->SelectMultiItem(i);
								}
								else
								{
									for(int i = iShiftDownIndex ; i <= m_iIndex ; i++)
										m_pOwner->SelectMultiItem(i);
								}
							}
						}
						else
						{
							if ((m_pOwner->GetSelectItemCount() == 1 && IsSelected()))
							{								
							}						
							else
							{
								m_pOwner->SetLastSingleSel(m_iIndex);
								m_pOwner->SelectItem(m_iIndex);
							}
						}					
					}
				}
				Invalidate();
			}
			m_pOwner->DoEvent(event);
			return;
		}
		CListLabelElementUI::DoEvent(event);
	}

	SIZE CListTextElementUI::EstimateSize(SIZE szAvailable)
	{
		TListInfoUI* pInfo = NULL;
		if( m_pOwner ) pInfo = m_pOwner->GetListInfo();

		SIZE cXY = m_cxyFixed;
		if( cXY.cy == 0 && m_pManager != NULL ) {
			cXY.cy = m_pManager->GetFontInfo(pInfo->nFont)->tm.tmHeight + 8;
			if( pInfo ) cXY.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
		}

		return cXY;
	}

	void CListTextElementUI::DrawItemText(const RECT& rcItem)
	{
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		DWORD iTextColor = pInfo->dwTextColor;

		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			iTextColor = pInfo->dwHotTextColor;
		}
		if( IsSelected() ) {
			iTextColor = pInfo->dwSelectedTextColor;
		}
		if( !IsEnabled() ) {
			iTextColor = pInfo->dwDisabledTextColor;
		}
		IListCallbackUI* pCallback = m_pOwner->GetTextCallback();

		m_nLinks = 0;
		int vLineColumns = pInfo->nColumns - 1;
		int nLinks = lengthof(m_rcLinks);
		for( int i = 0; i < pInfo->nColumns && m_uTextsStyle.size(); i++ )
		{
			int nTextAlgin = m_uTextsStyle[i];
			if(nTextAlgin < 0)
				nTextAlgin = DT_SINGLELINE | pInfo->uTextStyle;

			RECT rcItem = { pInfo->rcColumn[i].left, m_rcItem.top, pInfo->rcColumn[i].right, m_rcItem.bottom };
			RECT rcItemLine = rcItem;
			rcItem.left += pInfo->rcTextPadding.left;
			rcItem.right -= pInfo->rcTextPadding.right;
			rcItem.top += pInfo->rcTextPadding.top;
			rcItem.bottom -= pInfo->rcTextPadding.bottom;

			CDuiString strText;
			if( pCallback ) strText = pCallback->GetItemText(this, m_iIndex, i);
			else strText.Assign(GetText(i));
			if( pInfo->bShowHtml )
				m_pManager->GetRenderCore()->DrawHtmlText( m_pManager, rcItem, strText.GetData(), iTextColor, \
				&m_rcLinks[m_nLinks], &m_sLinks[m_nLinks], nLinks, nTextAlgin,m_bTextGlow);
			else
				m_pManager->GetRenderCore()->DrawText(m_pManager, rcItem, strText.GetData(), iTextColor, \
				pInfo->nFont,nTextAlgin,m_bTextGlow);

			m_nLinks += nLinks;
			nLinks = lengthof(m_rcLinks) - m_nLinks; 

			if(pInfo->dwLineColor != 0 && pInfo->bShowVLine && i < vLineColumns)
			{
				RECT nRc;
				nRc.left	= rcItemLine.right;
				nRc.top		= rcItemLine.top;
				nRc.right	= rcItemLine.right;
				nRc.bottom	= rcItemLine.bottom;

				m_pManager->GetRenderCore()->DrawLine(nRc,1,pInfo->dwLineColor);
			}
		}
		for( int i = m_nLinks; i < lengthof(m_rcLinks); i++ ) {
			::ZeroMemory(m_rcLinks + i, sizeof(RECT));
			((CDuiString*)(m_sLinks + i))->Empty();
		}
	}

	bool CListTextElementUI::Select( bool bSelect /*= true*/ )
	{
		try
		{
			if( !IsEnabled() )
				return false;
			if( bSelect == m_bSelected )
				return true;

			m_bSelected = bSelect;
			Invalidate();

			return true;
		}
		catch (...)
		{
			throw "CListTextElementUI::Select";
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//


	CListImageTextElementUI::CListImageTextElementUI() : m_aStartCheck(sizeof(bool)), m_aCheck(sizeof(bool))
	{
	}

	
	CListImageTextElementUI::~CListImageTextElementUI()
	{
		CDuiImage* pText;

		for( int it = 0; it < m_aImgNor.GetSize(); it++ ) {
			pText = static_cast<CDuiImage*>(m_aImgNor[it]);
			if( pText ) delete pText;
		}
		m_aImgNor.Empty();

		for( int it = 0; it < m_aImgSel.GetSize(); it++ ) {
			pText = static_cast<CDuiImage*>(m_aImgSel[it]);
			if( pText ) delete pText;
		}
		m_aImgSel.Empty();

		for( int it = 0; it < m_aImgNorCheck.GetSize(); it++ ) {
			pText = static_cast<CDuiImage*>(m_aImgNorCheck[it]);
			if( pText ) delete pText;
		}
		m_aImgNorCheck.Empty();

		for( int it = 0; it < m_aImgSelCheck.GetSize(); it++ ) {
			pText = static_cast<CDuiImage*>(m_aImgSelCheck[it]);
			if( pText ) delete pText;
		}
		m_aImgSelCheck.Empty();
	}

	LPCTSTR CListImageTextElementUI::GetClass() const
	{
		return _T("ListImageTextElementUI");
	}

	
	LPVOID CListImageTextElementUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("ListImageTextElementUI")) == 0 ) return static_cast<CListImageTextElementUI*>(this);
		return CListTextElementUI::GetInterface(pstrName);
	}

	
	CDuiImage* CListImageTextElementUI::GetNorImg(int iIndex) const
	{
		CDuiImage* pText = static_cast<CDuiImage*>(m_aImgNor.GetAt(iIndex));
		if( pText ) return pText;
		return NULL;
	}

	CDuiImage* CListImageTextElementUI::GetSelImg(int iIndex) const
	{
		CDuiImage* pText = static_cast<CDuiImage*>(m_aImgSel.GetAt(iIndex));
		if( pText ) return pText;
		return NULL;
	}

	void CListImageTextElementUI::SetNorImg(int iIndex, LPCTSTR pstrText)
	{
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( m_aImgNor.GetSize() < pInfo->nColumns ) { m_aImgNor.Add(NULL); }

		CDuiImage* pText = static_cast<CDuiImage*>(m_aImgNor[iIndex]);
		if( (pText == NULL && pstrText == NULL) || (pText && *pText == pstrText) ) return;

		if ( pText )
			pText->SetImage(pstrText);
		else{
			CDuiImage* pImage = new CDuiImage();
			pImage->SetImage(pstrText);
			m_aImgNor.SetAt(iIndex,pImage);
		}
		Invalidate();
	}

	void CListImageTextElementUI::SetSelImg(int iIndex, LPCTSTR pstrText)
	{
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( m_aImgSel.GetSize() < pInfo->nColumns ) { m_aImgSel.Add(NULL); }

		CDuiImage* pText = static_cast<CDuiImage*>(m_aImgSel[iIndex]);
		if( (pText == NULL && pstrText == NULL) || (pText && *pText == pstrText) ) return;

		if ( pText ) 
			pText->SetImage(pstrText);
		else{
			CDuiImage* pImage = new CDuiImage();
			pImage->SetImage(pstrText);
			m_aImgSel.SetAt(iIndex,pImage);
		}
		Invalidate();
	}

	void CListImageTextElementUI::SetCheckFlag(int iIndex, bool bStartCheck /*= TRUE*/)
	{
		if( m_pOwner == NULL ) return;
		bool bDefault = false;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( m_aStartCheck.GetSize() < pInfo->nColumns ) { m_aStartCheck.Add((LPVOID)(&bDefault)); }

		bool *pValue = (bool*)m_aStartCheck.GetAt(iIndex);	
		*pValue = bStartCheck;
	}

	bool CListImageTextElementUI::GetCheckFlag(int iIndex) const
	{
		bool *pValue = (bool*)m_aStartCheck.GetAt(iIndex);
		if (pValue) return *pValue;
		return false;
	}

	void CListImageTextElementUI::SetCheck(int iIndex, bool bCheck)
	{
		if( m_pOwner == NULL ) return;
		bool bDefault = false;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( m_aCheck.GetSize() < pInfo->nColumns ) { m_aCheck.Add((LPVOID)(&bDefault)); }

		bool *pValue = (bool*)m_aCheck.GetAt(iIndex);	
		*pValue = bCheck;
	}

	bool CListImageTextElementUI::GetCheck(int iIndex) const
	{
		bool *pValue = (bool*)m_aCheck.GetAt(iIndex);
		if (pValue) return *pValue;
		return false;
	}

	CDuiImage* CListImageTextElementUI::GetNorCheckImg(int iIndex) const
	{
		CDuiImage* pText = static_cast<CDuiImage*>(m_aImgNorCheck.GetAt(iIndex));
		if( pText ) return pText;
		return NULL;
	}

	CDuiImage* CListImageTextElementUI::GetSelCheckImg(int iIndex) const
	{
		CDuiImage* pText = static_cast<CDuiImage*>(m_aImgSelCheck.GetAt(iIndex));
		if( pText ) return pText;
		return NULL;
	}

	void CListImageTextElementUI::SetNorCheckImg(int iIndex, LPCTSTR pstrText)
	{
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( m_aImgNorCheck.GetSize() < pInfo->nColumns ) { m_aImgNorCheck.Add(NULL); }

		CDuiImage* pText = static_cast<CDuiImage*>(m_aImgNorCheck[iIndex]);
		if( (pText == NULL && pstrText == NULL) || (pText && *pText == pstrText) ) return;

		if ( pText )
			pText->SetImage(pstrText);
		else{
			CDuiImage* pImage = new CDuiImage();
			pImage->SetImage(pstrText);
			m_aImgNorCheck.SetAt(iIndex,pImage);
		}
		Invalidate();
	}

	void CListImageTextElementUI::SetSelCheckImg(int iIndex, LPCTSTR pstrText)
	{
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		if( iIndex < 0 || iIndex >= pInfo->nColumns ) return;
		while( m_aImgSelCheck.GetSize() < pInfo->nColumns ) { m_aImgSelCheck.Add(NULL); }

		CDuiString* pText = static_cast<CDuiString*>(m_aImgSelCheck[iIndex]);
		if( (pText == NULL && pstrText == NULL) || (pText && *pText == pstrText) ) return;

		if ( pText ) 
			pText->Assign(pstrText);
		else
			m_aImgSelCheck.SetAt(iIndex, new CDuiString(pstrText));
		Invalidate();
	}

	RECT CListImageTextElementUI::GetItemRect(int iIndex)
	{
		TListInfoUI* pInfo = m_pOwner->GetListInfo();		
		RECT rcRes = { pInfo->rcColumn[iIndex].left, m_rcItem.top, pInfo->rcColumn[iIndex].right, m_rcItem.bottom };
		return rcRes;
	}

	RECT CListImageTextElementUI::GetItemRectPadding(int iIndex)
	{	
		TListInfoUI* pInfo = m_pOwner->GetListInfo();	
		RECT rcRes = { pInfo->rcColumn[iIndex].left, m_rcItem.top, pInfo->rcColumn[iIndex].right, m_rcItem.bottom };
		rcRes.left += pInfo->rcTextPadding.left;
		rcRes.right -= pInfo->rcTextPadding.right;
		rcRes.top += pInfo->rcTextPadding.top;
		rcRes.bottom -= pInfo->rcTextPadding.bottom;
		return rcRes;
	}

	RECT CListImageTextElementUI::GetItemCheckRect(int iIndex)
	{
		RECT rcItemP = GetItemRectPadding(iIndex);
		RECT rcRes = {0, 0, 0, 0};
		if (IsRectEmpty(&rcItemP) || !GetCheckFlag(iIndex))
			return rcRes;	
		TListInfoUI* pInfo = m_pOwner->GetListInfo();	
		rcRes = rcItemP;	
		rcRes.right = rcRes.left + pInfo->szCheckImg.cx;
		if (rcRes.right > rcItemP.right)
			rcRes.right = rcItemP.right;	
		if (rcRes.left > rcRes.right)
			rcRes.left = rcRes.right;
		rcRes.top = rcRes.top + (rcItemP.bottom - rcItemP.top - pInfo->szCheckImg.cy) / 2;
		rcRes.bottom = rcRes.top + pInfo->szCheckImg.cy;
		if (rcRes.bottom > rcItemP.bottom)
			rcRes.bottom = rcItemP.bottom;
		if (rcRes.top > rcRes.bottom)
			rcRes.bottom = rcRes.top;

		return rcRes;
	}

	RECT CListImageTextElementUI::GetItemImgRect(int iIndex)
	{
		RECT rcItemP = GetItemRectPadding(iIndex);	
		RECT rcRes = {0, 0, 0, 0};
		if (IsRectEmpty(&rcItemP) || (!GetNorImg(iIndex) && !GetSelImg(iIndex)))
			return rcRes;	
		TListInfoUI* pInfo = m_pOwner->GetListInfo();	
		RECT rcItemCheck = GetItemCheckRect(iIndex);	
		rcRes = rcItemP;	
		if (!IsRectEmpty(&rcItemCheck))
		{
			rcRes.left = rcItemCheck.right + 2;
		}	
		rcRes.right = rcRes.left + pInfo->szIconImg.cx;
		if (rcRes.right > rcItemP.right)
			rcRes.right = rcItemP.right;	
		if (rcRes.left > rcRes.right)
			rcRes.left = rcRes.right;
		rcRes.top = rcRes.top + (rcItemP.bottom - rcItemP.top - pInfo->szIconImg.cy) / 2;
		rcRes.bottom = rcRes.top + pInfo->szIconImg.cy;
		if (rcRes.bottom > rcItemP.bottom)
			rcRes.bottom = rcItemP.bottom;
		if (rcRes.top > rcRes.bottom)
			rcRes.bottom = rcRes.top;

		return rcRes;
	}

	RECT CListImageTextElementUI::GetItemTextRect(int iIndex)
	{
		RECT rcRes = GetItemRectPadding(iIndex);
		RECT rcImg = GetItemImgRect(iIndex);
		RECT rcCheck =GetItemCheckRect(iIndex);
		if (!IsRectEmpty(&rcImg))
		{
			rcRes.left = rcImg.right + 2;
			if (rcRes.left > rcRes.right)
				rcRes.left = rcRes.right;
		}
		else
		{
			if (!IsRectEmpty(&rcCheck))
			{
				rcRes.left = rcCheck.right + 2;
				if (rcRes.left > rcRes.right)
					rcRes.left = rcRes.right;
			}
		}
		return rcRes;
	}

	int CListImageTextElementUI::HitTest(POINT pt, UINT* pFlags/* = NULL*/)
	{	
		if( m_pOwner == NULL ) return -1;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();	
		for( int i = 0; i < pInfo->nColumns; i++ )
		{
			RECT rc = GetItemRect(i);;		
			if (PtInRect(&rc, pt))
			{
				if (!pFlags)
					return i;
				*pFlags = 0;
				*pFlags |= ULVHT_ITEM;			
				rc = GetItemCheckRect(i);
				if (PtInRect(&rc, pt))
				{
					*pFlags |= ULVHT_CHECKBOX;
				}

				return i;
			}
		}

		return -1;
	}

	void CListImageTextElementUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else CListTextElementUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_BUTTONDOWN ) {
			SetFocus();

			if (IsEnabled())
			{
				UINT flags = 0;
				int iIndex = HitTest(event.ptMouse, &flags);
				if (flags & ULVHT_CHECKBOX && GetCheckFlag(iIndex))
				{			
					SetCheck(iIndex, !GetCheck(iIndex));
					Invalidate();
					m_pManager->SendNotify(this, _T("list_check_click"), m_iIndex, iIndex);
				}
			}
		}

		if( event.Type == UIEVENT_DBLCLICK )
		{
			if( IsEnabled() ) {		
				UINT flags = 0;
				int iIndex = HitTest(event.ptMouse, &flags);

				Invalidate();
				m_pManager->SendNotify(this, _T("list_item_dbclick"), m_iIndex, iIndex);
				return;
			}				
		}

		CListTextElementUI::DoEvent(event);
	}

	void CListImageTextElementUI::DrawItemText(const RECT& rcItem)
	{
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		DWORD iTextColor = pInfo->dwTextColor;

		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			iTextColor = pInfo->dwHotTextColor;
		}
		if( IsSelected() ) {
			iTextColor = pInfo->dwSelectedTextColor;
		}
		if( !IsEnabled() ) {
			iTextColor = pInfo->dwDisabledTextColor;
		}

		int vLineColumns = pInfo->nColumns -1;
		IListCallbackUI* pCallback = m_pOwner->GetTextCallback();
		for( int i = 0; i < pInfo->nColumns&&m_uTextsStyle.size(); i++ )
		{
			int nTextAlgin = m_uTextsStyle[i];
			if(nTextAlgin < 0)
				nTextAlgin = DT_SINGLELINE | pInfo->uTextStyle;

			if (GetCheckFlag(i))
			{
				RECT rcCheck = GetItemCheckRect(i);
				CDuiImage& pImg = GetCheck(i) ? *GetSelCheckImg(i) : *GetNorCheckImg(i);
				if (!pImg.IsEmpty())
					m_pManager->GetRenderCore()->DrawImageString(m_pManager,rcCheck,m_rcPaint,pImg);
			}

			RECT rcImg;
			rcImg = GetItemImgRect(i);		
			CDuiImage& pImg = IsSelected() ? *GetSelImg(i) : *GetNorImg(i);
			if (!pImg.IsEmpty())
				m_pManager->GetRenderCore()->DrawImageString(m_pManager,rcImg,m_rcPaint,pImg);

			RECT rcText = GetItemTextRect(i);
			RECT rcVLine = rcText;
			CDuiString strText;
			if( pCallback ) strText = pCallback->GetItemText(this, m_iIndex, i);
			else strText.Assign(GetText(i));
			if( pInfo->bShowHtml )
			{
				int nLinks = 0;
				m_pManager->GetRenderCore()->DrawHtmlText( m_pManager, rcText, strText.GetData(), iTextColor, \
					NULL, NULL, nLinks, nTextAlgin,m_bTextGlow);
			}
			else
				m_pManager->GetRenderCore()->DrawText(m_pManager, rcText, strText.GetData(), iTextColor, \
				pInfo->nFont, nTextAlgin,m_bTextGlow);

			if(pInfo->dwLineColor != 0 && pInfo->bShowVLine && i < vLineColumns)
			{
				RECT nRc;
				nRc.left	= rcVLine.right + pInfo->rcTextPadding.right;
				nRc.top		= rcVLine.top - pInfo->rcTextPadding.top;
				nRc.right	= nRc.left;
				nRc.bottom	= rcVLine.bottom + pInfo->rcTextPadding.top;

				m_pManager->GetRenderCore()->DrawLine(nRc,1,pInfo->dwLineColor);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	CListContainerElementUI::CListContainerElementUI() : 
		m_iIndex(-1),
		m_pOwner(NULL), 
		m_bSelected(false),
		m_bMultiExpand(false),
		m_uButtonState(0),
		m_dwSelectBorderColor(0xffffffff),
		m_nSelectBorderSize(0)
	{
		m_dwInnerBorderColor = 0xffffffff;
		m_nInnerBorderSize = 1;
		m_bNeedRet = true;
		m_bSortAble = true;
	}

	CListContainerElementUI::~CListContainerElementUI()
	{
		m_pOwner = NULL;
	}

	LPCTSTR CListContainerElementUI::GetClass() const
	{
		return _T("ListContainerElementUI");
	}

	UINT CListContainerElementUI::GetControlFlags() const
	{
		return IsWantReturn();
	}

	LPVOID CListContainerElementUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("ListItem")) == 0 ) return static_cast<IListItemUI*>(this);
		if( _tcscmp(pstrName, _T("ListContainerElement")) == 0 ) return static_cast<CListContainerElementUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	IListOwnerUI* CListContainerElementUI::GetOwner()
	{
		return m_pOwner;
	}

	void CListContainerElementUI::SetOwner(CControlUI* pOwner)
	{
		m_pOwner = static_cast<IListUI*>(pOwner->GetInterface(_T("IList")));
	}

	void CListContainerElementUI::SetVisible(bool bVisible)
	{
		CContainerUI::SetVisible(bVisible);
		if( !IsVisible() && m_bSelected)
		{
			m_bSelected = false;
			if( m_pOwner != NULL ) m_pOwner->SelectItem(-1);
		}
	}

	void CListContainerElementUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	int CListContainerElementUI::GetIndex() const
	{
		return m_iIndex;
	}

	void CListContainerElementUI::SetIndex(int iIndex)
	{
		m_iIndex = iIndex;
	}

	void CListContainerElementUI::Invalidate()
	{
		if( !IsVisible() ) return;

		if( GetParent() ) {
			CContainerUI* pParentContainer = static_cast<CContainerUI*>(GetParent()->GetInterface(_T("Container")));
			if( pParentContainer ) {
				RECT rc = pParentContainer->GetPos();
				RECT rcInset = pParentContainer->GetInset();
				rc.left += rcInset.left;
				rc.top += rcInset.top;
				rc.right -= rcInset.right;
				rc.bottom -= rcInset.bottom;
				CScrollBarUI* pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();
				if( pVerticalScrollBar && pVerticalScrollBar->IsTakePlace() ) rc.right -= pVerticalScrollBar->GetFixedWidth();
				CScrollBarUI* pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();
				if( pHorizontalScrollBar && pHorizontalScrollBar->IsTakePlace() ) rc.bottom -= pHorizontalScrollBar->GetFixedHeight();

				RECT invalidateRc = m_rcItem;
				if( !::IntersectRect(&invalidateRc, &m_rcItem, &rc) ) 
				{
					return;
				}

				CControlUI* pParent = GetParent();
				RECT rcTemp;
				RECT rcParent;
				while( pParent = pParent->GetParent() )
				{
					rcTemp = invalidateRc;
					rcParent = pParent->GetPos();
					if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
					{
						return;
					}
				}

				if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
			}
			else {
				CContainerUI::Invalidate();
			}
		}
		else {
			CContainerUI::Invalidate();
		}
	}

	bool CListContainerElementUI::Activate()
	{
		if( !CContainerUI::Activate() ) return false;
		if( m_pManager != NULL ) m_pManager->SendNotify(this, _T("itemactivate"));
		return true;
	}

	bool CListContainerElementUI::IsSelected() const
	{
		return m_bSelected;
	}

	bool CListContainerElementUI::Select(bool bSelect)
	{
		if( !IsEnabled() )
			return false;
		if( bSelect == m_bSelected )
			return true;

		m_bSelected = bSelect;
		Invalidate();

		return true;
	}

	bool CListContainerElementUI::IsExpanded() const
	{
		return false;
	}

	bool CListContainerElementUI::Expand(bool /*bExpand = true*/)
	{
		return false;
	}

	void CListContainerElementUI::MultiExpand(bool bMultiExpand)
	{
		m_bMultiExpand = bMultiExpand;
	}
	void CListContainerElementUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pOwner != NULL ) m_pOwner->DoEvent(event);
			else CContainerUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_DBLCLICK )
		{
			if( IsEnabled() ) {
				Activate();
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_KEYDOWN && IsEnabled() )
		{
			if( event.chKey == VK_RETURN ) {
				Activate();
				Invalidate();
				return;
			}
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
		{
			if( IsEnabled() &&IsMouseEnabled()) 
			{
				if (m_pOwner)
				{
					if (m_pOwner->GetSingleSelect())
					{
						if (!IsSelected())
							m_pOwner->SelectItem(m_iIndex);
					}				
					else
					{
						if ((GetKeyState(VK_CONTROL) & 0x8000))
						{	
							if (IsSelected())
								m_pOwner->UnSelectItem(m_iIndex);
							else
								m_pOwner->SelectMultiItem(m_iIndex);
						}
						else if((GetKeyState(VK_SHIFT) & 0x8000))
						{
							int iShiftDownIndex = m_pOwner->GetLastSingleSel();
							if(iShiftDownIndex != -1)
							{
								m_pOwner->UnSelectAllItems();
								if(m_iIndex < iShiftDownIndex)
								{
									for(int i = m_iIndex ; i <= iShiftDownIndex ; i++)
										m_pOwner->SelectMultiItem(i);
								}
								else
								{
									for(int i = iShiftDownIndex ; i <= m_iIndex ; i++)
										m_pOwner->SelectMultiItem(i);
								}
							}
						}
						else
						{
							if(!IsSelected())
							{
								m_pOwner->SetLastSingleSel(m_iIndex);
								m_pOwner->SelectItem(m_iIndex);
							}
							else
							{
								if(event.Type == UIEVENT_BUTTONDOWN && m_pOwner->GetSelectItemCount() != 1 )
								{
									m_pOwner->SetLastSingleSel(m_iIndex);
									m_pOwner->SelectItem(m_iIndex);
								}
							}
						}					
					}
				}
				Invalidate();
			}
			m_pOwner->DoEvent(event);
			m_pManager->SendNotify(this, _T("itemclick"));
			return;
		}
		if(event.Type ==UIEVENT_SCROLLWHEEL)
		{
			if(IsEnabled())
			{
				m_pOwner->DoEvent(event);
			return;
			}
		}
		if( event.Type == UIEVENT_BUTTONUP ) 
		{
			return CControlUI::DoEvent(event);
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return CControlUI::DoEvent(event);
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( (m_uButtonState & UISTATE_HOT) != 0 ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return CControlUI::DoEvent(event);
		}

		// An important twist: The list-item will send the event not to its immediate
		// parent but to the "attached" list. A list may actually embed several components
		// in its path to the item, but key-presses etc. needs to go to the actual list.
		if( m_pOwner != NULL ) m_pOwner->DoEvent(event); else CControlUI::DoEvent(event);
	}

	void CListContainerElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("selected")) == 0 ) Select();
		else CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CListContainerElementUI::DoPaint(const RECT& rcPaint)
	{
		if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
		DrawItemBk(m_rcItem);
		CContainerUI::DoPaint(rcPaint);
		DrawItemSelectBorder();
	}

	void CListContainerElementUI::DrawItemText(const RECT& rcItem)
	{
	}

	void CListContainerElementUI::DrawItemSelectBorder()
	{
		if(m_bSelected)
		{
			if(m_nSelectBorderSize)
			{
				m_pManager->GetRenderCore()->DrawRect(m_rcItem, m_nSelectBorderSize+m_nInnerBorderSize, GetAdjustColor(m_dwInnerBorderColor));
				m_pManager->GetRenderCore()->DrawRect(m_rcItem, m_nSelectBorderSize, GetAdjustColor(m_dwSelectBorderColor));
			}
		}
	}
	void CListContainerElementUI::SetInnerBorder(DWORD dwColor,int nSize)
	{
		m_dwInnerBorderColor = dwColor;
		m_nInnerBorderSize = nSize;
	}
	void CListContainerElementUI::SetSelectBorder(DWORD dwColor,int nSize)
	{
		m_nSelectBorderSize = nSize;
		m_dwSelectBorderColor = dwColor;
	}
	void CListContainerElementUI::DrawItemBk(const RECT& rcItem)
	{
		ASSERT(m_pOwner);
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		DWORD iBackColor = 0;
		if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) 
			iBackColor = pInfo->dwBkColor;

		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			iBackColor = pInfo->dwHotBkColor;
		}
		if( IsSelected() ) {
			iBackColor = pInfo->dwSelectedBkColor;
		}
		if( !IsEnabled() ) {
			iBackColor = pInfo->dwDisabledBkColor;
		}
		if ( iBackColor != 0 ) {
			m_pManager->GetRenderCore()->DrawColor(m_rcItem,GetAdjustColor(iBackColor));
		}
		if( !IsEnabled() ) {
			if( !pInfo->sDisabledImage.IsEmpty() ) {
				if( !DrawImage(pInfo->sDisabledImage) ) pInfo->sDisabledImage.Empty();
				else return;
			}
		}
		if( IsSelected() ) {
			if( !pInfo->sSelectedImage.IsEmpty() ) {
				if( !DrawImage(pInfo->sSelectedImage) ) pInfo->sSelectedImage.Empty();
				else return;
			}
		}
		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( !pInfo->sHotImage.IsEmpty() ) {
				if( !DrawImage(pInfo->sHotImage) ) pInfo->sHotImage.Empty();
				else return;
			}
		}
		if( !m_sBkImage.IsEmpty() ) {
			if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) {
				if( !DrawImage(m_sBkImage) ) m_sBkImage.Empty();
			}
		}

		if( m_sBkImage.IsEmpty() ) {
			if( !pInfo->sBkImage.IsEmpty() ) {
				if( !DrawImage(pInfo->sBkImage) ) pInfo->sBkImage.Empty();
				else return;
			}
		}

		if ( pInfo->dwLineColor != 0 && pInfo->bShowHLine ) {
			RECT rcLine = { m_rcItem.left, m_rcItem.bottom, m_rcItem.right, m_rcItem.bottom };
			m_pManager->GetRenderCore()->DrawLine(rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
		}
	}

	void CListContainerElementUI::SetPos( RECT rc )
	{
		CContainerUI::SetPos(rc);
		if (m_pOwner == NULL) return;
		CListHeaderUI *pHeader = ((CListUI *)m_pOwner)->GetHeader();
		if(!pHeader) return;
		int nCount = m_items.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			CControlUI *pHorizontalLayout = static_cast<CControlUI*>(m_items[i]);
			CListHeaderItemUI *pHeaderItem = static_cast<CListHeaderItemUI*>(pHeader->GetItemAt(i));
			if (pHorizontalLayout)
			{
				RECT rtHeader = pHeaderItem->GetPos();
				RECT rt = pHorizontalLayout->GetPos();
				rt.left = rtHeader.left;
				rt.right = rtHeader.right;
				pHorizontalLayout->SetPos(rt);
			}
		}
	}

	void CListContainerElementUI::SetSortAble( bool bSort )
	{
		m_bSortAble = bSort;
	}

	bool CListContainerElementUI::IsSortAble()
	{
		return m_bSortAble;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////
	CListContainerItemElementUI::CListContainerItemElementUI():m_bMouseEnterd(false)
	{
		pUpPanel = NULL;
		pDownPanel = NULL;
		m_pBkImg = NULL;
		m_pImg = NULL;
		m_pBtnItemDelete = NULL;
		m_pBtnLeftRotate = NULL;
		m_pBtnRightRotate = NULL;
		CControlUI::OnInit += MakeDelegate(this,&CListContainerItemElementUI::OnInit,_T(""));
	}

	CListContainerItemElementUI::~CListContainerItemElementUI()
	{
		if(m_pBkImg)
		{
			::DeleteObject(m_pBkImg->hBitmap);
			delete m_pBkImg;
			m_pBkImg = NULL;
		}
		if(m_pImg)
		{
			delete m_pImg;
			m_pImg = NULL;
		}
	}

	bool CListContainerItemElementUI::OnBtnDelete(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam)
	{
		if(pTNotifyUI->sType == _T("click"))
		{
			m_pManager->SendNotify(this,_T("itemdelete"));
		}
		return true;
	}
	bool CListContainerItemElementUI::OnBtnRightRotate(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam)
	{
		if(pTNotifyUI->sType == _T("click"))
		{
			m_pManager->SendNotify(this,_T("rightrotate"));
		}
		return true;
	}

	bool CListContainerItemElementUI::OnBtnLeftRotate(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam)
	{
		if(pTNotifyUI->sType == _T("click"))
		{
			m_pManager->SendNotify(this,_T("leftrotate"));
		}
		return true;
	}

	bool CListContainerItemElementUI::OnInit(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam)
	{
		pDownPanel = static_cast<CAnimationTabLayoutUI *>(m_pManager->FindSubControlByName(this,_T("DownPanel")));
		if(pDownPanel)
			pDownPanel->SetAnimationDirection(true);
		pUpPanel = static_cast<CAnimationTabLayoutUI *>(m_pManager->FindSubControlByName(this,_T("UpPanel")));
		if(pUpPanel)
			pUpPanel->SetAnimationDirection(true);
		
		m_pBtnItemDelete = static_cast<CButtonUI *>(m_pManager->FindSubControlByName(this,_T("ItemClose")));
		if(m_pBtnItemDelete) m_pBtnItemDelete->OnNotify += MakeDelegate(this,&CListContainerItemElementUI::OnBtnDelete,_T(""));
		m_pBtnLeftRotate = static_cast<CButtonUI *>(m_pManager->FindSubControlByName(this,_T("BtnLeftRotate")));;
		if(m_pBtnLeftRotate) m_pBtnLeftRotate->OnNotify += MakeDelegate(this,&CListContainerItemElementUI::OnBtnLeftRotate,_T(""));
		m_pBtnRightRotate = static_cast<CButtonUI *>(m_pManager->FindSubControlByName(this,_T("BtnRightRotate")));;
		if(m_pBtnRightRotate) m_pBtnRightRotate->OnNotify += MakeDelegate(this,&CListContainerItemElementUI::OnBtnRightRotate,_T(""));
		return true;
	}

	LPCTSTR CListContainerItemElementUI::GetClass() const
	{
		return _T("ListContainerItemElementUI");
	}

	LPVOID CListContainerItemElementUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("ListContainerItemElement")) == 0 ) return static_cast<CListContainerItemElementUI*>(this);
		return CListContainerElementUI::GetInterface(pstrName);
	}

	void CListContainerItemElementUI::SetOwner(CControlUI* pOwner)
	{
		CListContainerElementUI::SetOwner(pOwner);
		m_pOwner = static_cast<IListUI*>(pOwner->GetInterface(_T("IList")));
	}

	void CListContainerItemElementUI::DoEvent(TEventUI& event)
	{
		if(event.Type == UIEVENT_MOUSEENTER)
		{
			if(PtInRect(&m_rcItem,event.ptMouse)&&!m_bMouseEnterd)
			{
				m_bMouseEnterd = true;
				if(pDownPanel)
					pDownPanel->SelectItem(1);
				if(pUpPanel)
					pUpPanel->SelectItem(0);
			}
		}
		if(event.Type == UIEVENT_MOUSELEAVE)
		{
		
				if(!PtInRect(&m_rcItem,event.ptMouse)&&m_bMouseEnterd)
				{
					m_bMouseEnterd = false;
					if(pDownPanel)
						pDownPanel->SelectItem(0);
					if(pUpPanel)
						pUpPanel->SelectItem(1);
				}
			
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_RBUTTONDOWN )
		{
			if( IsEnabled() ) {
				m_pManager->SendNotify(this, _T("itemclick"));						
				if (m_pOwner)
				{
					if (m_pOwner->GetSingleSelect())
					{
						if (!IsSelected())
						{
							m_bSelected = true;
							m_pOwner->SelectItem(m_iIndex);
						}
					}
					else
					{
						if ((GetKeyState(VK_CONTROL) & 0x8000))
						{
							if (IsSelected())
							{
								m_bSelected = false;
								m_pOwner->UnSelectItem(m_iIndex);
							}
							else
								m_pOwner->SelectMultiItem(m_iIndex);
						}
						else
						{
							m_bSelected = true;
							m_pOwner->SelectItem(m_iIndex);	
						}					
					}
				}
			}
			return ;
		}
		CListContainerElementUI::DoEvent(event);
	}

	void CListContainerItemElementUI::SetItemImg(CDuiString dszPicPath)
	{
		if(m_pBkImg)
		{
			::DeleteObject(m_pBkImg->hBitmap);
			delete m_pBkImg;
			m_pBkImg = NULL;
		}
		if(m_pImg)
		{
			delete m_pImg;
			m_pImg = NULL;
		}
		m_pImg = new Bitmap(dszPicPath);
		HBITMAP hBitmap;
		m_pImg->GetHBITMAP(NULL,&hBitmap);
		m_pBkImg = new TImageInfo;
		m_pBkImg->hBitmap = hBitmap;
		m_pBkImg->nX = m_pImg->GetWidth();
		m_pBkImg->nY = m_pImg->GetHeight();
	}

	void CListContainerItemElementUI::SetItemImg(Bitmap *&pBitmap)
	{
		if(m_pBkImg)
		{
			::DeleteObject(m_pBkImg->hBitmap);
			delete m_pBkImg;
			m_pBkImg = NULL;
		}
		if(m_pImg)
		{
			delete m_pImg;
			m_pImg = NULL;
		}
		m_pImg = pBitmap;
		HBITMAP hBitmap;
		m_pImg->GetHBITMAP(NULL,&hBitmap);
		m_pBkImg = new TImageInfo;
		m_pBkImg->hBitmap = hBitmap;
		m_pBkImg->nX = m_pImg->GetWidth();
		m_pBkImg->nY = m_pImg->GetHeight();
		Invalidate();
	}

	Bitmap *& CListContainerItemElementUI::GetItemImg()
	{
		return m_pImg;
	}

	void CListContainerItemElementUI::SetItemSize(SIZE szItem)
	{
		m_szItem = szItem;
	}

	void CListContainerItemElementUI::DrawItemBk(HDC hDC,const RECT &rcItem)
	{
		ASSERT(m_pOwner);
		if( m_pOwner == NULL ) return;
		TListInfoUI* pInfo = m_pOwner->GetListInfo();
		DWORD iBackColor = 0;
		if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) 
			iBackColor = pInfo->dwBkColor;
		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			iBackColor = pInfo->dwHotBkColor;
		}
		if( IsSelected() ) {
			iBackColor = pInfo->dwSelectedBkColor;
		}
		if( !IsEnabled() ) {
			iBackColor = pInfo->dwDisabledBkColor;
		}
		if ( iBackColor != 0 ) {
			m_pManager->GetRenderCore()->DrawColor(m_rcItem,GetAdjustColor(iBackColor));
		}
		if( !IsEnabled() ) {
			if( !pInfo->sDisabledImage.IsEmpty() ) {
				if( !DrawImage(pInfo->sDisabledImage) ) pInfo->sDisabledImage.Empty();
				else return;
			}
		}
		if( IsSelected() ) {
			if( !pInfo->sSelectedImage.IsEmpty() ) {
				if( !DrawImage(pInfo->sSelectedImage) ) pInfo->sSelectedImage.Empty();
				else return;
			}
		}
		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( !pInfo->sHotImage.IsEmpty() ) {
				if( !DrawImage(pInfo->sHotImage) ) pInfo->sHotImage.Empty();
				else return;
			}
		}
		if( !m_sBkImage.IsEmpty() ) {
			if( !pInfo->bAlternateBk || m_iIndex % 2 == 0 ) {
				if( !DrawImage(m_sBkImage) ) m_sBkImage.Empty();
			}
		}

		if( m_sBkImage.IsEmpty() )
		{
			if( !pInfo->sBkImage.IsEmpty() ) 
			{
				if( !DrawImage(pInfo->sBkImage) ) pInfo->sBkImage.Empty();
				else return;
			}
			else if(m_pBkImg)
			{
				RECT rc = {0,0,0,0};
				RECT rcBmpPart={0,0,m_pBkImg->nX,m_pBkImg->nY};
				RECT rcPaint = rcItem;
				rcPaint.left = (rcItem.left+rcItem.right-rcBmpPart.right)/2;
				rcPaint.right = rcPaint.left+rcBmpPart.right;
				rcPaint.top = (rcItem.top+rcItem.bottom-rcBmpPart.bottom)/2;
				rcPaint.bottom = rcPaint.top+rcBmpPart.bottom;
				m_pManager->GetRenderCore()->DrawImage(m_pBkImg,rcPaint,rcPaint,rcBmpPart,rc,true);
			}
		}

		if ( pInfo->dwLineColor != 0 ) {
			RECT rcLine = { m_rcItem.left, m_rcItem.bottom, m_rcItem.right, m_rcItem.bottom };
			m_pManager->GetRenderCore()->DrawLine(rcLine, 1, GetAdjustColor(pInfo->dwLineColor));
		}
	}
} // namespace UiLib
