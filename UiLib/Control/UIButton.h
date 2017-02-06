#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#pragma once

namespace UiLib
{
	class UILIB_API CButtonUI : public CLabelUI
	{
	public:
		CButtonUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		bool Activate();
		void SetEnabled(bool bEnable = true);
		void DoEvent(TEventUI& event);
		bool OnIDOK(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);

		LPCTSTR GetNormalImage();
		virtual void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage();
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetPushedImage();
		void SetPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage();
		void SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetDisabledImage();
		void SetDisabledImage(LPCTSTR pStrImage);

		void SetHotBkColor(DWORD dwColor);
		DWORD GetHotBkColor() const;
		void SetCalendarValDest(LPCTSTR pstrValue);
		LPCTSTR GetCalendarValDest();
		void SetCalendarName(LPCTSTR pStrCalendarName);
		LPCTSTR GetCalendarName();
		void SetCalendarStyle(LPCTSTR pStrCalendarStyle);
		LPCTSTR GetCalendarStyle();
		void SetCalendarProfile(LPCTSTR pStrCalendarProfile);
		LPCTSTR GetCalendarProfile();

		void SetHotTextColor(DWORD dwColor);
		DWORD GetHotTextColor() const;
		LPCTSTR GetForeImage();
		void SetForeImage(LPCTSTR pStrImage);
		LPCTSTR GetHotForeImage();
		void SetHotForeImage(LPCTSTR pStrImage);
		void SetPushedTextColor(DWORD dwColor);
		DWORD GetPushedTextColor() const;
		void SetFocusedTextColor(DWORD dwColor);
		DWORD GetFocusedTextColor() const;
		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		unUserData GetAttribute( LPCTSTR pstrName );
		void OnPropertyActionTimer( IDuiTimer* pTimer,TProperty* pTProperty );

		void PaintText();
		void PaintStatusImage();

		void BindTabIndex(int _BindTabIndex);
		void BindTabLayoutName(LPCTSTR _TabLayoutName);
		void BindTriggerTabSel(int _SetSelectIndex = -1);
		void RemoveBindTabIndex();
		int	 GetBindTabLayoutIndex();
		CDuiString GetBindTabLayoutName();
		void SetStateDown();
		void SetStateUp();
		bool IsButtonPusd();
	protected:
		bool m_bHandState;
		UINT m_uButtonState;

		int m_nDashLineWidth;
		int m_nDashRectArc;
		int m_nDashRectShrink;
		
		DWORD m_dwHotBkColor;
		DWORD m_dwHotTextColor;
		DWORD m_dwPushedTextColor;
		DWORD m_dwFocusedTextColor;
		DWORD m_dwFocusedDashColor;
		
		CDuiImage	m_sNormalImage;
		CDuiImage	m_sHotImage;
		CDuiImage	m_sHotForeImage;
		CDuiImage	m_sPushedImage;
		CDuiImage	m_sPushedForeImage;
		CDuiImage	m_sFocusedImage;
		CDuiImage	m_sDisabledImage;

		CDuiString	m_sSalendarValDest;
		CDuiString	m_sCalendarName;
		CDuiString	m_sCalendarStyle;
		CDuiString	m_sCalendarProfile;


		int			m_iBindTabIndex;
		CDuiString	m_sBindTabLayoutName;
	};

}	// namespace UiLib

#endif // __UIBUTTON_H__