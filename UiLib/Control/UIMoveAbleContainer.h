#pragma once

namespace UiLib
{

	class UILIB_API CMoveAbleContainerUI : public CContainerUI
	{
	public:
		CMoveAbleContainerUI(void);
		virtual ~CMoveAbleContainerUI(void);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void DoEvent(TEventUI& event);

	protected:
		POINT m_ptBtnDown;
		BOOL m_bLBtnDown;
		BOOL m_bMoveing;
	};

}
