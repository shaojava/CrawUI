#include "StdAfx.h"
#include "UIRadioBox.h"

namespace UiLib
{

	REGIST_DUICLASS(CRadioBoxUI);

	CRadioBoxUI::CRadioBoxUI(void)
	{
	}


	CRadioBoxUI::~CRadioBoxUI(void)
	{
	}

	//************************************
	// Method:    GetClass
	// FullName:  CRadioBoxUI::GetClass
	// Access:    public 
	// Returns:   LPCTSTR
	// Qualifier: const
	// Note:	  
	//************************************
	LPCTSTR CRadioBoxUI::GetClass() const
	{
		return _T("RadioBoxUI");
	}

	//************************************
	// Method:    GetInterface
	// FullName:  CRadioBoxUI::GetInterface
	// Access:    public 
	// Returns:   LPVOID
	// Qualifier:
	// Parameter: LPCTSTR pstrName
	// Note:	  
	//************************************
	LPVOID CRadioBoxUI::GetInterface( LPCTSTR pstrName )
	{
		try
		{
			if( _tcscmp(pstrName, _T("RadioBox")) == 0 ) return static_cast<CRadioBoxUI*>(this);
			return COptionUI::GetInterface(pstrName);
		}
		catch (...)
		{
			throw "CRadioBoxUI::GetInterface";
		}
	}

}

