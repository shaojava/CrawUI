#include "StdAfx.h"
#include "UIAnimateManager.h"

namespace UiLib
{

	CUIAnimateManager::CUIAnimateManager(CPaintManagerUI *pManager)
	{
		m_hTimer = NULL;
		m_nFps = 60;
		m_pManager = pManager;
	}


	CUIAnimateManager::~CUIAnimateManager(void)
	{
		Destroy();
	}

	void CUIAnimateManager::OnWaitForHandleObjectCallback( HANDLE, LPARAM )
	{
		StoryBoardsIt iter = m_Storyboards.begin();

		for(; iter != m_Storyboards.end();)
		{
			CStoryBoard* pStoryboard = *iter;
			pStoryboard->Step(GetTickCount());
			if (pStoryboard->IsFinished())
			{
				iter = m_Storyboards.erase(iter);
				if(!m_Storyboards.size())
				{
					KillTimer();
					return;
				}
			}
			else
				iter++;
		}
		
	}

	CStoryBoard* CUIAnimateManager::CreateStoryboard()
	{
		CStoryBoard *pStoryBoard = new CStoryBoard();
		if(pStoryBoard)
		{
			pStoryBoard->SetManager(this);
			return pStoryBoard;
		}
		return NULL;
	}

	bool CUIAnimateManager::AddStoryboard(CStoryBoard* pStoryBoard)
	{
		if (!pStoryBoard)
			return false;
		StoryBoardsIt itRes = find(m_Storyboards.begin(),m_Storyboards.end(),pStoryBoard);
		if(itRes == m_Storyboards.end())
		{
			if(!pStoryBoard->GetManager())
				pStoryBoard->SetManager(this);
			m_Storyboards.push_back(pStoryBoard);
			if (NULL == m_hTimer)
				StartAnimate();
			return true;
		}
		return false;
	}

	bool CUIAnimateManager::AddStoryboardBlock( CStoryBoard* pStoryBoard )
	{
		if (!pStoryBoard)
			return false;

		int nSleep = 1000/m_nFps;

		while (1)
		{
			pStoryBoard->Step(GetTickCount());
			if(pStoryBoard->IsFinished())
				return true;
			else
				::Sleep(nSleep);    
		}
	}

	void CUIAnimateManager::RemoveStoryboard( CStoryBoard* pStoryBoard )
	{
		if (!pStoryBoard)
		{
			Destroy();
			return;
		}
		pStoryBoard->Stop();
		m_Storyboards.erase(find(m_Storyboards.begin(),m_Storyboards.end(),pStoryBoard));
		SAFE_DELETE(pStoryBoard);
		if (0 == m_Storyboards.size())
			KillTimer();
	}

	void CUIAnimateManager::Destroy()
	{
		EndAnimate();
		m_Storyboards.clear();
	}

	void CUIAnimateManager::StartAnimate()
	{
		KillTimer();
		SetTimer();
	}

	void CUIAnimateManager::EndAnimate()
	{
		KillTimer();
	}

	void CUIAnimateManager::SetTimer()
	{
		if (m_hTimer)
			return;

		if (0 == m_nFps)
			m_nFps = 10;

		int nPeriod = 1000/m_nFps;

		m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);

		CPaintManagerUI::m_WaitForHandlesMgr.AddHandle(m_hTimer, static_cast<IWaitForHandleCallback*>(this), 0);
		LARGE_INTEGER liDueTime;
		  // 第一次响应延迟时间。负值表示一个相对的时间，代表以100纳秒为单位的相对时间，（如从现在起的5ms，则设置为-50000）;
		liDueTime.QuadPart = -1000*10*nPeriod; 
		SetWaitableTimer(m_hTimer, &liDueTime, nPeriod, NULL, NULL, 0);
		
	}

	void CUIAnimateManager::KillTimer()
	{
		if (m_hTimer)
		{
			CPaintManagerUI::m_WaitForHandlesMgr.RemoveHandle(m_hTimer);
			CloseHandle(m_hTimer);
			m_hTimer = NULL;
		}
	}

	

}
