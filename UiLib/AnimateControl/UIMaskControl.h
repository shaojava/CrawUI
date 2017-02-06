#pragma once

namespace UiLib
{
	class UILIB_API CMaskControlUI : public CControlUI
	{
	public:
		CMaskControlUI(void);
		~CMaskControlUI(void);

		LPCTSTR GetClass() const;

		void SetMaskTarget(CControlUI *pControl);

		bool OnAnimateStart(TimeEvent *pEvent);
		bool OnAnimateStep(TimeEvent *pEvent);
		bool OnAnimateComplete(TimeEvent *pEvent);
		bool OnAnimateEnd(TimeEvent *pEvent);

		void AnimateTo(RECT rc,DWORD dwMills = 500);

		void Event(TEventUI& event);
		void DoInit();

	protected:
		float m_fStart,m_fEnd,m_fStartY,m_fEndY;
		CControlUI *m_pMaskTargetControl;
		CTimeLineTween *m_pTimeLine;
	};
}


