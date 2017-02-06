#include "stdafx.h"
#include "Utils.h"

namespace UiLib
{

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CDuiPoint::CDuiPoint()
	{
		x = y = 0;
	}

	CDuiPoint::CDuiPoint(const POINT& src)
	{
		x = src.x;
		y = src.y;
	}

	CDuiPoint::CDuiPoint(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	CDuiPoint::CDuiPoint(LPARAM lParam)
	{
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CDuiSize::CDuiSize()
	{
		cx = cy = 0;
	}

	CDuiSize::CDuiSize(const SIZE& src)
	{
		cx = src.cx;
		cy = src.cy;
	}

	CDuiSize::CDuiSize(const RECT rc)
	{
		cx = rc.right - rc.left;
		cy = rc.bottom - rc.top;
	}

	CDuiSize::CDuiSize(int _cx, int _cy)
	{
		cx = _cx;
		cy = _cy;
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CDuiRect::CDuiRect()
	{
		left = top = right = bottom = 0;
	}

	CDuiRect::CDuiRect(const RECT& src)
	{
		left = src.left;
		top = src.top;
		right = src.right;
		bottom = src.bottom;
	}

	CDuiRect::CDuiRect(int iLeft, int iTop, int iRight, int iBottom)
	{
		left = iLeft;
		top = iTop;
		right = iRight;
		bottom = iBottom;
	}

	int CDuiRect::GetWidth() const
	{
		return right - left;
	}

	int CDuiRect::GetHeight() const
	{
		return bottom - top;
	}

	void CDuiRect::Empty()
	{
		left = top = right = bottom = 0;
	}

	bool CDuiRect::IsNull() const
	{
		return (left == 0 && right == 0 && top == 0 && bottom == 0); 
	}

	void CDuiRect::Join(const RECT& rc)
	{
		if( rc.left < left ) left = rc.left;
		if( rc.top < top ) top = rc.top;
		if( rc.right > right ) right = rc.right;
		if( rc.bottom > bottom ) bottom = rc.bottom;
	}

	void CDuiRect::ResetOffset()
	{
		::OffsetRect(this, -left, -top);
	}

	void CDuiRect::Normalize()
	{
		if( left > right ) { int iTemp = left; left = right; right = iTemp; }
		if( top > bottom ) { int iTemp = top; top = bottom; bottom = iTemp; }
	}

	void CDuiRect::Offset(int cx, int cy)
	{
		::OffsetRect(this, cx, cy);
	}

	void CDuiRect::Inflate(int cx, int cy)
	{
		::InflateRect(this, cx, cy);
	}

	void CDuiRect::Deflate(int cx, int cy)
	{
		::InflateRect(this, -cx, -cy);
	}

	void CDuiRect::Union(CDuiRect& rc)
	{
		::UnionRect(this, this, &rc);
	}

	void CDuiRect::Intersect(CDuiRect &rc)
	{
		::IntersectRect(this,this,&rc);
	}

	void CDuiRect::Subtract(CDuiRect &rc)
	{
		::SubtractRect(this,this,&rc);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CStdPtrArray::CStdPtrArray(int iPreallocSize) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize)
	{
		ASSERT(iPreallocSize>=0);
		if( iPreallocSize > 0 ) m_ppVoid = static_cast<LPVOID*>(malloc(iPreallocSize * sizeof(LPVOID)));
	}

	CStdPtrArray::CStdPtrArray(const CStdPtrArray& src) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(0)
	{
		for(int i=0; i<src.GetSize(); i++)
			Add(src.GetAt(i));
	}

	CStdPtrArray::~CStdPtrArray()
	{
		if( m_ppVoid != NULL ) free(m_ppVoid);
	}

	void CStdPtrArray::Empty()
	{
		if( m_ppVoid != NULL ) free(m_ppVoid);
		m_ppVoid = NULL;
		m_nCount = m_nAllocated = 0;
	}

	void CStdPtrArray::Resize(int iSize)
	{
		Empty();
		m_ppVoid = static_cast<LPVOID*>(malloc(iSize * sizeof(LPVOID)));
		::ZeroMemory(m_ppVoid, iSize * sizeof(LPVOID));
		m_nAllocated = iSize;
		m_nCount = iSize;
	}

	bool CStdPtrArray::IsEmpty() const
	{
		return m_nCount == 0;
	}

	bool CStdPtrArray::Add(LPVOID pData)
	{
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
			if( ppVoid != NULL ) 
			{
				m_nAllocated = nAllocated;
				m_ppVoid = ppVoid;
			}
			else 
			{
				--m_nCount;
				return false;
			}
		}
		m_ppVoid[m_nCount - 1] = pData;
		return true;
	}

	bool CStdPtrArray::Insert(void *pItem,BASEPTRARRAYCOMPFUNC pCompFunc)
	{
		//空
		if (m_nCount==0) return Add(pItem);

		void *pItemTmp;
		if (m_nCount==1)
		{
			//只有单条记录
			pItemTmp=GetAt(0);
			if (pCompFunc(&pItem,&pItemTmp)<=0) return InsertAt(0,pItem);
			return Add(pItem);
		}
		int n1=0;
		int n2=m_nCount-1;
		//检查首条记录
		pItemTmp=GetAt(n1);
		if (pCompFunc(&pItem,&pItemTmp)<=0) return InsertAt(0,pItem);
		//检查最后记录
		pItemTmp=GetAt(n2);
		if (pCompFunc(&pItem,&pItemTmp)>=0) return Add(pItem);

#pragma warning(disable:4127)
		while (TRUE)
#pragma warning(default:4127)
		{
			//只剩两条记录了
			if (n1+1==n2) return InsertAt(n2,pItem);

			//取中间值
			int n=n1+(n2-n1)/2;
			pItemTmp=GetAt(n);
			int nRet=pCompFunc(&pItem,&pItemTmp);
			if (nRet==0) return InsertAt(n,pItem);
			else if (nRet<=0) n2=n;
			else n1=n;
		}
		return false;
	}

	bool CStdPtrArray::InsertAt(int iIndex, LPVOID pData)
	{
		if( iIndex == m_nCount ) return Add(pData);
		if( iIndex < 0 || iIndex > m_nCount ) return false;
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
			if( ppVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_ppVoid = ppVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		memmove(&m_ppVoid[iIndex + 1], &m_ppVoid[iIndex], (m_nCount - iIndex - 1) * sizeof(LPVOID));
		m_ppVoid[iIndex] = pData;
		return true;
	}

	bool CStdPtrArray::SetAt(int iIndex, LPVOID pData)
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return false;
		m_ppVoid[iIndex] = pData;
		return true;
	}

	void CStdPtrArray::Sink(int iIndex)
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return;
		LPVOID pData = m_ppVoid[iIndex];
		memmove(&m_ppVoid[iIndex], &m_ppVoid[iIndex+1], (m_nCount - iIndex - 1) * sizeof(LPVOID));
		m_ppVoid[m_nCount - 1] = pData;
	}

	bool CStdPtrArray::Remove(int iIndex)
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return false;
		if( iIndex < --m_nCount ) ::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + 1, (m_nCount - iIndex) * sizeof(LPVOID));
		return true;
	}

	bool CStdPtrArray::Remove(LPVOID pData)
	{
		int nIndex = Find(pData);
		return Remove(nIndex);
	}

	int CStdPtrArray::Find(LPVOID pData) const
	{
		for( int i = 0; i < m_nCount; i++ ) if( m_ppVoid[i] == pData ) return i;
		return -1;
	}

	int CStdPtrArray::GetSize() const
	{
		return m_nCount;
	}

	LPVOID* CStdPtrArray::GetData()
	{
		return m_ppVoid;
	}

	LPVOID CStdPtrArray::GetAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
		return m_ppVoid[iIndex];
	}

	LPVOID CStdPtrArray::operator[] (int iIndex) const
	{
		ASSERT(iIndex>=0 && iIndex<m_nCount);
		return m_ppVoid[iIndex];
	}

	CStdPtrArray& CStdPtrArray::operator =(CStdPtrArray& src)
	{
		Empty();
		for(int i=0; i<src.GetSize(); i++)
			Add(src.GetAt(i));
		return *this;
	}
	CStdPtrArray& CStdPtrArray::operator +=(CStdPtrArray& src)
	{
		for(int i=0; i<src.GetSize(); i++)
			Add(src.GetAt(i));
		return *this;
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CStdValArray::CStdValArray(int iElementSize, int iPreallocSize /*= 0*/) : 
	m_pVoid(NULL), 
		m_nCount(0), 
		m_iElementSize(iElementSize), 
		m_nAllocated(iPreallocSize)
	{
		ASSERT(iElementSize>0);
		ASSERT(iPreallocSize>=0);
		if( iPreallocSize > 0 ) m_pVoid = static_cast<LPBYTE>(malloc(iPreallocSize * m_iElementSize));
	}

	CStdValArray::~CStdValArray()
	{
		if( m_pVoid != NULL ) free(m_pVoid);
	}

	void CStdValArray::Empty(BOOL bFreeList)
	{   
		if (bFreeList)
		{
			free(m_pVoid);
			m_pVoid=NULL;
			m_nAllocated=0;
		}
		m_nCount = 0;  // NOTE: We keep the memory in place
	}

	bool CStdValArray::IsEmpty() const
	{
		return m_nCount == 0;
	}

	bool CStdValArray::Add(LPCVOID pData)
	{
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
		::CopyMemory(m_pVoid + ((m_nCount - 1) * m_iElementSize), pData, m_iElementSize);
		return true;
	}

	bool CStdValArray::Insert(int iIndex,LPCVOID pData)
	{
		if( iIndex >= m_nCount ) 
		{
			return Add(pData);
		}
		if( iIndex < 0) return false;
		if( ++m_nCount >= m_nAllocated) {
			m_nAllocated *= 2;
			if( m_nAllocated == 0 ) m_nAllocated = 11;
			m_pVoid = static_cast<LPBYTE>(realloc(m_pVoid, m_nAllocated * m_iElementSize));
			if( m_pVoid == NULL ) return false;
		}
		::CopyMemory(m_pVoid + (iIndex+1)*m_iElementSize, m_pVoid + iIndex * m_iElementSize, (m_nCount - iIndex - 1) * m_iElementSize);
		::CopyMemory(m_pVoid + iIndex * m_iElementSize ,pData, m_iElementSize);	
		return true;
	}

	//使用二分法进行快速插入,插入前是排序好的,插入后也是排序好的
	BOOL CStdValArray::Insert(void *pItem,BASEVALUECOMPFUNC pCompFunc)
	{
		//空
		if (m_nCount==0) return Add(pItem);

		void *pItemTmp;
		if (m_nCount==1)
		{
			//只有单条记录
			pItemTmp=GetAt(0);
			if (pCompFunc(pItem,pItemTmp)<=0) return Insert(0,pItem);
			return Add(pItem);
		}
		int n1=0;
		int n2=m_nCount-1;
		//检查首条记录
		pItemTmp=GetAt(n1);
		if (pCompFunc(pItem,pItemTmp)<=0) return Insert(0,pItem);
		//检查最后记录
		pItemTmp=GetAt(n2);
		if (pCompFunc(pItem,pItemTmp)>=0) return Add(pItem);

#pragma warning(disable:4127)
		while (TRUE)
#pragma warning(default:4127)
		{
			//只剩两条记录了
			if (n1+1==n2) return Insert(n2,pItem);

			//取中间值
			int n=n1+(n2-n1)/2;
			pItemTmp=GetAt(n);
			int nRet=pCompFunc(pItem,pItemTmp);
			if (nRet==0) return Insert(n,pItem);
			else if (nRet< 0) n2=n;
			else n1=n;
		}
		return FALSE;
	}

	//查找元素，顺序查找，在pCompFunc为NULL的时候，只比较pItem指针是否与m_pList中的相同，当有pCompFunc的时候，就会根据函数来比较两个元素是否相同
	//返回-1表示查找失败
	int CStdValArray::FindItemPos(void *pItem,BASEVALUECOMPFUNC pCompFunc)
	{
		if (m_nCount<=0) return -1;

		//pItem是列表中的一员
		if ((LPBYTE)pItem>=m_pVoid && (LPBYTE)pItem<m_pVoid+m_nCount*m_iElementSize) return ((LPBYTE)pItem-m_pVoid)/m_iElementSize;

		for (int i=0;i<m_nCount;i++)
		{
			BOOL bRet;
			if (pCompFunc==NULL)
			{
				bRet=memcmp(pItem,m_pVoid+i*m_iElementSize,m_iElementSize);
			}
			else
			{
				bRet=pCompFunc(m_pVoid+i*m_iElementSize,pItem);
			}
			if (bRet==0) return i;
		}
		return -1;
	}

	//查找元素，顺序查找，在pCompFunc为NULL的时候，只比较pItem指针是否与m_pList中的相同，当有pCompFunc的时候，就会根据函数来比较两个元素是否相同
	//返回NULL表示查找失败
	void* CStdValArray::FindItem(void*pItem,BASEVALUECOMPFUNC pCompFunc)
	{
		int nPos=FindItemPos(pItem,pCompFunc);
		if (nPos<0) return NULL;
		return m_pVoid+nPos*m_iElementSize;
	}

	bool CStdValArray::Remove(LPVOID pData)
	{
		int nIndex = FindItemPos(pData);
		return Remove(nIndex);	
	}

	bool CStdValArray::Remove(int iIndex)
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return false;
		if( iIndex < --m_nCount ) ::CopyMemory(m_pVoid + (iIndex * m_iElementSize), m_pVoid + ((iIndex + 1) * m_iElementSize), (m_nCount - iIndex) * m_iElementSize);
		return true;
	}

	int CStdValArray::GetSize() const
	{
		return m_nCount;
	}

	LPVOID CStdValArray::GetData()
	{
		return static_cast<LPVOID>(m_pVoid);
	}

	LPVOID CStdValArray::GetAt(int iIndex) const
	{
		if( iIndex < 0 || iIndex >= m_nCount ) return NULL;
		return m_pVoid + (iIndex * m_iElementSize);
	}

	LPVOID CStdValArray::operator[] (int iIndex) const
	{
		ASSERT(iIndex>=0 && iIndex<m_nCount);
		return m_pVoid + (iIndex * m_iElementSize);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CDuiString::CDuiString() : m_pstr(m_szBuffer)
	{
		m_szBuffer[0] = '\0';
	}

	CDuiString::CDuiString(const TCHAR ch) : m_pstr(m_szBuffer)
	{
		m_szBuffer[0] = ch;
		m_szBuffer[1] = '\0';
	}

	CDuiString::CDuiString(LPCTSTR lpsz, int nLen) : m_pstr(m_szBuffer)
	{      
		ASSERT(!::IsBadStringPtr(lpsz,-1) || lpsz==NULL);
		m_szBuffer[0] = '\0';
		Assign(lpsz, nLen);
	}

	CDuiString::CDuiString(const CDuiString& src) : m_pstr(m_szBuffer)
	{
		m_szBuffer[0] = '\0';
		Assign(src.m_pstr);
	}

	CDuiString::~CDuiString()
	{
		if( m_pstr != m_szBuffer ) free(m_pstr);
	}

	int CDuiString::GetLength() const
	{ 
		return (int) _tcslen(m_pstr); 
	}

	CDuiString::operator LPCTSTR() const 
	{ 
		return m_pstr; 
	}

	void CDuiString::Append(LPCTSTR pstr)
	{
		int nNewLength = GetLength() + (int) _tcslen(pstr);
		if( nNewLength >= MAX_LOCAL_STRING_LEN ) {
			if( m_pstr == m_szBuffer ) {
				m_pstr = static_cast<LPTSTR>(malloc((nNewLength + 1) * sizeof(TCHAR)));
				_tcscpy(m_pstr, m_szBuffer);
				_tcscat(m_pstr, pstr);
			}
			else {
				m_pstr = static_cast<LPTSTR>(realloc(m_pstr, (nNewLength + 1) * sizeof(TCHAR)));
				_tcscat(m_pstr, pstr);
			}
		}
		else {
			if( m_pstr != m_szBuffer ) {
				free(m_pstr);
				m_pstr = m_szBuffer;
			}
			_tcscat(m_szBuffer, pstr);
		}
	}

	void CDuiString::Assign(LPCTSTR pstr, int cchMax)
	{
		if( pstr == NULL ) pstr = _T("");
		cchMax = (cchMax < 0 ? (int) _tcslen(pstr) : cchMax);
		if( cchMax < MAX_LOCAL_STRING_LEN ) {
			if( m_pstr != m_szBuffer ) {
				free(m_pstr);
				m_pstr = m_szBuffer;
			}
		}
		else if( cchMax > GetLength() || m_pstr == m_szBuffer ) {
			if( m_pstr == m_szBuffer ) m_pstr = NULL;
			m_pstr = static_cast<LPTSTR>(realloc(m_pstr, (cchMax + 1) * sizeof(TCHAR)));
		}
		_tcsncpy(m_pstr, pstr, cchMax);
		m_pstr[cchMax] = '\0';
	}

	bool CDuiString::IsEmpty() const 
	{ 
		return m_pstr[0] == '\0'; 
	}

	void CDuiString::Empty() 
	{ 
		if( m_pstr != m_szBuffer ) free(m_pstr);
		m_pstr = m_szBuffer;
		m_szBuffer[0] = '\0'; 
	}

	LPCTSTR CDuiString::GetData() const
	{
		return m_pstr;
	}

	TCHAR CDuiString::GetAt(int nIndex) const
	{
		return m_pstr[nIndex];
	}

	TCHAR CDuiString::operator[] (int nIndex) const
	{ 
		return m_pstr[nIndex];
	}   

	const CDuiString& CDuiString::operator=(const CDuiString& src)
	{      
		Assign(src);
		return *this;
	}

	const CDuiString& CDuiString::operator=(LPCTSTR lpStr)
	{      
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtr(lpStr,-1));
			Assign(lpStr);
		}
		else
		{
			Empty();
		}
		return *this;
	}

