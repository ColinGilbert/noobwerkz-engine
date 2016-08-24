/*
% Copyright (C) 2003-2015 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            X   X  W   W  DDDD                               %
%                             X X   W   W  D   D                              %
%                              X    W   W  D   D                              %
%                             X X   W W W  D   D                              %
%                            X   X   W W   DDDD                               %
%                                                                             %
%                                                                             %
%               Read/Write X Windows System Window Dump Format.               %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
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
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/colormap.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteXWDImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s X W D                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsXWD returns True if the image format type, identified by the
%  magick string, is XWD.
%
%  The format of the IsXWD method is:
%
%      unsigned int IsXWD(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsXWD returns True if the image format type is XWD.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static unsigned int IsXWD(const unsigned char *magick,const size_t length)
{
  if (length < 8)
    return(False);
  if (memcmp(magick+1,"\000\000",2) == 0)
    {
      if (memcmp(magick+4,"\007\000\000",3) == 0)
        return(True);
      if (memcmp(magick+5,"\000\000\007",3) == 0)
        return(True);
    }
  return(False);
}

#if defined(HasX11)
#include "magick/xwindow.h"
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d X W D I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadXWDImage reads an X Window System window dump image file and
%  returns it.  It allocates the memory necessary for the new Image structure
%  and returns a pointer to the new image.
%
%  The format of the ReadXWDImage method is:
%
%      Image *ReadXWDImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadXWDImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
#define ThrowXWDReaderException(code_,reason_,image_) \
do { \
  MagickFreeMemory(comment); \
  if (ximage) \
    MagickFreeMemory(ximage->data); \
  MagickFreeMemory(ximage); \
  MagickFreeMemory(colors); \
  ThrowReaderException(code_,reason_,image_); \
} while (0);

static Image *ReadXWDImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    *comment = (char *) NULL;

  Image
    *image;

  IndexPacket
    index_val;

  int
    status;

  long
    y;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  register unsigned long
    pixel;

  size_t
    count,
    length;

  unsigned long
    lsb_first;

  XColor
    *colors = (XColor *) NULL;

  XImage
    *ximage = (XImage *) NULL;

  XWDFileHeader
    header;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowXWDReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Read in header information.

    XWDFileHeader is defined in /usr/include/X11/XWDFile.h

    All elements are 32-bit unsigned storage but non-mask properties
    in XImage use 32-bit signed values.
  */
  count=ReadBlob(image,sz_XWDheader,(char *) &header);
  if (count != sz_XWDheader)
    ThrowXWDReaderException(CorruptImageError,UnableToReadImageHeader,image);
  /*
    Ensure the header byte-order is most-significant byte first.
  */
  lsb_first=1;
  if (*(char *) &lsb_first)
    MSBOrderLong((unsigned char *) &header,sz_XWDheader);

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "XWDFileHeader:\n"
                        "    header_size      : %u\n"
                        "    file_version     : %u\n"
                        "    pixmap_format    : %s\n"
                        "    pixmap_depth     : %u\n"
                        "    pixmap_width     : %u\n"
                        "    pixmap_height    : %u\n"
                        "    xoffset          : %u\n"
                        "    byte_order       : %s\n"
                        "    bitmap_unit      : %u\n"
                        "    bitmap_bit_order : %s\n"
                        "    bitmap_pad       : %u\n"
                        "    bits_per_pixel   : %u\n"
                        "    bytes_per_line   : %u\n"
                        "    visual_class     : %s\n"
                        "    red_mask         : 0x%06X\n"
                        "    green_mask       : 0x%06X\n"
                        "    blue_mask        : 0x%06X\n"
                        "    bits_per_rgb     : %u\n"
                        "    colormap_entries : %u\n"
                        "    ncolors          : %u\n"
                        "    window_width     : %u\n"
                        "    window_height    : %u\n"
                        "    window_x         : %u\n"
                        "    window_y         : %u\n"
                        "    window_bdrwidth  : %u",
                        (unsigned int) header.header_size,
                        (unsigned int) header.file_version,
                        /* (unsigned int) header.pixmap_format, */
                        (header.pixmap_format == XYBitmap ? "XYBitmap" :
                         (header.pixmap_format == XYPixmap ? "XYPixmap" :
                          (header.pixmap_format == ZPixmap ? "ZPixmap" : "?"))),
                        (unsigned int) header.pixmap_depth,
                        (unsigned int) header.pixmap_width,
                        (unsigned int) header.pixmap_height,
                        (unsigned int) header.xoffset,
                        (header.byte_order == MSBFirst? "MSBFirst" :
                         (header.byte_order == LSBFirst ? "LSBFirst" : "?")),
                        (unsigned int) header.bitmap_unit,
                        (header.bitmap_bit_order == MSBFirst? "MSBFirst" :
                         (header.bitmap_bit_order == LSBFirst ? "LSBFirst" :
                          "?")),
                        (unsigned int) header.bitmap_pad,
                        (unsigned int) header.bits_per_pixel,
                        (unsigned int) header.bytes_per_line,
                        (header.visual_class == StaticGray ? "StaticGray" :
                         (header.visual_class == GrayScale ? "GrayScale" :
                          (header.visual_class == StaticColor ? "StaticColor" :
                           (header.visual_class == PseudoColor ? "PseudoColor" :
                            (header.visual_class == TrueColor ? "TrueColor" :
                             (header.visual_class == DirectColor ?
                              "DirectColor" : "?")))))),
                        (unsigned int) header.red_mask,
                        (unsigned int) header.green_mask,
                        (unsigned int) header.blue_mask,
                        (unsigned int) header.bits_per_rgb,
                        (unsigned int) header.colormap_entries,
                        (unsigned int) header.ncolors,
                        (unsigned int) header.window_width,
                        (unsigned int) header.window_height,
                        (unsigned int) header.window_x,
                        (unsigned int) header.window_y,
                        (unsigned int) header.window_bdrwidth
                        );

  /*
    Check to see if the dump file is in the proper format.
  */
  if (header.file_version != XWD_FILE_VERSION)
    ThrowXWDReaderException(CorruptImageError,InvalidFileFormatVersion,image);
  if (header.header_size < sz_XWDheader)
    ThrowXWDReaderException(CorruptImageError,CorruptImage,image);
  switch (header.visual_class)
    {
    case StaticGray:
    case GrayScale:
    case StaticColor:
    case PseudoColor:
    case TrueColor:
    case DirectColor:
      break;
    default:
      {
        ThrowXWDReaderException(CorruptImageError,CorruptImage,image);
      }
    }
  switch (header.pixmap_format)
    {
    case XYBitmap:
    case XYPixmap:
    case ZPixmap:
      break;
    default:
      {
        ThrowXWDReaderException(CorruptImageError,CorruptImage,image);
      }
    }

  /*
    Retrieve comment (if any)
  */
  length=header.header_size-sz_XWDheader;
  if (length > ((~0UL)/sizeof(*comment)))
    ThrowXWDReaderException(CorruptImageError,ImproperImageHeader,image);
  comment=MagickAllocateMemory(char *,length+1);
  if (comment == (char *) NULL)
    ThrowXWDReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  count=ReadBlob(image,length,comment);
  if (count != length)
    ThrowXWDReaderException(CorruptImageError,UnableToReadWindowNameFromDumpFile,
                            image);
  comment[length]='\0';
  (void) SetImageAttribute(image,"comment",comment);
  MagickFreeMemory(comment);

  /*
    Initialize the X image.
  */
  ximage=MagickAllocateMemory(XImage *,sizeof(XImage));
  if (ximage == (XImage *) NULL)
    ThrowXWDReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  ximage->depth=(int) header.pixmap_depth;
  ximage->format=(int) header.pixmap_format;
  ximage->xoffset=(int) header.xoffset;
  ximage->data=(char *) NULL;
  ximage->width=(int) header.pixmap_width;
  ximage->height=(int) header.pixmap_height;
  ximage->bitmap_pad=(int) header.bitmap_pad;
  ximage->bytes_per_line=(int) header.bytes_per_line;
  ximage->byte_order=(int) header.byte_order;
  ximage->bitmap_unit=(int) header.bitmap_unit;
  ximage->bitmap_bit_order=(int) header.bitmap_bit_order;
  ximage->bits_per_pixel=(int) header.bits_per_pixel;
  ximage->red_mask=header.red_mask;
  ximage->green_mask=header.green_mask;
  ximage->blue_mask=header.blue_mask;
  /*
    XImage uses signed integers rather than unsigned.  Check for
    overflow due to assignment.
  */
  if (ximage->width < 0 ||
      ximage->height < 0 ||
      ximage->format < 0 ||
      ximage->byte_order < 0 ||
      ximage->bitmap_unit < 0 ||
      ximage->bitmap_bit_order < 0 ||
      ximage->bitmap_pad < 0 ||
      ximage->depth < 0 ||
      ximage->bytes_per_line < 0 ||
      ximage->bits_per_pixel < 0)
    ThrowXWDReaderException(CorruptImageError,ImproperImageHeader,image);
  /* Guard against buffer overflow in libX11. */
  if (ximage->bits_per_pixel > 32 || ximage->bitmap_unit > 32)
    ThrowXWDReaderException(CorruptImageError,ImproperImageHeader,image);
  status=XInitImage(ximage);
  if (status == False)
    ThrowXWDReaderException(CorruptImageError,UnrecognizedXWDHeader,image);
  image->columns=ximage->width;
  image->rows=ximage->height;
  if (!image_info->ping)
    if (CheckImagePixelLimits(image, exception) != MagickPass)
      ThrowXWDReaderException(ResourceLimitError,ImagePixelLimitExceeded,image);
  image->depth=8;
  if ((header.ncolors == 0U) ||
      ((ximage->red_mask != 0) ||
       (ximage->green_mask != 0) ||
       (ximage->blue_mask != 0)))
    {
      image->storage_class=DirectClass;
      if (!image_info->ping)
        if ((ximage->red_mask == 0) ||
            (ximage->green_mask == 0) ||
            (ximage->blue_mask == 0))
          ThrowXWDReaderException(CorruptImageError,ImproperImageHeader,image);
    }
  else
    {
      image->storage_class=PseudoClass;
    }
  image->colors=header.ncolors;
  if (!image_info->ping)
    {
      /*
        Read colormap.
      */
      colors=(XColor *) NULL;
      if (header.ncolors != 0)
        {
          XWDColor
            color;

          register long
            i;

          length=(size_t) header.ncolors;
          if (length > ((~0UL)/sizeof(*colors)))
            ThrowXWDReaderException(CorruptImageError,ImproperImageHeader,image);
          colors=MagickAllocateArray(XColor *,length,sizeof(XColor));
          if (colors == (XColor *) NULL)
            ThrowXWDReaderException(ResourceLimitError,MemoryAllocationFailed,
                                    image);
          for (i=0; i < (long) header.ncolors; i++)
            {
              count=ReadBlob(image,sz_XWDColor,(char *) &color);
              if (count != sz_XWDColor)
                ThrowXWDReaderException(CorruptImageError,
                                        UnableToReadColormapFromDumpFile,image);
              colors[i].pixel=color.pixel;
              colors[i].red=color.red;
              colors[i].green=color.green;
              colors[i].blue=color.blue;
              colors[i].flags=color.flags;
            }
          /*
            Ensure the header byte-order is most-significant byte first.
          */
          lsb_first=1;
          if (*(char *) &lsb_first)
            for (i=0; i < (long) header.ncolors; i++)
              {
                MSBOrderLong((unsigned char *) &colors[i].pixel,
                             sizeof(unsigned long));
                MSBOrderShort((unsigned char *) &colors[i].red,
                              3*sizeof(unsigned short));
              }
        }
      /*
        Convert image to MIFF format.
      */
      /*
        Allocate the pixel buffer.
      */
#define XWD_OVERFLOW(c,a,b) ((b) != 0 && ((c)/((size_t) b) != ((size_t) a)))
      length=ximage->bytes_per_line*ximage->height;
      if (XWD_OVERFLOW(length,ximage->bytes_per_line,ximage->height))
        ThrowXWDReaderException(ResourceLimitError,MemoryAllocationFailed,image);
      if (ximage->format != ZPixmap)
        {
          size_t tmp=length;
          length*=ximage->depth;
          if (XWD_OVERFLOW(length,tmp,ximage->depth))
            ThrowXWDReaderException(ResourceLimitError,MemoryAllocationFailed,
                                    image);
        }
      ximage->data=MagickAllocateMemory(char *,length);
      if (ximage->data == (char *) NULL)
        ThrowXWDReaderException(ResourceLimitError,MemoryAllocationFailed,image);
      count=ReadBlob(image,length,ximage->data);
      if (count != length)
        ThrowXWDReaderException(CorruptImageError,
                                UnableToReadPixmapFromDumpFile,image);
      switch (image->storage_class)
        {
        case DirectClass:
        default:
          {
            register unsigned long
              color;

            unsigned long
              blue_mask,
              blue_shift,
              green_mask,
              green_shift,
              red_mask,
              red_shift;

            /*
              Determine shift and mask for red, green, and blue.
            */
            red_mask=ximage->red_mask;
            red_shift=0;
            while ((red_mask != 0) && ((red_mask & 0x01) == 0))
              {
                red_mask>>=1;
                red_shift++;
              }
            green_mask=ximage->green_mask;
            green_shift=0;
            while ((green_mask != 0) && ((green_mask & 0x01) == 0))
              {
                green_mask>>=1;
                green_shift++;
              }
            blue_mask=ximage->blue_mask;
            blue_shift=0;
            while ((blue_mask != 0) && ((blue_mask & 0x01) == 0))
              {
                blue_mask>>=1;
                blue_shift++;
              }

            /*
              Convert X image to DirectClass packets.
            */
            if (image->colors != 0)
              {
                for (y=0; y < (long) image->rows; y++)
                  {
                    q=SetImagePixels(image,0,y,image->columns,1);
                    if (q == (PixelPacket *) NULL)
                      break;
                    for (x=0; x < (long) image->columns; x++)
                      {
                        pixel=XGetPixel(ximage,(int) x,(int) y);
                        index_val=(unsigned short)
                          ((pixel >> red_shift) & red_mask);
                        q->red=ScaleShortToQuantum(colors[index_val].red);
                        index_val=(unsigned short)
                          ((pixel >> green_shift) & green_mask);
                        q->green=ScaleShortToQuantum(colors[index_val].green);
                        index_val=(unsigned short)
                          ((pixel >> blue_shift) & blue_mask);
                        q->blue=ScaleShortToQuantum(colors[index_val].blue);
                        q++;
                      }
                    if (!SyncImagePixels(image))
                      break;
                    if (QuantumTick(y,image->rows))
                      if (!MagickMonitorFormatted(y,image->rows,exception,
                                                  LoadImageText,image->filename,
                                                  image->columns,image->rows))
                        break;
                  }
              }
            else
              {
                if ((red_mask == 0) ||
                    (green_mask == 0) ||
                    (blue_mask == 0))
                  ThrowXWDReaderException(CorruptImageError,ImproperImageHeader,image);
                for (y=0; y < (long) image->rows; y++)
                  {
                    q=SetImagePixels(image,0,y,image->columns,1);
                    if (q == (PixelPacket *) NULL)
                      break;
                    for (x=0; x < (long) image->columns; x++)
                      {
                        pixel=XGetPixel(ximage,(int) x,(int) y);
                        color=(pixel >> red_shift) & red_mask;
                        q->red=ScaleShortToQuantum((color*65535L)/red_mask);
                        color=(pixel >> green_shift) & green_mask;
                        q->green=ScaleShortToQuantum((color*65535L)/green_mask);
                        color=(pixel >> blue_shift) & blue_mask;
                        q->blue=ScaleShortToQuantum((color*65535L)/blue_mask);
                        q++;
                      }
                    if (!SyncImagePixels(image))
                      break;
                    if (QuantumTick(y,image->rows))
                      if (!MagickMonitorFormatted(y,image->rows,exception,
                                                  LoadImageText,image->filename,
                                                  image->columns,image->rows))
                        break;
                  }
              }
            break;
          }
        case PseudoClass:
          {
            /*
              Convert X image to PseudoClass packets.
            */
            register long
              i;

            if (!AllocateImageColormap(image,image->colors))
              ThrowXWDReaderException(ResourceLimitError,MemoryAllocationFailed,
                                      image);
            for (i=0; i < (long) image->colors; i++)
              {
                image->colormap[i].red=ScaleShortToQuantum(colors[i].red);
                image->colormap[i].green=ScaleShortToQuantum(colors[i].green);
                image->colormap[i].blue=ScaleShortToQuantum(colors[i].blue);
              }
            for (y=0; y < (long) image->rows; y++)
              {
                q=SetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  break;
                indexes=AccessMutableIndexes(image);
                for (x=0; x < (long) image->columns; x++)
                  {
                    index_val=(IndexPacket) (XGetPixel(ximage,(int) x,(int) y));
                    VerifyColormapIndex(image,index_val);
                    indexes[x]=index_val;
                    *q++=image->colormap[index_val];
                  }
                if (!SyncImagePixels(image))
                  break;
                if (QuantumTick(y,image->rows))
                  if (!MagickMonitorFormatted(y,image->rows,exception,
                                              LoadImageText,
                                              image->filename,
                                              image->columns,image->rows))
                    break;
              }
            break;
          }
        }
    }
  /*
    Free image and colormap.
  */
  MagickFreeMemory(colors);
  MagickFreeMemory(ximage->data);
  MagickFreeMemory(ximage);
  CloseBlob(image);
  return(image);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r X W D I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterXWDImage adds attributes for the XWD image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterXWDImage method is:
