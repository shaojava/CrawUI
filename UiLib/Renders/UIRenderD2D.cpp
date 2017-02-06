#include "StdAfx.h"
#include "UIRenderD2D.h"
#include "../Utils/mem_image.h"

namespace UiLib {
	CUIRenderD2D::CUIRenderD2D()
	{
		
	}

	CUIRenderD2D::~CUIRenderD2D()
	{

	}

	bool CUIRenderD2D::LinkRenderCore()
	{
		hRenderAPI = GetModuleHandle(_T("Render_D2D.dll"));
		if(!hRenderAPI)
			hRenderAPI = LoadLibrary(_T("Render_D2D.dll"));
		GetRenderInterface_ GetInstance = NULL;
		GetInstance = (GetRenderInterface_)GetProcAddress(hRenderAPI,"GetRenderInstance");
		if(!GetInstance) return false;
		m_pIRenderCore = GetInstance();
		return true;
	}

	void CUIRenderD2D::SetPaintWindow( HWND hWnd )
	{
		if(m_pIRenderCore)
		{
			m_pIRenderCore->SetPaintWindow(hWnd);
			SetDefaultFont();
			ReleaseRenderObj();
		}
	}

	void CUIRenderD2D::ReleaseRenderObj()
	{
		if(m_pIRenderCore)
			m_pIRenderCore->ReleaseRenderObj();
	}

	UiLib::RenderWay CUIRenderD2D::GetRenderType()
	{
		return D2D_RENDER;
	}

	void CUIRenderD2D::Clear()
	{
		if(m_pIRenderCore)
			m_pIRenderCore->Clear();
	}

	void CUIRenderD2D::Clear( const RECT &rcClear )
	{
		if(m_pIRenderCore)
			m_pIRenderCore->Clear(rcClear);
	}

	void CUIRenderD2D::PushClip( const RECT& clipRect,RECT* rcRound,int nRoundX,int nRoundY,ClipOpt _ClipOpt /*= OP_REPLACE*/ )
	{
		if(m_pIRenderCore)
			m_pIRenderCore->PushClip(clipRect,rcRound,nRoundX,nRoundY,_ClipOpt);
	}

	void CUIRenderD2D::PopClip( const RECT & clipRect )
	{
		if(m_pIRenderCore)
			m_pIRenderCore->PopClip(clipRect);
	}

