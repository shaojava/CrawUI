#include "StdAfx.h"
#include "CryptographyExt.h"

void Encrypt::EncryptFile( LPCTSTR strSrcPath,LPCTSTR strDstPath,char *strPassword )
{
	FILE *hSource=NULL;
	FILE *hDestination=NULL;
	int eof = 0;
	HCRYPTPROV hProv = 0;
	HCRYPTKEY hKey = 0;
	HCRYPTKEY hXchgKey = 0;
	HCRYPTHASH hHash = 0;
	PBYTE pbKeyBlob = NULL;
	PBYTE pbBuffer = NULL;
	DWORD dwBlockLen;
	DWORD dwBufferLen;
	DWORD dwCount;

	_wfopen_s(&hSource,strSrcPath, L"rb");
	_wfopen_s(&hDestination,strDstPath, L"wb");
	if (!hSource) return;
	if (!hDestination) return;

	if (hSource==NULL) return ;
	if (hDestination==NULL) return;
	CryptAcquireContext(&hProv, NULL, NULL,PROV_RSA_FULL, 0);
	CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);
	CryptHashData(hHash, (const BYTE *)strPassword ,strlen(strPassword), 0);
	CryptDeriveKey(hProv, ENCRYPT_ALGORITHM, hHash, 0, &hKey);
	dwBlockLen = 1000 - 1000 % ENCRYPT_BLOCK_SIZE;
	if( ENCRYPT_BLOCK_SIZE> 1 )
		dwBufferLen=dwBlockLen+ENCRYPT_BLOCK_SIZE;
	else
		dwBufferLen=dwBlockLen;
	pbBuffer=(PBYTE )malloc(dwBufferLen);
	do
	{
		dwCount = fread(pbBuffer, 1, dwBlockLen, hSource);
		eof = feof(hSource);
		CryptEncrypt(hKey, 0, eof, 0, pbBuffer,&dwCount, dwBufferLen);
		fwrite(pbBuffer, 1, dwCount, hDestination);
	} while(!feof(hSource));

	CryptDestroyKey(hKey);
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv,NULL);
	fclose(hSource);
	fclose(hDestination);
}

void Encrypt::DecryptFile( LPCTSTR strSrcPath,LPCTSTR strDstPath,char * strPassword )
{
	FILE *hSource = NULL;
	FILE *hDestination = NULL;
	int eof = 0;
	HCRYPTPROV hProv = 0;
	HCRYPTKEY hKey = 0;
	HCRYPTKEY hXchgKey = 0;
	HCRYPTHASH hHash = 0;
	PBYTE pbKeyBlob = NULL;
	PBYTE pbBuffer = NULL;
	DWORD dwBlockLen;
	DWORD dwBufferLen;
	DWORD dwCount;

	_wfopen_s(&hSource,strSrcPath, L"rb");
	_wfopen_s(&hDestination,strDstPath, L"wb");
	if (!hSource) return;
	if (!hDestination) return;

	CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 0);
	CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);
	CryptHashData(hHash, (const BYTE *)strPassword ,strlen(strPassword) , 0);
	CryptDeriveKey(hProv, ENCRYPT_ALGORITHM,hHash, 0, &hKey);

	dwBlockLen = 1000 - 1000 % ENCRYPT_BLOCK_SIZE;
	if(ENCRYPT_BLOCK_SIZE > 1)
		dwBufferLen =dwBlockLen+ENCRYPT_BLOCK_SIZE;
	else
		dwBufferLen = dwBlockLen;

	pbBuffer =(PBYTE )malloc(dwBufferLen);
	do 
	{
		dwCount = fread(pbBuffer, 1, dwBlockLen, hSource);
		eof = feof(hSource);
		CryptDecrypt(hKey, 0, eof, 0, pbBuffer, &dwCount);
		fwrite(pbBuffer, 1, dwCount, hDestination);
	} while(!feof(hSource));
	CryptDestroyKey(hKey);
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv,NULL);
	fclose(hSource);
	fclose(hDestination);
}

void Encrypt::DecryptMem(char* strPassword,BYTE ** pSrc,DWORD& dwSize)
{
	HCRYPTPROV hProv = 0;
	HCRYPTKEY hKey = 0;
	HCRYPTHASH hHash = 0;
	LPBYTE pDst = new BYTE[dwSize];
	memcpy(pDst,*pSrc,(size_t)dwSize);
	if(!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 0)) return;
	if(!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) return;
	if(!CryptHashData(hHash, (const BYTE *)strPassword , strlen(strPassword) , 0)) return;
	if(!CryptDeriveKey(hProv, ENCRYPT_ALGORITHM,hHash, 0, &hKey)) return;
	CryptDecrypt(hKey, 0, TRUE, 0, pDst, &dwSize);
	SAFE_DELETE(*pSrc);
	*pSrc = pDst;
	CryptDestroyKey(hKey);
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv,NULL);
}
