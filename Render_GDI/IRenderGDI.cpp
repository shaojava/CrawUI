#include "StdAfx.h"
#include "IRenderGDI.h"

namespace UiLib
{
	CIRenderCore * GetRenderInstance()
	{
		CIRenderGDI *pGDIRender = new CIRenderGDI();
		g_uRef++;
		return pGDIRender;
	}

	bool ReleaseRef()
	{
		g_uRef--;
		if(0 == g_uRef)
			return true;
		return false;
	}

	CIRenderGDI::CIRenderGDI(void)
	{
		Gdiplus::GdiplusStartupInput input; 
		Gdiplus::GdiplusStartup(& m_Token, & input, NULL);
		m_hWnd = NULL;
		m_hDcPaintWnd = NULL;
		m_hDcOffScreen = NULL;
		m_hBmpOffScreen = NULL;
		m_hBmpOld = NULL;
		m_pOffBmpPixel = NULL;
		m_pDefaultFontInfo = NULL;
		nRestoreDCID = 0;
		m_nPitch = 0;
		m_bLayered = false;
		m_bGlass = false;
	}


	CIRenderGDI::~CIRenderGDI(void)
	{
		SAFE_DELETE(m_pDefaultFontInfo);
		Gdiplus::GdiplusShutdown(m_Token);
		ReleaseRenderObj();
	}

	void CIRenderGDI::SetPaintWindow( HWND hWnd )
	{
		if(hWnd)
		{
			if(m_hDcPaintWnd)
				::ReleaseDC(m_hWnd,m_hDcPaintWnd);
			m_hWnd = hWnd;
			m_hDcPaintWnd = ::GetDC(m_hWnd);
		}
	}

	HWND CIRenderGDI::GetPaintWindow()
	{
		return m_hWnd;
	}

	int CIRenderGDI::GetOffBmpPitch()
	{
		return m_nPitch;
	}

	void * CIRenderGDI::GetOffBmpPiexl()
	{
		return m_pOffBmpPixel;
	}

	void CIRenderGDI::SetWindowGlass( bool bGlass /*= true*/ )
	{
		m_bGlass = bGlass;
	}

	bool CIRenderGDI::IsWindowGlass()
	{
		return m_bGlass;
	}

	void CIRenderGDI::SetWindowLayered( bool bLayered /*= true*/ )
	{
		m_bLayered = bLayered;
	}

	bool CIRenderGDI::IsWindowLayered()
	{
		return m_bLayered;
	}

	void CIRenderGDI::TranslateRO(float fTransX,float fTransY)
	{
		if(m_hDcOffScreen)
		{
			XFORM xform;
			xform.eM11 = 1;
			xform.eM12 = 0;
			xform.eM21 = 0;
			xform.eM22 = 1;
			xform.eDx = fTransX;
			xform.eDy = fTransY;
			SetGraphicsMode(m_hDcOffScreen,GM_ADVANCED);
			SetWorldTransform(m_hDcOffScreen,&xform);
		}
	}

	void CIRenderGDI::BeginDraw( RECT *rc /*= NULL*/ )
	{
		if(m_bLayered)
		{
			DWORD dwExStyle=GetWindowLongPtr(m_hWnd,GWL_EXSTYLE);
			if((dwExStyle&WS_EX_LAYERED)!=0x80000)
				SetWindowLongPtr(m_hWnd,GWL_EXSTYLE,dwExStyle^WS_EX_LAYERED);
		}

		if(!m_hDcOffScreen && m_hWnd)
		{
			RECT rcClient = { 0 };
			::GetClientRect(m_hWnd, &rcClient);
			int nRcClientWidth = rcClient.right - rcClient.left;
			int nRcClientHeight = rcClient.bottom - rcClient.top;
			SAFE_DELETEDC(m_hDcOffScreen);
			m_hDcOffScreen = ::CreateCompatibleDC(m_hDcPaintWnd);
			BITMAPINFO bmi;
			memset(&bmi, 0, sizeof(bmi));
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth  = nRcClientWidth;
			bmi.bmiHeader.biHeight = -nRcClientHeight; 
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = 0;
			m_nPitch = 4*nRcClientWidth;
			SAFE_DELETEOBJ(m_hBmpOffScreen);
			m_hBmpOffScreen = ::CreateDIBSection(m_hDcPaintWnd, &bmi, DIB_RGB_COLORS, &m_pOffBmpPixel, NULL,0);
			m_hBmpOld = (HBITMAP)::SelectObject(m_hDcOffScreen,m_hBmpOffScreen);
		}
		if(m_bLayered&&rc)
		{
			HBRUSH hEraser = ::CreateSolidBrush(RGB(0,0,0));
			::FillRect(m_hDcOffScreen,rc,hEraser);
			SAFE_DELETEOBJ(hEraser);
		}
		nRestoreDCID = ::SaveDC(m_hDcOffScreen);
	}

