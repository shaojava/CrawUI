#include "stdafx.h"
#include <share.h>
#include "DuiAutoComplete.h"

#define ARRSIZE(x)	(sizeof(x)/sizeof(x[0]))

namespace UiLib
{

	//************************************
	// 函数名称: CDuiAutoComplete
	// 返回类型: 
	// 函数说明: 
	//************************************
	CDuiAutoComplete::CDuiAutoComplete()
	{
		InternalInit();
	}

	//************************************
	// 函数名称: ~CDuiAutoComplete
	// 返回类型: 
	// 函数说明: 
	//************************************
	CDuiAutoComplete::~CDuiAutoComplete()
	{
		Clear();
		if (m_fBound && GetIAutoComplete())
			GetIAutoComplete().Release();
		m_fBound = FALSE;
	}

	//************************************
	// 函数名称: Bind
	// 返回类型: BOOL
	// 参数信息: HWND p_hWndEdit
	// 参数信息: DWORD p_dwOptions
	// 参数信息: LPCTSTR p_lpszFormatString
	// 函数说明: 
	//************************************
	BOOL CDuiAutoComplete::Bind(HWND p_hWndEdit, DWORD p_dwOptions, LPCTSTR p_lpszFormatString)
	{
		ATLASSERT(::IsWindow(p_hWndEdit));
		if ((m_fBound) || (GetIAutoComplete()))
			return FALSE;

		HRESULT hr = GetIAutoComplete().CoCreateInstance(CLSID_AutoComplete);
		if (SUCCEEDED(hr))
		{
			if (p_dwOptions){
				CComQIPtr<IAutoComplete2> pAC2(GetIAutoComplete());
				if (pAC2){
					pAC2->SetOptions(p_dwOptions);
					pAC2.Release();
				}
			}

			USES_CONVERSION;
			if (SUCCEEDED(hr = GetIAutoComplete()->Init(p_hWndEdit, this, NULL, T2CW(p_lpszFormatString))))
			{
				m_fBound = TRUE;
				return TRUE;
			}
		}
		return FALSE;
	}

	//************************************
	// 函数名称: Unbind
	// 返回类型: VOID
	// 函数说明: 
	//************************************
	VOID CDuiAutoComplete::Unbind()
	{
		if (!m_fBound)
			return;
		if (m_fBound && GetIAutoComplete()){
			GetIAutoComplete().Release();
			m_fBound = FALSE;
		}
	}

	//************************************
	// 函数名称: FindItem
	// 返回类型: int
	// 参数信息: const CDuiString & rstr
	// 函数说明: 
	//************************************
	int CDuiAutoComplete::FindItem(const CDuiString& rstr)
	{
		for (int i = 0; i < m_sStringArrayMap.GetSize(); i++)
			if (m_sStringArrayMap.GetAt(i)->Compare(rstr.GetData()) == 0)
				return i;

		return -1;
	}

	//************************************
	// 函数名称: AddItem
	// 返回类型: BOOL
	// 参数信息: const CDuiString & p_sItem
	// 参数信息: int iPos
	// 函数说明: 
	//************************************
	BOOL CDuiAutoComplete::AddItem(const CDuiString& p_sItem, int iPos)
	{
		if (p_sItem.GetLength() != 0)
		{
			int oldpos=FindItem(p_sItem);
			if (oldpos == -1)
			{
				if (iPos == -1)
					m_sStringArrayMap.Add(new CDuiString(p_sItem));
				else
					m_sStringArrayMap.InsertAt(iPos,new CDuiString(p_sItem));

				while (m_sStringArrayMap.GetSize() > m_iMaxItemCount)
					m_sStringArrayMap.Remove(m_sStringArrayMap.GetSize() - 1,true);
				return TRUE;
			} else if (iPos!=-1) {
				m_sStringArrayMap.Remove(oldpos,true);
				if (oldpos<iPos) --iPos;
				m_sStringArrayMap.InsertAt(iPos,new CDuiString(p_sItem));

				while (m_sStringArrayMap.GetSize() > m_iMaxItemCount)
					m_sStringArrayMap.Remove(m_sStringArrayMap.GetSize() - 1,true);
				return TRUE;
			}
		}
		return FALSE;
	}

	//************************************
	// 函数名称: GetItemCount
	// 返回类型: int
	// 函数说明: 
	//************************************
	int CDuiAutoComplete::GetItemCount()
	{
		return (int)m_sStringArrayMap.GetSize();
	}

