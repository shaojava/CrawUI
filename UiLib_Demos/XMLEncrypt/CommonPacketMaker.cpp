#include "CommonPacketMaker.h"
#include "XZip.h"
#include "stdafx.h"

void CCommonPacketMaker::EncryptPack( CString szSrcPath,CString szPwd )
{
	CString szDst = szSrcPath;
	int nPos = szSrcPath.Find(_T(".zip"));
	if(nPos == -1)
	{
		szDst += _T(".zip");
		wchar_t *strFile = szDst.GetBuffer();
		HZIP hZip = CreateZip((void *)strFile, 0,ZIP_FILENAME);
		szDst.ReleaseBuffer();
		AddFolderContent(hZip,szSrcPath,_T(""));
		CloseZip(hZip);
	}
	if(!szPwd.IsEmpty())
	{
		USES_CONVERSION;
		CString szTmpPath;
		nPos = szDst.Find(_T(".zip"));
		szTmpPath = szDst.Left(nPos);
		szTmpPath += _T("X.zip");
		char *strPwd = T2A(szPwd);
		Encrypt::EncryptFile(szDst,szTmpPath,strPwd);
		::DeleteFile(szDst);
	}
}

void CCommonPacketMaker::DecryptPack( CString szSrcPath,CString szPwd )
{
	int nPos = szSrcPath.Find(_T("X.zip"));
	CString szDst = szSrcPath.Left(nPos);
	szDst += _T(".zip");
	if(nPos == -1)
	{
		nPos = szSrcPath.Find(_T(".zip"));
		szDst = szSrcPath.Left(nPos);
		szDst += _T("S.zip");
	}
	if(nPos == -1)
		return ;
	if(!szPwd.IsEmpty())
	{
		USES_CONVERSION;
		char *strPwd = T2A(szPwd);
		Encrypt::DecryptFile(szSrcPath,szDst,strPwd);
	}
}