	void CIRenderGDI::EndDraw( PAINTSTRUCT &ps )
	{
		::RestoreDC(m_hDcOffScreen,nRestoreDCID);
		if(m_bLayered)
		{
			RECT rcWnd={0};
			::GetWindowRect(m_hWnd, &rcWnd);
			POINT pt = {rcWnd.left,rcWnd.top};
			RECT rcClient = { 0 };
			::GetClientRect(m_hWnd, &rcClient);
			SIZE szWindow = {rcClient.right - rcClient.left,rcClient.bottom - rcClient.top};
			POINT ptSrc = {0, 0};
			BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
			::UpdateLayeredWindow(m_hWnd,NULL,&pt,&szWindow,m_hDcOffScreen,&ptSrc,0,&blendPixelFunction,ULW_ALPHA);
		}
		else
			::BitBlt(ps.hdc,ps.rcPaint.left,ps.rcPaint.top,ps.rcPaint.right-ps.rcPaint.left,ps.rcPaint.bottom-ps.rcPaint.top,m_hDcOffScreen,ps.rcPaint.left, ps.rcPaint.top,SRCCOPY);
	}

	void CIRenderGDI::SetRenderObj( TRenderObj* pRenderObj,int nCx,int nCy)
	{
		if(pRenderObj->hDC)
		{
			if(m_hDcOffScreen)
			{
				if(m_hBmpOld)
					::SelectObject(m_hDcOffScreen,m_hBmpOld);
				SAFE_DELETEDC(m_hDcOffScreen);
			}
			m_hDcOffScreen = pRenderObj->hDC;
		}
	}

	HDC CIRenderGDI::GetRenderHDC( BOOL bOffScreen /*= TRUE*/ )
	{
		if(bOffScreen)
			return m_hDcOffScreen;
		else
			return m_hDcPaintWnd;
	}

	void CIRenderGDI::FreeRenderHDC()
	{

	}

	void CIRenderGDI::ReleaseRenderObj()
	{
		if(m_hDcOffScreen)
		{
			if(m_hBmpOld)
				::SelectObject(m_hDcOffScreen,m_hBmpOld);
			SAFE_DELETEDC(m_hDcOffScreen);
		}
		SAFE_DELETEOBJ(m_hBmpOffScreen);
		m_pOffBmpPixel = NULL;
	}

	void CIRenderGDI::Clear()
	{

	}

	void CIRenderGDI::Clear( const RECT &rcClear )
	{

	}

	void CIRenderGDI::GetClipBox(RECT &rcClip)
	{
		if(m_hDcOffScreen)
			::GetClipBox(m_hDcOffScreen,&rcClip);
	}

	void CIRenderGDI::PushClip( const RECT& clipRect,RECT *rcRound,int nRoundX,int nRoundY,ClipOpt _ClipOpt )
	{
 		HRGN hRgnClip = ::CreateRectRgnIndirect(&clipRect);
		if(rcRound)
		{
			HRGN hRgnItem = ::CreateRoundRectRgn(rcRound->left, rcRound->top, rcRound->right + 1, rcRound->bottom + 1, nRoundX, nRoundY);
			::CombineRgn(hRgnClip, hRgnClip, hRgnItem, RGN_AND);
			::DeleteObject(hRgnItem);
		}
		switch(_ClipOpt)
		{
		case OP_REPLACE:
			SelectClipRgn(m_hDcOffScreen,hRgnClip);
			break;
		case OP_AND:
			ExtSelectClipRgn(m_hDcOffScreen,hRgnClip, RGN_AND);
			break;
		default:
			break;
		}
		::DeleteObject(hRgnClip);
	}

	void CIRenderGDI::PopClip( const RECT & clipRect )
	{
		HRGN rgnClip = ::CreateRectRgnIndirect(&clipRect);
		SelectClipRgn(m_hDcOffScreen,rgnClip);
		::DeleteObject(rgnClip);
	}

	void CIRenderGDI::Resize()
	{

	}

	TImageInfo* CIRenderGDI::LoadImage( TImagePrimData &ImagePrimData )
	{
		TImageInfo* data = new TImageInfo;
		data->hBitmap = ImagePrimData.hBitmap;
		data->nX = ImagePrimData.nX;
		data->nY = ImagePrimData.nY;
		data->alphaChannel = ImagePrimData.bAlphaChannel;
		return data;
	}

