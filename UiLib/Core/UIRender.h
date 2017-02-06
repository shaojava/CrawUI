#ifndef __UIRENDER_H__
#define __UIRENDER_H__

#pragma once



namespace UiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CRenderClip
{
public:
	CRenderClip();
    ~CRenderClip();
    RECT rcCur;
	RECT rcOld;

	CPaintManagerUI *m_pPaintManager;

	void SetPaintManager(CPaintManagerUI *pPaintManager);
    static void GenerateClip(RECT rc, CRenderClip& clip,ClipOpt uOpt = OP_REPLACE);
    static void GenerateRoundClip(RECT rc, RECT rcItem, int width, int height, CRenderClip& clip);
    static void UseOldClipBegin(CRenderClip& clip);
    static void UseOldClipEnd(CRenderClip& clip);
};

} // namespace UiLib

#endif // __UIRENDER_H__
