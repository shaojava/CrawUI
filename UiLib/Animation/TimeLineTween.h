#pragma once
#include "TimeLine.h"

namespace UiLib
{
	class UILIB_API CTimeLineTween: public CTimeLine
	{
	public:
		CTimeLineTween();
		~CTimeLineTween();

		void SetTweenInfo(float fDuration,short sTransition = EXPO,short sEquation = EASE_OUT);
		bool Step(long lDeltaMillis);
		bool DealRepeat();
		bool Stop(STOPSTATE ss = SS_CURRENT);
	protected:
		virtual void RunEquation(AnimateProperty* prop);
	protected:
		Easing *m_TweenFunc;
		short m_sEquation;
		float m_fDuration;
	};
	
}