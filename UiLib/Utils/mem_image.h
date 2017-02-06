/**
 @file mem_image.h
 @author Kevin Lynx
 @brief to load/save images from/to memory based on GDI+
*/
#ifndef ___MEM_IMAGE_H_
#define ___MEM_IMAGE_H_

#include <comdef.h>
#include <gdiplus.h>

/**
  create a new Gdiplus::Image from a block of memory, you must *delete* the image
  manually, i.e : delete img;
*/
Gdiplus::Image *mi_from_memory( const void *buf, size_t size );

/**
  save the image to a block of memory, you must *free* the outbuf manually, i.e:
  free( outbuf );.The image format is jpeg.

  @return the outbuf pinter
*/
void *mi_to_memory( Gdiplus::Image *image, void **outbuf, size_t *size );

#endif