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

	/////////////////////////////////////////////////////////////////////////////////////
	///////

	struct tagTImageInfo_D2D : public tagTImageInfo
	{
		ID2D1Bitmap *pTexture;
		ID2D1Bitmap *GetTexture(){return pTexture;}
		tagTImageInfo_D2D(){pTexture = NULL;}
		~tagTImageInfo_D2D(){SAFE_RELEASE(pTexture);}
	};
	typedef tagTImageInfo_D2D TD2DImageInfo;

	typedef struct tagTD2DFontInfo : public tagTFontInfo
	{
		IDWriteTextFormat* pTextFormat;
		tagTD2DFontInfo()
		{
			pTextFormat = NULL;
		}
		~tagTD2DFontInfo()
		{
			SAFE_RELEASE(pTextFormat);
		}
	} TD2DFontInfo;

	/////////////////////////////////////////////////////////////////////////////////////
	///////

	class CIRenderD2D : public CIRenderCore
	{
	public:
		CIRenderD2D(void);
		~CIRenderD2D(void);

		void SetPaintWindow(HWND hWnd);
		HWND GetPaintWindow();

		void SetWindowGlass(bool bGlass = true){m_bGlass = bGlass;}
		bool IsWindowGlass(){return m_bGlass;}
		void SetWindowLayered(bool bLayered = true){m_bLayered = bLayered;}
		bool IsWindowLayered(){return m_bLayered;}

		RenderWay GetRenderType();

		void TranslateRO(float fTransX,float fTransY);
		void BeginDraw(RECT *rc = NULL);
		void EndDraw(PAINTSTRUCT &ps);

		void SetRenderObj(TRenderObj* pRenderObj,int nCx = -1,int nCy = -1);
		void ReleaseRenderObj();
		HDC GetRenderHDC(BOOL bOffScreen = TRUE);
		void FreeRenderHDC();


		void Clear();
		void Clear(const RECT &rcClear);

		void Resize();

		void GetClipBox(RECT &rcClip);
		void PushClip(const RECT& clipRect,RECT *rcRound = NULL,int nRoundX = 0,int nRoundY = 0,ClipOpt _ClipOpt = OP_REPLACE);
		void PopClip(const RECT & clipRect);

		TImageInfo* LoadImage(TImagePrimData &ImagePrimData);

		TFontInfo* AddFont(TFontPrimData &FontPrimData);
		TFontInfo* GetDefaultFontInfo();
		void SetDefaultFont(TFontPrimData &FontPrimData);

		void DrawImage(const TImageInfo* ImgDate, const RECT& rc, const RECT& rcPaint, const RECT& rcBmpPart,const RECT& rcCorners, bool alphaChannel, BYTE uFade = 255, bool hole = false, bool xtiled = false, bool ytiled = false);
		void GetTextBounds(int& nWidth,int& nHeight,TFontInfo *pFontInfo,LPCTSTR pstrText,UINT uStyle);
		void DrawText(TFontInfo *pFontInfo, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor,UINT uStyle);
		void _DrawText(ID2D1RenderTarget *pRenderTarget,IDWriteFactory *pWriteFactory,RECT& rc,LPCTSTR pstrText,DWORD dwTextColor,TD2DFontInfo *pFontInfo,UINT uStyle);
		void DrawColor(const RECT& rc, DWORD color);
		void DrawGradient(const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical);
		void DrawLine(const RECT& rc, int nSize, DWORD dwPenColor,int nStyle = PS_SOLID);
		void DrawRect(const RECT& rc, int nSize, DWORD dwPenColor);
		void DrawRoundRect(const RECT& rc, int width, int height, int nSize, DWORD dwPenColor);
		void DrawCircle(int nCenterX,int nCenterY,int nRadius,DWORD dwColor);
		void FillCircle(int nCenterX,int nCenterY,int nRadius,DWORD dwColor);
		void DrawArc(int nCenterX,int nCenterY,int nRadius,float fStartAngle,float fSweepAngle,DWORD dwColor);
	private:
		BOOL CreateDeviceIndependentResource();
		BOOL CreateDeviceDependentResource();
		void ReSizeRenderTarget(RECT rc);

		void PushAxisClip();
		void PopAxisClip();

		void PushLayer();
		void PopLayer();
	private:
#pragma region D2D
		ID2D1Factory* m_pD2d1Factory;
		CComPtr<IWICImagingFactory> m_IWICFactory;
		IWICBitmap *m_pbitmap;
		ID2D1RenderTarget* m_pbitmapRT;
		ID2D1RenderTarget* m_pHwndRenderTarget;
		ID2D1RenderTarget* m_pCurRenderTarget;
		IDWriteFactory* m_pDWriteFactory;
		D2D1_PIXEL_FORMAT m_pixelFormat;
		ID2D1GdiInteropRenderTarget *m_pGDIRT;
		ID2D1Layer* m_pRoundLayer;
		RECT m_rcClip;
		RECT m_rcRound;
		int m_nRoundX;
		int m_nRoundY;
#pragma endregion
		HWND m_hWnd;

		HDC m_hDcPaintWnd;
		HDC m_hDcOffScreen;

		HBITMAP m_hBmpOffScreen;
		HBITMAP m_hBmpOld;

		int nRestoreDCID;
		bool m_bLayered;
		bool m_bGlass;

		void *m_pOffBmpPixel;
		BYTE *m_pByte;
		int m_nPitch;

		TFontInfo* m_pDefaultFontInfo;
		long m_nLayerRef;
	private:
		ULONG_PTR m_Token;
	};

}

