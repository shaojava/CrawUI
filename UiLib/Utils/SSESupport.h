#pragma once

namespace UiLib
{
	class CSSE
	{
	public:
		static BOOL IsSupportSSE();
		static void MemCopy(LPVOID pDest, LPVOID pSrc, int nSize);
		static void DoGray(LPVOID pBuff, int nSize);
		static void OpenAlpha(LPVOID pBuff, int nSize);
	private:
		static void MemCopySSE(LPVOID pDest, LPVOID pSrc, int nSize);
		static void OpenAlphaNormal(LPVOID pBuff, int nSize);
		static void OpenAlphaSSE(LPVOID pBuff, int nSize);
		static void DoGrayNormal(LPVOID pBuff, int nSize);
		static void DoGraySSE(LPVOID pBuff, int nSize);
		static int m_nSupportSSE;
	};

}