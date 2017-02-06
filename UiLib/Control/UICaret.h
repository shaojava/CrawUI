#pragma once

namespace UiLib
{
	class CCaretUI : public CControlUI
 	{
	public:
		CCaretUI(CPaintManagerUI *pManager);
		~CCaretUI(void);

		BOOL  Create(HBITMAP hbmp, int nWidth, int nHeight);
		void  Destroy(bool bRedraw);
		void  SetPos(int x, int y, bool bRedraw);
		void  Show(bool bRedraw);
		void  Hide(bool bRedraw);
		void  DoPostPaint(const RECT& rcPaint);
		void  DrawCaret();
	protected:
		void  Invalidate();
		void  CreateTimer();
		void  DestroyTimer();
		void  OnTimer(IDuiTimer* pTimer,void* param);
	protected:
		CPaintManagerUI *m_pPaintManager;
		IDuiTimer* m_pTimer;

		HWND   m_hWnd;
		HBITMAP  m_hBitmap;
		POINT  m_ptLast;
		SIZE   m_sizeCaret;
		RECT  m_rcDraw;
		UINT  m_uTimerId;
		bool   m_bOn;
		bool   m_bVisible;
	};

}
