#include "stdafx.h"
#include "AboutWnd.h"

namespace UiLib
{


CAboutWnd::CAboutWnd( void )
{

}

CAboutWnd::~CAboutWnd( void )
{

}

UiLib::CDuiString CAboutWnd::GetSkinFile()
{
    return CDuiString(_T("AboutWnd.xml"));
}

UiLib::CDuiString CAboutWnd::GetSkinFolder()
{
    return CDuiString(CPaintManagerUI::GetInstancePath()+_T("Skins\\ControlTest\\"));
}

void CAboutWnd::Notify( TNotifyUI& msg )
{
    if( msg.sType == DUI_MSGTYPE_WINDOWINIT )
        OnPrepare();

    IWindowBase::Notify(msg);
}

void CAboutWnd::Init()
{
    IWindowBase::Init();
}

void CAboutWnd::OnPrepare()
{
    FindControls();
    InitControls();
    BindControls();
}

void CAboutWnd::FindControls()
{
   
}

void CAboutWnd::InitControls()
{

}

void CAboutWnd::BindControls()
{
   
}

}