#include "StdAfx.h"
#include "UIRenderGDI.h"

extern "C"
{
	extern unsigned char *stbi_load_from_memory(unsigned char const *buffer, int len, int *x, int *y, \
		int *comp, int req_comp);
	extern void     stbi_image_free(void *retval_from_stbi_load);
};

namespace UiLib {

	CUIRenderGDI::CUIRenderGDI()
	{
	}

	CUIRenderGDI::~CUIRenderGDI()
	{
	}

	RenderWay CUIRenderGDI::GetRenderType()
	{
		return GDI_RENDER;
	}

	bool CUIRenderGDI::LinkRenderCore()
	{
		hRenderAPI = GetModuleHandle(_T("Render_GDI.dll"));
		if(!hRenderAPI)
			hRenderAPI = LoadLibrary(_T("Render_GDI.dll"));
		GetRenderInterface_ GetInstance = NULL;
		GetInstance = (GetRenderInterface_)GetProcAddress(hRenderAPI,"GetRenderInstance");
		if(!GetInstance) return false;
		m_pIRenderCore = GetInstance();
		return true;
	}

	void CUIRenderGDI::Clear()
	{
		if(m_pIRenderCore)
			m_pIRenderCore->Clear();
	}

	void CUIRenderGDI::Clear( const RECT &rcClear )
	{
		if(m_pIRenderCore)
			m_pIRenderCore->Clear(rcClear);
	}

	void CUIRenderGDI::PushClip( const RECT& clipRect,RECT* rcRound,int nRoundX,int nRoundY,ClipOpt _ClipOpt)
	{
		if(m_pIRenderCore)
			m_pIRenderCore->PushClip(clipRect,rcRound,nRoundX,nRoundY,_ClipOpt);
	}

	void CUIRenderGDI::PopClip(const RECT & clipRect)
	{
		if(m_pIRenderCore)
			m_pIRenderCore->PopClip(clipRect);
	}

	void CUIRenderGDI::SetPaintWindow( HWND hWnd )
	{
		if(m_pIRenderCore)
		{
			m_pIRenderCore->SetPaintWindow(hWnd);
			SetDefaultFont();
			ReleaseRenderObj();
		}
	}

	void CUIRenderGDI::ReleaseRenderObj()
	{
		if(m_pIRenderCore)
			m_pIRenderCore->ReleaseRenderObj();
	}

