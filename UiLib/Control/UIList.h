#ifndef __UILIST_H__
#define __UILIST_H__

#pragma once
#include "Layout/UIVerticalLayout.h"
#include "Layout/UIHorizontalLayout.h"
#include <gdiplus.h>
using namespace Gdiplus;

namespace UiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

#define ULVHT_NOWHERE				0x0001
#define ULVHT_ITEM					0x0002
#define ULVHT_CHECKBOX				0x0004

class CListHeaderUI;

#define UILIST_MAX_COLUMNS 32

typedef struct tagTListInfoUI
{
    int nColumns;
    RECT rcColumn[UILIST_MAX_COLUMNS];
    int nFont;
    UINT uTextStyle;
    RECT rcTextPadding;
    DWORD dwTextColor;
    DWORD dwBkColor;
    CDuiImage sBkImage;
    bool bAlternateBk;
    DWORD dwSelectedTextColor;
    DWORD dwSelectedBkColor;
    CDuiImage sSelectedImage;
    DWORD dwHotTextColor;
    DWORD dwHotBkColor;
    CDuiImage sHotImage;
    DWORD dwDisabledTextColor;
    DWORD dwDisabledBkColor;
    CDuiImage sDisabledImage;
    DWORD dwLineColor;
    bool bShowHtml;
    bool bMultiExpandable;
	SIZE szCheckImg;
	SIZE szIconImg;
	bool bShowVLine;
	bool bShowHLine;
} TListInfoUI;


/////////////////////////////////////////////////////////////////////////////////////
//

class IListCallbackUI
{
public:
    virtual LPCTSTR GetItemText(CControlUI* pList, int iItem, int iSubItem) = 0;
};

class IListOwnerUI
{
public:
    virtual TListInfoUI* GetListInfo() = 0;
    virtual int GetCurSel() const = 0;
    virtual bool SelectItem(int iIndex, bool bTakeFocus = false) = 0;
	virtual bool UnSelectItem(int iIndex) = 0;
    virtual void DoEvent(TEventUI& event) = 0;
};

class IListUI : public IListOwnerUI
{
public:
    virtual CListHeaderUI* GetHeader() const = 0;
    virtual CContainerUI* GetList() const = 0;
    virtual IListCallbackUI* GetTextCallback() const = 0;
    virtual void SetTextCallback(IListCallbackUI* pCallback) = 0;
    virtual bool ExpandItem(int iIndex, bool bExpand = true) = 0;
    virtual int GetExpandedItem() const = 0;

	virtual bool SelectMultiItem(int iIndex, bool bTakeFocus = false) = 0;
	virtual void SetSingleSelect(bool bSingleSel) = 0;
	virtual bool GetSingleSelect() const = 0;
	virtual int GetLastSingleSel() const = 0;
	virtual void SetLastSingleSel(int iLastSel) = 0;
	virtual void SelectAllItems() = 0;
	virtual void UnSelectAllItems() = 0;
	virtual int GetSelectItemCount() const = 0;
	virtual int GetNextSelItem(int nItem) const = 0;
};

class IListItemUI
{
public:
    virtual int GetIndex() const = 0;
    virtual void SetIndex(int iIndex) = 0;
    virtual IListOwnerUI* GetOwner() = 0;
    virtual void SetOwner(CControlUI* pOwner) = 0;
    virtual bool IsSelected() const = 0;
    virtual bool Select(bool bSelect = true) = 0;
    virtual bool IsExpanded() const = 0;
    virtual bool Expand(bool bExpand = true) = 0;
    virtual void DrawItemText(const RECT& rcItem) = 0;
};

typedef bool (*CompareFunc)(void *, void *);
/////////////////////////////////////////////////////////////////////////////////////
//

class CListBodyUI;
class CListHeaderUI;

