#pragma once
#include "TimeLine.h"

#define MAX_DELTA_TIME_SEC 0.064f
#define SOLVER_TIMESTEP_SEC 0.001f

namespace UiLib
{

	struct AnimateSpringProperty : public AnimateProperty
	{
		float fCurrentVelocity;
		float fPrevusValue;
		float fPrevusVelocity;
		float fTempValue;
		float fTempVelocity;
		AnimateSpringProperty()
		{
			fCurrentVelocity = 0;
			fPrevusValue = 0;
			fPrevusVelocity = 0;
			fTempValue = 0;
			fTempVelocity = 0;
		}
	};

	class UILIB_API CTimeLineSpring: public CTimeLine
	{
	public:
		CTimeLineSpring();
		~CTimeLineSpring();
		void AddProperty(CDuiString szHashKey,float fStartValue,float fEndValue);
		void SetSpringInfo(float fTension,float fFriction);
		void ReSetTimeLine();
		bool Step(long lDeltaMillis);
		bool Stop(STOPSTATE ss = SS_CURRENT);
	protected:
		bool IsAtRest();
		void interpolate(AnimateSpringProperty *pProperty,float alpha);
		void DoSpring(AnimateSpringProperty *pProperty);
	protected:

		float m_fTension;
		float m_fFriction;

		float m_fOffsetThreshold;
	};
	
}