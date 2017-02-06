#include "StdAfx.h"
///////////////////////////////////////////////////////////////////////////////////////

namespace UiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

CRenderClip::CRenderClip()
{
	rcCur.left = rcCur.top = rcCur.right = rcCur.bottom = 0;
	rcOld.left = rcOld.top = rcOld.right = rcOld.bottom = 0;
	m_pPaintManager = NULL;
}

CRenderClip::~CRenderClip()
{
	CUIRenderCore *pRenderCore = m_pPaintManager->GetRenderCore();
	pRenderCore->PopClip(rcOld);
}

void CRenderClip::SetPaintManager(CPaintManagerUI *pPaintManager)
{
	m_pPaintManager = pPaintManager;
}

void CRenderClip::GenerateClip(RECT rc, CRenderClip& clip,ClipOpt uOpt)
{
	CUIRenderCore *pRenderCore = clip.m_pPaintManager->GetRenderCore();
	pRenderCore->GetClipBox(clip.rcOld);
	pRenderCore->PushClip(rc,NULL,0,0,uOpt);
	pRenderCore->GetClipBox(clip.rcCur);
}

void CRenderClip::GenerateRoundClip(RECT rc, RECT rcItem, int width, int height, CRenderClip& clip)
{
	CUIRenderCore *pRenderCore = clip.m_pPaintManager->GetRenderCore();
	pRenderCore->GetClipBox(clip.rcOld);
	pRenderCore->PushClip(rc,&rcItem,width,height,OP_AND);
	pRenderCore->GetClipBox(clip.rcCur);
}

void CRenderClip::UseOldClipBegin(CRenderClip& clip)
{
	CUIRenderCore *pRenderCore = clip.m_pPaintManager->GetRenderCore();
	pRenderCore->PushClip(clip.rcOld);
}

void CRenderClip::UseOldClipEnd(CRenderClip& clip)
{
	CUIRenderCore *pRenderCore = clip.m_pPaintManager->GetRenderCore();
	pRenderCore->PushClip(clip.rcCur);
}

} // namespace UiLib
