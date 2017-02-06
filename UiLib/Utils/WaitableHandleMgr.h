#pragma once
#include<list>

namespace UiLib
{
	interface IWaitForHandleCallback
	{
		virtual void OnWaitForHandleObjectCallback(HANDLE, LPARAM) = 0;
	};

	/////////////////////////////////////////////////////////////////////////////////////////
	//////////////////

	class UILIB_API WaitForHandle
	{
	public:
		WaitForHandle(HANDLE h, IWaitForHandleCallback* pCB, LPARAM l);
		HANDLE  GetHandle() { return m_hHandle; }
		IWaitForHandleCallback* GetCB() { return m_pCallback; }
		LPARAM  GetLParam() { return m_lParam; }

	protected:
		HANDLE  m_hHandle;
		IWaitForHandleCallback*  m_pCallback;
		LPARAM  m_lParam;
	};
	/////////////////////////////////////////////////////////////////////////////////////////
	//////////////////
	using namespace std;
	typedef list<WaitForHandle*> _MyList;
	typedef list<WaitForHandle*>::iterator _MyIter;

	class UILIB_API CWaitableHandleMgr
	{
	public:
		CWaitableHandleMgr(void);
		~CWaitableHandleMgr(void);

		WaitForHandle* FindHandle(HANDLE h);
		_MyIter FindHandleIter(HANDLE h);
		bool  AddHandle(HANDLE h, IWaitForHandleCallback* pCB, LPARAM l);
		bool  RemoveHandle(HANDLE h);
		bool  UpdateHandleArray();
		void  Do(HANDLE h);

		HANDLE*  GetHandles() { return m_pHandles; }

	protected:
		list<WaitForHandle*>  m_list;
	public:
		HANDLE*  m_pHandles;
		DWORD    m_nHandleCount;
	};

}
