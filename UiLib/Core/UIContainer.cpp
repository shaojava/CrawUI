#include "StdAfx.h"

namespace UiLib
{

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	REGIST_DUICLASS(CContainerUI);

	CContainerUI::CContainerUI()
		: m_iChildPadding(0),
		delay_deltaY_(0),
		delay_deltaX_(0),
		delay_number_(0),
		delay_left_(0),
		m_bDamp(true),
		m_bAutoDestroy(true),
		m_bDelayedDestroy(true),
		m_bMouseChildEnabled(true),
		m_pVerticalScrollBar(NULL),
		m_pHorizontalScrollBar(NULL),
		m_bFlowContainer(false),
		m_pCompFunc(NULL)
	{
		m_bSizeGrow = false;
		m_rcInset.left = m_rcInset.right = m_rcInset.top = m_rcInset.bottom = 0;
		m_pScrollFlex = NULL;
		m_fFlexed = 0;
	}

	CContainerUI::~CContainerUI()
	{
		m_bDelayedDestroy = false;
		RemoveAll();
		if(m_pStoryBoard)
		{
			m_pStoryBoard->Stop(m_pScrollFlex);
			SAFE_DELETE(m_pScrollFlex);
		}
		if( m_pVerticalScrollBar ) delete m_pVerticalScrollBar;
		if( m_pHorizontalScrollBar ) delete m_pHorizontalScrollBar;
	}

	LPCTSTR CContainerUI::GetClass() const
	{
		return _T("ContainerUI");
	}

