#include "stdafx.h"
#include "UICaptcha.h"

namespace UiLib
{
	REGIST_DUICLASS(CCaptchaUI)

    const CHAR g_RandomAry[] = 
    {
        '1','2','3','4','5','6','7',
        '8','9','0','a','b','c','d',
        'e','f','g','h','i','j','k',
        'l','m','n','o','p','q','r',
        's','t','u','v','w','x','y',
        'z','A','B','C','D','E','F',
        'G','H','I','J','K','L','M',
        'N','O','P','Q','R','S','T',
        'U','V','W','X','Y','Z'
    };

    const CDuiString g_RandomFont[] = 
    {
        _T("Arial"),
        _T("Arial Narrow"),
        _T("Batang"),
        _T("BatangChe"),
        _T("Book Antiqua"),
        _T("Bookman Old Style"),
        _T("Dotum"),
        _T("Garamond"),
        _T("Gulim"),
        _T("Kartika"),
        _T("Monotype Corsiva"),
        _T("System"),
        _T("方正姚体"),
        _T("仿宋_GB2312"),
        _T("黑体"),
        _T("华文新魏"),
        _T("华文行楷"),
        _T("隶书"),
        _T("楷体_GB2312"),
        _T("新宋体"),
        _T("幼圆"),
        _T("Lucida Console"),
        _T("Monotype Corsiva")
    };

	CCaptchaUI::CCaptchaUI()
        : m_hCaptchaImg(NULL)
        , m_bbgDisturb(true)
        , m_bNoise(true)
        , m_bfontDisturb(true)
	{
	}

    CCaptchaUI::~CCaptchaUI()
    {
        if(m_hCaptchaImg)
            DeleteObject(m_hCaptchaImg);
        m_hCaptchaImg = NULL;
    }

	LPCTSTR CCaptchaUI::GetClass() const
	{
		return _T("CaptchaUI");
	}

