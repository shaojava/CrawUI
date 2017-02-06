#include "StdAfx.h"
#include "DuiSystemInfo.h"

namespace UiLib
{

LPOSVERSIONINFOW CDuiSystemInfo::g_pOS = NULL;

CDuiSystemInfo::CDuiSystemInfo(void)
{
}


CDuiSystemInfo::~CDuiSystemInfo(void)
{
}

void CDuiSystemInfo::GetSystemInfo( OSVERSIONINFO& OS )
{
	if(g_pOS)
	{
		OS = *g_pOS;
		return;
	}
	else
	{
		g_pOS = new OSVERSIONINFO;
		memset(g_pOS,0,sizeof(OSVERSIONINFO));
		GetVersionEx(g_pOS);
	}
}

}