	LPVOID CContainerUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("IContainer")) == 0 ) return static_cast<IContainerUI*>(this);
		else if( _tcscmp(pstrName, _T("Container")) == 0 ) return static_cast<CContainerUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	CControlUI* CContainerUI::GetItemAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_items.GetSize() ) return NULL;
		return static_cast<CControlUI*>(m_items[iIndex]);
	}

	int CContainerUI::GetItemIndex(CControlUI* pControl) const
	{
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			if( static_cast<CControlUI*>(m_items[it]) == pControl ) {
				return it;
			}
		}

		return -1;
	}

	bool CContainerUI::SetItemIndex(CControlUI* pControl, int iIndex)
	{
		for( int it = 0; it < m_items.GetSize(); it++ ) 
		{
			if( static_cast<CControlUI*>(m_items[it]) == pControl ) {
				NeedUpdate();
				m_items.Remove(it);
				return m_items.InsertAt(iIndex, pControl);
			}
		}
		return false;
	}

	int CContainerUI::GetCount() const
	{
		return m_items.GetSize();
	}

	bool CContainerUI::Add(CControlUI* pControl)
	{
		if( pControl == NULL) return false;
		bool bRes = false;
		if( m_pManager != NULL ) m_pManager->InitControls(pControl, this);
		if( IsVisible() ) NeedUpdate();
		else pControl->SetInternVisible(false);
		if (m_pCompFunc == NULL)
		{
			bRes = m_items.Add(pControl);
		}
		else
		{
			bRes =m_items.Insert(pControl,m_pCompFunc);
		}
		return  bRes;
	}

	bool CContainerUI::AddAt(CControlUI* pControl, int iIndex)
	{
		if( pControl == NULL) return false;

		if( m_pManager != NULL ) m_pManager->InitControls(pControl, this);
		if( IsVisible() ) NeedUpdate();
		else pControl->SetInternVisible(false);
		return m_items.InsertAt(iIndex, pControl);
	}

	bool CContainerUI::Remove(CControlUI* pControl)
	{
		if( pControl == NULL) return false;

		for( int it = 0; it < m_items.GetSize(); it++ ) {
			if( static_cast<CControlUI*>(m_items[it]) == pControl ) 
			{
				NeedUpdate();
				if( m_bAutoDestroy ) 
				{
					if( m_bDelayedDestroy && m_pManager ) m_pManager->AddDelayedCleanup(pControl);             
					else delete pControl;
				}
				return m_items.Remove(it);
			}
		}
		return false;
	}

	bool CContainerUI::RemoveAt(int iIndex)
	{
		CControlUI* pControl = GetItemAt(iIndex);
		if (pControl != NULL) {
			return CContainerUI::Remove(pControl);
		}
		return false;
	}

	void CContainerUI::Sink(CControlUI *pContorl)
	{
		if(!pContorl) return;
		int nIndex = m_items.Find((void *)pContorl);
		if(nIndex != -1)
			m_items.Sink(nIndex);
	}

	void CContainerUI::RemoveAll(bool bUpdate)
	{
		for( int it = 0; m_bAutoDestroy && it < m_items.GetSize(); it++ ) {
			if( m_bDelayedDestroy && m_pManager ) m_pManager->AddDelayedCleanup(static_cast<CControlUI*>(m_items[it]));             
			else delete static_cast<CControlUI*>(m_items[it]);
		}
		m_items.Empty();
		if(bUpdate)
			NeedUpdate();
	}

	bool CContainerUI::IsAutoDestroy() const
	{
		return m_bAutoDestroy;
	}

	void CContainerUI::SetAutoDestroy(bool bAuto)
	{
		m_bAutoDestroy = bAuto;
	}

	bool CContainerUI::IsDelayedDestroy() const
	{
		return m_bDelayedDestroy;
	}

	void CContainerUI::SetDelayedDestroy(bool bDelayed)
	{
		m_bDelayedDestroy = bDelayed;
	}

	RECT CContainerUI::GetInset() const
	{
		return m_rcInset;
	}

	void CContainerUI::SetInset(RECT rcInset)
	{
		m_rcInset = rcInset;
		NeedUpdate();
	}

	int CContainerUI::GetChildPadding() const
	{
		return m_iChildPadding;
	}

	void CContainerUI::SetChildPadding(int iPadding)
	{
		m_iChildPadding = iPadding;
		NeedUpdate();
	}

	bool CContainerUI::IsMouseChildEnabled() const
	{
		return m_bMouseChildEnabled;
	}

	void CContainerUI::SetMouseChildEnabled(bool bEnable)
	{
		m_bMouseChildEnabled = bEnable;
	}

	void CContainerUI::SetVisible(bool bVisible)
	{
		if( m_bVisible == bVisible ) return;
		CControlUI::SetVisible(bVisible);
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			static_cast<CControlUI*>(m_items[it])->SetInternVisible(IsVisible());
		}
	}

	// 逻辑上，对于Container控件不公开此方法
	// 调用此方法的结果是，内部子控件隐藏，控件本身依然显示，背景等效果存在
	void CContainerUI::SetInternVisible(bool bVisible)
	{
		CControlUI::SetInternVisible(bVisible);
		if( m_items.IsEmpty() ) return;
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			// 控制子控件显示状态
			// InternVisible状态应由子控件自己控制
			static_cast<CControlUI*>(m_items[it])->SetInternVisible(IsVisible());
		}
	}

	void CContainerUI::SetContainerFlowable(bool bFlowable)
	{
		m_bFlowContainer = bFlowable;
	}

	void CContainerUI::SetMouseEnabled(bool bEnabled)
	{
		if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->SetMouseEnabled(bEnabled);
		if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->SetMouseEnabled(bEnabled);
		CControlUI::SetMouseEnabled(bEnabled);
	}

	void CContainerUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CControlUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETFOCUS ) 
		{
			m_bFocused = true;
		}
		if( event.Type == UIEVENT_KILLFOCUS ) 
		{
			m_bFocused = false;
		}
		if (event.Type == UIEVENT_TIMER)
		{
			SIZE sz = GetScrollPos();
			int nFlexRange = GetFlexRange();
			if(event.wParam == IDT_SCROLL_V)
			{
				if (delay_left_ > 0)
				{
					--delay_left_;
					LONG lDeltaY =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaY_);
					sz.cy -= lDeltaY;
					SetScrollPosFlex(sz);
					if(sz.cy < -nFlexRange || sz.cy > nFlexRange+GetScrollRange().cy)
						delay_left_ = 0;
					return;
				}
				delay_deltaY_ = 0;
				delay_number_ = 0;
				delay_left_ = 0;
				m_pManager->KillTimer(this, IDT_SCROLL_V);
				if(sz.cy < 0 || sz.cy > GetScrollRange().cy)
				{
					SAFE_DELETE(m_pScrollFlex);
					m_pScrollFlex = new CTimeLineTween();
					m_pScrollFlex->SetTweenInfo(1000);
					m_fFlexed = sz.cy;
					if(m_fFlexed < 0)
						m_pScrollFlex->AddProperty(_T("flex"),m_fFlexed,0);
					else
						m_pScrollFlex->AddProperty(_T("flex"),m_fFlexed,GetScrollRange().cy);
					m_pStoryBoard->AddTimeLine(m_pScrollFlex);
				}
				return;
			}
			else if(event.wParam == IDT_SCROLL_H)
			{
				if (delay_left_ > 0)
				{
					--delay_left_;
					LONG lDeltaX =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaX_);
					sz.cx -= lDeltaX;
					SetScrollPosFlex(sz);
					if(sz.cx < -nFlexRange || sz.cx > nFlexRange+GetScrollRange().cx)
						delay_left_ = 0;
					return;
				}
				delay_deltaX_ = 0;
				delay_number_ = 0;
				delay_left_ = 0;
				m_pManager->KillTimer(this, IDT_SCROLL_H);
				if(sz.cx < 0 || sz.cx > GetScrollRange().cx)
				{
					SAFE_DELETE(m_pScrollFlex);
					m_pScrollFlex = new CTimeLineTween();
					m_pScrollFlex->SetTweenInfo(1000);
					m_fFlexed = sz.cx;
					if(m_fFlexed < 0)
						m_pScrollFlex->AddProperty(_T("flex"),m_fFlexed,0);
					else
						m_pScrollFlex->AddProperty(_T("flex"),m_fFlexed,GetScrollRange().cx);
					m_pStoryBoard->AddTimeLine(m_pScrollFlex);
				}
				return;
			}
		}
		if(m_pVerticalScrollBar&&m_pVerticalScrollBar->IsEnabled() && (m_pVerticalScrollBar->GetScrollRange() != 0 || m_bFlowContainer))
		{
			if( event.Type == UIEVENT_KEYDOWN ) 
			{
				switch( event.chKey ) 
				{
				case VK_DOWN:
					LineDown();
					return;
				case VK_UP:
					LineUp();
					return;
				case VK_NEXT:
					PageDown();
					return;
				case VK_PRIOR:
					PageUp();
					return;
				case VK_HOME:
					HomeUp();
					return;
				case VK_END:
					EndDown();
					return;
				}
			}
			else if( event.Type == UIEVENT_SCROLLWHEEL )
			{
				if(m_bDamp)
				{
					LONG lDeltaY = 0;
					if (delay_number_ > 0)
						lDeltaY =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaY_);
					switch (LOWORD(event.wParam))
					{
					case SB_LINEUP:
						if (delay_deltaY_ >= 0)
							delay_deltaY_ = lDeltaY + 8;
						else
							delay_deltaY_ = lDeltaY + 12;
						break;
					case SB_LINEDOWN:
						if (delay_deltaY_ <= 0)
							delay_deltaY_ = lDeltaY - 8;
						else
							delay_deltaY_ = lDeltaY - 12;
						break;
					}
					if
						(delay_deltaY_ > 100) delay_deltaY_ = 100;
					else if
						(delay_deltaY_ < -100) delay_deltaY_ = -100;
					delay_number_ = (DWORD)sqrt((double)abs(delay_deltaY_)) * 5;
					delay_left_ = delay_number_;
					if(m_pScrollFlex)
					{
						m_pStoryBoard->Stop(m_pScrollFlex);
						SAFE_DELETE(m_pScrollFlex);
					}
					
					m_pManager->SetTimer(this, IDT_SCROLL_V, 16U);
					return;
				}
				else
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
			}
		}
		else if(m_pHorizontalScrollBar&&!m_pVerticalScrollBar&&m_pHorizontalScrollBar->IsEnabled() && (m_pHorizontalScrollBar->GetScrollRange() != 0||m_bFlowContainer) ) 
		{
			if( event.Type == UIEVENT_KEYDOWN ) 
			{
				switch( event.chKey ) {
				case VK_DOWN:
					LineRight();
					return;
				case VK_UP:
					LineLeft();
					return;
				case VK_NEXT:
					PageRight();
					return;
				case VK_PRIOR:
					PageLeft();
					return;
				case VK_HOME:
					HomeLeft();
					return;
				case VK_END:
					EndRight();
					return;
				}
			}
			else if( event.Type == UIEVENT_SCROLLWHEEL )
			{
				if(m_bDamp)
				{
					LONG lDeltaX = 0;
					if (delay_number_ > 0)
						lDeltaX =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaX_);
					switch (LOWORD(event.wParam))
					{
					case SB_LINEUP:
						if (delay_deltaX_ >= 0)
							delay_deltaX_ = lDeltaX + 8;
						else
							delay_deltaX_ = lDeltaX + 12;
						break;
					case SB_LINEDOWN:
						if (delay_deltaX_ <= 0)
							delay_deltaX_ = lDeltaX - 8;
						else
							delay_deltaX_ = lDeltaX - 12;
						break;
					}
					if
						(delay_deltaX_ > 60) delay_deltaX_ = 100;
					else if
						(delay_deltaX_ < -100) delay_deltaX_ = -100;
					delay_number_ = (DWORD)sqrt((double)abs(delay_deltaX_)) * 5;
					delay_left_ = delay_number_;
					if(m_pScrollFlex)
					{
						m_pStoryBoard->Stop(m_pScrollFlex);
						SAFE_DELETE(m_pScrollFlex);
					}
					m_pManager->SetTimer(this, IDT_SCROLL_H, 16U);
					return;
				}
				else
				{
					switch( LOWORD(event.wParam) ) 
					{
					case SB_LINEUP:
						LineLeft();
						return;
					case SB_LINEDOWN:
						LineRight();
						return;
					}
				}
			}
		}
		CControlUI::DoEvent(event);
	}

	int CContainerUI::GetFlexRange()
	{
		int nFlexRange = 0;
		if( m_pVerticalScrollBar ) nFlexRange = m_pVerticalScrollBar->GetFlexRange();
		if( m_pHorizontalScrollBar) nFlexRange = m_pHorizontalScrollBar->GetFlexRange();
		return nFlexRange;
	}

	SIZE CContainerUI::GetScrollPos() const
	{
		SIZE sz = {0, 0};
		if( m_pVerticalScrollBar ) sz.cy = m_pVerticalScrollBar->GetScrollPos();
		if( m_pHorizontalScrollBar) sz.cx = m_pHorizontalScrollBar->GetScrollPos();
		return sz;
	}

	SIZE CContainerUI::GetScrollRange() const
	{
		SIZE sz = {0, 0};
		if( m_pVerticalScrollBar ) sz.cy = m_pVerticalScrollBar->GetScrollRange();
		if( m_pHorizontalScrollBar ) sz.cx = m_pHorizontalScrollBar->GetScrollRange();
		return sz;
	}
	void CContainerUI::SetScrollRange(SIZE sz) const
	{
		if( m_pVerticalScrollBar ) m_pVerticalScrollBar->SetScrollRange(sz.cy);
		if( m_pHorizontalScrollBar ) m_pHorizontalScrollBar->SetScrollRange(sz.cx);
	}

	bool CContainerUI::OnAnimateStep(TimeEvent *pEvent)
	{
		if(pEvent->pTimeLine == m_pScrollFlex)
		{
			m_pScrollFlex->GetPropertyCurrentValue(_T("flex"),m_fFlexed);
			SIZE szPos = {m_fFlexed,m_fFlexed};
			SetScrollPosFlex(szPos);
		}
		return true;
	}

	void CContainerUI::SetScrollPosFlex(SIZE szPos)
	{
		int cx = 0;
		int cy = 0;
		if( m_pVerticalScrollBar ) 
		{
			int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
			m_pVerticalScrollBar->SetScrollPos(szPos.cy,true);
			cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if( m_pHorizontalScrollBar ) 
		{
			int iLastScrollPos = m_pHorizontalScrollBar->GetScrollPos();
			m_pHorizontalScrollBar->SetScrollPos(szPos.cx,true);
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
	}

	void CContainerUI::ScrollBarTakePlace(bool bTake)
	{
		if(m_pVerticalScrollBar)
			m_pVerticalScrollBar->ShowTakePlace(bTake);
		if(m_pHorizontalScrollBar)
			m_pHorizontalScrollBar->ShowTakePlace(bTake);
	}

	void CContainerUI::ShowScrollBar(bool bShow)
	{
		if(m_pVerticalScrollBar)
			m_pVerticalScrollBar->SetVisible(bShow);
		if(m_pHorizontalScrollBar)
			m_pHorizontalScrollBar->SetVisible(bShow);
	}

	void CContainerUI::SetScrollPos(SIZE szPos)
	{
		int cx = 0;
		int cy = 0;
		if( m_pVerticalScrollBar) 
		{
			int iLastScrollPos = m_pVerticalScrollBar->GetScrollPos();
			m_pVerticalScrollBar->SetScrollPos(szPos.cy);
			cy = m_pVerticalScrollBar->GetScrollPos() - iLastScrollPos;
		}

		if( m_pHorizontalScrollBar) 
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
	}

	void CContainerUI::LineUp()
	{
		int cyLine = 8;
		if( m_pManager ) cyLine = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;

		SIZE sz = GetScrollPos();
		sz.cy -= cyLine;
		SetScrollPos(sz);
	}

	void CContainerUI::LineDown()
	{
		int cyLine = 8;
		if( m_pManager ) cyLine = m_pManager->GetDefaultFontInfo()->tm.tmHeight + 8;

		SIZE sz = GetScrollPos();
		sz.cy += cyLine;
		SetScrollPos(sz);
	}

	void CContainerUI::PageUp()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
		if( m_pHorizontalScrollBar) iOffset -= m_pHorizontalScrollBar->GetFixedHeight();
		sz.cy -= iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::PageDown()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.bottom - m_rcItem.top - m_rcInset.top - m_rcInset.bottom;
		if( m_pHorizontalScrollBar ) iOffset -= m_pHorizontalScrollBar->GetFixedHeight();
		sz.cy += iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::HomeUp()
	{
		SIZE sz = GetScrollPos();
		sz.cy = 0;
		SetScrollPos(sz);
	}

	void CContainerUI::EndDown()
	{
		SIZE sz = GetScrollPos();
		sz.cy = GetScrollRange().cy;
		SetScrollPos(sz);
	}

	void CContainerUI::LineLeft()
	{
		SIZE sz = GetScrollPos();
		sz.cx -= 8;
		SetScrollPos(sz);
	}

	void CContainerUI::LineRight()
	{
		SIZE sz = GetScrollPos();
		sz.cx += 8;
		SetScrollPos(sz);
	}

	void CContainerUI::PageLeft()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
		if( m_pVerticalScrollBar) iOffset -= m_pVerticalScrollBar->GetFixedWidth();
		sz.cx -= iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::PageRight()
	{
		SIZE sz = GetScrollPos();
		int iOffset = m_rcItem.right - m_rcItem.left - m_rcInset.left - m_rcInset.right;
		if( m_pVerticalScrollBar) iOffset -= m_pVerticalScrollBar->GetFixedWidth();
		sz.cx += iOffset;
		SetScrollPos(sz);
	}

	void CContainerUI::HomeLeft()
	{
		SIZE sz = GetScrollPos();
		sz.cx = 0;
		SetScrollPos(sz);
	}

	void CContainerUI::EndRight()
	{
		SIZE sz = GetScrollPos();
		sz.cx = GetScrollRange().cx;
		SetScrollPos(sz);
	}

	void CContainerUI::EnableScrollBar(bool bEnableVertical, bool bEnableHorizontal)
	{
		if( bEnableVertical && !m_pVerticalScrollBar ) {
			m_pVerticalScrollBar = new CScrollBarUI;
			m_pVerticalScrollBar->SetOwner(this);
			m_pVerticalScrollBar->SetManager(m_pManager, NULL, false);
			if ( m_pManager ) {
				LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("VScrollBar"));
				if( pDefaultAttributes ) {
					m_pVerticalScrollBar->ApplyAttributeList(pDefaultAttributes);
				}
			}
		}
		else if( !bEnableVertical && m_pVerticalScrollBar ) {
			delete m_pVerticalScrollBar;
			m_pVerticalScrollBar = NULL;
		}

		if( bEnableHorizontal && !m_pHorizontalScrollBar ) {
			m_pHorizontalScrollBar = new CScrollBarUI;
			m_pHorizontalScrollBar->SetHorizontal(true);
			m_pHorizontalScrollBar->SetOwner(this);
			m_pHorizontalScrollBar->SetManager(m_pManager, NULL, false);
			if ( m_pManager ) {
				LPCTSTR pDefaultAttributes = m_pManager->GetDefaultAttributeList(_T("HScrollBar"));
				if( pDefaultAttributes ) {
					m_pHorizontalScrollBar->ApplyAttributeList(pDefaultAttributes);
				}
			}
		}
		else if( !bEnableHorizontal && m_pHorizontalScrollBar ) {
			delete m_pHorizontalScrollBar;
			m_pHorizontalScrollBar = NULL;
		}

		NeedUpdate();
	}

	CScrollBarUI* CContainerUI::GetVerticalScrollBar() const
	{
		return m_pVerticalScrollBar;
	}

	CScrollBarUI* CContainerUI::GetHorizontalScrollBar() const
	{
		return m_pHorizontalScrollBar;
	}

	int CContainerUI::FindSelectable(int iIndex, bool bForward /*= true*/) const
	{
		// NOTE: This is actually a helper-function for the list/combo/ect controls
		//       that allow them to find the next enabled/available selectable item
		if( GetCount() == 0 ) return -1;
		iIndex = CLAMP(iIndex, 0, GetCount() - 1);
		if( bForward ) {
			for( int i = iIndex; i < GetCount(); i++ ) {
				if( GetItemAt(i)->GetInterface(_T("ListItem")) != NULL 
					&& GetItemAt(i)->IsVisible()
					&& GetItemAt(i)->IsEnabled() ) return i;
			}
			return -1;
		}
		else {
			for( int i = iIndex; i >= 0; --i ) {
				if( GetItemAt(i)->GetInterface(_T("ListItem")) != NULL 
					&& GetItemAt(i)->IsVisible()
					&& GetItemAt(i)->IsEnabled() ) return i;
			}
			return FindSelectable(0, true);
		}
	}

	void CContainerUI::NeedUpdate()
	{
		//如果尺寸可以拓展;
		if(m_bSizeGrow)
		{
			//先计算下应该需要的最小尺寸;
			SIZE szCur = EstimateNeedSize();
			int nFixedHeight = GetFixedHeight();
			int nFixedWidth = GetFixedWidth();
			//如果比自己的尺寸大;
			if(szCur.cy > nFixedHeight)
			{
				//当自己不是自适应的情况时;
				if(nFixedHeight > 0)//自己应该设置一个拓展大小;
					SetGrownHeight(szCur.cy-nFixedHeight);
				else
				{
					//当自己是自适应的时候,如果父布局给我的自适应总大小还没需要的最小尺寸大;
					//说明即便其他兄弟控件为0,也不够我自己用的,所有要使用最小尺寸;
					if(szCur.cy > m_rcItem.bottom-m_rcItem.top)
						SetGrownHeight(szCur.cy);
					else//如果父亲给我的总大小比我需要的最小尺寸大,我就不需要拓展,因为我可以自适应;
						SetGrownHeight(0);
				}
			}
			else//如果还没自己尺寸大,我也不需要拓展,不管我是否需要自适应;
				SetGrownHeight(0);

			if(szCur.cx > nFixedWidth)
			{
				if(nFixedWidth > 0)
					SetGrownWidth(szCur.cx-nFixedWidth);
				else
				{
					if(szCur.cx > m_rcItem.right-m_rcItem.left)
						SetGrownWidth(szCur.cx);
					else
						SetGrownWidth(0);
				}
			}
			else
				SetGrownWidth(0);

			if( !IsVisible() ) return;
			m_bUpdateNeeded = true;
			if( m_pManager != NULL ) m_pManager->NeedUpdate();
			Invalidate();
		}
		else
			CControlUI::NeedUpdate();
	}

	SIZE CContainerUI::EstimateNeedSize()
	{
		//计算这个容器所需要的最小尺寸;
		//如果没有子控件则直接是自己的EstimateSize;
		if(m_items.GetSize() == 0)
			return EstimateSize(m_cxyFixed);

		RECT rc = m_rcItem;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		SIZE szAvailable = {-1,-1};
		int cxFixed = 0;
		int cyFixed = 0;
		int nEstimateNum = 0;
		for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) 
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
			if(!pControl->IsVisible()) continue;
			if(pControl->IsFloat()) continue;
			//估算每个控件的大小,所需要的大小是拓展大小与实际大小的和;
			SIZE sz = pControl->EstimateSize(m_cxyFixed);
			//如果返回-1,不添加到Fixed大小里面去,有尺寸才加;
			if(sz.cx > 0)
			{
				if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
				if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
				cxFixed += sz.cx;
			}
			if(sz.cy > 0)
			{
				if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
				if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();
				cyFixed += sz.cy;
			}
			cxFixed += pControl->GetPadding().left + pControl->GetPadding().right;
			cyFixed += pControl->GetPadding().top + pControl->GetPadding().bottom;
			nEstimateNum++;
		}
		cxFixed += (nEstimateNum - 1) * m_iChildPadding;
		cyFixed += (nEstimateNum - 1) * m_iChildPadding;
		//最后算出有指定大小或者拓展大小的所有控件的总尺寸;
		if(cxFixed > 0)
		{
			szAvailable.cx = cxFixed;
			//如果这个尺寸比父布局自身大小还小,则采用父控件大小;
			if(cxFixed < m_cxyFixed.cx)
				szAvailable.cx = m_cxyFixed.cx;
		}//如果控件不需要指定或者拓展,则直接使用父控件大小;
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

	void CContainerUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		if( m_items.IsEmpty() ) return;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		for( int it = 0; it < m_items.GetSize(); it++ )
		{
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
			if( !pControl->IsVisible() ) 
				continue;
			if( pControl->IsFloat() )
			{
				SetFloatPos(it);
			}
			else 
			{
				RECT rcPadding = pControl->GetPadding();
				rc.left += rcPadding.left;
				rc.right -= rcPadding.right;
				rc.top += rcPadding.top;
				rc.bottom -= rcPadding.bottom;
				pControl->SetPos(rc);
			}
		}
	}

	void CContainerUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("inset")) == 0 ) {
			RECT rcInset = { 0 };
			LPTSTR pstr = NULL;
			rcInset.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			rcInset.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
			rcInset.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
			rcInset.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
			SetInset(rcInset);
		}
		else if( _tcscmp(pstrName, _T("mousechild")) == 0 ) SetMouseChildEnabled(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("vscrollbar")) == 0 ) EnableScrollBar(_tcscmp(pstrValue, _T("true")) == 0, GetHorizontalScrollBar() != NULL);
		else if( _tcscmp(pstrName, _T("vscrollbarstyle")) == 0 ) 
		{
			EnableScrollBar(true, GetHorizontalScrollBar() != NULL);
			if( GetVerticalScrollBar() ) GetVerticalScrollBar()->ApplyAttributeList(pstrValue);
		}
		else if( _tcscmp(pstrName, _T("hscrollbar")) == 0 ) EnableScrollBar(GetVerticalScrollBar() != NULL, _tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("hscrollbarstyle")) == 0 ) 
		{
			EnableScrollBar(GetVerticalScrollBar() != NULL, true);
			if( GetHorizontalScrollBar() ) GetHorizontalScrollBar()->ApplyAttributeList(pstrValue);
		}
		else if( _tcscmp(pstrName, _T("childpadding")) == 0 ) SetChildPadding(_ttoi(pstrValue));
		else if(_tcscmp(pstrName, _T("flowable")) == 0) SetContainerFlowable(_tcscmp(pstrValue, _T("true")) == 0);
		else if(_tcscmp(pstrName, _T("scrollbartakeplace")) == 0) ScrollBarTakePlace(_tcscmp(pstrValue, _T("true")) == 0);
		else if(_tcscmp(pstrName, _T("sizegrow")) == 0)
		{
			if(_tcscmp(pstrValue, _T("true")) == 0)
				m_bSizeGrow = true;
		}
		else CControlUI::SetAttribute(pstrName, pstrValue);
	}

	void CContainerUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
	{
		for( int it = 0; it < m_items.GetSize(); it++ )
			static_cast<CControlUI*>(m_items[it])->SetManager(pManager, this, bInit);
		if( m_pVerticalScrollBar != NULL ) m_pVerticalScrollBar->SetManager(pManager, this, bInit);
		if( m_pHorizontalScrollBar != NULL ) m_pHorizontalScrollBar->SetManager(pManager, this, bInit);
		CControlUI::SetManager(pManager, pParent, bInit);
	}

	CControlUI* CContainerUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
	{
		// Check if this guy is valid
		if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
		if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
		//一般从Point找都带HITTEST,用来直接判断是否在此区域内;
		if( (uFlags & UIFIND_HITTEST) != 0 ) {
			//不在此区域直接返回;
			if( !::PtInRect(&m_rcItem, *(static_cast<LPPOINT>(pData))) ) return NULL;
			//在本容器区域,若本区域鼠标禁用,则直接返回自己;
			if( !m_bMouseChildEnabled ) {
				CControlUI* pResult = NULL;
				if( m_pVerticalScrollBar != NULL ) pResult = m_pVerticalScrollBar->FindControl(Proc, pData, uFlags);
				if( pResult == NULL && m_pHorizontalScrollBar != NULL ) pResult = m_pHorizontalScrollBar->FindControl(Proc, pData, uFlags);
				if( pResult == NULL ) pResult = CControlUI::FindControl(Proc, pData, uFlags);
				return pResult;
			}
		}
		//先判断是否在滚动条内;
		CControlUI* pResult = NULL;
		if( m_pVerticalScrollBar != NULL ) pResult = m_pVerticalScrollBar->FindControl(Proc, pData, uFlags);
		if( pResult == NULL && m_pHorizontalScrollBar != NULL ) pResult = m_pHorizontalScrollBar->FindControl(Proc, pData, uFlags);
		if( pResult != NULL ) return pResult;
		//若带ME_FIRST则直接确认自己;
		if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
			CControlUI* pControl = CControlUI::FindControl(Proc, pData, uFlags);
			if( pControl != NULL ) return pControl;
		}
		RECT rc = m_rcItem;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsTakePlace() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
		if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsTakePlace() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
		//若带TOP_FIRST则从绘制的顶层开始找;
		if( (uFlags & UIFIND_TOP_FIRST) != 0 ) {
			for( int it = m_items.GetSize() - 1; it >= 0; it-- ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it])->FindControl(Proc, pData, uFlags);
				if( pControl != NULL ) {
					if( (uFlags & UIFIND_HITTEST) != 0 && !pControl->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
						continue;
					else 
						return pControl;
				}            
			}
		}
		else {
			for( int it = 0; it < m_items.GetSize(); it++ ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it])->FindControl(Proc, pData, uFlags);
				if( pControl != NULL ) {
					if( (uFlags & UIFIND_HITTEST) != 0 && !pControl->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
						continue;
					else 
						return pControl;
				} 
			}
		}
		//若没找到,且不带ME_FIRST,则在最后确认是不是自己;
		if( pResult == NULL && (uFlags & UIFIND_ME_FIRST) == 0 ) pResult = CControlUI::FindControl(Proc, pData, uFlags);
		return pResult;
	}

	void CContainerUI::DoPaintWithOutClip(const RECT& rcPaint)
	{
		RECT rcTemp = { 0 };
		CControlUI::DoPaint(rcPaint);
		if( m_items.GetSize() > 0 ) 
		{
			RECT rc = m_rcItem;
			rc.left += m_rcInset.left;
			rc.top += m_rcInset.top;
			rc.right -= m_rcInset.right;
			rc.bottom -= m_rcInset.bottom;
			if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsTakePlace() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
			if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsTakePlace() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
			
			for( int it = 0; it < m_items.GetSize(); it++ ) 
			{
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
				if( !pControl->IsVisible() ) continue;
				if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
				if( pControl ->IsFloat() )
				{
					if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
					pControl->DoPaintWithOutClip(rcPaint);
				}
				else
				{
					if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
					pControl->DoPaintWithOutClip(rcPaint);
				}
			}
		}

		if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible()&&m_pVerticalScrollBar->GetScrollRange() != 0) 
			if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()) ) 
				m_pVerticalScrollBar->DoPaint(rcPaint);

		if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible()&&m_pVerticalScrollBar->GetScrollRange() != 0 )
			if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()) )
				m_pHorizontalScrollBar->DoPaint(rcPaint);
	}

	void CContainerUI::DoPaint(const RECT& rcPaint)
	{
		RECT rcTemp = { 0 };
		if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return;
		
		CRenderClip clip;
		clip.SetPaintManager(m_pManager);
		CRenderClip::GenerateClip(rcTemp,clip,OP_AND);
		CControlUI::DoPaint(rcPaint);

		if( m_items.GetSize() > 0 ) 
		{
			RECT rc = m_rcItem;
			rc.left += m_rcInset.left;
			rc.top += m_rcInset.top;
			rc.right -= m_rcInset.right;
			rc.bottom -= m_rcInset.bottom;
 			if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsTakePlace() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
 			if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsTakePlace() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
			//After Inset.Op no Intersect Aera in RcPaint
			//Thus,float Controls are left to check in Update Aera, cause they can be float on the inset Aera.
			if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) 
			{
				for( int it = 0; it < m_items.GetSize(); it++ )
				{
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if( !pControl->IsVisible() ) continue;
					//Travers all float Controls in this Container.Find the Controls inside the RcPaint;
					if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
					//And its float Control;
					if( pControl ->IsFloat() )
					{
						//Finally,make sure they are inside the Container;If so,Paint it;
						if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
						pControl->DoPaint(rcPaint);
					}
				}
			}
			else 
			{
				CRenderClip childClip;
				childClip.SetPaintManager(m_pManager);
				//Get the intersectRc after inset in RcPaint;we clip it;
				CRenderClip::GenerateClip(rcTemp, childClip,OP_AND);
				for( int it = 0; it < m_items.GetSize(); it++ ) 
				{
					CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
					if( !pControl->IsVisible() ) continue;
					if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
					//Travers all the controls,find the Controls inside the RcPaint;
					if( pControl ->IsFloat() )
					{	//make sure they are inside the Container;
						if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
						CRenderClip::UseOldClipBegin(childClip);
						pControl->DoPaint(rcPaint);
						CRenderClip::UseOldClipEnd(childClip);
					}
					else
					{
						if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
						pControl->DoPaint(rcPaint);
					}
				}
			}
		}

		if( m_pVerticalScrollBar&&m_pVerticalScrollBar->IsVisible()&&m_pVerticalScrollBar->GetScrollRange() != 0) 
			if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos())) 
				m_pVerticalScrollBar->DoPaint(rcPaint);

		if( m_pHorizontalScrollBar&&m_pHorizontalScrollBar->IsVisible()&&m_pHorizontalScrollBar->GetScrollRange() != 0) 
			if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos())) 
				m_pHorizontalScrollBar->DoPaint(rcPaint);
	}

	void CContainerUI::SetFloatPos(int iIndex)
	{
		// 因为CControlUI::SetPos对float的操作影响，这里不能对float组件添加滚动条的影响
		if( iIndex < 0 || iIndex >= m_items.GetSize() ) return;

		CControlUI* pControl = static_cast<CControlUI*>(m_items[iIndex]);

		if( !pControl->IsVisible() ) return;
		if( !pControl->IsFloat() ) return;

		SIZE szXY = pControl->GetFixedXY();
		SIZE sz = {pControl->GetFixedWidth(), pControl->GetFixedHeight()};
		RECT rcCtrl = { 0 };
		if( szXY.cx >= 0 ) {
			rcCtrl.left = m_rcItem.left + szXY.cx;
			rcCtrl.right = m_rcItem.left + szXY.cx + sz.cx;
		}
		else {
			rcCtrl.left = m_rcItem.right + szXY.cx - sz.cx;
			rcCtrl.right = m_rcItem.right + szXY.cx;
		}
		if( szXY.cy >= 0 ) {
			rcCtrl.top = m_rcItem.top + szXY.cy;
			rcCtrl.bottom = m_rcItem.top + szXY.cy + sz.cy;
		}
		else {
			rcCtrl.top = m_rcItem.bottom + szXY.cy - sz.cy;
			rcCtrl.bottom = m_rcItem.bottom + szXY.cy;
		}
		if( pControl->IsRelativePos() )
		{
			TRelativePosUI tRelativePos = pControl->GetRelativePos();
			SIZE szParent = {m_rcItem.right-m_rcItem.left,m_rcItem.bottom-m_rcItem.top};
			if(tRelativePos.szParent.cx != 0)
			{
				int nIncrementX = szParent.cx-tRelativePos.szParent.cx;
				int nIncrementY = szParent.cy-tRelativePos.szParent.cy;
				rcCtrl.left += (nIncrementX*tRelativePos.nMoveXPercent/100);
				rcCtrl.top += (nIncrementY*tRelativePos.nMoveYPercent/100);
				rcCtrl.right = rcCtrl.left+sz.cx+(nIncrementX*tRelativePos.nZoomXPercent/100);
				rcCtrl.bottom = rcCtrl.top+sz.cy+(nIncrementY*tRelativePos.nZoomYPercent/100);
			}
			pControl->SetRelativeParentSize(szParent);
		}
		pControl->SetPos(rcCtrl);
	}

	void CContainerUI::ProcessScrollBar(RECT rc, int cxRequired, int cyRequired)
	{
		if(m_pVerticalScrollBar)
		{
			int cyScroll = cyRequired - (rc.bottom - rc.top);
			if( cyScroll <= 0 ) 
			{
				SIZE szPos = {0,0};
				if(m_pHorizontalScrollBar)
					szPos.cx = m_pHorizontalScrollBar->GetScrollPos();
				SetScrollPos(szPos);
				m_pVerticalScrollBar->SetScrollRange(0);
				m_pVerticalScrollBar->SetVisible(false);
			}
			else
			{
				m_pVerticalScrollBar->SetVisible(true);
				RECT rcScrollBarPos = {rc.right-m_pVerticalScrollBar->GetFixedWidth(), rc.top, rc.right , rc.bottom};
				if(m_pVerticalScrollBar->IsShowTakePlace())
				{
					rcScrollBarPos.left = rc.right;
					rcScrollBarPos.right = rc.right+m_pVerticalScrollBar->GetFixedWidth();
				}
				m_pVerticalScrollBar->SetPos(rcScrollBarPos);
				//当删除靠末尾的项时,Range <　滚动条OldPos.滚动条缩短的同时,布局也应该往上挪,否则还按照原来的IPos SetPos的就会出现空白;
				if(m_pVerticalScrollBar->SetScrollRange(::abs(cyScroll)))
					SetPos(m_rcItem);
			}
		}

		if(m_pHorizontalScrollBar)
		{
			int cxScroll = cxRequired - (rc.right - rc.left);
			if( cxScroll <= 0) 
			{
				SIZE szPos = {0,0};
				if(m_pVerticalScrollBar)
					szPos.cy = m_pVerticalScrollBar->GetScrollPos();
				SetScrollPos(szPos);
				m_pHorizontalScrollBar->SetScrollRange(0);
				m_pHorizontalScrollBar->SetVisible(false);
			}
			else
			{
				m_pHorizontalScrollBar->SetVisible(true);
				RECT rcScrollBarPos = { rc.left, rc.bottom-m_pHorizontalScrollBar->GetFixedHeight(), rc.right,rc.bottom};
				if(m_pHorizontalScrollBar->IsShowTakePlace())
				{
					rcScrollBarPos.top = rc.bottom;
					rcScrollBarPos.bottom = rc.bottom+m_pHorizontalScrollBar->GetFixedHeight();
				}
				m_pHorizontalScrollBar->SetPos(rcScrollBarPos);
				if(m_pHorizontalScrollBar->SetScrollRange(::abs(cxScroll)))
					SetPos(m_rcItem);
			}
		}
	}

	bool CContainerUI::SetSubControlText( LPCTSTR pstrSubControlName,LPCTSTR pstrText )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetText(pstrText);
			return TRUE;
		}
		else
			return FALSE;
	}

	bool CContainerUI::SetSubControlFixedHeight( LPCTSTR pstrSubControlName,int cy )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetFixedHeight(cy);
			return TRUE;
		}
		else
			return FALSE;
	}

	bool CContainerUI::SetSubControlFixedWdith( LPCTSTR pstrSubControlName,int cx )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetFixedWidth(cx);
			return TRUE;
		}
		else
			return FALSE;
	}

	bool CContainerUI::SetSubControlUserData( LPCTSTR pstrSubControlName,LPCTSTR pstrText )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl!=NULL)
		{
			pSubControl->SetUserData(pstrText);
			return TRUE;
		}
		else
			return FALSE;
	}

	UiLib::CDuiString CContainerUI::GetSubControlText( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return _T("");
		else
			return pSubControl->GetText();
	}

	int CContainerUI::GetSubControlFixedHeight( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return -1;
		else
			return pSubControl->GetFixedHeight();
	}

	int CContainerUI::GetSubControlFixedWdith( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return -1;
		else
			return pSubControl->GetFixedWidth();
	}

	const CDuiString CContainerUI::GetSubControlUserData( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=this->FindSubControl(pstrSubControlName);
		if (pSubControl==NULL)
			return _T("");
		else
			return pSubControl->GetUserData();
	}

	CControlUI* CContainerUI::FindSubControl( LPCTSTR pstrSubControlName )
	{
		CControlUI* pSubControl=NULL;
		pSubControl=static_cast<CControlUI*>(GetManager()->FindSubControlByName(this,pstrSubControlName));
		return pSubControl;
	}

	double CContainerUI::CalculateDelay(double state)
	{
		return pow(state, 2);
	}

	void CContainerUI::SetEnabled( bool bEnable,bool bSelf)
	{
		if(bSelf)
			CControlUI::SetEnabled(bEnable);
		else
		{
			for(int i = 0 ; i < GetCount() ;i++)
				GetItemAt(i)->SetEnabled(bEnable);
		}
	}

} // namespace UiLib
