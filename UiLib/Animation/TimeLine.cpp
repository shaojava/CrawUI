#include "StdAfx.h"
#include "TimeLine.h"

namespace UiLib
{
	CTimeLine::CTimeLine()
	{
		m_pManager = NULL;
		m_Properties.empty();
		m_nRepeatCount = 0;
		m_fDelayMilliSecs = 0;
		m_fTimeCounts = 0;
		m_nDirection = 1;
		m_bPause = false;
		m_bRepeatWithReverse = false;
		m_bStarted = false;
		m_bFinished = false;
		m_bDelayFinished = true;
	}

	CTimeLine::~CTimeLine()
	{
		Stop();
		CleanProperties();
	}

	void CTimeLine::ReSetTimeLine()
	{
		m_bFinished = false;
		m_nDirection = 1;
		m_fTimeCounts = 0;
	}

	void CTimeLine::SetDelayTime( float fDelayTime )
	{
		m_fDelayMilliSecs = fDelayTime;
		if(m_fDelayMilliSecs > 0)
			m_bDelayFinished = false;
	}

	void CTimeLine::SetRepeatWithReverse( int nRepeateTimes,bool bReverse)
	{
		m_nRepeatCount = nRepeateTimes-1;
		if (bReverse)
			m_bRepeatWithReverse = bReverse;
	}

	void CTimeLine::CleanProperties()
	{
		AnimatePropertiesIt it = m_Properties.begin(); 
		for (; it != m_Properties.end(); ++it) 
			SAFE_DELETE(it->second);
		m_Properties.clear();
	}

	bool CTimeLine::Step(long lDeltaMillis )
	{
		return true;
	}

	bool CTimeLine::DealRepeat()
	{
		return true;
	}

	void CTimeLine::AddProperty( CDuiString szHashKey,float fStartValue,float fEndValue )
	{
		AnimateProperty *tmpProperty = NULL;
		
		AnimatePropertiesIt PropertyIt = m_Properties.find(szHashKey);
		if(PropertyIt != m_Properties.end())
		{
			tmpProperty = PropertyIt->second;
			tmpProperty->fCurrentValue = fStartValue;
			tmpProperty->fStartValue = fStartValue;
			tmpProperty->fEndValue = fEndValue;
		}
		else
		{
			tmpProperty = new AnimateProperty;
			tmpProperty->fCurrentValue = fStartValue;
			tmpProperty->fStartValue = fStartValue;
			tmpProperty->fEndValue = fEndValue;
			m_Properties.insert(make_pair(szHashKey,tmpProperty));
		}
	}

	void CTimeLine::RemoveProperty(CDuiString szHashKey)
	{
		AnimatePropertiesIt PropertyIt = m_Properties.find(szHashKey);
		if(PropertyIt != m_Properties.end())
			m_Properties.erase(PropertyIt);
	}

	void CTimeLine::UpdateProperty( CDuiString szHashKey,float fCurrentValue )
	{
		AnimatePropertiesIt PropertyIt = m_Properties.find(szHashKey);
		if(PropertyIt != m_Properties.end())
			PropertyIt->second->fCurrentValue =  fCurrentValue;
		else
			return;
	}

	void CTimeLine::UpdateProperty( CDuiString szHashKey,float fStartValue,float fEndValue )
	{
		AnimatePropertiesIt PropertyIt = m_Properties.find(szHashKey);
		if(PropertyIt != m_Properties.end())
		{
			PropertyIt->second->fStartValue =  fStartValue;
			PropertyIt->second->fEndValue = fEndValue;
		}
		else
			return;
	}

	void CTimeLine::UpdateProperty( CDuiString szHashKey,float fCurrentValue,float fStartValue,float fEndValue )
	{
		AnimatePropertiesIt PropertyIt = m_Properties.find(szHashKey);
		if(PropertyIt != m_Properties.end())
		{
			PropertyIt->second->fStartValue =  fStartValue;
			PropertyIt->second->fEndValue = fEndValue;
			PropertyIt->second->fCurrentValue = fCurrentValue;
		}
		else
			return;
	}

	bool CTimeLine::GetProperty( CDuiString szHashKey,AnimateProperty **Property)
	{
		AnimatePropertiesIt PropertyIt = m_Properties.find(szHashKey);
		if(PropertyIt != m_Properties.end())
		{
			(*Property) = (*PropertyIt).second;
			return true;
		}
 		return false;
	}

	bool CTimeLine::GetPropertyCurrentValue( CDuiString szHashKey,float &fCurValue)
	{
		AnimatePropertiesIt PropertyIt = m_Properties.find(szHashKey);
		if(PropertyIt != m_Properties.end())
		{
			fCurValue = (*PropertyIt).second->fCurrentValue;
			return true;
		}
		return false;
	}

	bool CTimeLine::Stop(STOPSTATE ss)
	{
		return true;
	}

	void CTimeLine::Pause()
	{
		m_bPause = true;
	}

	void CTimeLine::Resume()
	{
		m_bPause = false;
	}

	bool CTimeLine::IsPause()
	{
		return m_bPause;
	}

}