#ifdef _UNICODE

	const CDuiString& CDuiString::operator=(LPCSTR lpStr)
	{
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtrA(lpStr,-1));
			int cchStr = 0;
			cchStr = MultiByteToWideChar(::GetACP(),0,lpStr,-1,NULL,0);
			LPWSTR pwstr = (LPWSTR) _alloca(cchStr*2);
			if( pwstr != NULL ) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr) ;
			Assign(pwstr);
		}
		else
		{
			Empty();
		}
		return *this;
	}

	const CDuiString& CDuiString::operator+=(LPCSTR lpStr)
	{
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtrA(lpStr,-1));
			//int cchStr = (int) strlen(lpStr) + 1;
			//LPWSTR pwstr = (LPWSTR) _alloca(cchStr);
			int cchStr = 0;
			cchStr = MultiByteToWideChar(::GetACP(),0,lpStr,-1,NULL,0);
			LPWSTR pwstr = (LPWSTR) _alloca(cchStr*2);
			if( pwstr != NULL ) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr) ;
			Append(pwstr);
		}

		return *this;
	}

#else

	const CDuiString& CDuiString::operator=(LPCWSTR lpwStr)
	{      
		if ( lpwStr )
		{
			ASSERT(!::IsBadStringPtrW(lpwStr,-1));
			int cchStr = ((int) wcslen(lpwStr) * 2) + 1;
			LPSTR pstr = (LPSTR) _alloca(cchStr);
			if( pstr != NULL ) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
			Assign(pstr);
		}
		else
		{
			Empty();
		}

		return *this;
	}

	const CDuiString& CDuiString::operator+=(LPCWSTR lpwStr)
	{
		if ( lpwStr )
		{
			ASSERT(!::IsBadStringPtrW(lpwStr,-1));
			int cchStr = ((int) wcslen(lpwStr) * 2) + 1;
			LPSTR pstr = (LPSTR) _alloca(cchStr);
			if( pstr != NULL ) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
			Append(pstr);
		}

		return *this;
	}

