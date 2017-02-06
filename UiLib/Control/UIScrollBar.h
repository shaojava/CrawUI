#ifndef __UISCROLLBAR_H__
#define __UISCROLLBAR_H__

#pragma once

namespace UiLib
{
	class UILIB_API CScrollBarUI : public CControlUI
	{
	public:
		CScrollBarUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		CContainerUI* GetOwner() const;
		void SetOwner(CContainerUI* pOwner);

		void SetVisible(bool bVisible = true);
		void SetEnabled(bool bEnable = true);
		void SetFocus();

		void SetFlexRange(int nFlexRange){m_nFlexRange = nFlexRange;}
		int GetFlexRange(){return m_nFlexRange;}

		bool IsHorizontal();
		void SetHorizontal(bool bHorizontal = true);
		int GetScrollRange() const;
		bool SetScrollRange(int nRange);
		void SetScrollRangeFlex(int nRange);
		int GetScrollPos() const;
		void SetScrollPos(int nPos,bool bFlex = false);
		void SetScrollPosDirect(int nPos){m_nScrollPos = nPos;}
		int GetLineSize() const;
		void SetLineSize(int nSize);

		bool GetShowButton1();
		void SetShowButton1(bool bShow);
		LPCTSTR GetButton1NormalImage();
		void SetButton1NormalImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1HotImage();
		void SetButton1HotImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1PushedImage();
		void SetButton1PushedImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1DisabledImage();
		void SetButton1DisabledImage(LPCTSTR pStrImage);

		bool GetShowButton2();
		void SetShowButton2(bool bShow);
		LPCTSTR GetButton2NormalImage();
		void SetButton2NormalImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2HotImage();
		void SetButton2HotImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2PushedImage();
		void SetButton2PushedImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2DisabledImage();
		void SetButton2DisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetThumbNormalImage();
		void SetThumbNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbHotImage();
		void SetThumbHotImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbPushedImage();
		void SetThumbPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbDisabledImage();
		void SetThumbDisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetRailNormalImage();
		void SetRailNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetRailHotImage();
		void SetRailHotImage(LPCTSTR pStrImage);
		LPCTSTR GetRailPushedImage();
		void SetRailPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetRailDisabledImage();
		void SetRailDisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetBkNormalImage();
		void SetBkNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetBkHotImage();
		void SetBkHotImage(LPCTSTR pStrImage);
		LPCTSTR GetBkPushedImage();
		void SetBkPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetBkDisabledImage();
		void SetBkDisabledImage(LPCTSTR pStrImage);

		void ShowTakePlace(bool bTake);
		bool IsShowTakePlace();
		bool IsTakePlace();

		void SetPos(RECT rc);
		void DoEvent(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void DoPaint(const RECT& rcPaint);

		void PaintBk();
		void PaintButton1();
		void PaintButton2();
		void PaintThumb();
		void PaintRail();

	protected:

		enum
		{ 
			DEFAULT_SCROLLBAR_SIZE = 16,
			DEFAULT_TIMERID = 10,
		};

		bool m_bHorizontal;
		int m_nRange;
		int m_nScrollPos;
		int m_nLineSize;
		CContainerUI* m_pOwner;
		POINT ptLastMouse;
		int m_nLastScrollPos;
		int m_nLastScrollOffset;
		int m_nScrollRepeatDelay;

		CDuiImage m_sBkNormalImage;
		CDuiImage m_sBkHotImage;
		CDuiImage m_sBkPushedImage;
		CDuiImage m_sBkDisabledImage;

		bool m_bTakePlace;
		bool m_bShowButton1;
		RECT m_rcButton1;
		UINT m_uButton1State;
		CDuiImage m_sButton1NormalImage;
		CDuiImage m_sButton1HotImage;
		CDuiImage m_sButton1PushedImage;
		CDuiImage m_sButton1DisabledImage;

		bool m_bShowButton2;
		RECT m_rcButton2;
		UINT m_uButton2State;
		CDuiImage m_sButton2NormalImage;
		CDuiImage m_sButton2HotImage;
		CDuiImage m_sButton2PushedImage;
		CDuiImage m_sButton2DisabledImage;

		RECT m_rcThumb;
		UINT m_uThumbState;
		CDuiImage m_sThumbNormalImage;
		CDuiImage m_sThumbHotImage;
		CDuiImage m_sThumbPushedImage;
		CDuiImage m_sThumbDisabledImage;

		CDuiImage m_sRailNormalImage;
		CDuiImage m_sRailHotImage;
		CDuiImage m_sRailPushedImage;
		CDuiImage m_sRailDisabledImage;

		RECT m_rcImageModify;

		int m_nFlexRange;
		int m_nFlexedRange;
	};
}

#endif // __UISCROLLBAR_H__