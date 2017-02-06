//*********************************************************************************************************************
//												CBinLayoutUI
//												Add by wangshj
//*********************************************************************************************************************
#include "stdafx.h"
#include "UIBinLayout.h"

namespace UiLib
{
	REGIST_DUICLASS(CBinLayoutUI);

	CBinLayoutUI::CBinLayoutUI()
		:m_aControlCellPos(sizeof(CONTROLCELLPOS))
		,m_nCellPadding(10)
		,m_nBinWidth(0)
		,m_nBinHeight(0)
		,m_bAutoCell(FALSE)
		,m_bAutoArrange(TRUE)
		,m_bShowCellLine(false)
		,m_dwCellLineColor(0x0098C45C)
	{
		m_szCell.cx = 120;
		m_szCell.cy = 120;
	}

	LPCTSTR CBinLayoutUI::GetClass() const
	{
		return _T("BinLayoutUI");
	}

	LPVOID CBinLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, GetClass()) == 0 ) return static_cast<CBinLayoutUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}


	void CBinLayoutUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		rc = m_rcItem;

		// Adjust for inset
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;

		//按照项的静态位置进行设置
		int nCount = m_aControlCellPos.GetSize();
		for (int i = 0;i < nCount;i++)
		{
			CONTROLCELLPOS* pControlCellPos = static_cast<CONTROLCELLPOS*>(m_aControlCellPos[i]);
			if (pControlCellPos&&pControlCellPos->pControl)
			{
				RECT rcItem = GetControlPos(*pControlCellPos);
				pControlCellPos->pControl->SetPos(rcItem);
			}
		}
	}

	void CBinLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("binwidth")) == 0 ) SetBinWidth(_ttoi(pstrValue));
		else if (_tcscmp(pstrName, _T("binheight")) == 0) SetBinHeight(_ttoi(pstrValue));
		else if (_tcscmp(pstrName, _T("cellpadding")) == 0) SetCellPadding(_ttoi(pstrValue));
		else if (_tcscmp(pstrName, _T("linecolor")) == 0)
		{
			while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			m_dwCellLineColor = clrColor;
		}
		else if (_tcscmp(pstrName, _T("cellsize")) == 0 ) 
		{
			SIZE szCell = {0,0};
			LPTSTR pstr = NULL;
			szCell.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szCell.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
			SetCellSize(szCell);
		}
		else if( _tcscmp(pstrName, _T("autocell")) == 0 ) SetAutoCell(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("autoarrange")) == 0 ) SetAutoArrange(_tcscmp(pstrValue, _T("true")) == 0);
		else CContainerUI::SetAttribute(pstrName, pstrValue);
	}

	void CBinLayoutUI::DoPaint(const RECT& rcPaint)
	{
		__super::DoPaint(rcPaint);
		// 绘制栅格
		if (m_bShowCellLine)
		{
			CUIRenderCore *pRenderCore = m_pManager->GetRenderCore();
			if(!pRenderCore) return;
			// 绘制横线
			RECT rc ={0,0,0,0};
			int nHeight = GetBinHeight();
			for (int i = 0;i < nHeight; i++)
			{
				rc.left = m_rcItem.left;
				rc.right = m_rcItem.right;
				rc.top = m_rcItem.top + (m_szCell.cy + m_nCellPadding) * i;
				rc.bottom = rc.top;
				pRenderCore->DrawLine(rc,1,m_dwCellLineColor);
				rc.top += m_szCell.cy;
				rc.bottom = rc.top;
				pRenderCore->DrawLine(rc,1,m_dwCellLineColor);
			}
			// 绘制竖线
			int nWidth = GetBinWidth();
			for (int i = 0;i < nWidth;i++)
			{
				rc.top = m_rcItem.top;
				rc.bottom = m_rcItem.bottom;
				rc.left = m_rcItem.left + (m_szCell.cx + m_nCellPadding)*i;
				rc.right = rc.left;
				pRenderCore->DrawLine(rc,1,m_dwCellLineColor);
				rc.left += m_szCell.cx;
				rc.right = rc.left;
				pRenderCore->DrawLine(rc,1,m_dwCellLineColor);
			}

		}
	}


	int CBinLayoutUI::GetCount() const
	{
		return m_aControlCellPos.GetSize();
	}
	bool CBinLayoutUI::Remove(CControlUI* pControl)
	{
		__super::Remove(pControl);
		int  nCount = m_aControlCellPos.GetSize();
		CONTROLCELLPOS* pTemp = NULL;
		for (int i = 0;i < nCount;i++)
		{
			pTemp = (CONTROLCELLPOS*)m_aControlCellPos.GetAt(i);
			if (pTemp)
			{
				if (pTemp->pControl == pControl)
				{
					m_aControlCellPos.Remove(pTemp);
					return TRUE;
				}
			}
		}
		return FALSE;
	}

	void CBinLayoutUI::RemoveAll(bool bUpdate)
	{
		__super::RemoveAll(bUpdate);
		m_aControlCellPos.Empty(TRUE);
	}

	CControlUI* CBinLayoutUI::GetItemAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_aControlCellPos.GetSize() ) return NULL;
		CONTROLCELLPOS* pControlPos =  (CONTROLCELLPOS*)m_aControlCellPos.GetAt(iIndex);
		if (pControlPos)
		{
			return pControlPos->pControl;
		}
		return NULL;
	}

	void CBinLayoutUI::RemoveAllItem(BOOL bDelete)
	{

		if (bDelete)
		{
			RemoveAll();
		}
		else
		{
			m_items.Empty();
			m_aControlCellPos.Empty(TRUE);
		}
	}


	int CBinLayoutUI::GetBinWidth()
	{
		if (m_bAutoCell)
		{
			//
			int nWidth = m_rcItem.right - m_rcItem.left /*- m_rcPadding.left - m_rcPadding.right*/;
			if (nWidth <= 0)
			{
				nWidth = m_cxyFixed.cx;
			}
			m_nBinWidth = nWidth/(m_szCell.cx + m_nCellPadding);
		}
		return m_nBinWidth;
	}

	void CBinLayoutUI::SetBinWidth(int nBinWidth,BOOL bAutoWidth)
	{
		if( nBinWidth <= 0 ) return;
		if (m_nBinWidth == nBinWidth)
		{
			return;
		}
		m_nBinWidth = nBinWidth;
		if (bAutoWidth)
		{
			// 计算布局宽度
			int nWidth = (m_szCell.cx + m_nCellPadding)*m_nBinWidth /*+ m_szInterval.cx*/;
			SetFixedWidth(nWidth);
		}
		//NeedUpdate();
	}
	int CBinLayoutUI::GetBinHeight()
	{
		if (m_bAutoCell)
		{
			// 
			int nHeight =m_rcItem.bottom - m_rcItem.top /*- m_rcPadding.top - m_rcPadding.bottom*/;
			if (nHeight <= 0)
			{
				nHeight = m_cxyFixed.cy;
			}
			m_nBinHeight = nHeight/(m_szCell.cy + m_nCellPadding);
		}
		return m_nBinHeight;
	}
	void CBinLayoutUI::SetBinHeight(int nBinHeight,BOOL bAutoHeight)
	{
		if( nBinHeight <= 0 ) return;
		if (m_nBinHeight == nBinHeight)
		{
			return;
		}
		m_nBinHeight = nBinHeight;
		if (bAutoHeight)
		{
			// 计算布局宽度
			int nHeight = (m_szCell.cy + m_nCellPadding)*m_nBinHeight /*+ m_szInterval.cy*/;
			SetFixedHeight(nHeight);
		}
	}

	SIZE CBinLayoutUI::GetCellSize() const
	{
		return m_szCell;
	}
	void CBinLayoutUI::SetCellSize(SIZE szCell)
	{
		m_szCell = szCell;
	}
	int	 CBinLayoutUI::GetCellPadding()
	{
		return m_nCellPadding;
	}
	void CBinLayoutUI::SetCellPadding(int nCellPadding)
	{
		m_nCellPadding = nCellPadding;
	}

	BOOL CBinLayoutUI::GetAutoCell()
	{
		return m_bAutoCell;
	}
	void CBinLayoutUI::SetAutoCell(BOOL bAutoCell)
	{
		m_bAutoCell = bAutoCell;
	}
	BOOL CBinLayoutUI::GetAutoArrange()
	{
		return m_bAutoArrange;
	}
	void CBinLayoutUI::SetAutoArrange(BOOL bAutoArrange)
	{
		m_bAutoArrange = bAutoArrange;
	}

	BOOL CBinLayoutUI::GetShowCellLine()
	{
		return m_bShowCellLine;
	}
	void CBinLayoutUI::SetShowCellLine(BOOL bShowCellLine)
	{
		m_bShowCellLine = bShowCellLine;
		NeedUpdate();
	}
	void CBinLayoutUI::SetControlCellPos(CControlUI* pControl, RECT& rcCellPos,BASEVALUECOMPFUNC pCompFunc)
	{
		BOOL bFind = FALSE;
		int nCount = m_aControlCellPos.GetSize();
		CONTROLCELLPOS* pTemp = NULL;
		for (int i = 0;i < nCount;i++)
		{
			pTemp = (CONTROLCELLPOS*)m_aControlCellPos.GetAt(i);
			if (pTemp)
			{
				if (pTemp->pControl == pControl)
				{
					bFind = TRUE;
					pTemp->rcCellPos = rcCellPos;
					return;
				}
			}
		}
		// 添加
		CONTROLCELLPOS cellPos;
		cellPos.rcCellPos = rcCellPos;
		cellPos.pControl = pControl;
		if (NULL == pCompFunc)
		{
			m_aControlCellPos.Add(&cellPos);
		}
		else
		{
			m_aControlCellPos.Insert(&cellPos,pCompFunc);
		}
	}

	// 获取空间栅格位置
	CONTROLCELLPOS* CBinLayoutUI::GetControlCellPos(int nIndex)
	{
		int nCount = m_aControlCellPos.GetSize();
		if (nIndex >= nCount)
		{
			return NULL;
		}
		return (CONTROLCELLPOS*)m_aControlCellPos.GetAt(nIndex);
	}

	//获取控件位置
	RECT CBinLayoutUI::GetControlPos(CONTROLCELLPOS& cellPos)
	{
		// 
		RECT rcItem = {0,0,0,0};
		if (cellPos.pControl)
		{
			int nLeft = cellPos.rcCellPos.left;
			int nTop = cellPos.rcCellPos.top;
			int nWidth = cellPos.rcCellPos.right -cellPos.rcCellPos.left;
			int nHeight = cellPos.rcCellPos.bottom -cellPos.rcCellPos.top;
			rcItem.left = m_rcItem.left /*+ m_szInterval.cx*/  + (m_szCell.cx + m_nCellPadding) * nLeft;
			rcItem.right = rcItem.left + (m_szCell.cx + m_nCellPadding)*nWidth - m_nCellPadding;
			rcItem.top = m_rcItem.top + /* m_szInterval.cy  + */(m_szCell.cy + m_nCellPadding) * nTop;
			rcItem.bottom = rcItem.top + (m_szCell.cy + m_nCellPadding)*nHeight - m_nCellPadding;
		}
		return rcItem;
	}

	// 指定区域是否存 存在列表中
	BOOL  CBinLayoutUI::HitTest(POINT& ptLeftTop,SIZE szSize)
	{
		if (ptLeftTop.x + szSize.cx  > m_nBinWidth || ptLeftTop.y + szSize.cy > m_nBinHeight)
		{
			return FALSE;
		}
		RECT rcInsertItem = {ptLeftTop.x,ptLeftTop.y,ptLeftTop.x + szSize.cx,ptLeftTop.y + szSize.cy};
		RECT rcIntersect;
		int nCount = m_aControlCellPos.GetSize();
		CONTROLCELLPOS* pControlPos = NULL;
		for(int i = 0;i < nCount;i++)
		{
			pControlPos = (CONTROLCELLPOS*)m_aControlCellPos.GetAt(i);
			if (pControlPos)
			{
				RECT rc =  pControlPos->rcCellPos;
				if (::IntersectRect(&rcIntersect,&rc,&rcInsertItem))
				{
					return FALSE;
				}
			}
		}
		return TRUE;
	}
	// 指定栅格上的控件
	CControlUI* CBinLayoutUI::GetControl(POINT& ptCell,BASEVALUECOMPFUNC pCompFunc)
	{
		int nCount = m_aControlCellPos.GetSize();
		CONTROLCELLPOS* pControlPos = NULL;
		for(int i = 0;i < nCount;i++)
		{
			pControlPos = (CONTROLCELLPOS*)m_aControlCellPos.GetAt(i);
			if (pControlPos)
			{
				RECT rc =  pControlPos->rcCellPos;
				if (::PtInRect(&rc,ptCell))
				{
					return pControlPos->pControl;
				}
			}
		}
		return GetAdjacentControl(ptCell,pCompFunc);
	}

	// 获取制定位置临近控件 
	CControlUI* CBinLayoutUI::GetAdjacentControl(POINT& ptCell,BASEVALUECOMPFUNC pCompFunc)
	{
		if (NULL == pCompFunc)
		{
			return NULL;
		}
		int nCount = m_aControlCellPos.GetSize();
		//空
		if (nCount==0) return NULL;

		CONTROLCELLPOS objItem;
		objItem.pControl = NULL;
		objItem.rcCellPos.left = ptCell.x;
		objItem.rcCellPos.top = ptCell.y;
		objItem.rcCellPos.right = ptCell.x + 1;
		objItem.rcCellPos.bottom = ptCell.y + 1;

		CONTROLCELLPOS* pItemTmpPos=NULL;
		if (nCount==1)
		{
			//只有单条记录
			pItemTmpPos = (CONTROLCELLPOS*)m_aControlCellPos.GetAt(0);
			if (pCompFunc(&objItem,pItemTmpPos)<=0) return pItemTmpPos->pControl;
			return NULL;
		}
		int n1=0;
		int n2=nCount-1;
		//检查首条记录
		pItemTmpPos=(CONTROLCELLPOS*)m_aControlCellPos.GetAt(n1);
		if (pCompFunc(&objItem,pItemTmpPos)<=0) return pItemTmpPos->pControl;
		//检查最后记录
		pItemTmpPos=(CONTROLCELLPOS*)m_aControlCellPos.GetAt(n2);
		if (pCompFunc(&objItem,pItemTmpPos)>=0) return NULL;

#pragma warning(disable:4127)
		while (TRUE)
#pragma warning(default:4127)
		{
			//只剩两条记录了
			if (n1+1==n2) return ((CONTROLCELLPOS*)m_aControlCellPos.GetAt(n2))->pControl;

			//取中间值
			int n=n1+(n2-n1)/2;
			pItemTmpPos= (CONTROLCELLPOS*)m_aControlCellPos.GetAt(n);
			int nRet=pCompFunc(&objItem,pItemTmpPos);
			if (nRet==0) return pItemTmpPos->pControl;
			else if (nRet< 0) n2=n;
			else n1=n;
		}
	}

	POINT	CBinLayoutUI::PtInCell(POINT& pt)
	{
		//
		POINT ptCellIndex;
		ptCellIndex.x = (pt.x - m_rcItem.left /*- m_szInterval.cx*/)/(m_szCell.cx + m_nCellPadding);
		ptCellIndex.y = (pt.y - m_rcItem.top/* - m_szInterval.cy*/)/(m_szCell.cy + m_nCellPadding);
		return ptCellIndex;
	}

	// 根据坐标点获取区域
	RECT	CBinLayoutUI::GetCellRange(POINT& pt, SIZE szSize)
	{
		RECT rcCellRange;
		memset(&rcCellRange,0,sizeof(RECT));
		POINT ptCellIndex = PtInCell(pt);
		rcCellRange.left = m_rcItem.left + ptCellIndex.x * (m_szCell.cx + m_nCellPadding);
		rcCellRange.top = m_rcItem.top + ptCellIndex.y * (m_szCell.cy + m_nCellPadding);
		rcCellRange.right = rcCellRange.left + szSize.cx*((m_szCell.cx + m_nCellPadding)) - m_nCellPadding;
		rcCellRange.bottom = rcCellRange.top + szSize.cy*(m_szCell.cy + m_nCellPadding)- m_nCellPadding;
		return rcCellRange;
	}
	// 获取指定控件的索引
	int CBinLayoutUI::GetControlIndex(CControlUI* pControl)
	{
		int nCount = m_aControlCellPos.GetSize();
		CONTROLCELLPOS* pControlPos = NULL;
		for(int i = 0;i < nCount;i++)
		{
			pControlPos = (CONTROLCELLPOS*)m_aControlCellPos.GetAt(i);
			if (pControlPos && pControlPos->pControl == pControl)
			{
				return i;
			}
		}
		return -1;
	}
	// 获取指定栅格坐标上控件的索引
	int	CBinLayoutUI::GetControlIndexFromCell(POINT pt)
	{
		int nCount = m_aControlCellPos.GetSize();
		CONTROLCELLPOS* pControlPos = NULL;
		RECT rcTemp;
		for(int i = 0;i < nCount;i++)
		{
			pControlPos = (CONTROLCELLPOS*)m_aControlCellPos.GetAt(i);
			if (pControlPos)
			{
				rcTemp = pControlPos->rcCellPos;
				if (rcTemp.left <= pt.x && rcTemp.right > pt.x && rcTemp.top <= pt.y && rcTemp.bottom > pt.y)
				{
					return i;
				}
			}
		}
		return -1;
	}

}