%
%      RegisterXWDImage(void)
%
*/
ModuleExport void RegisterXWDImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("XWD");
#if defined(HasX11)
  entry->decoder=(DecoderHandler) ReadXWDImage;
  entry->encoder=(EncoderHandler) WriteXWDImage;
#endif
  entry->magick=(MagickHandler) IsXWD;
  entry->adjoin=False;
  entry->coder_class=UnstableCoderClass;
  entry->description="X Windows system window dump (color)";
  entry->module="XWD";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r X W D I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterXWDImage removes format registrations made by the
%  XWD module from the list of supported formats.
%
%  The format of the UnregisterXWDImage method is:
%
%      UnregisterXWDImage(void)
%
*/
ModuleExport void UnregisterXWDImage(void)
{
  (void) UnregisterMagickInfo("XWD");
}

#if defined(HasX11)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e X W D I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteXWDImage writes an image to a file in X window dump
%  rasterfile format.
%
%  The format of the WriteXWDImage method is:
%
%      unsigned int WriteXWDImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteXWDImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static unsigned int WriteXWDImage(const ImageInfo *image_info,Image *image)
{
  long
    y;

  register const PixelPacket
    *p;

  register long
    x;

  register long
    i;

  register unsigned char
    *q;

  unsigned char
    *pixels;

  size_t
    pixels_size;

  unsigned int
    status;

  unsigned long
    bits_per_pixel,
    bytes_per_line,
    lsb_first,
    scanline_pad;

  XWDFileHeader
    xwd_info;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  (void) TransformColorspace(image,RGBColorspace);
  /*
    XWD does not support more than 256 colors.
  */
  if ((image->storage_class == PseudoClass) && (image->colors > 256))
    SetImageType(image,TrueColorType);
  /*
    Initialize XWD file header.
  */
  (void) memset(&xwd_info,0,sizeof(xwd_info));
  xwd_info.header_size=(CARD32) (sz_XWDheader+strlen(image->filename)+1);
  xwd_info.file_version=(CARD32) XWD_FILE_VERSION;
  xwd_info.pixmap_format=(CARD32) ZPixmap;
  xwd_info.pixmap_depth=(CARD32) (image->storage_class == DirectClass ? 24 : 8);
  xwd_info.pixmap_width=(CARD32) image->columns;
  xwd_info.pixmap_height=(CARD32) image->rows;
  xwd_info.xoffset=(CARD32) 0;
  xwd_info.byte_order=(CARD32) MSBFirst;
  xwd_info.bitmap_unit=(CARD32) (image->storage_class == DirectClass ? 32 : 8);
  xwd_info.bitmap_bit_order=(CARD32) MSBFirst;
  xwd_info.bitmap_pad=(CARD32) (image->storage_class == DirectClass ? 32 : 8);
  bits_per_pixel=(image->storage_class == DirectClass ? 24 : 8);
  xwd_info.bits_per_pixel=(CARD32) bits_per_pixel;
  bytes_per_line=(CARD32) ((((xwd_info.bits_per_pixel*
    xwd_info.pixmap_width)+((xwd_info.bitmap_pad)-1))/
    (xwd_info.bitmap_pad))*((xwd_info.bitmap_pad) >> 3));
  xwd_info.bytes_per_line=(CARD32) bytes_per_line;
  xwd_info.visual_class=(CARD32)
    (image->storage_class == DirectClass ? DirectColor : PseudoColor);
  xwd_info.red_mask=(CARD32)
    (image->storage_class == DirectClass ? 0xff0000 : 0);
  xwd_info.green_mask=(CARD32)
    (image->storage_class == DirectClass ? 0xff00 : 0);
  xwd_info.blue_mask=(CARD32) (image->storage_class == DirectClass ? 0xff : 0);
  xwd_info.bits_per_rgb=(CARD32) (image->storage_class == DirectClass ? 24 : 8);
  xwd_info.colormap_entries=(CARD32)
    (image->storage_class == DirectClass ? 256 : image->colors);
  xwd_info.ncolors=(unsigned int)
    (image->storage_class == DirectClass ? 0 : image->colors);
  xwd_info.window_width=(CARD32) image->columns;
  xwd_info.window_height=(CARD32) image->rows;
  xwd_info.window_x=0;
  xwd_info.window_y=0;
  xwd_info.window_bdrwidth=(CARD32) 0;
  /*
    Write XWD header.
  */
  lsb_first=1;
  if (*(char *) &lsb_first)
    MSBOrderLong((unsigned char *) &xwd_info,sizeof(xwd_info));
  (void) WriteBlob(image,sz_XWDheader,(char *) &xwd_info);
  (void) WriteBlob(image,strlen(image->filename)+1,(char *) image->filename);
  if (image->storage_class == PseudoClass)
    {
      XColor
        *colors;

      XWDColor
        color;

      /*
        Dump colormap to file.
      */
      (void) memset(&color,0,sizeof(color));
      colors=MagickAllocateArray(XColor *,image->colors,sizeof(XColor));
      if (colors == (XColor *) NULL)
        ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
      for (i=0; i < (long) image->colors; i++)
      {
        colors[i].pixel=i;
        colors[i].red=ScaleQuantumToShort(image->colormap[i].red);
        colors[i].green=ScaleQuantumToShort(image->colormap[i].green);
        colors[i].blue=ScaleQuantumToShort(image->colormap[i].blue);
        colors[i].flags=DoRed | DoGreen | DoBlue;
        colors[i].pad=0;
        if (*(char *) &lsb_first)
          {
            MSBOrderLong((unsigned char *) &colors[i].pixel,sizeof(long));
            MSBOrderShort((unsigned char *) &colors[i].red,3*sizeof(short));
          }
      }
      for (i=0; i < (long) image->colors; i++)
      {
        color.pixel=(CARD32) colors[i].pixel;
        color.red=colors[i].red;
        color.green=colors[i].green;
        color.blue=colors[i].blue;
        color.flags=colors[i].flags;
        (void) WriteBlob(image,sz_XWDColor,(char *) &color);
      }
      MagickFreeMemory(colors);
    }
  /*
    Allocate memory for pixels.
  */
  scanline_pad=(bytes_per_line-((image->columns*bits_per_pixel) >> 3));
  pixels_size=image->columns*(image->storage_class == PseudoClass ? 1 : 3)+scanline_pad;
  pixels=MagickAllocateMemory(unsigned char *,pixels_size);
  if (pixels == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  (void) memset(pixels,0,pixels_size);
  /*
    Convert MIFF to XWD raster pixels.
  */
  for (y=0; y < (long) image->rows; y++)
  {
    p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
    if (p == (const PixelPacket *) NULL)
      break;
    q=pixels;

    if (image->storage_class == PseudoClass)
      {
        register const IndexPacket
          *indexes;

        indexes=AccessImmutableIndexes(image);
        for (x=0; x < (long) image->columns; x++)
          *q++=(unsigned char) indexes[x];
      }
    else
      {
        for (x=(long) image->columns; x > 0; x--)
          {
            
            *q++=ScaleQuantumToChar(p->red);
            *q++=ScaleQuantumToChar(p->green);
            *q++=ScaleQuantumToChar(p->blue);
            p++;
          }
      }
    for (x=(long) scanline_pad; x > 0; x--)
      *q++=0;
    (void) WriteBlob(image,(size_t) (q-pixels),(char *) pixels);
    if (image->previous == (Image *) NULL)
      if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows,&image->exception,
                                    SaveImageText,image->filename,
				    image->columns,image->rows))
          break;
  }
  MagickFreeMemory(pixels);
  CloseBlob(image);
  return(True);
}
#endif
