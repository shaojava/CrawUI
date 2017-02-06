#pragma once
#include "TimeLine.h"

namespace UiLib
{
	class UILIB_API CTimeLineDecay: public CTimeLine
	{
	public:
		CTimeLineDecay();
		~CTimeLineDecay();
	public:
		void SetDecayInfo(float fDecaleeration,float fVelocityThreshold,float fOffsetThreshold);
		void SetVelocity(float fVelocity);
		void UpdateVelocity(float fVelocity);
		void ReSetTimeLine();
		bool Step(long lDeltaMillis);
		bool Stop(STOPSTATE ss = SS_CURRENT);
		float GetPlanDuration();
		float GetPlanDestination();
	protected:
		void DecayPosition(float *x,float *v,double dTime,float fDeceleration);
		bool IsAtRest(float fCurrvalue = -1);
	private:
		float m_fVelocity;
		float m_fCurrentValue;
		float m_fDecayDeceleration;
		float m_fVelocityThreshold;
		float m_fOffsetThreshold;
	};
	
}