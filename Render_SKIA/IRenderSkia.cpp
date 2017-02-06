#include "StdAfx.h"
#include "IRenderSkia.h"


namespace UiLib
{
	CIRenderCore * GetRenderInstance()
	{
		ATLTRACE(_T("LoadSuccess"));
		CIRenderSKIA *pSkiaRender = new CIRenderSKIA();
		g_uRef++;
		return pSkiaRender;
	}

	bool ReleaseRef()
	{
		g_uRef--;
		if(0 == g_uRef)
			return true;
		return false;
	}

	CIRenderSKIA::CIRenderSKIA()
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
		m_pSkBakCanvas = NULL;
	}

	CIRenderSKIA::~CIRenderSKIA()
	{
		SAFE_DELETE(m_pDefaultFontInfo);
		Gdiplus::GdiplusShutdown(m_Token);
		ReleaseRenderObj();
	}

	void CIRenderSKIA::SetPaintWindow( HWND hWnd )
	{
		if(hWnd)
		{
			if(m_hDcPaintWnd)
				::ReleaseDC(m_hWnd,m_hDcPaintWnd);
			m_hWnd = hWnd;
			m_hDcPaintWnd = ::GetDC(m_hWnd);
		}
	}

	HWND CIRenderSKIA::GetPaintWindow()
	{
		return m_hWnd;
	}

	void CIRenderSKIA::ReleaseRenderObj()
	{
		SAFE_DELETE(m_pSkBakCanvas);
		if(m_hDcOffScreen)
		{
			if(m_hBmpOld)
				::SelectObject(m_hDcOffScreen,m_hBmpOld);
			SAFE_DELETEDC(m_hDcOffScreen);
		}
		SAFE_DELETEOBJ(m_hBmpOffScreen);
		m_pOffBmpPixel = NULL;
	}

	int CIRenderSKIA::GetOffBmpPitch()
	{
		return m_nPitch;
	}

	void * CIRenderSKIA::GetOffBmpPiexl()
	{
		return m_pOffBmpPixel;
	}

	void CIRenderSKIA::SetWindowGlass( bool bGlass /*= true*/ )
	{
		m_bGlass = bGlass;
	}

	bool CIRenderSKIA::IsWindowGlass()
	{
		return m_bGlass;
	}

	void CIRenderSKIA::SetWindowLayered( bool bLayered /*= true*/ )
	{
		m_bLayered = bLayered;
	}

	bool CIRenderSKIA::IsWindowLayered()
	{
		return m_bLayered;
	}

	void CIRenderSKIA::TranslateRO(float fTransX,float fTransY)
	{
		if(m_pSkBakCanvas)
			m_pSkBakCanvas->translate(SkScalar(fTransX),SkScalar(fTransY));
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

	void CIRenderSKIA::BeginDraw( RECT *rc /*= NULL*/ )
	{
		if(m_bLayered)
		{
			DWORD dwExStyle=GetWindowLongPtr(m_hWnd,GWL_EXSTYLE);
			if((dwExStyle&WS_EX_LAYERED)!=0x80000)
				SetWindowLongPtr(m_hWnd,GWL_EXSTYLE,dwExStyle^WS_EX_LAYERED);
		}
		if(!m_hBmpOffScreen &&  m_hWnd)
		{
			RECT rcClient = { 0 };
			::GetClientRect(m_hWnd, &rcClient);
			int nRcClientWidth = rcClient.right - rcClient.left;
			int nRcClientHeight = rcClient.bottom - rcClient.top;
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
			SkImageInfo info = SkImageInfo::Make(nRcClientWidth, nRcClientHeight, kN32_SkColorType, kPremul_SkAlphaType);
			m_SkBakBitmap.allocPixels(info);
			m_SkBakBitmap.setPixels(m_pOffBmpPixel);
			SAFE_DELETE(m_pSkBakCanvas);
			m_pSkBakCanvas = new SkCanvas(m_SkBakBitmap);
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

	void CIRenderSKIA::EndDraw( PAINTSTRUCT &ps )
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

	void CIRenderSKIA::SetRenderObj( TRenderObj* pRenderObj,int nCx,int nCy)
	{
		//failed here;  surface DC can not attach with DIB,we can't turn DIB to skia for Drawing;
		//Use Surface to SkBitmap
		RECT rcClient = { 0 };
		::GetClientRect(m_hWnd, &rcClient);

		int nRcClientWidth = rcClient.right - rcClient.left;
		int nRcClientHeight = rcClient.bottom - rcClient.top;
		if(nCx > 0)
			nRcClientWidth = nCx;
		if(nCy > 0)
			nRcClientHeight = nCy;

		if(m_hDcOffScreen)
		{
			if(m_hBmpOld)
				::SelectObject(m_hDcOffScreen,m_hBmpOld);
			SAFE_DELETEDC(m_hDcOffScreen);
		}
		m_hDcOffScreen = pRenderObj->hDC;
		if(pRenderObj->pBits)
		{
			m_pOffBmpPixel = pRenderObj->pBits;
			SkImageInfo info = SkImageInfo::Make(nRcClientWidth, nRcClientHeight, kN32_SkColorType, kPremul_SkAlphaType);
			m_SkBakBitmap.allocPixels(info);
			m_SkBakBitmap.setPixels(pRenderObj->pBits);
			SAFE_DELETE(m_pSkBakCanvas);
			m_pSkBakCanvas = new SkCanvas(m_SkBakBitmap);
		}
		else
		{
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
			SkImageInfo info = SkImageInfo::Make(nRcClientWidth, nRcClientHeight, kN32_SkColorType, kPremul_SkAlphaType);
			m_SkBakBitmap.allocPixels(info);
			m_SkBakBitmap.setPixels(m_pOffBmpPixel);
			SAFE_DELETE(m_pSkBakCanvas);
			m_pSkBakCanvas = new SkCanvas(m_SkBakBitmap);
			m_hBmpOld = (HBITMAP)::SelectObject(m_hDcOffScreen,m_hBmpOffScreen);
		}
	}

	HDC CIRenderSKIA::GetRenderHDC( BOOL bOffScreen /*= TRUE*/ )
	{
		if(bOffScreen)
			return m_hDcOffScreen;
		else
			return m_hDcPaintWnd;
	}

	void CIRenderSKIA::FreeRenderHDC()
	{

	}

	void CIRenderSKIA::Clear()
	{

	}

	void CIRenderSKIA::Clear( const RECT &rcClear )
	{

	}

	void CIRenderSKIA::GetClipBox(RECT &rcClip)
	{
		SkRect skRcClip;
		skRcClip.iset(0,0,0,0);
		if(m_pSkBakCanvas->getClipBounds(&skRcClip))
		{
			rcClip.left = (int)skRcClip.left()+1;
			rcClip.top = (int)skRcClip.top()+1;
			rcClip.right = (int)skRcClip.right()-1;
			rcClip.bottom = (int)skRcClip.bottom()-1;
		}
	}

	void CIRenderSKIA::PushClip( const RECT& clipRect,RECT *rcRound,int nRoundX,int nRoundY,ClipOpt _ClipOpt)
	{
		if(!m_pSkBakCanvas) return;
		SkRect rcClip;
		rcClip.iset(clipRect.left,clipRect.top,clipRect.right,clipRect.bottom);

		SkRegion::Op opClip = SkRegion::kIntersect_Op;
		switch(_ClipOpt)
		{
		case OP_REPLACE:
			opClip = SkRegion::kReplace_Op;
			break;
		case OP_XOR:
			opClip = SkRegion::kXOR_Op;
			break;
		case OP_DIFF:
			opClip = SkRegion::kDifference_Op;
			break;
		case OP_AND:
			opClip = SkRegion::kIntersect_Op;
			break;
		case OP_OR:
			opClip = SkRegion::kUnion_Op;
			break;
		default:
			break;
		}
		m_pSkBakCanvas->clipRect(rcClip,opClip);
		if(rcRound)
		{
			SkPath  path;
			SkRect rcItem;
			rcItem.iset(rcRound->left,rcRound->top,rcRound->right,rcRound->bottom);
			path.addRoundRect(rcItem,(SkScalar)nRoundX,(SkScalar)nRoundY);
			m_pSkBakCanvas->clipPath(path,opClip);
		}
	}

	void CIRenderSKIA::PopClip( const RECT & clipRect )
	{
		if(!m_pSkBakCanvas) return;
		SkRect rcClip;
		rcClip.iset(clipRect.left,clipRect.top,clipRect.right,clipRect.bottom);
		m_pSkBakCanvas->clipRect(rcClip,SkRegion::kReplace_Op);
	}

	void CIRenderSKIA::DCPushClip(HDC hDC)
	{
		if(!m_pSkBakCanvas) return;
		RECT clipRect = {0};
		SkRect skRcClip;
		skRcClip.iset(0,0,0,0);
		m_pSkBakCanvas->getClipBounds(&skRcClip);
		clipRect.left = (int)skRcClip.left()+1;
		clipRect.top = (int)skRcClip.top()+1;
		clipRect.right = (int)skRcClip.right()-1;
		clipRect.bottom = (int)skRcClip.bottom()-1;

		XFORM xform = {0};
		xform.eM11 = 1;
		xform.eM12 = 0;
		xform.eM21 = 0;
		xform.eM22 = 1;
		xform.eDx = 0;
		xform.eDy = 0;
		SetGraphicsMode(m_hDcOffScreen,GM_ADVANCED);
		GetWorldTransform(m_hDcOffScreen,&xform);
		OffsetRect(&clipRect,(int)xform.eDx,(int)xform.eDy);
		HRGN rgnClip = ::CreateRectRgnIndirect(&clipRect);
		::SelectClipRgn(m_hDcOffScreen,rgnClip);
		::DeleteObject(rgnClip);
	}

	void CIRenderSKIA::DCPopClip(HDC hDC)
	{
		::ExtSelectClipRgn(m_hDcOffScreen,NULL, RGN_COPY);
	}

	void CIRenderSKIA::Resize()
	{

	}


	TImageInfo* CIRenderSKIA::LoadImage( TImagePrimData &ImagePrimData )
	{
		Bitmap *pBitmapAttach = (Bitmap *)(ImagePrimData.pImage);
		if(!pBitmapAttach) return NULL;
		TGDIPImageInfo *pImageInfo = new TGDIPImageInfo;
		pImageInfo->Attach(pBitmapAttach);
		TSKIAImageInfo* data = new TSKIAImageInfo;
		data->skImg = pImageInfo;
		data->nX = pBitmapAttach->GetWidth();
		data->nY = pBitmapAttach->GetHeight();
		return data;
	}

	TFontInfo* CIRenderSKIA::GetDefaultFontInfo()
	{
		if( m_pDefaultFontInfo&&m_pDefaultFontInfo->tm.tmHeight == 0 ) 
		{
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaintWnd, m_pDefaultFontInfo->hFont);
			::GetTextMetrics(m_hDcPaintWnd, &m_pDefaultFontInfo->tm);
			::SelectObject(m_hDcPaintWnd, hOldFont);
		}
		return m_pDefaultFontInfo;
	}

	void CIRenderSKIA::SetDefaultFont(TFontPrimData &FontPrimData )
	{
		SAFE_DELETE(m_pDefaultFontInfo);
		m_pDefaultFontInfo = new TFontInfo;
		m_pDefaultFontInfo->bBold = FontPrimData.bBold;
		m_pDefaultFontInfo->bItalic = FontPrimData.bItalic;
		m_pDefaultFontInfo->bUnderline = FontPrimData.bUnderline;
		m_pDefaultFontInfo->hFont = FontPrimData.hFont;
		m_pDefaultFontInfo->iSize = FontPrimData.iSize;
		_tcscpy_s(m_pDefaultFontInfo->sFontName, FontPrimData.sFontName);
		m_pDefaultFontInfo->tm = FontPrimData.tm;
		if(m_hDcPaintWnd)
		{
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaintWnd, m_pDefaultFontInfo->hFont);
			::GetTextMetrics(m_hDcPaintWnd, &(m_pDefaultFontInfo->tm));
			::SelectObject(m_hDcPaintWnd,hOldFont);
		}
	}

	TFontInfo* CIRenderSKIA::AddFont( TFontPrimData &FontPrimData )
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

	void CIRenderSKIA::DrawImage( const TImageInfo* ImgDate, const RECT& rc, const RECT& rcPaint, const RECT& rcBmpPart, const RECT& rcCorners, bool alphaChannel, BYTE uFade /*= 255*/, bool hole /*= false*/, bool xtiled /*= false*/, bool ytiled /*= false*/ )
	{
		if(!ImgDate)
			return;
		SkBitmap *pBitmap =((TSKIAImageInfo *)ImgDate)->GetSkBitmap();

		SkPaint _paint;
		if(uFade < 255)
			_paint.setAlpha(uFade);

		RECT rcTemp = {0};
		RECT rcDest = {0};
		SkIRect srcRect;
		SkRect  dstRect;
		SkScalar SkX1,SkY1,skWidth,skHeight;
		int nX1,nY1,nWidth,nHeight;
#pragma region Center
		if(!hole)
		{
			nX1 = rcDest.left = rc.left + rcCorners.left;
			nY1 = rcDest.top = rc.top + rcCorners.top;
			nWidth = rc.right - rc.left - rcCorners.left - rcCorners.right;
			nHeight = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
			rcDest.right = rcDest.left+nWidth;
			rcDest.bottom = rcDest.top+nHeight;
			SkX1 = SkScalar(nX1);
			SkY1 = SkScalar(nY1);
			skWidth = SkScalar(nWidth);
			skHeight = SkScalar(nHeight);
			dstRect.setLTRB(SkX1,SkY1,SkX1+skWidth,SkY1+skHeight);

			nX1 = rcBmpPart.left + rcCorners.left;
			nY1 = rcBmpPart.top + rcCorners.top;
			nWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
			nHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
			srcRect.setLTRB(nX1,nY1,nX1+nWidth,nY1+nHeight);
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) 
			{
				if( !xtiled && !ytiled )
					m_pSkBakCanvas->drawBitmapRect(*pBitmap,&srcRect,dstRect,&_paint);
				else if( xtiled && ytiled)
				{
					LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
					LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
					int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
					int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
					for( int j = 0; j < iTimesY; ++j )
					{
						LONG lDestTop = rcDest.top + lHeight * j;
						LONG lDestBottom = rcDest.top + lHeight * (j + 1);
						LONG lDrawHeight = lHeight;
						if( lDestBottom > rcDest.bottom )
						{
							lDrawHeight -= lDestBottom - rcDest.bottom;
							lDestBottom = rcDest.bottom;
						}
						for( int i = 0; i < iTimesX; ++i ) 
						{
							int lDestLeft = rcDest.left + lWidth * i;
							int lDestRight = rcDest.left + lWidth * (i + 1);
							int lDrawWidth = lWidth;
							if( lDestRight > rcDest.right )
							{
								lDrawWidth -= lDestRight - rcDest.right;
								lDestRight = rcDest.right;
							}
							SkX1 = SkScalar(lDestLeft);
							SkY1 = SkScalar(lDestTop);
							skWidth = SkScalar(lDestRight - lDestLeft);
							skHeight = SkScalar( lDestBottom - lDestTop);
							dstRect.setLTRB(SkX1,SkY1,SkX1+skWidth,SkY1+skHeight);
							srcRect.setLTRB(nX1,nY1,nX1+lDrawWidth,nY1+lDrawHeight);
							m_pSkBakCanvas->drawBitmapRect(*pBitmap,&srcRect,dstRect,&_paint);
						}
					}
				}
				else if( xtiled)
				{
					LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
					int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
					for( int i = 0; i < iTimes; ++i )
					{
						int lDestLeft = rcDest.left + lWidth * i;
						int lDestRight = rcDest.left + lWidth * (i + 1);
						int lDrawWidth = lWidth;
						if( lDestRight > rcDest.right ) 
						{
							lDrawWidth -= lDestRight - rcDest.right;
							lDestRight = rcDest.right;
						}
						SkX1 = SkScalar(lDestLeft);
						SkY1 = SkScalar(rcDest.top);
						skWidth = SkScalar(lDestRight - lDestLeft);
						skHeight = SkScalar(rcDest.bottom);
						dstRect.setLTRB(SkX1,SkY1,SkX1+skWidth,SkY1+skHeight);
						srcRect.setLTRB(nX1,nY1,nX1+lDrawWidth,nY1+nHeight);
						m_pSkBakCanvas->drawBitmapRect(*pBitmap,&srcRect,dstRect,&_paint);
					}
				}
				else
				{
					LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
					int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
					for( int i = 0; i < iTimes; ++i ) 
					{
						int lDestTop = rcDest.top + lHeight * i;
						int lDestBottom = rcDest.top + lHeight * (i + 1);
						int lDrawHeight = lHeight;
						if( lDestBottom > rcDest.bottom ) 
						{
							lDrawHeight -= lDestBottom - rcDest.bottom;
							lDestBottom = rcDest.bottom;
						}
						SkX1 = SkScalar(rcDest.left);
						SkY1 = SkScalar(lDestTop);
						skWidth = SkScalar(rcDest.right);
						skHeight = SkScalar(lDestBottom - lDestTop);
						dstRect.setLTRB(SkX1,SkY1,SkX1+skWidth,SkY1+skHeight);
						srcRect.setLTRB(nX1,nY1,nWidth,nY1+lDrawHeight);
						m_pSkBakCanvas->drawBitmapRect(*pBitmap,&srcRect,dstRect,&_paint);
					}
				}
			}
		}
#pragma endregion
#pragma region Left-Top
		if( rcCorners.left > 0 && rcCorners.top > 0 ) 
		{
			rcDest.left = rc.left;
			rcDest.top = rc.top;
			rcDest.right = rcCorners.left;
			rcDest.bottom = rcCorners.top;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) 
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				SkX1 = SkScalar(rcDest.left);
				SkY1 = SkScalar(rcDest.top);
				skWidth = SkScalar(rcDest.right);
				skHeight = SkScalar(rcDest.bottom);
				dstRect.setLTRB(SkX1,SkY1,SkX1+skWidth,SkY1+skHeight);
				srcRect.setLTRB(rcBmpPart.left,rcBmpPart.top,rcBmpPart.left+rcCorners.left,rcBmpPart.top+rcCorners.top);
				m_pSkBakCanvas->drawBitmapRect(*pBitmap,&srcRect,dstRect,&_paint);
			}
		}
