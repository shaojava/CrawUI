#include "stdafx.h"
#include "UIAnimationTabLayout.h"

namespace UiLib{
	REGIST_DUICLASS(CAnimationTabLayoutUI);

CAnimationTabLayoutUI::CAnimationTabLayoutUI() : 
m_bIsRunning(false),
m_bIsVerticalDirection(false), 
m_nPositiveDirection( 1 )
{
	m_AnimatID = 1001;
	m_iOldXOffset = 0;
	m_iOldYOffset = 0;
	m_nStep = 0;
	CControlUI::OnInit += MakeDelegate(this,&CAnimationTabLayoutUI::OnInit,_T(""));
}

LPCTSTR CAnimationTabLayoutUI::GetClass() const
{
	return _T("AnimationTabLayoutUI");
}

LPVOID CAnimationTabLayoutUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("AnimationTabLayoutUI")) == 0 )
		return static_cast<CAnimationTabLayoutUI*>(this);
	return CTabLayoutUI::GetInterface(pstrName);
}

bool CAnimationTabLayoutUI::SelectItem( int iIndex )
{
	if( iIndex < 0 || iIndex >= m_items.GetSize() ) return false;
	if( iIndex == m_iCurSel ) return true;
	if( iIndex > m_iCurSel ) m_nPositiveDirection = -1;
	if( iIndex < m_iCurSel ) m_nPositiveDirection = 1;

	m_iOldXOffset = 0;
	m_iOldYOffset = 0;
	if(m_bIsVerticalDirection)
		m_iOldYOffset = (m_rcItem.bottom-m_rcItem.top)*m_nPositiveDirection;
	else
		m_iOldXOffset = (m_rcItem.right-m_rcItem.left)*m_nPositiveDirection;

	m_rcCurPos = m_rcOldPos;

	m_iOldSel = m_iCurSel;
	m_iCurSel = iIndex;
	StopAnimation( m_AnimatID);
	StartAnimation( TAB_ANIMATION_ELLAPSE, TAB_ANIMATION_FRAME_COUNT, m_AnimatID);

	if( m_pManager != NULL ) {
		m_pManager->SetNextTabControl();
		m_pManager->SendNotify(this, _T("tabselect"), m_iCurSel, m_iOldSel);
	}
	return true;
}

void CAnimationTabLayoutUI::AnimationSwitch()
{
	if( !m_bIsVerticalDirection )
		m_nStep = ( m_rcItem.right - m_rcItem.left ) * m_nPositiveDirection / GetTotalFram(m_AnimatID);
	else
		m_nStep = ( m_rcItem.bottom - m_rcItem.top ) * m_nPositiveDirection / GetTotalFram(m_AnimatID);
	if(!m_bIsRunning)
	{
		if( !m_bIsVerticalDirection )
		{
			m_rcCurPos.top = m_rcItem.top;
			m_rcCurPos.bottom = m_rcItem.bottom;
			m_rcCurPos.left = m_rcItem.left - ( m_rcItem.right - m_rcItem.left ) * m_nPositiveDirection;
			m_rcCurPos.right = m_rcItem.right - ( m_rcItem.right - m_rcItem.left ) * m_nPositiveDirection;				
		}
		else
		{
			m_rcCurPos.left = m_rcItem.left;
			m_rcCurPos.right = m_rcItem.right;
			m_rcCurPos.top = m_rcItem.top - ( m_rcItem.bottom - m_rcItem.top ) * m_nPositiveDirection;
			m_rcCurPos.bottom = m_rcItem.bottom - ( m_rcItem.bottom - m_rcItem.top ) * m_nPositiveDirection;		
		}
	}
}

bool CAnimationTabLayoutUI::OnInit(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam)
{
	SetOwner(this);
	return true;
}


void CAnimationTabLayoutUI::DoEvent(TEventUI& event)
{
	if( event.Type == UIEVENT_TIMER ) 
		OnTimer(  event.wParam );
	__super::DoEvent( event );
}

void CAnimationTabLayoutUI::OnTimer( int nTimerID )
{
	OnAnimationElapse( nTimerID );
}

void CAnimationTabLayoutUI::OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID)
{
	int iStepLen = 0;
	if( !m_bIsVerticalDirection )
	{

		if( nCurFrame <= nTotalFrame)
		{
			m_rcCurPos.left = m_rcCurPos.left + m_nStep;
			m_rcCurPos.right = m_rcCurPos.right +m_nStep;
		}
		else
			m_rcCurPos = m_rcItem;
	}
	else
	{
		if(nCurFrame <= nTotalFrame)
		{
			m_rcCurPos.top = m_rcCurPos.top + m_nStep;
			m_rcCurPos.bottom = m_rcCurPos.bottom +m_nStep;			
		}
		else
		{
			m_rcCurPos = m_rcItem;
		}
	}

	CControlUI* pCur = static_cast<CControlUI *>(m_items[m_iCurSel]);
	if(pCur)
		pCur->SetPos(m_rcCurPos);
	
	CControlUI* pOld = static_cast<CControlUI *>(m_items[m_iOldSel]);
	RECT tmp = {m_rcCurPos.left+m_iOldXOffset,m_rcCurPos.top+m_iOldYOffset,m_rcCurPos.right+m_iOldXOffset,m_rcCurPos.bottom+m_iOldYOffset};
 	m_rcOldPos = tmp;
	if(pOld)
		pOld->SetPos(m_rcOldPos);
}

void CAnimationTabLayoutUI::OnAnimationStart(INT nAnimationID, BOOL bFirstLoop)
{
	m_pManager->SendNotify(this,_T("animationstart"));
	AnimationSwitch();
	GetItemAt(m_iCurSel)->SetVisible(true);
	GetItemAt(m_iCurSel)->SetFocus();
	m_bIsRunning = true;
}

void CAnimationTabLayoutUI::OnAnimationStop(INT nAnimationID) 
{
	m_pManager->SendNotify(this,_T("animationend"));
	for( int it = 0; it < m_items.GetSize(); it++ )
	{
		if(it != m_iCurSel)
			GetItemAt(it)->SetVisible(false);
	}
	m_bIsRunning = false;
}

void CAnimationTabLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("animation_direction")) == 0 && _tcscmp( pstrValue, _T("vertical")) == 0 ) m_bIsVerticalDirection = true; // pstrValue = "vertical" or "horizontal"
	if( _tcscmp(pstrName, _T("selectedid")) == 0 ) 
		CTabLayoutUI::SelectItem(_ttoi(pstrValue));
	if(_tcscmp(pstrName, _T("AnimatID")) == 0)
		m_AnimatID = _ttoi(pstrValue);
	return CTabLayoutUI::SetAttribute(pstrName, pstrValue);
}

void CAnimationTabLayoutUI::SetAnimationDirection(bool bVertical)
{
	m_bIsVerticalDirection = bVertical;
}

}