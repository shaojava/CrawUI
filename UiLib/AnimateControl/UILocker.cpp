#include "StdAfx.h"
#include "UILocker.h"

namespace UiLib
{
	REGIST_DUICLASS(CLockerItemUI);
	REGIST_DUICLASS(CLockerUI);

	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////

	CLockerItemUI::CLockerItemUI()
	{
		m_pOwner = NULL;
		m_bExpand = false;
	}

	CLockerItemUI::~CLockerItemUI()
	{

	}

	LPCTSTR CLockerItemUI::GetClass() const
	{
		return _T("LockerItemUI");
	}

	void CLockerItemUI::SetTrayShrinkSize( int nTrayHeight )
	{
		m_nTrayShrinkHeight = nTrayHeight;
	}

	int CLockerItemUI::GetTrayShrinkSize()
	{
		return m_nTrayShrinkHeight;
	}

	int CLockerItemUI::GetTrayExpandSize()
	{
		return EstimateNeedSize().cy;
	}

	void CLockerItemUI::SetOwner( CLockerUI *pLocker )
	{
		m_pOwner = pLocker;
	}

	void CLockerItemUI::DoEvent( TEventUI& event )
	{
		if(event.Type == UIEVENT_BUTTONDOWN)
		{
			if(m_pOwner)
				m_pOwner->ExpandItem(this);
		}
		CVerticalLayoutUI::DoEvent(event);
	}

	LPVOID CLockerItemUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, _T("LockerItem")) == 0 ) return static_cast<CLockerItemUI*>(this);
		return CVerticalLayoutUI::GetInterface(pstrName);
	}

	void CLockerItemUI::Expand( bool bExpand)
	{
		m_bExpand = bExpand;
	}

	bool CLockerItemUI::IsExpand()
	{
		return m_bExpand;
	}


	///////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////

	CLockerUI::CLockerUI(void)
	{
		m_pExpandTime = NULL;
		//m_bDamp = false;
	}


	CLockerUI::~CLockerUI(void)
	{
		m_pStoryBoard->Stop(m_pExpandTime);
		SAFE_DELETE(m_pExpandTime);
	}

	LPCTSTR CLockerUI::GetClass() const
	{
		return _T("LockerUI");
	}

	void CLockerUI::DoInit()
	{
		UpdateItemHeightInfo();
		__super::DoInit();
	}

	bool CLockerUI::Add( CControlUI* pControl )
	{
		if(pControl->GetInterface(_T("LockerItem"))) 
		{
			CLockerItemUI *pLokerItem = (CLockerItemUI *)pControl;
			pLokerItem->SetOwner(this);
			pLokerItem->SetTrayShrinkSize(100);
			pLokerItem->SetFixedHeight(100);
		}
		return CVerticalLayoutUI::Add(pControl);
	}

	void CLockerUI::ExpandItem( CLockerItemUI *pItem )
	{
		if(pItem->IsExpand())
			return;
		m_pStoryBoard->Stop(m_pExpandTime);
		SAFE_DELETE(m_pExpandTime);
		m_pExpandTime = new CTimeLineTween();
		m_pExpandTime->SetTweenInfo(1000);
		for(int i = 0 ; i < m_ItemInfos.size() ; i++)
		{
			if(pItem == m_ItemInfos[i]->pItem)
			{
				m_pExpandTime->AddProperty(m_ItemInfos[i]->szTag,m_ItemInfos[i]->fHeight,m_ItemInfos[i]->pItem->GetTrayExpandSize());
				m_ItemInfos[i]->pItem->Expand(true);
			}
			else
			{
				m_pExpandTime->AddProperty(m_ItemInfos[i]->szTag,m_ItemInfos[i]->fHeight,m_ItemInfos[i]->pItem->GetTrayShrinkSize());
				m_ItemInfos[i]->pItem->Expand(false);
			}
		}
		m_pStoryBoard->AddTimeLine(m_pExpandTime);
	}

	void CLockerUI::ShrinkItem( CLockerItemUI *pItem )
	{
		if(pItem)
			pItem->SetFixedHeight(pItem->GetTrayShrinkSize());
	}

	void CLockerUI::UpdateItemHeightInfo()
	{
		int nItemCount = m_items.GetSize();
		if(!nItemCount) return;
		for(int i = 0 ; i < nItemCount ;i++)
		{
			CControlUI *pControl = static_cast<CControlUI*>(m_items[i]);
			if(pControl->GetInterface(_T("LockerItem")))
			{
				CLockerItemUI *pLokerItem = (CLockerItemUI *)pControl;
				ItemHeightInfo *pItemInfo = new ItemHeightInfo;
				pItemInfo->pItem = pLokerItem;
				pItemInfo->fHeight = (float)pLokerItem->GetFixedHeight();
				pItemInfo->szTag.Format(_T("%d"),i);
				m_ItemInfos.push_back(pItemInfo);
			}
		}
	}

	bool CLockerUI::OnAnimateStep( TimeEvent *pEvent )
	{
		if(pEvent->pTimeLine == m_pExpandTime)
		{
			for(int i = 0 ; i < m_ItemInfos.size() ; i++)
			{
				CLockerItemUI *pLokerItem = m_ItemInfos[i]->pItem;
				if(pLokerItem)
				{
					m_pExpandTime->GetPropertyCurrentValue(m_ItemInfos[i]->szTag,m_ItemInfos[i]->fHeight);
					pLokerItem->SetFixedHeight(m_ItemInfos[i]->fHeight);
				}
			}
		}
		return __super::OnAnimateStep(pEvent);
	}

	void CLockerUI::ProcessScrollBar( RECT rc, int cxRequired, int cyRequired )
	{
		if(m_pVerticalScrollBar)
		{
			int cyScroll = cyRequired - (rc.bottom - rc.top);
			if( cyScroll <= 0) 
			{
				m_pVerticalScrollBar->SetScrollPos(0);
				m_pVerticalScrollBar->SetScrollRange(0);
			}
			else
			{
				RECT rcScrollBarPos = {rc.right-m_pVerticalScrollBar->GetFixedWidth(), rc.top, rc.right , rc.bottom};
				if(m_pVerticalScrollBar->IsShowTakePlace())
				{
					rcScrollBarPos.left = rc.right;
					rcScrollBarPos.right = rc.right+m_pVerticalScrollBar->GetFixedWidth();
				}
				m_pVerticalScrollBar->SetPos(rcScrollBarPos);
				int nOldRange = m_pVerticalScrollBar->GetScrollRange();
				if(nOldRange)
				{
					float fDelataPercent = (nOldRange-cyScroll)/(float)nOldRange;
					int iScrollPos = m_pVerticalScrollBar->GetScrollPos();
					int nNewPos = iScrollPos-iScrollPos*fDelataPercent;
					m_pVerticalScrollBar->SetScrollPosDirect(nNewPos);
					m_pVerticalScrollBar->SetScrollRangeFlex(::abs(cyScroll));
				}
				else
					m_pVerticalScrollBar->SetScrollRangeFlex(::abs(cyScroll));
			}
		}
	}

	void CLockerUI::SetPos( RECT rc )
	{
		__super::SetPos(rc);
	}

}