#endif // _UNICODE

	const CDuiString& CDuiString::operator=(const TCHAR ch)
	{
		Empty();
		m_szBuffer[0] = ch;
		m_szBuffer[1] = '\0';
		return *this;
	}

	CDuiString CDuiString::operator+(const CDuiString& src) const
	{
		CDuiString sTemp = *this;
		sTemp.Append(src);
		return sTemp;
	}

	CDuiString CDuiString::operator+(LPCTSTR lpStr) const
	{
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtr(lpStr,-1));
			CDuiString sTemp = *this;
			sTemp.Append(lpStr);
			return sTemp;
		}

		return *this;
	}

	const CDuiString& CDuiString::operator+=(const CDuiString& src)
	{      
		Append(src);
		return *this;
	}

	const CDuiString& CDuiString::operator+=(LPCTSTR lpStr)
	{      
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtr(lpStr,-1));
			Append(lpStr);
		}

		return *this;
	}

	const CDuiString& CDuiString::operator+=(const TCHAR ch)
	{      
		TCHAR str[] = { ch, '\0' };
		Append(str);
		return *this;
	}

	bool CDuiString::operator == (LPCTSTR str) const { return (Compare(str) == 0); };
	bool CDuiString::operator != (LPCTSTR str) const { return (Compare(str) != 0); };
	bool CDuiString::operator <= (LPCTSTR str) const { return (Compare(str) <= 0); };
	bool CDuiString::operator <  (LPCTSTR str) const { return (Compare(str) <  0); };
	bool CDuiString::operator >= (LPCTSTR str) const { return (Compare(str) >= 0); };
	bool CDuiString::operator >  (LPCTSTR str) const { return (Compare(str) >  0); };

	void CDuiString::SetAt(int nIndex, TCHAR ch)
	{
		ASSERT(nIndex>=0 && nIndex<GetLength());
		m_pstr[nIndex] = ch;
	}

	int CDuiString::Compare(LPCTSTR lpsz) const 
	{ 
		return _tcscmp(m_pstr, lpsz); 
	}

	int CDuiString::CompareNoCase(LPCTSTR lpsz) const 
	{ 
		return _tcsicmp(m_pstr, lpsz); 
	}

	void CDuiString::MakeUpper() 
	{ 
		_tcsupr(m_pstr); 
	}

	void CDuiString::MakeLower() 
	{ 
		_tcslwr(m_pstr); 
	}

	CDuiString CDuiString::Left(int iLength) const
	{
		if( iLength < 0 ) iLength = 0;
		if( iLength > GetLength() ) iLength = GetLength();
		return CDuiString(m_pstr, iLength);
	}

	CDuiString CDuiString::Mid(int iPos, int iLength) const
	{
		if( iLength < 0 ) iLength = GetLength() - iPos;
		if( iPos + iLength > GetLength() ) iLength = GetLength() - iPos;
		if( iLength <= 0 ) return CDuiString();
		return CDuiString(m_pstr + iPos, iLength);
	}

	CDuiString CDuiString::Right(int iLength) const
	{
		int iPos = GetLength() - iLength;
		if( iPos < 0 ) {
			iPos = 0;
			iLength = GetLength();
		}
		return CDuiString(m_pstr + iPos, iLength);
	}

	int CDuiString::Find(TCHAR ch, int iPos /*= 0*/) const
	{
		ASSERT(iPos>=0 && iPos<=GetLength());
		if( iPos != 0 && (iPos < 0 || iPos >= GetLength()) ) return -1;
		LPCTSTR p = _tcschr(m_pstr + iPos, ch);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int CDuiString::Find(LPCTSTR pstrSub, int iPos /*= 0*/) const
	{
		ASSERT(!::IsBadStringPtr(pstrSub,-1));
		ASSERT(iPos>=0 && iPos<=GetLength());
		if( iPos != 0 && (iPos < 0 || iPos > GetLength()) ) return -1;
		LPCTSTR p = _tcsstr(m_pstr + iPos, pstrSub);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int CDuiString::ReverseFind(TCHAR ch) const
	{
		LPCTSTR p = _tcsrchr(m_pstr, ch);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int CDuiString::Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo)
	{
		CDuiString sTemp;
		int nCount = 0;
		int iPos = Find(pstrFrom);
		if( iPos < 0 ) return 0;
		int cchFrom = (int) _tcslen(pstrFrom);
		int cchTo = (int) _tcslen(pstrTo);
		while( iPos >= 0 ) {
			sTemp = Left(iPos);
			sTemp += pstrTo;
			sTemp += Mid(iPos + cchFrom);
			Assign(sTemp);
			iPos = Find(pstrFrom, iPos + cchTo);
			nCount++;
		}
		return nCount;
	}

	char* CDuiString::UnicodeToAnsi()
	{
		int nBufferLen = 0;
		char* pBuffer = NULL;
#ifdef _UNICODE
		nBufferLen = WideCharToMultiByte(::GetACP(),0,GetData(),-1,NULL,0,NULL,NULL);
		pBuffer = new char[nBufferLen];
		if (pBuffer != NULL)
			WideCharToMultiByte(::GetACP(),0,GetData(),-1,pBuffer,nBufferLen,NULL,NULL);
#else
		nBufferLen = GetLength()+1;
		pBuffer = new char[nBufferLen];
		if (pBuffer != NULL)
			strcpy(pBuffer,GetData());
#endif
		return pBuffer;
	}

	int CDuiString::Format(LPCTSTR pstrFormat, ...)
	{
		LPTSTR szSprintf = NULL;
		va_list argList;
		int nLen;
		va_start(argList, pstrFormat);
		nLen = ::_vsntprintf(NULL, 0, pstrFormat, argList);
		szSprintf = (TCHAR*)malloc((nLen + 1) * sizeof(TCHAR));
		ZeroMemory(szSprintf, (nLen + 1) * sizeof(TCHAR));
		int iRet = ::_vsntprintf(szSprintf, nLen + 1, pstrFormat, argList);
		va_end(argList);
		Assign(szSprintf);
		free(szSprintf);
		return iRet;
	}

	int CDuiString::SmallFormat(LPCTSTR pstrFormat, ...)
	{
		CDuiString sFormat = pstrFormat;
		TCHAR szBuffer[64] = { 0 };
		va_list argList;
		va_start(argList, pstrFormat);
		int iRet = ::wvsprintf(szBuffer, sFormat, argList);
		va_end(argList);
		Assign(szBuffer);
		return iRet;
	}

	//************************************
	// 函数名称: GetStringW
	// 返回类型: std::wstring
	// 函数说明: 
	//************************************
	std::wstring CDuiString::GetStringW()
	{
#ifdef _UNICODE
		return GetData();
#else
		if (!this->IsEmpty()){
			wstring nRet = CA2W(GetData());
			return nRet;
		}
		return _T("");
#endif // _UNICODE
	}

	//************************************
	// 函数名称: GetStringA
	// 返回类型: std::string
	// 函数说明: 
	//************************************
	std::string CDuiString::GetStringA()
	{
#ifdef _UNICODE
		if(!IsEmpty()){
			string nRet = CW2A(GetData());
			return nRet;
		}
		return "";
#else
		return GetData();
#endif // _UNICODE
	}


	/////////////////////////////////////////////////////////////////////////////
	//
	//

	static UINT HashKey(LPCTSTR Key)
	{
		UINT i = 0;
		SIZE_T len = _tcslen(Key);
		while( len-- > 0 ) i = (i << 5) + i + Key[len];
		return i;
	}

	static UINT HashKey(const CDuiString& Key)
	{
		return HashKey((LPCTSTR)Key);
	};

	CStdStringPtrMap::CStdStringPtrMap(int nSize) : m_nCount(0)
	{
		if( nSize < 16 ) nSize = 16;
		m_nBuckets = nSize;
		m_aT = new TITEM*[nSize];
		memset(m_aT, 0, nSize * sizeof(TITEM*));
	}

	CStdStringPtrMap::~CStdStringPtrMap()
	{
		if( m_aT ) {
			int len = m_nBuckets;
			while( len-- ) {
				TITEM* pItem = m_aT[len];
				while( pItem ) {
					TITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}
	}

	void CStdStringPtrMap::RemoveAll()
	{
		this->Resize(0);
	}

	void CStdStringPtrMap::Resize(int nSize)
	{
		if( m_aT ) {
			int len = m_nBuckets;
			while( len-- ) {
				TITEM* pItem = m_aT[len];
				while( pItem ) {
					TITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}

		if( nSize <= 0 ) nSize = 0;
		if( nSize > 0 ) {
			m_aT = new TITEM*[nSize];
			memset(m_aT, 0, nSize * sizeof(TITEM*));
		} 
		m_nBuckets = nSize;
		m_nCount = 0;
	}

	LPVOID CStdStringPtrMap::Find(LPCTSTR key, bool optimize) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return NULL;

		UINT slot = HashKey(key) % m_nBuckets;
		for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) {
			if( pItem->Key == key ) {
				if (optimize && pItem != m_aT[slot]) {
					if (pItem->pNext) {
						pItem->pNext->pPrev = pItem->pPrev;
					}
					pItem->pPrev->pNext = pItem->pNext;
					pItem->pPrev = NULL;
					pItem->pNext = m_aT[slot];
					pItem->pNext->pPrev = pItem;
					//将item移动至链条头部
					m_aT[slot] = pItem;
				}
				return pItem->Data;
			}        
		}

		return NULL;
	}

	bool CStdStringPtrMap::Insert(LPCTSTR key, LPVOID pData)
	{
		if( m_nBuckets == 0 ) return false;
		if( Find(key) ) return false;

		// Add first in bucket
		UINT slot = HashKey(key) % m_nBuckets;
		TITEM* pItem = new TITEM;
		pItem->Key = key;
		pItem->Data = pData;
		pItem->pPrev = NULL;
		pItem->pNext = m_aT[slot];
		if (pItem->pNext)
			pItem->pNext->pPrev = pItem;
		m_aT[slot] = pItem;
		m_nCount++;
		return true;
	}

	bool CStdStringPtrMap::ForceInsert(LPCTSTR key, LPVOID pData)
	{
		if( m_nBuckets == 0 ) return false;
		if( Find(key) )
			Remove(key);
		// Add first in bucket
		UINT slot = HashKey(key) % m_nBuckets;
		TITEM* pItem = new TITEM;
		pItem->Key = key;
		pItem->Data = pData;
		pItem->pPrev = NULL;
		pItem->pNext = m_aT[slot];
		if (pItem->pNext)
			pItem->pNext->pPrev = pItem;
		m_aT[slot] = pItem;
		m_nCount++;
		return true;
	}

	LPVOID CStdStringPtrMap::Set(LPCTSTR key, LPVOID pData)
	{
		if( m_nBuckets == 0 ) return pData;

		if (GetSize()>0) {
			UINT slot = HashKey(key) % m_nBuckets;
			// Modify existing item
			for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) {
				if( pItem->Key == key ) {
					LPVOID pOldData = pItem->Data;
					pItem->Data = pData;
					return pOldData;
				}
			}
		}

		Insert(key, pData);
		return NULL;
	}

	bool CStdStringPtrMap::Remove(LPCTSTR key)
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return false;

		UINT slot = HashKey(key) % m_nBuckets;
		TITEM** ppItem = &m_aT[slot];
		while( *ppItem ) {
			if( (*ppItem)->Key == key ) {
				TITEM* pKill = *ppItem;
				*ppItem = (*ppItem)->pNext;
				if (*ppItem)
					(*ppItem)->pPrev = pKill->pPrev;
				delete pKill;
				m_nCount--;
				return true;
			}
			ppItem = &((*ppItem)->pNext);
		}

		return false;
	}

	int CStdStringPtrMap::GetSize() const
	{
#if 0//def _DEBUG
		int nCount = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( const TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) nCount++;
		}
		ASSERT(m_nCount==nCount);
#endif
		return m_nCount;
	}

	LPCTSTR CStdStringPtrMap::GetAt(int iIndex) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return false;

		int pos = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) {
				if( pos++ == iIndex ) {
					return pItem->Key.GetData();
				}
			}
		}

		return NULL;
	}

	LPCTSTR CStdStringPtrMap::operator[] (int nIndex) const
	{
		return GetAt(nIndex);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CWaitCursor::CWaitCursor()
	{
		m_hOrigCursor = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	}

	CWaitCursor::~CWaitCursor()
	{
		::SetCursor(m_hOrigCursor);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	CDuiImage::CDuiImage():m_iResType(0),m_bHole(false),m_bTiledX(false),m_bDestNeeded(true),m_bTiledY(0),m_bFade(255),m_dwMask(0)
	{
		Init();
	}

	void CDuiImage::Init()
	{
		m_rcItem.left = 0;
		m_rcItem.right = 0;
		m_rcItem.top = 0;
		m_rcItem.bottom = 0;

		m_rcSource.left = 0;
		m_rcSource.right = 0;
		m_rcSource.top = 0;
		m_rcSource.bottom = 0;

		m_rcDest.left = 0;
		m_rcDest.right = 0;
		m_rcDest.top = 0;
		m_rcDest.bottom = 0;

		m_rcRelativeDest.left = 0;
		m_rcRelativeDest.right = 0;
		m_rcRelativeDest.top = 0;
		m_rcRelativeDest.bottom = 0;

		m_rcCorner.left = 0;
		m_rcCorner.right = 0;
		m_rcCorner.top = 0;
		m_rcCorner.bottom = 0;
	}

	CDuiImage::CDuiImage( const CDuiString& src )
	{
		SetImage(src);
	}

	CDuiImage::CDuiImage( const CDuiImage& src )
	{
		SetImage(src);
	}

	CDuiImage::CDuiImage( LPCTSTR lpsz, int nLen /*= -1*/ )
	{
		if(nLen <= 0)
			SetImage(lpsz);
		else 
			SetImage(CDuiString(lpsz).Left(nLen));
	}

	CDuiImage::~CDuiImage()
	{

	}

	void CDuiImage::SetResType( int _iResType )
	{
		m_iResType = _iResType;
	}

	void CDuiImage::SetHole( bool _bHole )
	{
		m_bFade = _bHole;
	}

	void CDuiImage::SetTiledX( bool _bTiledX )
	{
		m_bTiledX = _bTiledX;
	}

	void CDuiImage::SetTiledY( bool _bTiledY )
	{
		m_bTiledY = _bTiledY;
	}

	void CDuiImage::SetFade( BYTE _bFade )
	{
		if(_bFade >= 0 && _bFade <= 255)
			m_bFade = _bFade;
	}

	void CDuiImage::SetMask( DWORD _dwMask )
	{
		m_dwMask = _dwMask;
	}

	void CDuiImage::SetControlRect( RECT _rcControl )
	{
		if(m_bDestNeeded || (m_rcItem.left == m_rcItem.right) || (m_rcItem.top == m_rcItem.bottom) || (m_rcItem.left != _rcControl.left) || (m_rcItem.top != _rcControl.top) || (m_rcItem.right != _rcControl.right) || (m_rcItem.bottom != _rcControl.bottom))
		{
			m_bDestNeeded = false;
			m_rcItem = _rcControl;

			m_rcDest.left = m_rcItem.left + m_rcRelativeDest.left;
			m_rcDest.top = m_rcItem.top + m_rcRelativeDest.top;
			m_rcDest.right = m_rcItem.left + m_rcRelativeDest.right;
			m_rcDest.bottom	= m_rcItem.top + m_rcRelativeDest.bottom;

			if(m_rcDest.right > m_rcItem.right)
				m_rcDest.right	= m_rcItem.right;
			if(m_rcDest.bottom > m_rcItem.bottom)
				m_rcDest.bottom	= m_rcItem.bottom;
		}
	}

	void CDuiImage::SetSource( RECT _rcSource )
	{
		m_rcSource = _rcSource;
	}

	void CDuiImage::SetDest( RECT _rcDest )
	{
		if((_rcDest.left == _rcDest.right ) || (_rcDest.top == _rcDest.bottom))
			return;
		m_bDestNeeded = true;
		m_rcRelativeDest = _rcDest;
	}

	void CDuiImage::SetCorner( RECT _rcCorner )
	{
		m_rcCorner = _rcCorner;
	}
	
	void CDuiImage::SetImage( LPCTSTR _strImage )
	{
		SetImage(_strImage,m_rcItem);
	}

	void CDuiImage::SetImage( LPCTSTR _strImage,RECT _rcControl )
	{
		if(!_strImage)
			return;

		CDuiString::Assign(_strImage);

		CDuiString sItem;
		CDuiString sValue;
		LPTSTR pstr		= NULL;
		m_sImageFile	= _strImage;
		m_rcItem		= _rcControl;

		while(*_strImage != _T('\0')){
			sItem.Empty();
			sValue.Empty();
			while(*_strImage > _T('\0') && *_strImage <= _T(' '))
				_strImage = ::CharNext(_strImage);

			while(*_strImage != _T('\0') && *_strImage != _T('=') && *_strImage > _T(' ') ) {
				LPTSTR pstrTemp = ::CharNext(_strImage);
				while( _strImage < pstrTemp){
					sItem += *_strImage++;
				}
			}
			while(*_strImage > _T('\0') && *_strImage <= _T(' '))
				_strImage = ::CharNext(_strImage);

			if(*_strImage++ != _T('=') )
				break;

			while(*_strImage > _T('\0') && *_strImage <= _T(' '))
				_strImage = ::CharNext(_strImage);

			if(*_strImage++ != _T('\'') )
				break;

			while(*_strImage != _T('\0') && *_strImage != _T('\'')){
				LPTSTR pstrTemp = ::CharNext(_strImage);
				while(_strImage < pstrTemp)
					sValue += *_strImage++;
			}
			if(*_strImage++ != _T('\'')) break;
			if(!sValue.IsEmpty() ){
				if(sItem == _T("file") || sItem == _T("res"))
					m_sImageFile = sValue;
				else if(sItem == _T("restype"))
					m_sRes = sValue;
				else if(sItem == _T("dest"))
				{
					m_rcRelativeDest.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);    
					m_rcRelativeDest.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
					m_rcRelativeDest.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
					m_rcRelativeDest.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);

					m_rcDest.left = m_rcItem.left + m_rcRelativeDest.left;     
					m_rcDest.top = m_rcItem.top + m_rcRelativeDest.top; 
					m_rcDest.right = m_rcItem.left + m_rcRelativeDest.right;
					if (m_rcDest.right > m_rcItem.right)
						m_rcDest.right = m_rcItem.right;

					m_rcDest.bottom = m_rcItem.top + m_rcRelativeDest.bottom;
					if (m_rcDest.bottom > m_rcItem.bottom)
						m_rcDest.bottom = m_rcItem.bottom;
				}
				else if(sItem == _T("source")){
					m_rcSource.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);    
					m_rcSource.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
					m_rcSource.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
					m_rcSource.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);  
				}
				else if(sItem == _T("corner")){
					m_rcCorner.left = _tcstol(sValue.GetData(), &pstr, 10);  ASSERT(pstr);    
					m_rcCorner.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
					m_rcCorner.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
					m_rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
				}
				else if(sItem == _T("mask")){
					if(sValue[0] == _T('#'))
						m_dwMask	= _tcstoul(sValue.GetData() + 1, &pstr, 16);
					else
						m_dwMask	= _tcstoul(sValue.GetData(), &pstr, 16);
				}
				else if(sItem == _T("fade"))
					m_bFade		= (BYTE)_tcstoul(sValue.GetData(), &pstr, 10);
				else if(sItem == _T("hole"))
					m_bHole		= (_tcscmp(sValue.GetData(), _T("true")) == 0);
				else if(sItem == _T("xtiled"))
					m_bTiledX	= (_tcscmp(sValue.GetData(), _T("true")) == 0);
				else if( sItem == _T("ytiled"))
					m_bTiledY	= (_tcscmp(sValue.GetData(), _T("true")) == 0);
			}
			if( *_strImage++ != _T(' ') ) break;
		}
	}

	void CDuiImage::SetRes( LPCTSTR _strResType )
	{
		if(!_strResType)
			m_sRes.Empty();

		m_sRes = _strResType;
	}

	int CDuiImage::GetResType()
	{
		return m_iResType;
	}

	bool CDuiImage::GetHole()
	{
		return m_bHole;
	}

	bool CDuiImage::GetTiledX()
	{
		return m_bTiledX;
	}

	bool CDuiImage::GetTiledY()
	{
		return m_bTiledY;
	}

	BYTE CDuiImage::GetFade()
	{
		return m_bFade;
	}

	DWORD CDuiImage::GetMask()
	{
		return m_dwMask;
	}

	RECT CDuiImage::GetSource()
	{
		return m_rcSource;
	}

	RECT CDuiImage::GetDest()
	{
		if((m_rcDest.left == m_rcDest.right ) || (m_rcDest.top == m_rcDest.bottom))
			return m_rcItem;
		return m_rcDest;
	}

	RECT CDuiImage::GetCorner()
	{
		return m_rcCorner;
	}

	UiLib::CDuiString CDuiImage::GetRes()
	{
		return m_sRes;
	}

	UiLib::CDuiString CDuiImage::GetImagePath()
	{
		return m_sImageFile;
	}

	UiLib::CDuiString CDuiImage::GetImageSetting()
	{
		if(m_sImageFile.IsEmpty() && m_sRes.IsEmpty())
			return _T("");

		CDuiString mImageSetting;
		mImageSetting.Format(_T("file='%s'"),m_sImageFile.GetData());
		if(!m_sRes.IsEmpty())
			mImageSetting.Format(_T(" %s res='%s'"),mImageSetting.GetData(),m_sRes.GetData());
		mImageSetting.Format(_T(" %s dest='%d,%d,%d,%d'"),mImageSetting.GetData(),m_rcDest.left,m_rcDest.top,m_rcDest.right,m_rcDest.bottom);
		mImageSetting.Format(_T(" %s source='%d,%d,%d,%d'"),mImageSetting.GetData(),m_rcSource.left,m_rcSource.top,m_rcSource.right,m_rcSource.bottom);
		mImageSetting.Format(_T(" %s corner='%d,%d,%d,%d'"),mImageSetting.GetData(),m_rcCorner.left,m_rcCorner.top,m_rcCorner.right,m_rcCorner.bottom);
		if(!m_dwMask)
			mImageSetting.Format(_T(" %s mask='#FF%x%x%x'"),mImageSetting.GetData(),GetRValue(m_dwMask),GetGValue(m_dwMask),GetBValue(m_dwMask));
		mImageSetting.Format(_T(" %s fade='%d'"),mImageSetting.GetData(),m_bFade);
		mImageSetting.Format(_T(" %s hole='%s'"),mImageSetting.GetData(),m_bHole?_T("true"):_T("false"));
		mImageSetting.Format(_T(" %s xtiled='%s'"),mImageSetting.GetData(),m_bTiledX?_T("true"):_T("false"));
		mImageSetting.Format(_T(" %s ytiled='%s'"),mImageSetting.GetData(),m_bTiledY?_T("true"):_T("false"));

		return mImageSetting;
	}

	const CDuiImage& CDuiImage::operator=( const CDuiImage& src )
	{
		Init();
		SetImage(src.GetData());
		return *this;
	}

	const CDuiImage& CDuiImage::operator=( LPCTSTR pstr )
	{
		Init();
		SetImage(pstr);
		return *this;
	}

} // namespace UiLib