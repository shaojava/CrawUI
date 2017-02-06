#pragma once
#include "../Utils/IRenderDefine.h"

namespace UiLib
{

    class CIRenderCore
    {
    public:
        CIRenderCore(){}
        virtual ~CIRenderCore(){}

        virtual void SetPaintWindow(HWND hWnd){};
        virtual HWND GetPaintWindow(){return NULL;}

        virtual int GetOffBmpPitch(){return -1;}
        virtual void *GetOffBmpPiexl(){return NULL;}
        
        virtual void SetWindowGlass(bool bGlass = true){}
		virtual bool IsWindowGlass(){return false;}
        virtual void SetWindowLayered(bool bLayered = true){}
        virtual bool IsWindowLayered(){return false;}

		virtual void TranslateRO(float fTransX,float fTransY){}

        virtual void BeginDraw(RECT *rc = NULL){}
        virtual void EndDraw(PAINTSTRUCT &ps){}

        virtual void SetRenderObj(TRenderObj* pRenderObj,int nCx = -1,int nCy = -1){}
        virtual HDC GetRenderHDC(BOOL bOffScreen = TRUE){return NULL;}
        virtual void FreeRenderHDC(){}
        virtual void ReleaseRenderObj(){}

        virtual void Clear(){}
        virtual void Clear(const RECT &rcClear){}

		virtual void GetClipBox(RECT &rcClip){}
        virtual void PushClip(const RECT& clipRect,RECT *rcRound = NULL,int nRoundX = 0,int nRoundY = 0,ClipOpt _ClipOpt = OP_REPLACE){}
        virtual void PopClip(const RECT & clipRect){}
		virtual void DCPushClip(HDC hDC = NULL){}
		virtual void DCPopClip(HDC hDC = NULL){}

        virtual void Resize(){}

        virtual TImageInfo* LoadImage(TImagePrimData &ImagePrimData){return NULL;}
        virtual TFontInfo* GetDefaultFontInfo(){return NULL;}
        virtual void SetDefaultFont(TFontPrimData &FontPrimData){}
        virtual TFontInfo* AddFont(TFontPrimData &FontPrimData){return NULL;}
    
        // date,current item destrc||current invalide rc||item image rc||nine in one rc;
        virtual void DrawImage(const TImageInfo* ImgDate, const RECT& rc, const RECT& rcPaint, const RECT& rcBmpPart, const RECT& rcCorners, bool alphaChannel, BYTE uFade = 255, bool hole = false, bool xtiled = false, bool ytiled = false){}

		virtual void GetTextBounds(int& nWidth,int& nHeight,TFontInfo *pFontInfo,LPCTSTR pstrText,UINT uStyle){}
        virtual void DrawText(TFontInfo *pFontInfo, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor,UINT uStyle){}
        //virtual void DrawHtmlText(....................);
		//cause htmlformat with GDI,so we implement it in UIRenderCore with GDI;
        virtual void DrawColor(const RECT& rc, DWORD color){}
        virtual void DrawGradient(const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical){}
        virtual void DrawLine(const RECT& rc, int nSize, DWORD dwPenColor,int nStyle = PS_SOLID){}
        virtual void DrawRect(const RECT& rc, int nSize, DWORD dwPenColor){}
		virtual void DrawRoundRect(const RECT& rc, int width, int height, int nSize, DWORD dwPenColor){}
		virtual void DrawRoundDotRect( const RECT& rc,int width,int height,int nSize, DWORD dwPenColor){}
		virtual void FillRoundRect(const RECT& rc, int width, int height, int nSize, DWORD dwPenColor){}
		virtual void DrawCircle(int nCenterX,int nCenterY,int nRadius,DWORD dwColor){}
		virtual void FillCircle(int nCenterX,int nCenterY,int nRadius,DWORD dwColor){}
		virtual void DrawArc(int nCenterX,int nCenterY,int nRadius,float fStartAngle,float fSweepAngle,DWORD dwColor){}
    };

}