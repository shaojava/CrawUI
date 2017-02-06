#include "StdAfx.h"
#include "StoryBoard.h"

namespace UiLib
{
	CStoryBoard::CStoryBoard()
	{
		lastTime = 0;
		m_bPause = false;
		m_pManager = NULL;
		m_StoryBoradState = SB_IDLING;
	}

	CStoryBoard::~CStoryBoard()
	{
		m_StoryBoradState = SB_IDLING;
		ClearTimeLine();
	}

	int CStoryBoard::GetTimeLineSize()
	{
		return m_nTimeLineCounts;
	}

	void CStoryBoard::AddTimeLine(CTimeLine* param)
	{
		if(!param) return;
		param->SetManager(this);
		m_TimeLines.push_back(param);
		m_nTimeLineCounts = m_TimeLines.size();
		Begin();
	}

	void CStoryBoard::UpdateTimeLine(CTimeLine *param)
	{
		if(!param) return;
		param->ReSetTimeLine();
		TimeLinesIt itRef = find(m_TimeLines.begin(),m_TimeLines.end(),param);
		if(itRef == m_TimeLines.end())
			AddTimeLine(param);
	}

	void CStoryBoard::RemoveTimeLine(CTimeLine* param)
	{
		m_nTimeLineCounts = m_TimeLines.size();
		m_TimeLinesIt = m_TimeLines.begin();
		for (int i=0; i <  m_nTimeLineCounts; i++,m_TimeLinesIt++) 
		{
			if (param == (*m_TimeLinesIt))
			{
				m_TimeLines.erase(m_TimeLinesIt);
				--m_nTimeLineCounts;
				break;
			}
		}
	}

	void CStoryBoard::ClearTimeLine()
	{
		m_TimeLines.clear();
		m_nTimeLineCounts = 0;
		m_StoryBoradState = SB_FINSHED;
		DispatchEvent(NULL,ON_END);
	}

	void CStoryBoard::Begin()
	{
		if(m_pManager)
		{
			lastTime = 0;
			m_pManager->AddStoryboard(this);
		}
	}

	void CStoryBoard::Step( long currentMillis )
	{
		if(lastTime == 0)
			lastTime = currentMillis;

		if(!m_nTimeLineCounts)
		{
			DispatchEvent(NULL,ON_END);
			lastTime = 0;
			return;
		}

		int t = 0 ;
		int d = 0;
		int dif = (currentMillis - lastTime);

		if(m_bPause)
		{
			m_StoryBoradState = SB_PAUSING;
			lastTime = currentMillis;
			return;
		}
		m_StoryBoradState = SB_RUNNING;


		m_TimeLinesIt = m_TimeLines.begin();

		for(; m_TimeLinesIt != m_TimeLines.end();)
		{
			CTimeLine *pTimeLine = *m_TimeLinesIt;
			if(!pTimeLine->Step(dif))
			{
				m_TimeLinesIt = m_TimeLines.erase(m_TimeLinesIt);
				m_nTimeLineCounts--;
			}
			else
				m_TimeLinesIt++;
		}

		lastTime = currentMillis;
	}

	void CStoryBoard::End(STOPSTATE ss)
	{
		for (m_TimeLinesIt = m_TimeLines.begin();  m_TimeLinesIt != m_TimeLines.end(); ++m_TimeLinesIt )
		{
			CTimeLine *pTimeLine = (*m_TimeLinesIt);
			pTimeLine->Stop(ss);
		}
		if(ss != SS_BACKTOINIT)
		{
			DispatchEvent(NULL,ON_END);
			ClearTimeLine();
			lastTime = 0;
			m_StoryBoradState = SB_FINSHED;
		}
	}

	void CStoryBoard::Stop( CTimeLine *param,STOPSTATE ss)
	{
		if(!m_nTimeLineCounts)
			return;
		if(!param)
			End(ss);
		else if((param))
		{
			if(param->Stop(ss))
				RemoveTimeLine(param);
		}
	}

	void CStoryBoard::Pause()
	{
		if(m_StoryBoradState == SB_RUNNING)
			m_bPause = true;
	}

	void CStoryBoard::Resume()
	{
		m_bPause = false;
	}

	bool CStoryBoard::IsPause()
	{
		return m_bPause;
	}

	void CStoryBoard::DispatchEvent( CTimeLine *param, short eventType )
	{
		if(OnAnimateTick)
		{
			TimeEvent tEvent;
			tEvent.pStoryBoard = this;
			tEvent.pTimeLine = param;
			tEvent.uState = (KeyFrameState)eventType;
			OnAnimateTick(&tEvent);
		}
	}

	bool CStoryBoard::IsFinished(CTimeLine *param)
	{
		if(!param)
		{
			if(m_nTimeLineCounts)
				return false;
		}
		else
		{
			TimeLinesIt itRef = find(m_TimeLines.begin(),m_TimeLines.end(),param);
			if(itRef != m_TimeLines.end())
				return false;
		}
		return true;
	}


	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////
	CAnimateSource::CAnimateSource()
	{
		m_aDelegates.Empty();
	}

	CAnimateSource::~CAnimateSource()
	{
		for( int i = 0; i < m_aDelegates.GetSize(); i++ ) 
		{
			CAnimateDelegateBase* pObject = m_aDelegates.GetAt(i);
			SAFE_DELETE(pObject);
		}
	}

	CAnimateSource::operator bool()
	{
		return m_aDelegates.GetSize() > 0;
	}

	void CAnimateSource::operator+=( const CAnimateDelegateBase& d )
	{
		for( int i = 0; i < m_aDelegates.GetSize(); i++ ) 
		{
			CAnimateDelegateBase* pObject = m_aDelegates.GetAt(i);
			if( pObject && pObject->Equals(d) ) return;
		}
		m_aDelegates.Add(d.Copy());
	}

	void CAnimateSource::operator-=( const CAnimateDelegateBase& d )
	{
		for( int i = 0; i < m_aDelegates.GetSize(); i++ ) 
		{
			CAnimateDelegateBase* pObject = m_aDelegates.GetAt(i);
			if( pObject && pObject->Equals(d) ) 
			{
				SAFE_DELETE(pObject);
				m_aDelegates.Remove(i);
				return;
			}
		}
	}

	bool CAnimateSource::operator()( TimeEvent* param )
	{
		for( int i = 0; i < m_aDelegates.GetSize(); i++ ) 
		{
			CAnimateDelegateBase* pObject = m_aDelegates.GetAt(i);
			if( pObject && !pObject->Invoke(param) ) 
				return false;
		}
		return true;
	}

}