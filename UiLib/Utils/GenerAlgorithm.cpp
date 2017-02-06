#include "StdAfx.h"
#include "math.h"
#include "GenerAlgorithm.h"

#ifndef M_PI
const double piDouble = 3.14159265358979323846;
const float piFloat = 3.14159265358979323846f;
#else
const double piDouble = M_PI;
const float piFloat = static_cast<float>(M_PI);
#endif

namespace UiLib
{

	CGenerAlgorithm::CGenerAlgorithm(void)
	{
	}


	CGenerAlgorithm::~CGenerAlgorithm(void)
	{
	}

	void CGenerAlgorithm::FixGDIAlpha( void **ppBits,RECT* lprc,int nPitch )
	{
		BYTE* pBits = (BYTE*)*ppBits;
		BYTE* p = NULL;
		BYTE* pEnd = NULL;
		for(int i = lprc->top ; i < lprc->bottom ; i++)
		{
			p  = pBits + i*nPitch;
			for(int j = lprc->left ; j < lprc->right ;j++)
			{
				pEnd  = p + (j<<2)+3;
				if(*pEnd == 0)
					*pEnd = 0xff;
			}
		}
	}

	const float OneThird = 1.0f / 3;
	void CGenerAlgorithm::HSLtoRGB(DWORD* ARGB, float H, float S, float L) 
	{
		const float
			q = 2*L<1?L*(1+S):(L+S-L*S),
			p = 2*L-q,
			h = H/360,
			tr = h + OneThird,
			tg = h,
			tb = h - OneThird,
			ntr = tr<0?tr+1:(tr>1?tr-1:tr),
			ntg = tg<0?tg+1:(tg>1?tg-1:tg),
			ntb = tb<0?tb+1:(tb>1?tb-1:tb),
			R = 255*(6*ntr<1?p+(q-p)*6*ntr:(2*ntr<1?q:(3*ntr<2?p+(q-p)*6*(2.0f*OneThird-ntr):p))),
			G = 255*(6*ntg<1?p+(q-p)*6*ntg:(2*ntg<1?q:(3*ntg<2?p+(q-p)*6*(2.0f*OneThird-ntg):p))),
			B = 255*(6*ntb<1?p+(q-p)*6*ntb:(2*ntb<1?q:(3*ntb<2?p+(q-p)*6*(2.0f*OneThird-ntb):p)));
		*ARGB &= 0xFF000000;
		*ARGB |= RGB( (BYTE)(R<0?0:(R>255?255:R)), (BYTE)(G<0?0:(G>255?255:G)), (BYTE)(B<0?0:(B>255?255:B)) );
	}

	void CGenerAlgorithm::RGBtoHSL(DWORD ARGB, float* H, float* S, float* L) 
	{
		const float
			R = (float)GetRValue(ARGB),
			G = (float)GetGValue(ARGB),
			B = (float)GetBValue(ARGB),
			nR = (R<0?0:(R>255?255:R))/255,
			nG = (G<0?0:(G>255?255:G))/255,
			nB = (B<0?0:(B>255?255:B))/255,
			m = min(min(nR,nG),nB),
			M = max(max(nR,nG),nB);
		*L = (m + M)/2;
		if (M==m) *H = *S = 0;
		else {
			const float
				f = (nR==m)?(nG-nB):((nG==m)?(nB-nR):(nR-nG)),
				i = (nR==m)?3.0f:((nG==m)?5.0f:1.0f);
			*H = (i-f/(M-m));
			if (*H>=6) *H-=6;
			*H*=60;
			*S = (2*(*L)<=1)?((M-m)/(M+m)):((M-m)/(2-M-m));
		}
	}

	DWORD CGenerAlgorithm::AdjustColor( DWORD dwColor, short H, short S, short L )
	{
		if( H == 180 && S == 100 && L == 100 ) return dwColor;
		float fH, fS, fL;
		float S1 = S / 100.0f;
		float L1 = L / 100.0f;
		RGBtoHSL(dwColor, &fH, &fS, &fL);
		fH += (H - 180);
		fH = fH > 0 ? fH : fH + 360; 
		fS *= S1;
		fL *= L1;
		HSLtoRGB(&dwColor, fH, fS, fL);
		return dwColor;
	}

#pragma region GlowText
	enum
	{
		leftLobe = 0,
		rightLobe = 1
	};

