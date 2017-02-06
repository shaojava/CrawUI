#pragma once
#if defined(UILIB_EXPORTS)
#if defined(_MSC_VER)
#define UILIB_API __declspec(dllexport)
#else
#define UILIB_API 
#endif
#else
#if defined(_MSC_VER)
#define UILIB_API __declspec(dllimport)
#else
#define UILIB_API 
#endif
#endif


#define UILIB_COMDAT __declspec(selectany) 

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <atlbase.h>
#include <atlstr.h>
#include <xstring>

#pragma comment(lib,"oledlg.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"comctl32.lib")

#include "Utils/GdiPlusEnv.h"
#include "Utils/GenerAlgorithm.h"
#include "Utils/SSESupport.h"
#include "Utils/DuiPointer.h"
#include "Utils/Utils.h"
#include "Utils/Internal.h"
#include "Utils/UIDelegate.h"
#include "Utils/DuiAutoComplete.h"
#include "Utils/DuiTrayIcon.h"
#include "Utils/DuiHardwareInfo.h"
#include "Utils/observer_impl_base.h"

#include "Animation/TimeLine.h"
#include "Animation/TimeLineTween.h"
#include "Animation/TimeLineBezier.h"
#include "Animation/TimeLineDecay.h"
#include "Animation/TimeLineSpring.h"

#include "Core/UITimer.h"
#include "Core/UIDefine.h"
#include "Core/UIBase.h"
#include "Core/UIManager.h"

#ifdef UILIB_D3D
#include "Core/UIDxAnimation.h"
#endif // UILIB_D3D

#include "Core/UIControl.h"
#include "Core/UIContainer.h"
#include "Core/UIMarkup.h"
#include "Core/UIDlgBuilder.h"
#include "Core/UIRender.h"

#include "Layout/UIVerticalLayout.h"
#include "Layout/UIHorizontalLayout.h"
#include "Layout/UITileLayout.h"
#include "Layout/UITabLayout.h"
#include "Layout/UIChildLayout.h"
#include "Layout/UIAnimationTabLayout.h"

#include "Control/UICaret.h"
#include "Control/UIWndContainer.h"
#include "Control/UIList.h"
#include "Control/UICombo.h"
#include "Control/UIScrollBar.h"
#include "Control/UITreeView.h"
#include "Control/UiListView.h"
#include "Control/UIChartView.h"


#include "Control/UILabel.h"
#include "Control/UIText.h"
#include "Control/UIEdit.h"
#include "Control/UIMenu.h"
#include "Control/UIGifAnim.h"

#include <vector>
#include <algorithm>
#include "Control/UIAnimation.h"
#include "Control/UIFadeButton.h"
#include "Control/UIButton.h"
#include "Control/UIOption.h"
#include "Control/UICheckBox.h"
#include "Control/UIRadioBox.h"

#include "Control/UITab.h"
#include "Control/UIDateTime.h"

#include "Control/UIProgress.h"
#include "Control/UISlider.h"

#include "Control/UIComboBox.h"
#include "Control/UIRichEdit.h"
#include "Control/UICalendar.h"

#include "Control/UIActiveX.h"
#include "Control/UIWebBrowser.h"
#include "Control/UIFlash.h"

#include "Control/UIToolTips.h"

#include "AnimateControl/UIMaskControl.h"
#include "AnimateControl/UILocker.h"

#include "Utils/IWindowBase.h"
#include "Utils/WinImplBase.h"
#include "Utils/CryptographyExt.h"
