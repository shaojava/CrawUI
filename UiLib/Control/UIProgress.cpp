#include "stdafx.h"
#include "UIProgress.h"

namespace UiLib
{
	REGIST_DUICLASS(CProgressUI);

	CProgressUI::CProgressUI() : m_bHorizontal(true), m_nMin(0), m_nMax(100), m_nValue(0), m_bStretchForeImage(true)
	{
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		SetFixedHeight(12);
		m_nTextStyle = 0;
	}

	LPCTSTR CProgressUI::GetClass() const
	{
		return _T("ProgressUI");
	}

	LPVOID CProgressUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("Progress")) == 0 ) return static_cast<CProgressUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	bool CProgressUI::IsHorizontal()
	{
		return m_bHorizontal;
	}

	void CProgressUI::SetHorizontal(bool bHorizontal)
	{
		if( m_bHorizontal == bHorizontal ) return;

		m_bHorizontal = bHorizontal;
		Invalidate();
	}

	int CProgressUI::GetMinValue() const
	{
		return m_nMin;
	}

	void CProgressUI::SetMinValue(int nMin)
	{
		m_nMin = nMin;
		Invalidate();
	}

	int CProgressUI::GetMaxValue() const
	{
		return m_nMax;
	}

	void CProgressUI::SetMaxValue(int nMax)
	{
		m_nMax = nMax;
		Invalidate();
	}

	int CProgressUI::GetValue() const
	{
		return m_nValue;
	}

	void CProgressUI::SetValue(int nValue)
	{
		if(nValue == m_nValue || nValue<m_nMin || nValue > m_nMax)
			return;
		m_nValue = nValue;
		Invalidate();
	}

	LPCTSTR CProgressUI::GetForeImage() const
	{
		return m_sForeImage;
	}

	void CProgressUI::SetForeImage(LPCTSTR pStrImage)
	{
		if( m_sForeImage == pStrImage ) return;

		m_sForeImage = pStrImage;
		Invalidate();
	}

	void CProgressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("foreimage")) == 0 ) SetForeImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hor")) == 0 ) SetHorizontal(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("min")) == 0 ) SetMinValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("max")) == 0 ) SetMaxValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("value")) == 0 ) SetValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("isstretchfore"))==0) SetStretchForeImage(_tcscmp(pstrValue, _T("true")) == 0? true : false);
		else if( _tcscmp(pstrName, _T("showtextstyle")) == 0 )
			m_nTextStyle = _ttoi(pstrValue);
		else CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CProgressUI::PaintStatusImage()
	{
		if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
		if( m_nValue > m_nMax ) m_nValue = m_nMax;
		if( m_nValue < m_nMin ) m_nValue = m_nMin;

		RECT rc = {0};
		if( m_bHorizontal ) {
			rc.right = (m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
			rc.bottom = m_rcItem.bottom - m_rcItem.top;
		}
		else {
			rc.top = (m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
			rc.right = m_rcItem.right - m_rcItem.left;
			rc.bottom = m_rcItem.bottom - m_rcItem.top;
		}

		if(rc.left == rc.right || rc.top == rc.bottom)
			return;

		if( !m_sForeImage.IsEmpty() ) 
		{
			RECT rcOldDest = m_sForeImage.GetDest(),rcOldSource = m_sForeImage.GetSource();
			if(m_bStretchForeImage)
				m_sForeImage.SetDest(rc);

			if(!m_bStretchForeImage)
				m_sForeImage.SetSource(rc);

			DrawImage(m_sForeImage);

			if(m_nTextStyle)
			{
				CDuiString szProgress;
				szProgress.Format(_T("%d%%"),m_nValue);
				RECT rcText = m_rcItem;
				if(m_nTextStyle == 1)
				{
					m_uTextStyle |= DT_VCENTER|DT_CENTER;
					rcText.right = (m_rcItem.right-m_rcItem.left)*m_nValue*0.01;
				}
				else if(m_nTextStyle == 2)
				{
					m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_RIGHT;
					rcText.right = (m_rcItem.right-m_rcItem.left)*m_nValue*0.01;
				}
				else
					m_uTextStyle |= DT_VCENTER | DT_RIGHT;
				m_pManager->GetRenderCore()->DrawText(m_pManager, rcText, szProgress, m_dwTextColor,m_iFont, m_uTextStyle,m_bTextGlow);
			}

			if(m_bStretchForeImage)
				m_sForeImage.SetDest(rcOldDest);

			if(!m_bStretchForeImage)
				m_sForeImage.SetSource(rcOldSource);
		}
	}

	bool CProgressUI::IsStretchForeImage()
	{
		return m_bStretchForeImage;
	}

	void CProgressUI::SetStretchForeImage( bool bStretchForeImage /*= true*/ )
	{
		if (m_bStretchForeImage==bStretchForeImage)		return;
		m_bStretchForeImage=bStretchForeImage;
		Invalidate();
	}
}