	TImageInfo* CUIRenderD2D::LoadImage( STRINGorID bitmap, LPCTSTR type /*= NULL*/, DWORD mask /*= 0*/ )
	{
		LPBYTE pData = NULL;
		DWORD dwSize = 0;
		Bitmap *pImage = NULL;
		do 
		{
			if( type == NULL )
			{
				CDuiString sFile = CPaintManagerUI::GetResourcePath();
				if( CPaintManagerUI::GetResourceZip().IsEmpty() ) 
				{
					sFile += bitmap.m_lpstr;
					pImage = new Bitmap(sFile);
					if(pImage->GetWidth() == 0 || pImage->GetHeight() == 0)
						SAFE_DELETE(pImage);
				}
				else
				{
					sFile += CPaintManagerUI::GetResourceZip();
					HZIP hz = NULL;
					if( CPaintManagerUI::IsCachedResourceZip() ) hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
					else hz = OpenZip((void*)sFile.GetData(), 0, 2);
					if( hz == NULL ) break;
					ZIPENTRY ze;
					int i; 
					if( FindZipItem(hz, bitmap.m_lpstr, true, &i, &ze) != 0 ) break;
					dwSize = ze.unc_size;
					if( dwSize == 0 ) break;
					pData = new BYTE[ dwSize ];
					int res = UnzipItem(hz, i, pData, dwSize, 3);
					if( res != 0x00000000 && res != 0x00000600) 
					{
						SAFE_DELETEARRY(pData);
						if( !CPaintManagerUI::IsCachedResourceZip() ) 
							CloseZip(hz);
						break;
					}
					if( !CPaintManagerUI::IsCachedResourceZip() ) 
						CloseZip(hz);
				}
			}
			else
			{
				HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), bitmap.m_lpstr, type);
				if( hResource == NULL ) break;
				HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
				if( hGlobal == NULL ) {
					FreeResource(hResource);
					break;
				}
				dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
				if( dwSize == 0 ) break;
				pData = new BYTE[ dwSize ];
				::CopyMemory(pData, (LPBYTE)::LockResource(hGlobal), dwSize);
				::FreeResource(hResource);
			}
		} while (0);

		if(!pImage && !pData)
		{
			pImage = new Bitmap(bitmap.m_lpstr);
			if(pImage->GetWidth() == 0|| pImage->GetHeight() == 0)
				SAFE_DELETE(pImage);
		}

		if(!pImage && pData)
			pImage = (Bitmap *)mi_from_memory(pData,dwSize);
		if(!pImage)
		{
			SAFE_DELETEARRY(pData);
			return NULL;
		}
		SAFE_DELETEARRY(pData);

		if(m_pIRenderCore)
		{
			TImagePrimData ImagePrimData;
			ImagePrimData.pImage = pImage;
			ImagePrimData.pData = NULL;
			return m_pIRenderCore->LoadImage(ImagePrimData);
		}
		return NULL;
	}

	TImageInfo* CUIRenderD2D::LoadImage(HBITMAP hBitmap,int nWidth, int nHeight, bool bAlpha)
	{
		Bitmap *pImage = CGenerAlgorithm::CreateBitmapFromHBITMAP(hBitmap);
		if(m_pIRenderCore)
		{
			TImagePrimData ImagePrimData;
			ImagePrimData.pImage = pImage;
			ImagePrimData.pData = NULL;
			return m_pIRenderCore->LoadImage(ImagePrimData);
		}
		return NULL;
	}

	TFontInfo* CUIRenderD2D::AddFont( LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic )
	{
		LOGFONT lf = { 0 };
		::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		_tcsncpy(lf.lfFaceName, pStrFontName,LF_FACESIZE);
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfHeight = -nSize;
		if( bBold ) lf.lfWeight += FW_BOLD;
		if( bUnderline ) lf.lfUnderline = TRUE;
		if( bItalic ) lf.lfItalic = TRUE;
		HFONT hFont = ::CreateFontIndirect(&lf);
		if( hFont == NULL ) return NULL;
		if(m_pIRenderCore)
		{
			TFontPrimData FontPrimData;
			FontPrimData.hFont = hFont;
			_tcsncpy(FontPrimData.sFontName,pStrFontName,LF_FACESIZE);
			FontPrimData.iSize = nSize;
			FontPrimData.bBold = bBold;
			FontPrimData.bUnderline = bUnderline;
			FontPrimData.bItalic = bItalic;
			return m_pIRenderCore->AddFont(FontPrimData);
		}
		return NULL;
	}

	void CUIRenderD2D::SetDefaultFont( LPCTSTR pStrFontName /*= _T("")*/, int nSize /*= 0*/, bool bBold /*= false*/, bool bUnderline /*= false*/, bool bItalic /*= false*/ )
	{
		LOGFONT lf = { 0 };
		::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		lf.lfCharSet = DEFAULT_CHARSET;
		HFONT hFont = NULL;
		TFontPrimData FontPrimData;
		if(pStrFontName == _T("") )
		{
			hFont = ::CreateFontIndirect(&lf);
			if( hFont == NULL ) return;
			FontPrimData.hFont = hFont;
			if (CPaintManagerUI::m_pStrDefaultFontName.GetLength()>0)
				_tcscpy_s(lf.lfFaceName, LF_FACESIZE, CPaintManagerUI::m_pStrDefaultFontName.GetData());
			_tcsncpy(FontPrimData.sFontName, lf.lfFaceName,LF_FACESIZE);
			FontPrimData.iSize = -lf.lfHeight;
			FontPrimData.bBold = (lf.lfWeight >= FW_BOLD);
			FontPrimData.bUnderline = (lf.lfUnderline == TRUE);
			FontPrimData.bItalic = (lf.lfItalic == TRUE);
		}
		else
		{
			_tcsncpy(lf.lfFaceName, pStrFontName,LF_FACESIZE);
			lf.lfHeight = -nSize;
			if( bBold ) lf.lfWeight += FW_BOLD;
			if( bUnderline ) lf.lfUnderline = TRUE;
			if( bItalic ) lf.lfItalic = TRUE;
			hFont = ::CreateFontIndirect(&lf);
			if( hFont == NULL ) return;
			FontPrimData.hFont = hFont;
			_tcsncpy(FontPrimData.sFontName, pStrFontName,LF_FACESIZE);
			FontPrimData.iSize = nSize;
			FontPrimData.bBold = bBold;
			FontPrimData.bUnderline = bUnderline;
			FontPrimData.bItalic = bItalic;
		}
		::ZeroMemory(&(FontPrimData.tm), sizeof(FontPrimData.tm));

		if(m_pIRenderCore)
			m_pIRenderCore->SetDefaultFont(FontPrimData);
	}

}