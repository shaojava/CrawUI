#include "StdAfx.h"
#include "UIMaskControl.h"

namespace UiLib
{
	REGIST_DUICLASS(CMaskControlUI);

	CMaskControlUI::CMaskControlUI(void)
	{
		m_pMaskTargetControl = NULL;
		m_pTimeLine = NULL;
	}

	CMaskControlUI::~CMaskControlUI(void)
	{
		m_pStoryBoard->Stop(m_pTimeLine);
		SAFE_DELETE(m_pTimeLine);
	}

	bool CMaskControlUI::OnAnimateStart( TimeEvent *pEvent )
	{
		return true;
	}

	bool CMaskControlUI::OnAnimateStep( TimeEvent *pEvent )
	{
		float fLeft,fRight,fTop,fBottom;
		m_pTimeLine->GetPropertyCurrentValue(_T("left"),fLeft);
		m_pTimeLine->GetPropertyCurrentValue(_T("right"),fRight);
		m_pTimeLine->GetPropertyCurrentValue(_T("top"),fTop);
		m_pTimeLine->GetPropertyCurrentValue(_T("bottom"),fBottom);
		RECT rcNew = {fLeft,fTop,fRight,fBottom};
		SetPos(rcNew);
		return true;
	}

	bool CMaskControlUI::OnAnimateComplete( TimeEvent *pEvent )
	{
		return true;
	}

	bool CMaskControlUI::OnAnimateEnd( TimeEvent *pEvent )
	{
		return true;
	}

	void CMaskControlUI::DoInit()
	{
		__super::DoInit();
		SetFloat(true);
	}

	void CMaskControlUI::AnimateTo( RECT rc ,DWORD dwMills)
	{
		if(m_pStoryBoard->GetStoryBoardState() == SB_IDLING || m_pStoryBoard->GetStoryBoardState() == SB_FINSHED)
		{
			SAFE_DELETE(m_pTimeLine);
			m_pTimeLine = new CTimeLineTween();
			m_pTimeLine->SetTweenInfo(dwMills,BACK,EASE_OUT);
			m_fStart = m_rcItem.left;
			m_fEnd = m_rcItem.right;
			m_fStartY = m_rcItem.top;
			m_fEndY = m_rcItem.bottom;

			m_pTimeLine->AddProperty(_T("left"),m_fStart,rc.left);
			m_pTimeLine->AddProperty(_T("right"),m_fEnd,rc.right);
			m_pTimeLine->AddProperty(_T("top"),m_fStartY,rc.top);
			m_pTimeLine->AddProperty(_T("bottom"),m_fEndY,rc.bottom);

			m_pStoryBoard->AddTimeLine(m_pTimeLine);
			m_pManager->AddStoryBoard(m_pStoryBoard);
		}
		else
		{
			m_pStoryBoard->Stop(m_pTimeLine);
			SAFE_DELETE(m_pTimeLine);
			m_pTimeLine = new CTimeLineTween();
			m_pTimeLine->SetTweenInfo(dwMills,BACK,EASE_OUT);
			m_fStart = m_rcItem.left;
			m_fEnd = m_rcItem.right;
			m_fStartY = m_rcItem.top;
			m_fEndY = m_rcItem.bottom;

			m_pTimeLine->AddProperty(_T("left"),m_fStart,rc.left);
			m_pTimeLine->AddProperty(_T("right"),m_fEnd,rc.right);
			m_pTimeLine->AddProperty(_T("top"),m_fStartY,rc.top);
			m_pTimeLine->AddProperty(_T("bottom"),m_fEndY,rc.bottom);

			m_pStoryBoard->AddTimeLine(m_pTimeLine);
			m_pManager->AddStoryBoard(m_pStoryBoard);
		}
	}

	void CMaskControlUI::SetMaskTarget( CControlUI *pControl )
	{
		if(pControl)
			m_pMaskTargetControl = pControl;
	}

	void CMaskControlUI::Event( TEventUI& event )
	{
		if(m_pMaskTargetControl)
			m_pMaskTargetControl->Event(event);
		else
			__super::Event(event);
	}

	LPCTSTR CMaskControlUI::GetClass() const
	{
		if(m_pMaskTargetControl)
			return m_pMaskTargetControl->GetClass();
		else
			return _T("MaskControlUI");
	}

}

