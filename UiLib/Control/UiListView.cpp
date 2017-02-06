#include "stdAfx.h"
#include "UiListView.h"

namespace UiLib {

	CListViewUI::CListViewUI()
	{
		m_pCallback = NULL;
		m_bUnSelectAll = true;
		m_bSingleSel = false;
		m_bNeedTab = true;
		m_ListInfo.nColumns = 0;
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
		::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));
		::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));
		::ZeroMemory(&m_ListInfo.szCheckImg, sizeof(m_ListInfo.szCheckImg));
		::ZeroMemory(&m_ListInfo.szIconImg, sizeof(m_ListInfo.szIconImg));
	}
	CListViewUI::~CListViewUI()
	{

	}

	LPCTSTR CListViewUI::GetClass() const
	{
		return _T("ListViewUI");
	}

	UINT CListViewUI::GetControlFlags() const
	{
		return IsWantTab();
	}

	LPVOID CListViewUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("ListView")) == 0 ) 
			return static_cast<CListViewUI*>(this);
		if( _tcscmp(pstrName, _T("IList")) == 0 ) 
			return static_cast<IListUI*>(this);
		if( _tcscmp(pstrName, _T("IListOwner")) == 0 ) 
			return static_cast<IListOwnerUI*>(this);
		return CTileLayoutUI::GetInterface(pstrName);
	}

	TListInfoUI* CListViewUI::GetListInfo()
	{
		return &m_ListInfo;
	}

	IListCallbackUI* CListViewUI::GetTextCallback() const
	{
		return m_pCallback;
	}

	void CListViewUI::SetTextCallback(IListCallbackUI* pCallback)
	{
		m_pCallback = pCallback;
	}

	void CListViewUI::SetSingleSelect(bool bSingleSel)
	{
		try
		{
			m_bSingleSel = bSingleSel;
			UnSelectAllItems();
		}
		catch (...)
		{
			throw "CListViewUI::SetSingleSelect";
		}
	}

	bool CListViewUI::GetSingleSelect() const
	{
		try
		{
			return m_bSingleSel;
		}
		catch (...)
		{
			throw "CListViewUI::GetSingleSelect";
		}
	}

	void CListViewUI::SetItemFont(int index)
	{
		m_ListInfo.nFont = index;
		NeedUpdate();
	}

	void CListViewUI::SetItemTextStyle(UINT uStyle)
	{
		m_ListInfo.uTextStyle = uStyle;
		NeedUpdate();
	}

	void CListViewUI::SetItemTextPadding(RECT rc)
	{
		m_ListInfo.rcTextPadding = rc;
		NeedUpdate();
	}

	RECT CListViewUI::GetItemTextPadding() const
	{
		return m_ListInfo.rcTextPadding;
	}

	void CListViewUI::SetItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwTextColor = dwTextColor;
		Invalidate();
	}

	void CListViewUI::SetItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwBkColor = dwBkColor;
		Invalidate();
	}

	void CListViewUI::SetItemBkImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sBkImage = pStrImage;
		Invalidate();
	}

	void CListViewUI::SetAlternateBk(bool bAlternateBk)
	{
		m_ListInfo.bAlternateBk = bAlternateBk;
		Invalidate();
	}

	DWORD CListViewUI::GetItemTextColor() const
	{
		return m_ListInfo.dwTextColor;
	}

	DWORD CListViewUI::GetItemBkColor() const
	{
		return m_ListInfo.dwBkColor;
	}

	LPCTSTR CListViewUI::GetItemBkImage() const
	{
		return m_ListInfo.sBkImage;
	}

	bool CListViewUI::IsAlternateBk() const
	{
		return m_ListInfo.bAlternateBk;
	}

	void CListViewUI::SetSelectedItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwSelectedTextColor = dwTextColor;
		Invalidate();
	}

	void CListViewUI::SetSelectedItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwSelectedBkColor = dwBkColor;
		Invalidate();
	}

	void CListViewUI::SetSelectedItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sSelectedImage = pStrImage;
		Invalidate();
	}

	DWORD CListViewUI::GetSelectedItemTextColor() const
	{
		return m_ListInfo.dwSelectedTextColor;
	}

	DWORD CListViewUI::GetSelectedItemBkColor() const
	{
		return m_ListInfo.dwSelectedBkColor;
	}

	LPCTSTR CListViewUI::GetSelectedItemImage() const
	{
		return m_ListInfo.sSelectedImage;
	}

	void CListViewUI::SetHotItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwHotTextColor = dwTextColor;
		Invalidate();
	}

	void CListViewUI::SetHotItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwHotBkColor = dwBkColor;
		Invalidate();
	}

	void CListViewUI::SetHotItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sHotImage = pStrImage;
		Invalidate();
	}

	DWORD CListViewUI::GetHotItemTextColor() const
	{
		return m_ListInfo.dwHotTextColor;
	}
	DWORD CListViewUI::GetHotItemBkColor() const
	{
		return m_ListInfo.dwHotBkColor;
	}

	LPCTSTR CListViewUI::GetHotItemImage() const
	{
		return m_ListInfo.sHotImage;
	}

	void CListViewUI::SetDisabledItemTextColor(DWORD dwTextColor)
	{
		m_ListInfo.dwDisabledTextColor = dwTextColor;
		Invalidate();
	}

	void CListViewUI::SetDisabledItemBkColor(DWORD dwBkColor)
	{
		m_ListInfo.dwDisabledBkColor = dwBkColor;
		Invalidate();
	}

	void CListViewUI::SetDisabledItemImage(LPCTSTR pStrImage)
	{
		m_ListInfo.sDisabledImage = pStrImage;
		Invalidate();
	}

	DWORD CListViewUI::GetDisabledItemTextColor() const
	{
		return m_ListInfo.dwDisabledTextColor;
	}

	DWORD CListViewUI::GetDisabledItemBkColor() const
	{
		return m_ListInfo.dwDisabledBkColor;
	}

	LPCTSTR CListViewUI::GetDisabledItemImage() const
	{
		return m_ListInfo.sDisabledImage;
	}

	DWORD CListViewUI::GetItemLineColor() const
	{
		return m_ListInfo.dwLineColor;
	}

	void CListViewUI::SetItemLineColor(DWORD dwLineColor)
	{
		m_ListInfo.dwLineColor = dwLineColor;
		Invalidate();
	}

	bool CListViewUI::IsItemShowHtml()
	{
		return m_ListInfo.bShowHtml;
	}

	void CListViewUI::SetItemShowHtml(bool bShowHtml)
	{
		if( m_ListInfo.bShowHtml == bShowHtml ) return;

		m_ListInfo.bShowHtml = bShowHtml;
		NeedUpdate();
	}

	SIZE CListViewUI::GetCheckImgSize() const
	{
		try
		{
			return m_ListInfo.szCheckImg;
		}
		catch (...)
		{
			throw "CListViewUI::GetCheckImgSize";
		}
	}

	void CListViewUI::SetIconImgSize( SIZE szIconImg )
	{
		try
		{
			m_ListInfo.szIconImg = szIconImg;
		}
		catch (...)
		{
			throw "CListViewUI::SetIconImgSize";
		}
	}

	void CListViewUI::SetMultipleItem(bool bMultipleable)
	{
		m_bSingleSel =!bMultipleable;
	}

	bool CListViewUI::GetMultipleItem() const
	{
		try
		{
			return !m_bSingleSel;
		}
		catch (...)
		{
			throw "CListViewUI::GetMultipleItem";
		}
	}

	int CListViewUI::GetCurSel() const
	{
		if (m_aSelItems.GetSize() <= 0)
		{
			return -1;
		}
		else
		{
			return (int)m_aSelItems.GetAt(m_aSelItems.GetSize()-1);
		}
		return -1;
	}

	CStdPtrArray CListViewUI::GetSelectItems()
	{
		return m_aSelItems;
	}

	CControlUI* CListViewUI::GetItemAt(int iIndex) const
	{
		return CTileLayoutUI::GetItemAt(iIndex);
	}

	int CListViewUI::GetItemIndex(CControlUI* pControl) const
	{
		return CTileLayoutUI::GetItemIndex(pControl);
	}

	bool CListViewUI::SetItemIndex(CControlUI* pControl, int iIndex)
	{
		int iOrginIndex = CTileLayoutUI::GetItemIndex(pControl);
		if( iOrginIndex == -1 ) return false;
		if( iOrginIndex == iIndex ) return true;

		IListItemUI* pSelectedListItem = NULL;
		if( !CTileLayoutUI::SetItemIndex(pControl, iIndex) ) return false;
		int iMinIndex = min(iOrginIndex, iIndex);
		int iMaxIndex = max(iOrginIndex, iIndex);
		for(int i = iMinIndex; i < iMaxIndex + 1; ++i) {
			CControlUI* p = CTileLayoutUI::GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}
		UnSelectAllItems();
		return true;
	}

	bool CListViewUI::Add(CControlUI* pControl)
	{
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(GetCount());
		}
		return CTileLayoutUI::Add(pControl);
	}

	bool CListViewUI::AddAt(CControlUI* pControl, int iIndex)
	{
		if (!CTileLayoutUI::AddAt(pControl, iIndex)) return false;

		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) {
			pListItem->SetOwner(this);
			pListItem->SetIndex(iIndex);
		}

		for(int i = iIndex + 1; i <CTileLayoutUI::GetCount(); ++i) 
		{
			CControlUI* p = GetItemAt(i);
			pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) 
				pListItem->SetIndex(i);
		}
		UnSelectAllItems();
		return true;
	}

	bool CListViewUI::Remove(CControlUI* pControl)
	{
		int iIndex = GetItemIndex(pControl);
		if (iIndex == -1) return false;

		if (!RemoveAt(iIndex)) return false;
		for(int i = iIndex; i < CTileLayoutUI::GetCount(); ++i) {
			CControlUI* p = GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) {
				pListItem->SetIndex(i);
			}
		}
		return true;
	}

	bool CListViewUI::RemoveAt(int iIndex)
	{
		if (!CTileLayoutUI::RemoveAt(iIndex)) return false;
		m_aSelItems.Empty();
		for(int i = iIndex; i < CTileLayoutUI::GetCount(); ++i)
		{
			CControlUI* p = GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) pListItem->SetIndex(i);
		}

		//删除前面的,选中项要相应的往前移动;
		for(int i = 0 ; i < GetCount() ; i++)
		{
			CControlUI* p = GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if(pListItem->IsSelected())
				m_aSelItems.Add((LPVOID)i);
		}
		return true;
	}

	bool CListViewUI::RemoveItems(int iIndex,int iSel)
	{
		if (!CTileLayoutUI::RemoveAt(iIndex)) return false;

		for(int i = iIndex; i < CTileLayoutUI::GetCount(); ++i)
		{
			CControlUI* p = GetItemAt(i);
			IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
			if( pListItem != NULL ) pListItem->SetIndex(i);
		}
	
		m_aSelItems.Remove(m_aSelItems.Find((LPVOID)iSel));
		return true;
	}

	void CListViewUI::RemoveAll(bool bUpdate)
	{
		m_aSelItems.Empty();
		CTileLayoutUI::RemoveAll(bUpdate);
	}

	bool CListViewUI::RemoveSelectItems()
	{
		int itemIndex;
		for (int i = 0; i < m_aSelItems.GetSize(); ++i)
		{
			itemIndex = (int)m_aSelItems.GetAt(i);
			if(!RemoveAt(itemIndex))
				return false;
		}
		return true;
	}

	bool CListViewUI::SelectItem(int iIndex, bool bTakeFocus)
	{
		if( iIndex < 0 ) return false;
		CControlUI* pControl = GetItemAt(iIndex);
		if( pControl == NULL ) return false;
		if( !pControl->IsVisible() ) return false;
		if( !pControl->IsEnabled() ) return false;
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
		if( pListItem == NULL ) return false;
		

		if (!(GetKeyState(VK_CONTROL) & 0x8000) || m_bSingleSel)
		{
			if(m_bUnSelectAll)
			UnSelectAllItems();
		}
		
		if(m_bSingleSel && m_aSelItems.GetSize() > 0) {
			CControlUI* pControl = GetItemAt((int)m_aSelItems.GetAt(0));
			if( pControl != NULL) {
				IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
				if( pListItem != NULL ) pListItem->Select(false);
			}
		}	

		if( !pListItem->IsSelected() && !pListItem->Select(true) ) {
			return false;
		}
	
		if(m_aSelItems.Find((LPVOID)iIndex) >= 0) 
			return false;
		m_aSelItems.Add((LPVOID)iIndex);

		if( bTakeFocus ) pControl->SetFocus();
		if( m_pManager != NULL ) {
			m_pManager->SendNotify(this, _T("itemselect"), iIndex);
		}
		Invalidate();
		return true;
	}

	bool CListViewUI::UnSelectItem(int iIndex)
	{
		try
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
			if( m_pManager != NULL ) {
				m_pManager->SendNotify(this, _T("itemunselect"), iIndex);
			}

			return true;
		}
		catch (...)
		{
			throw "CListViewUI::UnSelectItem";
		}
	}

	int CListViewUI::GetNextSelItem( int nItem ) const
	{
		try
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
		catch (...)
		{
			throw "CListViewUI::GetNextSelItem";
		}
	}

	bool CListViewUI::SelectMultiItem(int iIndex, bool bTakeFocus)
	{
		try
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

			if( !pListItem->Select(true) )
				return false;


			if( bTakeFocus ) pControl->SetFocus();
			if( m_pManager != NULL ) {
				m_pManager->SendNotify(this, _T("itemselect"), iIndex);
			}
			return true;
		}
		catch (...)
		{
			throw "CListViewUI::SelectMultiItem";
		}
	}

	void CListViewUI::SelectAllItems()
	{
		for (int i = 0; i < GetCount(); ++i)
			SelectMultiItem(i);
	}

	void CListViewUI::UnSelectAllItems()
	{
		try
		{
			CControlUI* pControl;
			int itemIndex;
			for (int i = 0; i < m_aSelItems.GetSize(); i)
			{
				itemIndex = (int)m_aSelItems.GetAt(i);
				pControl = GetItemAt(itemIndex);
				if(pControl == NULL)
					continue;
				IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));
				if( pListItem == NULL )
					continue;
				if( !pListItem->Select(false) )
				{
					continue;		
				}
			}
			m_aSelItems.Empty();
		}
		catch (...)
		{
			throw "CListViewUI::UnSelectAllItems";
		}
	}

	int CListViewUI::GetSelectItemCount() const
	{
		try
		{
			return m_aSelItems.GetSize();
		}
		catch (...)
		{
			throw "CListViewUI::GetSelectItemCount";
		}
	}

	int CListViewUI::GetMinSelItemIndex()
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

	int CListViewUI::GetMaxSelItemIndex()
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

	void CListViewUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CTileLayoutUI::DoEvent(event);
			return;
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

		switch( event.Type ) {
		case UIEVENT_KEYDOWN:
			switch( event.chKey ) {
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
					index + 1 > GetCount() ? SelectItem(GetCount() - 1, true) : SelectItem(index, true);					
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
				if (GetCount() > 0)
					SelectItem(0, true);
			}
			return;
		case VK_END:
			{
				if (GetCount() > 0)
					SelectItem(GetCount() - 1, true);
			}
			return;        
		case 0x41:
			{
				if (!m_bSingleSel && (GetKeyState(VK_CONTROL) & 0x8000))
					SelectAllItems();
			}
			return;
			break;
		case UIEVENT_SCROLLWHEEL:
			{
				switch( LOWORD(event.wParam) )
				{
				case SB_LINEUP:
					 LineUp();
					return;
				case SB_LINEDOWN:
					 LineDown();
					return;
				}
			}
			break;
			}
		}
		CTileLayoutUI::DoEvent(event);
	}
}