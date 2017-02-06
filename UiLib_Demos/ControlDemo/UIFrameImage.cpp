#include "stdafx.h"
#include "UIFrameImage.h"

CFrameIamgeUI::CFrameIamgeUI(void)
{
	m_nElapse = 0;
	m_nPlayIndex = 0;
}

CFrameIamgeUI::~CFrameIamgeUI(void)
{
	m_pManager->KillTimer(this, UPDATE_FRAME_TIMER);
	ClearFrame();
}

LPCTSTR CFrameIamgeUI::GetClass() const
{
	return _T("FrameIamgeUI");
}

LPVOID CFrameIamgeUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("FrameIamge")) == 0 ) return static_cast<CFrameIamgeUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

void CFrameIamgeUI::DoEvent(TEventUI &event)
{
	if (event.Type == UIEVENT_TIMER && event.wParam == UPDATE_FRAME_TIMER)
	{
		if(m_nPlayIndex < m_ImgFrames.size() -1)
			m_nPlayIndex ++;
		else
			m_nPlayIndex = 0;
		Invalidate();
	}
	CControlUI::DoEvent(event);
}

void CFrameIamgeUI::DoPaint(const RECT& rcPaint)
{
	if(m_nPlayIndex < 0 || m_nPlayIndex >= m_ImgFrames.size()) return;
	__super::DoPaint(rcPaint);
	Graphics gs(m_pManager->GetRenderCore()->GetRenderHDC());
	RECT rcClip;
	m_pManager->GetRenderCore()->GetClipBox(rcClip);
	Rect gClip(rcClip.left,rcClip.top,rcClip.right-rcClip.left,rcClip.bottom-rcClip.top);
	gs.SetClip(gClip);
	Image *pImage = m_ImgFrames[m_nPlayIndex];
	Rect rcBound(m_rcItem.left,m_rcItem.top,m_rcItem.right-m_rcItem.left,m_rcItem.bottom-m_rcItem.top);
	gs.SetInterpolationMode(InterpolationModeHighQuality);
	gs.DrawImage(pImage, rcBound, 0, 0, pImage->GetWidth(), pImage->GetHeight(), UnitPixel);
}

bool CFrameIamgeUI::AddFrame(Image *pImage,int nIndex)
{
	if(!pImage) return false;
	if(nIndex == -1)
		m_ImgFrames.push_back(pImage);
	else if(nIndex >= 0 || nIndex <= m_ImgFrames.size())
		m_ImgFrames.insert(m_ImgFrames.begin()+nIndex,pImage);
	else return false;
	return true;
}

void CFrameIamgeUI::ClearFrame()
{
	for(int i = 0 ; i < m_ImgFrames.size() ; i++)
	{
		Image *pImage = m_ImgFrames[i];
		SAFE_DELETE(pImage);
	}
	m_ImgFrames.clear();
}

void CFrameIamgeUI::StartPlay(int nElapse)
{
	if(m_ImgFrames.size() < 2) return;
	m_nElapse = nElapse;
	m_nPlayIndex = 0;
	m_pManager->SetTimer(this,UPDATE_FRAME_TIMER,nElapse);
}

void CFrameIamgeUI::DoPaintWithOutClip(const RECT& rcPaint)
{
	__super::DoPaintWithOutClip(rcPaint);
}
