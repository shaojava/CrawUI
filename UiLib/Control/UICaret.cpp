#include "StdAfx.h"
#include "UICaret.h"

namespace UiLib
{

	CCaretUI::CCaretUI(CPaintManagerUI *pManager)
	{
		m_pPaintManager = pManager;
		m_pPaintManager->AddPostPaint(this);
		m_pTimer = NULL;
		m_hBitmap = NULL;
		m_ptLast.x = m_ptLast.y = 0;
	}


	CCaretUI::~CCaretUI(void)
	{
		m_pPaintManager->RemovePostPaint(this);
		Destroy(true);
		SAFE_DELETE(m_pTimer);
	}

	BOOL CCaretUI::Create(HBITMAP hbmp, int nWidth, int nHeight )
	{
		Destroy(false);

		m_sizeCaret.cx = nWidth;
		m_sizeCaret.cy = nHeight;
		m_hBitmap = hbmp;

		if (m_hBitmap > (HBITMAP)1)
		{
			BITMAP bm = {0};
			::GetObject(m_hBitmap, sizeof(bm), &bm);
			m_sizeCaret.cx = bm.bmWidth;
			m_sizeCaret.cy = bm.bmHeight;
		}
		return TRUE;
	}

	void CCaretUI::Destroy( bool bRedraw )
	{
		Hide(bRedraw);

		m_hWnd = NULL;
		m_ptLast.x = m_ptLast.y = -1;
		m_sizeCaret.cx = m_sizeCaret.cy = 0;
		m_rcDraw.left = m_rcDraw.right = m_rcDraw.top = m_rcDraw.bottom = 0;
		SAFE_DELETEOBJ(m_hBitmap);
	}

	void CCaretUI::SetPos( int x, int y, bool bRedraw )
	{
		m_ptLast.x = x;
		m_ptLast.y = y;

 		DestroyTimer();
 		CreateTimer();
		m_bVisible = true;

		if(bRedraw)
			Invalidate();
	}

	void CCaretUI::Show(bool bRedraw)
	{
		CreateTimer();
		m_bOn = true;
		m_bVisible = true;
		if(bRedraw)
			Invalidate();

	}

	void CCaretUI::Hide( bool bRedraw )
	{
		DestroyTimer();
		m_bOn = false;
		m_bVisible = false;

		if(bRedraw)
			Invalidate();

	}

	void CCaretUI::DoPostPaint( const RECT& rcPaint )
	{
		DrawCaret();
	}

	void CCaretUI::DrawCaret()
	{
		POINT pt = {m_ptLast.x,m_ptLast.y+1};
		RECT rc = {pt.x,pt.y,pt.x,pt.y+m_sizeCaret.cy};
		if(m_bVisible)
			m_pPaintManager->GetRenderCore()->DrawLine(rc,1,0xff000000);
	}

	void CCaretUI::OnTimer( IDuiTimer* pTimer,void* param )
	{
		m_bVisible = !m_bVisible;
		Invalidate();
	}

	void CCaretUI::CreateTimer()
	{
		DestroyTimer();
		m_pTimer = MakeDuiTimer(this,&CCaretUI::OnTimer,(void *)NULL,GetCaretBlinkTime());
		m_pTimer->SetDuiTimer();
	}

	void CCaretUI::DestroyTimer()
	{
		if(m_pTimer)
			m_pTimer->KillDuiTimer();
		SAFE_DELETE(m_pTimer);
	}

	void CCaretUI::Invalidate()
	{
		POINT pt = {m_ptLast.x,m_ptLast.y};
		RECT rc = {pt.x,pt.y,pt.x+1,pt.y+m_sizeCaret.cy+2};
		m_pPaintManager->Invalidate(rc);
	}

	

}
