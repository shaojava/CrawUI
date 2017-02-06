#include "StdAfx.h"
#include "TimeLineTween.h"

namespace UiLib
{
 
	CTimeLineTween::CTimeLineTween()
	{
		m_TweenFunc = &fExpo;
		m_sEquation = EASE_OUT;
		m_fDuration = 0;
	}

	CTimeLineTween::~CTimeLineTween()
	{
	}

	bool CTimeLineTween::DealRepeat()
	{
		if(m_nRepeatCount <= 0 || !m_Properties.size()) return false;
		float fSwap;
		for each (AnimateProperties::value_type elem in m_Properties)
		{
			AnimateProperty* prop = elem.second;
			if(m_bRepeatWithReverse)
			{
				fSwap = prop->fEndValue;
				prop->fEndValue = prop->fStartValue;
				prop->fStartValue = fSwap;
			}
			prop->fCurrentValue = prop->fStartValue;
		}
		m_bStarted = false;
		m_fTimeCounts = 0;
		m_nRepeatCount--;
		return true;
	}
 
	bool CTimeLineTween::Step(long lDeltaMillis )
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
			if ( m_fTimeCounts >= 0 && m_fTimeCounts <= m_fDuration) 
			{
				for each (AnimateProperties::value_type elem in m_Properties) 
					RunEquation(elem.second);
			}
			else
			{
				if(m_fTimeCounts < 0)
					m_fTimeCounts = 0;
				if(m_fTimeCounts > m_fDuration)
					m_fTimeCounts = m_fDuration;
				for each (AnimateProperties::value_type elem in m_Properties) 
				{
					AnimateProperty *prop = elem.second;
					if(m_nDirection > 0)
						prop->fCurrentValue = prop->fEndValue;
					else
						prop->fCurrentValue = prop->fStartValue;
				}
				if(!DealRepeat())
				{
					m_bFinished = true;
					if(m_pManager)
					{
						m_pManager->DispatchEvent(this,ON_STEP);
						m_pManager->DispatchEvent(this,ON_COMPLETE);
					}
					return false;
				}
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

	bool CTimeLineTween::Stop(STOPSTATE ss)
	{
		m_bFinished = true;
		m_nRepeatCount = 0;
		switch(ss)
		{
		case SS_CURRENT:
			m_fTimeCounts = m_fDuration;
			break;
		case SS_INIT:
			{
				m_fTimeCounts = m_fDuration;
				for each (AnimateProperties::value_type elem in m_Properties) 
				{
					AnimateProperty *prop = elem.second;
					prop->fCurrentValue = prop->fStartValue;
				}
			}
			break;
		case SS_FINAL:
			{
				m_fTimeCounts = m_fDuration;
				for each (AnimateProperties::value_type elem in m_Properties) 
				{
					AnimateProperty *prop = elem.second;
					prop->fCurrentValue = prop->fEndValue;
				}
			}
			break;
		case SS_BACKTOINIT:
			m_bFinished = false;
			m_nDirection = -1;
			break;
		default:
			break;
		}
		if(m_pManager)
		{
			m_pManager->DispatchEvent(this,ON_STEP);
			m_pManager->DispatchEvent(this,ON_COMPLETE);
		}
		return m_bFinished;
	}

	void CTimeLineTween::SetTweenInfo(float fDuration, short sTransition ,short sEquation )
	{
		m_fDuration = fDuration;
		m_sEquation = sEquation;
		switch(sTransition)
		{
		case LINEAR:
			m_TweenFunc = &fLinear;
			break;
		case SINE:
			m_TweenFunc = &fSine;
			break;
		case QUINT:
			m_TweenFunc = &fQuint;
			break;
		case QUART:
			m_TweenFunc = &fQuart;
			break;
		case QUAD:
			m_TweenFunc = &fQuad;
			break;
		case EXPO:
			m_TweenFunc = &fExpo;
			break;
		case ELASTIC:
			m_TweenFunc = &fElastic;
			break;
		case CUBIC:
			m_TweenFunc = &fCubic;
			break;
		case CIRC:
			m_TweenFunc = &fCirc;
			break;
		case BOUNCE:
			m_TweenFunc = &fBounce;
			break;
		case BACK:
			m_TweenFunc = &fBack;
			break;
		default:
			break;
		}
	}

	void CTimeLineTween::RunEquation(AnimateProperty* prop)
	{
		if(!prop) return;
		float fRes = 0;
		switch (m_sEquation)
		{
		case EASE_IN:
			fRes = m_TweenFunc->easeIn(m_fTimeCounts,prop->fStartValue,prop->fEndValue-prop->fStartValue,m_fDuration);
			break;
		case EASE_OUT:
			fRes = m_TweenFunc->easeOut(m_fTimeCounts,prop->fStartValue,prop->fEndValue-prop->fStartValue,m_fDuration);
			break;
		case EASE_IN_OUT:
			fRes = m_TweenFunc->easeInOut(m_fTimeCounts,prop->fStartValue,prop->fEndValue-prop->fStartValue,m_fDuration);
			break;
		default:
			break;
		}
		prop->fCurrentValue = fRes;
	}

	


}