	//************************************
	// 函数名称: RemoveItem
	// 返回类型: BOOL
	// 参数信息: const CDuiString & p_sItem
	// 函数说明: 
	//************************************
	BOOL CDuiAutoComplete::RemoveItem(const CDuiString& p_sItem)
	{
		if (p_sItem.GetLength() != 0)
		{
			int iPos = FindItem(p_sItem);
			if (iPos != -1)
			{
				m_sStringArrayMap.Remove(iPos,true);
				return TRUE;
			}
		}
		return FALSE;
	}


	//************************************
	// 函数名称: Clear
	// 返回类型: BOOL
	// 函数说明: 
	//************************************
	BOOL CDuiAutoComplete::Clear()
	{
		if (m_sStringArrayMap.GetSize() != 0)
		{
			while (m_sStringArrayMap.GetSize() > 0)
				m_sStringArrayMap.Remove(m_sStringArrayMap.GetSize()-1,true);

			return TRUE;
		}
		return FALSE;
	}

	//************************************
	// 函数名称: Disable
	// 返回类型: BOOL
	// 函数说明: 
	//************************************
	BOOL CDuiAutoComplete::Disable()
	{
		if ((!GetIAutoComplete()) || (!m_fBound))
			return FALSE;
		return SUCCEEDED(EnDisable(FALSE));
	}

	//************************************
	// 函数名称: Enable
	// 返回类型: BOOL
	// 参数信息: VOID
	// 函数说明: 
	//************************************
	BOOL CDuiAutoComplete::Enable(VOID)
	{
		if ((!GetIAutoComplete()) || (m_fBound))
			return FALSE;
		return SUCCEEDED(EnDisable(TRUE));
	}

	//************************************
	// 函数名称: GetStringArray
	// 返回类型: const TStdPtrArray<CDuiString*>&
	// 函数说明: 
	//************************************
	const TStdPtrArray<CDuiString*>& CDuiAutoComplete::GetStringArray() const
	{
		return m_sStringArrayMap;
	}

	//************************************
	// 函数名称: AddRef
	// 返回类型: STDMETHODIMP_(ULONG)
	// 函数说明: 
	//************************************
	STDMETHODIMP_(ULONG) CDuiAutoComplete::AddRef()
	{
		ULONG nCount = ::InterlockedIncrement(reinterpret_cast<LONG*>(&m_nRefCount));
		return nCount;
	}

	//************************************
	// 函数名称: Release
	// 返回类型: STDMETHODIMP_(ULONG)
	// 函数说明: 
	//************************************
	STDMETHODIMP_(ULONG) CDuiAutoComplete::Release()
	{
		ULONG nCount = 0;
		nCount = (ULONG) ::InterlockedDecrement(reinterpret_cast<LONG*>(&m_nRefCount));
		return nCount;
	}

	//************************************
	// 函数名称: QueryInterface
	// 返回类型: STDMETHODIMP
	// 参数信息: REFIID riid
	// 参数信息: void * * ppvObject
	// 函数说明: 
	//************************************
	STDMETHODIMP CDuiAutoComplete::QueryInterface(REFIID riid, void** ppvObject)
	{
		HRESULT hr = E_NOINTERFACE;
		if (ppvObject != NULL)
		{
			*ppvObject = NULL;

			if (IID_IUnknown == riid)
				*ppvObject = static_cast<IUnknown*>(this);
			else if (IID_IEnumString == riid)
				*ppvObject = static_cast<IEnumString*>(this);
			if (*ppvObject != NULL)
			{
				hr = S_OK;
				((LPUNKNOWN)*ppvObject)->AddRef();
			}
		}
		else
			hr = E_POINTER;
		return hr;
	}

	//************************************
	// 函数名称: Next
	// 返回类型: STDMETHODIMP
	// 参数信息: ULONG celt
	// 参数信息: LPOLESTR * rgelt
	// 参数信息: ULONG * pceltFetched
	// 函数说明: 
	//************************************
	STDMETHODIMP CDuiAutoComplete::Next(ULONG celt, LPOLESTR* rgelt, ULONG* pceltFetched)
	{
		USES_CONVERSION;
		HRESULT hr = S_FALSE;

		if (!celt)
			celt = 1;
		ULONG i;
		for (i = 0; i < celt; i++)
		{
			if (m_nCurrentElement == (ULONG)m_sStringArrayMap.GetSize())
				break;

			rgelt[i] = (LPWSTR)::CoTaskMemAlloc((ULONG) sizeof(WCHAR) * (m_sStringArrayMap.GetAt(m_nCurrentElement)->GetLength() + 1));
			wcscpy(rgelt[i], T2CW(m_sStringArrayMap.GetAt(m_nCurrentElement)->GetData()));

			if (pceltFetched)
				*pceltFetched++;

			m_nCurrentElement++;
		}

		if (i == celt)
			hr = S_OK;

		return hr;
	}

