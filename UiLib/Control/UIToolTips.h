#pragma once
#include "../Utils/IWindowBase.h"

namespace UiLib
{
	class CToolTipsUI;

	class UILIB_API CToolTipsWnd : public IWindowBase
	{
	public:
		CToolTipsWnd(HWND hParent = NULL);
		~CToolTipsWnd();
		static CToolTipsWnd *CreateToolTipsWnd(HWND hWnd);
		void SetSkinFile(CDuiString szSkinFilePath);
		CDuiString GetSkinFile();
		CDuiString GetSkinFolder();
		LPCTSTR GetWindowClassName() const ;
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

		void Init();
		void Notify(TNotifyUI& msg);
		void OnFinalMessage(HWND hWnd);

		void Attach(CControlUI *pOwner,SIZE szWndSize,POINT ptMouse);
		void Detach();

		void SetTipString(CDuiString szTipString);
	protected:
		CDuiString m_szSkinFilePath;
		POINT m_ptMouse;
		SIZE m_WndSize;
		CControlUI *m_pOwner;
		CToolTipsUI *m_pToolTips;
		HWND m_hParent;
	};

	////////////////////////////////////////////////////////////////////////////////
	//////////////
	class UILIB_API CToolTipsUI : public CContainerUI
	{
	public:
		CToolTipsUI(void);
		~CToolTipsUI(void);
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetTipString(CDuiString szString);
		void SetFont(int nIndex);
	protected:
		CLabelUI *m_pTextLable;
		CLabelUI *m_pArrowTag;
	};

}