	LPVOID CCaptchaUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_CAPTCHA) == 0 ) return static_cast<CCaptchaUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

    void CCaptchaUI::SetBgDisturb(LPCTSTR lpszValue)
    {
        if(_tcscmp(lpszValue,_T("false")) == 0)
            m_bbgDisturb = false;
        else
            m_bbgDisturb = true;
    }

    void CCaptchaUI::SetNoise(LPCTSTR lpszValue)
    {
        if(_tcscmp(lpszValue,_T("false")) == 0)
            m_bNoise = false;
        else
            m_bNoise = true;
    }

    void CCaptchaUI::SetFontDisturb(LPCTSTR lpszValue)
    {
        if(_tcscmp(lpszValue,_T("false")) == 0)
            m_bfontDisturb = false;
        else
            m_bfontDisturb = true;
    }

    void CCaptchaUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        if( _tcscmp(pstrName, _T("bgdisturb")) == 0 ) SetBgDisturb(pstrValue);
        else if( _tcscmp(pstrName, _T("noise")) == 0 ) SetNoise(pstrValue);
        else if( _tcscmp(pstrName, _T("fontdisturb")) == 0) SetFontDisturb(pstrValue);
        else CLabelUI::SetAttribute(pstrName, pstrValue);
    }

    void CCaptchaUI::PaintBkImage()
    {
        if(m_hCaptchaImg == NULL)
            return;

        RECT rcBmpPart = {0,0,m_cxyFixed.cx,m_cxyFixed.cy};
        RECT rcCorners = {0};
		TImageInfo ImageInfo;
		ImageInfo.hBitmap = m_hCaptchaImg;
        m_pManager->GetRenderCore()->DrawImage(&ImageInfo,m_rcItem,m_rcPaint,rcBmpPart,rcCorners,false);
    }

    bool CCaptchaUI::GenerateCaptchaImg()
    {
        if(m_hCaptchaImg)
            DeleteObject(m_hCaptchaImg);
        m_hCaptchaImg = NULL;

        HDC hDC = GetDC(NULL);
        HDC hMem = CreateCompatibleDC(hDC);

        BITMAPINFO bmInfo;
        ZeroMemory(&bmInfo,sizeof(BITMAPINFO));
        bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmInfo.bmiHeader.biWidth = m_cxyFixed.cx;
        bmInfo.bmiHeader.biHeight = m_cxyFixed.cy;
        bmInfo.bmiHeader.biBitCount = 32;
        bmInfo.bmiHeader.biPlanes = 1;
        bmInfo.bmiHeader.biCompression = BI_RGB;
        bmInfo.bmiHeader.biSizeImage = m_cxyFixed.cx * m_cxyFixed.cy * sizeof(DWORD);

        BYTE* pData = NULL;
        m_hCaptchaImg = CreateDIBSection(hDC,&bmInfo,DIB_RGB_COLORS,(void**)&pData,NULL,0);
        if(m_hCaptchaImg)
        {
            HBITMAP hOldBmp = (HBITMAP)SelectObject(hMem,m_hCaptchaImg);
            SetBkMode(hMem,TRANSPARENT);

            GenerateBackGround(pData,bmInfo.bmiHeader.biWidth,bmInfo.bmiHeader.biHeight);

            GenerateVerifyString();

            PixelDisturb(pData,bmInfo.bmiHeader.biWidth,bmInfo.bmiHeader.biHeight);

            HFONT hFont = FontDisturb(m_cxyFixed.cx/5,m_cxyFixed.cy/5);   
            HFONT hOldFont = (HFONT)SelectObject(hMem,hFont); 
            ::SetTextColor(hMem,RGB(255,255,255)); 

            RECT rc = {0,0,bmInfo.bmiHeader.biWidth,bmInfo.bmiHeader.biHeight};
            DrawText(hMem,m_szCaptcha.GetData(),-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER); 

            SelectObject(hMem,hOldFont); 
            DeleteObject(hFont); 
            SelectObject(hMem,hOldBmp);   
            ::DeleteDC(hMem);

            Invalidate();
            return true;
        }

        return false;
    }

    void CCaptchaUI::GenerateBackGround(BYTE* pdata,int width,int height)
    {
        DWORD* pBit = (DWORD*)pdata;

        if(m_bbgDisturb)
        {
            srand((unsigned)timeGetTime());

            DWORD randleft = 0;
            DWORD randright = 0;
            
            for (int i=0;i<width*height;i++)
            {
                randleft  = rand() % 0x00FFFFFF;
                randright = rand() % 0x00FFFFFF;

                pBit[i] = randleft & randright;
            }
        }
        else
        {
            for (int i=0;i<width*height;i++)
            {
                pBit[i] = 0xFF000000;
            }
        }
    }

    void CCaptchaUI::GenerateVerifyString()
    {
        srand((unsigned)timeGetTime());
        
        m_szCaptcha.Empty();
        for(int i=0;i<4;i++)
        {
            char c = g_RandomAry[rand() % (sizeof(g_RandomAry)/sizeof(CHAR))];
            m_szCaptcha += c;
        }
    }

    void CCaptchaUI::PixelDisturb(BYTE* pdata,int width,int height)
    {
        DWORD totalPixel = width*height - 1;
        DWORD pixelPos = 1;

        DWORD* pBits = (DWORD*)pdata;
        for(DWORD j=0;j<((DWORD)(totalPixel*0.1));j++)
        {
            pixelPos = rand() % totalPixel;
            pBits[pixelPos] = 0x00FFFFFF;
        } 
    }

    HFONT CCaptchaUI::FontDisturb(int width/* = 0*/,int height/* = 0*/)
    {
        if(width <= 12)
            width = 12;
        if(height <= 12)
            height = 12;

        srand((unsigned)timeGetTime());

        LOGFONT lf;
        ZeroMemory(&lf,sizeof(LOGFONT));

        int fontindex = rand() % (sizeof(g_RandomFont)/sizeof(CDuiString));
        _tcscpy(lf.lfFaceName,g_RandomFont[fontindex].GetData());

        lf.lfHeight     =  width + (rand()%5);     
        lf.lfWidth      =  height + (rand()%5);
        lf.lfWeight     =  100 * (rand()%7); 
        lf.lfItalic     =  TRUE;
        lf.lfEscapement =  rand()%80;

        return CreateFontIndirect(&lf);
    }
}