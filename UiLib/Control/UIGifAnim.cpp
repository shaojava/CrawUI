#include "StdAfx.h"
#include "UIGifAnim.h"

namespace UiLib
{
	REGIST_DUICLASS(CGifAnimUI);

	CGifAnimUI::CGifAnimUI(void)
	{
		m_pGifImage			=	NULL;
		m_pPropertyItem		=	NULL;
		m_nFrameCount		=	0;	
		m_nFramePosition	=	0;	
	}


	CGifAnimUI::~CGifAnimUI(void)
	{
		DeleteGif();
		m_pManager->KillTimer( this, EVENT_TIEM_ID );
	}

	//************************************
	// Method:    GetClass
	// FullName:  CGifAnimUI::GetClass
	// Access:    public 
	// Returns:   LPCTSTR
	// Qualifier: const
	// Note:	  
	//************************************
	LPCTSTR CGifAnimUI::GetClass() const
	{
		try
		{
			return _T("GifAnimUI");
		}
		catch (...)
		{
			throw "CGifAnimUI::GetClass";
		}
	}

	//************************************
	// Method:    GetInterface
	// FullName:  CGifAnimUI::GetInterface
	// Access:    public 
	// Returns:   LPVOID
	// Qualifier:
	// Parameter: LPCTSTR pstrName
	// Note:	  
	//************************************
	LPVOID CGifAnimUI::GetInterface( LPCTSTR pstrName )
	{
		try
		{
			if( _tcscmp(pstrName, _T("GifAnim")) == 0 ) return static_cast<CGifAnimUI*>(this);
			return CLabelUI::GetInterface(pstrName);
		}
		catch (...)
		{
			throw "CGifAnimUI::GetInterface";
		}
	}

	void CGifAnimUI::DoPaint(const RECT& rcPaint )
	{
		try
		{
			if( !::IntersectRect( &m_rcPaint, &rcPaint, &m_rcItem ) ) return;
			if ( NULL == m_pGifImage )
			{		
				InitGifImage();
			}
			HDC hDC = m_pManager->GetRenderCore()->GetRenderHDC();
			DrawFrame( hDC );
		}
		catch (...)
		{
			throw "CGifAnimUI::DoPaint";
		}
	}

	void CGifAnimUI::DoEvent( TEventUI& event )
	{
		try
		{
			if( event.Type == UIEVENT_TIMER )
				OnTimer( (UINT_PTR)event.wParam );
		}
		catch (...)
		{
			throw "CGifAnimUI::DoEvent";
		}
	}

	//************************************
	// Method:    InitGifImage
	// FullName:  CGifAnimUI::InitGifImage
	// Access:    private 
	// Returns:   void
	// Qualifier:
	// Note:	  
	//************************************
	void CGifAnimUI::InitGifImage()
	{
		try
		{
			CDuiString sImageName	=	GetBkImage();
			CDuiString sImagePath	=	CPaintManagerUI::GetResourcePath();
			sImagePath += sImageName;
			if ( NULL == sImagePath ) return;
			m_pGifImage = Image::FromFile( sImagePath );
			if ( NULL == m_pGifImage ) return;
			UINT nCount	= 0;
			nCount	=	m_pGifImage->GetFrameDimensionsCount();
			GUID* pDimensionIDs	=	new GUID[ nCount ];
			m_pGifImage->GetFrameDimensionsList( pDimensionIDs, nCount );
			m_nFrameCount	=	m_pGifImage->GetFrameCount( &pDimensionIDs[0] );
			int nSize		=	m_pGifImage->GetPropertyItemSize( PropertyTagFrameDelay );
			m_pPropertyItem	=	(PropertyItem*) malloc( nSize );
			m_pGifImage->GetPropertyItem( PropertyTagFrameDelay, nSize, m_pPropertyItem );
			delete  pDimensionIDs;
			pDimensionIDs = NULL;

			long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
			if ( lPause == 0 ) lPause = 100;
			m_pManager->SetTimer( this, EVENT_TIEM_ID, lPause );
		}
		catch (...)
		{
			throw "CGifAnimUI::InitGifImage";
		}
	}

	//************************************
	// Method:    DeleteGif
	// FullName:  CGifAnimUI::DeleteGif
	// Access:    private 
	// Returns:   void
	// Qualifier:
	// Note:	  
	//************************************
	void CGifAnimUI::DeleteGif()
	{
		try
		{
			if ( m_pGifImage != NULL )
			{
				delete m_pGifImage;
				m_pGifImage = NULL;
			}

			if ( m_pPropertyItem != NULL )
			{
				free( m_pPropertyItem );
				m_pPropertyItem = NULL;
			}
			m_nFrameCount		=	0;	
			m_nFramePosition	=	0;	
		}
		catch (...)
		{
			throw "CGifAnimUI::DeleteGif";
		}
	}

	//************************************
	// Method:    OnTimer
	// FullName:  CGifAnimUI::OnTimer
	// Access:    private 
	// Returns:   void
	// Qualifier:
	// Parameter: UINT_PTR idEvent
	// Note:	  
	//************************************
	void CGifAnimUI::OnTimer( UINT_PTR idEvent )
	{
		try
		{
			if ( idEvent != EVENT_TIEM_ID )
				return;
			m_pManager->KillTimer( this, EVENT_TIEM_ID );
			m_nFramePosition++;
			if (m_nFramePosition == m_nFrameCount)
				m_nFramePosition = 0;
			this->Invalidate();
			long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
			if ( lPause == 0 ) lPause = 100;
			m_pManager->SetTimer( this, EVENT_TIEM_ID, lPause );
		}
		catch (...)
		{
			throw "CGifAnimUI::OnTimer";
		}
	}

	//************************************
	// Method:    DrawFrame
	// FullName:  CGifAnimUI::DrawFrame
	// Access:    private 
	// Returns:   void
	// Qualifier:
	// Parameter: HDC hDC
	// Note:	  
	//************************************
	void CGifAnimUI::DrawFrame( HDC hDC )
	{
		try
		{
			if ( NULL == hDC || NULL == m_pGifImage ) return;
			GUID pageGuid = Gdiplus::FrameDimensionTime;
			Graphics graphics( hDC );
			graphics.DrawImage( m_pGifImage, m_rcItem.left, m_rcItem.top );
			m_pGifImage->SelectActiveFrame( &pageGuid, m_nFramePosition );
		}
		catch (...)
		{
			throw "CGifAnimUI::DrawFrame";
		}
	}

}
