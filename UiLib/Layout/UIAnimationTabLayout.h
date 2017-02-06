#ifndef __UIANIMATIONTABLAYOUT_H__
#define __UIANIMATIONTABLAYOUT_H__

#pragma once
#include "../Control/UIAnimation.h"

namespace UiLib
{
	class UILIB_API CAnimationTabLayoutUI : public CTabLayoutUI, public CUIAnimation
	{
	public:
		CAnimationTabLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool SelectItem( int iIndex );
		void AnimationSwitch();
		void DoEvent(TEventUI& event);
		void OnTimer( int nTimerID );
		bool OnInit(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);

		virtual void OnAnimationStart(INT nAnimationID, BOOL bFirstLoop);
		virtual void OnAnimationStep(INT nTotalFrame, INT nCurFrame, INT nAnimationID);
		virtual void OnAnimationStop(INT nAnimationID);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetAnimationDirection(bool bVertical);
	protected:
		bool m_bIsVerticalDirection;
		bool m_bIsRunning;
		int m_nPositiveDirection;
		RECT m_rcCurPos;
		RECT m_rcOldPos;
		RECT m_rcItemOld;
		int m_iOldXOffset;
		int m_iOldYOffset;
		int m_AnimatID;
		int m_nStep;
		enum
		{
			TAB_ANIMATIONEXECUT_ID = 1,
	
			TAB_ANIMATION_ELLAPSE = 10,
			TAB_ANIMATION_FRAME_COUNT = 10,
		};
	};
}
#endif