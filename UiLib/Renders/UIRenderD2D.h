#pragma once
#include "../Core/UIRenderCore.h"

namespace UiLib {
	class UILIB_API CUIRenderD2D : public CUIRenderCore
	{
	public:
		CUIRenderD2D(void);
		~CUIRenderD2D(void);

		bool LinkRenderCore();

		void SetPaintWindow(HWND hWnd);

		void ReleaseRenderObj();

		RenderWay GetRenderType();

		void Clear();
		void Clear(const RECT &rcClear);

		void PushClip(const RECT& clipRect,RECT* rcRound = NULL,int nRoundX = 0,int nRoundY = 0,ClipOpt _ClipOpt = OP_REPLACE);
		void PopClip(const RECT & clipRect);

		TImageInfo* LoadImage(STRINGorID bitmap, LPCTSTR type = NULL, DWORD mask = 0);
		TImageInfo* LoadImage(HBITMAP hBitmap,int nWidth, int nHeight, bool bAlpha);
		
		TFontInfo* AddFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
		void SetDefaultFont(LPCTSTR pStrFontName = _T(""), int nSize = 0, bool bBold = false, bool bUnderline = false, bool bItalic = false);
	};

}