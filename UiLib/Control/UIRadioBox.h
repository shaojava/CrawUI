#ifndef UIRadioBox_h__
#define UIRadioBox_h__

#pragma once

namespace UiLib
{
	class UILIB_API CRadioBoxUI : public COptionUI
	{
	public:
		CRadioBoxUI(void);
		~CRadioBoxUI(void);

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
	};
}



#endif // UIRadioBox_h__
