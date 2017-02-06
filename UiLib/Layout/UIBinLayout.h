#ifndef __UIBINLAYOUT_H__
#define __UIBINLAYOUT_H__

#pragma once

namespace UiLib
{

typedef struct 
{
	CControlUI* pControl;
	RECT	rcCellPos;		// 控件栅格位置
} CONTROLCELLPOS;

class  UILIB_API CBinLayoutUI :public CContainerUI
{
public:

public:
	CBinLayoutUI(void);
	virtual ~CBinLayoutUI(void){};
	virtual LPCTSTR GetClass() const;
	virtual LPVOID GetInterface(LPCTSTR pstrName);
	virtual void SetPos(RECT rc);
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual void DoPaint(const RECT& rcPaint);

	virtual int GetCount() const;
	virtual bool Remove(CControlUI* pControl);
	virtual void RemoveAll(bool bUpdate = true);
	virtual CControlUI* GetItemAt(int iIndex) const;

public:
	int GetBinWidth() ;
	void SetBinWidth(int nBinWidth,BOOL bAutoWidth = FALSE);
	int GetBinHeight() ;
	void SetBinHeight(int nBinWidth,BOOL bAutoHeight = FALSE);
	SIZE GetCellSize() const;
	void SetCellSize(SIZE szCell);
	int	 GetCellPadding();
	void SetCellPadding(int nCellPadding);
	BOOL GetAutoCell();
	void SetAutoCell(BOOL bAutoCell);
	BOOL GetAutoArrange();
	void SetAutoArrange(BOOL bAutoArrange);

	BOOL GetShowCellLine();
	void SetShowCellLine(BOOL bShowCellLine);
public:
	// 记录控件栅格位置
	// 重载可控制控件列表插入顺序 
	virtual void SetControlCellPos(CControlUI* pControl,RECT& rcCellPos,BASEVALUECOMPFUNC pCompFunc = NULL);
	// 获取空间栅格位置
	CONTROLCELLPOS* GetControlCellPos(int nIndex);
	//获取控件位置
	RECT GetControlPos(CONTROLCELLPOS& cellPos);
	// 根据鼠标点获取栅格坐标
	POINT	PtInCell(POINT& pt);
	// 根据坐标点获取区域
	RECT	GetCellRange(POINT& pt, SIZE szSize);
	// 指定区域是否存 存在列表中
	virtual BOOL  HitTest(POINT& ptLeftTop,SIZE szSize);
	// 指定栅格上的控件
	CControlUI* GetControl(POINT& ptCell,BASEVALUECOMPFUNC pCompFunc = NULL);
	// 获取制定位置临近控件 
	CControlUI* GetAdjacentControl(POINT& ptCell,BASEVALUECOMPFUNC pCompFunc);
	// 删除项
	void RemoveAllItem(BOOL bDelete = TRUE);

public:
	// 获取指定控件的索引
	int GetControlIndex(CControlUI* pControl);
	// 获取指定栅格坐标上控件的索引   pt为栅格坐标
	int	GetControlIndexFromCell(POINT pt);
protected:
	SIZE		m_szCell;			// 栅格大小
	//SIZE		m_szInterval;		// 栅格左上间距
	int			m_nCellPadding;		// 栅格间距
	int			m_nBinWidth;		// Bin宽度		单位:栅格
	int			m_nBinHeight;		// Bin高度		单位:栅格
	BOOL		m_bAutoCell;		// 根据窗口大小计算Bin的宽度和高度
	BOOL		m_bAutoArrange;		// 是否自动对齐
	BOOL		m_bShowCellLine;	// 是否显示栅格线
	DWORD		m_dwCellLineColor;	// 栅格线颜色
	CStdValArray m_aControlCellPos;		// 控件位置
};

}
#endif // __UIBINLAYOUT_H__