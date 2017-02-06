#pragma once
#include "../../Common/Include/UiLib/Utils/IWindowBase.h"

namespace UiLib
{

//////////////////////////////////////////////////////////////////////////
//
class CAboutWnd: public IWindowBase
{
public:
    CAboutWnd(void);
    virtual ~CAboutWnd(void);
public:
    virtual CDuiString GetSkinFile();
    virtual CDuiString GetSkinFolder();
    virtual void Notify(TNotifyUI& msg);
    virtual void Init();

protected:
    void OnPrepare();
    void FindControls();
    void InitControls();
    void BindControls();
};

} // UiLib