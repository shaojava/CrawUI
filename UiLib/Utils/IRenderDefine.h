#pragma once
#include "Internal.h"
#include "CommonDefine.h"

#ifdef RENDER_EXPORTS
#define RENDER_API __declspec(dllexport)
#else
#define RENDER_API __declspec(dllimport)
#endif

#ifdef RENDER_C_EXPORTS
#define RENDER_C_API extern "C" __declspec(dllexport)
#else
#define RENDER_C_API extern "C" __declspec(dllimport)
#endif


#ifndef SAFE_DELETEOBJ
#define SAFE_DELETEOBJ(x) if(x) {::DeleteObject(x);x=NULL;}
#endif

#ifndef SAFE_DELETEDC
#define SAFE_DELETEDC(x) if(x) {::DeleteDC(x);x=NULL;}
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if(x){delete x;x=NULL;} 
#endif

#ifndef SAFE_DELETEARRY
#define SAFE_DELETEARRY(x) if(x){delete []x; x=NULL;}
#endif

#ifndef UN_ZIP
DECLARE_HANDLE(HZIP);	// An HZIP identifies a zip file that has been opened
typedef DWORD ZRESULT;
typedef struct
{ 
	int index;                 // index of this file within the zip
	char name[MAX_PATH];       // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime,ctime,mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRY;
typedef struct
{ 
	int index;                 // index of this file within the zip
	TCHAR name[MAX_PATH];      // filename within the zip
	DWORD attr;                // attributes, as in GetFileAttributes.
	FILETIME atime,ctime,mtime;// access, create, modify filetimes
	long comp_size;            // sizes of item, compressed and uncompressed. These
	long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRYW;
#define OpenZip OpenZipU
#define CloseZip(hz) CloseZipU(hz)
extern HZIP OpenZipU(void *z,unsigned int len,DWORD flags);
extern ZRESULT CloseZipU(HZIP hz);
#ifdef _UNICODE
#define ZIPENTRY ZIPENTRYW
#define GetZipItem GetZipItemW
#define FindZipItem FindZipItemW
#else
#define GetZipItem GetZipItemA
#define FindZipItem FindZipItemA
#endif
extern ZRESULT GetZipItemA(HZIP hz, int index, ZIPENTRY *ze);
extern ZRESULT GetZipItemW(HZIP hz, int index, ZIPENTRYW *ze);
extern ZRESULT FindZipItemA(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze);
extern ZRESULT FindZipItemW(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *ze);
extern ZRESULT UnzipItem(HZIP hz, int index, void *dst, unsigned int len, DWORD flags);
#endif

namespace UiLib
{

    typedef enum RenderWay
    {
        GDI_RENDER,
        SKIA_RENDER,
        D2D_RENDER
    };

    typedef enum ClipOpt
    {
        OP_REPLACE,
        OP_XOR,
        OP_OR,
        OP_DIFF,
        OP_AND
    };

    typedef struct tagTFontInfo
    {
        HFONT hFont;
        TCHAR sFontName[32];
        int iSize;
        bool bBold;
        bool bUnderline;
        bool bItalic;
        TEXTMETRIC tm;
        tagTFontInfo()
        {
            hFont = NULL;
            bBold = false;
            bUnderline = false;
            bItalic = false;
        }
        virtual ~tagTFontInfo()
        {
            if(hFont)
            {
                ::DeleteObject(hFont);
                hFont = NULL;
            }
        }
    } TFontInfo;

    typedef struct tagTImageInfo
    {
        HBITMAP hBitmap;
        int nX;
        int nY;
        bool alphaChannel;
        LPCTSTR sResType;
        DWORD dwMask;
        tagTImageInfo()
        {
			sResType = NULL;
            alphaChannel = false;
            hBitmap = NULL;
        }
        virtual ~tagTImageInfo()
        {
            if(hBitmap)
            {
                ::DeleteObject(hBitmap) ; 
                hBitmap = NULL;
            }
        }
    } TImageInfo;



    typedef struct tagTRenderObj
    {
		tagTRenderObj(){pBits = NULL;hDC = NULL;}
        HDC hDC;
		void* pBits;
    }TRenderObj;


    typedef struct tagImagePrimData
    {
        //Res byte[];
        LPBYTE pData;
        //Actually it's a Bitmap;
        void *pImage;
		HBITMAP hBitmap;
		DWORD dwSize;
		int nX;
		int nY;
		bool bAlphaChannel;

        tagImagePrimData()
        {
            pData = NULL;
            dwSize = 0;
            pImage = NULL;
			hBitmap = NULL;
        }
        //don't provide destrutor as the ImagePrimData just a transputer;
    }TImagePrimData;

    typedef struct tagFontPrimData
    {
        HFONT hFont;
        TCHAR sFontName[32];
        int iSize;
        bool bBold;
        bool bUnderline;
        bool bItalic;
        TEXTMETRIC tm;
        tagFontPrimData()
        {
            hFont = NULL;
            bBold = false;
            bUnderline = false;
            bItalic = false;
        }
    }TFontPrimData;
}