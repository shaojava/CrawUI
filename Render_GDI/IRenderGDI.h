#pragma once
#include "../Common/Include/UiLib/Core/IRenderCore.h"

#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib, "gdiplus.lib")


namespace UiLib
{
	static UINT g_uRef = 0;
	RENDER_C_API CIRenderCore *GetRenderInstance();
	RENDER_C_API bool ReleaseRef();

	class CIRenderGDI : public CIRenderCore
	{
	public:
		CIRenderGDI(void);
		~CIRenderGDI(void);

		void SetPaintWindow(HWND hWnd);
		HWND GetPaintWindow();

		int GetOffBmpPitch();
		void *GetOffBmpPiexl();

		void SetWindowGlass(bool bGlass = true);
		bool IsWindowGlass();
		void SetWindowLayered(bool bLayered = true);
		bool IsWindowLayered();

		void TranslateRO(float fTransX,float fTransY);
		void BeginDraw(RECT *rc = NULL);
		void EndDraw(PAINTSTRUCT &ps);

		void SetRenderObj(TRenderObj* pRenderObj,int nCx = -1,int nCy = -1);
		HDC GetRenderHDC(BOOL bOffScreen = TRUE);
		void FreeRenderHDC();
		void ReleaseRenderObj();

		void Clear();
		void Clear(const RECT &rcClear);

		void GetClipBox(RECT &rcClip);
		void PushClip(const RECT& clipRect,RECT *rcRound = NULL,int nRoundX = 0,int nRoundY = 0,ClipOpt _ClipOpt = OP_REPLACE);
		void PopClip(const RECT & clipRect);

		void Resize();

		TImageInfo* LoadImage(TImagePrimData &ImagePrimData);
		TFontInfo* GetDefaultFontInfo();
		void SetDefaultFont(TFontPrimData &FontPrimData);
		TFontInfo* AddFont(TFontPrimData &FontPrimData);

		// date,current item destrc||current invalide rc||item image rc||nine in one rc;
		void DrawImage(const TImageInfo* ImgDate, const RECT& rc, const RECT& rcPaint, const RECT& rcBmpPart, const RECT& rcCorners, bool alphaChannel, BYTE uFade = 255, bool hole = false, bool xtiled = false, bool ytiled = false);

		void GetTextBounds(int& nWidth,int& nHeight,TFontInfo *pFontInfo,LPCTSTR pstrText,UINT uStyle);
		void DrawText(TFontInfo *pFontInfo, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor,UINT uStyle);
		void DrawColor(const RECT& rc, DWORD color);
		void DrawGradient(const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical);
		void DrawLine(const RECT& rc, int nSize, DWORD dwPenColor,int nStyle = PS_SOLID);
		void DrawRect(const RECT& rc, int nSize, DWORD dwPenColor);
		void DrawRoundRect(const RECT& rc, int width, int height, int nSize, DWORD dwPenColor);
		void DrawCircle(int nCenterX,int nCenterY,int nRadius,DWORD dwColor);
		void FillCircle(int nCenterX,int nCenterY,int nRadius,DWORD dwColor);
		void DrawArc(int nCenterX,int nCenterY,int nRadius,float fStartAngle,float fSweepAngle,DWORD dwColor);
	private:
		HWND m_hWnd;

		HDC m_hDcPaintWnd;
		HDC m_hDcOffScreen;

		HBITMAP m_hBmpOffScreen;
		HBITMAP m_hBmpOld;

		int nRestoreDCID;
		bool m_bLayered;
		bool m_bGlass;

		void *m_pOffBmpPixel;
		int m_nPitch;

		TFontInfo* m_pDefaultFontInfo;
	private:
		ULONG_PTR m_Token;
	};

}