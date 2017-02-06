#ifndef __UITABCTRL_H__
#define __UITABCTRL_H__

#pragma once

namespace UiLib
{

//////////////////////////////////////////////////////////////////////////
//
	class CTabBodyUI;

	class UILIB_API CTabUI : public CHorizontalLayoutUI
	{
	public:
		CTabUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void PaintBkColor();
		void PaintBkImage();
		void DoEvent(TEventUI& event);

		bool Add(CControlUI* pControl);
		bool AddAt(CControlUI* pControl, int iIndex);
		bool Remove(CControlUI* pControl);
		void RemoveAll(bool bUpdate = true);
		int GetCurSel() const;
		bool SelectItem(int iIndex);
		bool SelectItem(CControlUI* pControl);
		CControlUI* GetItemAt(int iIndex) const;
		int	GetItemIndex(CControlUI* pControl) const;
		int GetCount() const;

		void SetPos(RECT rc);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		//滚动速度
		int GetScrollSpeed();
		void SetScrollSpeed(int nSpeed);

		//按钮间隔
		int GetBtnInterval();
		void SetBtnInterval(int nInterval);

		//Tab间隔
		int GetInterval();
		void SetInterval(int nInterval);

		//左侧按钮图像
		LPCTSTR GetLNoramlImage();
		void SetLNormalImage(LPCTSTR pstrName);
		LPCTSTR GetLForeImage();
		void SetLForeImage(LPCTSTR pstrName);
		LPCTSTR GetLHotImage();
		void SetLHotImage(LPCTSTR pstrName);
		LPCTSTR GetLHotForeImage();
		void SetLHotForeImage(LPCTSTR pstrName);
		LPCTSTR GetLPushedImage();
		void SetLPushedImage(LPCTSTR pstrName);
		LPCTSTR GetLPushedForeImage();
		void SetLPushedForeImage(LPCTSTR pstrName);
		LPCTSTR GetLFocusedImage();
		void SetLFocusedImage(LPCTSTR pstrName);
		LPCTSTR GetLDisabledImage();
		void SetLDisabledImage(LPCTSTR pstrName);

		//右侧按钮图像
		LPCTSTR GetRNoramlImage();
		void SetRNormalImage(LPCTSTR pstrName);
		LPCTSTR GetRForeImage();
		void SetRForeImage(LPCTSTR pstrName);
		LPCTSTR GetRHotImage();
		void SetRHotImage(LPCTSTR pstrName);
		LPCTSTR GetRHotForeImage();
		void SetRHotForeImage(LPCTSTR pstrName);
		LPCTSTR GetRPushedImage();
		void SetRPushedImage(LPCTSTR pstrName);
		LPCTSTR GetRPushedForeImage();
		void SetRPushedForeImage(LPCTSTR pstrName);
		LPCTSTR GetRFocusedImage();
		void SetRFocusedImage(LPCTSTR pstrName);
		LPCTSTR GetRDisabledImage();
		void SetRDisabledImage(LPCTSTR pstrName);

		//得到Tab项相关的URL
		CDuiString GetItemUrl(int nIndex);
		//设置Tab项相关的URL
		void SetItemUrl(int nIndex, LPCTSTR lpszUrl);

		//得到Tab项相关的Text
		CDuiString GetItemText(int nIndex);
		//设置Tab项相关的Text
		void SetItemText(int nIndex, LPCTSTR lpszText);

		//得到Tab项相关的ForeImage
		LPCTSTR GetItemForeImage(int nIndex);
		//设置Tab项相关的ForeImage
		void SetItemForeImage(int nIndex, LPCTSTR pstrName);

	protected:
		//得到所有项目的长度
		int GetItemsLength();
		//是否显示左侧按钮
		BOOL IsShowLBtn();
		//是否显示右侧按钮
		BOOL IsShowRBtn();
		//左滚动
		void ScrollLeft();
		//右滚动
		void ScrollRight();
		//绘制左按钮
		void DrawLButton();
		//绘制右按钮
		void DrawRButton();
		//绘制左按钮图像
		void DrawLButtonImage();
		//绘制右按钮图像
		void DrawRButtonImage();
		//得到图像文件
		void GetImageFileName(LPCTSTR pstrName, TCHAR* pstrRes);
		//得到某项的可见状态 = 0 完全可见 = 1 部分可见 = 2 左侧与边界相连 = 3 右侧与边界相连 = 4 完全不可见 = -1 不存在该控件
		int GetItemVisible(int nIndex);

	protected:
		int				m_iCurSel;				//当前选中项索引
		int				m_nSpeed;				//滚动动画速度
		int				m_nBtnInterval;			//按钮间隔
		RECT			m_rcLBtn;				//左侧滚动按钮区域
		RECT			m_rcRBtn;				//右侧滚动按钮区域
		DWORD			m_uLBtnState;			//左侧滚动按钮状态
		DWORD			m_uRBtnState;			//右侧滚动按钮状态
		CTabBodyUI*		m_pTabList;				//TabItem容器

		CDuiImage		m_sLNormalImage;		//左按钮Normal图像
		CDuiImage		m_sLForeImage;			//左按钮Normal前置图像
		CDuiImage		m_sLHotImage;			//左按钮Hover图像
		CDuiImage		m_sLHotForeImage;		//左按钮Hover前置图像
		CDuiImage		m_sLPushedImage;		//左按钮Push图像
		CDuiImage		m_sLPushedForeImage;	//左按钮Push前置图像
		CDuiImage		m_sLFocusedImage;		//左按钮Focus图像
		CDuiImage		m_sLDisabledImage;		//左按钮Disable图像

		CDuiImage		m_sRNormalImage;		//右按钮Normal图像
		CDuiImage		m_sRForeImage;			//右按钮Normal前置图像
		CDuiImage		m_sRHotImage;			//右按钮Hover图像
		CDuiImage		m_sRHotForeImage;		//右按钮Hover前置图像
		CDuiImage		m_sRPushedImage;		//右按钮Push图像
		CDuiImage		m_sRPushedForeImage;	//右按钮Push前置图像
		CDuiImage		m_sRFocusedImage;		//右按钮Focus图像
		CDuiImage		m_sRDisabledImage;		//右按钮Disable图像
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CTabBodyUI : public CHorizontalLayoutUI
	{
	public:
		CTabBodyUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CTabItemUI : public COptionUI
	{
	public:
		CTabItemUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		CDuiString GetUrl() {return m_sUrl;};
		void SetUrl(LPCTSTR lpszUrl) {m_sUrl = lpszUrl;};

		void Selected(bool bSelected);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	private:
		CDuiString	m_sUrl;
	};
}
#endif // __UITABLAYOUT_H__
