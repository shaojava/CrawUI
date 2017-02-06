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
    typedef struct tagTGDIPImageInfo
    {
        Bitmap *pBitmap;
        void *bit;
        SkBitmap *pSkBitmap;
        tagTGDIPImageInfo()
        {
            pSkBitmap = NULL;
            pBitmap = NULL;
            bit = NULL;
        }
        ~tagTGDIPImageInfo()
        {
            if(pBitmap)
            {
                delete pBitmap;
                pBitmap = NULL;
                bit = NULL;
            }
            if(pSkBitmap)
            {
                delete pSkBitmap;
                pSkBitmap = NULL;
            }
        }
        void Attach(Bitmap *_pBitmap)
        {
            if(_pBitmap)
                pBitmap = _pBitmap;
            if(pBitmap)
            {
                BitmapData data;
                pBitmap->LockBits(&Rect(0,0,pBitmap->GetWidth(),pBitmap->GetHeight()),ImageLockModeRead,PixelFormat32bppPARGB,&data);
                bit = data.Scan0;
                pSkBitmap = new SkBitmap;
				SkImageInfo info = SkImageInfo::Make(pBitmap->GetWidth(), pBitmap->GetHeight(), kN32_SkColorType, kPremul_SkAlphaType);
				pSkBitmap->allocPixels(info);
                pSkBitmap->lockPixels();
                pSkBitmap->setPixels(data.Scan0);
                pSkBitmap->unlockPixels();
                pBitmap->UnlockBits(&data);
            }
        }
    }TGDIPImageInfo;

    struct tagTImageInfo_SKIA : public tagTImageInfo
    {
        TGDIPImageInfo *skImg;
        SkBitmap * GetSkBitmap()
        {
            if(skImg)
                return skImg->pSkBitmap;
            else
                return NULL;
        }
        tagTImageInfo_SKIA()
        {
            skImg = NULL;
        }
        ~tagTImageInfo_SKIA()
        {
            if(skImg)
            {
                delete skImg;
                skImg = NULL;
            }
        }
    };
    typedef tagTImageInfo_SKIA TSKIAImageInfo;

    /////////////////////////////////////////////////////////////////////////////////////
    ///////

    class CIRenderSKIA : public CIRenderCore
    {
    public:
        CIRenderSKIA(void);
        ~CIRenderSKIA(void);

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
		void DCPushClip(HDC hDC = NULL);
		void DCPopClip(HDC hDC = NULL);

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
		void DrawRoundDotRect(const RECT& rc,int width,int height,int nSize, DWORD dwPenColor);
		void FillRoundRect(const RECT& rc, int width, int height, int nSize, DWORD dwPenColor);
		void DrawCircle(int nCenterX,int nCenterY,int nRadius,DWORD dwColor);
		void FillCircle(int nCenterX,int nCenterY,int nRadius,DWORD dwColor);
		void DrawArc(int nCenterX,int nCenterY,int nRadius,float fStartAngle,float fSweepAngle,DWORD dwColor);
    protected:
        SkRect FromRECT(RECT rc);
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

        SkBitmap m_SkBakBitmap;
        SkCanvas *m_pSkBakCanvas;
    private:
        ULONG_PTR m_Token;
    };

}