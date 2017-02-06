/**
 @file mem_image.cpp
 @author Kevin Lynx
 @brief to load/save images from/to memory based on GDI+
*/
#include "stdafx.h"
#include "mem_image.h"
#include <string.h>

static int GetEncoderClsid( const WCHAR *format, CLSID *pClsid )
{
	UINT num = 0, size = 0;
	Gdiplus::ImageCodecInfo *pImageCodecInfo = NULL; 
	Gdiplus::GetImageEncodersSize( &num, &size );
	if( size == 0 )
	{
		return -1;
	}
	pImageCodecInfo = (Gdiplus::ImageCodecInfo*) malloc( size );
	Gdiplus::GetImageEncoders( num, size, pImageCodecInfo );
	for( UINT i = 0; i < num; ++ i )
	{
		if( wcscmp( pImageCodecInfo[i].MimeType, format ) == 0 )
		{
			*pClsid = pImageCodecInfo[i].Clsid;
			free( pImageCodecInfo );
			return i;
		}
	}
	free( pImageCodecInfo );
	return -1;
}

static bool mem_to_global( const void *buf, size_t size, HGLOBAL global )
{
	void *dest = ::GlobalLock( global );
	if( dest == NULL )
	{
		return false;
	}
	memcpy( dest, buf, size );
	::GlobalUnlock( global );
	return true;
}

static bool stream_to_mem( IStream *stream, void **outbuf, size_t *size )
{
	ULARGE_INTEGER ulnSize;
	LARGE_INTEGER lnOffset;
	lnOffset.QuadPart = 0;
	/* get the stream size */
	if( stream->Seek( lnOffset, STREAM_SEEK_END, &ulnSize ) != S_OK )
	{
		return false;
	}
	if( stream->Seek( lnOffset, STREAM_SEEK_SET, NULL ) != S_OK )
	{
		return false;
	}

	/* read it */
	*outbuf = malloc( (size_t)ulnSize.QuadPart );
	*size = (size_t) ulnSize.QuadPart;
	ULONG bytesRead;
	if( stream->Read( *outbuf, (ULONG)ulnSize.QuadPart, &bytesRead ) != S_OK )
	{
		free( *outbuf );
		return false;
	}

	return true;
}

Gdiplus::Image *mi_from_memory( const void *buf, size_t size )
{
	IStream *stream = NULL;
	HGLOBAL global = ::GlobalAlloc( GMEM_MOVEABLE, size );
	if( global == NULL )
	{
		return NULL;
	}
	/* copy the buf content to the HGLOBAL */
	if( !mem_to_global( buf, size, global ) )
	{
		::GlobalFree( global );
		return NULL;
	}
	/* get the IStream from the global object */
	if( ::CreateStreamOnHGlobal( global, TRUE, &stream ) != S_OK )
	{
		::GlobalFree( global );
		return NULL;
	}
	/* create the image from the stream */
	Gdiplus::Image *image = Gdiplus::Image::FromStream( stream );

	stream->Release();
	/* i suppose when the reference count for stream is 0, it will 
	GlobalFree automatically. The Image maintain the object also.*/	
	return image;
}

void *mi_to_memory( Gdiplus::Image *image, void **outbuf, size_t *size )
{
	IStream *stream = NULL;
	if( ::CreateStreamOnHGlobal( NULL, TRUE, &stream ) != S_OK )
	{
		return NULL;
	}
	/* get the jpg encoder */
	::CLSID jpgClsid;
	GetEncoderClsid( L"image/jpeg", &jpgClsid );

	/* save the image to stream */
	Gdiplus::Status save_s = image->Save( stream, &jpgClsid );
	if( save_s != Gdiplus::Ok )
	{
		stream->Release();
		return NULL;
	}

	/* read the stream to buffer */
	if( !stream_to_mem( stream, outbuf, size ) )
	{
		stream->Release();
		return NULL;
	}
	return *outbuf;
}
