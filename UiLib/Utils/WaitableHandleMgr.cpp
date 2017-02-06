#include "StdAfx.h"
#include "WaitableHandleMgr.h"

namespace UiLib
{

	WaitForHandle::WaitForHandle(HANDLE h, IWaitForHandleCallback* pCB, LPARAM l)
	{
		m_hHandle = h;
		m_pCallback = pCB;
		m_lParam = l;
	}


	/////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////

	CWaitableHandleMgr::CWaitableHandleMgr(void)
	{
		m_pHandles = NULL;
		m_nHandleCount = 0;
	}


	CWaitableHandleMgr::~CWaitableHandleMgr(void)
	{
		_MyIter	iter = m_list.begin();
		_MyIter	iterEnd = m_list.end();
		for (; iter != iterEnd; iter++)
			delete (*iter);
		m_list.clear();
		m_nHandleCount = 0;

		SAFE_DELETEARRY(m_pHandles);
		m_nHandleCount = 0;
	}

	WaitForHandle* CWaitableHandleMgr::FindHandle( HANDLE h )
	{
		if (NULL == h)
			return NULL;

		_MyIter	iter = m_list.begin();
		_MyIter	iterEnd = m_list.end();

		for (;iter != iterEnd; iter++)
			if (h == (*iter)->GetHandle())
				return *iter;

		return NULL;
	}

	_MyIter CWaitableHandleMgr::FindHandleIter( HANDLE h )
	{
		if (NULL == h)
			return m_list.end();

		_MyIter	iter = m_list.begin();
		_MyIter	iterEnd = m_list.end();

		for (;iter != iterEnd; iter++)
			if (h == (*iter)->GetHandle())
				return iter;

		return m_list.end();
	}

	bool CWaitableHandleMgr::AddHandle( HANDLE h, IWaitForHandleCallback* pCB, LPARAM l )
	{
		if (NULL == h || NULL == pCB)
			return false;

		if (FindHandle(h))
			return false;

		WaitForHandle* p = new WaitForHandle(h, pCB, l);
		m_list.push_back(p);

		UpdateHandleArray();
		return true;
	}

	bool CWaitableHandleMgr::RemoveHandle( HANDLE h )
	{
		if (NULL == h)
			return false;

		_MyIter iter = FindHandleIter(h);
		if (iter == m_list.end())
			return false;

		WaitForHandle* p = *iter;
		SAFE_DELETE(p);
		m_list.erase(iter);

		UpdateHandleArray();
		return true;
	}

	bool CWaitableHandleMgr::UpdateHandleArray()
	{
		SAFE_DELETEARRY(m_pHandles);
		m_nHandleCount = (int)m_list.size();

		if (0 == m_nHandleCount)
			return true;

		m_pHandles = new HANDLE[m_nHandleCount];
		_MyIter	iter = m_list.begin();
		_MyIter	iterEnd = m_list.end();

		for (int i = 0; iter != iterEnd; iter++, i++)
		{
			m_pHandles [i] = (*iter)->GetHandle();
		}
		return true;
	}

	void CWaitableHandleMgr::Do( HANDLE h )
	{
		WaitForHandle* pWaitForHandle = this->FindHandle(h);
		if (NULL == pWaitForHandle)
			return;
		pWaitForHandle->GetCB()->OnWaitForHandleObjectCallback(h, pWaitForHandle->GetLParam());
	}

}