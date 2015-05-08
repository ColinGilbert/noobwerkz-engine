/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%                        Read/Write Windows Clipboard.                        %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                             Leonard Rosenthol                               %
%                                 May 2002                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/

#include "magick/studio.h"
#include "magick/nt_feature.h"
#include "magick/blob.h"
#include "magick/pixel_cache.h"
#include "magick/magick.h"
#include "magick/utility.h"
#if defined(HasWINGDI32)
#  if defined(__CYGWIN__)
#    include <windows.h>
#  else
     /* All MinGW needs ... */
#    include <wingdi.h>
#  endif
#endif


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d C L I P B O A R D I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadCLIPBOARDImage reads an image from the system clipboard and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadCLIPBOARDImage method is:
%
%      Image *ReadCLIPBOARDImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadCLIPBOARDImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
#if defined(HasWINGDI32)
static Image *ReadCLIPBOARDImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  Image
    *image;

  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);

  {
    HBITMAP
      bitmapH;

    OpenClipboard( NULL );
    bitmapH = GetClipboardData( CF_BITMAP );
    CloseClipboard();

    if ( bitmapH != NULL )
      {
        BITMAPINFO
          DIBinfo;

        BITMAP
          bitmap;

        HBITMAP
          hBitmap,
          hOldBitmap;

        HDC
          hDC,
          hMemDC;

        RGBQUAD
          *pBits,
          *ppBits;

        /* create an offscreen DC for the source */
        hMemDC = CreateCompatibleDC( NULL );
        hOldBitmap = SelectObject( hMemDC, bitmapH );
        GetObject( bitmapH, sizeof( BITMAP ), (LPSTR) &bitmap );
        if ((image->columns == 0) || (image->rows == 0))
          {
            image->rows=bitmap.bmHeight;
            image->columns=bitmap.bmWidth;
          }

        /*
          Initialize the bitmap header info.
          Create a temp DC for the DIB to go into
        */
        (void) memset(&DIBinfo,0,sizeof(BITMAPINFO));
        DIBinfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
        DIBinfo.bmiHeader.biWidth=image->columns;
        DIBinfo.bmiHeader.biHeight=(-1)*image->rows;
        DIBinfo.bmiHeader.biPlanes=1;
        DIBinfo.bmiHeader.biBitCount=32;
        DIBinfo.bmiHeader.biCompression=BI_RGB;
        hDC=GetDC(NULL);
        if (!hDC)
          ThrowReaderException(CoderError,UnableToCreateADC,image);
        hBitmap= CreateDIBSection(hDC,&DIBinfo,DIB_RGB_COLORS,(void **) &ppBits,NULL,0);
        ReleaseDC(NULL,hDC);
        if (!hBitmap)
          ThrowReaderException(CoderError,UnableToCreateBitmap,image);

        /* create an offscreen DC */
        hDC=CreateCompatibleDC(NULL);
        if (!hDC)
          {
            DeleteObject(hBitmap);
            ThrowReaderException(CoderError,UnableToCreateADC,image);
          }
        hOldBitmap=(HBITMAP) SelectObject(hDC,hBitmap);
        if (!hOldBitmap)
          {
            DeleteDC(hDC);
            DeleteObject(hBitmap);
            ThrowReaderException(CoderError,UnableToCreateBitmap,image);
          }

        /* bitblt from the memory to the DIB-based one */
        BitBlt( hDC, 0, 0, image->columns, image->rows, hMemDC, 0, 0, SRCCOPY );

        /* finally copy the pixels! */
        pBits=ppBits;
        for (y=0; y < (long) image->rows; y++)
          {
            q=SetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) image->columns; x++)
              {
                q->red    = ScaleCharToQuantum(pBits->rgbRed);
                q->green  = ScaleCharToQuantum(pBits->rgbGreen);
                q->blue    = ScaleCharToQuantum(pBits->rgbBlue);
                q->opacity  = OpaqueOpacity;
                pBits++;
                q++;
              }
            if (!SyncImagePixels(image))
              break;
          }
      } else
        ThrowReaderException(CoderError,NoBitmapOnClipboard,image);
  }

  CloseBlob(image);
  return(image);
}
#endif /* HasWINGDI32 */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e C L I P B O A R D I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteCLIPBOARDImage writes an image to the system clipboard.
%
%  The format of the WriteCLIPBOARDImage method is:
%
%      unsigned int WriteCLIPBOARDImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteCLIPBOARDImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
#if defined(HasWINGDI32)
static unsigned int WriteCLIPBOARDImage(const ImageInfo *image_info,Image *image)
{
  /*
    Allocate memory for pixels.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  {
    HBITMAP
      bitmapH;

    OpenClipboard( NULL );
    EmptyClipboard();

  bitmapH = ImageToHBITMAP(image);
  SetClipboardData(CF_BITMAP, bitmapH);

  CloseClipboard();

    return(True);
  }

  return(False);
}
#endif /* HasWINGDI32 */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r C L I P B O A R D I m a g e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterCLIPBOARDImage adds attributes for the clipboard "image format" to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterCLIPBOARDImage method is:
%
%      RegisterCLIPBOARDImage(void)
%
*/
ModuleExport void RegisterCLIPBOARDImage(void)
{
#if defined(HasWINGDI32)
  MagickInfo
    *entry;

  entry=SetMagickInfo("CLIPBOARD");
  entry->decoder=(DecoderHandler) ReadCLIPBOARDImage;
  entry->encoder=(EncoderHandler) WriteCLIPBOARDImage;
  entry->adjoin = False;
  entry->description="Windows System Clipboard";
  entry->module="CLIPBOARD";
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r C L I P B O A R D I m a g e                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterCLIPBOARDImage removes format registrations made by the
%  RGB module from the list of supported formats.
%
%  The format of the UnregisterCLIPBOARDImage method is:
%
%      UnregisterCLIPBOARDImage(void)
%
*/
ModuleExport void UnregisterCLIPBOARDImage(void)
{
#if defined(HasWINGDI32)
  (void) UnregisterMagickInfo("CLIPBOARD");
#endif
}

