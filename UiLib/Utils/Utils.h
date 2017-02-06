#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once
#include <hash_map>
using namespace stdext;

namespace UiLib
{
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class STRINGorID
	{
	public:
		STRINGorID(LPCTSTR lpString) : m_lpstr(lpString)
		{ }
		STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
		{ }
		LPCTSTR m_lpstr;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiPoint : public tagPOINT
	{
	public:
		CDuiPoint();
		CDuiPoint(const POINT& src);
		CDuiPoint(int x, int y);
		CDuiPoint(LPARAM lParam);
		CDuiPoint operator+(IN const CDuiPoint& point) const
		{
			return CDuiPoint(x + point.x,y + point.y);
		}

		CDuiPoint operator-(IN const CDuiPoint& point) const
		{
			return CDuiPoint(x - point.x,y - point.y);
		}
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiSize : public tagSIZE
	{
	public:
		CDuiSize();
		CDuiSize(const SIZE& src);
		CDuiSize(const RECT rc);
		CDuiSize(int cx, int cy);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiRect : public tagRECT
	{
	public:
		CDuiRect();
		CDuiRect(const RECT& src);
		CDuiRect(int iLeft, int iTop, int iRight, int iBottom);

		int GetWidth() const;
		int GetHeight() const;
		void Empty();
		bool IsNull() const;
		void Join(const RECT& rc);
		void ResetOffset();
		void Normalize();
		void Offset(int cx, int cy);
		void Inflate(int cx, int cy);
		void Deflate(int cx, int cy);

		void Union(CDuiRect& rc);
		void Intersect(CDuiRect &rc);
		void Subtract(CDuiRect &rc);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	typedef int (__cdecl *BASEPTRARRAYCOMPFUNC)(const void *elem1,const void *elem2);


	class UILIB_API CStdPtrArray
	{
	public:
		CStdPtrArray(int iPreallocSize = 0);
		CStdPtrArray(const CStdPtrArray& src);
		~CStdPtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(LPVOID iIndex) const;
		bool Add(LPVOID pData);
		bool SetAt(int iIndex, LPVOID pData);
		bool Insert(void *pItem,BASEPTRARRAYCOMPFUNC pCompFunc);
		bool InsertAt(int iIndex, LPVOID pData);
		bool Remove(int iIndex);
		bool Remove(LPVOID pData);
		void Sink(int iIndex);
		int GetSize() const;
		LPVOID* GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

		CStdPtrArray& operator =(CStdPtrArray& src);
		CStdPtrArray& operator +=(CStdPtrArray& src);

	protected:
		LPVOID* m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};

	template<typename T = LPVOID>
	class UILIB_API TStdPtrArray : public CStdPtrArray
	{
	public:
		TStdPtrArray(int iPreallocSize = 0):CStdPtrArray(iPreallocSize){};
		TStdPtrArray(const TStdPtrArray& src):CStdPtrArray(src){};
		int Find(T iIndex) const {return CStdPtrArray::Find(iIndex);};
		bool Add(T pData){return CStdPtrArray::Add(pData);};
		bool SetAt(int iIndex, T pData){return CStdPtrArray::SetAt(iIndex,pData);};
		bool InsertAt(int iIndex, T pData){return CStdPtrArray::InsertAt(iIndex,pData);};
		bool Remove(int iIndex,bool bDeleteObj = false){
			if(bDeleteObj){
				T p = GetAt(iIndex);
				if(p)
					delete p;
			}
			return CStdPtrArray::Remove(iIndex);
		}
		T* GetData(){return static_cast<T>(CStdPtrArray::GetData());};
		T GetAt(int iIndex) const {return static_cast<T>(CStdPtrArray::GetAt(iIndex));};
		T operator[] (int nIndex) const{return static_cast<T>(CStdPtrArray::operator[](nIndex));};
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	typedef int (__cdecl *BASEVALUECOMPFUNC)(const void *elem1,const void *elem2);

	class UILIB_API CStdValArray
	{
	public:
		CStdValArray(int iElementSize, int iPreallocSize = 0);
		~CStdValArray();


		void Empty(BOOL bFreeList=FALSE);
		bool IsEmpty() const;
		bool Add(LPCVOID pData);
		bool Insert(int iIndex,LPCVOID pData);
		//使用二分法进行快速插入,插入前是排序好的,插入后也是排序好的
		BOOL Insert(void *pItem,BASEVALUECOMPFUNC pCompFunc);
		//查找元素，顺序查找，在pCompFunc为NULL的时候，只比较pItem指针是否与m_pList中的相同，当有pCompFunc的时候，就会根据函数来比较两个元素是否相同
		//返回-1表示查找失败
		int FindItemPos(void *pItem,BASEVALUECOMPFUNC pCompFunc=NULL);
		//查找元素，顺序查找，在pCompFunc为NULL的时候，只比较pItem指针是否与m_pList中的相同，当有pCompFunc的时候，就会根据函数来比较两个元素是否相同
		//返回NULL表示查找失败
		void* FindItem(void*pItem,BASEVALUECOMPFUNC pCompFunc=NULL);
		bool Remove(LPVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPBYTE m_pVoid;
		int m_iElementSize;
		int m_nCount;
		int m_nAllocated;
	};

	template<typename T = LPVOID,typename T1 = T>
	class UILIB_API TStdValArray : public CStdValArray
	{
	public:
		TStdValArray(int iElementSize = sizeof(T), int iPreallocSize = 0):CStdValArray(iElementSize,iPreallocSize){};
		bool Add(const T1 pData){return CStdValArray::Add((LPCVOID)&pData);};
		bool InsertAt(int iIndex,const T pData){
			if( iIndex == m_nCount ) return Add(pData);
			if( iIndex < 0 || iIndex > m_nCount ) return false;
			if( ++m_nCount >= m_nAllocated) {
				int nAllocated = m_nAllocated * 2;
				if( nAllocated == 0 ) nAllocated = 11;
				LPBYTE pVoid = static_cast<LPBYTE>(realloc(m_pVoid, nAllocated * m_iElementSize));
				if( pVoid != NULL ) {
					m_nAllocated = nAllocated;
					m_pVoid = pVoid;
				}
				else {
					--m_nCount;
					return false;
				}
			}

			memmove(&m_pVoid + (iIndex+1) * m_iElementSize, &m_pVoid + iIndex * m_iElementSize,m_iElementSize);
			::CopyMemory(m_pVoid + (iIndex * m_iElementSize), &pData, m_iElementSize);
			return true;
		}
		T GetData(){return static_cast<T>(CStdValArray::GetData());};
		T GetAt(int iIndex) const {return *static_cast<T1*>(CStdValArray::GetAt(iIndex));};
		T operator[] (int nIndex) const{return (T)CStdValArray:::operator[](nIndex);};
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiString
	{
	public:
		enum { MAX_LOCAL_STRING_LEN = 63 };

		CDuiString();
		CDuiString(const TCHAR ch);
		CDuiString(const CDuiString& src);
		CDuiString(LPCTSTR lpsz, int nLen = -1);
		~CDuiString();

		void Empty();
		int GetLength() const;
		bool IsEmpty() const;
		TCHAR GetAt(int nIndex) const;
		void Append(LPCTSTR pstr);
		void Assign(LPCTSTR pstr, int nLength = -1);
		LPCTSTR GetData() const;
		std::wstring GetStringW();
		std::string GetStringA();

		void SetAt(int nIndex, TCHAR ch);
		operator LPCTSTR() const;

		TCHAR operator[] (int nIndex) const;
		const CDuiString& operator=(const CDuiString& src);
		const CDuiString& operator=(const TCHAR ch);
		const CDuiString& operator=(LPCTSTR pstr);
#ifdef _UNICODE
		const CDuiString& operator=(LPCSTR lpStr);
		const CDuiString& operator+=(LPCSTR lpStr);
#else
		const CDuiString& operator=(LPCWSTR lpwStr);
		const CDuiString& operator+=(LPCWSTR lpwStr);
#endif
		CDuiString operator+(const CDuiString& src) const;
		CDuiString operator+(LPCTSTR pstr) const;
		const CDuiString& operator+=(const CDuiString& src);
		const CDuiString& operator+=(LPCTSTR pstr);
		const CDuiString& operator+=(const TCHAR ch);

		bool operator == (LPCTSTR str) const;
		bool operator != (LPCTSTR str) const;
		bool operator <= (LPCTSTR str) const;
		bool operator <  (LPCTSTR str) const;
		bool operator >= (LPCTSTR str) const;
		bool operator >  (LPCTSTR str) const;

		int Compare(LPCTSTR pstr) const;
		int CompareNoCase(LPCTSTR pstr) const;

		void MakeUpper();
		void MakeLower();

		CDuiString Left(int nLength) const;
		CDuiString Mid(int iPos, int nLength = -1) const;
		CDuiString Right(int nLength) const;

		int Find(TCHAR ch, int iPos = 0) const;
		int Find(LPCTSTR pstr, int iPos = 0) const;
		int ReverseFind(TCHAR ch) const;
		int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);

		char* UnicodeToAnsi();

		int __cdecl Format(LPCTSTR pstrFormat, ...);
		int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);

	protected:
		LPTSTR m_pstr;
		TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class UILIB_API CDuiImage : public CDuiString
	{
	public:
		CDuiImage();
		CDuiImage(const CDuiString& src);
		CDuiImage(const CDuiImage& src);
		CDuiImage(LPCTSTR lpsz, int nLen = -1);
		~CDuiImage();

		void SetResType(int _iResType);
		void SetHole(bool _bHole);
		void SetTiledX(bool _bTiledX);
		void SetTiledY(bool _bTiledY);
		void SetFade(BYTE _bFade);
		void SetMask(DWORD _dwMask);
		
		void SetSource(RECT _rcSource);
		void SetDest(RECT _rcDest);
		void SetCorner(RECT _rcCorner);
		void SetImage(LPCTSTR _strImage);
		void SetImage(LPCTSTR _strImage,RECT _rcControl);
		void SetRes(LPCTSTR _strRes);
		int GetResType();
		void SetControlRect(RECT _rcControl);
		bool GetHole();
		bool GetTiledX();
		bool GetTiledY();
		BYTE GetFade();
		DWORD GetMask();
		RECT GetSource();
		RECT GetDest();
		RECT GetCorner();
		CDuiString GetRes();
		CDuiString GetImagePath();
		CDuiString GetImageSetting();
		void Init();
		const CDuiImage& operator=(const CDuiImage& src);
		const CDuiImage& operator=(LPCTSTR pstr);
	private:
		int m_iResType;
		bool m_bHole;
		bool m_bTiledX;
		bool m_bTiledY;
		bool m_bDestNeeded;
		BYTE m_bFade;
		DWORD m_dwMask;
		RECT m_rcItem;
		RECT m_rcSource;
		RECT m_rcDest;
		RECT m_rcRelativeDest;
		RECT m_rcCorner;
		CDuiString m_sRes;
		CDuiString m_sImageFile;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	struct DuiString_Compare : public stdext::hash_compare<CDuiString>
	{ 
		size_t operator()(const CDuiString & str)const
		{
			long _Quot = (long)(hash_value(str.GetData()) & LONG_MAX);
			ldiv_t _Qrem = ldiv(_Quot, 127773);
			_Qrem.rem = 16807 * _Qrem.rem - 2836 * _Qrem.quot;
			if (_Qrem.rem < 0)
				_Qrem.rem += LONG_MAX;
			return ((size_t)_Qrem.rem);
		}
		bool operator()(const CDuiString & a1, const CDuiString & a2)const
		{
			if(_tcsicmp(a1.GetData(),a2.GetData()) < 0)
				return true;
			return false;
		}
	};
	typedef hash_map<CDuiString,CDuiString,DuiString_Compare>  StringResMap;
	typedef hash_map<CDuiString,CDuiString,DuiString_Compare>::iterator StringResMapIt;
	/////////////////////////////////////////////////////////////////////////////////////
	//

	struct TITEM
	{
		CDuiString Key;
		LPVOID Data;
		struct TITEM* pPrev;
		struct TITEM* pNext;
	};

	class UILIB_API CStdStringPtrMap
	{
	public:
		CStdStringPtrMap(int nSize = 83);
		~CStdStringPtrMap();

		void Resize(int nSize = 83);
		LPVOID Find(LPCTSTR key, bool optimize = true) const;
		bool Insert(LPCTSTR key, LPVOID pData);
		bool ForceInsert(LPCTSTR key, LPVOID pData);
		LPVOID Set(LPCTSTR key, LPVOID pData);
		bool Remove(LPCTSTR key);
		void RemoveAll();
		int GetSize() const;
		LPCTSTR GetAt(int iIndex) const;
		LPCTSTR operator[] (int nIndex) const;

	protected:
		TITEM** m_aT;
		int m_nBuckets;
		int m_nCount;
	};

	template<typename T = LPVOID>
	class UILIB_API TStdStringPtrMap : public CStdStringPtrMap
	{
	public:
		TStdStringPtrMap(int nSize = 83):CStdStringPtrMap(nSize){};
		T GetAtObj(int iIndex) const {
			LPCTSTR nkey = GetAt(iIndex);
			if(!nkey)
				return NULL;
			return Find(nkey);
		}
		T Find(LPCTSTR key, bool optimize = true) const{return static_cast<T>(CStdStringPtrMap::Find(key,optimize));};
		bool Insert(LPCTSTR key,T pData){return CStdStringPtrMap::Insert(key);};
		T Set(LPCTSTR key,T pData){return static_cast<T>(CStdStringPtrMap::Set(key,pData));};
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CWaitCursor
	{
	public:
		CWaitCursor();
		~CWaitCursor();

	protected:
		HCURSOR m_hOrigCursor;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CVariant : public VARIANT
	{
	public:
		CVariant() 
		{ 
			VariantInit(this); 
		}
		CVariant(int i)
		{
			VariantInit(this);
			this->vt = VT_I4;
			this->intVal = i;
		}
		CVariant(float f)
		{
			VariantInit(this);
			this->vt = VT_R4;
			this->fltVal = f;
		}
		CVariant(LPOLESTR s)
		{
			VariantInit(this);
			this->vt = VT_BSTR;
			this->bstrVal = s;
		}
		CVariant(IDispatch *disp)
		{
			VariantInit(this);
			this->vt = VT_DISPATCH;
			this->pdispVal = disp;
		}

		~CVariant() 
		{ 
			VariantClear(this); 
		}
	};


	//////////////////////////////////////////////////////////////////////////////////////
	//
	enum Win32Type
	{
		Win32s,
		WinNT3,
		Win95,
		Win98,
		WinME,
		WinNT4,
		Win2000,
		WinXP,
		WinVista,
		Win7
	};

	class CWinVersion
	{
	public:
		CWinVersion(){}
		~CWinVersion(){}
		static Win32Type IsShellType()
		{
			Win32Type  ShellType;
			DWORD winVer;
			OSVERSIONINFO *osvi;

			winVer=GetVersion();
			if(winVer<0x80000000){/*NT */
				ShellType=WinNT3;
				osvi= (OSVERSIONINFO *)malloc(sizeof(OSVERSIONINFO));
				if (osvi!=NULL){
					memset(osvi,0,sizeof(OSVERSIONINFO));
					osvi->dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
					GetVersionEx(osvi);
					if(osvi->dwMajorVersion==4L)ShellType=WinNT4;
					else if(osvi->dwMajorVersion==5L&&osvi->dwMinorVersion==0L)ShellType=Win2000;
					else if(osvi->dwMajorVersion==5L&&osvi->dwMinorVersion==1L)ShellType=WinXP;
					else if(osvi->dwMajorVersion==6L&&osvi->dwMinorVersion==0L)ShellType=WinVista;
					else if(osvi->dwMajorVersion==6L&&osvi->dwMinorVersion==1L)ShellType=Win7;
					free(osvi);
				}
			}
			else if  (LOBYTE(LOWORD(winVer))<4)
				ShellType=Win32s;
			else{
				ShellType=Win95;
				osvi= (OSVERSIONINFO *)malloc(sizeof(OSVERSIONINFO));
				if (osvi!=NULL){
					memset(osvi,0,sizeof(OSVERSIONINFO));
					osvi->dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
					GetVersionEx(osvi);
					if(osvi->dwMajorVersion==4L&&osvi->dwMinorVersion==10L)ShellType=Win98;
					else if(osvi->dwMajorVersion==4L&&osvi->dwMinorVersion==90L)ShellType=WinME;
					free(osvi);
				}
			}
			return ShellType;
		}
	};
	static Win32Type g_Shell=CWinVersion::IsShellType();
}// namespace UiLib

#endif // __UTILS_H__