	TFontInfo* CIRenderGDI::GetDefaultFontInfo()
	{
		if( m_pDefaultFontInfo&&m_pDefaultFontInfo->tm.tmHeight == 0 ) 
		{
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaintWnd, m_pDefaultFontInfo->hFont);
			::GetTextMetrics(m_hDcPaintWnd, &m_pDefaultFontInfo->tm);
			::SelectObject(m_hDcPaintWnd, hOldFont);
		}
		return m_pDefaultFontInfo;
	}

	void CIRenderGDI::SetDefaultFont( TFontPrimData &FontPrimData )
	{
		SAFE_DELETE(m_pDefaultFontInfo);
		m_pDefaultFontInfo = new TFontInfo;
		m_pDefaultFontInfo->bBold = FontPrimData.bBold;
		m_pDefaultFontInfo->bItalic = FontPrimData.bItalic;
		m_pDefaultFontInfo->bUnderline = FontPrimData.bUnderline;
		m_pDefaultFontInfo->hFont = FontPrimData.hFont;
		m_pDefaultFontInfo->iSize = FontPrimData.iSize;
		_tcscpy_s(m_pDefaultFontInfo->sFontName,FontPrimData.sFontName);
		m_pDefaultFontInfo->tm = FontPrimData.tm;
		if(m_hDcPaintWnd)
		{
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaintWnd, m_pDefaultFontInfo->hFont);
			::GetTextMetrics(m_hDcPaintWnd, &(m_pDefaultFontInfo->tm));
			::SelectObject(m_hDcPaintWnd,hOldFont);
		}
	}

	TFontInfo* CIRenderGDI::AddFont( TFontPrimData &FontPrimData )
	{
		TFontInfo* pFontInfo = new TFontInfo;
		pFontInfo->hFont = FontPrimData.hFont;
		_tcscpy_s(pFontInfo->sFontName, FontPrimData.sFontName);
		pFontInfo->iSize = FontPrimData.iSize;
		pFontInfo->bBold = FontPrimData.bBold;
		pFontInfo->bUnderline = FontPrimData.bUnderline;
		pFontInfo->bItalic = FontPrimData.bItalic;
		if( m_hDcPaintWnd) {
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaintWnd, pFontInfo->hFont);
			::GetTextMetrics(m_hDcPaintWnd, &pFontInfo->tm);
			::SelectObject(m_hDcPaintWnd, hOldFont);
		}
		return pFontInfo;
	}

	static COLORREF PixelAlpha(COLORREF clrSrc, double src_darken, COLORREF clrDest, double dest_darken)
	{
		return RGB (GetRValue (clrSrc) * src_darken + GetRValue (clrDest) * dest_darken, 
			GetGValue (clrSrc) * src_darken + GetGValue (clrDest) * dest_darken, 
			GetBValue (clrSrc) * src_darken + GetBValue (clrDest) * dest_darken);

	}

	static BOOL WINAPI AlphaBitBlt(HDC hDC, int nDestX, int nDestY, int dwWidth, int dwHeight, HDC hSrcDC, int nSrcX, int nSrcY, int wSrc, int hSrc, BLENDFUNCTION ftn)
	{
		HDC hTempDC = ::CreateCompatibleDC(hDC);
		if (NULL == hTempDC)
			return FALSE;

		//Creates Source DIB
		LPBITMAPINFO lpbiSrc = NULL;
		// Fill in the BITMAPINFOHEADER
		lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
		if (lpbiSrc == NULL)
		{
			::DeleteDC(hTempDC);
			return FALSE;
		}
		lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		lpbiSrc->bmiHeader.biWidth = dwWidth;
		lpbiSrc->bmiHeader.biHeight = dwHeight;
		lpbiSrc->bmiHeader.biPlanes = 1;
		lpbiSrc->bmiHeader.biBitCount = 32;
		lpbiSrc->bmiHeader.biCompression = BI_RGB;
		lpbiSrc->bmiHeader.biSizeImage = dwWidth * dwHeight;
		lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
		lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
		lpbiSrc->bmiHeader.biClrUsed = 0;
		lpbiSrc->bmiHeader.biClrImportant = 0;

		COLORREF* pSrcBits = NULL;
		HBITMAP hSrcDib = CreateDIBSection (
			hSrcDC, lpbiSrc, DIB_RGB_COLORS, (void **)&pSrcBits,
			NULL, NULL);

		if ((NULL == hSrcDib) || (NULL == pSrcBits)) 
		{
			delete [] lpbiSrc;
			::DeleteDC(hTempDC);
			return FALSE;
		}

		HBITMAP hOldTempBmp = (HBITMAP)::SelectObject (hTempDC, hSrcDib);
		::StretchBlt(hTempDC, 0, 0, dwWidth, dwHeight, hSrcDC, nSrcX, nSrcY, wSrc, hSrc, SRCCOPY);
		::SelectObject (hTempDC, hOldTempBmp);

		//Creates Destination DIB
		LPBITMAPINFO lpbiDest = NULL;
		// Fill in the BITMAPINFOHEADER
		lpbiDest = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
		if (lpbiDest == NULL)
		{
			delete [] lpbiSrc;
			::DeleteObject(hSrcDib);
			::DeleteDC(hTempDC);
			return FALSE;
		}

		lpbiDest->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		lpbiDest->bmiHeader.biWidth = dwWidth;
		lpbiDest->bmiHeader.biHeight = dwHeight;
		lpbiDest->bmiHeader.biPlanes = 1;
		lpbiDest->bmiHeader.biBitCount = 32;
		lpbiDest->bmiHeader.biCompression = BI_RGB;
		lpbiDest->bmiHeader.biSizeImage = dwWidth * dwHeight;
		lpbiDest->bmiHeader.biXPelsPerMeter = 0;
		lpbiDest->bmiHeader.biYPelsPerMeter = 0;
		lpbiDest->bmiHeader.biClrUsed = 0;
		lpbiDest->bmiHeader.biClrImportant = 0;

		COLORREF* pDestBits = NULL;
		HBITMAP hDestDib = CreateDIBSection (
			hDC, lpbiDest, DIB_RGB_COLORS, (void **)&pDestBits,
			NULL, NULL);

		if ((NULL == hDestDib) || (NULL == pDestBits))
		{
			delete [] lpbiSrc;
			::DeleteObject(hSrcDib);
			::DeleteDC(hTempDC);
			return FALSE;
		}

		::SelectObject (hTempDC, hDestDib);
		::BitBlt (hTempDC, 0, 0, dwWidth, dwHeight, hDC, nDestX, nDestY, SRCCOPY);
		::SelectObject (hTempDC, hOldTempBmp);

		double src_darken;
		BYTE nAlpha;

		for (int pixel = 0; pixel < dwWidth * dwHeight; pixel++, pSrcBits++, pDestBits++)
		{
			nAlpha = LOBYTE(*pSrcBits >> 24);
			src_darken = (double) (nAlpha * ftn.SourceConstantAlpha) / 255.0 / 255.0;
			if( src_darken < 0.0 ) src_darken = 0.0;
			*pDestBits = PixelAlpha(*pSrcBits, src_darken, *pDestBits, 1.0 - src_darken);
		} //for

		::SelectObject (hTempDC, hDestDib);
		::BitBlt (hDC, nDestX, nDestY, dwWidth, dwHeight, hTempDC, 0, 0, SRCCOPY);
		::SelectObject (hTempDC, hOldTempBmp);

		delete [] lpbiDest;
		::DeleteObject(hDestDib);

		delete [] lpbiSrc;
		::DeleteObject(hSrcDib);

		::DeleteDC(hTempDC);
		return TRUE;
	}

	void CIRenderGDI::DrawImage( const TImageInfo* ImgDate, const RECT& rc, const RECT& rcPaint, const RECT& rcBmpPart, const RECT& rcCorners, bool alphaChannel, BYTE uFade /*= 255*/, bool hole /*= false*/, bool xtiled /*= false*/, bool ytiled /*= false*/ )
	{
		HDC hDC = m_hDcOffScreen;
		HBITMAP hBitmap = NULL;
		if(ImgDate)
			hBitmap = ImgDate->hBitmap;
		else
			return;

		typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
		static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");

		if( lpAlphaBlend == NULL ) lpAlphaBlend = AlphaBitBlt;
		if( hBitmap == NULL ) return;

		HDC hCloneDC = ::CreateCompatibleDC(hDC);
		HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
		RECT rcTemp = {0};
		RECT rcDest = {0};
		if( m_bGlass || m_bLayered || (lpAlphaBlend && (alphaChannel || uFade < 255) )) {
			BLENDFUNCTION bf = { AC_SRC_OVER, 0, uFade, AC_SRC_ALPHA };
			// middle
			if( !hole ) {
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					if( !xtiled && !ytiled ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
							rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
							rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
					}
					else if( xtiled && ytiled ) {
						LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
						LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
						int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
						int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
						for( int j = 0; j < iTimesY; ++j ) {
							LONG lDestTop = rcDest.top + lHeight * j;
							LONG lDestBottom = rcDest.top + lHeight * (j + 1);
							LONG lDrawHeight = lHeight;
							if( lDestBottom > rcDest.bottom ) {
								lDrawHeight -= lDestBottom - rcDest.bottom;
								lDestBottom = rcDest.bottom;
							}
							for( int i = 0; i < iTimesX; ++i ) {
								LONG lDestLeft = rcDest.left + lWidth * i;
								LONG lDestRight = rcDest.left + lWidth * (i + 1);
								LONG lDrawWidth = lWidth;
								if( lDestRight > rcDest.right ) {
									lDrawWidth -= lDestRight - rcDest.right;
									lDestRight = rcDest.right;
								}
								lpAlphaBlend(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, 
									lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, 
									rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, lDrawWidth, lDrawHeight, bf);
							}
						}
					}
					else if( xtiled ) {
						LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
						int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
						for( int i = 0; i < iTimes; ++i ) {
							LONG lDestLeft = rcDest.left + lWidth * i;
							LONG lDestRight = rcDest.left + lWidth * (i + 1);
							LONG lDrawWidth = lWidth;
							if( lDestRight > rcDest.right ) {
								lDrawWidth -= lDestRight - rcDest.right;
								lDestRight = rcDest.right;
							}
							lpAlphaBlend(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
								hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
								lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
						}
					}
					else { // ytiled
						LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
						int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
						for( int i = 0; i < iTimes; ++i ) {
							LONG lDestTop = rcDest.top + lHeight * i;
							LONG lDestBottom = rcDest.top + lHeight * (i + 1);
							LONG lDrawHeight = lHeight;
							if( lDestBottom > rcDest.bottom ) {
								lDrawHeight -= lDestBottom - rcDest.bottom;
								lDestBottom = rcDest.bottom;
							}
							lpAlphaBlend(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
								hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
								rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, bf);                    
						}
					}
				}
			}

			// left-top
			if( rcCorners.left > 0 && rcCorners.top > 0 ) {
				rcDest.left = rc.left;
				rcDest.top = rc.top;
				rcDest.right = rcCorners.left;
				rcDest.bottom = rcCorners.top;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, bf);
				}
			}
			// top
			if( rcCorners.top > 0 ) {
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.top;
				rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				rcDest.bottom = rcCorners.top;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
						rcCorners.left - rcCorners.right, rcCorners.top, bf);
				}
			}
			// right-top
			if( rcCorners.right > 0 && rcCorners.top > 0 ) {
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.top;
				rcDest.right = rcCorners.right;
				rcDest.bottom = rcCorners.top;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, bf);
				}
			}
			// left
			if( rcCorners.left > 0 ) {
				rcDest.left = rc.left;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rcCorners.left;
				rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
						rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
				}
			}
			// right
			if( rcCorners.right > 0 ) {
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rcCorners.right;
				rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
						rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, bf);
				}
			}
			// left-bottom
			if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
				rcDest.left = rc.left;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rcCorners.left;
				rcDest.bottom = rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, bf);
				}
			}
			// bottom
			if( rcCorners.bottom > 0 ) {
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
				rcDest.bottom = rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
						rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, bf);
				}
			}
			// right-bottom
			if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rcCorners.right;
				rcDest.bottom = rcCorners.bottom;
				rcDest.right += rcDest.left;
				rcDest.bottom += rcDest.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcDest.right -= rcDest.left;
					rcDest.bottom -= rcDest.top;
					lpAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
						rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
						rcCorners.bottom, bf);
				}
			}
		}
		else 
		{
			::SetStretchBltMode(hDC, HALFTONE);
			if (rc.right - rc.left == rcBmpPart.right - rcBmpPart.left \
				&& rc.bottom - rc.top == rcBmpPart.bottom - rcBmpPart.top \
				&& rcCorners.left == 0 && rcCorners.right == 0 && rcCorners.top == 0 && rcCorners.bottom == 0)
			{
				if( ::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
					::BitBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top, \
						hCloneDC, rcBmpPart.left + rcTemp.left - rc.left, rcBmpPart.top + rcTemp.top - rc.top, SRCCOPY);
				}
			}
			else
			{
				// middle
				if( !hole ) {
					rcDest.left = rc.left + rcCorners.left;
					rcDest.top = rc.top + rcCorners.top;
					rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
					rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						if( !xtiled && !ytiled ) {
							rcDest.right -= rcDest.left;
							rcDest.bottom -= rcDest.top;
							::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
								rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
								rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, \
								rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
						}
						else if( xtiled && ytiled ) {
							LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
							LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
							int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
							int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
							for( int j = 0; j < iTimesY; ++j ) {
								LONG lDestTop = rcDest.top + lHeight * j;
								LONG lDestBottom = rcDest.top + lHeight * (j + 1);
								LONG lDrawHeight = lHeight;
								if( lDestBottom > rcDest.bottom ) {
									lDrawHeight -= lDestBottom - rcDest.bottom;
									lDestBottom = rcDest.bottom;
								}
								for( int i = 0; i < iTimesX; ++i ) {
									LONG lDestLeft = rcDest.left + lWidth * i;
									LONG lDestRight = rcDest.left + lWidth * (i + 1);
									LONG lDrawWidth = lWidth;
									if( lDestRight > rcDest.right ) {
										lDrawWidth -= lDestRight - rcDest.right;
										lDestRight = rcDest.right;
									}
									::BitBlt(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j, \
										lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC, \
										rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, SRCCOPY);
								}
							}
						}
						else if( xtiled ) {
							LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
							int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
							for( int i = 0; i < iTimes; ++i ) {
								LONG lDestLeft = rcDest.left + lWidth * i;
								LONG lDestRight = rcDest.left + lWidth * (i + 1);
								LONG lDrawWidth = lWidth;
								if( lDestRight > rcDest.right ) {
									lDrawWidth -= lDestRight - rcDest.right;
									lDestRight = rcDest.right;
								}
								::StretchBlt(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom, 
									hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
									lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
							}
						}
						else { // ytiled
							LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
							int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
							for( int i = 0; i < iTimes; ++i ) {
								LONG lDestTop = rcDest.top + lHeight * i;
								LONG lDestBottom = rcDest.top + lHeight * (i + 1);
								LONG lDrawHeight = lHeight;
								if( lDestBottom > rcDest.bottom ) {
									lDrawHeight -= lDestBottom - rcDest.bottom;
									lDestBottom = rcDest.bottom;
								}
								::StretchBlt(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop, 
									hCloneDC, rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top, \
									rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, lDrawHeight, SRCCOPY);                    
							}
						}
					}
				}

				// left-top
				if( rcCorners.left > 0 && rcCorners.top > 0 ) {
					rcDest.left = rc.left;
					rcDest.top = rc.top;
					rcDest.right = rcCorners.left;
					rcDest.bottom = rcCorners.top;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.left, rcBmpPart.top, rcCorners.left, rcCorners.top, SRCCOPY);
					}
				}
				// top
				if( rcCorners.top > 0 ) {
					rcDest.left = rc.left + rcCorners.left;
					rcDest.top = rc.top;
					rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
					rcDest.bottom = rcCorners.top;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.left + rcCorners.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
							rcCorners.left - rcCorners.right, rcCorners.top, SRCCOPY);
					}
				}
				// right-top
				if( rcCorners.right > 0 && rcCorners.top > 0 ) {
					rcDest.left = rc.right - rcCorners.right;
					rcDest.top = rc.top;
					rcDest.right = rcCorners.right;
					rcDest.bottom = rcCorners.top;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.right - rcCorners.right, rcBmpPart.top, rcCorners.right, rcCorners.top, SRCCOPY);
					}
				}
				// left
				if( rcCorners.left > 0 ) {
					rcDest.left = rc.left;
					rcDest.top = rc.top + rcCorners.top;
					rcDest.right = rcCorners.left;
					rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.left, rcBmpPart.top + rcCorners.top, rcCorners.left, rcBmpPart.bottom - \
							rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
					}
				}
				// right
				if( rcCorners.right > 0 ) {
					rcDest.left = rc.right - rcCorners.right;
					rcDest.top = rc.top + rcCorners.top;
					rcDest.right = rcCorners.right;
					rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.right - rcCorners.right, rcBmpPart.top + rcCorners.top, rcCorners.right, \
							rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom, SRCCOPY);
					}
				}
				// left-bottom
				if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
					rcDest.left = rc.left;
					rcDest.top = rc.bottom - rcCorners.bottom;
					rcDest.right = rcCorners.left;
					rcDest.bottom = rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.left, rcBmpPart.bottom - rcCorners.bottom, rcCorners.left, rcCorners.bottom, SRCCOPY);
					}
				}
				// bottom
				if( rcCorners.bottom > 0 ) {
					rcDest.left = rc.left + rcCorners.left;
					rcDest.top = rc.bottom - rcCorners.bottom;
					rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
					rcDest.bottom = rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.left + rcCorners.left, rcBmpPart.bottom - rcCorners.bottom, \
							rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right, rcCorners.bottom, SRCCOPY);
					}
				}
				// right-bottom
				if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
					rcDest.left = rc.right - rcCorners.right;
					rcDest.top = rc.bottom - rcCorners.bottom;
					rcDest.right = rcCorners.right;
					rcDest.bottom = rcCorners.bottom;
					rcDest.right += rcDest.left;
					rcDest.bottom += rcDest.top;
					if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
						rcDest.right -= rcDest.left;
						rcDest.bottom -= rcDest.top;
						::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
							rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom, rcCorners.right, \
							rcCorners.bottom, SRCCOPY);
					}
				}
			}
		}
		::SelectObject(hCloneDC, hOldBitmap);
		::DeleteDC(hCloneDC);
	}

	void CIRenderGDI::GetTextBounds( int& nWidth,int& nHeight,TFontInfo *pFontInfo,LPCTSTR pstrText,UINT uStyle )
	{
		FontFamily fontfamily;
		Font myFont(m_hDcOffScreen,pFontInfo->hFont);
		myFont.GetFamily(&fontfamily);
		StringFormat format;
		StringAlignment align = StringAlignmentNear; 
		if ((uStyle & DT_CENTER) == DT_CENTER)
			align = StringAlignmentCenter; 
		else if ((uStyle & DT_RIGHT) == DT_RIGHT)
			align = StringAlignmentFar;
		format.SetAlignment(align);
		format.SetLineAlignment(StringAlignmentCenter);
		if(uStyle & DT_END_ELLIPSIS)format.SetTrimming( StringTrimmingEllipsisCharacter );
		if(uStyle & DT_SINGLELINE) format.SetFormatFlags( StringFormatFlagsNoWrap );
		RectF rcBound;
		Graphics gs(m_hDcOffScreen);
		gs.MeasureString(pstrText,-1,&myFont,PointF(0,0),&format,&rcBound);
		nWidth = rcBound.Width;
		nHeight = rcBound.Height;
	}

	void CIRenderGDI::DrawText( TFontInfo *pFontInfo, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor,UINT uStyle )
	{
		if(!m_hDcOffScreen || !pstrText || !pFontInfo) return;
		::SetBkMode(m_hDcOffScreen, TRANSPARENT);
		if((dwTextColor >= 0xff000000 || dwTextColor <= 0x00ffffff) && !m_bLayered && !m_bGlass)
		{
			::SetTextColor(m_hDcOffScreen, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
			HFONT hOldFont = (HFONT)::SelectObject(m_hDcOffScreen, pFontInfo->hFont);
			::DrawText(m_hDcOffScreen, pstrText, -1, &rc, uStyle | DT_NOPREFIX|DT_WORDBREAK|DT_EDITCONTROL);
			::SelectObject(m_hDcOffScreen, hOldFont);
		}
		else
		{
			Graphics gs(m_hDcOffScreen);
			Font myFont(m_hDcOffScreen,pFontInfo->hFont);
			RectF layoutRect(static_cast<REAL>(rc.left),static_cast<REAL>(rc.top+1),static_cast<REAL>(rc.right - rc.left),static_cast<REAL>(rc.bottom - rc.top));
			StringFormat format;
			StringAlignment align = StringAlignmentNear; 
			if ((uStyle & DT_CENTER) == DT_CENTER)
				align = StringAlignmentCenter; 
			else if ((uStyle & DT_RIGHT) == DT_RIGHT)
				align = StringAlignmentFar;
			format.SetAlignment(align);
			format.SetLineAlignment(StringAlignmentCenter);
			if(uStyle & DT_END_ELLIPSIS)format.SetTrimming( StringTrimmingEllipsisCharacter );
			if(uStyle & DT_SINGLELINE) format.SetFormatFlags( StringFormatFlagsNoWrap );
			Color textcolor(dwTextColor);
			if(textcolor.GetAlpha() == 255 || textcolor.GetAlpha() == 0)
				textcolor = Color(254,textcolor.GetR(),textcolor.GetG(),textcolor.GetB());
			SolidBrush textBrush(textcolor);
			gs.SetSmoothingMode(SmoothingModeAntiAlias);
// 			if(m_bLayered)
// 				gs.SetTextRenderingHint(TextRenderingHint(TextRenderingHintAntiAlias));
			gs.DrawString(pstrText,-1,&myFont,layoutRect,&format,&textBrush);
		}
	}

	void CIRenderGDI::DrawColor( const RECT& rc, DWORD color )
	{
		if( (color >= 0xFF000000 || color <= 0x00FFFFFF) && !m_bLayered && !m_bGlass)
		{
			::SetBkColor(m_hDcOffScreen, RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
			::ExtTextOut(m_hDcOffScreen, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
		}
		else
		{
			Graphics gs(m_hDcOffScreen);
			Color clr(color);
			if(clr.GetA() == 255 || clr.GetA() == 0)
				clr = Color(254,clr.GetR(),clr.GetG(),clr.GetB());
			SolidBrush brush(clr);
			gs.FillRectangle((Brush*)&brush,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top);
		}
	}

	void CIRenderGDI::DrawGradient( const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical )
	{
		Graphics gs(m_hDcOffScreen);
		Point ptStart,ptEnd;
		if(bVertical)
		{
			ptStart.X = ptEnd.X = rc.left;
			ptStart.Y = rc.top;
			ptEnd.Y = rc.bottom;
		}
		else
		{
			ptStart.Y = ptEnd.Y = rc.top;
			ptStart.X = rc.left;
			ptEnd.X = rc.right;
		}
		Color clrStart(dwFirst);
		Color clrEnd(dwSecond);
		LinearGradientBrush linGrBrush(ptStart,ptEnd,clrStart,clrEnd);
		int nX,nY,nWidth,nHeight;
		nX = rc.left < rc.right ? rc.left : rc.right;
		nY = rc.top < rc.bottom ? rc.top : rc.bottom;
		nWidth = abs(rc.right-rc.left);
		nHeight = abs(rc.bottom-rc.top);
		gs.FillRectangle(&linGrBrush,nX,nY,nWidth,nHeight);
	}

	void CIRenderGDI::DrawLine( const RECT& rc, int nSize, DWORD dwPenColor,int nStyle /*= PS_SOLID*/ )
	{
		if(!m_hDcOffScreen)
			return;
		if((dwPenColor >= 0xFF000000 || dwPenColor <= 0x00ffffff) && !m_bLayered && !m_bGlass)
		{
			LOGPEN lg;
			lg.lopnColor = RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor));
			lg.lopnStyle = nStyle;
			lg.lopnWidth.x = nSize;
			HPEN hPen = CreatePenIndirect(&lg);
			HPEN hOldPen = (HPEN)::SelectObject(m_hDcOffScreen, hPen);
			POINT ptTemp = { 0 };
			::MoveToEx(m_hDcOffScreen, rc.left, rc.top, &ptTemp);
			::LineTo(m_hDcOffScreen, rc.right, rc.bottom);
			::SelectObject(m_hDcOffScreen, hOldPen);
			::DeleteObject(hPen);
		}
		else
		{
			Graphics gs(m_hDcOffScreen);
			Pen pen((ARGB)dwPenColor,(REAL)nSize);
			pen.SetDashStyle(DashStyle(nStyle));
			gs.DrawLine(&pen,Point(rc.left,rc.top),Point(rc.right,rc.bottom));
		}
	}

	void CIRenderGDI::DrawRect( const RECT& rc, int nSize, DWORD dwPenColor )
	{
		if(!m_hDcOffScreen)
			return;
		if((dwPenColor >= 0xff000000 || dwPenColor <= 0x00ffffff) && !m_bLayered && !m_bGlass)
		{
			HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
			HPEN hOldPen = (HPEN)::SelectObject(m_hDcOffScreen, hPen);
			::SelectObject(m_hDcOffScreen, ::GetStockObject(HOLLOW_BRUSH));
			::Rectangle(m_hDcOffScreen, rc.left, rc.top, rc.right, rc.bottom);
			::SelectObject(m_hDcOffScreen, hOldPen);
			::DeleteObject(hPen);
		}
		else
		{
			Graphics gs(m_hDcOffScreen);
			Pen pen((ARGB)dwPenColor,(REAL)nSize);
			gs.DrawRectangle(&pen,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top-1);
		}
	}

	void CIRenderGDI::DrawRoundRect( const RECT& rc, int width, int height, int nSize, DWORD dwPenColor )
	{
		if(!m_hDcOffScreen)
			return;
		if((dwPenColor >= 0xff000000 || dwPenColor <= 0x00ffffff) && !m_bLayered && !m_bGlass)
		{
			HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
			HPEN hOldPen = (HPEN)::SelectObject(m_hDcOffScreen, hPen);
			::SelectObject(m_hDcOffScreen, ::GetStockObject(HOLLOW_BRUSH));
			::RoundRect(m_hDcOffScreen, rc.left, rc.top, rc.right, rc.bottom, width, height);
			::SelectObject(m_hDcOffScreen, hOldPen);
			::DeleteObject(hPen);
		}
		else
		{
			Graphics gs(m_hDcOffScreen);
			GraphicsPath path;
			path.StartFigure();
			Gdiplus::Rect rect(rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top);
			REAL rwidth = rect.Width- 1.5f;
			REAL rheight = rect.Height - 1.0f;
			RectF r((REAL)rect.X, (REAL)rect.Y, rwidth, rheight);
			REAL dia = (REAL)width;
			if(dia > r.Width)     dia = r.Width;
			if(dia > r.Height)     dia = r.Height;
			RectF Corner(r.X, r.Y, dia, dia);
			path.AddArc(Corner, 180, 90);
			if(dia == 20)
			{
				Corner.Width += 1; 
				Corner.Height += 1; 
				r.Width -=1; r.Height -= 1;
			}
			Corner.X += (r.Width - dia - 1);
			path.AddArc(Corner, 270, 90);    
			Corner.Y += (r.Height - dia - 1);
			path.AddArc(Corner,   0, 90);    
			Corner.X -= (r.Width - dia - 1);
			path.AddArc(Corner, 90, 90);
			path.CloseFigure();
			gs.SetSmoothingMode(SmoothingModeHighQuality);
			Pen pen(dwPenColor, (REAL)nSize);
			gs.DrawPath(&pen, &path);
		}
	}

	void CIRenderGDI::DrawCircle( int nCenterX,int nCenterY,int nRadius,DWORD dwColor )
	{

	}

	void CIRenderGDI::FillCircle( int nCenterX,int nCenterY,int nRadius,DWORD dwColor )
	{

	}

	void CIRenderGDI::DrawArc( int nCenterX,int nCenterY,int nRadius,float fStartAngle,float fSweepAngle ,DWORD dwColor)
	{

	}

}