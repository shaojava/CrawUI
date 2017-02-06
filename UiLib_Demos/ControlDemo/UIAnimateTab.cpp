#include "UIAnimateTab.h"

CAnimateTabUI::CAnimateTabUI(void)
{
	m_fPosX = 0;
	m_pMoveTL = NULL;
}

CAnimateTabUI::~CAnimateTabUI(void)
{
	SAFE_DELETE(m_pMoveTL);
}

void CAnimateTabUI::DoEvent( TEventUI& event )
{
	if(event.Type == UIEVENT_SCROLLWHEEL)
	{
		if(m_pMoveTL)
		{
			switch (LOWORD(event.wParam))
			{
			case SB_LINEUP:
				m_pMoveTL->UpdateProperty(_T("Move"),m_fPosX,0);
				break;
			case SB_LINEDOWN:
				if(m_pMoveTL->IsPause())
					m_pMoveTL->Resume();
				else
					m_pMoveTL->Pause();
				
				return;
 			}
 			m_pStoryBoard->UpdateTimeLine(m_pMoveTL);
		}
		else
		{	
			m_pMoveTL = new CTimeLineSpring;
			RECT rcPos = GetPos();
			m_fPosX = rcPos.left;
			switch (LOWORD(event.wParam))
			{
			case SB_LINEUP:
				m_pMoveTL->AddProperty(_T("Move"),m_fPosX,m_fPosX+50);
				break;
			}
			m_pStoryBoard->AddTimeLine(m_pMoveTL);
 		}
	}
}

bool CAnimateTabUI::OnAnimateTimerTick( TimeEvent *pEvent )
{
	if(pEvent->pTimeLine == m_pMoveTL)
	{
		

		m_pMoveTL->GetPropertyCurrentValue(_T("Move"),m_fPosX);
		RECT rcPosNew = GetPos();
		rcPosNew.right = m_fPosX+100;
		rcPosNew.left = m_fPosX;
		SetPos(rcPosNew);
	}
	return true;
}

void CAnimateTabUI::Pause( bool bPause)
{
	if(bPause)
		m_pStoryBoard->Pause();
	else
		m_pStoryBoard->Resume();
}
