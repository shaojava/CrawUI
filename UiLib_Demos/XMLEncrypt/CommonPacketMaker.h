#pragma once
#include <atlbase.h>
#include <atlstr.h>

class CCommonPacketMaker
{
public:
	static void EncryptPack(CString szSrcPath,CString szPwd);
	static void DecryptPack(CString szSrcPath,CString szPwd);
};
