#pragma once

#include <wincrypt.h>

#ifdef USE_BLOCK_CIPHER
#define ENCRYPT_ALGORITHM CALG_RC2
#define ENCRYPT_BLOCK_SIZE 8
#else
#define ENCRYPT_ALGORITHM CALG_RC4
#define ENCRYPT_BLOCK_SIZE 1
#endif

class UILIB_API Encrypt 
{
public:
	static void EncryptFile(LPCTSTR strSrcPath,LPCTSTR strDstPath,char *strPassword);
	static void DecryptFile(LPCTSTR strSrcPath,LPCTSTR strDstPath,char *strPassword);
	static void DecryptMem(char* strPassword,BYTE ** pSrc,DWORD& dwSize);
};