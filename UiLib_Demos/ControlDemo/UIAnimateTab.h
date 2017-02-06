#pragma once
#include "../../Common/Include/UiLib/UIlib.h"

class CAnimateTabUI : public CControlUI
{
public:
	CAnimateTabUI(void);
	~CAnimateTabUI(void);

	void DoEvent(TEventUI& event);
	bool OnAnimateTimerTick(TimeEvent *pEvent);
	void Pause(bool bPause = true);
protected:
	float m_fPosX;
	CTimeLineSpring *m_pMoveTL;
};