	static const int blurSumShift = 15;
	//
	//  d = floor(s * 3*sqrt(2*pi)/4 + 0.5)
	//
	// Takes a two dimensional array with three rows and two columns for the lobes.
	static void calculateLobes(int lobes[][2], float blurRadius, bool shadowsIgnoreTransforms)
	{
		int diameter;
		if (shadowsIgnoreTransforms)
		{
			diameter = max(2, static_cast<int>(floorf((2 / 3.f) * blurRadius))); // Canvas shadow. FIXME: we should adjust the blur radius higher up.
		}
		else 
		{
			float stdDev = blurRadius / 2;
			const float gaussianKernelFactor = 3 / 4.f * sqrtf(2 * piFloat);
			const float fudgeFactor = 0.88f;
			diameter = max(2, static_cast<int>(floorf(stdDev * gaussianKernelFactor * fudgeFactor + 0.5f)));
		}

		if (diameter & 1) 
		{
			// if d is odd, use three box-blurs of size 'd', centered on the output pixel.
			int lobeSize = (diameter - 1) / 2;
			lobes[0][leftLobe] = lobeSize;
			lobes[0][rightLobe] = lobeSize;
			lobes[1][leftLobe] = lobeSize;
			lobes[1][rightLobe] = lobeSize;
			lobes[2][leftLobe] = lobeSize;
			lobes[2][rightLobe] = lobeSize;
		} 
		else
		{
			// if d is even, two box-blurs of size 'd' (the first one centered on the pixel boundary
			// between the output pixel and the one to the left, the second one centered on the pixel
			// boundary between the output pixel and the one to the right) and one box blur of size 'd+1' centered on the output pixel
			int lobeSize = diameter / 2;
			lobes[0][leftLobe] = lobeSize;
			lobes[0][rightLobe] = lobeSize - 1;
			lobes[1][leftLobe] = lobeSize - 1;
			lobes[1][rightLobe] = lobeSize;
			lobes[2][leftLobe] = lobeSize;
			lobes[2][rightLobe] = lobeSize;
		}
	}

	void blurLayerImage(byte* imageData, const SIZE& size, int rowStride, RECT* prcBlur, int nBlurRadius)
	{
		const int channels[4] = { 3, 0, 1, 3 }; 

		int lobes[3][2];
		calculateLobes(lobes, (float)nBlurRadius, false);

		int stride = 4; 
		int pitch = rowStride;

		int jstart = prcBlur->top;
		int jfinal = prcBlur->bottom;

		int dim = prcBlur->right - prcBlur->left;     
		for (int pass = 0; pass < 2; ++pass) 
		{
			unsigned char* pixels = imageData + prcBlur->top*pitch + prcBlur->left*stride;

			if (!pass && !nBlurRadius)
				jfinal = jstart;

			for (int j = jstart; j < jfinal; ++j, pixels += pitch) 
			{

				for (int step = 0; step < 3; ++step) 
				{
					int side1 = lobes[step][leftLobe];
					int side2 = lobes[step][rightLobe];
					int pixelCount = side1 + 1 + side2;
					int invCount = ((1 << blurSumShift) + pixelCount - 1) / pixelCount;
					int ofs = 1 + side2;
					int firstalpha = pixels[channels[step]];
					int lastalpha = pixels[(dim - 1) * stride + channels[step]];

					unsigned char* ptr = pixels + channels[step + 1]; 
					unsigned char* prev = pixels + stride + channels[step];  
					unsigned char* next = pixels + ofs * stride + channels[step];

					int i;
					int sum = side1 * firstalpha + firstalpha;
					int limit = (dim < side2 + 1) ? dim : side2 + 1;

					for (i = 1; i < limit; ++i, prev += stride) 
						sum += *prev;

					if (limit <= side2)
						sum += (side2 - limit + 1) * lastalpha;

					limit = (side1 < dim) ? side1 : dim;
					for (i = 0; i < limit; ptr += stride, next += stride, ++i, ++ofs) 
					{
						*ptr = (sum * invCount) >> blurSumShift;  // avg: pixel = sum / count
						sum += ((ofs < dim) ? *next : lastalpha) - firstalpha;  // nextsum = sum + next - prev
					}

					prev = pixels + channels[step];
					for (; ofs < dim; ptr += stride, prev += stride, next += stride, ++i, ++ofs) 
					{
						*ptr = (sum * invCount) >> blurSumShift;
						sum += (*next) - (*prev);
					}

					for (; i < dim; ptr += stride, prev += stride, ++i) 
					{
						*ptr = (sum * invCount) >> blurSumShift;
						sum += lastalpha - (*prev);
					}
				}
			}

			// Last pass is vertical.
			stride = rowStride;
			pitch = 4;
			jstart = prcBlur->left;
			jfinal = prcBlur->right;
			dim = prcBlur->bottom - prcBlur->top;
		}
	}

