#ifndef __UIFLASH_H__
#define __UIFLASH_H__
#pragma once
#include "Utils/FlashEventHandler.h"
#include "Utils/flash12.tlh"

#pragma warning(disable:4275)

namespace UiLib
{
	using namespace ShockwaveFlashObjects;
	class UILIB_API CFlashUI
		: public CActiveXUI
		, public _IShockwaveFlashEvents
		, public ITranslateAccelerator
	{
	public:
		CFlashUI(void);
		~CFlashUI(void);

		void SetFlashEventHandler(CFlashEventHandler* pHandler);
		virtual bool DoCreateControl();
		IShockwaveFlash* m_pFlash;
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface( LPCTSTR pstrName );
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	private:
		virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( __RPC__out UINT *pctinfo );
		virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo );
		virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( __RPC__in REFIID riid, __RPC__in_ecount_full(cNames ) LPOLESTR *rgszNames, UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId);
		virtual HRESULT STDMETHODCALLTYPE Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr );

		virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void **ppvObject );
		virtual ULONG STDMETHODCALLTYPE AddRef( void );
		virtual ULONG STDMETHODCALLTYPE Release( void );

		HRESULT OnReadyStateChange (long newState);
		HRESULT OnProgress(long percentDone );
		HRESULT FSCommand (_bstr_t command, _bstr_t args);
		HRESULT FlashCall (_bstr_t request );

		virtual void ReleaseControl();
		HRESULT RegisterEventHandler(BOOL inAdvise);

		virtual LRESULT TranslateAccelerator( MSG *pMsg );

	private:
		LONG m_dwRef;
		DWORD m_dwCookie;
		CFlashEventHandler* m_pFlashEventHandler;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////

	class UILIB_API CFlashWnd : public IWindowBase
	{
	public:
		CFlashWnd(IWindowBase *pParent = NULL);
		~CFlashWnd(void);
		virtual CDuiString GetSkinFile(){return _T("");}
		LPCTSTR GetWindowClassName() const ;
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		void CreateFlashWnd();
		void Notify(TNotifyUI& msg);
		CFlashUI *GetFlashUI();
		
	protected:
		IWindowBase *m_pParent;
		CFlashUI *m_pFlashUI;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////

	class UILIB_API CFlashWndUI : public CControlUI
	{
	public:
		CFlashWndUI();
		~CFlashWndUI();
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void SetPos(RECT rc);
		virtual void DoInit();
		virtual void DoEvent(TEventUI& event);
		void SetFlashEventHandler(CFlashEventHandler* pHandler);
		virtual bool DoCreateControl();
	private:
		CFlashWnd *m_pFlashWnd;
	};
}

#endif // __UIFLASH_H__
