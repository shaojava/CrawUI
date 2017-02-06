#pragma once

namespace UiLib
{

	class UILIB_API CWndContainerUI : public CControlUI
	{
	public:
		CWndContainerUI(void);
		virtual ~CWndContainerUI(void);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void Event(TEventUI& event);
		BOOL Attach(CWindowWnd *pWindow);
		BOOL Attach(HWND hWndNew);
		HWND Detach();
		virtual void SetPos(RECT rc);
		virtual void SetInternVisible(bool bVisible = true);
		virtual void SetVisible(bool bVisible = true);
		HWND GetHWND();
	protected:
		CWindowWnd *m_pWindow;
		HWND m_hWndAttached;
	};

}