class UILIB_API CListUI : public CVerticalLayoutUI, public IListUI
{
public:
    CListUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    bool GetScrollSelect();
    void SetScrollSelect(bool bScrollSelect);
    int GetCurSel() const;
	CStdPtrArray GetSelectItems();
	void SetSeleteItems(CStdPtrArray arry);
	int GetLastSingleSel() const;
	void SetLastSingleSel(int iLastSel);
    bool SelectItem(int iIndex, bool bTakeFocus = false);

	bool SelectMultiItem(int iIndex, bool bTakeFocus = false);
	void SetSingleSelect(bool bSingleSel);
	bool GetSingleSelect() const;
	bool UnSelectItem(int iIndex);
	void SelectAllItems();
	void UnSelectAllItems();
	int GetSelectItemCount() const;
	int GetNextSelItem(int nItem) const;

    CListHeaderUI* GetHeader() const;  
    CContainerUI* GetList() const;
    TListInfoUI* GetListInfo();

    CControlUI* GetItemAt(int iIndex) const;
    int GetItemIndex(CControlUI* pControl) const;
    bool SetItemIndex(CControlUI* pControl, int iIndex);
    int GetCount() const;
    bool Add(CControlUI* pControl);
    bool AddAt(CControlUI* pControl, int iIndex);
    bool Remove(CControlUI* pControl);
    bool RemoveAt(int iIndex);
	bool RemoveSelectItems();
	bool RemoveItems(int iIndex,int iSel);
    void RemoveAll(bool bUpdate = true);

    void EnsureVisible(int iIndex);
    void Scroll(int dx, int dy);

    int GetChildPadding() const;
    void SetChildPadding(int iPadding);

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
	void SetShowVLine(bool bVLine);
	void SetShowHLine(bool bHLine);
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
	bool IsShowVLine() const;
	bool IsShowHLine() const;

    void SetMultiExpanding(bool bMultiExpandable);
	int GetExpandedItem() const;
	void SetMultipleItem(bool bMultipleable);
	bool GetMultipleItem() const;
    bool ExpandItem(int iIndex, bool bExpand = true);

	void SetPos(RECT rc);
    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    IListCallbackUI* GetTextCallback() const;
    void SetTextCallback(IListCallbackUI* pCallback);

    SIZE GetScrollPos() const;
    SIZE GetScrollRange() const;
    void SetScrollPos(SIZE szPos);
    void LineUp();
    void LineDown();
    void PageUp();
    void PageDown();
    void HomeUp();
    void EndDown();
    void LineLeft();
    void LineRight();
    void PageLeft();
    void PageRight();
    void HomeLeft();
    void EndRight();
    void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
    virtual CScrollBarUI* GetVerticalScrollBar() const;
    virtual CScrollBarUI* GetHorizontalScrollBar() const;

