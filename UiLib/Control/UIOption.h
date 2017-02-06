#ifndef __UIOPTION_H__
#define __UIOPTION_H__

#pragma once

namespace UiLib
{
	class UILIB_API COptionUI : public CButtonUI
	{
	public:
		COptionUI();
		~COptionUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);

		bool Activate();
		void SetEnabled(bool bEnable = true);

		LPCTSTR GetSelectedImage();
		void SetSelectedImage(LPCTSTR pStrImage);

		LPCTSTR GetSelectedHotImage();
		void SetSelectedHotImage(LPCTSTR pStrImage);

		void SetSelectedTextColor(DWORD dwTextColor);
		DWORD GetSelectedTextColor();

		void SetSelectedBkColor(DWORD dwBkColor);
		DWORD GetSelectBkColor();

		void SetSelectedFont(int iFont);
		int GetSeletedFont();

		LPCTSTR GetForeImage();
		void SetForeImage(LPCTSTR pStrImage);

		LPCTSTR GetGroup() const;
		void SetGroup(LPCTSTR pStrGroupName = NULL);
		bool IsSelected() const;
		virtual void Selected(bool bSelected);

		void SetSelect(bool bSelect);
		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintStatusImage();
		void PaintText();

	protected:
		bool			m_bSelected;
		CDuiString		m_sGroupName;

		int		m_iSeletedFont;
		DWORD			m_dwSelectedBkColor;
		DWORD			m_dwSelectedTextColor;

		CDuiImage		m_sSelectedImage;
		CDuiImage		m_sSelectedHotImage;
		CDuiImage		m_sForeImage;
	};

} // namespace UiLib

#endif // __UIOPTION_H__