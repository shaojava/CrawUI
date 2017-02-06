#pragma once
#include "TimeLine.h"

namespace UiLib
{
	class CUIAnimateManager;
	class CStoryBoard;

	enum KeyFrameState
	{
		ON_START, 
		ON_STEP, 
		ON_COMPLETE,
		ON_END
	};

	struct TimeEvent
	{
		CTimeLine *pTimeLine;
		KeyFrameState uState;
		CStoryBoard *pStoryBoard;
		TimeEvent(){pTimeLine = NULL;pStoryBoard = NULL;}
	};

	typedef bool (*FunTimeEvent)(TimeEvent* pParam);

	class UILIB_API CAnimateDelegateBase
	{
	public:
		CAnimateDelegateBase(void* pObject, FunTimeEvent pFn){m_pObject = pObject;m_pFunTemplate = pFn;}
		CAnimateDelegateBase(const CAnimateDelegateBase& rhs){m_pObject=rhs.m_pObject;m_pFunTemplate=rhs.m_pFunTemplate;}
		virtual ~CAnimateDelegateBase(){}

		bool operator() (TimeEvent* param){return Invoke(param);}
		virtual bool Invoke(TimeEvent* param){return true;}
		virtual CAnimateDelegateBase* Copy() const {return NULL;}

		void *GetObj(){return m_pObject;}
		bool Equals(const CAnimateDelegateBase& rhs) const
		{
			return m_pObject==rhs.m_pObject&&m_pFunTemplate==rhs.m_pFunTemplate;
		}
	public:
		FunTimeEvent m_pFunTemplate;
		void* m_pObject;
	};

	template <class O,class T>
	class UILIB_API CAnimateDelegate : public CAnimateDelegateBase
	{
	public:
		typedef bool (T::*CMFunTimeEvent)(TimeEvent *pParam);
	public:
		CAnimateDelegate(O* pObject, CMFunTimeEvent pFn):CAnimateDelegateBase(pObject,*(FunTimeEvent*)&pFn),m_pCMFunTemplate(pFn){}
		CAnimateDelegate(const CAnimateDelegate& rhs):CAnimateDelegateBase(rhs){m_pCMFunTemplate = rhs.m_pCMFunTemplate;}
		virtual ~CAnimateDelegate(){}
		virtual CAnimateDelegateBase* Copy() const {return new CAnimateDelegate(*this);}
	protected:
		bool Invoke(TimeEvent* pParam)
		{
			O* pObject = (O*) GetObj();
			if(pObject&&m_pCMFunTemplate)
				return (pObject->*m_pCMFunTemplate)(pParam);
			return true;
		}
	private:
		CMFunTimeEvent m_pCMFunTemplate;
	};

	template <class O,class T>
	CAnimateDelegate<O,T> MakeAnimateDelegate(O* pObject, bool (T::* pFn)(TimeEvent* pParam))
	{
		return CAnimateDelegate<O,T>(pObject,pFn);
	}

	class UILIB_API CAnimateSource
	{
	public:
		CAnimateSource();
		~CAnimateSource();
		operator bool();
		void operator+= (const CAnimateDelegateBase& d);
		void operator-= (const CAnimateDelegateBase& d);
		bool operator() (TimeEvent* param);
	protected:
		TStdPtrArray<CAnimateDelegateBase*> m_aDelegates;
	};

	enum StoryBoardState
	{
		SB_IDLING = 0,
		SB_RUNNING = 1,
		SB_PAUSING = 2,
		SB_FINSHED = 3
	};

	class UILIB_API CStoryBoard 
	{
		typedef vector<CTimeLine *> TimeLines;
		typedef vector<CTimeLine *>::iterator TimeLinesIt;
	public:
		CStoryBoard();
		~CStoryBoard();

		int GetTimeLineSize();

		void SetManager(CUIAnimateManager *pManager){m_pManager = pManager;}
		CUIAnimateManager *GetManager(){return m_pManager;}

		void AddTimeLine(CTimeLine* param);
		void UpdateTimeLine(CTimeLine* param);
		void RemoveTimeLine(CTimeLine* param);
		void ClearTimeLine();

		void Begin();
		void Step(long currentMillis) ;
		void Stop(CTimeLine *param = NULL,STOPSTATE ss = SS_CURRENT);
		void End(STOPSTATE ss);
		void Pause();
		void Resume();
		bool IsPause();
		bool IsFinished(CTimeLine *param = NULL);
		StoryBoardState GetStoryBoardState(){return m_StoryBoradState;}
		void DispatchEvent(CTimeLine *param, short eventType);

	protected :
		StoryBoardState m_StoryBoradState;
	
		long lastTime;
		TimeLines m_TimeLines;
		TimeLinesIt m_TimeLinesIt;

		INT_PTR m_nTimeLineCounts;

		bool m_bPause;

		CUIAnimateManager *m_pManager;
	public:
		CAnimateSource OnAnimateTick;
	};

	
}