	void SortItems(CompareFunc pfnCompare,bool bContainer = false);
protected:
	int GetMinSelItemIndex();
	int GetMaxSelItemIndex();

protected:
    bool m_bScrollSelect;
	bool m_bSingleSel;
	CStdPtrArray m_aSelItems;
    int m_iExpandedItem;
	int m_iLastSingleSelect;
    IListCallbackUI* m_pCallback;
    CListBodyUI* m_pList;
    CListHeaderUI* m_pHeader;
    TListInfoUI m_ListInfo;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CListBodyUI : public CVerticalLayoutUI
{
public:
    CListBodyUI(CListUI* pOwner);

    void SetScrollPos(SIZE szPos);
    void SetPos(RECT rc);
    void DoEvent(TEventUI& event);
	void SortItems(CompareFunc pfnCompare,bool bContainer);
protected:
    CListUI* m_pOwner;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CListHeaderUI : public CHorizontalLayoutUI
{
public:
	CListHeaderUI();
	void SetOwner(CListUI *pOwner);
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
protected:
	CListUI* m_pOwner;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CListHeaderItemUI : public CContainerUI
{
public:
    CListHeaderItemUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    void SetEnabled(bool bEnable = true);

	bool IsDragable() const;
    void SetDragable(bool bDragable);
	DWORD GetSepWidth() const;
    void SetSepWidth(int iWidth);
	DWORD GetTextStyle() const;
    void SetTextStyle(UINT uStyle);
	DWORD GetTextColor() const;
    void SetTextColor(DWORD dwTextColor);
	void SetTextPadding(RECT rc);
	RECT GetTextPadding() const;
    void SetFont(int index);
    bool IsShowHtml();
    void SetShowHtml(bool bShowHtml = true);
    LPCTSTR GetNormalImage() const;
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage() const;
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage() const;
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage() const;
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetSepImage() const;
    void SetSepImage(LPCTSTR pStrImage);

    void DoEvent(TEventUI& event);
	void SetPos(RECT rc);
    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    RECT GetThumbRect() const;

    void PaintText();
    void PaintStatusImage();
	
protected:
    POINT ptLastMouse;
    bool m_bDragable;
    UINT m_uButtonState;
    int m_iSepWidth;
    DWORD m_dwTextColor;
    int m_iFont;
    UINT m_uTextStyle;
    bool m_bShowHtml;
	RECT m_rcTextPadding;
    CDuiImage m_sNormalImage;
    CDuiImage m_sHotImage;
    CDuiImage m_sPushedImage;
    CDuiImage m_sFocusedImage;
    CDuiImage m_sSepImage;
    CDuiImage m_sSepImageModify;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CListElementUI : public CControlUI, public IListItemUI
{
public:
    CListElementUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetEnabled(bool bEnable = true);

    int GetIndex() const;
    void SetIndex(int iIndex);

    IListOwnerUI* GetOwner();
    void SetOwner(CControlUI* pOwner);
    void SetVisible(bool bVisible = true);

    bool IsSelected() const;
    bool Select(bool bSelect = true);
    bool IsExpanded() const;
    bool Expand(bool bExpand = true);

    void Invalidate();
    bool Activate();

    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void DrawItemBk(const RECT& rcItem);
	void SetSortAble(bool bSortAble);
	bool IsSortAble();
protected:
    int m_iIndex;
    bool m_bSelected;
	bool m_bSortAble;
    UINT m_uButtonState;
    IListOwnerUI* m_pOwner;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CListLabelElementUI : public CListElementUI
{
public:
    CListLabelElementUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void DoEvent(TEventUI& event);
    SIZE EstimateSize(SIZE szAvailable);
    void DoPaint(const RECT& rcPaint);

    void DrawItemText(const RECT& rcItem);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class UILIB_API CListTextElementUI : public CListLabelElementUI
{
public:
    CListTextElementUI();
    ~CListTextElementUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

	bool Select(bool bSelect = true);

    LPCTSTR GetText(int iIndex) const;
    void SetText(int iIndex, LPCTSTR pstrText,int uTextStyle = -1);

    void SetOwner(CControlUI* pOwner);
    CDuiString* GetLinkContent(int iIndex);

    void DoEvent(TEventUI& event);
    SIZE EstimateSize(SIZE szAvailable);

    void DrawItemText(const RECT& rcItem);
protected:
    enum { MAX_LINK = 8 };
    int m_nLinks;
    RECT m_rcLinks[MAX_LINK];
    CDuiString m_sLinks[MAX_LINK];
    int m_nHoverLink;
    IListUI* m_pOwner;
    CStdPtrArray m_aTexts;
	std::vector<int> m_uTextsStyle;
};

/////////////////////////////////////////////////////////////////////////////////////
//


class UILIB_API CListImageTextElementUI : public CListTextElementUI
{
public:
	CListImageTextElementUI();
	~CListImageTextElementUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	CDuiImage* GetNorImg(int iIndex) const;
	CDuiImage* GetSelImg(int iIndex) const;
	void SetNorImg(int iIndex, LPCTSTR pstrText);
	void SetSelImg(int iIndex, LPCTSTR pstrText);

	void SetCheckFlag(int iIndex, bool bStartCheck = TRUE);
	bool GetCheckFlag(int iIndex) const;
	void SetCheck(int iIndex, bool bCheck);
	bool GetCheck(int iIndex) const;	
	CDuiImage* GetNorCheckImg(int iIndex) const;
	CDuiImage* GetSelCheckImg(int iIndex) const;
	void SetNorCheckImg(int iIndex, LPCTSTR pstrText);
	void SetSelCheckImg(int iIndex, LPCTSTR pstrText);

	int HitTest(POINT pt, UINT* pFlags = NULL);

	RECT GetItemRect(int iIndex);
	RECT GetItemRectPadding(int iIndex);
	RECT GetItemCheckRect(int iIndex);
	RECT GetItemImgRect(int iIndex);
	RECT GetItemTextRect(int iIndex);

	void DoEvent(TEventUI& event);
	void DrawItemText(const RECT& rcItem);	
protected:	
	CStdPtrArray m_aImgNor;
	CStdPtrArray m_aImgSel;
	CStdValArray m_aStartCheck;
	CStdValArray m_aCheck;
	CStdPtrArray m_aImgNorCheck;
	CStdPtrArray m_aImgSelCheck;
};


/////////////////////////////////////////////////////////////////////////////////////
//
class UILIB_API CListContainerElementUI : public CContainerUI, public IListItemUI
{
public:
    CListContainerElementUI();
	~CListContainerElementUI();
    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    int GetIndex() const;
    void SetIndex(int iIndex);

    IListOwnerUI* GetOwner();
    void SetOwner(CControlUI* pOwner);
    void SetVisible(bool bVisible = true);
    void SetEnabled(bool bEnable = true);

	void SetSelect(bool bSelect = true){m_bSelected = bSelect;}
    bool IsSelected() const;
    virtual bool Select(bool bSelect = true);
    bool IsExpanded() const;
    bool Expand(bool bExpand = true);
	void MultiExpand(bool bMultiExpand=false);

    void Invalidate();
    bool Activate();

    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void DoPaint(const RECT& rcPaint);
	void SetPos(RECT rc);

    void DrawItemText(const RECT& rcItem);    
    virtual void DrawItemBk(const RECT& rcItem);
	void DrawItemSelectBorder();
	void SetInnerBorder(DWORD dwColor,int nSize);
	void SetSelectBorder(DWORD dwColor,int nSize);

	void SetSortAble(bool bSortAble);
	bool IsSortAble();
protected:
    int m_iIndex;
	DWORD m_dwSelectBorderColor;
	int m_nSelectBorderSize;
	DWORD m_dwInnerBorderColor;
	int m_nInnerBorderSize;
	bool m_bMultiExpand;
    bool m_bSelected;
	bool m_bSortAble;
    UINT m_uButtonState;
    IListUI* m_pOwner;
};

class UILIB_API CListContainerItemElementUI : public CListContainerElementUI
{
public:
	CListContainerItemElementUI();
	~CListContainerItemElementUI();
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	void SetOwner(CControlUI* pOwner);
	void DoEvent(TEventUI& event);
	void SetItemImg(CDuiString dszPicPath);
	void SetItemImg(Bitmap *&pBitmap);
	Bitmap * &GetItemImg();
	void DrawItemBk(HDC hDC,const RECT &rcItem);
	bool OnInit(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	void SetItemSize(SIZE szItem);
protected:
	bool OnBtnDelete(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnBtnLeftRotate(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
	bool OnBtnRightRotate(TNotifyUI* pTNotifyUI,LPARAM lParam,WPARAM wParam);
protected:
	IListUI* m_pOwner;
	bool m_bMouseEnterd;
	Bitmap *m_pImg;
	TImageInfo *m_pBkImg;
	CAnimationTabLayoutUI *pUpPanel;
	CAnimationTabLayoutUI *pDownPanel;
	CControlUI *m_pBtnItemDelete;
	CControlUI *m_pBtnLeftRotate;
	CControlUI *m_pBtnRightRotate;
	RECT m_RcRender;
	SIZE m_szItem;
};
} // namespace UiLib

#endif // __UILIST_H__
