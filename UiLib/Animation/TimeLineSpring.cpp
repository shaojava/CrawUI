#include "StdAfx.h"
#include "TimeLineSpring.h"

namespace UiLib
{

	CTimeLineSpring::CTimeLineSpring()
	{
		m_fTension = 232.2f;
		m_fFriction = 5;
		m_fOffsetThreshold = 0.01f;
	}

	CTimeLineSpring::~CTimeLineSpring()
	{

	}

	void CTimeLineSpring::SetSpringInfo( float fTension,float fFriction )
	{
		m_fTension = fTension;
		m_fFriction = fFriction;
	}

	void CTimeLineSpring::interpolate( AnimateSpringProperty *pProperty,float alpha )
	{
		if(!pProperty) return;

		pProperty->fCurrentValue = pProperty->fCurrentValue * alpha + pProperty->fPrevusValue *(1-alpha);
		pProperty->fCurrentVelocity = pProperty->fCurrentVelocity * alpha + pProperty->fPrevusVelocity *(1-alpha);
	}

	void CTimeLineSpring::AddProperty( CDuiString szHashKey,float fStartValue,float fEndValue )
	{
		AnimateSpringProperty *tmpProperty = NULL;

		AnimatePropertiesIt PropertyIt = m_Properties.find(szHashKey);
		if(PropertyIt != m_Properties.end())
		{
			tmpProperty = (AnimateSpringProperty *)(PropertyIt->second);
			tmpProperty->fCurrentValue = fStartValue;
			tmpProperty->fCurrentVelocity = 0;
			tmpProperty->fTempValue = 0;
			tmpProperty->fTempVelocity = 0;
			tmpProperty->fPrevusValue = 0;
			tmpProperty->fPrevusVelocity = 0;
			tmpProperty->fStartValue = fStartValue;
			tmpProperty->fEndValue = fEndValue;
		}
		else
		{
			tmpProperty = new AnimateSpringProperty();
			tmpProperty->fCurrentValue = fStartValue;
			tmpProperty->fCurrentVelocity = 0;
			tmpProperty->fTempValue = 0;
			tmpProperty->fTempVelocity = 0;
			tmpProperty->fPrevusValue = 0;
			tmpProperty->fPrevusVelocity = 0;
			tmpProperty->fStartValue = fStartValue;
			tmpProperty->fEndValue = fEndValue;
			m_Properties.insert(make_pair(szHashKey,tmpProperty));
		}
	}

	bool CTimeLineSpring::IsAtRest()
	{
		if(m_Properties.size())
		{
			AnimatePropertiesIt PropertyIt = m_Properties.begin();
			AnimateSpringProperty *tmpProperty = (AnimateSpringProperty *)(PropertyIt->second);
			if(fabs(tmpProperty->fEndValue-tmpProperty->fCurrentValue) <= m_fOffsetThreshold)
				return true;
		}
		return false;
	}

	bool CTimeLineSpring::Step( long lDeltaMillis )
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

		float fDeltaMills = lDeltaMillis *0.001;
		if (fDeltaMills > MAX_DELTA_TIME_SEC)
			fDeltaMills = MAX_DELTA_TIME_SEC;

		m_fTimeCounts += fDeltaMills;

		if (m_bDelayFinished)
		{
			if (!IsAtRest()) 
			{
				for each (AnimateProperties::value_type elem in m_Properties) 
				{
					AnimateSpringProperty *prop = (AnimateSpringProperty *)(elem.second);
					DoSpring(prop);
				}
			}
			else
			{
				for each (AnimateProperties::value_type elem in m_Properties) 
				{
					AnimateSpringProperty *prop = (AnimateSpringProperty *)(elem.second);
					prop->fCurrentValue = prop->fEndValue;
				}
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

	void CTimeLineSpring::DoSpring( AnimateSpringProperty *pProperty)
	{
		if(!pProperty) return;

		float position = pProperty->fCurrentValue;
		float velocity = pProperty->fCurrentVelocity;
		float tempPosition = pProperty->fTempValue;
		float tempVelocity = pProperty->fTempVelocity;

		float aVelocity, aAcceleration;
		float bVelocity, bAcceleration;
		float cVelocity, cAcceleration;
		float dVelocity, dAcceleration;

		float dxdt, dvdt;

		while (m_fTimeCounts >= SOLVER_TIMESTEP_SEC)
		{
			m_fTimeCounts -= SOLVER_TIMESTEP_SEC;

			if (m_fTimeCounts < SOLVER_TIMESTEP_SEC) 
			{
				pProperty->fPrevusValue = position;
				pProperty->fPrevusVelocity = velocity;
			}
			aVelocity = velocity;
			aAcceleration = (m_fTension * (pProperty->fEndValue - tempPosition)) - m_fFriction * velocity;

			tempPosition = position + aVelocity * SOLVER_TIMESTEP_SEC * 0.5;
			tempVelocity = velocity + aAcceleration * SOLVER_TIMESTEP_SEC * 0.5;
			bVelocity = tempVelocity;
			bAcceleration = (m_fTension * (pProperty->fEndValue - tempPosition)) - m_fFriction * tempVelocity;

			tempPosition = position + bVelocity * SOLVER_TIMESTEP_SEC * 0.5;
			tempVelocity = velocity + bAcceleration * SOLVER_TIMESTEP_SEC * 0.5;
			cVelocity = tempVelocity;
			cAcceleration = (m_fTension * (pProperty->fEndValue - tempPosition)) - m_fFriction * tempVelocity;

			tempPosition = position + cVelocity * SOLVER_TIMESTEP_SEC;
			tempVelocity = velocity + cAcceleration * SOLVER_TIMESTEP_SEC;
			dVelocity = tempVelocity;
			dAcceleration = (m_fTension * (pProperty->fEndValue - tempPosition)) - m_fFriction * tempVelocity;

			dxdt = 1.0/6.0 * (aVelocity + 2.0 * (bVelocity + cVelocity) + dVelocity);
			dvdt = 1.0/6.0 * (aAcceleration + 2.0 * (bAcceleration + cAcceleration) + dAcceleration);

			position += dxdt * SOLVER_TIMESTEP_SEC;
			velocity += dvdt * SOLVER_TIMESTEP_SEC;
		}

		pProperty->fTempValue = tempPosition;
		pProperty->fTempVelocity = tempVelocity;

		pProperty->fCurrentValue = position;
		pProperty->fCurrentVelocity = velocity;

		if (m_fTimeCounts > 0)
			interpolate(pProperty,m_fTimeCounts / SOLVER_TIMESTEP_SEC);
	}

	bool CTimeLineSpring::Stop( STOPSTATE ss)
	{
		m_bFinished = true;
		for each (AnimateProperties::value_type elem in m_Properties) 
		{
			AnimateSpringProperty *prop = (AnimateSpringProperty *)(elem.second);
			prop->fCurrentValue = prop->fEndValue;
		}
		if(m_pManager)
		{
			m_pManager->DispatchEvent(this,ON_STEP);
			m_pManager->DispatchEvent(this,ON_COMPLETE);
		}
		return m_bFinished;
	}

	void CTimeLineSpring::ReSetTimeLine()
	{
		m_bFinished = false;
	}

}

