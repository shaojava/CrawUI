#pragma once

#include <vector>
using namespace std;
namespace UiLib {
/////////////////////////////////////////////////////////////////////////////////////
//
class CComboBoxWnd;
class CComboBoxEditWnd;

class UILIB_API CComboBoxUI : public CContainerUI, public IListOwnerUI
{
    friend class CComboBoxWnd;
	friend class CComboBoxEditWnd;
public:
    CComboBoxUI();
	~CComboBoxUI();
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void DoInit();
    UINT GetControlFlags() const;

    CDuiString GetText() const;
    void SetEnabled(bool bEnable = true);

    CDuiString GetDropBoxAttributeList();
    void SetDropBoxAttributeList(LPCTSTR pstrList);
    SIZE GetDropBoxSize() const;
    void SetDropBoxSize(SIZE szDropBox);

	void SetLastScrollPos(int nPos,int nRange);
    int GetCurSel() const;  
    bool SelectItem(int iIndex, bool bTakeFocus = false);
	bool UnSelectItem(int niInex){return true;}
    bool SetItemIndex(CControlUI* pControl, int iIndex);
    bool Add(CControlUI* pControl);
	bool Add(CDuiString strtext);
	void Sort(bool bSort = false);
	void OnLoadStringList();
    bool AddAt(CControlUI* pControl, int iIndex);
    bool Remove(CControlUI* pControl);
    bool RemoveAt(int iIndex);
    void RemoveAll(bool bUpdate = true);

    bool ActivateDropWnd();
	bool ActivateEditWnd();
	void SelectEnd();

	void SetWheelAble(bool bAble = false);
	bool GetHweelAble();
    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    LPCTSTR GetNormalImage() const;
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage() const;
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage() const;
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage() const;
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage() const;
    void SetDisabledImage(LPCTSTR pStrImage);

    TListInfoUI* GetListInfo();
    void SetItemFont(int index);
    void SetItemTextStyle(UINT uStyle);
	RECT GetItemTextPadding() const;
    void SetItemTextPadding(RECT rc);
	DWORD GetItemTextColor() const;
    void SetItemTextColor(DWORD dwTextColor);
	DWORD GetItemBkColor() const;
    void SetItemBkColor(DWORD dwBkColor);
	LPCTSTR GetItemBkImage() const;
    void SetItemBkImage(LPCTSTR pStrImage);
    bool IsAlternateBk() const;
    void SetAlternateBk(bool bAlternateBk);
	DWORD GetSelectedItemTextColor() const;
    void SetSelectedItemTextColor(DWORD dwTextColor);
	DWORD GetSelectedItemBkColor() const;
    void SetSelectedItemBkColor(DWORD dwBkColor);
	LPCTSTR GetSelectedItemImage() const;
    void SetSelectedItemImage(LPCTSTR pStrImage);
	DWORD GetHotItemTextColor() const;
    void SetHotItemTextColor(DWORD dwTextColor);
	DWORD GetHotItemBkColor() const;
    void SetHotItemBkColor(DWORD dwBkColor);
	LPCTSTR GetHotItemImage() const;
    void SetHotItemImage(LPCTSTR pStrImage);
	DWORD GetDisabledItemTextColor() const;
    void SetDisabledItemTextColor(DWORD dwTextColor);
	DWORD GetDisabledItemBkColor() const;
    void SetDisabledItemBkColor(DWORD dwBkColor);
	LPCTSTR GetDisabledItemImage() const;
    void SetDisabledItemImage(LPCTSTR pStrImage);
	DWORD GetItemLineColor() const;
    void SetItemLineColor(DWORD dwLineColor);
    bool IsItemShowHtml();
    void SetItemShowHtml(bool bShowHtml = true);
	void SetArrowWidth(int nArrowWidth);

    SIZE EstimateSize(SIZE szAvailable);
    void SetPos(RECT rc);
    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    
    void DoPaint(const RECT& rcPaint);
    void PaintText();
    void PaintStatusImage();
	void PaintArrow();

	void SetFont(int index);
	int GetFont() const;
	void SetTextColor(DWORD dwTextColor);
	DWORD GetTextColor() const;

	void SetTextStyle(UINT uStyle);
	UINT GetTextStyle() const;

	RECT GetEditInset();

	void SetSelAll();
	void SetMaxChar(int uMax);
	int GetMaxChar();

	CDuiString GetRegex();
protected:
    CComboBoxWnd* m_pDropWnd;
	CComboBoxEditWnd* m_pEditWnd;
#pragma warning(disable : 4251)
	vector<CDuiString>	m_VectorFont;
	bool m_bWheelReaction;
	bool m_bSelectChange;
	int m_nArrowWidth;
	int m_nLastPos;
	int m_nScrollRange;
	int m_iCurSel;
    int m_iOldSel;;
    RECT m_rcTextPadding;
    CDuiString m_sDropBoxAttributes;
    SIZE m_szDropBox;
    UINT m_uButtonState;

    CDuiImage m_sNormalImage;
    CDuiImage m_sHotImage;
    CDuiImage m_sPushedImage;
    CDuiImage m_sFocusedImage;
    CDuiImage m_sDisabledImage;
	CDuiImage m_sArrowNormal;
	CDuiImage m_sArrowHot;
	CDuiImage m_sArrowPushed;
	CDuiImage m_sTipImage;

    TListInfoUI m_ListInfo;
	RECT m_rcEditInset;
	int		m_iFont;
	DWORD	m_dwTextColor;
	UINT	m_uTextStyle;

	int m_uMaxChar;
	CDuiString m_szEditRegex;
};

} // namespace UiLib