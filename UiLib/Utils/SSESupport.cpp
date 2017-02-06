#include "StdAfx.h"
#include "SSESupport.h"

namespace UiLib
{

	int CSSE::m_nSupportSSE = -1;

	BOOL CSSE::IsSupportSSE()
	{
		if(m_nSupportSSE == -1)
		{
			__try 
			{
				__asm 
				{
					xorpd xmm0, xmm0        // executing SSE instruction
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER) 
			{
				if (_exception_code() == STATUS_ILLEGAL_INSTRUCTION) 
				{
					m_nSupportSSE = 0;
				}
				m_nSupportSSE = 0;
			}
			if(m_nSupportSSE == -1)
			{
				m_nSupportSSE = 1;
			}
		}
		return m_nSupportSSE == 1;
	}

	void CSSE::MemCopy(LPVOID pDest, LPVOID pSrc, int nSize)
	{
		if(IsSupportSSE())
		{
			MemCopySSE(pDest, pSrc, nSize);
		}
		else
		{
			memcpy(pDest, pSrc, nSize);
		}
	}

	void CSSE::MemCopySSE(LPVOID pDest, LPVOID pSrc, int nSize)
	{
		int nSSESize = nSize >> 4;
		int nTemp = nSSESize << 4;
		nSize -= nTemp;
		if(nSSESize)
		{
			__asm
			{
				push ecx
					push edx
					push eax
					mov	eax, [pSrc]
				mov edx, [pDest]
				mov ecx, [nSSESize]		
memcpy_process:
				movups xmm0, [eax]
				add eax, 10h
					movups [edx], xmm0
					add edx, 10h
					dec ecx
					jnz memcpy_process
					pop eax
					pop edx
					pop ecx
					emms
			}
		}
		if(nSize)
		{
			memcpy((BYTE *)pDest + nTemp, (BYTE *)pSrc + nTemp, nSize);
		}
	}

	void CSSE::DoGray(LPVOID pBuff, int nSize)
	{
		if(IsSupportSSE())
		{
			DoGraySSE(pBuff, nSize);
		}
		else
		{
			DoGrayNormal(pBuff, nSize);
		}
	}

	void CSSE::DoGrayNormal(LPVOID pBuff, int nSize)
	{
		if(nSize <= 0 || nSize % 4)
		{
			return;
		}
		for(DWORD * p = (DWORD *)pBuff; nSize > 0; nSize -= 4, p++)
		{
			BYTE c = (GetRValue(*p) + GetGValue(*p) + (int)GetBValue(*p)) / 3;
			BYTE * pSrc = (BYTE *)p;
			*pSrc++ = c;
			*pSrc++ = c;
			*pSrc++ = c;
		}
	}

	void CSSE::DoGraySSE(LPVOID pBuff, int nSize)
	{
		DWORD dwMask = 0xff;
		DWORD dwDiv3 = 85;
		int nSSESize = nSize >> 4;
		if(nSSESize)
		{
			__asm
			{
				push ecx
					push eax
					mov ecx, [nSSESize]
				movss xmm1, [dwMask]
				shufps xmm1, xmm1, 0
					movss xmm2, [dwDiv3]
				shufps xmm2, xmm2, 0
					movaps xmm3, xmm1
					pslld xmm3, 24
					mov	eax, [pBuff]			
gray_process:
				movups xmm0, [eax]
				movaps xmm4, xmm0
					movaps xmm5, xmm0
					movaps xmm6, xmm0
					andps xmm4, xmm1
					psrld xmm5, 8
					andps xmm5, xmm1
					psrld xmm6, 16
					andps xmm6, xmm1
					paddd xmm4, xmm5
					paddd xmm4, xmm6
					pmullw xmm4, xmm2
					psrld xmm4, 8
					movaps xmm5, xmm4
					pslld xmm5, 8
					orps xmm4, xmm5
					pslld xmm5, 8
					orps xmm4, xmm5
					andps xmm0, xmm3
					orps xmm0, xmm4
					movups [eax], xmm0
					add eax, 10h
					dec ecx
					jnz gray_process
					pop eax
					pop ecx
					emms
			}
		}
		DoGrayNormal((BYTE *)pBuff + (nSSESize << 4), nSize - (nSSESize << 4));
	}

	void CSSE::OpenAlpha(LPVOID pBuff, int nSize)
	{
		if(IsSupportSSE())
		{
			OpenAlphaSSE(pBuff, nSize);
		}
		else
		{
			OpenAlphaNormal(pBuff, nSize);
		}
	}

	void CSSE::OpenAlphaSSE(LPVOID pBuff, int nSize)
	{
		int nSizeSSE = nSize >> 4;
		DWORD dwMask = 0xff000000;
		if(nSizeSSE)
		{
			__asm
			{
				push eax
					push ecx
					movss xmm1, [dwMask]
				shufps xmm1, xmm1, 0	// 0xff00000000¼Ä´æ
					xorps xmm3, xmm3		// 0¼Ä´æ
					mov eax, [pBuff]
				mov ecx, [nSizeSSE]
open_alpha:
				movups xmm0, [eax]
				movaps xmm2, xmm0
					andps xmm2, xmm1
					pcmpeqd xmm2, xmm3
					andps xmm0, xmm2
					andps xmm2, xmm1
					orps xmm0, xmm2
					movups [eax], xmm0
					add eax, 10h
					dec ecx
					jnz open_alpha
					pop ecx
					pop eax
					emms
			}
		}
		OpenAlphaNormal((BYTE *)pBuff + (nSizeSSE << 4), nSize - (nSizeSSE << 4));
	}

	void CSSE::OpenAlphaNormal(LPVOID pBuff, int nSize)
	{
		if(nSize <= 0 || nSize % 4)
		{
			return;
		}
		DWORD * pSrc = (DWORD *)pBuff;
		for(int i = 0; i < nSize; i += 4)
		{
			if(*pSrc & 0xff000000)
			{
				*pSrc = 0;
			}
			else
			{
				*pSrc |= 0xff000000;
			}
			pSrc++;
		}
	}

}