#pragma endregion
#pragma region Top
		if( rcCorners.top > 0 ) 
		{
			rcDest.left = rc.left + rcCorners.left;
			rcDest.top = rc.top;
			rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
			rcDest.bottom = rcCorners.top;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) 
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;

				SkX1 = SkScalar(rcDest.left);
				SkY1 = SkScalar(rcDest.top);
				skWidth = SkScalar(rcDest.right);
				skHeight = SkScalar(rcDest.bottom);
				dstRect.setLTRB(SkX1,SkY1,SkX1+skWidth,SkY1+skHeight);
				srcRect.setLTRB(rcBmpPart.left+rcCorners.left,rcBmpPart.top,rcBmpPart.right - rcCorners.right,rcBmpPart.top+rcCorners.top);
				m_pSkBakCanvas->drawBitmapRect(*pBitmap,&srcRect,dstRect,&_paint);
			}
		}
#pragma endregion
#pragma region Right-Top
		if( rcCorners.right > 0 && rcCorners.top > 0 ) 
		{
			rcDest.left = rc.right - rcCorners.right;
			rcDest.top = rc.top;
			rcDest.right = rcCorners.right;
			rcDest.bottom = rcCorners.top;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) 
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;

				SkX1 = SkScalar(rcDest.left);
				SkY1 = SkScalar(rcDest.top);
				skWidth = SkScalar(rcDest.right);
				skHeight = SkScalar(rcDest.bottom);
				dstRect.setLTRB(SkX1,SkY1,SkX1+skWidth,SkY1+skHeight);
				srcRect.setLTRB(rcBmpPart.right - rcCorners.right,rcBmpPart.top,rcBmpPart.right,rcBmpPart.top+rcCorners.top);
				m_pSkBakCanvas->drawBitmapRect(*pBitmap,&srcRect,dstRect,&_paint);
			}
		}
