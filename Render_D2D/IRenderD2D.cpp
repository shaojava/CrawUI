#include "StdAfx.h"
#include "IRenderD2D.h"
//Primedata type transfer warning 
#pragma warning (disable : 4244)

namespace UiLib
{
	using namespace D2D1;

	CIRenderCore * GetRenderInstance()
	{
		CIRenderD2D *pD2DRender = new CIRenderD2D();
		g_uRef++;
		return pD2DRender;
	}

	bool ReleaseRef()
	{
		g_uRef--;
		if(0 == g_uRef)
			return true;
		return false;
	}

	BOOL CheckRectEqual(RECT rc1,RECT rc2)
	{
		BOOL bEqual = TRUE;
		if(rc1.left != rc2.left || rc1.right != rc2.right || rc1.top != rc2.top || rc1.bottom != rc2.bottom)
			bEqual = FALSE;
		return bEqual;
	}

	CIRenderD2D::CIRenderD2D(void)
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
		m_nRoundX = 0;
		m_nRoundY = 0;
		m_nLayerRef = 0;
		m_bLayered = false;
		m_bGlass = false;

		m_IWICFactory = NULL;
		m_pD2d1Factory = NULL;
		m_pHwndRenderTarget = NULL;
		m_pCurRenderTarget = NULL;
		m_pDWriteFactory = NULL;
		m_pGDIRT = NULL;
		m_pbitmap = NULL;
		m_pbitmapRT = NULL;
		m_pRoundLayer = NULL;
		m_pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM  ,D2D1_ALPHA_MODE_PREMULTIPLIED);

		memset(&m_rcClip,0,sizeof(RECT));
		memset(&m_rcRound,0,sizeof(RECT));
	}

	CIRenderD2D::~CIRenderD2D(void)
	{
		Gdiplus::GdiplusShutdown(m_Token);
		if(m_hDcPaintWnd)
			::ReleaseDC(m_hWnd,m_hDcPaintWnd);
		ReleaseRenderObj();

		SAFE_DELETE(m_pDefaultFontInfo);
		SAFE_RELEASE(m_pDWriteFactory);
		SAFE_RELEASE(m_pD2d1Factory);
		SAFE_RELEASE(m_pHwndRenderTarget);
		SAFE_RELEASE(m_pGDIRT);
		SAFE_RELEASE(m_pbitmap);
		SAFE_RELEASE(m_pbitmapRT);
		SAFE_RELEASE(m_pRoundLayer);
	}

	void CIRenderD2D::SetPaintWindow( HWND hWnd )
	{
		if(hWnd)
		{
			if(m_hDcPaintWnd)
				::ReleaseDC(m_hWnd,m_hDcPaintWnd);
			m_hWnd = hWnd;
			m_hDcPaintWnd = ::GetDC(m_hWnd);
			CreateDeviceIndependentResource();
			CreateDeviceDependentResource();
		}
	}

	HWND CIRenderD2D::GetPaintWindow()
	{
		return m_hWnd;
	}

	UiLib::RenderWay CIRenderD2D::GetRenderType()
	{
		return D2D_RENDER;
	}

	BOOL CIRenderD2D::CreateDeviceIndependentResource()
	{
		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,&m_pD2d1Factory);
		if (SUCCEEDED(hr))
			hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(m_pDWriteFactory),reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
		if(!m_IWICFactory)
			m_IWICFactory.CoCreateInstance(CLSID_WICImagingFactory);
		return (hr == S_OK);
	}

	BOOL CIRenderD2D::CreateDeviceDependentResource()
	{
		RECT rcClient = { 0 };
		::GetClientRect(m_hWnd, &rcClient);
		if(!m_pD2d1Factory)
			return FALSE;
		if(m_pHwndRenderTarget)
			return TRUE;
		D2D1_SIZE_U size = SizeU(rcClient.right-rcClient.left,rcClient.bottom-rcClient.top);
		D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_SOFTWARE);
		props.pixelFormat = m_pixelFormat;
		props.usage = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
		//m_pD2d1Factory->CreateDCRenderTarget(&props,(ID2D1DCRenderTarget **)(&m_pHwndRenderTarget));
		HRESULT hr = m_pD2d1Factory->CreateHwndRenderTarget(&props,&(HwndRenderTargetProperties(m_hWnd,size)),(ID2D1HwndRenderTarget **)(&m_pHwndRenderTarget));
		m_pHwndRenderTarget->QueryInterface(__uuidof(ID2D1GdiInteropRenderTarget), (void**)&m_pGDIRT); 
		m_pHwndRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		m_pHwndRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);
		SAFE_RELEASE(m_pRoundLayer);
		m_pHwndRenderTarget->CreateLayer(&m_pRoundLayer);
		m_pCurRenderTarget = m_pHwndRenderTarget;
		return (hr == S_OK);
	}

	void CIRenderD2D::ReSizeRenderTarget( RECT rc )
	{
		if(m_pCurRenderTarget)
			((ID2D1HwndRenderTarget *)m_pCurRenderTarget)->Resize(SizeU(rc.right-rc.left,rc.bottom-rc.top));
	}

	void CIRenderD2D::PushAxisClip()
	{
		if(m_pCurRenderTarget )
			m_pCurRenderTarget->PushAxisAlignedClip(&D2D1::RectF((float)m_rcClip.left,(float)m_rcClip.top,(float)m_rcClip.right,(float)m_rcClip.bottom), D2D1_ANTIALIAS_MODE_ALIASED);
	}

	void CIRenderD2D::PopAxisClip()
	{
		if(m_pCurRenderTarget )
			m_pCurRenderTarget->PopAxisAlignedClip();
	}

	void CIRenderD2D::PushLayer()
	{
		if(!m_pCurRenderTarget) return;
		if(m_nRoundX != 0 || m_nRoundY != 0)
		{
			D2D1_ROUNDED_RECT rcRound = {D2D1::RectF((float)m_rcRound.left,(float)m_rcRound.top,(float)m_rcRound.right,(float)m_rcRound.bottom),m_nRoundX,m_nRoundY};
			ID2D1RoundedRectangleGeometry *pRoundRc = NULL;
			m_pD2d1Factory->CreateRoundedRectangleGeometry(rcRound,&pRoundRc);
			m_pCurRenderTarget->PushLayer(LayerParameters(D2D1::RectF((float)m_rcClip.left,(float)m_rcClip.top,(float)m_rcClip.right,(float)m_rcClip.bottom),pRoundRc),m_pRoundLayer);
			m_nLayerRef++;
		}
	}

	void CIRenderD2D::PopLayer()
	{
		if(!m_pCurRenderTarget) return;
		while(m_nLayerRef)
		{
			m_pCurRenderTarget->PopLayer();
			m_nLayerRef--;
		}
	}

	void CIRenderD2D::TranslateRO(float fTransX,float fTransY)
	{
		if (m_pCurRenderTarget)
			m_pCurRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(fTransX,fTransY));
	}

	void CIRenderD2D::BeginDraw( RECT *rc /*= NULL*/ )
	{
		if(m_hWnd)
		{
			if(m_bLayered)
			{
				DWORD dwExStyle=GetWindowLongPtr(m_hWnd,GWL_EXSTYLE);
				if((dwExStyle&WS_EX_LAYERED)!=0x80000)
					SetWindowLongPtr(m_hWnd,GWL_EXSTYLE,dwExStyle^WS_EX_LAYERED);
			}
			CreateDeviceDependentResource();
			if(!m_pCurRenderTarget) return;
			m_pCurRenderTarget->BeginDraw();
			m_rcClip.left = rc->left;
			m_rcClip.right = rc->right;
			m_rcClip.top = rc->top;
			m_rcClip.bottom = rc->bottom;
			PushAxisClip();
			if(m_bLayered)
 				m_pCurRenderTarget->Clear( D2D1::ColorF( 0, 0.0f ));
		}
	}

	void CIRenderD2D::EndDraw( PAINTSTRUCT &ps )
	{
		if(!m_pCurRenderTarget) return;
		
		PopAxisClip();
		if(m_nLayerRef)
			PopLayer();
		if(m_bLayered)
		{
			RECT rcWnd={0};
			::GetWindowRect(m_hWnd, &rcWnd);
			POINT pt = {rcWnd.left,rcWnd.top};
			RECT rcClient = { 0 };
			::GetClientRect(m_hWnd, &rcClient);
			SIZE szWindow = {rcClient.right - rcClient.left,rcClient.bottom - rcClient.top};
			POINT ptSrc = {0,0};
			HDC dc;
			m_pGDIRT->GetDC(D2D1_DC_INITIALIZE_MODE_COPY,&dc);
			BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
			::UpdateLayeredWindow(m_hWnd,NULL,&pt,&szWindow,dc,&ptSrc,0,&blendPixelFunction,ULW_ALPHA);
			m_pGDIRT->ReleaseDC(NULL);
		}
		HRESULT hr = m_pCurRenderTarget->EndDraw() ;
		if(hr == D2DERR_RECREATE_TARGET)
			ReleaseRenderObj();
	}

	void CIRenderD2D::SetRenderObj( TRenderObj* pRenderObj,int nCx,int nCy)
	{
		RECT rcClient = { 0 };
		m_pByte = (BYTE *)(pRenderObj->pBits);
		if(!m_pByte) return;
		::GetClientRect(m_hWnd, &rcClient);
		int nRcClientWidth = rcClient.right - rcClient.left;
		int nRcClientHeight = rcClient.bottom - rcClient.top;
		HRESULT hRes = S_FALSE;
		if(nCx < 0 && nCy < 0)
		{	// just copy the whole rendertarget;
			int nPitch = nRcClientWidth*4;
			if(!m_IWICFactory)
				m_IWICFactory.CoCreateInstance(CLSID_WICImagingFactory);
			SAFE_RELEASE(m_pbitmap);
			m_IWICFactory->CreateBitmap(nRcClientWidth,nRcClientHeight,GUID_WICPixelFormat32bppPBGRA,WICBitmapCacheOnLoad,&m_pbitmap);
			D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
			props.pixelFormat = m_pixelFormat;
			props.usage = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
			SAFE_RELEASE(m_pbitmapRT);
			hRes = m_pD2d1Factory->CreateWicBitmapRenderTarget(m_pbitmap,props,&m_pbitmapRT);
			if(hRes == S_OK)
			{
				ID2D1Bitmap *pCopyBitmap;
				const D2D1_BITMAP_PROPERTIES Pixelproperty = D2D1::BitmapProperties(m_pixelFormat);
				m_pbitmapRT->CreateBitmap(D2D1_SIZE_U(SizeU(nRcClientWidth,nRcClientHeight)),Pixelproperty,&pCopyBitmap);
				if(pCopyBitmap)
				{
					D2D1_POINT_2U destPoint = Point2U();
					D2D1_RECT_U srcRect = RectU(0,0,nRcClientWidth,nRcClientHeight);
					HRESULT hr = pCopyBitmap->CopyFromRenderTarget(&destPoint,m_pCurRenderTarget,&srcRect);
				}
				//	draw on surface
				m_pbitmapRT->BeginDraw();
				m_pbitmapRT->DrawBitmap(pCopyBitmap, D2D1::RectF(0,0,nRcClientWidth,nRcClientHeight),1.0f,D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,&D2D1::RectF(0,0,nRcClientWidth,nRcClientHeight));
				hRes = m_pbitmapRT->EndDraw();
				SAFE_RELEASE(pCopyBitmap);
				SAFE_RELEASE(m_pbitmapRT);
			}
			if(hRes == S_OK)
			{
				WICRect rcLock = {0,0,nRcClientWidth,nRcClientHeight};
				CComPtr<IWICBitmapLock> pBitmapLock;
				m_pbitmap->Lock(&rcLock,WICBitmapLockRead,&pBitmapLock);
				BYTE *pBitmapDate = NULL;
				UINT dwSize;
				pBitmapLock->GetDataPointer(&dwSize,&pBitmapDate);
				memcpy(pRenderObj->pBits,pBitmapDate,dwSize);
				SAFE_RELEASE(m_pbitmap);
				/*//for save;
				CComPtr<IWICStream> pStream;
				CComPtr<IWICBitmapEncoder> pEncoder;
				CComPtr<IWICBitmapFrameEncode> pFrameEncode;
				IWICFactory->CreateStream(&pStream);
				pStream->InitializeFromFilename(_T("C:\\ress\\d2d.png"), GENERIC_WRITE);
				IWICFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &pEncoder);
				pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
				pEncoder->CreateNewFrame(&pFrameEncode, NULL);
				pFrameEncode->Initialize(NULL);
				pFrameEncode->SetSize(nRcClientWidth, nRcClientHeight);
				WICPixelFormatGUID format = GUID_WICPixelFormat32bppPBGRA;
				pFrameEncode->SetPixelFormat(&format);
				pFrameEncode->WriteSource(bitmap, NULL);
				pFrameEncode->Commit();
				pEncoder->Commit();
				*/
			}
		}
		else
		{
			nRcClientWidth = nCx;
			nRcClientHeight = nCy;
			m_IWICFactory->CreateBitmap(nRcClientWidth,nRcClientHeight,GUID_WICPixelFormat32bppPBGRA,WICBitmapCacheOnLoad,&m_pbitmap);
			D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_SOFTWARE);
			props.pixelFormat = m_pixelFormat;
			props.usage = D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE;
			SAFE_RELEASE(m_pbitmapRT);
			hRes = m_pD2d1Factory->CreateWicBitmapRenderTarget(m_pbitmap,props,&m_pbitmapRT);
			SAFE_RELEASE(m_pGDIRT);
			m_pbitmapRT->QueryInterface(__uuidof(ID2D1GdiInteropRenderTarget), (void**)&m_pGDIRT); 
			m_pbitmapRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
			m_pbitmapRT->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);
			SAFE_RELEASE(m_pRoundLayer);
			m_pbitmapRT->CreateLayer(&m_pRoundLayer);
			m_pCurRenderTarget = m_pbitmapRT;
			m_pCurRenderTarget->BeginDraw();
		}
	}

	void CIRenderD2D::ReleaseRenderObj()
	{
		if(m_pbitmap)
		{
			m_pCurRenderTarget->EndDraw();
			UINT uWidth,uHeight;
			CComPtr<IWICBitmapLock> pBitmapLock;
			m_pbitmap->GetSize(&uWidth,&uHeight);
			WICRect rcLock = {0,0,uWidth,uHeight};
			m_pbitmap->Lock(&rcLock,WICBitmapLockRead,&pBitmapLock);
			BYTE *pBitmapDate = NULL;
			UINT dwSize;
			if(pBitmapLock)
				pBitmapLock->GetDataPointer(&dwSize,&pBitmapDate);
// 
// 			CComPtr<IWICStream> pStream;
// 			CComPtr<IWICBitmapEncoder> pEncoder;
// 			CComPtr<IWICBitmapFrameEncode> pFrameEncode;
// 			m_IWICFactory->CreateStream(&pStream);
// 			pStream->InitializeFromFilename(_T("C:\\d2d.png"), GENERIC_WRITE);
// 			m_IWICFactory->CreateEncoder(GUID_ContainerFormatPng, NULL, &pEncoder);
// 			pEncoder->Initialize(pStream, WICBitmapEncoderNoCache);
// 			pEncoder->CreateNewFrame(&pFrameEncode, NULL);
// 			if(pFrameEncode)
// 			{
// 				pFrameEncode->Initialize(NULL);
// 				pFrameEncode->SetSize(uWidth, uHeight);
// 				WICPixelFormatGUID format = GUID_WICPixelFormat32bppPBGRA;
// 				pFrameEncode->SetPixelFormat(&format);
// 				pFrameEncode->WriteSource(m_pbitmap, NULL);
// 				pFrameEncode->Commit();
// 				pEncoder->Commit();
// 			}
			if(pBitmapDate)
				memcpy(m_pByte,pBitmapDate,dwSize);
			SAFE_RELEASE(m_pbitmap);
			m_pCurRenderTarget = m_pHwndRenderTarget;
			SAFE_RELEASE(m_pGDIRT);
			m_pHwndRenderTarget->QueryInterface(__uuidof(ID2D1GdiInteropRenderTarget), (void**)&m_pGDIRT);
			SAFE_RELEASE(m_pRoundLayer);
			m_pCurRenderTarget->CreateLayer(&m_pRoundLayer);
		}
		
	}

	HDC CIRenderD2D::GetRenderHDC( BOOL bOffScreen /*= TRUE*/ )
	{
		if(!bOffScreen)
			return m_hDcPaintWnd;
		else
		{
			HDC hDC = NULL;
			if(m_pGDIRT)
				m_pGDIRT->GetDC(D2D1_DC_INITIALIZE_MODE_COPY,&hDC);
			D2D1_MATRIX_3X2_F matTrans;
			m_pCurRenderTarget->GetTransform(&matTrans);
			XFORM xform;
			xform.eM11 = matTrans._11;
			xform.eM12 = matTrans._12;
			xform.eM21 = matTrans._21;
			xform.eM22 = matTrans._22;
			xform.eDx = matTrans._31;
			xform.eDy = matTrans._32;
			SetGraphicsMode(hDC,GM_ADVANCED);
			SetWorldTransform(hDC,&xform);
			return hDC;
		}
	}

	void CIRenderD2D::FreeRenderHDC()
	{
		if(m_pGDIRT)
			m_pGDIRT->ReleaseDC(0);
	}

	void CIRenderD2D::Clear()
	{

	}

	void CIRenderD2D::Clear( const RECT &rcClear )
	{

	}

	void CIRenderD2D::Resize()
	{
		::GetClientRect(m_hWnd, &m_rcClip);
		ReSizeRenderTarget(m_rcClip);
	}

	void CIRenderD2D::GetClipBox(RECT &rcClip)
	{
		if(m_rcClip.left == m_rcClip.right || m_rcClip.top == m_rcClip.bottom)
			::GetClientRect(m_hWnd, &m_rcClip);
		rcClip = m_rcClip;
	}

	void CIRenderD2D::PushClip( const RECT& clipRect,RECT *rcRound,int nRoundX,int nRoundY,ClipOpt _ClipOpt)
	{
		PopAxisClip();
		switch(_ClipOpt)
		{
		case OP_REPLACE:
			m_rcClip = clipRect;
			break;
		case OP_AND:
			::IntersectRect(&m_rcClip,&m_rcClip,&clipRect);
			break;
		case OP_OR:
			::UnionRect(&m_rcClip,&m_rcClip,&clipRect);
			break;
		default:
			break;
		}
		if(rcRound)
		{
			PopLayer();
			m_rcRound.left = rcRound->left;
			m_rcRound.right = rcRound->right;
			m_rcRound.top = rcRound->top;
			m_rcRound.bottom = rcRound->bottom;
			m_nRoundX = nRoundX;
			m_nRoundY = nRoundY;
			PushLayer();
		}
		else
		{
			m_nRoundX = 0;
			m_nRoundY = 0;
		}
		PushAxisClip();
	}

	void CIRenderD2D::PopClip( const RECT & clipRect )
	{
		PopAxisClip();
		m_rcClip = clipRect;
		PopLayer();
		PushAxisClip();
	}

	TImageInfo* CIRenderD2D::LoadImage(TImagePrimData &ImagePrimData)
	{
		Bitmap *pImage = (Bitmap *)ImagePrimData.pImage;
		if(!pImage) return NULL;
		int nWidth = pImage->GetWidth();
		int nHeight= pImage->GetHeight();
		BitmapData bmdate;
		pImage->LockBits(&Gdiplus::Rect(0,0,nWidth,nHeight),ImageLockModeRead,PixelFormat32bppPARGB,&bmdate);
		pImage->UnlockBits(&bmdate);
		D2D1_BITMAP_PROPERTIES Pixelproperty = D2D1::BitmapProperties(m_pixelFormat);
		ID2D1Bitmap *pTexture = NULL;
		m_pCurRenderTarget->CreateBitmap(D2D1::SizeU(nWidth,nHeight),bmdate.Scan0,bmdate.Stride,Pixelproperty,&pTexture);
		SAFE_DELETE(pImage);
		if(pTexture)
		{
			TD2DImageInfo* pImgInfo = new TD2DImageInfo;
			pImgInfo->pTexture = pTexture;
			pImgInfo->nX = nWidth;
			pImgInfo->nY = nHeight;
			return pImgInfo;
		}
		else
			return NULL;
	}

	TFontInfo* CIRenderD2D::AddFont( TFontPrimData &FontPrimData )
	{
		TD2DFontInfo* pFontInfo = new TD2DFontInfo;
		pFontInfo->bBold = FontPrimData.bBold;
		pFontInfo->bItalic = FontPrimData.bItalic;
		pFontInfo->bUnderline = FontPrimData.bUnderline;
		pFontInfo->hFont = FontPrimData.hFont;
		pFontInfo->iSize = FontPrimData.iSize;
		_tcscpy_s(pFontInfo->sFontName, FontPrimData.sFontName);
		pFontInfo->tm = FontPrimData.tm;

		if( m_hDcPaintWnd ) {
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaintWnd, pFontInfo->hFont);
			::GetTextMetrics(m_hDcPaintWnd, &pFontInfo->tm);
			::SelectObject(m_hDcPaintWnd, hOldFont);
		}
		DWRITE_FONT_WEIGHT  fontWeight = DWRITE_FONT_WEIGHT_NORMAL;
		DWRITE_FONT_STYLE  fontStyle = DWRITE_FONT_STYLE_NORMAL;
		DWRITE_FONT_STRETCH  fontStretch = DWRITE_FONT_STRETCH_NORMAL;
		if(pFontInfo->bBold)
			fontWeight = DWRITE_FONT_WEIGHT_BOLD;
		if(pFontInfo->bItalic) 
			fontStyle = DWRITE_FONT_STYLE_ITALIC;
		m_pDWriteFactory->CreateTextFormat(pFontInfo->sFontName,NULL,fontWeight,fontStyle,fontStretch,(float)pFontInfo->iSize,L"", &pFontInfo->pTextFormat);
		return (TFontInfo *)pFontInfo;
	}

	TFontInfo* CIRenderD2D::GetDefaultFontInfo()
	{
		if( m_pDefaultFontInfo&&m_pDefaultFontInfo->tm.tmHeight == 0 ) 
		{
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaintWnd, m_pDefaultFontInfo->hFont);
			::GetTextMetrics(m_hDcPaintWnd, &m_pDefaultFontInfo->tm);
			::SelectObject(m_hDcPaintWnd, hOldFont);
		}
		return m_pDefaultFontInfo;
	}

	void CIRenderD2D::SetDefaultFont(TFontPrimData &FontPrimData)
	{
		SAFE_DELETE(m_pDefaultFontInfo);
		m_pDefaultFontInfo = new TD2DFontInfo;
		m_pDefaultFontInfo->bBold = FontPrimData.bBold;
		m_pDefaultFontInfo->bItalic = FontPrimData.bItalic;
		m_pDefaultFontInfo->bUnderline = FontPrimData.bUnderline;
		m_pDefaultFontInfo->hFont = FontPrimData.hFont;
		m_pDefaultFontInfo->iSize = FontPrimData.iSize;
		_tcscpy_s(m_pDefaultFontInfo->sFontName, FontPrimData.sFontName);
		m_pDefaultFontInfo->tm = FontPrimData.tm;
		DWRITE_FONT_WEIGHT  fontWeight = DWRITE_FONT_WEIGHT_NORMAL;
		DWRITE_FONT_STYLE  fontStyle = DWRITE_FONT_STYLE_NORMAL;
		DWRITE_FONT_STRETCH  fontStretch = DWRITE_FONT_STRETCH_NORMAL;
		if(m_pDefaultFontInfo->bBold)
			fontWeight = DWRITE_FONT_WEIGHT_BOLD;
		if(m_pDefaultFontInfo->bItalic) 
			fontStyle = DWRITE_FONT_STYLE_ITALIC;
		m_pDWriteFactory->CreateTextFormat(m_pDefaultFontInfo->sFontName,NULL,fontWeight,fontStyle,fontStretch,(float)m_pDefaultFontInfo->iSize,L"", &(((TD2DFontInfo *)m_pDefaultFontInfo)->pTextFormat));
		::ZeroMemory(&m_pDefaultFontInfo->tm, sizeof(m_pDefaultFontInfo->tm));
		if( m_hDcPaintWnd ) 
		{
			HFONT hOldFont = (HFONT) ::SelectObject(m_hDcPaintWnd, m_pDefaultFontInfo->hFont);
			::GetTextMetrics(m_hDcPaintWnd, &m_pDefaultFontInfo->tm);
			::SelectObject(m_hDcPaintWnd, hOldFont);
		}
	}

	void _DrawBitmap(ID2D1RenderTarget *pRenderTarget, ID2D1Bitmap *bitmap,const RECT &rcDest,const RECT *rcSrc,int opacity)
	{
		if(pRenderTarget)
			pRenderTarget->DrawBitmap(bitmap,D2D1::RectF( (FLOAT)rcDest.left,(FLOAT)rcDest.top,(FLOAT)rcDest.right,(FLOAT)rcDest.bottom),(FLOAT)(opacity/255.0),D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,&D2D1::RectF( (FLOAT)rcSrc->left,(FLOAT)rcSrc->top,(FLOAT)rcSrc->right,(FLOAT)rcSrc->bottom));
	}

	void CIRenderD2D::DrawImage( const TImageInfo* ImgDate, const RECT& rc, const RECT& rcPaint, const RECT& rcBmpPart,const RECT& rcCorners, bool alphaChannel, BYTE uFade /*= 255*/, bool hole /*= false*/, bool xtiled /*= false*/, bool ytiled /*= false*/ )
	{
		if(!ImgDate || !m_pCurRenderTarget)
			return;
		ID2D1Bitmap *pBitmap =((TD2DImageInfo *)ImgDate)->GetTexture();
		if(!pBitmap)
			return;

		RECT rcTemp = {0};
		RECT rcDest = {0};
		RECT rcSrc = {0};
		if (rc.right - rc.left == rcBmpPart.right - rcBmpPart.left \
			&& rc.bottom - rc.top == rcBmpPart.bottom - rcBmpPart.top \
			&& rcCorners.left == 0 && rcCorners.right == 0 && rcCorners.top == 0 && rcCorners.bottom == 0)
		{
			if( ::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
				rcSrc.left = rcBmpPart.left + rcTemp.left - rc.left;
				rcSrc.top = rcBmpPart.top + rcTemp.top - rc.top;
				rcSrc.right = rcBmpPart.right+rcTemp.right-rc.right;
				rcSrc.bottom = rcBmpPart.bottom+rcTemp.bottom-rc.bottom;
				_DrawBitmap(m_pCurRenderTarget,pBitmap,rcTemp,&rcSrc,uFade);
			}
		}else
		{
			// middle
			if( !hole ) 
			{
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rc.right - rcCorners.right;
				rcDest.bottom = rc.bottom - rcCorners.bottom;

				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					if( !xtiled && !ytiled ) {
						rcSrc.left = rcBmpPart.left + rcCorners.left;
						rcSrc.top = rcBmpPart.top + rcCorners.top;
						rcSrc.right = rcBmpPart.right  - rcCorners.right;
						rcSrc.bottom = rcBmpPart.bottom - rcCorners.bottom;
						_DrawBitmap(m_pCurRenderTarget,pBitmap,rcDest,&rcSrc,uFade);
					}else if( xtiled && ytiled ) 
					{
						// 图片中间区域的宽高
						LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
						LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
						// 需要重复的次数
						int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
						int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
						for( int j = 0; j < iTimesY; ++j ) 
						{
							LONG lDestTop = rcDest.top + lHeight * j;
							LONG lDestBottom = rcDest.top + lHeight * (j + 1);
							if( lDestBottom > rcDest.bottom )
							{
								lDestBottom = rcDest.bottom;
							}
							for( int i = 0; i < iTimesX; ++i )
							{
								LONG lDestLeft = rcDest.left + lWidth * i;
								LONG lDestRight = rcDest.left + lWidth * (i + 1);
								if( lDestRight > rcDest.right ) 
								{
									lDestRight = rcDest.right;
								}
								RECT rcTmpDest={lDestLeft, lDestRight,lDestTop, lDestBottom};
								RECT rcTmpSrc={rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top,
									rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom};
								_DrawBitmap(m_pCurRenderTarget,pBitmap,rcTmpDest,&rcTmpSrc,uFade);
							}
						}
					}
					else if( xtiled )
					{
						LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcCorners.left - rcCorners.right;
						int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
						for( int i = 0; i < iTimes; ++i ) 
						{
							LONG lDestLeft = rcDest.left + lWidth * i;
							LONG lDestRight = rcDest.left + lWidth * (i + 1);
							if( lDestRight > rcDest.right )
								lDestRight = rcDest.right;
							RECT rcTmpDest={lDestLeft, rcDest.top, lDestRight, rcDest.bottom};
							RECT rcTmpSrc={rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top,
								rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom};
							_DrawBitmap(m_pCurRenderTarget,pBitmap,rcTmpDest,&rcTmpSrc,uFade);
						}
					}
					else { // ytiled
						LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcCorners.top - rcCorners.bottom;
						int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
						for( int i = 0; i < iTimes; ++i ) {
							LONG lDestTop = rcDest.top + lHeight * i;
							LONG lDestBottom = rcDest.top + lHeight * (i + 1);
							if( lDestBottom > rcDest.bottom ) {
								lDestBottom = rcDest.bottom;
							}
							RECT rcTmpDest={rcDest.left, lDestTop, rcDest.right, lDestBottom};
							RECT rcTmpSrc={rcBmpPart.left + rcCorners.left, rcBmpPart.top + rcCorners.top,
								rcBmpPart.right - rcCorners.right, rcBmpPart.bottom - rcCorners.bottom};
							_DrawBitmap(m_pCurRenderTarget,pBitmap,rcTmpDest,&rcTmpSrc,uFade	);
						}
					}
				}
			}
			// left-top
			if( rcCorners.left > 0 && rcCorners.top > 0 ) {
				rcDest.left = rc.left;
				rcDest.top = rc.top;
				rcDest.right = rc.left+rcCorners.left;
				rcDest.bottom = rc.top+rcCorners.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcSrc.left = rcBmpPart.left;
					rcSrc.top = rcBmpPart.top;
					rcSrc.right = rcBmpPart.left + rcCorners.left;
					rcSrc.bottom = rcBmpPart.top + rcCorners.top;				
					_DrawBitmap(m_pCurRenderTarget,pBitmap,rcDest,&rcSrc,uFade);
				}
			}
			// top
			if( rcCorners.top > 0 ) 
			{
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.top;
				rcDest.right = rc.right - rcCorners.right;
				rcDest.bottom = rc.top + rcCorners.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcSrc.left = rcBmpPart.left + rcCorners.left;
					rcSrc.top = rcBmpPart.top;
					rcSrc.right = rcBmpPart.right - rcCorners.right;
					rcSrc.bottom = rcBmpPart.top + rcCorners.top;
					_DrawBitmap(m_pCurRenderTarget,pBitmap,rcDest,&rcSrc,uFade);
				}
			}
			// right-top
			if( rcCorners.right > 0 && rcCorners.top > 0 ) {
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.top;
				rcDest.right = rc.right;
				rcDest.bottom = rc.top + rcCorners.top;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcSrc.left = rcBmpPart.right - rcCorners.right;
					rcSrc.top = rcBmpPart.top;
					rcSrc.right = rcBmpPart.right;
					rcSrc.bottom = rcBmpPart.top + rcCorners.top;
					_DrawBitmap(m_pCurRenderTarget,pBitmap,rcDest,&rcSrc,uFade);
				}
			}
			// left
			if( rcCorners.left > 0 ) {
				rcDest.left = rc.left;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rc.left + rcCorners.left;
				rcDest.bottom = rc.bottom - rcCorners.bottom;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcSrc.left = rcBmpPart.left;
					rcSrc.top = rcBmpPart.top + rcCorners.top;
					rcSrc.right = rcBmpPart.left + rcCorners.left;
					rcSrc.bottom = rcBmpPart.bottom - rcCorners.bottom;
					_DrawBitmap(m_pCurRenderTarget,pBitmap,rcDest,&rcSrc,uFade);
				}
			}
			// right
			if( rcCorners.right > 0 ) {
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.top + rcCorners.top;
				rcDest.right = rc.right;
				rcDest.bottom = rc.bottom - rcCorners.bottom;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcSrc.left = rcBmpPart.right - rcCorners.right;
					rcSrc.top = rcBmpPart.top + rcCorners.top;
					rcSrc.right = rcBmpPart.right;
					rcSrc.bottom = rcBmpPart.bottom - rcCorners.bottom;
					_DrawBitmap(m_pCurRenderTarget,pBitmap,rcDest,&rcSrc,uFade);
				}
			}
			// left-bottom
			if( rcCorners.left > 0 && rcCorners.bottom > 0 ) {
				rcDest.left = rc.left;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rc.left + rcCorners.left;
				rcDest.bottom = rc.bottom;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcSrc.left = rcBmpPart.left;
					rcSrc.top = rcBmpPart.bottom - rcCorners.bottom;
					rcSrc.right = rcBmpPart.left + rcCorners.left;
					rcSrc.bottom = rcBmpPart.bottom;
					_DrawBitmap(m_pCurRenderTarget,pBitmap,rcDest,&rcSrc,uFade);
				}
			}
			// bottom
			if( rcCorners.bottom > 0 ) {
				rcDest.left = rc.left + rcCorners.left;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rc.right-rcCorners.right;
				rcDest.bottom = rc.bottom;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcSrc.left = rcBmpPart.left + rcCorners.left;
					rcSrc.top = rcBmpPart.bottom - rcCorners.bottom;
					rcSrc.right = rcBmpPart.right- rcCorners.right;
					rcSrc.bottom = rcBmpPart.bottom;
					_DrawBitmap(m_pCurRenderTarget,pBitmap,rcDest,&rcSrc,uFade);
				}
			}
			// right-bottom
			if( rcCorners.right > 0 && rcCorners.bottom > 0 ) {
				rcDest.left = rc.right - rcCorners.right;
				rcDest.top = rc.bottom - rcCorners.bottom;
				rcDest.right = rc.right;
				rcDest.bottom = rc.bottom;
				if( ::IntersectRect(&rcTemp, &rcPaint, &rcDest) ) {
					rcSrc.left = rcBmpPart.right - rcCorners.right;
					rcSrc.top = rcBmpPart.bottom - rcCorners.bottom;
					rcSrc.right = rcBmpPart.right;
					rcSrc.bottom = rcBmpPart.bottom;
					_DrawBitmap(m_pCurRenderTarget,pBitmap,rcDest,&rcSrc,uFade);	
				}
			}
		}
	}


	void CIRenderD2D::DrawText( TFontInfo *pFontInfo, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor,UINT uStyle )
	{
		_DrawText(m_pCurRenderTarget, m_pDWriteFactory, rc, pstrText,dwTextColor,(TD2DFontInfo *)pFontInfo,uStyle);
	}

	void CIRenderD2D::_DrawText( ID2D1RenderTarget *pRenderTarget,IDWriteFactory *pWriteFactory,RECT& rc,LPCTSTR pstrText,DWORD dwTextColor,TD2DFontInfo *pFontInfo,UINT uStyle )
	{
		int len = _tcslen(pstrText);
		if( pstrText == NULL || pRenderTarget == NULL) return;
		ID2D1SolidColorBrush *pGridBrush = NULL;
		pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(dwTextColor),&pGridBrush);
		IDWriteTextLayout * pWriteTextLayout = NULL;
		pWriteFactory->CreateTextLayout(pstrText,len,pFontInfo->pTextFormat,(float)(rc.right - rc.left),(float)(rc.bottom - rc.top),&pWriteTextLayout);
		D2D1_POINT_2F pt = D2D1::Point2F(static_cast<FLOAT>(rc.left), static_cast<FLOAT>(rc.top-1));
		DWRITE_TEXT_RANGE textRange={0, len};
		if(pFontInfo->bBold)
			pWriteTextLayout->SetFontWeight(DWRITE_FONT_WEIGHT_BOLD, textRange);
		if(pFontInfo->bItalic)
			pWriteTextLayout->SetFontStyle(DWRITE_FONT_STYLE_ITALIC, textRange);
		if(pFontInfo->bUnderline)
			pWriteTextLayout->SetUnderline(TRUE, textRange);
		if(uStyle&DT_TOP)
			pWriteTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		if(uStyle&DT_LEFT)
			pWriteTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		if(uStyle&DT_CENTER)
			pWriteTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		if(uStyle&DT_RIGHT)
			pWriteTextLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		if(uStyle&DT_VCENTER)
			pWriteTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		if(uStyle&DT_BOTTOM)
			pWriteTextLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		pRenderTarget->DrawTextLayout(pt,pWriteTextLayout, pGridBrush);
		SAFE_RELEASE(pGridBrush);
		SAFE_RELEASE(pWriteTextLayout);
	}

	void CIRenderD2D::DrawColor( const RECT& rc, DWORD color )
	{
		if(!m_pCurRenderTarget)	return;
		ID2D1SolidColorBrush *pGridBrush = NULL;
		int nAlpha = (color&0xff000000) >> 24;
		float fAlpha = nAlpha/255.0;
		m_pCurRenderTarget->CreateSolidColorBrush(D2D1::ColorF(color,fAlpha),&pGridBrush);
		m_pCurRenderTarget->FillRectangle(D2D1::RectF((FLOAT)rc.left,(FLOAT)rc.top,(FLOAT)rc.right,(FLOAT)rc.bottom), pGridBrush);
		SAFE_RELEASE(pGridBrush);
	}

	void CIRenderD2D::DrawGradient( const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical )
	{
		if(!m_pCurRenderTarget) return;
		ID2D1LinearGradientBrush  *brush;
		ID2D1GradientStopCollection *gradientStopCollection;
		D2D1_GRADIENT_STOP gradientStops[2] ;
		gradientStops[0].color = D2D1::ColorF(dwFirst) ;
		gradientStops[0].position = 0.0f ;
		gradientStops[1].color = D2D1::ColorF(dwSecond) ;
		gradientStops[1].position = 1.0f ;
		m_pCurRenderTarget->CreateGradientStopCollection(gradientStops,2,D2D1_GAMMA_2_2,D2D1_EXTEND_MODE_CLAMP,&gradientStopCollection);

		D2D1_POINT_2F ptStart,ptEnd;
		if(bVertical)
		{
			ptStart.x = ptEnd.x = rc.left;
			ptStart.y = rc.top;
			ptEnd.y = rc.bottom;
		}
		else
		{
			ptStart.y = ptEnd.y = rc.top;
			ptStart.x = rc.left;
			ptEnd.x = rc.right;
		}
		m_pCurRenderTarget->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(ptStart,ptEnd),gradientStopCollection,&brush) ;
		m_pCurRenderTarget->FillRectangle(D2D1::RectF((FLOAT)rc.left,(FLOAT)rc.top,(FLOAT)rc.right,(FLOAT)rc.bottom),brush);
		SAFE_RELEASE(gradientStopCollection);
		SAFE_RELEASE(brush);
	}

	void CIRenderD2D::DrawLine( const RECT& rc, int nSize, DWORD dwPenColor,int nStyle /*= PS_SOLID*/ )
	{
		if(!m_pCurRenderTarget) return;
		ID2D1SolidColorBrush *pGridBrush = NULL;
		m_pCurRenderTarget->CreateSolidColorBrush(D2D1::ColorF(dwPenColor),&pGridBrush);

		ID2D1StrokeStyle* pStrokeStyleCustom = NULL;
		if(m_pD2d1Factory && nStyle != 0)
		{
			D2D1_DASH_STYLE uStyle = (D2D1_DASH_STYLE)nStyle;
			float dashes[] = {1.0f, 2.0f, 2.0f, 3.0f, 2.0f, 2.0f};
			m_pD2d1Factory->CreateStrokeStyle(
				D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_FLAT,D2D1_CAP_STYLE_FLAT,D2D1_CAP_STYLE_ROUND,D2D1_LINE_JOIN_MITER,10.0f,uStyle),
				dashes,ARRAYSIZE(dashes),&pStrokeStyleCustom
				);
		}
		int nStartX = rc.left;
		int nEndX = rc.right;
		if(rc.left == rc.right)
			nEndX += 1;
		m_pCurRenderTarget->DrawLine(D2D1::Point2F(nStartX, rc.top+1),D2D1::Point2F(nEndX, rc.bottom+1),pGridBrush,nSize,pStrokeStyleCustom);
		SAFE_RELEASE(pGridBrush);
		SAFE_RELEASE(pStrokeStyleCustom);
	}

	void CIRenderD2D::DrawRect( const RECT& rc, int nSize, DWORD dwPenColor )
	{
		if(!m_pCurRenderTarget) return;
		ID2D1SolidColorBrush *pGridBrush = NULL;
		m_pCurRenderTarget->CreateSolidColorBrush(D2D1::ColorF(dwPenColor),&pGridBrush);
		m_pCurRenderTarget->DrawRectangle(D2D1::RectF((FLOAT)rc.left+1,(FLOAT)rc.top+1,(FLOAT)rc.right,(FLOAT)rc.bottom),pGridBrush,nSize);
		SAFE_RELEASE(pGridBrush);
	}

	void CIRenderD2D::DrawRoundRect( const RECT& rc, int width, int height, int nSize, DWORD dwPenColor )
	{
		if(!m_pCurRenderTarget) return;
		ID2D1SolidColorBrush *pGridBrush = NULL;
		m_pCurRenderTarget->CreateSolidColorBrush(D2D1::ColorF(dwPenColor),&pGridBrush);
		D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(D2D1::RectF(rc.left, rc.top, rc.right, rc.bottom),width,width);
		m_pCurRenderTarget->DrawRoundedRectangle(roundedRect,pGridBrush,nSize);
		SAFE_RELEASE(pGridBrush);
	}

	void CIRenderD2D::DrawCircle( int nCenterX,int nCenterY,int nRadius,DWORD dwColor)
	{

	}

	void CIRenderD2D::FillCircle( int nCenterX,int nCenterY,int nRadius,DWORD dwColor )
	{

	}

	void CIRenderD2D::DrawArc( int nCenterX,int nCenterY,int nRadius,float fStartAngle,float fSweepAngle,DWORD dwColor)
	{

	}

	void CIRenderD2D::GetTextBounds( int& nWidth,int& nHeight,TFontInfo *pFontInfo,LPCTSTR pstrText,UINT uStyle )
	{

	}

}
