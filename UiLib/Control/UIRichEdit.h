#ifndef __UIRICHEDIT_H__
#define __UIRICHEDIT_H__

#pragma once
#include <Imm.h>
#include "../Utils/IWindowBase.h"
#pragma comment(lib,"imm32.lib")

namespace UiLib {

	class CRichEditUI;

	class UILIB_API CRichEditWnd : public IWindowBase
	{
	public:
		CRichEditWnd(HWND hWndParent = NULL);
		~CRichEditWnd(void);

		CDuiString GetSkinFile();
		CDuiString GetSkinFolder();
		LPCTSTR GetWindowClassName() const ;
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void Init();
		void Notify(TNotifyUI& msg);
		void OnFinalMessage(HWND hWnd);
		void SetAlpha(int nOpticy);
	protected:
		CRichEditUI *m_pRichEdit;
	};

	//////////////////////////////////////////////////////////////////////////

	class CTxtWinHost;

	class UILIB_API CRichEditUI : public CContainerUI, public IMessageFilterUI
	{
	public:
		CRichEditUI();
		~CRichEditUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		bool IsWantCtrlReturn();
		void SetWantCtrlReturn(bool bWantCtrlReturn = true);
		bool IsRich();
		void SetRich(bool bRich = true);
		bool IsReadOnly();
		void SetReadOnly(bool bReadOnly = true);
		bool GetWordWrap();
		void SetWordWrap(bool bWordWrap = true);
		int GetFont();
		void SetFont(int index);
		void SetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
		LONG GetWinStyle();
		void SetWinStyle(LONG lStyle);
		DWORD GetTextColor();
		void SetTextColor(DWORD dwTextColor);
		int GetLimitText();
		void SetLimitText(int iChars);
		long GetTextLength(DWORD dwFlags = GTL_DEFAULT) const;
		CDuiString GetText() const;
		void SetText(LPCTSTR pstrText);
		bool GetModify() const;
		void SetModify(bool bModified = true) const;
		void GetSel(CHARRANGE &cr) const;
		void GetSel(long& nStartChar, long& nEndChar) const;
		int SetSel(CHARRANGE &cr);
		int SetSel(long nStartChar, long nEndChar);
		void ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo);
		void ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo = false);
		CDuiString GetSelText() const;
		int SetSelAll();
		int SetSelNone();
		WORD GetSelectionType() const;
		bool GetZoom(int& nNum, int& nDen) const;
		bool SetZoom(int nNum, int nDen);
		bool SetZoomOff();
		bool GetAutoURLDetect() const;
		bool SetAutoURLDetect(bool bAutoDetect = true);
		DWORD GetEventMask() const;
		DWORD SetEventMask(DWORD dwEventMask);
		CDuiString GetTextRange(long nStartChar, long nEndChar) const;
		void HideSelection(bool bHide = true, bool bChangeStyle = false);
		void ScrollCaret();
		int InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo = false);
		int AppendText(LPCTSTR lpstrText, bool bCanUndo = false);
		DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const;
		bool SetDefaultCharFormat(CHARFORMAT2 &cf);
		DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const;
		bool SetSelectionCharFormat(CHARFORMAT2 &cf);
		bool SetWordCharFormat(CHARFORMAT2 &cf);
		DWORD GetParaFormat(PARAFORMAT2 &pf) const;
		bool SetParaFormat(PARAFORMAT2 &pf);
		bool Redo();
		bool Undo();
		void Clear();
		void Copy();
		void Cut();
		void Paste();
		int GetLineCount() const;
		CDuiString GetLine(int nIndex, int nMaxLength) const;
		int LineIndex(int nLine = -1) const;
		int LineLength(int nLine = -1) const;
		bool LineScroll(int nLines, int nChars = 0);
		CDuiPoint GetCharPos(long lChar) const;
		long LineFromChar(long nIndex) const;
		CDuiPoint PosFromChar(UINT nChar) const;
		int CharFromPos(CDuiPoint pt) const;
		void EmptyUndoBuffer();
		UINT SetUndoLimit(UINT nLimit);
		long StreamIn(int nFormat, EDITSTREAM &es);
		long StreamOut(int nFormat, EDITSTREAM &es);
		void SetAccumulateDBCMode(bool bDBCMode);
		bool IsAccumulateDBCMode();

		void DoInit();
		// 注意：TxSendMessage和SendMessage是有区别的，TxSendMessage没有multibyte和unicode自动转换的功能，
		// 而richedit2.0内部是以unicode实现的，在multibyte程序中，必须自己处理unicode到multibyte的转换
		bool SetDropAcceptFile(bool bAccept);
		virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const; 
		IDropTarget* GetTxDropTarget();
		virtual bool OnTxViewChanged();
		virtual void OnTxNotify(DWORD iNotify, void *pv);

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

		SIZE EstimateSize(SIZE szAvailable);
		void SetPos(RECT rc);
		void DoEvent(TEventUI& event);
		void DoPaint(const RECT& rcPaint);
		void PaintBkColor();

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	public:
		CCaretUI *m_pCaret;
		bool m_bDuringTxDraw;
	protected:
		CTxtWinHost* m_pTwh;
		
		bool m_bVScrollBarFixing;
		bool m_bWantTab;
		bool m_bWantReturn;
		bool m_bWantCtrlReturn;
		bool m_bRich;
		bool m_bReadOnly;
		bool m_bWordWrap;
		DWORD m_dwTextColor;
		int m_iFont;
		int m_iLimitText;
		LONG m_lTwhStyle;
		bool m_bInited;
		bool  m_fAccumulateDBC ; // TRUE - need to cumulate ytes from 2 WM_CHAR msgs
		// we are in this mode when we receive VK_PROCESSKEY
		UINT m_chLeadByte; // use when we are in _fAccumulateDBC mode
	};

} // namespace UiLib

#endif // __UIRICHEDIT_H__