	//************************************
	// 函数名称: Skip
	// 返回类型: STDMETHODIMP
	// 参数信息: ULONG celt
	// 函数说明: 
	//************************************
	STDMETHODIMP CDuiAutoComplete::Skip(ULONG celt)
	{
		m_nCurrentElement += celt;
		if (m_nCurrentElement > (ULONG)m_sStringArrayMap.GetSize())
			m_nCurrentElement = 0;

		return S_OK;
	}

	//************************************
	// 函数名称: Reset
	// 返回类型: STDMETHODIMP
	// 参数信息: void
	// 函数说明: 
	//************************************
	STDMETHODIMP CDuiAutoComplete::Reset(void)
	{
		m_nCurrentElement = 0;
		return S_OK;
	}

	//************************************
	// 函数名称: Clone
	// 返回类型: STDMETHODIMP
	// 参数信息: IEnumString * * ppenum
	// 函数说明: 
	//************************************
	STDMETHODIMP CDuiAutoComplete::Clone(IEnumString** ppenum)
	{
		if (!ppenum)
			return E_POINTER;

		CDuiAutoComplete* pnew = new CDuiAutoComplete();
		pnew->AddRef();
		*ppenum = pnew;
		return S_OK;
	}

	//************************************
	// 函数名称: InternalInit
	// 返回类型: void
	// 函数说明: 
	//************************************
	void CDuiAutoComplete::InternalInit()
	{
		m_nCurrentElement = 0;
		m_nRefCount = 0;
		m_fBound = FALSE;
		m_iMaxItemCount = 30;
	}

	//************************************
	// 函数名称: EnDisable
	// 返回类型: HRESULT
	// 参数信息: BOOL p_fEnable
	// 函数说明: 
	//************************************
	HRESULT CDuiAutoComplete::EnDisable(BOOL p_fEnable)
	{
		HRESULT hr = GetIAutoComplete()->Enable(p_fEnable);
		if (SUCCEEDED(hr))
			m_fBound = p_fEnable;
		return hr;
	}

	//************************************
	// 函数名称: LoadList
	// 返回类型: BOOL
	// 参数信息: LPCTSTR pszFileName
	// 函数说明: 
	//************************************
	BOOL CDuiAutoComplete::LoadList(LPCTSTR pszFileName)
	{
		FILE* fp = _tfsopen(pszFileName, _T("rb"), _SH_DENYWR);
		if (fp == NULL)
			return FALSE;

		// 校验 Unicode 编码字节序 mark 0xFEFF
		WORD wBOM = fgetwc(fp);
		if (wBOM != 0xFEFF){
			fclose(fp);
			return FALSE;
		}

		TCHAR szItem[256];
		while (_fgetts(szItem, ARRSIZE(szItem), fp) != NULL){
			CDuiString strItem(szItem);
			strItem.Replace(_T("\r"),_T(""));
			strItem.Replace(_T("\n"),_T(""));
			AddItem(strItem, -1);
		}
		fclose(fp);
		return TRUE;
	}

	//************************************
	// 函数名称: SaveList
	// 返回类型: BOOL
	// 参数信息: LPCTSTR pszFileName
	// 函数说明: 
	//************************************
	BOOL CDuiAutoComplete::SaveList(LPCTSTR pszFileName)
	{
		FILE* fp = _tfsopen(pszFileName, _T("wb"), _SH_DENYWR);
		if (fp == NULL)
			return FALSE;

		// 写 Unicode 编码字节序 mark 0xFEFF
		fputwc(0xFEFF, fp);

		for (int i = 0; i < m_sStringArrayMap.GetSize(); i++)
			_ftprintf(fp, _T("%s\r\n"), m_sStringArrayMap[i]);
		fclose(fp);
		return !ferror(fp);
	}

	//************************************
	// 函数名称: GetItem
	// 返回类型: UiLib::CDuiString
	// 参数信息: int pos
	// 函数说明: 
	//************************************
	CDuiString CDuiAutoComplete::GetItem(int pos){
		if (pos>=m_sStringArrayMap.GetSize())
			return CDuiString(); 
		else
			return m_sStringArrayMap.GetAt(pos)->GetData();
	}

	//************************************
	// 函数名称: GetIAutoComplete
	// 返回类型: CComPtr<IAutoComplete>&
	// 函数说明: 
	//************************************
	CComPtr<IAutoComplete>& CDuiAutoComplete::GetIAutoComplete()
	{
		static CComPtr<IAutoComplete> m_pac;
		return m_pac;
	}

}