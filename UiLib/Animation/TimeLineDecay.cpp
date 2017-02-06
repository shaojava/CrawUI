#include "StdAfx.h"
#include "TimeLineDecay.h"

namespace UiLib
{

	CTimeLineDecay::CTimeLineDecay()
	{
		m_fDecayDeceleration = 0.998f;
		m_fVelocityThreshold = 0.005f;
		m_fOffsetThreshold = 0.5;
		m_fCurrentValue = 0;
	}

	CTimeLineDecay::~CTimeLineDecay()
	{

	}

	void CTimeLineDecay::DecayPosition( float *x,float *v,double dTime,float fDeceleration )
	{
		// v0 = v / 1000
		// v = v0 * powf(deceleration, dt);
		// v = v * 1000;
		// x0 = x;
		// x = x0 + v0 * deceleration * (1 - powf(deceleration, dt)) / (1 - deceleration)
		float kv = powf(fDeceleration, dTime);
		float kx = fDeceleration * (1 - kv) / (1 - fDeceleration);
		float v0 = (*v)*0.001f;
		(*v)*= kv;
		(*x)+= v0 * kx;
	}

	void CTimeLineDecay::SetVelocity( float fVelocity )
	{
		m_fVelocity = fVelocity;
	}

	void CTimeLineDecay::UpdateVelocity( float fVelocity )
	{
		if(!m_bPause)
			m_fVelocity += fVelocity;
	}

	void CTimeLineDecay::SetDecayInfo( float fDecaleeration,float fVelocityThreshold,float fOffsetThreshold )
	{
		m_fDecayDeceleration = fDecaleeration;
		m_fVelocityThreshold = fVelocityThreshold;
		m_fOffsetThreshold = fOffsetThreshold;
	}

	bool CTimeLineDecay::IsAtRest( float fCurrvalue )
	{
		if(fabs(m_fVelocity) <= m_fVelocityThreshold)
			return true;
		if(fCurrvalue != -1)
		{
			if(fCurrvalue <= m_fOffsetThreshold)
				return true;
		}
		return false;
	}

	bool CTimeLineDecay::Step( long lDeltaMillis )
	{
		if(m_bFinished)
			return false;
		if(m_bPause)
			return true;
		if(!m_bStarted)
		{
			m_bStarted = true;
			if(m_pManager) m_pManager->DispatchEvent(this,ON_START);
		}

		lDeltaMillis *= m_nDirection;
		m_fTimeCounts += lDeltaMillis;

		if (m_bDelayFinished)
		{
			if (!IsAtRest()) 
			{
				float fPreValue = m_fCurrentValue;
				DecayPosition(&m_fCurrentValue,&m_fVelocity,lDeltaMillis,m_fDecayDeceleration);
				if (lDeltaMillis&&IsAtRest(fabs(m_fCurrentValue-fPreValue))) 
				{
					m_fVelocity = 0;
					m_bFinished = true;
					if(m_pManager)
					{
						m_pManager->DispatchEvent(this,ON_STEP);
						m_pManager->DispatchEvent(this,ON_COMPLETE);
					}
					return false;
				}
				else
				{
					for each (AnimateProperties::value_type elem in m_Properties) 
					{
						AnimateProperty *prop = elem.second;
						prop->fCurrentValue = prop->fStartValue+m_fCurrentValue;
					}
				}
			}
			else
			{
				m_fVelocity = 0;
				m_bFinished = true;
				if(m_pManager)
				{
					m_pManager->DispatchEvent(this,ON_STEP);
					m_pManager->DispatchEvent(this,ON_COMPLETE);
				}
				return false;
			}
		}
		else if(!m_bDelayFinished && m_fTimeCounts > m_fDelayMilliSecs)
		{
			m_bDelayFinished = true;
			m_fTimeCounts = 0;
		}
		if(m_pManager) m_pManager->DispatchEvent(this,ON_STEP);
		return true;
	}

	float CTimeLineDecay::GetPlanDuration()
	{
		// compute duration till threshold velocity
		double scaledVelocity = m_fVelocity * 0.001;
		double k = m_fVelocityThreshold  *0.001;
		double v = k / scaledVelocity;
		double d = log(m_fDecayDeceleration) * 1000.0f;
		float duration = log(fabs(v)) / d;
		// ensure velocity threshold is exceeded
		if (duration < 0)
			duration = 0;
		return duration;
	}

	float CTimeLineDecay::GetPlanDestination()
	{
		float fromValue = m_fCurrentValue;
		float duration = GetPlanDuration();
		float toValue = fromValue;
		float velocity = m_fVelocity;
		DecayPosition(&toValue,&velocity,duration,m_fDecayDeceleration);
		return toValue;
	}

	bool CTimeLineDecay::Stop( STOPSTATE ss)
	{
		m_bFinished = true;
		m_fVelocity = 0;
		if(m_pManager)
		{
			m_pManager->DispatchEvent(this,ON_STEP);
			m_pManager->DispatchEvent(this,ON_COMPLETE);
		}
		return m_bFinished;
	}

	void CTimeLineDecay::ReSetTimeLine()
	{
		m_bFinished = false;
		m_fCurrentValue = 0;
	}

}

