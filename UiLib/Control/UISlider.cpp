#include "StdAfx.h"
#include "UISlider.h"

namespace UiLib
{
	REGIST_DUICLASS(CSliderUI);

	CSliderUI::CSliderUI() : m_uButtonState(0), m_nStep(1)
	{
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		m_szThumb.cx = m_szThumb.cy = 10;
		m_bNeedCursor = true;
	}

	LPCTSTR CSliderUI::GetClass() const
	{
		return _T("SliderUI");
	}

	UINT CSliderUI::GetControlFlags() const
	{
		if( IsEnabled() ) return IsWantCursor();
		else return 0;
	}

	LPVOID CSliderUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("Slider")) == 0 ) return static_cast<CSliderUI*>(this);
		return CProgressUI::GetInterface(pstrName);
	}

	void CSliderUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	int CSliderUI::GetChangeStep()
	{
		return m_nStep;
	}

	void CSliderUI::SetChangeStep(int step)
	{
		m_nStep = step;
	}

	void CSliderUI::SetThumbSize(SIZE szXY)
	{
		m_szThumb = szXY;
	}

	RECT CSliderUI::GetThumbRect() const
	{
		if( m_bHorizontal ) {
			int left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - m_szThumb.cx) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			int top = (m_rcItem.bottom + m_rcItem.top - m_szThumb.cy) / 2;
			return CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
		else {
			int left = (m_rcItem.right + m_rcItem.left - m_szThumb.cx) / 2;
			int top = m_rcItem.bottom - m_szThumb.cy - (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			return CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
	}

	LPCTSTR CSliderUI::GetThumbImage() const
	{
		return m_sThumbImage;
	}

	void CSliderUI::SetThumbImage(LPCTSTR pStrImage)
	{
		m_sThumbImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CSliderUI::GetThumbHotImage() const
	{
		return m_sThumbHotImage;
	}

	void CSliderUI::SetThumbHotImage(LPCTSTR pStrImage)
	{
		m_sThumbHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CSliderUI::GetThumbPushedImage() const
	{
		return m_sThumbPushedImage;
	}

	void CSliderUI::SetThumbPushedImage(LPCTSTR pStrImage)
	{
		m_sThumbPushedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR CSliderUI::GetThumDisabledImage()
	{
		return m_sThumbDisableImage;
	}
	void CSliderUI::SetThumDisabledImage(LPCTSTR pStrImage)
	{
		m_sThumbDisableImage = pStrImage;
		Invalidate();
	}
	void CSliderUI::DoEvent(TEventUI& event)
	{
		if(!IsEnabled())
		{
			return ;
		}
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) 
				m_pParent->DoEvent(event);
			else 
				CProgressUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
		{
			if( IsEnabled() ) {
				RECT rcThumb = GetThumbRect();
				if( ::PtInRect(&rcThumb, event.ptMouse) ) {
					m_uButtonState |= UISTATE_CAPTURED;
					m_pManager->SendNotify(this, _T("captured"));
				}
			}
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				m_uButtonState &= ~UISTATE_CAPTURED;
				m_pManager->SendNotify(this, _T("release"));
			}
			if( m_bHorizontal ) {
				if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) SetValue(m_nMax);
				else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) SetValue(m_nMin);
				else SetValue(m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx));
			}
			else {
				if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) SetValue(m_nMin);
				else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) SetValue(m_nMax);
				else SetValue(m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy));
			}
			m_pManager->SendNotify(this, _T("valuechanged"));
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		if( event.Type == UIEVENT_SCROLLWHEEL ) 
		{
			switch( LOWORD(event.wParam) ) {
		case SB_LINEUP:
			{
				int nStep = GetValue() - GetChangeStep();
				if(nStep <= GetMinValue())
					nStep = GetMinValue();
				SetValue(nStep);
			}
			m_pManager->SendNotify(this, _T("valuechanging"));
			return;
		case SB_LINEDOWN:
			{
				int nStep = GetValue() + GetChangeStep();
				if(nStep >= GetMaxValue())
					nStep = GetMaxValue();
				SetValue(nStep);
			}
			m_pManager->SendNotify(this, _T("valuechanging"));
			return;
			}
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
				if( m_bHorizontal ) {
					if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) SetValue(m_nMax);
					else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) SetValue(m_nMin);
					else SetValue(m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx));
				}
				else {
					if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) SetValue(m_nMin);
					else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) SetValue(m_nMax);
					else SetValue(m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy));
				}
				m_pManager->SendNotify(this, _T("valuechanging"));
			}
			return;
		}
		if( event.Type == UIEVENT_SETCURSOR )
		{
			RECT rcThumb = GetThumbRect();
			if( IsEnabled() && ::PtInRect(&rcThumb, event.ptMouse) ) {
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
				return;
			}
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			if( IsEnabled() ) {
				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if(event.Type == UIEVENT_KEYDOWN)
		{
			switch( event.chKey ) 
			{
			case VK_LEFT:
				{
					int nStep = GetValue() - GetChangeStep();
					if(nStep <= GetMinValue())
						nStep = GetMinValue();
					SetValue(nStep);
					break;
				}
			case VK_RIGHT:
				{
					int nStep = GetValue() + GetChangeStep();
					if(nStep >= GetMaxValue())
						nStep = GetMaxValue();
					SetValue(nStep);
					break;
				}
			default:
				break;
			}
			m_pManager->SendNotify(this, _T("valuechanged"));
		}
		CControlUI::DoEvent(event);
	}


	void CSliderUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("thumbimage")) == 0 ) SetThumbImage(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbhotimage")) == 0 ) SetThumbHotImage(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbpushedimage")) == 0 ) SetThumbPushedImage(pstrValue);
		else if(_tcscmp(pstrName,_T("thumbdisableimage")) == 0) SetThumDisabledImage(pstrValue);
		else if( _tcscmp(pstrName, _T("thumbsize")) == 0 ) {
			SIZE szXY = {0};
			LPTSTR pstr = NULL;
			szXY.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szXY.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
			SetThumbSize(szXY);
		}
		else if( _tcscmp(pstrName, _T("step")) == 0 ) {
			SetChangeStep(_ttoi(pstrValue));
		}
		else CProgressUI::SetAttribute(pstrName, pstrValue);
	}

	void CSliderUI::PaintStatusImage()
	{
		CProgressUI::PaintStatusImage();
		if( !IsEnabled() ) m_uButtonState |= UISTATE_DISABLED;
		else m_uButtonState &= ~ UISTATE_DISABLED;

		RECT rcThumb = GetThumbRect();
		rcThumb.left -= m_rcItem.left;
		rcThumb.top -= m_rcItem.top;
		rcThumb.right -= m_rcItem.left;
		rcThumb.bottom -= m_rcItem.top;
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
			if( !m_sThumbPushedImage.IsEmpty() ) {
				if( !DrawImage(m_sThumbPushedImage,rcThumb) ) m_sThumbPushedImage.Empty();
				else return;
			}
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			if( !m_sThumbHotImage.IsEmpty() ) {
				if( !DrawImage(m_sThumbHotImage,rcThumb) ) m_sThumbHotImage.Empty();
				else return;
			}
		}
		else if((m_uButtonState & UISTATE_DISABLED) != 0)
		{
			if( !m_sThumbDisableImage.IsEmpty() ) {
				if( !DrawImage(m_sThumbDisableImage, rcThumb) ) m_sThumbDisableImage.Empty();
				else return;
			}
		}
		if( !m_sThumbImage.IsEmpty() ) {
			if( !DrawImage(m_sThumbImage,rcThumb) ) m_sThumbImage.Empty();
			else return;
		}
	}
}