	void CGenerAlgorithm::ShadowBlur( void **ppBits,int nPitch, COLORREF colorShadow, RECT rcBlur, int nRadius )
	{
		SIZE s = {rcBlur.right-rcBlur.left, rcBlur.bottom-rcBlur.top};

		BYTE* pBits = (BYTE*)*ppBits;

		// blur
		blurLayerImage(pBits, s, nPitch, &rcBlur, nRadius);


		byte r = GetRValue(colorShadow);
		byte g = GetGValue(colorShadow);
		byte b = GetBValue(colorShadow);
		byte* pLineBit = pBits + rcBlur.top*nPitch;

		if (RGB(r,g,b) == RGB(255,255,255))
		{
			for (int y = rcBlur.top; y < rcBlur.bottom; y++)
			{
				byte* pBit = pLineBit + rcBlur.left * 4;
				for (int x = rcBlur.left; x < rcBlur.right; x++, pBit+=4)
				{
					pBit[0] = pBit[3];
					pBit[1] = pBit[3];
					pBit[2] = pBit[3];
				}
				pLineBit += nPitch;
			}
		}
		else if (RGB(r,g,b) == RGB(0,0,0))
		{
			for (int y = rcBlur.top; y < rcBlur.bottom; y++)
			{
				byte* pBit = pLineBit + rcBlur.left * 4;
				for (int x = rcBlur.left; x < rcBlur.right; x++, pBit+=4)
				{
					*((DWORD*)pBit) = pBit[3]<<24;
				}
				pLineBit += nPitch;
			}
		}
		else
		{
			for (int y = rcBlur.top; y < rcBlur.bottom; y++)
			{
				byte* pBit = pLineBit + rcBlur.left * 4;
				for (int x = rcBlur.left; x < rcBlur.right; x++, pBit+=4)
				{
					pBit[0] = r * pBit[3] >> 8;
					pBit[1] = g * pBit[3] >> 8;
					pBit[2] = b * pBit[3] >> 8;
				}
				pLineBit += nPitch;
			}
		}
	}
#pragma endregion

	Bitmap * CGenerAlgorithm::CreateBitmapFromHBITMAP(IN HBITMAP hBitmap)
	{
		BITMAP bmp = {0};
		::GetObject(hBitmap,sizeof(BITMAP),(LPVOID)&bmp);
		BYTE * piexlsSrc = NULL ;
		LONG cbSize = bmp . bmWidthBytes * bmp . bmHeight ;
		piexlsSrc = new BYTE [ cbSize ];
		BITMAPINFO bmpInfo = { 0 };
		bmpInfo . bmiHeader . biSize = sizeof ( BITMAPINFOHEADER );
		bmpInfo . bmiHeader . biWidth = bmp . bmWidth ;
		bmpInfo . bmiHeader . biHeight = bmp . bmHeight ;
		bmpInfo . bmiHeader . biPlanes = bmp . bmPlanes ;
		bmpInfo . bmiHeader . biBitCount = bmp . bmBitsPixel ;
		bmpInfo . bmiHeader . biCompression = BI_RGB ;
		HDC hdcScreen = CreateDC (L"DISPLAY" , NULL , NULL , NULL );
		LONG cbCopied = GetDIBits(hdcScreen , hBitmap , 0, bmp . bmHeight ,piexlsSrc , & bmpInfo , DIB_RGB_COLORS );
		DeleteDC ( hdcScreen );
		if ( 0 == cbCopied )
		{
			SAFE_DELETEARRY(piexlsSrc)
			SAFE_DELETEOBJ(hBitmap);
			return FALSE ;
		}
		Bitmap * pBitmap = new Bitmap ( bmp . bmWidth , bmp . bmHeight , PixelFormat32bppPARGB );
		BitmapData bitmapData ;
		Rect rect (0, 0, bmp . bmWidth , bmp . bmHeight );
		if ( Ok != pBitmap -> LockBits (& rect , ImageLockModeRead ,PixelFormat32bppPARGB , & bitmapData ) )
		{
			SAFE_DELETE(pBitmap);
			SAFE_DELETEARRY(piexlsSrc)
			SAFE_DELETEOBJ(hBitmap);
			return NULL ;
		}
		BYTE * pixelsDest = ( BYTE *) bitmapData . Scan0 ;
		int nLinesize = bmp . bmWidth * sizeof ( UINT );
		int nHeight = bmp . bmHeight ;
		for ( int y = 0; y < nHeight ; y ++ )
			memcpy_s (( pixelsDest + y * nLinesize ),nLinesize ,( piexlsSrc + ( nHeight - y - 1) * nLinesize ),nLinesize );
		if ( Ok != pBitmap -> UnlockBits (& bitmapData ) )
		{
			SAFE_DELETE(pBitmap);
		}
		SAFE_DELETEARRY(piexlsSrc)
		SAFE_DELETEOBJ(hBitmap);
		return pBitmap ;
	}
}