#pragma endregion
#pragma region Left
		if( rcCorners.left > 0 ) 
		{
			rcDest.left = rc.left;
			rcDest.top = rc.top + rcCorners.top;
			rcDest.right = rcCorners.left;
			rcDest.bottom = rc.bottom - rc.top - rcCorners.top - rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest)) 
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				SkX1 = SkScalar(rcDest.left);
				SkY1 = SkScalar(rcDest.top);
				skWidth = SkScalar(rcDest.right);
				skHeight = SkScalar(rcDest.bottom);
				dstRect.setLTRB(SkX1,SkY1,SkX1+skWidth,SkY1+skHeight);
				srcRect.setLTRB(rcBmpPart.left,rcBmpPart.top + rcCorners.top,rcBmpPart.left+rcCorners.left,rcBmpPart.bottom-rcCorners.bottom);
				m_pSkBakCanvas->drawBitmapRect(*pBitmap,&srcRect,dstRect,&_paint);
			}
		}
#pragma endregion
#pragma region Right
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
				SkX1 = SkScalar(rcDest.left);
				SkY1 = SkScalar(rcDest.top);
				skWidth = SkScalar(rcDest.right);
				skHeight = SkScalar(rcDest.bottom);
				dstRect.setLTRB(SkX1,SkY1,SkX1+skWidth,SkY1+skHeight);
				srcRect.setLTRB(rcBmpPart.right - rcCorners.right,rcBmpPart.top + rcCorners.top,rcBmpPart.right,rcBmpPart.bottom-rcCorners.bottom);
				m_pSkBakCanvas->drawBitmapRect(*pBitmap,&srcRect,dstRect,&_paint);
			}
		}
