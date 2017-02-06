#include "StdAfx.h"
#include "UIMoveAbleContainer.h"

namespace UiLib
{
	REGIST_DUICLASS(CMoveAbleContainerUI);

	CMoveAbleContainerUI::CMoveAbleContainerUI(void)
	{
		m_bMoveing = FALSE;
		m_ptBtnDown.x = m_ptBtnDown.y = 0;
		m_bLBtnDown = FALSE;
		m_bFloat = TRUE;
	}


	CMoveAbleContainerUI::~CMoveAbleContainerUI(void)
	{
	}

	LPCTSTR CMoveAbleContainerUI::GetClass() const
	{
		return _T("MoveAbleConatinerUI");
	}

	LPVOID CMoveAbleContainerUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, _T("MoveAbleConatiner")) == 0 ) 
			return static_cast<CMoveAbleContainerUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	void CMoveAbleContainerUI::DoEvent( TEventUI& event )
	{
		if(event.Type == UIEVENT_BUTTONDOWN)
		{
			m_ptBtnDown = m_pManager->GetMousePos();
			m_bLBtnDown = TRUE;
			m_bFocused = TRUE;
		}
		else if(event.Type == UIEVENT_BUTTONUP)
		{
			m_bLBtnDown = FALSE;
			m_bFocused = FALSE;
		}
		else if(event.Type == UIEVENT_MOUSEMOVE)
		{
			if(m_bLBtnDown == TRUE && m_bFloat == TRUE)
			{
				POINT ptCur = m_pManager->GetMousePos();
				int nDeltaX = ptCur.x-m_ptBtnDown.x;
				int nDeltaY = ptCur.y-m_ptBtnDown.y;
				RECT rcPos = CContainerUI::GetPos();
				::OffsetRect(&rcPos,nDeltaX,nDeltaY);
				m_ptBtnDown = ptCur;
				CContainerUI::SetPos(rcPos);
			}
		}
		if(!event.bHandle)
			CContainerUI::DoEvent(event);
	}

}
