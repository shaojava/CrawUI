#include "StdAfx.h"
#include "UIRenderSKIA.h"
#include "../Utils/mem_image.h"

namespace UiLib {
	CUIRenderSKIA::CUIRenderSKIA()
	{
	}

	CUIRenderSKIA::~CUIRenderSKIA()
	{
	}

	RenderWay CUIRenderSKIA::GetRenderType()
	{
		return SKIA_RENDER;
	}

    bool CUIRenderSKIA::LinkRenderCore()
    {
		BOOL bRe = FALSE;
		hRenderAPI = GetModuleHandle(_T("Render_SKIA.dll"));
		if(!hRenderAPI)
			hRenderAPI = LoadLibrary(_T("Render_SKIA.dll"));
		if(hRenderAPI) ATLTRACE(_T("LoadSuccess-"));
        GetRenderInterface_ GetInstance = NULL;
        GetInstance = (GetRenderInterface_)GetProcAddress(hRenderAPI,"GetRenderInstance");
		if(!GetInstance) ATLTRACE(_T("Can't find"));
		if(!GetInstance) return false;
        m_pIRenderCore = GetInstance();
		return true;
    }


    void CUIRenderSKIA::Clear()
    {
        if(m_pIRenderCore)
            m_pIRenderCore->Clear();
    }

    void CUIRenderSKIA::Clear(const RECT &rcClear)
    {
        if(m_pIRenderCore)
            m_pIRenderCore->Clear(rcClear);
    }

    void CUIRenderSKIA::PushClip(const RECT& clipRect,RECT* rcRound,int nRoundX,int nRoundY,ClipOpt _ClipOpt)
    {
        if(m_pIRenderCore)
            m_pIRenderCore->PushClip(clipRect,rcRound,nRoundX,nRoundY,_ClipOpt);
    }

    void CUIRenderSKIA::PopClip(const RECT & clipRect)
    {
        if(m_pIRenderCore)
            m_pIRenderCore->PopClip(clipRect);
    }

    void CUIRenderSKIA::SetPaintWindow( HWND hWnd )
    {
        if(m_pIRenderCore)
        {
            m_pIRenderCore->SetPaintWindow(hWnd);
            SetDefaultFont();
            ReleaseRenderObj();
        }
    }

    void CUIRenderSKIA::ReleaseRenderObj()
    {
        if(m_pIRenderCore)
            m_pIRenderCore->ReleaseRenderObj();
    }

    TImageInfo* CUIRenderSKIA::LoadImage( STRINGorID bitmap, LPCTSTR type /*= NULL*/, DWORD mask /*= 0*/ )
    {
        LPBYTE pData = NULL;
        DWORD dwSize = 0;
        Bitmap *pImage = NULL;
        do 
        {
            if( type == NULL )
            {
                CDuiString sFile = CPaintManagerUI::GetResourcePath();
                //直接读取文件;
                if( CPaintManagerUI::GetResourceZip().IsEmpty() ) 
                {
					sFile += bitmap.m_lpstr;
                    pImage = new Bitmap(sFile);
                    if(pImage->GetWidth() == 0 || pImage->GetHeight() == 0)
                        SAFE_DELETE(pImage);
                }
                else	//从zip中加载图片;
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
            else	//从资源ID中加载图片;
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

		//读不到图片, 则直接去读取bitmap.m_lpstr指向的路径;
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

	

	TImageInfo* CUIRenderSKIA::LoadImage(HBITMAP hBitmap,int nWidth, int nHeight, bool bAlpha)
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

    void CUIRenderSKIA::SetDefaultFont( LPCTSTR pStrFontName /*= _T("")*/, int nSize /*= 0*/, bool bBold /*= false*/, bool bUnderline /*= false*/, bool bItalic /*= false*/ )
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

    TFontInfo* CUIRenderSKIA::AddFont( LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic )
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
}