#pragma endregion
#pragma region Left-Bottom
		if( rcCorners.left > 0 && rcCorners.bottom > 0 ) 
		{
			rcDest.left = rc.left;
			rcDest.top = rc.bottom - rcCorners.bottom;
			rcDest.right = rcCorners.left;
			rcDest.bottom = rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				SkX1 = SkScalar(rcDest.left);
				SkY1 = SkScalar(rcDest.top);
				skWidth = SkScalar(rcDest.right);
				skHeight = SkScalar(rcDest.bottom);
				dstRect.setLTRB(SkX1,SkY1,SkX1+skWidth,SkY1+skHeight);
				srcRect.setLTRB(rcBmpPart.left,rcBmpPart.bottom-rcCorners.bottom,rcBmpPart.left+rcCorners.left,rcBmpPart.bottom);
				m_pSkBakCanvas->drawBitmapRect(*pBitmap,&srcRect,dstRect,&_paint);
			}
		}
#pragma endregion
#pragma region Bottom
		if( rcCorners.bottom > 0 ) 
		{
			rcDest.left = rc.left + rcCorners.left;
			rcDest.top = rc.bottom - rcCorners.bottom;
			rcDest.right = rc.right - rc.left - rcCorners.left - rcCorners.right;
			rcDest.bottom = rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) )
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;
				SkX1 = SkScalar(rcDest.left);
				SkY1 = SkScalar(rcDest.top);
				skWidth = SkScalar(rcDest.right);
				skHeight = SkScalar(rcDest.bottom);
				dstRect.setLTRB(SkX1,SkY1,SkX1+skWidth,SkY1+skHeight);
				srcRect.setLTRB(rcBmpPart.left+rcCorners.left,rcBmpPart.bottom-rcCorners.bottom,rcBmpPart.right-rcCorners.right,rcBmpPart.bottom);
				m_pSkBakCanvas->drawBitmapRect(*pBitmap,&srcRect,dstRect,&_paint);
			}
		}
