#pragma once
#include "CommonDefine.h"
namespace UiLib
{

	class UILIB_API CGenerAlgorithm
	{
	public:
		CGenerAlgorithm(void);
		~CGenerAlgorithm(void);

		static int F2I(float f)
		{
			int n;
			FLOAT_TO_INT(f,n);
			return n;
		}
		static void FixGDIAlpha(void **ppBits,RECT* lprc,int nPitch);
		
		static DWORD AdjustColor( DWORD dwColor, short H, short S, short L );
		static void RGBtoHSL(DWORD ARGB, float* H, float* S, float* L);
		static void HSLtoRGB(DWORD* ARGB, float H, float S, float L);

		static void  ShadowBlur(void **ppBits,int nPitch, COLORREF colorShadow, RECT rcBlur, int nRadius);
		static Bitmap* CreateBitmapFromHBITMAP(IN HBITMAP hBitmap);
	};

}
