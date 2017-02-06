#ifndef __UICAPTCHA_H__
#define __UICAPTCHA_H__

#pragma once

namespace UiLib
{
    class UILIB_API CCaptchaUI : public CLabelUI
    {
    public:
        CCaptchaUI();
        ~CCaptchaUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        void SetBgDisturb(LPCTSTR lpszValue);
        void SetNoise(LPCTSTR lpszValue);
        void SetFontDisturb(LPCTSTR lpszValue);
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
        void PaintBkImage();

        bool GenerateCaptchaImg();
        CDuiString GetCaptchaString(){return m_szCaptcha;}

    protected:
        void GenerateBackGround(BYTE* pdata,int width,int height);
        void GenerateVerifyString();
        void PixelDisturb(BYTE* pdata,int width,int height);
        HFONT FontDisturb(int width = 0,int height = 0);

    protected:
        bool m_bbgDisturb;
        bool m_bNoise;
        bool m_bfontDisturb;

        CDuiString m_szCaptcha;
        HBITMAP m_hCaptchaImg;
    };

}	// namespace DuiLib

#endif // __UIBUTTON_H__