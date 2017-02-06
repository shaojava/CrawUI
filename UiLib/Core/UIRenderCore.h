#pragma once
///////////////////////////////////////////////////////////////////////////////////////
#include "../Utils/IRenderDefine.h"
#include "IRenderCore.h"

namespace UiLib {

    typedef CIRenderCore* (WINAPI *GetRenderInterface_)(void);
	typedef bool (WINAPI *ReleaseRef_)(void);
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CUIRenderCore
	{
	public:
		CUIRenderCore(void);
		virtual ~CUIRenderCore(void);
		static CUIRenderCore* GetInstance(RenderWay _RenderWay);
	public:
		void DisConnectRenderCore();
        virtual bool LinkRenderCore();
		virtual void SetPaintWindow(HWND hWnd);
		HWND GetPaintWindow();

		int GetOffBmpPitch();
		void *GetOffBmpPiexl();

		void SetWindowGlass(bool bGlass = true);
		bool IsWindowGlass();
		void SetWindowLayered(bool bLayered = true);
		bool IsWindowLayered();

		virtual void TranslateRO(float fTransX,float fTransY);

		virtual void BeginDraw(RECT *rc = NULL);
		virtual void EndDraw(PAINTSTRUCT &ps);

		virtual void SetRenderObj(TRenderObj* pRenderObj,int nCx = -1,int nCy = -1);
		virtual HDC GetRenderHDC(BOOL bOffScreen = TRUE);
		virtual void FreeRenderHDC();
		virtual void ReleaseRenderObj();

		virtual RenderWay GetRenderType() = 0;

		virtual void Clear() = 0;
		virtual void Clear(const RECT &rcClear) = 0;

		virtual void GetClipBox(RECT& rcClip);
		virtual void PushClip(const RECT& clipRect,RECT* rcRound = NULL,int nRoundX = 0,int nRoundY = 0,ClipOpt _ClipOpt = OP_REPLACE) = 0;
		virtual void PopClip(const RECT & clipRect) = 0;

		virtual void Resize();

		virtual TImageInfo* LoadImage(STRINGorID bitmap, LPCTSTR type = NULL, DWORD mask = 0);
		virtual TImageInfo* LoadImage(HBITMAP hBitmap,int nWidth, int nHeight, bool bAlpha);

		virtual TFontInfo* GetDefaultFontInfo();
		virtual void SetDefaultFont(LPCTSTR pStrFontName = _T(""), int nSize = 0, bool bBold = false, bool bUnderline = false, bool bItalic = false);
		virtual TFontInfo* AddFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);

		virtual bool DrawImageString(CPaintManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint,CDuiImage& sImage, bool bNeedAlpha = FALSE, BYTE bNewFade = 255);
		virtual bool DrawImage(CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, const CDuiImage& sImageName,const CDuiString& sImageResType, RECT rcItem, RECT rcBmpPart, RECT rcCorner, DWORD dwMask, BYTE bFade, bool bHole, bool bTiledX, bool bTiledY, bool bNeedAlpha = FALSE);
																			// date,current item destrc||current invalide rc||item image rc||nine in one rc;
		virtual void DrawImage(const TImageInfo* ImgDate, const RECT& rc, const RECT& rcPaint, const RECT& rcBmpPart, const RECT& rcCorners, bool alphaChannel, BYTE uFade = 255, bool hole = false, bool xtiled = false, bool ytiled = false);

		virtual void GetTextBounds(int& nWidth,int& nHeight,CPaintManagerUI* pManager, LPCTSTR pstrText, int iFont, UINT uStyle);
		virtual void DrawText(CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, int iFont, UINT uStyle,BOOL bBlur = FALSE);
		virtual void DrawHtmlText(CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, RECT* prcLinks, CDuiString* sLinks, int& nLinkRects, UINT uStyle ,BOOL bBlur = FALSE);
		virtual void _DrawHtmlText(CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, RECT* prcLinks, CDuiString* sLinks, int& nLinkRects, UINT uStyle,HDC hDC = NULL);

		virtual void DrawColor(const RECT& rc, DWORD color);
		virtual void DrawGradient(const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical);
		virtual void DrawLine(const RECT& rc, int nSize, DWORD dwPenColor,int nStyle = PS_SOLID);
		virtual void DrawRect(const RECT& rc, int nSize, DWORD dwPenColor);
		virtual void DrawRoundRect(const RECT& rc, int width, int height, int nSize, DWORD dwPenColor);
		virtual void DrawRoundDotRect(const RECT& rc,int width,int height,int nSize, DWORD dwPenColor);
		virtual void FillRoundRect(const RECT& rc, int width, int height, int nSize, DWORD dwPenColor);
		virtual void DrawCircle(int nCenterX,int nCenterY,int nRadius,DWORD dwColor);
		virtual void FillCircle(int nCenterX,int nCenterY,int nRadius,DWORD dwColor);
		virtual void DrawArc(int nCenterX,int nCenterY,int nRadius,float fStartAngle,float fSweepAngle,DWORD dwColor);
	public:
        HMODULE hRenderAPI;
        CIRenderCore *m_pIRenderCore;
	private:
		static CUIRenderCore *RenderCore;
	};
}