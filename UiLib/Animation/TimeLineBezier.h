#pragma once
#include "TimeLineTween.h"

namespace UiLib
{
	typedef vector<float> BezierFacotrs;
	class UILIB_API CTimeLineBezier: public CTimeLineTween
	{
	public:
		CTimeLineBezier();
		~CTimeLineBezier();

		void SetBezierFactor(float fp1,float fp2,float fp3,float fp4);
	protected:
		void RunEquation(AnimateProperty* prop);
	protected:
		BezierFacotrs m_BezierFactor;
	};
	
}