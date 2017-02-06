#include "StdAfx.h"
#include "TimeLineBezier.h"

namespace UiLib
{

	CTimeLineBezier::CTimeLineBezier()
	{
		m_BezierFactor.empty();
	}

	CTimeLineBezier::~CTimeLineBezier()
	{

	}

	void CTimeLineBezier::SetBezierFactor( float fp1,float fp2,float fp3,float fp4 )
	{
		m_BezierFactor.clear();
		m_BezierFactor.push_back(fp1);
		m_BezierFactor.push_back(fp2);
		m_BezierFactor.push_back(fp3);
		m_BezierFactor.push_back(fp4);
	}

	void CTimeLineBezier::RunEquation( AnimateProperty* prop )
	{
		if(!prop) return;
		float fRes = 0;
		float cPercent = solveCubicBezierFunction(m_BezierFactor[0],m_BezierFactor[1],m_BezierFactor[2],m_BezierFactor[3],m_fTimeCounts/m_fDuration,m_fDuration);
		fRes = prop->fStartValue+cPercent*(prop->fEndValue-prop->fStartValue);
		prop->fCurrentValue = fRes;
	}

}