#pragma endregion
#pragma region Right-Bottom
		if( rcCorners.right > 0 && rcCorners.bottom > 0 ) 
		{
			rcDest.left = rc.right - rcCorners.right;
			rcDest.top = rc.bottom - rcCorners.bottom;
			rcDest.right = rcCorners.right;
			rcDest.bottom = rcCorners.bottom;
			rcDest.right += rcDest.left;
			rcDest.bottom += rcDest.top;
			if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) 
			{
				rcDest.right -= rcDest.left;
				rcDest.bottom -= rcDest.top;

				SkX1 = SkScalar(rcDest.left);
				SkY1 = SkScalar(rcDest.top);
				skWidth = SkScalar(rcDest.right);
				skHeight = SkScalar(rcDest.bottom);
				dstRect.setLTRB(SkX1,SkY1,SkX1+skWidth,SkY1+skHeight);
				srcRect.setLTRB(rcBmpPart.right-rcCorners.right,rcBmpPart.bottom-rcCorners.bottom,rcBmpPart.right,rcBmpPart.bottom);
				m_pSkBakCanvas->drawBitmapRect(*pBitmap,&srcRect,dstRect,&_paint);
			}
		}
#pragma endregion
	}

	void CIRenderSKIA::GetTextBounds( int& nWidth,int& nHeight,TFontInfo *pFontInfo,LPCTSTR pstrText,UINT uStyle )
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
		nWidth = (int)rcBound.Width;
		nHeight = (int)rcBound.Height;
	}

	void CIRenderSKIA::DrawText( TFontInfo *pFontInfo, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor,UINT uStyle )
	{
		if(!m_hDcOffScreen || !pstrText || !pFontInfo) return;
		::SetBkMode(m_hDcOffScreen, TRANSPARENT);
		if((dwTextColor >= 0xff000000 || dwTextColor <= 0x00ffffff) && !m_bLayered && !m_bGlass)
		{
			::SetTextColor(m_hDcOffScreen, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
			HFONT hOldFont = (HFONT)::SelectObject(m_hDcOffScreen, pFontInfo->hFont);
			::DrawText(m_hDcOffScreen, pstrText, -1, &rc, uStyle | DT_NOPREFIX |DT_WORDBREAK|DT_EDITCONTROL);
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

	SkRect CIRenderSKIA::FromRECT( RECT rc )
	{
		SkRect Rc;
		Rc.iset(rc.left,rc.top,rc.right,rc.bottom);
		return Rc;
	}

	void CIRenderSKIA::DrawColor( const RECT& rc, DWORD color )
	{
		if(!m_pSkBakCanvas) return;
		SkRect rcPaint = FromRECT(rc);
		SkPaint SkClrPaint;
		SkClrPaint.setColor(color);
		SkClrPaint.setStyle(SkPaint::kFill_Style);
		m_pSkBakCanvas->drawRect(rcPaint,SkClrPaint);
	}

	void CIRenderSKIA::DrawGradient( const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical )
	{
		if(!m_pSkBakCanvas) return;
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
		SkPoint p = SkPoint::Make(SkIntToScalar(ptStart.X),SkIntToScalar(ptStart.Y));
		SkPoint q = SkPoint::Make(SkIntToScalar(ptEnd.X),SkIntToScalar(ptEnd.Y));
		SkPoint pts[] ={p,q};
		SkColor colors[] = {SkColor(dwFirst),SkColor(dwSecond)};
		SkShader *pLinerGradientShader = SkGradientShader::CreateLinear(pts,colors,NULL,2,SkShader::kMirror_TileMode);
		SkPaint skGradientPaint;
		skGradientPaint.setShader(pLinerGradientShader);
		pLinerGradientShader->unref();
		SkRect rcPaint = FromRECT(rc);
		m_pSkBakCanvas->drawRect(rcPaint,skGradientPaint);
	}

	void CIRenderSKIA::DrawLine( const RECT& rc, int nSize, DWORD dwPenColor,int nStyle /*= PS_SOLID*/ )
	{
		if(!m_pSkBakCanvas) return;
		SkPaint linePainter;
		linePainter.setColor(dwPenColor);
		linePainter.setStrokeWidth(SkScalar(nSize));
		linePainter.setStyle(SkPaint::kStroke_Style);
		if(rc.left != rc.right && rc.top != rc.bottom)
			linePainter.setAntiAlias(true);

		float fFixedX = 0.0f;
		float fFixedY = 0.0f;

		if(rc.left == rc.right && rc.left == 0)
			fFixedX+=0.5;
		if(rc.top == rc.bottom && rc.top == 0)
			fFixedY+=0.5;
		m_pSkBakCanvas->drawLine(SkScalar(rc.left)+fFixedX,SkScalar(rc.top)+fFixedY,SkScalar(rc.right)+fFixedX,SkScalar(rc.bottom)+fFixedY,linePainter);
	}

	void CIRenderSKIA::DrawRect( const RECT& rc, int nSize, DWORD dwPenColor )
	{
		if(!m_pSkBakCanvas) return;
		SkRect rcPaint = FromRECT(rc);
		rcPaint.fBottom -= 1.0f;
		rcPaint.fRight -= 1.0f;
		SkPaint SkClrPaint;
		SkClrPaint.setColor(dwPenColor);
		SkClrPaint.setStrokeWidth(SkIntToScalar(nSize));
		SkClrPaint.setStyle(SkPaint::kStroke_Style);
		m_pSkBakCanvas->drawRect(rcPaint,SkClrPaint);
	}

	void CIRenderSKIA::DrawRoundRect( const RECT& rc, int width, int height, int nSize, DWORD dwPenColor )
	{
		if(!m_pSkBakCanvas) return;
		SkRect rcPaint = FromRECT(rc);
		rcPaint.fBottom -= 1.0f;
		SkPaint SkClrPaint;
		SkClrPaint.setAntiAlias(true);
		SkClrPaint.setColor(dwPenColor);
		SkClrPaint.setStrokeWidth(SkIntToScalar(nSize));
		SkClrPaint.setStyle(SkPaint::kStroke_Style);
		m_pSkBakCanvas->drawRoundRect(rcPaint,SkIntToScalar(width),SkIntToScalar(height),SkClrPaint);
	}

	void CIRenderSKIA::DrawRoundDotRect( const RECT& rc,int width,int height,int nSize, DWORD dwPenColor )
	{
		if(!m_pSkBakCanvas) return;
		SkRect rcPaint = FromRECT(rc);
		rcPaint.fBottom -= 1.0f;
		SkPaint SkClrPaint;
		SkClrPaint.setAntiAlias(true);
		SkClrPaint.setColor(dwPenColor);
		SkClrPaint.setStrokeWidth(SkIntToScalar(nSize));
		SkClrPaint.setStyle(SkPaint::kStroke_Style);
		const SkScalar intervals[] = 
		{
			SkIntToScalar(1),
			SkIntToScalar(1),
		};
		SkClrPaint.setPathEffect(SkDashPathEffect::Create(intervals, 2, 0))->unref();
		m_pSkBakCanvas->drawRoundRect(rcPaint,SkIntToScalar(width),SkIntToScalar(height),SkClrPaint);
	}

	void CIRenderSKIA::FillRoundRect( const RECT& rc, int width, int height, int nSize, DWORD dwPenColor )
	{
		if(!m_pSkBakCanvas) return;
		SkRect rcPaint = FromRECT(rc);
		rcPaint.fBottom -= 1.0f;
		SkPaint SkClrPaint;
		SkClrPaint.setAntiAlias(true);
		SkClrPaint.setColor(dwPenColor);
		SkClrPaint.setStrokeWidth(SkIntToScalar(nSize));
		SkClrPaint.setStyle(SkPaint::kFill_Style);
		m_pSkBakCanvas->drawRoundRect(rcPaint,SkIntToScalar(width),SkIntToScalar(height),SkClrPaint);
	}

	void CIRenderSKIA::DrawCircle( int nCenterX,int nCenterY,int nRadius,DWORD dwColor)
	{
		if(!m_pSkBakCanvas) return;
		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setColor(dwColor);
		m_pSkBakCanvas->drawCircle(SkIntToScalar(nCenterX),SkIntToScalar(nCenterY),SkIntToScalar(nRadius),paint);
	}

	void CIRenderSKIA::FillCircle( int nCenterX,int nCenterY,int nRadius,DWORD dwColor )
	{
		if(!m_pSkBakCanvas) return;
		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setStyle(SkPaint::kFill_Style);
		paint.setColor(dwColor);
		m_pSkBakCanvas->drawCircle(SkIntToScalar(nCenterX),SkIntToScalar(nCenterY),SkIntToScalar(nRadius),paint);
	}

	void CIRenderSKIA::DrawArc( int nCenterX,int nCenterY,int nRadius,float fStartAngle,float fSweepAngle,DWORD dwColor)
	{
		if(!m_pSkBakCanvas) return;
		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setColor(dwColor);
		SkRect r = {SkIntToScalar(nCenterX-nRadius),SkIntToScalar(nCenterY-nRadius),SkIntToScalar(nCenterX+nRadius),SkIntToScalar(nCenterY+nRadius)};
		m_pSkBakCanvas->drawArc(r,fStartAngle,fSweepAngle,true,paint);
	}

}