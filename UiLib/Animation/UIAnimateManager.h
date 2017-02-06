#pragma once
#include "../Utils/WaitableHandleMgr.h"
#include "StoryBoard.h"

namespace UiLib
{
	class UILIB_API CUIAnimateManager : IWaitForHandleCallback
	{
		typedef vector<CStoryBoard*>  StoryBoards;
		typedef vector<CStoryBoard*>::iterator StoryBoardsIt;
	public:
		CUIAnimateManager(CPaintManagerUI *pManager);
		~CUIAnimateManager(void);

		int GetFps() {return m_nFps;}
		void SetFps(int nFps){m_nFps = nFps;}


		CStoryBoard* CreateStoryboard();

		bool AddStoryboard(CStoryBoard* pStoryBoard);
		bool AddStoryboardBlock(CStoryBoard* pStoryBoard);
		void RemoveStoryboard(CStoryBoard* pStoryBoard);

		void OnWaitForHandleObjectCallback(HANDLE, LPARAM);

		void    SetTimer();
	protected:
		void    Destroy();

		void    StartAnimate();
		void    EndAnimate();

		void    KillTimer();
	protected:
		CPaintManagerUI *m_pManager;
		HANDLE m_hTimer;
		int    m_nFps;

		StoryBoards  m_Storyboards;
	};

}

