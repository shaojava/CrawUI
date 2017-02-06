#pragma once
#include "Tween.h"

namespace UiLib
{
	class CStoryBoard;
	struct AnimateProperty 
	{
		float fCurrentValue;
		float fStartValue;
		float fEndValue;
		AnimateProperty()
		{
			fCurrentValue = 0;
			fStartValue = 0;
			fEndValue = 0;
		}
	};

	enum STOPSTATE
	{
		SS_CURRENT = 0,
		SS_INIT = 1,
		SS_FINAL = 2,
		SS_BACKTOINIT = 3
	};

	typedef hash_map<CDuiString,AnimateProperty*,DuiString_Compare> AnimateProperties;
	typedef hash_map<CDuiString,AnimateProperty*,DuiString_Compare>::iterator AnimatePropertiesIt;

	class UILIB_API CTimeLine
	{
	public:
		CTimeLine();
		virtual ~CTimeLine();
	public:
		void SetManager(CStoryBoard *pManager){m_pManager = pManager;}
		CStoryBoard *GetManager(){return m_pManager;}

		
		void SetDelayTime(float fDelayTime);
		void SetRepeatWithReverse(int nRepeateTimes,bool bReverse = false);
		void CleanProperties();

		virtual void ReSetTimeLine();
		virtual bool Step(long lDeltaMillis);
		virtual bool DealRepeat();
		virtual bool Stop(STOPSTATE ss = SS_CURRENT);
		virtual void AddProperty(CDuiString szHashKey,float fStartValue,float fEndValue);
		virtual void RemoveProperty(CDuiString szHashKey);
		virtual void UpdateProperty(CDuiString szHashKey,float fCurrentValue);
		virtual void UpdateProperty(CDuiString szHashKey,float fStartValue,float fEndValue);
		virtual void UpdateProperty(CDuiString szHashKey,float fCurrentValue,float fStartValue,float fEndValue);
		void Pause();
		void Resume();
		bool IsPause();
		bool GetProperty(CDuiString szHashKey,AnimateProperty **Property);
		bool GetPropertyCurrentValue(CDuiString szHashKey,float &fCurValue);
	protected:
		CStoryBoard *m_pManager;

		AnimateProperties m_Properties;

		float m_fDelayMilliSecs;
		float m_fTimeCounts;

		int m_nRepeatCount;
		int m_nDirection;

		bool m_bPause;
		bool m_bRepeatWithReverse;
		bool m_bStarted;
		bool m_bFinished;
		bool m_bDelayFinished;
	};

}