	TImageInfo* CUIRenderGDI::LoadImage( STRINGorID bitmap, LPCTSTR type /*= NULL*/, DWORD mask /*= 0*/ )
	{
		LPBYTE pData = NULL;
		DWORD dwSize = 0;

		do 
		{
			if( type == NULL ) {
				CDuiString sFile = CPaintManagerUI::GetResourcePath();
				if( CPaintManagerUI::GetResourceZip().IsEmpty() ) 
				{
					sFile += bitmap.m_lpstr;
					HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
						FILE_ATTRIBUTE_NORMAL, NULL);
					if( hFile == INVALID_HANDLE_VALUE ) break;
					dwSize = ::GetFileSize(hFile, NULL);
					if( dwSize == 0 ) break;

					DWORD dwRead = 0;
					pData = new BYTE[ dwSize ];
					::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
					::CloseHandle( hFile );

					if( dwRead != dwSize ) 
					{
						delete[] pData;
						pData = NULL;
						break;
					}
				}
				else {
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
					if( res != 0x00000000 && res != 0x00000600) {
						delete[] pData;
						pData = NULL;
						if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
						break;
					}
					if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
				}
			}
			else {
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

		while (!pData)
		{
			//读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
			HANDLE hFile = ::CreateFile(bitmap.m_lpstr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
				FILE_ATTRIBUTE_NORMAL, NULL);
			if( hFile == INVALID_HANDLE_VALUE ) break;
			dwSize = ::GetFileSize(hFile, NULL);
			if( dwSize == 0 ) break;

			DWORD dwRead = 0;
			pData = new BYTE[ dwSize ];
			::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
			::CloseHandle( hFile );

			if( dwRead != dwSize ) {
				delete[] pData;
				pData = NULL;
			}
			break;
		}

		if (!pData)
			return NULL;

		LPBYTE pImage = NULL;
		int x,y,n;
		pImage = stbi_load_from_memory(pData, dwSize, &x, &y, &n, 4);
		delete[] pData;
		if( !pImage )
			return NULL;

		BITMAPINFO bmi;
		::ZeroMemory(&bmi, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = x;
		bmi.bmiHeader.biHeight = -y;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = x * y * 4;

		bool bAlphaChannel = false;
		LPBYTE pDest = NULL;
		HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
		if( !hBitmap )
			return NULL;

		for( int i = 0; i < x * y; i++ ) 
		{
			pDest[i*4 + 3] = pImage[i*4 + 3];
			if( pDest[i*4 + 3] < 255 )
			{
				pDest[i*4] = (BYTE)(DWORD(pImage[i*4 + 2])*pImage[i*4 + 3]/255);
				pDest[i*4 + 1] = (BYTE)(DWORD(pImage[i*4 + 1])*pImage[i*4 + 3]/255);
				pDest[i*4 + 2] = (BYTE)(DWORD(pImage[i*4])*pImage[i*4 + 3]/255); 
				bAlphaChannel = true;
			}
			else
			{
				pDest[i*4] = pImage[i*4 + 2];
				pDest[i*4 + 1] = pImage[i*4 + 1];
				pDest[i*4 + 2] = pImage[i*4]; 
			}

			if( *(DWORD*)(&pDest[i*4]) == mask ) {
				pDest[i*4] = (BYTE)0;
				pDest[i*4 + 1] = (BYTE)0;
				pDest[i*4 + 2] = (BYTE)0; 
				pDest[i*4 + 3] = (BYTE)0;
				bAlphaChannel = true;
			}
		}

		stbi_image_free(pImage);


		if(m_pIRenderCore)
		{
			TImagePrimData ImagePrimData;
			ImagePrimData.hBitmap = hBitmap;
			ImagePrimData.nX = x;
			ImagePrimData.nY = y;
			ImagePrimData.bAlphaChannel = bAlphaChannel;
			return m_pIRenderCore->LoadImage(ImagePrimData);
		}
		return NULL;
	}

	TImageInfo* CUIRenderGDI::LoadImage(HBITMAP hBitmap,int nWidth, int nHeight, bool bAlpha)
	{
		if(m_pIRenderCore)
		{
			TImagePrimData ImagePrimData;
			ImagePrimData.hBitmap = hBitmap;
			ImagePrimData.nX = nWidth;
			ImagePrimData.nY = nHeight;
			ImagePrimData.bAlphaChannel = bAlpha;
			return m_pIRenderCore->LoadImage(ImagePrimData);
		}
		return NULL;
	}

	void CUIRenderGDI::SetDefaultFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
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

	TFontInfo* CUIRenderGDI::AddFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic)
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

		TFontPrimData FontPrimData;

		FontPrimData.hFont = hFont;
		_tcsncpy(FontPrimData.sFontName, pStrFontName,LF_FACESIZE);
		FontPrimData.iSize = nSize;
		FontPrimData.bBold = bBold;
		FontPrimData.bUnderline = bUnderline;
		FontPrimData.bItalic = bItalic;

		if(m_pIRenderCore)
			return m_pIRenderCore->AddFont(FontPrimData);
		return NULL;
	}

	void CUIRenderGDI::DrawColor( const RECT& rc, DWORD color )
	{
		if(m_pIRenderCore)
			m_pIRenderCore->DrawColor(rc,color);
	}

	void CUIRenderGDI::DrawGradient( const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical )
	{
		if(m_pIRenderCore)
			m_pIRenderCore->DrawGradient(rc,dwFirst,dwSecond,bVertical);
	}

	void CUIRenderGDI::DrawLine(const RECT& rc, int nSize, DWORD dwPenColor,int nStyle /*= PS_SOLID*/ )
	{
		if(m_pIRenderCore)
			m_pIRenderCore->DrawLine(rc,nSize,dwPenColor,nStyle);
	}

	void CUIRenderGDI::DrawRect(const RECT& rc, int nSize, DWORD dwPenColor )
	{
		if(m_pIRenderCore)
			m_pIRenderCore->DrawRect(rc,nSize,dwPenColor);
	}

	void CUIRenderGDI::DrawRoundRect( const RECT& rc, int width, int height, int nSize, DWORD dwPenColor )
	{
		if(m_pIRenderCore)
			m_pIRenderCore->DrawRoundRect(rc,width,height,nSize,dwPenColor);
	}
}