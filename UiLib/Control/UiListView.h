#ifndef  __UILISTVIEW_H__
#define __UILISTVIEW_H__

#pragma once
#include "UIList.h"
#include "Layout/UITileLayout.h"

namespace UiLib
{
	class UILIB_API CListViewUI : public CTileLayoutUI ,public IListUI
	{
	public:
		CListViewUI();
		~CListViewUI();
		//获取类名;
		virtual LPCTSTR GetClass() const;
		//控件标志位TabStop;
		UINT GetControlFlags() const;
		//动态识别返回类名;
		LPVOID GetInterface(LPCTSTR pstrName);
		//设置是否为单选或多选;
		void SetSingleSelect(bool bSingleSel);
		bool GetSingleSelect() const;
		void SetMultipleItem(bool bMultipleable);
		bool GetMultipleItem() const;
		//获取当前选中项,如果是多选则返回第一个;
		int GetCurSel() const;
		CStdPtrArray GetSelectItems();
		//获取各种状态;
		TListInfoUI* GetListInfo();
		CListHeaderUI* GetHeader() const{return NULL;}
		CContainerUI* GetList() const{return NULL;}
		
		IListCallbackUI* GetTextCallback() const;
		void SetTextCallback(IListCallbackUI* pCallback);

		void SetItemFont(int index);
		void SetItemTextStyle(UINT uStyle);
		void SetItemTextPadding(RECT rc);
		void SetItemTextColor(DWORD dwTextColor);
		void SetItemBkColor(DWORD dwBkColor);
		void SetItemBkImage(LPCTSTR pStrImage);
		void SetAlternateBk(bool bAlternateBk);
		void SetSelectedItemTextColor(DWORD dwTextColor);
		void SetSelectedItemBkColor(DWORD dwBkColor);
		void SetSelectedItemImage(LPCTSTR pStrImage); 
		void SetHotItemTextColor(DWORD dwTextColor);
		void SetHotItemBkColor(DWORD dwBkColor);
		void SetHotItemImage(LPCTSTR pStrImage);
		void SetDisabledItemTextColor(DWORD dwTextColor);
		void SetDisabledItemBkColor(DWORD dwBkColor);
		void SetDisabledItemImage(LPCTSTR pStrImage);
		void SetItemLineColor(DWORD dwLineColor);
		void SetCheckImgSize(SIZE szCheckImg);
		void SetIconImgSize(SIZE szIconImg);
		bool IsItemShowHtml();
		void SetItemShowHtml(bool bShowHtml = true);
		RECT GetItemTextPadding() const;
		DWORD GetItemTextColor() const;
		DWORD GetItemBkColor() const;
		LPCTSTR GetItemBkImage() const;
		bool IsAlternateBk() const;
		DWORD GetSelectedItemTextColor() const;
		DWORD GetSelectedItemBkColor() const;
		LPCTSTR GetSelectedItemImage() const;
		DWORD GetHotItemTextColor() const;
		DWORD GetHotItemBkColor() const;
		LPCTSTR GetHotItemImage() const;
		DWORD GetDisabledItemTextColor() const;
		DWORD GetDisabledItemBkColor() const;
		LPCTSTR GetDisabledItemImage() const;
		DWORD GetItemLineColor() const;
		SIZE GetCheckImgSize() const;
		SIZE GetIconImgSize() const;


		CControlUI* GetItemAt(int iIndex) const;
		int GetItemIndex(CControlUI* pControl) const;
		bool SetItemIndex(CControlUI* pControl, int iIndex);
		bool Add(CControlUI* pControl);
		bool AddAt(CControlUI* pControl, int iIndex);
		bool Remove(CControlUI* pControl);
		bool RemoveAt(int iIndex);
		bool RemoveItems(int iIndex,int iSel);
		void RemoveAll(bool bUpdate = true);
		//删除选中项;
		bool RemoveSelectItems();
		//选中某一项;
		virtual bool SelectItem(int iIndex, bool bTakeFocus = false);
		int GetNextSelItem(int nItem) const;
		//多选;
		bool SelectMultiItem(int iIndex, bool bTakeFocus = false);
		//反选某一项;
		bool UnSelectItem(int iIndex);
		//选择所有Items;
		virtual void SelectAllItems();
		//反选所有Items;
		void UnSelectAllItems();
		//获取选中项总数;
		int GetSelectItemCount() const;
		//响应内部事件;
		 void DoEvent(TEventUI& event);
		 
		 void AllowUnSelectAll(bool bAllow = true){m_bUnSelectAll = bAllow;}
		 bool ExpandItem(int iIndex, bool bExpand = true){return true;}
		 int GetExpandedItem() const{return 0;}
	protected:
		int GetMinSelItemIndex();
		int GetMaxSelItemIndex();
	public:
		bool m_bUnSelectAll;
		TListInfoUI m_ListInfo;
		//是否单选;
		bool m_bSingleSel;
		//已选中项目;
		CStdPtrArray m_aSelItems;
		 IListCallbackUI* m_pCallback;
	};
}
#endif