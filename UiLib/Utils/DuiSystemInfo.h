#pragma once
namespace UiLib
{

class UILIB_API CDuiSystemInfo
{
public:
	CDuiSystemInfo(void);
	~CDuiSystemInfo(void);

	static void GetSystemInfo(OSVERSIONINFO &os);

	static LPOSVERSIONINFOW g_pOS;
	
};

}