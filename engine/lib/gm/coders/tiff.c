/*
% Copyright (C) 2003 - 2015 GraphicsMagick Group
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
%                        TTTTT  IIIII  FFFFF  FFFFF                           %
%                          T      I    F      F                               %
%                          T      I    FFF    FFF                             %
%                          T      I    F      F                               %
%                          T    IIIII  F      F                               %
%                                                                             %
%                                                                             %
%                       Read/Write TIFF Image Format.                         %
%                                                                             %
%                                                                             %
%                        Original Software Design                             %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                     Re-Written For GraphicsMagick                           %
%                             Bob Friesenhahn                                 %
%                                2002-2009                                    %
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
#include "magick/analyze.h"
#include "magick/attribute.h"
#include "magick/bit_stream.h"
#include "magick/blob.h"
#include "magick/colormap.h"
#include "magick/constitute.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/profile.h"
#include "magick/quantize.h"
#include "magick/resize.h"
#include "magick/tempfile.h"
#include "magick/tsd.h"
#include "magick/utility.h"
#include "magick/version.h"
#if defined(HasTIFF)
#  if defined(HAVE_TIFFCONF_H)
#    include "tiffconf.h"
#  endif
#  include "tiff.h"
#  include "tiffio.h"
#  if !defined(COMPRESSION_ADOBE_DEFLATE)
#    define COMPRESSION_ADOBE_DEFLATE  8
#  endif  /* !defined(COMPRESSION_ADOBE_DEFLATE) */
#if defined(TIFF_VERSION_BIG)
#  define HasBigTIFF 1
#endif /* defined(TIFF_BIGTIFF_VERSION) */

/*
  Set to 1 in order to log low-level BLOB I/O at "coder" level.
*/
#if !defined(LOG_TIFF_BLOB_IO)
#  define LOG_TIFF_BLOB_IO 0
#endif /* !defined(LOG_TIFF_BLOB_IO) */

#if !defined(PREDICTOR_NONE)
#define     PREDICTOR_NONE              1
#endif
#if !defined(PREDICTOR_HORIZONTAL)
#define     PREDICTOR_HORIZONTAL        2
#endif
#if !defined(PREDICTOR_FLOATINGPOINT)
#define     PREDICTOR_FLOATINGPOINT     3
#endif
#if !defined(SAMPLEFORMAT_COMPLEXINT)
#define     SAMPLEFORMAT_COMPLEXINT     5
#endif
#if !defined(SAMPLEFORMAT_COMPLEXIEEEFP)
#define     SAMPLEFORMAT_COMPLEXIEEEFP  6
#endif

#if !defined(TIFFTAG_COPYRIGHT)
#define     TIFFTAG_COPYRIGHT 33432
#endif
#if !defined(TIFFTAG_OPIIMAGEID)
#define     TIFFTAG_OPIIMAGEID 32781
#endif

/*
  Global declarations.
*/
static MagickTsdKey_t tsd_key = (MagickTsdKey_t) 0;

/* static ExceptionInfo */
/*   *tiff_exception; */

typedef struct _Magick_TIFF_ClientData
{
  Image
    *image;

  const ImageInfo
    *image_info;
} Magick_TIFF_ClientData;

/*
  Forward declarations.
*/
static tsize_t
  TIFFReadBlob(thandle_t,tdata_t,tsize_t);

static MagickPassFail
  WriteGROUP4RAWImage(const ImageInfo *image_info,Image *image),
  WritePTIFImage(const ImageInfo *,Image *),
  WriteTIFFImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s T I F F                                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsTIFF returns True if the image format type, identified by the
%  magick string, is TIFF.
%
%  The format of the IsTIFF method is:
%
%      MagickBool IsTIFF(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsTIFF returns MagickTrue if the image format type
%               is TIFF.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static MagickBool
IsTIFF(const unsigned char *magick,const size_t length)
{
  if (length < 8)
    return(False);

  /* Big endian Classic TIFF*/
  if (memcmp(magick,"\115\115\000\052",4) == 0)
    return(True);

  /* Little endian Classic TIFF */
  if (memcmp(magick,"\111\111\052\000",4) == 0)
    return(True);

#if defined(HasBigTIFF)
  /*
   * From http://www.remotesensing.org/libtiff/bigtiffdesign.html
   *
   * * The Version ID, in header bytes 2-3, formerly decimal 42, now changes to 43
   * * Header bytes 4-5 contain the decimal number 8.
   *   - If there is some other number here, a reader should give up.
   *   - This is to provide a nice way to move to 16-byte pointers some day.
   *
   * * Header bytes 6-7 are reserved and must be zero.
   *    - If they're not, a reader should give up.
   *
   * Also http://www.awaresystems.be/imaging/tiff/bigtiff.html
   */

  /* Big endian BigTIFF*/
  if (memcmp(magick,"\115\115\000\053\000\010\000\000",8) == 0)
    return(True);

  /* Little endian BigTIFF */
  if (memcmp(magick,"\111\111\053\000\010\000\000\000",8) == 0)
    return(True);
#endif

  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d T I F F I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadTIFFImage reads a Tagged image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadTIFFImage method is:
%
%      Image *ReadTIFFImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadTIFFImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
#if defined(HAVE_TIFFMERGEFIELDINFO) && defined(HAVE_TIFFSETTAGEXTENDER)
#  define EXTEND_TIFF_TAGS 1
#  if !defined(TIFFTAG_EXIFIFD)
#    define TIFFTAG_EXIFIFD 34665
#  endif
#  if TIFFLIB_VERSION < 20050704
/* It seems that support was added for the EXIFIFDOffset tag in
   libtiff release 3-7-3 which corresponds with TIFFLIB_VERSION
   20050704 */
static const TIFFFieldInfo
  ExtensionTiffFieldInfo[] =
  {
    {
      TIFFTAG_EXIFIFD, -1, -1, TIFF_LONG, FIELD_CUSTOM,
      MagickFalse, MagickTrue, "EXIFIFDOffset"
    }
  };
#  endif

/*
  Ignore specific tags
*/
static void
TIFFIgnoreTags(TIFF *tiff)
{
  char
    *q;

  const char
    *p,
    *tags;

  const ImageInfo
    *image_info;

  register ssize_t
    i;

  size_t
    count;

  TIFFFieldInfo
    *ignore;

  if (TIFFGetReadProc(tiff) != TIFFReadBlob)
    return;

  image_info=((Magick_TIFF_ClientData *)TIFFClientdata(tiff))->image_info;
  tags=AccessDefinition(image_info,"tiff","ignore-tags");
  if (tags == (const char *) NULL)
    return;
  count=0;
  p=tags;
  while (*p != '\0')
    {
      while ((isspace((int) ((unsigned char) *p)) != 0))
        p++;

      {
        /* Avoid warning about unused strtol return value on Linux */
        long ignored = strtol(p,&q,10);
        (void) ignored;
      }
      if (p == q)
        return;

      p=q;
      count++;

      while ((isspace((int) ((unsigned char) *p)) != 0) || (*p == ','))
        p++;
    }
  if (count == 0)
    return;
  i=0;
  p=tags;
  ignore=MagickAllocateArray(TIFFFieldInfo*,count,sizeof(*ignore));
  /* This also sets field_bit to 0 (FIELD_IGNORE) */
  (void) memset(ignore,0,count*sizeof(*ignore));
  while (*p != '\0')
    {
      while ((isspace((int) ((unsigned char) *p)) != 0))
        p++;

      ignore[i].field_tag=(ttag_t) strtol(p,&q,10);

      p=q;
      i++;

      while ((isspace((int) ((unsigned char) *p)) != 0) || (*p == ','))
        p++;
    }
  (void) TIFFMergeFieldInfo(tiff,ignore,(uint32) count);
  MagickFreeMemory(ignore);
}

/*
  Merge in our new fields and then call the next extender if there is
  one in effect.
*/
static TIFFExtendProc _ParentExtender = NULL;
static void
ExtensionTagsDefaultDirectory(TIFF *tif)
{
#  if TIFFLIB_VERSION < 20050704
  /* Install the extended Tag field info */
  TIFFMergeFieldInfo(tif, ExtensionTiffFieldInfo,
                     sizeof(ExtensionTiffFieldInfo)/
                     sizeof(ExtensionTiffFieldInfo[0]));
#  endif

  /* Since an XTIFF client module may have overridden
   * the default directory method, we call it now to
   * allow it to set up the rest of its own methods.
   */
  if (_ParentExtender) 
    (*_ParentExtender)(tif);
  TIFFIgnoreTags(tif);
}

/*
  Obtain the current handler at the front of the chain, and register
  ourselves as the new first handler.
*/
static
void ExtensionTagsInitialize(void)
{
  static int
    first_time=1;
	
  if (! first_time) return; /* Been there. Done that. */
  first_time = 0;

  /* Grab the inherited method and install */
  _ParentExtender = TIFFSetTagExtender(ExtensionTagsDefaultDirectory);
}

#endif /* defined(HAVE_TIFFMERGEFIELDINFO) && defined(HAVE_TIFFSETTAGEXTENDER) */

/*
  Return MagickTrue if libtiff supports the indicated compression type.
  Sets buffer pointed to by 'compression_name' to the name of the compression.
*/
static MagickBool
CompressionSupported(const CompressionType compression,
		     char *compression_name)
{
  uint16
    compress_tag;

  MagickBool
    status;

  status = MagickFalse;
  compress_tag=COMPRESSION_NONE;
  strlcpy(compression_name,"Undefined",MaxTextExtent);

  /*
    This switch statement should match all the values of CompressionType.
  */
  switch (compression)
    {
    case UndefinedCompression:
      {
        strlcpy(compression_name,"Undefined",MaxTextExtent);
        break;
      }
    case NoCompression:
      {
        strlcpy(compression_name,"No",MaxTextExtent);
        compress_tag=COMPRESSION_NONE;
        status=MagickTrue;
        break;
      }
    case BZipCompression:
      {
        strlcpy(compression_name,"BZip",MaxTextExtent);
        break;
      }
    case FaxCompression:
      {
        strlcpy(compression_name,"Group3 FAX",MaxTextExtent);
#if defined(COMPRESSION_CCITTFAX3)
        compress_tag=COMPRESSION_CCITTFAX3;
        status=MagickTrue;
#endif
        break;
      }
    case Group4Compression:
      {
        strlcpy(compression_name,"Group4 FAX",MaxTextExtent);
#if defined(COMPRESSION_CCITTFAX4)
        compress_tag=COMPRESSION_CCITTFAX4; 
        status=MagickTrue;
#endif
        break;
      }
    case JBIG1Compression:
      {
	strlcpy(compression_name,"JBIG",MaxTextExtent);
#if defined(COMPRESSION_JBIG)
        compress_tag=COMPRESSION_JBIG;
        status=MagickTrue;
#endif
	break;
      }
    case JBIG2Compression:
      {
	strlcpy(compression_name,"JBIG2",MaxTextExtent);
	break;
      }
    case JPEGCompression:
      {
        strlcpy(compression_name,"JPEG",MaxTextExtent);
#if defined(COMPRESSION_JPEG)
        compress_tag=COMPRESSION_JPEG;
        status=MagickTrue;
#endif
        break;
      }
    case JPEG2000Compression:
      {
        strlcpy(compression_name,"JPEG2000",MaxTextExtent);
        break;
      }
    case LosslessJPEGCompression:
      {
        strlcpy(compression_name,"Lossless JPEG",MaxTextExtent);
        break;
      }
    case LZMACompression:
      {
        strlcpy(compression_name,"LZMA",MaxTextExtent);
#if defined(COMPRESSION_LZMA)
        compress_tag=COMPRESSION_LZMA;
        status=MagickTrue;
#endif
        break;
      }
    case LZWCompression:
      {
        strlcpy(compression_name,"LZW",MaxTextExtent);
#if defined(COMPRESSION_LZW)
        compress_tag=COMPRESSION_LZW;
        status=MagickTrue;
#endif
        break;
      }
    case RLECompression:
      {
        strlcpy(compression_name,"Macintosh RLE (Packbits)",MaxTextExtent);
#if defined(COMPRESSION_PACKBITS)
        compress_tag=COMPRESSION_PACKBITS;
        status=MagickTrue;
#endif
        break;
      }
    case ZipCompression:
      {
        strlcpy(compression_name,"Adobe Deflate",MaxTextExtent);
#if defined(COMPRESSION_ADOBE_DEFLATE)
        compress_tag=COMPRESSION_ADOBE_DEFLATE;
        status=MagickTrue;
#endif
        break;
      }
    }

  if (MagickTrue == status)
    {
#if defined(HAVE_TIFFISCODECCONFIGURED) || (TIFFLIB_VERSION > 20040919)
      if (compress_tag != COMPRESSION_NONE)
        {
          if (TIFFIsCODECConfigured(compress_tag))
	    status = MagickTrue;
	  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"TIFFIsCODECConfigured says support for %s "
				"compression %s configured.",
				compression_name,
				(status == MagickTrue ? "is" : "is not"));
        }
#else
      switch (compress_tag)
        {
#  if defined(CCITT_SUPPORT)
        case COMPRESSION_CCITTFAX3:
        case COMPRESSION_CCITTFAX4:
#  endif
#  if defined(YCBCR_SUPPORT) && defined(JPEG_SUPPORT)
        case COMPRESSION_JPEG:
#  endif
#  if defined(LZW_SUPPORT)
        case COMPRESSION_LZW:
#  endif
#  if defined(PACKBITS_SUPPORT)
        case COMPRESSION_PACKBITS:
#  endif
#  if defined(ZIP_SUPPORT)
        case COMPRESSION_ADOBE_DEFLATE:
#  endif
        case COMPRESSION_NONE:
          {
            status = MagickTrue;
            break;
          }
        default:
          {
            status = MagickFalse;
            break;
          }
        }
#endif
    }
  return status;
}

/*
  Convert a libtiff compression tag to a human readable string.
*/
static const char *
CompressionTagToString(unsigned int compress_tag)
{
  const char
    *result = "Unknown";

  switch (compress_tag)
    {
    case COMPRESSION_ADOBE_DEFLATE:
      result="ZIP deflate (Adobe)";
      break;
#if defined(COMPRESSION_DEFLATE)
    case COMPRESSION_DEFLATE:
      result="ZIP deflate (Pixar)";
      break;
#endif
    case COMPRESSION_CCITTFAX3:
      result="CCITT Group 3 fax";
      break;
    case COMPRESSION_CCITTFAX4:
      result="CCITT Group 4 fax";
      break;
    case COMPRESSION_CCITTRLE:
      result="CCITT modified Huffman RLE";
      break;
    case COMPRESSION_CCITTRLEW:
      result="CCITT modified Huffman RLE (Word aligned)";
      break;
#if defined(COMPRESSION_OJPEG)
    case COMPRESSION_OJPEG:
      result="JPEG DCT (Old)";
      break;
#endif
    case COMPRESSION_JPEG:
      result="JPEG DCT";
      break;
#if defined(COMPRESSION_JBIG)
    case COMPRESSION_JBIG:
      result="JBIG";
      break;
#endif
    case COMPRESSION_LZW:
      result="LZW";
      break;
#if defined(COMPRESSION_NEXT)
    case COMPRESSION_NEXT:
      result="NeXT 2-bit RLE";
      break;
#endif
    case COMPRESSION_NONE:
      result="not compressed";
      break;
    case COMPRESSION_PACKBITS:
      result="Macintosh RLE (Packbits)";
      break;
#if defined(COMPRESSION_THUNDERSCAN)
    case COMPRESSION_THUNDERSCAN:
      result="ThunderScan RLE";
      break;
#endif
  }
  return result;
}

static const char *
PhotometricTagToString(unsigned int photometric)
{
  const char
    *result = "Unknown";
  
  switch (photometric)
    {
    case  PHOTOMETRIC_CIELAB:
      result="CIELAB";
      break;
    case PHOTOMETRIC_LOGL:
      result="CIE Log2(L)";
      break;
    case PHOTOMETRIC_LOGLUV:
      result="LOGLUV";
      break;
#if defined(PHOTOMETRIC_MASK)
    case PHOTOMETRIC_MASK:
      result="MASK";
      break;
#endif
    case PHOTOMETRIC_MINISBLACK:
      result="MINISBLACK";
      break;
    case PHOTOMETRIC_MINISWHITE:
      result="MINISWHITE";
      break;
    case PHOTOMETRIC_PALETTE:
      result="PALETTE";
      break;
    case PHOTOMETRIC_RGB:
      result="RGB";
      break;
    case PHOTOMETRIC_SEPARATED:
      result="SEPARATED";
      break;
    case PHOTOMETRIC_YCBCR:
      result="YCBCR";
      break;
    }

  return result;
}

static unsigned int
ReadNewsProfile(char *text,long int length,Image *image,int type)
{
  register unsigned char
    *p;

  if (length <= 0)
    return(False);
  p=(unsigned char *) text;
  if (type == TIFFTAG_RICHTIFFIPTC)
    {
      /*
        Handle IPTC tag.
      */
      length*=4;
      return SetImageProfile(image,"IPTC",p,(size_t) length);
    }
  /*
    Handle PHOTOSHOP tag.
  */
  while (length > 0)
  {
#if defined(GET_ONLY_IPTC_DATA)
    if (LocaleNCompare((char *) p,"8BIM44",6) == 0)
#else
    if (LocaleNCompare((char *) p,"8BIM",4) == 0)
#endif
      break;
    length-=2;
    p+=2;
  }
  if (length <= 0)
    return(False);
#if defined(GET_ONLY_IPTC_DATA)
  /*
    Eat OSType, IPTC ID code, and Pascal string length bytes.
  */
  p+=6;
  length=(*p++);
  if (length)
    p+=length;
  if ((length & 0x01) == 0)
    p++;  /* align to an even byte boundary */
  length=(p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
  p+=4;
#endif
  return SetImageProfile(image,"8BIM",p,(size_t) length);
}

/*
  Return MagickTrue if TIFF warnings should be thrown as a warning
  exception rather than logged.
*/
static MagickBool
CheckThrowWarnings(const ImageInfo *image_info)
{
  const char *
    definition_value;

  MagickBool
    report_warnings=MagickFalse;

  if ((definition_value=AccessDefinition(image_info,"tiff","report-warnings")))
    {
      if (LocaleCompare(definition_value,"TRUE") == 0)
        report_warnings=MagickTrue;
    }

  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Reporting TIFF warnings via %s",
                        (report_warnings ? "exception" :
                         "log message"));

  return report_warnings;
}


#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/* Close BLOB */
static int
TIFFCloseBlob(thandle_t image_handle)
{
  Image
    *image = ((Magick_TIFF_ClientData *) image_handle)->image;

#if LOG_TIFF_BLOB_IO
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"TIFF close blob");
#endif /* LOG_TIFF_BLOB_IO */
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseBlob(image);
  return(0);
}

/* Report errors. */
static unsigned int
TIFFErrors(const char *module,const char *format,
  va_list warning)
{
  ExceptionInfo
    *tiff_exception;

  char
    message[MaxTextExtent];

  errno=0;
  (void) vsnprintf(message,MaxTextExtent-2,format,warning);
  message[MaxTextExtent-2]='\0';
  (void) strlcat(message,".",MaxTextExtent);
  tiff_exception=(ExceptionInfo *) MagickTsdGetSpecific(tsd_key);
  ThrowException2(tiff_exception,CoderError,message,module);
  return(True);
}

/* Memory map entire input file in read-only mode. */
static int
TIFFMapBlob(thandle_t image_handle,tdata_t *base,toff_t *size)
{
  Image
    *image = ((Magick_TIFF_ClientData *) image_handle)->image;

  *base = (tdata_t *) GetBlobStreamData(image);
  if (*base)
    *size = (toff_t) GetBlobSize(image);

  if (*base)
    {
#if LOG_TIFF_BLOB_IO
      if (image->logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			      "TIFF mapped blob: base=0x%p size=%" MAGICK_OFF_F
			      "d",*base, (magick_off_t) *size);
#endif
      return 1;
    }
  return(0);
}

/* Read BLOB data at current offset */
static tsize_t
TIFFReadBlob(thandle_t image_handle,tdata_t data,tsize_t size)
{
  Image
    *image = ((Magick_TIFF_ClientData *) image_handle)->image;

  tsize_t
    result;

  result=(tsize_t) ReadBlob(image,(size_t) size,data);

#if LOG_TIFF_BLOB_IO
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "TIFF read blob: data=0x%p size=%"
                          MAGICK_SIZE_T_F "u, returns %"
			  MAGICK_SIZE_T_F "u",
			  data, (MAGICK_SIZE_T) size,
                          (MAGICK_SIZE_T) result);
#endif /* LOG_TIFF_BLOB_IO */

  return result;
}

/* Seek to BLOB offset */
static toff_t
TIFFSeekBlob(thandle_t image_handle,toff_t offset,int whence)
{
  Image
    *image = ((Magick_TIFF_ClientData *) image_handle)->image;

  toff_t
    result;

  result=SeekBlob(image,offset,whence);
#if LOG_TIFF_BLOB_IO
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "TIFF seek blob: offset=%" MAGICK_OFF_F
			  "u whence=%d (%s), returns %" MAGICK_OFF_F "d",
                          (magick_off_t) offset,
			  whence,
                          (whence == SEEK_SET ? "SET" :
                           (whence == SEEK_CUR ? "CUR" :
                            (whence == SEEK_END ? "END" : "unknown"))),
			  (magick_off_t) result);
#endif  /* LOG_TIFF_BLOB_IO */
  return result;
}

/* Obtain BLOB size */
static toff_t
TIFFGetBlobSize(thandle_t image_handle)
{
  Image
    *image = ((Magick_TIFF_ClientData *) image_handle)->image;

  toff_t
    result;

  result=(toff_t) GetBlobSize(image);

#if LOG_TIFF_BLOB_IO
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			  "TIFF get blob size returns %" MAGICK_OFF_F "d",
			  (magick_off_t) result);
#endif /* LOG_TIFF_BLOB_IO */

  return result;
}

/* Unmap BLOB memory */
static void
TIFFUnmapBlob(thandle_t ARGUNUSED(image),
              tdata_t ARGUNUSED(base),
              toff_t ARGUNUSED(size))
{
#if LOG_TIFF_BLOB_IO
  Image
    *image = ((Magick_TIFF_ClientData *) image_handle)->image;

  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			  "TIFF unmap blob: base=0x%p size=%" MAGICK_OFF_F "d",
			  base,(magick_off_t) size);
#endif  /* LOG_TIFF_BLOB_IO */
}

/* Report warnings as a coder log message. */
static unsigned int
TIFFWarningsLogOnly(const char *module,const char *format,va_list warning)
{
/*   ExceptionInfo */
/*     *tiff_exception; */

  char
    message[MaxTextExtent];

  (void) module;
  errno=0;
  (void) vsnprintf(message,MaxTextExtent-2,format,warning);
  message[MaxTextExtent-2]='\0';
  (void) strlcat(message,".",MaxTextExtent);
/*   tiff_exception=(ExceptionInfo *) MagickTsdGetSpecific(tsd_key); */
/*   ThrowException2(tiff_exception,CoderWarning,message,module); */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "TIFF Warning: %s",message);
  return(True);
}

/* Report warnings as exception in thread-specific ExceptionInfo */
static unsigned int
TIFFWarningsThrowException(const char *module,const char *format,va_list warning)
{
  ExceptionInfo
    *tiff_exception;

  char
    message[MaxTextExtent];

  (void) module;
  errno=0;
  (void) vsnprintf(message,MaxTextExtent-2,format,warning);
  message[MaxTextExtent-2]='\0';
  (void) strlcat(message,".",MaxTextExtent);
  tiff_exception=(ExceptionInfo *) MagickTsdGetSpecific(tsd_key);
  ThrowException2(tiff_exception,CoderWarning,message,module);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "TIFF Warning: %s",message);
  return(True);
}

/* Write data at current offset */
static tsize_t
TIFFWriteBlob(thandle_t image_handle,tdata_t data,tsize_t size)
{
  Image
    *image = ((Magick_TIFF_ClientData *) image_handle)->image;

  tsize_t
    result;

  result=(tsize_t) WriteBlob(image,(size_t) size,data);

#if LOG_TIFF_BLOB_IO
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			  "TIFF write blob: data=0x%p size=%" MAGICK_SIZE_T_F
			  "u, returns %" MAGICK_SIZE_T_F "u",
			  data, (MAGICK_SIZE_T) size,
			  (MAGICK_SIZE_T) result);
#endif  /* LOG_TIFF_BLOB_IO */

  return result;
}

/*
  Convert TIFF data from libtiff "native" format to byte-parsable big endian
*/
#if !defined(WORDS_BIGENDIAN)
static void
SwabDataToBigEndian(const uint16 bits_per_sample, tdata_t data,
		    const tsize_t size)
{
  if (bits_per_sample == 64U)
    {
      TIFFSwabArrayOfDouble((double*) data,
			    NumberOfObjectsInArray(size,sizeof(double)));
    }
  else if (bits_per_sample == 32U)
    {
      TIFFSwabArrayOfLong((uint32*) data,
			  NumberOfObjectsInArray(size,sizeof(uint32)));
    }
#if defined(HAVE_TIFFSWABARRAYOFTRIPLES)
  /* New libtiff function to swap 24 bit values.  Grumble ... */
  else if (bits_per_sample == 24U)
    {
      TIFFSwabArrayOfTriples(data,
			     NumberOfObjectsInArray(size,3));
    }
#endif
  else if (bits_per_sample == 16U)
    {
      TIFFSwabArrayOfShort((uint16*) data,
			   NumberOfObjectsInArray(size,sizeof(uint16)));
    }
}
#endif

/*
  Convert TIFF data from byte-parsable big endian to libtiff "native" format.
*/
#if !defined(WORDS_BIGENDIAN)
static void
SwabDataToNativeEndian(const uint16 bits_per_sample, tdata_t data,
		       const tsize_t size)
{
  if (bits_per_sample == 64)
    {
      TIFFSwabArrayOfDouble((double*) data,
			    NumberOfObjectsInArray(size,sizeof(double)));
    }
  else if (bits_per_sample == 32)
    {
      TIFFSwabArrayOfLong((uint32*) data,
			  NumberOfObjectsInArray(size,sizeof(uint32)));
    }
#if defined(HAVE_TIFFSWABARRAYOFTRIPLES)
  /* New libtiff function to swap 24 bit values.  Grumble ... */
  else if (bits_per_sample == 24U)
    {
      TIFFSwabArrayOfTriples(data,
			     NumberOfObjectsInArray(size,3));
    }
#endif
  else if (bits_per_sample == 16)
    {
      TIFFSwabArrayOfShort((uint16*) data,
			   NumberOfObjectsInArray(size,sizeof(uint16)));
    }
}
#endif

/*
  Initialize the image colormap.
*/
static MagickPassFail
InitializeImageColormap(Image *image, TIFF *tiff)
{
  uint16
    bits_per_sample,
    photometric;

  register unsigned int
    i;

  unsigned int
    max_sample_value,
    status = MagickFail;

  if (TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,&bits_per_sample) != 1)
    return status;
  if (TIFFGetFieldDefaulted(tiff,TIFFTAG_PHOTOMETRIC,&photometric) != 1)
    return status;

  /*
    Compute colormap size
  */
  max_sample_value=MaxValueGivenBits(bits_per_sample);

  image->colors=0;
  if (MaxColormapSize > max_sample_value)
    image->colors=max_sample_value+1;
  else if (MaxColormapSize > MaxRGB)
    {
      if (photometric == PHOTOMETRIC_PALETTE)
        return status;
      else
        image->colors=MaxColormapSize;
    }
  
  if (image->colors > 0)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "Allocating colormap with %u colors",
                            image->colors);

      /*
        Allocate colormap.
      */
      if (AllocateImageColormap(image,image->colors) == MagickFail)
        return status;

      /*
        Create colormap.
      */
      switch (photometric)
        {
        case PHOTOMETRIC_MINISBLACK:
        case PHOTOMETRIC_MINISWHITE:
        default:
          {
            /*
              Ascending order produced by AllocateImageColormap() is sufficient.
            */
            status = MagickPass;
            break;
          }
        case PHOTOMETRIC_PALETTE:
          {
            long
              range;
        
            uint16
              *blue_colormap,
              *green_colormap,
              *red_colormap;
        
            (void) TIFFGetField(tiff,TIFFTAG_COLORMAP,&red_colormap,
                                &green_colormap,&blue_colormap);
            range=256L;  /* might be old style 8-bit colormap */
            for (i=0; i < image->colors; i++)
              if ((red_colormap[i] >= 256) || (green_colormap[i] >= 256) ||
                  (blue_colormap[i] >= 256))
                {
                  range=65535L;
                  break;
                }
            for (i=0; i < image->colors; i++)
              {
                image->colormap[i].red=(Quantum)
                  (((double) MaxRGB*red_colormap[i])/range+0.5);
                image->colormap[i].green=(Quantum)
                  (((double) MaxRGB*green_colormap[i])/range+0.5);
                image->colormap[i].blue=(Quantum)
                  (((double) MaxRGB*blue_colormap[i])/range+0.5);
              }
            status = MagickPass;
            break;
          }
        }
      if (status == MagickPass)
        {
          register const PixelPacket
            *p;

          register unsigned int
            scale;

          unsigned int
            depth=1;

          /*
            Evaluate colormap depth.
          */
          p=image->colormap;
          scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth));
          for (i=image->colors; i != 0; i--)
            {
              if ((p->red != scale*(p->red/scale)) ||
                  (p->green != scale*(p->green/scale)) ||
                  (p->blue != scale*(p->blue/scale)))
                {
                  depth++;
                  if (depth == QuantumDepth)
                    break;
                  scale=MaxRGB / (MaxRGB >> (QuantumDepth-depth));
                  continue;
                }
              p++;
            }
          if (depth < 8)
            depth=8;
          else
            depth=16;
          image->depth=depth;
        }
    }

  return status;
}

/*
  Determine the quauntum import/export method to use with
  ImportImagePixelArea() and ExportImagePixelArea() based on the
  nature of the image, the TIFF photometric, sample format, samples
  per pixel, the desired planar configuration, and the specified plane
  (0 for contiguous planar configuration). Updates quantum_type with
  the import/export method, and quantum_samples with the number of
  samples consumed by one pixel. Returns MagickPass if the photometric
  is supported.
*/
static MagickPassFail
QuantumTransferMode(const Image *image,
		    const uint16 photometric,
		    const uint16 sample_format,
                    const unsigned int samples_per_pixel,
		    const uint16 planar_config,
		    const unsigned int plane,
		    QuantumType *quantum_type,
		    int *quantum_samples,
                    ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);

  *quantum_type=UndefinedQuantum;
  *quantum_samples=0;
  if ((sample_format == SAMPLEFORMAT_INT) ||
      (sample_format == SAMPLEFORMAT_UINT) ||
      (sample_format == SAMPLEFORMAT_VOID) ||
      (sample_format == SAMPLEFORMAT_IEEEFP))
    {
      switch (photometric)
        {
        case PHOTOMETRIC_CIELAB:
          {
            /* samples_per_pixel may be 1 or 3 */
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                switch (plane)
                  {
                  case 0:
                    if (samples_per_pixel == 1)
                      *quantum_type=GrayQuantum;
                    else
                      *quantum_type=RedQuantum;
                    break;
                  case 1:
                    *quantum_type=GreenQuantum;
                    break;
                  case 2:
                    *quantum_type=BlueQuantum;
                    break;
                  case 3:
                    *quantum_type=AlphaQuantum;
                    break;
                  }
                *quantum_samples=1;
              }
            else
              {
                if (samples_per_pixel == 1)
                  {
                    if (image->matte)
                      {
                        *quantum_type=GrayAlphaQuantum;
                        *quantum_samples=2;
                      }
                    else
                      {
                        *quantum_type=GrayQuantum;
                        *quantum_samples=1;
                      }
                  }
                else if (samples_per_pixel == 3)
                  {
                    if (image->matte)
                      {
                        *quantum_type=RGBAQuantum;
                        *quantum_samples=4;
                      }
                    else
                      {
                        *quantum_type=RGBQuantum;
                        *quantum_samples=3;
                      }
                  }
              }
            break;
          }
        case PHOTOMETRIC_LOGL:
          {
            *quantum_type=CIEYQuantum;
            *quantum_samples=1;
            break;
          }
        case PHOTOMETRIC_LOGLUV:
          {
            if (samples_per_pixel == 1)
              {
                /* FIXME: this might not work. */
                *quantum_type=CIEYQuantum;
                *quantum_samples=1;
              }
            else
              {
                *quantum_type=CIEXYZQuantum;
                *quantum_samples=3;
              }
            break;
          }
        case PHOTOMETRIC_MINISBLACK:
        case PHOTOMETRIC_MINISWHITE:
          {
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                switch (plane)
                  {
                  case 0:
                    *quantum_type=GrayQuantum;
                    *quantum_samples=1;
                    break;
                  case 1:
                    *quantum_type=AlphaQuantum;
                    *quantum_samples=1;
                    break;
                  }
              }
            else
              {
                if (image->matte)
                  {
                    *quantum_type=GrayAlphaQuantum;
                    *quantum_samples=2;
                  }
                else
                  {
                    *quantum_type=GrayQuantum;
                    *quantum_samples=1;
                  }
              }
            break;
          }
        case PHOTOMETRIC_PALETTE:
          {
            if (sample_format == SAMPLEFORMAT_UINT)
              {
                if (image->matte)
                  {
                    *quantum_type=IndexAlphaQuantum;
                    *quantum_samples=2;
                  }
                else
                  {
                    *quantum_type=IndexQuantum;
                    *quantum_samples=1;
                  }
              }
            break;
          }
        case PHOTOMETRIC_RGB:
          {
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                switch (plane)
                  {
                  case 0:
                    *quantum_type=RedQuantum;
                    break;
                  case 1:
                    *quantum_type=GreenQuantum;
                    break;
                  case 2:
                    *quantum_type=BlueQuantum;
                    break;
                  case 3:
                    *quantum_type=AlphaQuantum;
                    break;
                  }
                *quantum_samples=1;
              }
            else
              {
                if (image->matte)
                  {
                    *quantum_type=RGBAQuantum;
                    *quantum_samples=4;
                  }
                else
                  {
                    *quantum_type=RGBQuantum;
                    *quantum_samples=3;
                  }
              }
            break;
          }
        case PHOTOMETRIC_SEPARATED:
          {
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                switch (plane)
                  {
                  case 0:
                    *quantum_type=CyanQuantum;
                    break;
                  case 1:
                    *quantum_type=MagentaQuantum;
                    break;
                  case 2:
                    *quantum_type=YellowQuantum;
                    break;
                  case 3:
                    *quantum_type=BlackQuantum;
                    break;
                  case 4:
                    *quantum_type=AlphaQuantum;
                    break;
                  }
                *quantum_samples=1;
              }
            else
              {
                if (image->matte)
                  {
                    *quantum_type=CMYKAQuantum;
                    *quantum_samples=5;
                  }
                else
                  {
                    *quantum_type=CMYKQuantum;
                    *quantum_samples=4;
                  }
              }
            break;
          }
        case PHOTOMETRIC_YCBCR:
          {
            /*
              This is here to support JPEGCOLORMODE_RGB which claims a
              YCbCr photometric, but passes RGB to libtiff.
             */
            if (samples_per_pixel == 3)
              {
                *quantum_type=RGBQuantum;
                *quantum_samples=3;
              }
            break;
          }
        }
    }
  /* fprintf(stderr,"Quantum Type: %d Quantum Samples: %d\n",(int) *quantum_type,*quantum_samples); */
  /* FIXME: Throw exception if there is an error */
  /* FIXME: We do need to support YCbCr! */
  
  return (*quantum_samples != 0 ? MagickPass : MagickFail);
}

/*
  Compact samples to only contain raster data. This may seem
  inefficient, but it allows us to easily deal with contiguous images
  which contain extra samples while optimizing performance for images
  without extra samples.
*/
static void
CompactSamples( const unsigned long total_pixels,
		const unsigned int bits_per_sample,
		const unsigned int samples_per_pixel,
		const unsigned int quantum_samples,
		unsigned char *samples)
{
  if (samples_per_pixel > quantum_samples)
    {
      /*
        Compact scanline to only contain raster data.
      */

      BitStreamReadHandle
        read_stream;

      BitStreamWriteHandle
        write_stream;

      unsigned long
        pixels;

      unsigned int
        count,
        quantum_value;

      MagickBitStreamInitializeRead(&read_stream,samples);
      MagickBitStreamInitializeWrite(&write_stream,samples);

      for (pixels = total_pixels; pixels != 0 ; pixels--)
        {
          for (count = quantum_samples; count != 0 ; count--)
            {
              quantum_value=MagickBitStreamMSBRead(&read_stream,bits_per_sample);
              MagickBitStreamMSBWrite(&write_stream,bits_per_sample,quantum_value);
            }
          for (count = samples_per_pixel-quantum_samples; count != 0 ; count--)
            {
              (void) MagickBitStreamMSBRead(&read_stream,bits_per_sample);
            }
        }
    }
}


/*
  Convert selected pixel area to associated alpha representation.
*/
static void
AssociateAlphaRegion(Image *image)
{
  register PixelPacket
    *q;

  register long
    x;

  register double
    alpha,
    value;

  long
    number_pixels;

  number_pixels=(long) GetPixelCacheArea(image);
  q=AccessMutablePixels(image);

  for (x = number_pixels; x > 0; --x)
    {
      alpha=((double) MaxRGB-q->opacity)/MaxRGB;
      value=(double) q->red*alpha;
      q->red=RoundDoubleToQuantum(value);
      value=(double) q->green*alpha;
      q->green=RoundDoubleToQuantum(value);
      value=(double) q->blue*alpha;
      q->blue=RoundDoubleToQuantum(value);
      q++;
    }
}

/*
  Convert associated alpha to internal representation for selected
  pixel area.
*/
static void
DisassociateAlphaRegion(Image *image)
{
  register PixelPacket
    *q;

  register long
    x;

  register double
    alpha,
    value;

  long
    number_pixels;

  number_pixels=(long) GetPixelCacheArea(image);
  q=AccessMutablePixels(image);

  for (x = number_pixels; x > 0; --x)
    {
      if (q->opacity != (Quantum) MaxRGB)
        {
          alpha=((double) MaxRGB-q->opacity)/MaxRGB;
          value=(double) q->red/alpha;
          q->red=RoundDoubleToQuantum(value);
          value=(double) q->green/alpha;
          q->green=RoundDoubleToQuantum(value);
          value=(double) q->blue/alpha;
          q->blue=RoundDoubleToQuantum(value);
        }
      q++;
    }
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

typedef enum
{
  ScanLineMethod,            /* Scanline method */
  StrippedMethod,            /* Stripped method */
  TiledMethod,               /* Tiled method */
  RGBAStrippedMethod,        /* RGBA stripped method */
  RGBATiledMethod,           /* RGBA tiled method */
  RGBAPuntMethod             /* RGBA whole-image method (last resort) */
} TIFFMethod;

static Image *
ReadTIFFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    *text;

  const char *
    definition_value;

  float
    *chromaticity,
    x_resolution,
    y_resolution;

  Image
    *image;

  unsigned int
    y;

  register unsigned int
    x;

  register PixelPacket
    *q;

  register unsigned int
    i;

  TIFF
    *tiff;

  uint16
    compress_tag,
    bits_per_sample,
    extra_samples,
    fill_order,
    max_sample_value,
    min_sample_value,
    orientation,
    pages,
    photometric,
    planar_config,
    *sample_info,
    sample_format,
    samples_per_pixel,
    units;

  uint32
    height,
    rows_per_strip,
    width;

  TIFFMethod
    method;

  Magick_TIFF_ClientData
    client_data;

  ImportPixelAreaOptions
    import_options;

  AlphaType
    alpha_type=UnspecifiedAlpha;

  MagickBool
    logging,
    more_frames;

  MagickPassFail
    status;

  /*
    Open image.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  logging=IsEventLogging();
  image=AllocateImage(image_info);
  more_frames=MagickFalse;
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFail)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  (void) MagickTsdSetSpecific(tsd_key,(void *) exception);
  (void) TIFFSetErrorHandler((TIFFErrorHandler) TIFFErrors);
  (void) TIFFSetWarningHandler((TIFFErrorHandler) (CheckThrowWarnings(image_info) ?
                                                   TIFFWarningsThrowException :
                                                   TIFFWarningsLogOnly));
  client_data.image=image;
  client_data.image_info=image_info;
  tiff=TIFFClientOpen(image->filename,"rb",(thandle_t) &client_data,TIFFReadBlob,
		      TIFFWriteBlob,TIFFSeekBlob,TIFFCloseBlob,
		      TIFFGetBlobSize,TIFFMapBlob,TIFFUnmapBlob);
  if (tiff == (TIFF *) NULL)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);

  if (image_info->subrange != 0)
    while (image->scene < image_info->subimage)
      {
        /*
          Skip to next image.
        */
        image->scene++;
        status=TIFFReadDirectory(tiff);
        if (status == False)
          {
            TIFFClose(tiff);
            ThrowReaderException(CorruptImageError,UnableToReadSubImageData,
                                 image);
          }
      }
  do
    {
      if (image_info->verbose > 1)
        TIFFPrintDirectory(tiff,stdout,False);

      /*
        Read critical tags.  We need a value for these tags in order to proceed.
      */
      status = 1;
      status &= (TIFFGetFieldDefaulted(tiff,TIFFTAG_PHOTOMETRIC,&photometric) == 1);
      if ((status == 1) &&
          ((photometric == PHOTOMETRIC_LOGL) || (photometric == PHOTOMETRIC_LOGLUV)))
        status &= (TIFFSetField(tiff,TIFFTAG_SGILOGDATAFMT,SGILOGDATAFMT_FLOAT) == 1);
      status &= (TIFFGetFieldDefaulted(tiff,TIFFTAG_COMPRESSION,&compress_tag) == 1);
      status &= (TIFFGetField(tiff,TIFFTAG_IMAGEWIDTH,&width) == 1);
      status &= (TIFFGetField(tiff,TIFFTAG_IMAGELENGTH,&height) == 1);
      status &= (TIFFGetFieldDefaulted(tiff,TIFFTAG_PLANARCONFIG,&planar_config) == 1);
      status &= (TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLESPERPIXEL,&samples_per_pixel) == 1);
      status &= (TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,&bits_per_sample) == 1);
      status &= (TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLEFORMAT,&sample_format) == 1);
      status &= (TIFFGetFieldDefaulted(tiff,TIFFTAG_MINSAMPLEVALUE,&min_sample_value) == 1);
      status &= (TIFFGetFieldDefaulted(tiff,TIFFTAG_MAXSAMPLEVALUE,&max_sample_value) == 1);
      status &= (TIFFGetFieldDefaulted(tiff,TIFFTAG_ROWSPERSTRIP,&rows_per_strip) == 1);
      status &= (TIFFGetFieldDefaulted(tiff,TIFFTAG_FILLORDER,&fill_order) == 1);
      if (status == 0)
        {
          TIFFClose(tiff);
          /*
            Promote TIFF warnings to errors for these critical tags.
          */
          if ((exception->severity > WarningException) &&
              (exception->severity < ErrorException))
            exception->severity += (ErrorException - WarningException);
          ThrowReaderException(CorruptImageError,ImproperImageHeader,
                               image);
        }
      if (TIFFGetField(tiff,TIFFTAG_ORIENTATION,&orientation) == 1)
        image->orientation=(OrientationType) orientation;
      if (logging)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Geometry: %ux%u",
                                (unsigned int) width,(unsigned int) height);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),"PlanarConfiguration: %s",
                                planar_config == PLANARCONFIG_CONTIG ? "contiguous" :
                                planar_config == PLANARCONFIG_SEPARATE ? "separate" :
                                "UNKNOWN");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Samples per pixel: %u", samples_per_pixel);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Sample format: %s",
                                sample_format == SAMPLEFORMAT_UINT ? "Unsigned integer" :
                                sample_format == SAMPLEFORMAT_INT ? "Signed integer" :
                                sample_format == SAMPLEFORMAT_IEEEFP ? "IEEE floating point" :
                                sample_format == SAMPLEFORMAT_VOID ? "Untyped data" :
                                sample_format == SAMPLEFORMAT_COMPLEXINT ? "Complex signed int" :
                                sample_format == SAMPLEFORMAT_COMPLEXIEEEFP ? "Complex IEEE floating point" :
                                "UNKNOWN");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Bits per sample: %u",bits_per_sample);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Min sample value: %u",min_sample_value);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Max sample value: %u",max_sample_value);
          if (sample_format == SAMPLEFORMAT_IEEEFP)
            {
              double
                value;
              
              if (TIFFGetField(tiff,TIFFTAG_SMINSAMPLEVALUE,&value) == 1)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "Special min sample value: %g", value);
              if (TIFFGetField(tiff,TIFFTAG_SMAXSAMPLEVALUE,&value) == 1)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "Special max sample value: %g", value);
            }
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Photometric: %s", PhotometricTagToString(photometric));
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Compression: %s", CompressionTagToString(compress_tag));
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Byte swapped: %s",TIFFIsByteSwapped(tiff) ?
                                "true" : "false");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Bit fill order: %s",
                                fill_order == FILLORDER_LSB2MSB ? "LSB2MSB" :
                                fill_order == FILLORDER_MSB2LSB ? "MSB2LSB" :
                                "unknown");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Rows per strip: %u",(unsigned int) rows_per_strip);
        }
      ImportPixelAreaOptionsInit(&import_options);
      if (photometric == PHOTOMETRIC_CIELAB)
        {
#if 1
          image->colorspace=LABColorspace;
#else
          TIFFClose(tiff);
          ThrowReaderException(CoderError,UnableToReadCIELABImages,image);
#endif
        }
      if (photometric == PHOTOMETRIC_SEPARATED)
        image->colorspace=CMYKColorspace;
      if (planar_config == PLANARCONFIG_SEPARATE)
	image->interlace=PlaneInterlace;
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_RESOLUTIONUNIT,&units);
      x_resolution=image->x_resolution;
      y_resolution=image->y_resolution;
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_XRESOLUTION,&x_resolution);
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_YRESOLUTION,&y_resolution);
      image->x_resolution=x_resolution;
      image->y_resolution=y_resolution;
      chromaticity=(float *) NULL;
      if (TIFFGetField(tiff,TIFFTAG_WHITEPOINT,&chromaticity) == 1)
        {
          if (chromaticity != (float *) NULL)
            {
              image->chromaticity.white_point.x=chromaticity[0];
              image->chromaticity.white_point.y=chromaticity[1];
            }
        }
      chromaticity=(float *) NULL;
      if (TIFFGetField(tiff,TIFFTAG_PRIMARYCHROMATICITIES,&chromaticity) == 1)
        {
          if (chromaticity != (float *) NULL)
            {
              image->chromaticity.red_primary.x=chromaticity[0];
              image->chromaticity.red_primary.y=chromaticity[1];
              image->chromaticity.green_primary.x=chromaticity[2];
              image->chromaticity.green_primary.y=chromaticity[3];
              image->chromaticity.blue_primary.x=chromaticity[4];
              image->chromaticity.blue_primary.y=chromaticity[5];
            }
        }
      {
        /*
          Retrieve embedded profiles.
        */
        uint32
          length;
        /*
          ICC ICM color profile.
        */
#if defined(TIFFTAG_ICCPROFILE)
        if (TIFFGetField(tiff,TIFFTAG_ICCPROFILE,&length,&text) == 1)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "ICC ICM embedded profile with length %lu bytes",
                                  (unsigned long)length);
            (void) SetImageProfile(image,"ICM",(unsigned char *) text,(size_t) length);
          }
#endif /* defined(TIFFTAG_ICCPROFILE) */
        /*
          IPTC/Photoshop profile.
        */
#if defined(TIFFTAG_PHOTOSHOP)
        /* Photoshop profile (with embedded IPTC profile) */
        if (TIFFGetField(tiff,TIFFTAG_PHOTOSHOP,&length,&text) == 1)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Photoshop embedded profile with length %lu bytes",
                                  (unsigned long) length);
            (void) ReadNewsProfile(text,(long) length,image,TIFFTAG_PHOTOSHOP);
          }
#elif defined(TIFFTAG_RICHTIFFIPTC)
        /* IPTC TAG from RichTIFF specifications */
        if (TIFFGetField(tiff,TIFFTAG_RICHTIFFIPTC,&length,&text) == 1)
          {
            if (TIFFIsByteSwapped(tiff))
              TIFFSwabArrayOfLong((uint32 *) text,length);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "IPTC Newsphoto embedded profile with length %u bytes",length);
            ReadNewsProfile(text,length,image,TIFFTAG_RICHTIFFIPTC);
          }
#endif
        /*
          XML XMP profile.
        */
#if defined(TIFFTAG_XMLPACKET)
        /* %XML packet [Adobe XMP Specification, Janary 2004] */
        if (TIFFGetField(tiff,TIFFTAG_XMLPACKET,&length,&text) == 1)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "XMP embedded profile with length %lu bytes",
                                  (unsigned long) length);
            SetImageProfile(image,"XMP",(unsigned char *) text,(size_t) length);
          }
#endif
      }
      /*
        Allocate memory for the image and pixel buffer.
      */
      switch (compress_tag)
        {
        case COMPRESSION_NONE: image->compression=NoCompression; break;
        case COMPRESSION_CCITTFAX3: image->compression=FaxCompression; break;
        case COMPRESSION_CCITTFAX4: image->compression=Group4Compression; break;
        case COMPRESSION_JPEG: image->compression=JPEGCompression; break;
        case COMPRESSION_OJPEG: image->compression=JPEGCompression; break;
        case COMPRESSION_LZW: image->compression=LZWCompression; break;
#if defined(COMPRESSION_LZMA)
	case COMPRESSION_LZMA: image->compression=LZMACompression; break;
#endif /* defined(COMPRESSION_LZMA) */
        case COMPRESSION_DEFLATE: image->compression=ZipCompression; break;
        case COMPRESSION_ADOBE_DEFLATE: image->compression=ZipCompression; break;
        default: image->compression=RLECompression; break;
        }
      image->columns=width;
      image->rows=height;
      image->depth=bits_per_sample;

      /*
        Obtain information about any extra samples.
      */
      extra_samples=0;
      if (TIFFGetField(tiff,TIFFTAG_EXTRASAMPLES,&extra_samples,
                       &sample_info) == 1)
        {
          int
            sample_index;

          if (extra_samples != 0)
            {
              alpha_type=AssociatedAlpha;
              image->matte=True;

              if (sample_info[0] == EXTRASAMPLE_UNSPECIFIED)
                alpha_type=UnspecifiedAlpha;
              else if (sample_info[0] == EXTRASAMPLE_UNASSALPHA)
                alpha_type=UnassociatedAlpha;
              else if (sample_info[0] == EXTRASAMPLE_ASSOCALPHA)
                alpha_type=AssociatedAlpha;
            }
          for (sample_index=0 ; sample_index < extra_samples; sample_index++)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Extra sample %u contains %s alpha",sample_index+1,
                                    ((sample_info[sample_index] == EXTRASAMPLE_ASSOCALPHA) ? "ASSOCIATED" :
                                     (sample_info[sample_index] == EXTRASAMPLE_UNASSALPHA) ? "UNASSOCIATED" :
                                     "UNSPECIFIED"));
            }
        }
      /*
        Handle RGBA images which are improperly marked.
      */
      if (extra_samples == 0)
        if ((photometric == PHOTOMETRIC_RGB) && (samples_per_pixel == 4))
          {
            extra_samples=1;
            alpha_type=AssociatedAlpha;
            image->matte=MagickTrue;
          }

      /*
        Allow the user to over-ride the alpha channel type.
      */
      if (image->matte)
        {
          if ((definition_value=AccessDefinition(image_info,"tiff","alpha")))
            {
              if (LocaleCompare(definition_value,"unspecified") == 0)
                alpha_type=UnspecifiedAlpha;
              else if (LocaleCompare(definition_value,"associated") == 0)
                alpha_type=AssociatedAlpha;
              else if (LocaleCompare(definition_value,"unassociated") == 0)
                alpha_type=UnassociatedAlpha;
            }
        }

      /*
        Describe how the alpha channel will be treated.
      */
      if (image->matte)
        {
          char
            alpha_string[MaxTextExtent];

          switch(alpha_type)
            {
            default:
            case UnspecifiedAlpha:
              (void) strlcpy(alpha_string,"Unspecified",MaxTextExtent);
              break;
            case UnassociatedAlpha:
              (void) strlcpy(alpha_string,"Unassociated",MaxTextExtent);
              break;
            case AssociatedAlpha:
              (void) strlcpy(alpha_string,"Associated",MaxTextExtent);
              break;
            }
          (void) SetImageAttribute(image,"alpha",alpha_string);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Image has a matte channel of type: %s",
                                alpha_string);
        }

      if (units == RESUNIT_INCH)
        image->units=PixelsPerInchResolution;
      if (units == RESUNIT_CENTIMETER)
        image->units=PixelsPerCentimeterResolution;
      {
	uint16
	  pagenumber;

	pagenumber=(unsigned short) image->scene;
	if (TIFFGetFieldDefaulted(tiff,TIFFTAG_PAGENUMBER,&pagenumber,&pages) == 1)
          image->scene=pagenumber;
      }

      if (TIFFGetField(tiff,TIFFTAG_ARTIST,&text) == 1)
        (void) SetImageAttribute(image,"artist",text);

      if (TIFFGetField(tiff,TIFFTAG_COPYRIGHT,&text) == 1) /* TIFFTAG_COPYRIGHT */
        (void) SetImageAttribute(image,"copyright",text);

      if (TIFFGetField(tiff,TIFFTAG_DATETIME,&text) == 1)
        (void) SetImageAttribute(image,"timestamp",text);

      if (TIFFGetField(tiff,TIFFTAG_DOCUMENTNAME,&text) == 1)
        (void) SetImageAttribute(image,"document",text);

      if (TIFFGetField(tiff,TIFFTAG_HOSTCOMPUTER,&text) == 1)
        (void) SetImageAttribute(image,"hostcomputer",text);

      if (TIFFGetField(tiff,TIFFTAG_IMAGEDESCRIPTION,&text) == 1)
        (void) SetImageAttribute(image,"comment",text);

      if (TIFFGetField(tiff,TIFFTAG_MAKE,&text) == 1)
        (void) SetImageAttribute(image,"make",text);

      if (TIFFGetField(tiff,TIFFTAG_MODEL,&text) == 1)
        (void) SetImageAttribute(image,"model",text);

      if (TIFFGetField(tiff,TIFFTAG_PAGENAME,&text) == 1)
        (void) SetImageAttribute(image,"label",text);

      if (TIFFGetField(tiff,TIFFTAG_SOFTWARE,&text) == 1)
        (void) SetImageAttribute(image,"software",text);

      {
	/*
	  "Unsupported" tags return two arguments.
	*/

	uint32
	  count;

	char attribute[MaxTextExtent];

	if (TIFFGetField(tiff,TIFFTAG_OPIIMAGEID,&count,&text) == 1)
	  {
	    (void) strlcpy(attribute,text,Min(sizeof(attribute),(count+1)));
	    (void) SetImageAttribute(image,"imageid",attribute);
	  }
	
	if (TIFFGetField(tiff,33423,&count,&text) == 1)
	  {
	    (void) strlcpy(attribute,text,Min(sizeof(attribute),(count+1)));
	    (void) SetImageAttribute(image,"kodak-33423",attribute);
	  }
	
	if (TIFFGetField(tiff,36867,&count,&text) == 1)
	  {
	    (void) strlcpy(attribute,text,Min(sizeof(attribute),(count+1)));
	    (void) SetImageAttribute(image,"kodak-36867",attribute);
	  }
      }

      if ((photometric == PHOTOMETRIC_PALETTE) ||
          ((photometric == PHOTOMETRIC_MINISWHITE ||
            photometric == PHOTOMETRIC_MINISBLACK) &&
           ((image_info->type == PaletteType) ||
            (image_info->type == PaletteMatteType)) &&
            (MaxColormapSize > MaxValueGivenBits(bits_per_sample))
	   )
	  )
        {
          /*
            Palette image
          */
          if (MaxColormapSize > MaxValueGivenBits(bits_per_sample))
            {
              (void) InitializeImageColormap(image,tiff);
            }
          else
            {
              TIFFClose(tiff);
              ThrowReaderException(CoderError,ColormapTooLarge,image);
            }
        }

      /*
        Quit if in "ping" mode and we are outside of requested range,
        otherwise continue to next frame.
      */
      if (image_info->ping)
        {
          if (image_info->subrange != 0)
            if (image->scene >= (image_info->subimage+image_info->subrange-1))
              break;

          goto read_next_frame;
        }

      if (CheckImagePixelLimits(image, exception) != MagickPass)
        {
          TIFFClose(tiff);
          ThrowReaderException(ResourceLimitError,ImagePixelLimitExceeded,image);
        }

      /*
        Determine which method to use for reading pixels.
      */
      {
        int
          quantum_samples;

        QuantumType
          quantum_type;

        if ((samples_per_pixel > 1) && (compress_tag == COMPRESSION_JPEG) &&
	    (photometric == PHOTOMETRIC_YCBCR))
          {
            /* Following hack avoids the error message "Application
               transferred too many scanlines. (JPEGLib)." caused by
               YCbCr subsampling, but it returns data in RGB rather
               than YCbCr. */
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Resetting photometric from %s to %s for JPEG RGB",
                                    PhotometricTagToString(photometric),
				    PhotometricTagToString(PHOTOMETRIC_RGB));
            (void) TIFFSetField( tiff, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB );
            photometric=PHOTOMETRIC_RGB;
          }
        method=RGBAPuntMethod;
        quantum_type=UndefinedQuantum;
        quantum_samples=0;
        if (QuantumTransferMode(image,photometric,sample_format,samples_per_pixel,
                                planar_config,0,&quantum_type,&quantum_samples,
                                exception)
            == MagickPass)
          {
            method=ScanLineMethod;
            if (compress_tag == COMPRESSION_JPEG)
              {
                if (TIFFIsTiled(tiff))
                  method=TiledMethod;
                else
                  method=StrippedMethod;
              }
#if defined(COMPRESSION_JBIG)
            else if (compress_tag == COMPRESSION_JBIG)
              /* libtiff jbig coder only handles strips */
              method=StrippedMethod;
#endif
            else if (TIFFIsTiled(tiff))
              method=TiledMethod;
            else if ((TIFFStripSize(tiff)) <= (1024*64))
              method=StrippedMethod;
            if (photometric == PHOTOMETRIC_MINISWHITE)
              import_options.grayscale_miniswhite=MagickTrue;
          }
        else
          {
            if (TIFFIsTiled(tiff))
              method=RGBATiledMethod;
            else if (TIFFGetField(tiff,TIFFTAG_ROWSPERSTRIP,&rows_per_strip) == 1)
              method=RGBAStrippedMethod;
          }
      }

      /*
        Set extra import options for floating point.
      */
      if (sample_format == SAMPLEFORMAT_IEEEFP)
        {
          double
            value;

          import_options.sample_type=FloatQuantumSampleType;
          if (TIFFGetField(tiff,TIFFTAG_SMINSAMPLEVALUE,&value) == 1)
            import_options.double_minvalue=value;
          if (TIFFGetField(tiff,TIFFTAG_SMAXSAMPLEVALUE,&value) == 1)
            import_options.double_maxvalue=value;
          if ((definition_value=AccessDefinition(image_info,"tiff","min-sample-value")))
            import_options.double_minvalue=strtod(definition_value,(char **)NULL);
          if ((definition_value=AccessDefinition(image_info,"tiff","max-sample-value")))
            import_options.double_maxvalue=strtod(definition_value,(char **)NULL);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Using min sample value %g, max sample value %g",
                                import_options.double_minvalue,
                                import_options.double_maxvalue);
        }

      /*
        For sample sizes matching a CPU native word, use native endian
        order for import.
      */
      if ((16 == bits_per_sample) || (32 == bits_per_sample) || (64 == bits_per_sample))
        import_options.endian=NativeEndian;
      
      switch (method)
        {
        case ScanLineMethod:
          {
            /*
              Read TIFF image as scanlines.
            */
            unsigned char
              *scanline;

            int
              max_sample,
              quantum_samples,
              sample;

            tsize_t
              scanline_size;

            QuantumType
              quantum_type;

            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using scanline %s read method with %u bits per sample",
                                    PhotometricTagToString(photometric),bits_per_sample);
            /*
              Allocate memory for one scanline.
            */
            scanline_size=TIFFScanlineSize(tiff);
            if (0 == scanline_size)
              {
                status=MagickFail;
                break;
              }

            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Allocating scanline buffer of %lu bytes",
                                    (unsigned long) scanline_size);

            scanline=MagickAllocateMemory(unsigned char *,(size_t) scanline_size);
            if (scanline == (unsigned char *) NULL)
              {
                TIFFClose(tiff);
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
              }
            /*
              Prepare for separate/contiguous retrieval.
            */
            max_sample=1;
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                if (QuantumTransferMode(image,photometric,sample_format,
                                        samples_per_pixel,PLANARCONFIG_CONTIG,0,
                                        &quantum_type,&quantum_samples,exception)
                    == MagickPass)
                  max_sample=quantum_samples;
              }
	    for (sample=0; sample < max_sample; sample++)
	      {
		for (y=0; y < image->rows; y++)
		  {
		    if (sample == 0)
		      q=SetImagePixels(image,0,y,image->columns,1);
		    else
		      q=GetImagePixels(image,0,y,image->columns,1);
		    if (q == (PixelPacket *) NULL)
		      {
			CopyException(exception,&image->exception);
			status=MagickFail;
			break;
		      }
                    /*
                      Obtain a scanline
                    */
                    if (TIFFReadScanline(tiff,(char *) scanline,(uint32) y,sample) == -1)
                      {
                        status=MagickFail;
                        break;
                      }
#if !defined(WORDS_BIGENDIAN)
                    if (24 == bits_per_sample)
                      SwabDataToBigEndian(bits_per_sample,scanline,scanline_size);
#endif
                    /*
                      Determine quantum parse method.
                    */
                    if (QuantumTransferMode(image,photometric,sample_format,
                                            samples_per_pixel,planar_config,sample,
                                            &quantum_type,&quantum_samples,exception)
                        == MagickFail)
                      {
                        CopyException(exception,&image->exception);
                        status=MagickFail;
                        break;
                      }
                    /*
                      Compact scanline to only contain raster data.
                    */
                    if ((samples_per_pixel > quantum_samples) &&
                        (planar_config == PLANARCONFIG_CONTIG))
                      CompactSamples(image->columns, bits_per_sample,
                                     samples_per_pixel, quantum_samples, scanline);
                    /*
                      Import scanline into image.
                    */
                    if (ImportImagePixelArea(image,quantum_type,bits_per_sample,scanline,
                                             &import_options,0) == MagickFail)
                      {
                        CopyException(exception,&image->exception);
                        status=MagickFail;
                        break;
                      }
		    /*
		      Disassociate alpha from pixels if necessary.
		    */
		    if ((image->matte) && (alpha_type == AssociatedAlpha) &&
			(sample == (max_sample-1)))
		      DisassociateAlphaRegion(image);
		    /*
		      Save our updates.
		    */
		    if (!SyncImagePixels(image))
		      {
			CopyException(exception,&image->exception);
			status=MagickFail;
			break;
		      }
		    if (image->previous == (Image *) NULL)
		      if (QuantumTick(y+sample*image->rows,image->rows*max_sample))
			if (!MagickMonitorFormatted(y+sample*image->rows,
						    image->rows*max_sample,exception,
						    LoadImageText,image->filename,
						    image->columns,image->rows))
			  break;
		  }
              }
            MagickFreeMemory(scanline);
            break;
          }
        case StrippedMethod:
          {
            /*
              Read TIFF image using multi-row strip storage.
            */
            unsigned char
              *p,
              *strip;

            long
              /* pixels_per_strip, */
              stride,
              rows_remaining;

            int
              max_sample,
              quantum_samples,
              sample;

            tsize_t
              strip_size,
              strip_size_max;

            tstrip_t
              strip_id;

            QuantumType
              quantum_type;

            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using stripped read method with %u bits per sample",
                                    bits_per_sample);
            /* pixels_per_strip=rows_per_strip*image->columns; */
            p=0;
            strip_size=0;
            strip_id=0;
            /*
              Allocate memory for one strip.
            */
            strip_size_max=TIFFStripSize(tiff);
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Maximum strip size %" MAGICK_SIZE_T_F "u",
                                    (MAGICK_SSIZE_T) strip_size_max);
            if (0 == strip_size_max)
              {
                status=MagickFail;
                break;
              }

            strip=MagickAllocateMemory(unsigned char *,(size_t) strip_size_max);
            if (strip == (unsigned char *) NULL)
              {
                TIFFClose(tiff);
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
              }
            /*
              Prepare for separate/contiguous retrieval.
            */
            max_sample=1;
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                if (QuantumTransferMode(image,photometric,sample_format,
                                        samples_per_pixel,PLANARCONFIG_CONTIG,
                                        0,&quantum_type,&quantum_samples,
                                        exception)
                    == MagickPass)
                  max_sample=quantum_samples;
              }
            /*
              Compute per-row stride.
            */
            stride=TIFFVStripSize(tiff,1);
            /*
              Process each plane
            */
            for (sample=0; sample < max_sample; sample++)
              {
                rows_remaining=0;
                /*
                  Determine quantum parse method.
                */
                if (QuantumTransferMode(image,photometric,sample_format,
                                        samples_per_pixel,planar_config,sample,
                                        &quantum_type,&quantum_samples,exception)
                    == MagickFail)
                  {
                    CopyException(exception,&image->exception);
                    status=MagickFail;
                    break;
                  }
                for (y=0; y < image->rows; y++)
                  {
                    /*
                      Access Magick pixels.
                    */
                    if (sample == 0)
                      q=SetImagePixels(image,0,y,image->columns,1);
                    else
                      q=GetImagePixels(image,0,y,image->columns,1);
                    if (q == (PixelPacket *) NULL)
                      {
                        CopyException(exception,&image->exception);
                        status=MagickFail;
                        break;
                      }
                    if (rows_remaining == 0)
                      {
                        /*
                          Obtain a strip
                        */
                        if (((strip_size=TIFFReadEncodedStrip(tiff,strip_id,strip,
                                                              strip_size_max)) == -1))
                          {
                            status=MagickFail;
                            break;
                          }
#if !defined(WORDS_BIGENDIAN)
                        if (24 == bits_per_sample)
                          SwabDataToBigEndian(bits_per_sample,strip,strip_size);
#endif
                        rows_remaining=rows_per_strip;
                        if (y+rows_per_strip > image->rows)
                          rows_remaining=(rows_per_strip-(y+rows_per_strip-image->rows));
                        p=strip;
                        strip_id++;
                      }
                    /*
                      Compact strip row to only contain raster data.
                    */
                    if ((samples_per_pixel > quantum_samples) &&
                        (planar_config == PLANARCONFIG_CONTIG))
                      CompactSamples(image->columns, bits_per_sample,
                                     samples_per_pixel, quantum_samples, p);
                    /*
                      Import strip row into image.
                    */
                    if (ImportImagePixelArea(image,quantum_type,bits_per_sample,p,
                                             &import_options,0) == MagickFail)
                      {
                        CopyException(exception,&image->exception);
                        status=MagickFail;
                        break;
                      }
                    /*
                      Disassociate alpha from pixels if necessary.
                    */
                    if ((image->matte) && (alpha_type == AssociatedAlpha)
                        && (sample == (max_sample-1)))
                      DisassociateAlphaRegion(image);
                    /*
                      Save our updates.
                    */
                    if (!SyncImagePixels(image))
                      {
                        CopyException(exception,&image->exception);
                        status=MagickFail;
                        break;
                      }
                    /*
                      Advance to next row
                    */
                    p += stride;
                    rows_remaining--;

                    if (image->previous == (Image *) NULL)
                      if (QuantumTick(y+image->rows*sample,image->rows*max_sample))
                        if (!MagickMonitorFormatted(y+image->rows*sample,image->rows*max_sample,exception,
                                                    LoadImageText,image->filename,
						    image->columns,image->rows))
                          {
                            status=MagickFail;
                            break;
                          }
                  }
                if (status == MagickFail)
                  break;
              }
            MagickFreeMemory(strip);
            break;
          }
        case TiledMethod:
          {
            /*
              Read TIFF using tiled storage.
            */
            unsigned char
              *tile;

            uint32
              tile_columns,
              tile_rows;

            tsize_t
              stride,
              tile_size,
              tile_size_max;

            int
              max_sample,
              quantum_samples,
              sample;

            QuantumType
              quantum_type;

            unsigned long
              tile_total_pixels;
        
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using tiled %s read method with %u bits per sample",
                                    PhotometricTagToString(photometric), bits_per_sample);
            /*
              Obtain tile geometry
            */
            if(!(TIFFGetField(tiff,TIFFTAG_TILEWIDTH,&tile_columns) == 1) ||
               !(TIFFGetField(tiff,TIFFTAG_TILELENGTH,&tile_rows) == 1))
              {
                TIFFClose(tiff);
                ThrowReaderException(CoderError,ImageIsNotTiled,image);
              }
            /*
              Obtain the maximum number of bytes required to contain a tile.
            */
            tile_size_max=TIFFTileSize(tiff);
            if (0 == tile_size_max)
              {
                status=MagickFail;
                break;
              }
            /*
              Compute the total number of pixels in one tile
            */
            tile_total_pixels=tile_columns*tile_rows;
            if (logging)
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "TIFF tile geometry %ux%u, %lu pixels",
                                      (unsigned int)tile_columns,
                                      (unsigned int)tile_rows,
                                      tile_total_pixels);
              }
            /*
              Allocate tile buffer
            */
            tile=MagickAllocateMemory(unsigned char *, (size_t) tile_size_max);
            if (tile == (unsigned char *) NULL)
              {
                TIFFClose(tiff);
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
              }
            /*
              Prepare for separate/contiguous retrieval.
            */
            max_sample=1;
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                if (QuantumTransferMode(image,photometric,sample_format,
                                        samples_per_pixel,PLANARCONFIG_CONTIG,
                                        0,&quantum_type,&quantum_samples,
                                        exception)
                    == MagickPass)
                  max_sample=quantum_samples;
              }
            /*
              Compute per-row stride.
            */
            stride=TIFFTileRowSize(tiff);

	    /*
	      Process each plane.
	    */
	    for (sample=0; sample < max_sample; sample++)
	      {
		/*
		  Determine quantum parse method.
		*/
		if (QuantumTransferMode(image,photometric,sample_format,
                                        samples_per_pixel,planar_config,
                                        sample,&quantum_type,&quantum_samples,
                                        exception)
		    == MagickFail)
		  {
		    CopyException(exception,&image->exception);
		    status=MagickFail;
		    break;
		  }
		for (y=0; y < image->rows; y+=tile_rows)
		  {
		    for (x=0; x < image->columns; x+=tile_columns)
		      {
			long
			  tile_set_columns,
			  tile_set_rows;
			
			unsigned char
			  *p;
			
			register long
			  yy;
			
			/*
			  Compute image region corresponding to tile.
			*/
			if (x+tile_columns > image->columns)
			  tile_set_columns=(tile_columns-(x+tile_columns-image->columns));
			else
			  tile_set_columns=tile_columns;
			if (y+tile_rows > image->rows)
			  tile_set_rows=(tile_rows-(y+tile_rows-image->rows));
			else
			  tile_set_rows=tile_rows;
			
			/*
			  Read a tile.
			*/
			if ((tile_size=TIFFReadTile(tiff,tile,x,y,0,sample)) == -1)
			  {
			    status=MagickFail;
			    break;
			  }
#if !defined(WORDS_BIGENDIAN)
			if (24 == bits_per_sample)
			  SwabDataToBigEndian(bits_per_sample,tile,tile_size);
#endif
			p=tile;
			for (yy=y; yy < (long) y+tile_set_rows; yy++)
			  {
			    /*
			      Obtain pixel region corresponding to tile row.
			    */
			    if (sample == 0)
			      q=SetImagePixels(image,x,yy,tile_set_columns,1);
			    else
			      q=GetImagePixels(image,x,yy,tile_set_columns,1);
			    if (q == (PixelPacket *) NULL)
			      {
				CopyException(exception,&image->exception);
				status=MagickFail;
				break;
			      }
			    /*
			      Compact tile row to only contain raster data.
			    */
			    if ((samples_per_pixel > quantum_samples) &&
				(planar_config == PLANARCONFIG_CONTIG))
			      CompactSamples(tile_set_columns, bits_per_sample,
					     samples_per_pixel, quantum_samples, p);
			    /*
			      Import tile row
			    */
			    if (ImportImagePixelArea(image,quantum_type,
						     bits_per_sample,p,
						     &import_options,0)
				== MagickFail)
			      {
				CopyException(exception,&image->exception);
				status=MagickFail;
				break;
			      }
			    /*
			      Disassociate alpha from pixels if necessary.
			    */
			    if ((image->matte) && (alpha_type == AssociatedAlpha)
				&& (sample == (max_sample-1)))
			      DisassociateAlphaRegion(image);
			    /*
			      Save our updates.
			    */
			    if (!SyncImagePixels(image))
			      {
				CopyException(exception,&image->exception);
				status=MagickFail;
				break;
			      }
			    p += stride;
			  }
			if (status == MagickFail)
			  break;
                      }
                    if (status == MagickFail)
                      break;
		    
                    if (image->previous == (Image *) NULL)
                      if (QuantumTick((y+sample*image->rows)/tile_rows,
				      (image->rows*max_sample)/tile_rows))
                        if (!MagickMonitorFormatted((y+sample*image->rows)/tile_rows,
						    (image->rows*max_sample)/tile_rows,
						    exception,
                                                    LoadImageText,image->filename,
						    image->columns,image->rows))
                          {
                            status=MagickFail;
                            break;
                          }
                  }
                if (status == MagickFail)
                  break;
              }

            MagickFreeMemory(tile);
            break;
          }
        case RGBAStrippedMethod:
          {
            unsigned long
              number_pixels;

            uint32
              *strip_pixels;

            register uint32
              *p;

            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using RGB stripped read method with %u bits per sample",
                                    bits_per_sample);
            /*
              Convert stripped TIFF image to DirectClass MIFF image.
            */
            number_pixels=(unsigned long) image->columns*rows_per_strip;
            if ((number_pixels*sizeof(uint32)) != (size_t)
                (number_pixels*sizeof(uint32)))
              {
                TIFFClose(tiff);
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
              }
            strip_pixels=MagickAllocateMemory(uint32 *, (number_pixels*sizeof(uint32)));
            if (strip_pixels == (uint32 *) NULL)
              {
                TIFFClose(tiff);
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
              }
            /*
              Convert image to DirectClass pixel packets.
            */
            i=0;
            p=0;
            for (y=0; y < image->rows; y++)
              {
                q=SetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  {
                    CopyException(exception,&image->exception);
                    status=MagickFail;
                    break;
                  }
                if (0 == i)
                  {
                    if (!TIFFReadRGBAStrip(tiff,y,strip_pixels))
                      {
                        status=MagickFail;
                        break;
                      }
                    i=(long) Min(rows_per_strip,image->rows-y);
                  }
                i--;
                p=strip_pixels+image->columns*i;
                for (x=0; x < image->columns; x++)
                  {
                    q->red=ScaleCharToQuantum(TIFFGetR(*p));
                    q->green=ScaleCharToQuantum(TIFFGetG(*p));
                    q->blue=ScaleCharToQuantum(TIFFGetB(*p));
                    if (image->matte)
                      q->opacity=(Quantum) ScaleCharToQuantum(TIFFGetA(*p));
                    p++;
                    q++;
                  }
                /*
                  Disassociate alpha from pixels if necessary.
                */
                if ((image->matte) && (alpha_type == AssociatedAlpha))
                  DisassociateAlphaRegion(image);                
                if (!SyncImagePixels(image))
                  {
                    CopyException(exception,&image->exception);
                    status=MagickFail;
                    break;
                  }
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    if (!MagickMonitorFormatted(y,image->rows,exception,
                                                LoadImageText,image->filename,
						image->columns,image->rows))
                      {
                        status=MagickFail;
                        break;
                      }
              }
            MagickFreeMemory(strip_pixels);
            break;
          }
        case RGBATiledMethod:
          {
            /*
              Convert tiled TIFF image to DirectClass MIFF image.
            */
            register uint32
              *p;
        
            uint32
              *tile_pixels,
              tile_columns,
              tile_rows;
        
            unsigned long
              tile_total_pixels;
        
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using RGB tiled read method with %u bits per sample",
                                    bits_per_sample);
            /*
              Obtain tile geometry
            */
            if (!(TIFFGetField(tiff,TIFFTAG_TILEWIDTH,&tile_columns) == 1) ||
                !(TIFFGetField(tiff,TIFFTAG_TILELENGTH,&tile_rows) == 1))
              {
                TIFFClose(tiff);
                ThrowReaderException(CoderError,ImageIsNotTiled,image);
              }
            tile_total_pixels=tile_columns*tile_rows;
            if (logging)
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Reading TIFF tiles ...");
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "TIFF tile geometry %ux%u, %lu pixels",
                                      (unsigned int)tile_columns,
                                      (unsigned int)tile_rows, tile_total_pixels);
              }
            /*
              Allocate tile buffer
            */
            tile_pixels=MagickAllocateMemory(uint32*,tile_columns*tile_rows*sizeof (uint32));
            if (tile_pixels == (uint32 *) NULL)
              {
                TIFFClose(tiff);
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
              }
            for (y=0; y < image->rows; y+=tile_rows)
              {
                /*
                  Retrieve a tile height's worth of rows
                */
                PixelPacket
                  *strip;

                unsigned int
                  tile_columns_remaining,
                  tile_rows_remaining;

                /* Compute remaining tile rows */
                if (y+tile_rows < image->rows)
                  tile_rows_remaining=tile_rows;
                else
                  tile_rows_remaining=image->rows-y;
                /*
                  Obtain a row of pixels
                */
                strip=SetImagePixels(image,0,y,image->columns,tile_rows_remaining);
                if (strip == (PixelPacket *) NULL)
                  {
                    CopyException(exception,&image->exception);
                    status=MagickFail;
                    break;
                  }
                for (x=0; x < image->columns; x+=tile_columns)
                  {
                    register unsigned int
                      tile_column,
                      tile_row;

                    /*
                      Obtain one tile.  Origin is bottom left of tile.
                    */
                    if (!TIFFReadRGBATile(tiff,x,y,tile_pixels))
                      {
                        status=MagickFail;
                        break;
                      }
                    /*
                      Compute remaining tile columns
                    */
                    if (x+tile_columns < image->columns)
                      tile_columns_remaining=tile_columns;
                    else
                      tile_columns_remaining=image->columns-x;
                    /*
                      Transfer tile to image
                    */
                    p=tile_pixels+(tile_rows-tile_rows_remaining)*tile_columns;
                    q=strip+(x+(tile_rows_remaining-1)*image->columns);
                    for ( tile_row=tile_rows_remaining; tile_row != 0; tile_row--)
                      {
                        if (image->matte)
                          for (tile_column=tile_columns_remaining; tile_column != 0;
                               tile_column--)
                            {
                              q->red=ScaleCharToQuantum(TIFFGetR(*p));
                              q->green=ScaleCharToQuantum(TIFFGetG(*p));
                              q->blue=ScaleCharToQuantum(TIFFGetB(*p));
                              q->opacity=(Quantum) ScaleCharToQuantum(TIFFGetA(*p));
                              q++;
                              p++;
                            }
                        else
                          for (tile_column=tile_columns_remaining; tile_column != 0;
                               tile_column--)
                            {
                              q->red=ScaleCharToQuantum(TIFFGetR(*p));
                              q->green=ScaleCharToQuantum(TIFFGetG(*p));
                              q->blue=ScaleCharToQuantum(TIFFGetB(*p));
                              q++;
                              p++;
                            }
                        p+=tile_columns-tile_columns_remaining;
                        q-=(image->columns+tile_columns_remaining);
                      }
                    if (status == MagickFail)
                      break;
                  }
                if (status == MagickFail)
                  break;
                /*
                  Disassociate alpha from pixels if necessary.
                */
                if ((image->matte) && (alpha_type == AssociatedAlpha))
                  DisassociateAlphaRegion(image);                
                if (!SyncImagePixels(image))
                  {
                    CopyException(exception,&image->exception);
                    status=MagickFail;
                    break;
                  }
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    if (!MagickMonitorFormatted(y,image->rows,exception,
                                                LoadImageText,image->filename,
						image->columns,image->rows))
                      {
                        status=MagickFail;
                        break;
                      }
              }
            MagickFreeMemory(tile_pixels);
            break;
          }
        case RGBAPuntMethod:
        default:
          {
            register uint32
              *p;

            uint32
              *pixels;

            unsigned long
              number_pixels;

            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using RGB punt read method with %u bits per sample",
                                    bits_per_sample);
            /*
              Convert TIFF image to DirectClass MIFF image.
            */
            number_pixels=(unsigned long) image->columns*image->rows;
            pixels=MagickAllocateMemory(uint32 *,
                                        (number_pixels+6*image->columns)*sizeof(uint32));
            if (pixels == (uint32 *) NULL)
              {
                TIFFClose(tiff);
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
              }
            if (!TIFFReadRGBAImage(tiff,(uint32) image->columns,
                                   (uint32) image->rows,
                                   pixels+image->columns*sizeof(uint32),0))
              {
                MagickFreeMemory(pixels);
                TIFFClose(tiff);
                return ((Image *) NULL);
              }
            /*
              Convert image to DirectClass pixel packets.
            */
            p=pixels+number_pixels+image->columns*sizeof(uint32)-1;
            for (y=0; y < image->rows; y++)
              {
                q=SetImagePixels(image,0,y,image->columns,1);
                if (q == (PixelPacket *) NULL)
                  {
                    CopyException(exception,&image->exception);
                    status=MagickFail;
                    break;
                  }
                q+=image->columns-1;
                if (image->matte)
                  for (x=(long) image->columns; x != 0; x--)
                    {
                      q->red=ScaleCharToQuantum(TIFFGetR(*p));
                      q->green=ScaleCharToQuantum(TIFFGetG(*p));
                      q->blue=ScaleCharToQuantum(TIFFGetB(*p));
                      q->opacity=(Quantum) ScaleCharToQuantum(TIFFGetA(*p));
                      p--;
                      q--;
                    }
                else
                  for (x=(long) image->columns; x != 0; x--)
                    {
                      q->red=ScaleCharToQuantum(TIFFGetR(*p));
                      q->green=ScaleCharToQuantum(TIFFGetG(*p));
                      q->blue=ScaleCharToQuantum(TIFFGetB(*p));
                      p--;
                      q--;
                    }
                /*
                  Disassociate alpha from pixels if necessary.
                */
                if ((image->matte) && (alpha_type == AssociatedAlpha))
                  DisassociateAlphaRegion(image);
                if (!SyncImagePixels(image))
                  {
                    CopyException(exception,&image->exception);
                    status=MagickFail;
                    break;
                  }
                if (image->previous == (Image *) NULL)
                  if (QuantumTick(y,image->rows))
                    if (!MagickMonitorFormatted(y,image->rows,exception,
                                                LoadImageText,image->filename,
						image->columns,image->rows))
                      {
                        status=MagickFail;
                        break;
                      }
                if (status == MagickFail)
                  break;
              }
            MagickFreeMemory(pixels);
            break;
          }
        }

    read_next_frame:
      if (status == MagickPass)
        {
          if (image->depth > QuantumDepth)
            image->depth=QuantumDepth;
          if ((photometric == PHOTOMETRIC_LOGL) ||
              (photometric == PHOTOMETRIC_MINISBLACK) ||
              (photometric == PHOTOMETRIC_MINISWHITE))
            image->is_grayscale=MagickTrue;
          if ((image->is_grayscale == MagickTrue) && (bits_per_sample == 1))
            image->is_monochrome=MagickTrue;
          /*
            Proceed to next image.
          */
          if (image_info->subrange != 0)
            if (image->scene >= (image_info->subimage+image_info->subrange-1))
              break;
          more_frames=TIFFReadDirectory(tiff);
          if (more_frames)
            {
              /*
                Allocate next image structure.
              */
              AllocateNextImage(image_info,image);
              if (image->next == (Image *) NULL)
                {
                  DestroyImageList(image);
                  return((Image *) NULL);
                }
              image=SyncNextImageInList(image);
              status=MagickMonitorFormatted(image->scene-1,image->scene,
                                            &image->exception,
                                            LoadImageText,image->filename,
					    image->columns,image->rows);
            }
        }
      
      if (status == MagickFail)
        break;

    } while ((status == MagickPass) && (more_frames));
  TIFFClose(tiff);
  if (status == MagickFail)
      DeleteImageFromList(&image);
  return GetFirstImageInList(image);
}
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r T I F F I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterTIFFImage adds attributes for the TIFF image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterTIFFImage method is:
%
%      void RegisterTIFFImage(void)
%
*/
ModuleExport void
RegisterTIFFImage(void)
{
#define BIGTIFFDescription  "Tagged Image File Format (64-bit offsets)"
#define GROUP4RAWDescription "CCITT Group4 RAW"
#define PTIFDescription "Pyramid encoded TIFF"
#define TIFFDescription  "Tagged Image File Format"
#if defined(HasTIFF)

  static char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  /*
    Initialize thread specific data key.
  */
  if (tsd_key == (MagickTsdKey_t) 0)
    (void) MagickTsdKeyCreate(&tsd_key);

  version[0]='\0';
  {
    int
      i;

    const char
      *p;

    /* TIFFGetVersion() is in libtiff 3.5.3 and later */
    for (p=TIFFGetVersion(), i=0;
          (i < MaxTextExtent-1) && (*p != 0) && (*p != '\n');
          p++, i++)
      version[i] = *p;
    version[i]=0;
  }

  /*
    Big TIFF (64-bit offsets)
  */
#if defined(HasBigTIFF)
  entry=SetMagickInfo("BIGTIFF");
  entry->thread_support=MagickFalse; /* libtiff uses libjpeg which is not thread safe */
  entry->decoder=(DecoderHandler) ReadTIFFImage;
  entry->encoder=(EncoderHandler) WriteTIFFImage;
  entry->seekable_stream=MagickTrue;
  entry->description=BIGTIFFDescription;
  entry->module="TIFF";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);
#endif /* defined(HasBigTIFF) */

  /*
    CCITT Group4 RAW encoded page.
  */
  entry=SetMagickInfo("GROUP4RAW");
  entry->thread_support=MagickTrue;
  /* entry->decoder=(DecoderHandler) ReadGROUP4RAWImage; */
  entry->encoder=(EncoderHandler) WriteGROUP4RAWImage;
  entry->raw=MagickTrue;
  entry->adjoin=MagickFalse;
  entry->seekable_stream=MagickFalse;
  entry->extension_treatment=IgnoreExtensionTreatment;
  entry->stealth=MagickTrue; /* Don't list in '-list format' output */
  entry->description=GROUP4RAWDescription;
  entry->module="TIFF";
  (void) RegisterMagickInfo(entry);

  /*
    Pyramid TIFF (sequence of successively smaller versions of the same image)
  */
  entry=SetMagickInfo("PTIF");
  entry->thread_support=MagickFalse; /* libtiff uses libjpeg which is not thread safe */
  entry->decoder=(DecoderHandler) ReadTIFFImage;
  entry->encoder=(EncoderHandler) WritePTIFImage;
  entry->seekable_stream=MagickTrue;
  entry->description=PTIFDescription;
  entry->module="TIFF";
  (void) RegisterMagickInfo(entry);

  /*
    Another name for 32-bit TIFF
  */
  entry=SetMagickInfo("TIF");
  entry->thread_support=MagickFalse; /* libtiff uses libjpeg which is not thread safe */
  entry->decoder=(DecoderHandler) ReadTIFFImage;
  entry->encoder=(EncoderHandler) WriteTIFFImage;
  entry->seekable_stream=MagickTrue;
  entry->description=TIFFDescription;
  if (*version != '\0')
    entry->version=version;
  entry->stealth=MagickTrue; /* Don't list in '-list format' output */
  entry->module="TIFF";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

  /*
    Traditional 32-bit TIFF
  */
  entry=SetMagickInfo("TIFF");
  entry->thread_support=MagickFalse; /* libtiff uses libjpeg which is not thread safe */
  entry->decoder=(DecoderHandler) ReadTIFFImage;
  entry->encoder=(EncoderHandler) WriteTIFFImage;
  entry->magick=(MagickHandler) IsTIFF;
  entry->seekable_stream=MagickTrue;
  entry->description=TIFFDescription;
  if (*version != '\0')
    entry->version=version;
  entry->module="TIFF";
  entry->coder_class=PrimaryCoderClass;
  (void) RegisterMagickInfo(entry);

#if defined(EXTEND_TIFF_TAGS)
  /*
    Add our own TIFF tag extensions.
  */
  ExtensionTagsInitialize();
#endif /* defined(EXTEND_TIFF_TAGS) */


#endif /* if defined(HasTIFF) */
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r T I F F I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterTIFFImage removes format registrations made by the
%  TIFF module from the list of supported formats.
%
%  The format of the UnregisterTIFFImage method is:
%
%      void UnregisterTIFFImage(void)
%
*/
ModuleExport void
UnregisterTIFFImage(void)
{
#if defined(HasTIFF)
#if defined(HasBigTIFF)
  (void) UnregisterMagickInfo("BIGTIFF");
#endif /* defined(HasBigTIFF) */
  (void) UnregisterMagickInfo("GROUP4RAW");
  (void) UnregisterMagickInfo("PTIF");
  (void) UnregisterMagickInfo("TIF");
  (void) UnregisterMagickInfo("TIFF");

  /*
    Destroy thread specific data key.
  */
  if (tsd_key != (MagickTsdKey_t) 0)
    {
      (void) MagickTsdKeyDelete(tsd_key);
      tsd_key = (MagickTsdKey_t) 0;
    }
#endif
}

#if defined(HasTIFF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e G R O U P 4 R A W I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteGROUP4RAWImage writes an image as raw Group4 compressed data.
%
%  The format of the WriteGROUP4RAWImage method is:
%
%      MagickPassFail WriteGROUP4RAWImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method WriteGROUP4RAWImage return True if the image is written.
%      False is returned is there is of a memory shortage or if the image
%      file cannot be opened for writing.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static MagickPassFail
WriteGROUP4RAWImage(const ImageInfo *image_info,Image *image)
{
  char
    temporary_filename[MaxTextExtent];

  Image
    *huffman_image;

  ImageInfo
    *clone_info;

  TIFF
    *tiff;

  toff_t
    *byte_counts,
    count,
    strip_size;

  unsigned char
    *strip;

 unsigned int
   i;

  MagickPassFail
    status;

  /*
    Write image as CCITTFax4 TIFF image.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if(!AcquireTemporaryFileName(temporary_filename))
    ThrowWriterException(FileOpenError,UnableToCreateTemporaryFile,image);

  huffman_image=CloneImage(image,0,0,True,&image->exception);
  if (huffman_image == (Image *) NULL)
    return(False);

  (void) SetImageType(huffman_image,BilevelType);
  FormatString(huffman_image->filename,"tiff:%s",temporary_filename);
  clone_info=CloneImageInfo((const ImageInfo *) NULL);
/*   clone_info->blob=0; */
  clone_info->compression=Group4Compression;
  clone_info->type=BilevelType;
  (void) AddDefinitions(clone_info,"tiff:strip-per-page=TRUE",
                        &image->exception);
  (void) AddDefinitions(clone_info,"tiff:fill-order=msb2lsb",
                        &image->exception);
  status=WriteImage(clone_info,huffman_image);
  if (status == MagickFail)
    CopyException(&image->exception,&huffman_image->exception);
  DestroyImageInfo(clone_info);
  DestroyImage(huffman_image);
  if (status == MagickFail)
    {
      (void) LiberateTemporaryFile(temporary_filename);
      return MagickFail;
    }

  (void) MagickTsdSetSpecific(tsd_key,(void *) (&image->exception));
  (void) TIFFSetErrorHandler((TIFFErrorHandler) TIFFErrors);
  (void) TIFFSetWarningHandler((TIFFErrorHandler) (CheckThrowWarnings(image_info) ?
                                                   TIFFWarningsThrowException :
                                                   TIFFWarningsLogOnly));

  tiff=TIFFOpen(temporary_filename,"rb");
  if (tiff == (TIFF *) NULL)
    {
      (void) LiberateTemporaryFile(temporary_filename);
      return MagickFail;
    }

  /*
    Allocate raw strip buffer.
  */
  if (TIFFGetField(tiff,TIFFTAG_STRIPBYTECOUNTS,&byte_counts) != 1)
    {
      TIFFClose(tiff);
      (void) LiberateTemporaryFile(temporary_filename);
      return MagickFail;
    }
  strip_size=byte_counts[0];
  for (i=1; i < TIFFNumberOfStrips(tiff); i++)
    if (byte_counts[i] > strip_size)
      strip_size=byte_counts[i];
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Allocating %lu bytes of memory for TIFF strip",
                        (unsigned long) strip_size);
  strip=MagickAllocateMemory(unsigned char *,(size_t) strip_size);
  if (strip == (unsigned char *) NULL)
    {
      TIFFClose(tiff);
      (void) LiberateTemporaryFile(temporary_filename);
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
			   image);
    }
  /*
    Open blob for output
  */
  if ((status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception))
      == MagickFail)
    {
      MagickFreeMemory(strip);
      TIFFClose(tiff);
      (void) LiberateTemporaryFile(temporary_filename);
      ThrowWriterException(FileOpenError,UnableToOpenFile,image);
    }

  /*
    Compress runlength encoded to 2D Huffman pixels.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			"Output 2D Huffman pixels.");
  for (i=0; i < TIFFNumberOfStrips(tiff); i++)
  {
    count=TIFFReadRawStrip(tiff,(uint32) i,strip,strip_size);
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			  "Writing strip %u (%lu bytes) to blob ...",
			  i,(unsigned long) count);
    if ((toff_t) WriteBlob(image,count,strip) != count)
      status=MagickFail;
  }

  MagickFreeMemory(strip);
  TIFFClose(tiff);

  (void) LiberateTemporaryFile(temporary_filename);
  CloseBlob(image);
  return status;
}
#endif /* if defined(HasTIFF) */

#if defined(HasTIFF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P T I F I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WritePTIFImage() writes an image in the pyrimid-encoded Tagged image file
%  format.
%
%  The format of the WritePTIFImage method is:
%
%      MagickPassFail WritePTIFImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method WritePTIFImage return MagickPass if the image is written.
%      MagickFail is returned is there is of a memory shortage or if the image
%      file cannot be opened for writing.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/
static MagickPassFail
WritePTIFImage(const ImageInfo *image_info,Image *image)
{
  Image
    *pyramid_image;

  ImageInfo
    *clone_info;

  FilterTypes
    filter;

  unsigned int
    status;

  /*
    Create pyramid-encoded TIFF image.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  filter=TriangleFilter;
  if (image->is_monochrome)
    filter=PointFilter;
  pyramid_image=CloneImage(image,0,0,True,&image->exception);
  if (pyramid_image == (Image *) NULL)
    ThrowWriterException2(FileOpenError,image->exception.reason,image);
  do
    {
      pyramid_image->next=ResizeImage(image,pyramid_image->columns/2,
                                      pyramid_image->rows/2,filter,
                                      1.0,&image->exception);
      if (pyramid_image->next == (Image *) NULL)
        ThrowWriterException2(FileOpenError,image->exception.reason,image);
      if ((!image->is_monochrome) && (image->storage_class == PseudoClass))
        (void) MapImage(pyramid_image->next,image,False);
      pyramid_image->next->x_resolution=pyramid_image->x_resolution/2;
      pyramid_image->next->y_resolution=pyramid_image->y_resolution/2;
      pyramid_image->next->previous=pyramid_image;
      pyramid_image=pyramid_image->next;
    } while ((pyramid_image->columns > 64) && (pyramid_image->rows > 64));
  while (pyramid_image->previous != (Image *) NULL)
    pyramid_image=pyramid_image->previous;
  /*
    Write pyramid-encoded TIFF image.
  */
  clone_info=CloneImageInfo(image_info);
  clone_info->adjoin=True;
  (void) strlcpy(clone_info->magick,"TIFF",MaxTextExtent);
  (void) strlcpy(image->magick,"TIFF",MaxTextExtent);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "Invoking \"%.1024s\" encoder, monochrome=%s, grayscale=%s",
                        "TIFF",
                        MagickBoolToString(image->is_monochrome),
                        MagickBoolToString(image->is_grayscale));
  status=WriteTIFFImage(clone_info,pyramid_image);
  DestroyImageList(pyramid_image);
  DestroyImageInfo(clone_info);
  return(status);
}
#endif /* defined(HasTIFF) */

#if defined(HasTIFF)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e T I F F I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteTIFFImage writes an image in the Tagged image file format.
%
%  The format of the WriteTIFFImage method is:
%
%      MagickPassFail WriteTIFFImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method WriteTIFFImage return True if the image is written.
%      False is returned is there is of a memory shortage or if the image
%      file cannot be opened for writing.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/

static void
WriteNewsProfile(TIFF *tiff,
		 int profile_tag,
		 const unsigned char *profile_data,
		 const size_t profile_length)
{
  unsigned char
    *profile=0;

  uint32
    length;

  assert(tiff != (TIFF *) NULL);
  assert(profile_tag != 0);
  assert(profile_data != (const unsigned char *) NULL);

  length = (uint32) profile_length;
  if (length == 0)
    return;

  if (profile_tag == TIFFTAG_RICHTIFFIPTC)
    {
      /*
        Handle TIFFTAG_RICHTIFFIPTC tag.
      */
      length += (4-(length & 0x03)); /* Round up for long word alignment */
      profile=MagickAllocateMemory(unsigned char *,length);
      if (profile == (unsigned char *) NULL)
        return;
      (void) memset(profile,0,length);
      (void) memcpy(profile,profile_data,profile_length);

      if (TIFFIsByteSwapped(tiff))
        TIFFSwabArrayOfLong((uint32 *) profile,length/4);

      /* Tag is type TIFF_LONG so byte length is divided by four */
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "TIFFSetField(tiff=0x%p,tag=%d,length=%lu,data=0x%p)",
                            tiff,profile_tag,(unsigned long) length/4,profile);
      (void) TIFFSetField(tiff,profile_tag,(uint32) length/4,(void *) profile);
    }
  else if (profile_tag == TIFFTAG_PHOTOSHOP)
    {
      /*
        Handle TIFFTAG_PHOTOSHOP tag.
      */
      length += (length & 0x01); /* Round up for Photoshop */
#if defined(GET_ONLY_IPTC_DATA)
      length += 12;              /* Space for 8BIM header */
      profile=MagickAllocateMemory(unsigned char *,length);
      if (profile == (unsigned char *) NULL)
        return;
      (void) memset(profile,0,length);
      (void) memcpy(profile,"8BIM\04\04\0\0",8);
      profile[8]=(length >> 24) & 0xff;
      profile[9]=(length >> 16) & 0xff;
      profile[10]=(length >> 8) & 0xff;
      profile[11]=length & 0xff;
      (void) memcpy(profile+12,profile_data,profile_length);
#else
      profile=MagickAllocateMemory(unsigned char *,length);
      if (profile == (unsigned char *) NULL)
        return;
      (void) memset(profile,0,length);
      (void) memcpy(profile,profile_data,profile_length);
#endif
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "TIFFSetField(tiff=0x%p,tag=%d,length=%lu,data=0x%p)",
                            tiff,profile_tag,(unsigned long) length,profile);
      (void) TIFFSetField(tiff,profile_tag,(uint32) length,(void *) profile);
    }

  MagickFreeMemory(profile);
}

#if !defined(TIFFDefaultStripSize)
#define TIFFDefaultStripSize(tiff,request)  ((8*1024)/TIFFScanlineSize(tiff))
#endif
static MagickPassFail
WriteTIFFImage(const ImageInfo *image_info,Image *image)
{
  char
    filename[MaxTextExtent],
    open_flags[MaxTextExtent];

  const ImageAttribute
    *attribute;

  unsigned int
    x,
    y;

  register unsigned int
    i;

  TIFF
    *tiff;

  uint16
    bits_per_sample,
    compress_tag,
    fill_order,
    photometric,
    planar_config,
    predictor,
    sample_format,
    samples_per_pixel;

  uint32
    rows_per_strip;

  tsize_t
    scanline_size;

  AlphaType
    alpha_type;

  CompressionType
    compression=UndefinedCompression;

  TIFFMethod
    method;

  Magick_TIFF_ClientData
    client_data;

  ExportPixelAreaOptions
    export_options;

  ExportPixelAreaInfo
    export_info;

  MagickBool
    logging=MagickFalse;

  MagickPassFail
    status;

  unsigned long
    depth,
    scene;

  /*
    Open TIFF file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  logging=IsEventLogging();
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFail)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  (void) MagickTsdSetSpecific(tsd_key,(void *) (&image->exception));
  (void) TIFFSetErrorHandler((TIFFErrorHandler) TIFFErrors);
  (void) TIFFSetWarningHandler((TIFFErrorHandler) (CheckThrowWarnings(image_info) ?
                                                   TIFFWarningsThrowException :
                                                   TIFFWarningsLogOnly));
  (void) strlcpy(filename,image->filename,MaxTextExtent);
  /*
    Open TIFF file

    'w'  open for write
    'l'  force little-endian byte order
    'b'  force big-endian byte order
    'L'  force LSB to MSB bit order (weird)
    'B'  force MSB to LSB bit order (normal)
    '8'  64-bit offsets (BigTIFF)
  */
  (void) strlcpy(open_flags, "w", sizeof(open_flags));
  switch (image_info->endian)
    {
    case LSBEndian:
      (void) strlcat(open_flags, "l", sizeof(open_flags));
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "Using little endian byte order");
      break;
    case MSBEndian:
      (void) strlcat(open_flags, "b", sizeof(open_flags));
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "Using big endian byte order");
      break;
    default:
    case UndefinedEndian:
      {
        /* Default is native byte order */
        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Using native endian byte order");
      }
    }

#if defined(HasBigTIFF)
  if (strcmp(image_info->magick,"BIGTIFF") == 0)
    {
      (void) strlcat(open_flags, "8", sizeof(open_flags));
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "Using 64-bit offsets (BigTIFF format)");
    }
#endif

  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "Opening TIFF file \"%s\" using open flags \"%s\".",
                          filename,open_flags);
  client_data.image=image;
  client_data.image_info=image_info;
  tiff=TIFFClientOpen(filename,open_flags,(thandle_t) &client_data,
                      TIFFReadBlob,TIFFWriteBlob,TIFFSeekBlob,
                      TIFFCloseBlob,TIFFGetBlobSize,TIFFMapBlob,
                      TIFFUnmapBlob);
  if (tiff == (TIFF *) NULL)
    {
      /* CloseBlob(image); */
      return(MagickFail);
    }
  scene=0;
  do
    {
      ImageCharacteristics
        characteristics;

      /*
        Initialize TIFF fields.
      */
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLESPERPIXEL,
                                   &samples_per_pixel);
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,
                                   &bits_per_sample);
      (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLEFORMAT,&sample_format);
      if (LocaleCompare(image_info->magick,"PTIF") == 0)
        if (image->previous != (Image *) NULL)
          (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_REDUCEDIMAGE);
      (void) TIFFSetField(tiff,TIFFTAG_IMAGELENGTH,(uint32) image->rows);
      (void) TIFFSetField(tiff,TIFFTAG_IMAGEWIDTH,(uint32) image->columns);

      ExportPixelAreaOptionsInit(&export_options);
      depth=image->depth;
      bits_per_sample=8;
      predictor=0U;
      method=ScanLineMethod;
      if ((AccessDefinition(image_info,"tiff","tile")) ||
          (AccessDefinition(image_info,"tiff","tile-geometry")) ||
          (AccessDefinition(image_info,"tiff","tile-width")) ||
          (AccessDefinition(image_info,"tiff","tile-height")))
        method=TiledMethod;

      /*
        Decide how to compress the image.
      */
      compression=image->compression;
      if (image_info->compression != UndefinedCompression)
        compression=image_info->compression;
      if (UndefinedCompression == compression)
        compression=NoCompression;

      /*
        Ensure that only supported compression types are requested.
      */
      {
        char
          compression_name[MaxTextExtent];

        if (CompressionSupported(compression,compression_name) != MagickTrue)
          {
            compression=NoCompression;
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "%s compression not supported.  "
				    "Compression request removed",
                                    compression_name);
          }
      }

      /*
        Determine libtiff compression settings.
      */
      compress_tag=COMPRESSION_NONE;
      fill_order=FILLORDER_MSB2LSB;
      switch (compression)
        {
          /*
            Note that RFC 2301 recommends using LSB2MSB fill order for FAX
            since it is the transmission order used on the wire for FAX.
            However, it also states that all conforming readers should be
            able to read data in both bit orders.
          */
        case FaxCompression:
          {
            compress_tag=COMPRESSION_CCITTFAX3;
            fill_order=FILLORDER_LSB2MSB;
            break;
          }
        case Group4Compression:
          {
            compress_tag=COMPRESSION_CCITTFAX4;
            fill_order=FILLORDER_LSB2MSB;
            break;
          }
#if defined(COMPRESSION_JBIG)
        case JBIG1Compression:
          {
            compress_tag=COMPRESSION_JBIG;
	    fill_order=FILLORDER_LSB2MSB;
            break;
          }
#endif /* defined(COMPRESSION_JBIG) */
        case JPEGCompression:
          {
            compress_tag=COMPRESSION_JPEG;
            break;
          }
        case LZWCompression:
          {
            compress_tag=COMPRESSION_LZW;
            break;
          }
#if defined(COMPRESSION_LZMA)
        case LZMACompression:
          {
            compress_tag=COMPRESSION_LZMA;
            break;
          }
#endif /* defined(COMPRESSION_LZMA) */
        case RLECompression:
          {
            compress_tag=COMPRESSION_PACKBITS;
            break;
          }
        case ZipCompression:
          {
            compress_tag=COMPRESSION_ADOBE_DEFLATE;
            break;
          }
        default:
          {
            compress_tag=COMPRESSION_NONE;
            break;
          }
        }

      /*
        Ensure that image is in desired output space
      */
      if ((image_info->type != UndefinedType) &&
          (image_info->type != OptimizeType))
        (void) SetImageType(image,image_info->type);
      else if (!IsCMYKColorspace(image->colorspace) &&
               (!IsRGBColorspace(image->colorspace)))
        (void) TransformColorspace(image,RGBColorspace);

      /*
        Analyze image to be written.
      */
      if (!GetImageCharacteristics(image,&characteristics,
                                   (OptimizeType == image_info->type),
                                   &image->exception))
        {
          status=MagickFail;
          break;
        }

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "Image characteristics: cmyk=%c, gray=%c, mono=%c,"
			    " opaque=%c, palette=%c",
                            (characteristics.cmyk ? 'y' : 'n'),
                            (characteristics.grayscale ? 'y' : 'n'),
                            (characteristics.monochrome ? 'y' : 'n'),
                            (characteristics.opaque ? 'y' : 'n'),
                            (characteristics.palette ? 'y' : 'n'));

      /*
        Choose best photometric.
      */
      if (characteristics.cmyk)
        {
          photometric=PHOTOMETRIC_SEPARATED;
        }
      else if (characteristics.monochrome)
        {
          photometric=PHOTOMETRIC_MINISWHITE;
        }
      else if (characteristics.palette)
        {
          photometric=PHOTOMETRIC_PALETTE;
        }
      else if (characteristics.grayscale)
        {
          photometric=PHOTOMETRIC_MINISBLACK;
        }
      else
        {
          photometric=PHOTOMETRIC_RGB;
        }

      /*
        Currently we only support JPEG compression with MINISWHITE,
        MINISBLACK, and RGB.  FAX compression types require
        MINISWHITE.  CMYK takes precedence over JPEG compression.
      */
      if ((compress_tag == COMPRESSION_JPEG) &&
	  (photometric == PHOTOMETRIC_PALETTE))
        {
          photometric=PHOTOMETRIC_RGB;
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Using RGB photometric due to request for"
				  " JPEG compression.");
        }
      else if (compress_tag == COMPRESSION_CCITTFAX3)
        {
          photometric=PHOTOMETRIC_MINISWHITE;
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Using MINISWHITE photometric due to request"
				  " for Group3 FAX compression.");
        }
      else if (compress_tag == COMPRESSION_CCITTFAX4)
        {
          photometric=PHOTOMETRIC_MINISWHITE;
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Using MINISWHITE photometric due to request"
				  " for Group4 FAX compression.");
        }
#if defined(COMPRESSION_JBIG)
      else if (compress_tag == COMPRESSION_JBIG)
        {
          photometric=PHOTOMETRIC_MINISWHITE;
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Using MINISWHITE photometric due to request"
				  " for JBIG compression.");
        }
#endif /* defined(COMPRESSION_JBIG) */

      /*
        Allow user to override the photometric.
      */
      switch (image_info->type)
        {
        case BilevelType:
          {
            photometric=PHOTOMETRIC_MINISWHITE;
            break;
          }
        case GrayscaleType:
          {
            photometric=PHOTOMETRIC_MINISBLACK;
            break;
          }
        case GrayscaleMatteType:
          {
            photometric=PHOTOMETRIC_MINISBLACK;
            if (!image->matte)
              SetImageOpacity(image,OpaqueOpacity);
            break;
          }
        case PaletteType:
          {
            photometric=PHOTOMETRIC_PALETTE;
            break;
          }
        case PaletteMatteType:
          {
            photometric=PHOTOMETRIC_PALETTE;
            if (!image->matte)
              SetImageOpacity(image,OpaqueOpacity);
            break;
          }
        case TrueColorType:
          {
            photometric=PHOTOMETRIC_RGB;
            break;
          }
        case TrueColorMatteType:
          {
            photometric=PHOTOMETRIC_RGB;
            if (!image->matte)
              SetImageOpacity(image,OpaqueOpacity);
            break;
          }
        case ColorSeparationType:
          {
            photometric=PHOTOMETRIC_SEPARATED;
            break;
          }
        case ColorSeparationMatteType:
          {
            photometric=PHOTOMETRIC_SEPARATED;
            if (!image->matte)
              SetImageOpacity(image,OpaqueOpacity);
            break;
          }
        case UndefinedType:
        case OptimizeType:
          {
            /* No special handling */
          }
        }

      /*
        If the user has selected something other than MINISWHITE,
        MINISBLACK, or RGB, then remove JPEG compression.  Also remove
        fax compression if photometric is not compatible.
      */
      if ((compress_tag == COMPRESSION_JPEG) &&
          ((photometric != PHOTOMETRIC_MINISWHITE) &&
	   (photometric != PHOTOMETRIC_MINISBLACK) &&
	   (photometric != PHOTOMETRIC_RGB) &&
           (photometric != PHOTOMETRIC_YCBCR)))
        {
          compress_tag=COMPRESSION_NONE;
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Ignoring request for JPEG compression due "
				  "to incompatible photometric.");
        }
      else if ((compress_tag == COMPRESSION_CCITTFAX3) &&
               (photometric != PHOTOMETRIC_MINISWHITE))
        {
          compress_tag=COMPRESSION_NONE;
          fill_order=FILLORDER_MSB2LSB;
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Ignoring request for Group3 FAX compression"
				  " due to incompatible photometric.");
        }
      else if ((compress_tag == COMPRESSION_CCITTFAX4) &&
               (photometric != PHOTOMETRIC_MINISWHITE))
        {
          compress_tag=COMPRESSION_NONE;
          fill_order=FILLORDER_MSB2LSB;
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Ignoring request for Group4 FAX compression"
				  " due to incompatible photometric.");
        }
#if defined(COMPRESSION_JBIG)
      else if ((compress_tag == COMPRESSION_JBIG) &&
               (photometric != PHOTOMETRIC_MINISWHITE))
        {
          compress_tag=COMPRESSION_NONE;
          fill_order=FILLORDER_MSB2LSB;
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Ignoring request for JBIG compression"
				  " due to incompatible photometric.");
        }
#endif /* defined(COMPRESSION_JBIG) */

      /*
        Bilevel presents a bit of a quandary since the user is free to
        change the type so we don't want to set depth in advance.  So
        we will intuit the depth here.  For the moment, we assume that
        if the photometric is PHOTOMETRIC_MINISWHITE that we are
        probably outputting bilevel.  Note that the user is still able
        to override bits_per_sample.
      */
      if (PHOTOMETRIC_MINISWHITE == photometric)
        depth=1;

      /*
        Support writing bits per sample of 8, 16, & 32 by default.
      */
      for (bits_per_sample=8; bits_per_sample < depth; )
        bits_per_sample*=2;

      /*
        Now chose appropriate settings for the photometric.
      */
      switch (photometric)
        {
        case PHOTOMETRIC_MINISWHITE:
        case PHOTOMETRIC_MINISBLACK:
          {
            samples_per_pixel=1;
            if (depth == 1)
              bits_per_sample=1;
            break;
          }
        case PHOTOMETRIC_RGB:
          {
            samples_per_pixel=3;
            break;
          }
        case PHOTOMETRIC_PALETTE:
          {
            samples_per_pixel=1;
            bits_per_sample=1;
            /*
              Support colormap indexes of 1, 2, 4, 8, and 16 by default.
            */
            while ((1UL << bits_per_sample) < image->colors)
              bits_per_sample*=2;
            break;
          }
        case PHOTOMETRIC_SEPARATED:
          {
            samples_per_pixel=4; /* CMYK */

            (void) TIFFSetField(tiff,TIFFTAG_INKSET,INKSET_CMYK);
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using INKSET_CMYK");
            break;
          }
        case PHOTOMETRIC_YCBCR:
          {
            /* This is here to support JPEGCOLORMODE_RGB*/
            samples_per_pixel=3;
            break;
          }
        }

      if (COMPRESSION_JPEG == compress_tag)
        {
          /*
            JPEG compression can only use size specified by BITS_IN_JSAMPLE.
            FIXME
          */
#if BITS_IN_JSAMPLE == 12
          depth=12;
          bits_per_sample=12;
#else
          depth=8;
          bits_per_sample=8;
#endif
        }

      alpha_type=UnspecifiedAlpha;
      if (image->matte)
        {
          /*
            Image has a matte channel.  Mark it correctly.
          */
          uint16
            extra_samples,
            sample_info[1];

          const char *
            value;

          if ((value=AccessDefinition(image_info,"tiff","alpha")))
            {
              if (LocaleCompare(value,"unspecified") == 0)
                alpha_type=UnspecifiedAlpha;
              else if (LocaleCompare(value,"associated") == 0)
                alpha_type=AssociatedAlpha;
              else if (LocaleCompare(value,"unassociated") == 0)
                alpha_type=UnassociatedAlpha;
            }
          else if ((attribute=GetImageAttribute(image,"alpha")))
            {
              if (LocaleCompare(attribute->value,"unspecified") == 0)
                alpha_type=UnspecifiedAlpha;
              else if (LocaleCompare(attribute->value,"associated") == 0)
                alpha_type=AssociatedAlpha;
              else if (LocaleCompare(attribute->value,"unassociated") == 0)
                alpha_type=UnassociatedAlpha;
            }

          samples_per_pixel += 1;
          extra_samples=1;
          sample_info[0]=EXTRASAMPLE_ASSOCALPHA;
          switch (alpha_type)
            {
            case UnspecifiedAlpha:
              {
                sample_info[0]=EXTRASAMPLE_UNSPECIFIED;
                break;
              }
            case AssociatedAlpha:
              {
                sample_info[0]=EXTRASAMPLE_ASSOCALPHA;
                break;
              }
            case UnassociatedAlpha:
              {
                sample_info[0]=EXTRASAMPLE_UNASSALPHA;
                break;
              }
            }
            
          (void) TIFFSetField(tiff,TIFFTAG_EXTRASAMPLES,extra_samples,
                              &sample_info);
        }

      /*
        Allow the advanced user to over-ride some TIFF write options
      */
      {
        const char *
          value;

        unsigned int
          old_value;

        /*
          Fill order
        */
        value=AccessDefinition(image_info,"tiff","fill-order");
        if (value)
          {
            if (LocaleNCompare(value,"msb2lsb",3) == 0)
              fill_order=FILLORDER_MSB2LSB;
            else if (LocaleNCompare(value,"lsb2msb",3) == 0)
              fill_order=FILLORDER_LSB2MSB;
          }

        /*
          Sample format
        */
        value=AccessDefinition(image_info,"tiff","sample-format");
        if (value)
          {
            if (LocaleCompare(value,"unsigned") == 0)
              sample_format=SAMPLEFORMAT_UINT;
            else if (LocaleCompare(value,"ieeefp") == 0)
              sample_format=SAMPLEFORMAT_IEEEFP;
          }

        /*
          Bits per sample (needs to be after sample format)
        */
        value=AccessDefinition(image_info,"tiff","bits-per-sample");
        if (value)
          {
            old_value=bits_per_sample;
            bits_per_sample=MagickAtoI(value);
            if (sample_format == SAMPLEFORMAT_IEEEFP)
              {
                /*
                  If floating point is selected, ensure that valid
                  bits-per-sample values are specified.
                */
                if ((bits_per_sample != 16) &&
                    (bits_per_sample != 24) &&
                    (bits_per_sample != 32) &&
                    (bits_per_sample != 64))
                  bits_per_sample=32;
              }
            else
              {
                /* Clamp maximum unsigned bits per sample to 32 bits */
                if ((bits_per_sample < 1) ||
                    ((bits_per_sample > 32) && (bits_per_sample != 64)))
		  bits_per_sample=old_value;
              }
            if ((logging) && (old_value != bits_per_sample))
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "User override (bits-per-sample): %u bits "
				    "per sample (was %u)",
                                    (unsigned int) bits_per_sample, old_value);
          }

        /*
          Samples per pixel
        */
        value=AccessDefinition(image_info,"tiff","samples-per-pixel");
        if (value)
          {
            old_value=samples_per_pixel;
            samples_per_pixel=MagickAtoI(value);
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "User override (samples-per-pixel): %u "
				    "samples per pixel (was %u)",
                                    (unsigned int) samples_per_pixel, old_value);
          }
      }

      /*
        Determine planar configuration.
      */
      planar_config=PLANARCONFIG_CONTIG;
      if (samples_per_pixel > 1)
	{
	  switch (image->interlace)
	    {
	    case UndefinedInterlace:
	      break;
	    case NoInterlace:
	    case LineInterlace:
	    case PartitionInterlace:
	      planar_config=PLANARCONFIG_CONTIG;
	      break;
	    case PlaneInterlace:
	      planar_config=PLANARCONFIG_SEPARATE;
	      break;
	    }

	  switch (image_info->interlace)
	    {
	    case UndefinedInterlace:
	      break;
	    case NoInterlace:
	    case LineInterlace:
	    case PartitionInterlace:
	      planar_config=PLANARCONFIG_CONTIG;
	      break;
	    case PlaneInterlace:
	      planar_config=PLANARCONFIG_SEPARATE;
	      break;
	    }
	}

      /*
        YCbCr encoding compresses much better than RGB.  Use YCbCr
        encoding for JPEG (normal for JPEG files).

        FIXME: Need to find a way to select between RGB or YCbCr
        encoding with JPEG compression.  Testing shows that YCbCr is
        over 2.5X smaller than RGB but the PSNR is a bit lower,
        particularly in red and blue channels.  Someone might want to
        use RGB to minimize color loss.
      */
      if ((compress_tag == COMPRESSION_JPEG) &&
          (planar_config == PLANARCONFIG_CONTIG) &&
          (photometric == PHOTOMETRIC_RGB))
        photometric=PHOTOMETRIC_YCBCR;

      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "Using %s photometric, %u samples per pixel, "
			      "%u bits per sample, format %s",
                              PhotometricTagToString(photometric),
                              (unsigned int) samples_per_pixel,
                              (unsigned int) bits_per_sample,
                              sample_format == SAMPLEFORMAT_UINT ? "Unsigned" :
                              sample_format == SAMPLEFORMAT_IEEEFP ? "IEEEFP" :
                              "unknown");

      /*
        Only set fill order if the setting is not the default.
      */
      if (FILLORDER_MSB2LSB != fill_order)
        (void) TIFFSetField(tiff,TIFFTAG_FILLORDER,fill_order);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "Using %s bit fill order",
                              (fill_order == FILLORDER_MSB2LSB ? "MSB2LSB" :
                               (fill_order == FILLORDER_LSB2MSB ? "LSB2MSB" :
                                "undefined")));
      if (image->orientation != UndefinedOrientation)
        (void) TIFFSetField(tiff,TIFFTAG_ORIENTATION,(uint16) image->orientation);
      (void) TIFFSetField(tiff,TIFFTAG_PHOTOMETRIC,photometric);
      (void) TIFFSetField(tiff,TIFFTAG_BITSPERSAMPLE,bits_per_sample);
      (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,samples_per_pixel);
      (void) TIFFSetField(tiff,TIFFTAG_SAMPLEFORMAT,sample_format);
      (void) TIFFSetField(tiff,TIFFTAG_PLANARCONFIG,planar_config);
      if (logging)
	(void) LogMagickEvent(CoderEvent,GetMagickModule(),
			      "%s image planes",
			      (planar_config == PLANARCONFIG_SEPARATE ?
			       "Separate" : "Contiguous"));
      (void) TIFFSetField(tiff,TIFFTAG_COMPRESSION,compress_tag);

      /*
        Obtain recommended rows per strip given current image width,
        bits per sample, samples per pixel, tags, and compression
        specific requirements.  Tries to create strips with 8K of
        uncompressed data.
      */
      scanline_size=TIFFScanlineSize(tiff);
      rows_per_strip=TIFFDefaultStripSize(tiff,0);
      if (rows_per_strip < 1)
        rows_per_strip=1;
      /*
        It seems that some programs fail to handle more than 32K or
        64K strips in an image due to using a 16-bit strip counter.
        The solution is to use a larger strip size.  This approach
        might cause excessively large strips for mega-sized images and
        someday we may remove the solution since the problematic
        readers will have expired.
      */
      if ((image->rows/rows_per_strip) > 32767)
        rows_per_strip=image->rows/32768;
      if (rows_per_strip < 1)
        rows_per_strip=1;

      switch (compress_tag)
        {
        case COMPRESSION_JPEG:
          {
            /*
              RowsPerStrip must be multiple of 16 for JPEG.

	      RowsPerStrip is required to be a multiple of 8 times the
	      largest vertical sampling factor.  If YCbCr subsampling
	      is 2,2 then RowsPerStrip must be a multiple of 16.
            */
	    rows_per_strip=(((rows_per_strip < 16 ? 16 : rows_per_strip)+1)/16)*16;
            (void) TIFFSetField(tiff,TIFFTAG_JPEGQUALITY,image_info->quality);
            if (IsRGBColorspace(image->colorspace))
              (void) TIFFSetField(tiff,TIFFTAG_JPEGCOLORMODE,JPEGCOLORMODE_RGB);
            if (bits_per_sample == 12)
              (void) TIFFSetField(tiff,TIFFTAG_JPEGTABLESMODE,JPEGTABLESMODE_QUANT);
            break;
          }
        case COMPRESSION_ADOBE_DEFLATE:
          {
            /*
              Deflate strips compress better up to 32K of data
              (enlarge if necessary)..
            */
            unsigned int
              proposed_rows_per_strip;

            proposed_rows_per_strip = (uint32) (32*1024) / Max(scanline_size,1);
            if (proposed_rows_per_strip > rows_per_strip)
              rows_per_strip=proposed_rows_per_strip;
            /*
              Use horizontal differencing (type 2) for images which are
              likely to be continuous tone.  The TIFF spec says that this
              usually leads to better compression.
            */
            if (((photometric == PHOTOMETRIC_RGB) ||
                 (photometric == PHOTOMETRIC_MINISBLACK)) &&
                ((bits_per_sample == 8) || (bits_per_sample == 16)))
	      predictor=PREDICTOR_HORIZONTAL;
            {
              /*
                Zip quality has a useful range of 1-9.
              */
              unsigned int zip_quality=image_info->quality / 10;
              if (zip_quality < 1)
                zip_quality=1;
              if (zip_quality > 9)
                zip_quality=9;
              (void) TIFFSetField(tiff,TIFFTAG_ZIPQUALITY,zip_quality);
            }
            break;
          }
        case COMPRESSION_CCITTFAX3:
          {
            /*
              Set Group 3 Options.  Group 3 options are arranged as 32 flag bits.
              Specify byte-aligned EOL padding option.

              Group3Options = 4,5. LONG. Data may be one- or
              two-dimensional, but EOLs must be
              byte-aligned. Uncompressed data is not allowed.
              
              bit 0 = 0 for 1-Dimensional, 1 for 2-Dimensional
              
              bit 1 = must be 0 (uncompressed data not allowed)

              bit 2 = 1 for byte-aligned EOLs

            */
	    uint32
	      group_three_options = 4;

	    const char *
	      value;
	
	    if ((value=AccessDefinition(image_info,"tiff","group-three-options")))
	      {
		group_three_options=(uint32) strtol(value,(char **)NULL, 10);
	      }
	    (void) TIFFSetField(tiff,TIFFTAG_GROUP3OPTIONS,group_three_options);

            /*
              It is recommended (but not required) to output FAX as
              one strip. We will limit strip size to 16 megapixels by
              default.
            */
            rows_per_strip=16000000UL/image->columns;
            if (rows_per_strip < 1)
              rows_per_strip=1;
            if (rows_per_strip > image->rows)
              rows_per_strip=(uint32) image->rows;
            break;
          }
        case COMPRESSION_CCITTFAX4:
          {
            /*
              It is recommended (but not required) to output FAX as
              one strip. We will limit strip size to 16 megapixels by
              default.
            */
            rows_per_strip=16000000UL/image->columns;
            if (rows_per_strip < 1)
              rows_per_strip=1;
            if (rows_per_strip > image->rows)
              rows_per_strip=(uint32) image->rows;
            break;
          }
#if defined(COMPRESSION_LZMA)
        case COMPRESSION_LZMA:
          {
	    unsigned int
	      lzma_preset;

	    const char *
	      lzma_preset_str;

	    /*
	      Lzma preset has a useful range of 1-9.

	      We default to 1 since testing does not show much benefit
	      from use of larger values.  However, we allow the
	      power-user who wants to experiment to change the preset
	      value via syntax like '-define tiff:lzmapreset=7'.  This
	      ability is intentionally not documented other than here.
	    */

	    lzma_preset=1;
	    if ((lzma_preset_str=AccessDefinition(image_info,"tiff","lzmapreset")))
	      lzma_preset=(unsigned short) MagickAtoI(lzma_preset_str);

	    if (lzma_preset < 1)
	      lzma_preset=1;
	    if (lzma_preset > 9)
	      lzma_preset=9;
	    (void) TIFFSetField(tiff,TIFFTAG_LZMAPRESET,lzma_preset);
	    if (logging)
	      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				    "LZMA PRESET set to %u", lzma_preset);

	    {
	      /*
		Provide a default rows-per-strip which is suitably
		tailored for the compression level.
	      */
	      
	      /*
		Strip memory target for various compression preset levels.
		Values are arbitrary.  LZMA is a memory and CPU pig.
	      */
	      static const long
		lzma_memory_mb[] =
		{      /* Level  Compress  Decompress */
		  1,   /*   1       2 MB      1 MB    */
		  4 ,  /*   2      12 MB      2 MB    */
		  4,   /*   3      12 MB      1 MB    */
		  4,   /*   4      16 MB      2 MB    */
		  6,   /*   5      26 MB      3 MB    */
		  10,  /*   6      45 MB      5 MB    */
		  18,  /*   7      83 MB      9 MB    */
		  34,  /*   8     159 MB     17 MB    */
		  66   /*   9     311 MB     33 MB    */
		};
	      
	      rows_per_strip = (uint32) ((lzma_memory_mb[lzma_preset-1]*1024*1024))/
		(((bits_per_sample*samples_per_pixel)/8)*image->rows);
	      if (rows_per_strip < 1)
		rows_per_strip=1;
	      if (rows_per_strip > image->rows)
		rows_per_strip=image->rows;
	    }

            /*
              Use horizontal differencing (type 2) for images which are
              likely to be continuous tone.  The TIFF spec says that this
              usually leads to better compression.
            */
            if (((photometric == PHOTOMETRIC_RGB) ||
                 (photometric == PHOTOMETRIC_MINISBLACK)) &&
                ((bits_per_sample == 8) || (bits_per_sample == 16)))
	      predictor=PREDICTOR_HORIZONTAL;
            break;
          }
#endif /* COMPRESSION_LZMA */
#if defined(COMPRESSION_JBIG)
	case COMPRESSION_JBIG:
	  {
            /*
              It is recommended (but not required) to output FAX as
              one strip. We will limit strip size to 16 megapixels by
              default.
            */
            rows_per_strip=16000000UL/image->columns;
            if (rows_per_strip < 1)
              rows_per_strip=1;
            if (rows_per_strip > image->rows)
              rows_per_strip=(uint32) image->rows;
	    break;
	  }
#endif /* COMPRESSION_JBIG */
        case COMPRESSION_LZW:
          {
            /*
              Use horizontal differencing (type 2) for images which are
              likely to be continuous tone.  The TIFF spec says that this
              usually leads to better compression.
            */
            if (((photometric == PHOTOMETRIC_RGB) ||
                 (photometric == PHOTOMETRIC_MINISBLACK)) &&
                ((bits_per_sample == 8) || (bits_per_sample == 16)))
	      predictor=PREDICTOR_HORIZONTAL;
            break;
          }
        default:
          {
            break;
          }
        }

      /*
	Allow the user to specify the predictor (at their own peril)
      */
      {
	const char *
            value;
	
	if ((value=AccessDefinition(image_info,"tiff","predictor")))
	  predictor=(unsigned short) MagickAtoI(value);
      }

      if (predictor != 0)
	{
	  if (logging)
	    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				  "Requesting predictor %u",
				  (unsigned int) predictor);
	  (void) TIFFSetField(tiff,TIFFTAG_PREDICTOR,predictor);
	}

      /*
        Allow the user to override rows-per-strip settings.
      */
      if (method != TiledMethod)
        {
          const char *
            value;

          if ((value=AccessDefinition(image_info,"tiff","rows-per-strip")))
            {
              unsigned int
                old_value;
              
              old_value=rows_per_strip;
              rows_per_strip=MagickAtoI(value);
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "User override (rows_per_strip): %u rows"
				      " per strip (was %u)",
                                      (unsigned int) rows_per_strip, old_value);
            }
          if ((value=AccessDefinition(image_info,"tiff","strip-per-page")))
            {
              if (LocaleCompare("TRUE",value) == 0)
                {
                  rows_per_strip=(uint32) image->rows;
                  
                  if (logging)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "User requested a single strip per "
					  "page (strip-per-page)");
                }
            }
        }

      if (COMPRESSION_JPEG == compress_tag)
	{
	  /*
	    RowsPerStrip must be multiple of 16 for JPEG.
	  */
	  rows_per_strip=(((rows_per_strip < 16 ? 16 : rows_per_strip)+1)/16)*16;
	}

      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "Using %s compression",
			      CompressionTagToString(compress_tag));
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "Image depth %lu bits",depth);
      if (method != TiledMethod)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Rows per strip: %u (%lu bytes/strip)",
                                (unsigned int) rows_per_strip,
                                (unsigned long) scanline_size*rows_per_strip);
          (void) TIFFSetField(tiff,TIFFTAG_ROWSPERSTRIP,rows_per_strip);
        }


      if ((image->x_resolution != 0) && (image->y_resolution != 0))
        {
          unsigned short
            units;

          /*
            Set image resolution.
          */
          units=RESUNIT_NONE;
          if (image->units == PixelsPerInchResolution)
            units=RESUNIT_INCH;
          if (image->units == PixelsPerCentimeterResolution)
            units=RESUNIT_CENTIMETER;
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Resolution %gx%g pixels%s",
                                  image->x_resolution, image->y_resolution,
                                  (units == RESUNIT_NONE ? " (undefined units)" :
                                   (units == RESUNIT_INCH ? " per inch" :
                                    (units == RESUNIT_CENTIMETER ? " per centimeter"
                                     : "BAD VALUE"))));
          (void) TIFFSetField(tiff,TIFFTAG_RESOLUTIONUNIT,(uint16) units);
          (void) TIFFSetField(tiff,TIFFTAG_XRESOLUTION,image->x_resolution);
          (void) TIFFSetField(tiff,TIFFTAG_YRESOLUTION,image->y_resolution);
        }
      if (image->chromaticity.white_point.x != 0.0)
        {
          float
            chromaticity[6];

          /*
            Set image primary chromaticities (x,y coordinates of RGB
            colorants and white point).
          */
          chromaticity[0]=image->chromaticity.red_primary.x;
          chromaticity[1]=image->chromaticity.red_primary.y;
          chromaticity[2]=image->chromaticity.green_primary.x;
          chromaticity[3]=image->chromaticity.green_primary.y;
          chromaticity[4]=image->chromaticity.blue_primary.x;
          chromaticity[5]=image->chromaticity.blue_primary.y;
          (void) TIFFSetField(tiff,TIFFTAG_PRIMARYCHROMATICITIES,chromaticity);
          chromaticity[0]=image->chromaticity.white_point.x;
          chromaticity[1]=image->chromaticity.white_point.y;
          (void) TIFFSetField(tiff,TIFFTAG_WHITEPOINT,chromaticity);
        }
      {
        /*
          Copy any embedded profiles to TIFF.
        */
        size_t
          profile_length=0;

        const unsigned char
          *profile_data=0;

#if defined(TIFFTAG_XMLPACKET)
        /*
          XML XMP profile.
        */
        if ((profile_data=GetImageProfile(image,"XMP",&profile_length)) != 0)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "XMP embedded profile with length %lu bytes",
                                  (unsigned long) profile_length);
            (void) TIFFSetField(tiff,TIFFTAG_XMLPACKET,(uint32) profile_length,
                                profile_data);
          }
#endif /* defined(TIFFTAG_XMLPACKET) */
      
#if defined(TIFFTAG_ICCPROFILE)
        /*
          ICC color profile.
        */
        if ((profile_data=GetImageProfile(image,"ICM",&profile_length)) != 0)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "ICC ICM embedded profile with length %lu bytes",
                                  (unsigned long) profile_length);
            (void) TIFFSetField(tiff,TIFFTAG_ICCPROFILE,(uint32)profile_length,
                                profile_data);
          }
#endif /* defined(ICC_SUPPORT) */
      
        /*
          IPTC NewsPhoto profile.
        */
        if ((profile_data=GetImageProfile(image,"IPTC",&profile_length)) != 0)
          {
            int
              profile_tag=0;
#if defined(TIFFTAG_PHOTOSHOP)
            /* Photoshop profile (with embedded IPTC profile). */
            profile_tag=TIFFTAG_PHOTOSHOP;
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "Photoshop embedded profile with length %lu"
				  " bytes",
                                  (unsigned long) profile_length);
#else
            /* IPTC TAG from RichTIFF specifications */
            profile_tag=TIFFTAG_RICHTIFFIPTC;
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "IPTC Newsphoto embedded profile with length"
				  " %lu bytes",
                                  (unsigned long) profile_length);

#endif /* defined(PHOTOSHOP_SUPPORT) */
            WriteNewsProfile(tiff,profile_tag,profile_data,profile_length);
          }
      }
      {
        /*
          Page and Page number tags.  Page is the current page number
          (0 based) and pages is the total number of pages.
	*/

        uint16
          page,
          pages;

        page=(uint16) scene;
        pages=GetImageListLength(image);

        if (image_info->adjoin && pages > 1)
          {
            /*
	      SubFileType = 2. LONG. The value 2 identifies a single
	      page of a multi-page image.
	    */
            (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_PAGE);
          }

        (void) TIFFSetField(tiff,TIFFTAG_PAGENUMBER,page,pages);
      }
      attribute=GetImageAttribute(image,"artist");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_ARTIST,attribute->value);

      attribute=GetImageAttribute(image,"copyright");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,33432,attribute->value);  /* TIFFTAG_COPYRIGHT */

      attribute=GetImageAttribute(image,"timestamp");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_DATETIME,attribute->value);

      (void) TIFFSetField(tiff,TIFFTAG_DOCUMENTNAME,image->filename);

      attribute=GetImageAttribute(image,"hostcomputer");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_HOSTCOMPUTER,attribute->value);

      attribute=GetImageAttribute(image,"comment");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_IMAGEDESCRIPTION,attribute->value);

      attribute=GetImageAttribute(image,"make");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_MAKE,attribute->value);
 
      attribute=GetImageAttribute(image,"model");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_MODEL,attribute->value);

      attribute=GetImageAttribute(image,"label");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_PAGENAME,attribute->value);

      attribute=GetImageAttribute(image,"software");
      if (attribute != (const ImageAttribute *) NULL)
        {
          /*
            Allow skipping software tag if attribute exists but is empty.
          */
          if (strlen(attribute->value))
            (void) TIFFSetField(tiff,TIFFTAG_SOFTWARE,attribute->value);
        }
      else
        {
          (void) TIFFSetField(tiff,TIFFTAG_SOFTWARE,
                              GetMagickVersion((unsigned long *) NULL));
        }

#if 0
      /*
	This tag is not supported by libtiff so the tag extension
	mechanism would need to be used to add support for it.
      */
      attribute=GetImageAttribute(image,"imageid");
      if (attribute != (const ImageAttribute *) NULL)
        (void) TIFFSetField(tiff,TIFFTAG_OPIIMAGEID,attribute->value);
#endif

      if (photometric == PHOTOMETRIC_PALETTE)
        {
          uint16
            *blue,
            *green,
            *red;

          /*
            Initialize TIFF colormap.
          */
          blue=MagickAllocateMemory(unsigned short *,
                                    65536L*sizeof(unsigned short));
          green=MagickAllocateMemory(unsigned short *,
                                     65536L*sizeof(unsigned short));
          red=MagickAllocateMemory(unsigned short *,
                                   65536L*sizeof(unsigned short));
          if ((blue == (unsigned short *) NULL) ||
              (green == (unsigned short *) NULL) ||
              (red == (unsigned short *) NULL))
            ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
                                 image);
          (void) memset(red,0,65536L*sizeof(unsigned short));
          (void) memset(green,0,65536L*sizeof(unsigned short));
          (void) memset(blue,0,65536L*sizeof(unsigned short));
          for (i=0; i < image->colors; i++)
            {
              red[i]=ScaleQuantumToShort(image->colormap[i].red);
              green[i]=ScaleQuantumToShort(image->colormap[i].green);
              blue[i]=ScaleQuantumToShort(image->colormap[i].blue);
            }
          (void) TIFFSetField(tiff,TIFFTAG_COLORMAP,red,green,blue);
          MagickFreeMemory(red);
          MagickFreeMemory(green);
          MagickFreeMemory(blue);
        }
      /*
        Set extra export options for grayscale.
      */
      if (photometric == PHOTOMETRIC_MINISWHITE)
        export_options.grayscale_miniswhite=MagickTrue;
      /*
        Set extra export options for floating point.
      */
      if (sample_format == SAMPLEFORMAT_IEEEFP)
        {
          const char *
            definition_value;

          export_options.sample_type=FloatQuantumSampleType;
          if ((definition_value=AccessDefinition(image_info,"tiff","min-sample-value")))
            export_options.double_minvalue=strtod(definition_value,(char **)NULL);
          if ((definition_value=AccessDefinition(image_info,"tiff","max-sample-value")))
            export_options.double_maxvalue=strtod(definition_value,(char **)NULL);
          (void) TIFFSetField(tiff,TIFFTAG_SMINSAMPLEVALUE,
                              export_options.double_minvalue);
          (void) TIFFSetField(tiff,TIFFTAG_SMAXSAMPLEVALUE,
                              export_options.double_maxvalue);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "Using min sample value %g, max sample value %g",
                                export_options.double_minvalue,
                                export_options.double_maxvalue);
        }

      /*
        For sample sizes matching a CPU native word, use native endian
        order for import.
      */
      if ((16 == bits_per_sample) || (32 == bits_per_sample) || (64 == bits_per_sample))
        export_options.endian=NativeEndian;

      /*
        Export pixels to TIFF.

        FIXME: JBIG needs a Strip writer.
      */
      switch (method)
        {
        default:
        case ScanLineMethod:
          {
            /*
              Write TIFF image as scanlines
            */
            unsigned char
              *scanline;

            const PixelPacket
              *p;

            int
              max_sample,
              quantum_samples,
              sample;

            QuantumType
              quantum_type;
            
            /*
              Allocate memory for one scanline.
            */
            scanline_size=TIFFScanlineSize(tiff);

            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using scanline %s write method with %u "
				    "bits per sample (%lu bytes/scanline)",
                                    PhotometricTagToString(photometric),
				    bits_per_sample, (unsigned long) scanline_size);

            scanline=MagickAllocateMemory(unsigned char *,(size_t) scanline_size);
            if (scanline == (unsigned char *) NULL)
              ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
            /*
              Prepare for separate/contiguous retrieval.
            */
            max_sample=1;
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                if (QuantumTransferMode(image,photometric,sample_format,
                                        samples_per_pixel,PLANARCONFIG_CONTIG,
                                        0,&quantum_type,&quantum_samples,
                                        &image->exception)
                    == MagickPass)
                  max_sample=quantum_samples;
              }
            /*
              For each plane
            */
            for (sample=0; sample < max_sample; sample++)
              {
                /*
                  Determine quantum parse method.
                */
                if (QuantumTransferMode(image,photometric,sample_format,
                                        samples_per_pixel,planar_config,sample,
                                        &quantum_type,&quantum_samples,
                                        &image->exception)
                    == MagickFail)
                  {
                    status=MagickFail;
                    break;
                  }
                for (y=0; y < image->rows; y++)
                  {
                    if ((image->matte) && (alpha_type == AssociatedAlpha))
                      p=GetImagePixels(image,0,y,image->columns,1);
                    else
                      p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
                    if (p == (const PixelPacket *) NULL)
                      {
                        status=MagickFail;
                        break;
                      }
                    /*
                      Convert to associated alpha if necessary.
                    */
                    if ((sample == 0) && (image->matte) &&
                        (alpha_type == AssociatedAlpha))
                      AssociateAlphaRegion(image);
                    /*
                      Export pixels to scanline.
                    */
                    if (ExportImagePixelArea(image,quantum_type,bits_per_sample,
                                             scanline,&export_options,&export_info)
                        == MagickFail)
                      {
                        status=MagickFail;
                        break;
                      }
                    /*
                      Write scanline.
                    */
#if !defined(WORDS_BIGENDIAN)
                    if (24 == bits_per_sample)
                      SwabDataToNativeEndian(bits_per_sample,scanline,scanline_size);
#endif
                    if (TIFFWriteScanline(tiff, scanline,y,sample) == -1)
                      {
                        status=MagickFail;
                        break;
                      }

                    if (image->previous == (Image *) NULL)
		      if (QuantumTick(y+sample*image->rows,image->rows*max_sample))
			if (!MagickMonitorFormatted(y+sample*image->rows,
						    image->rows*max_sample,&image->exception,
						    SaveImageText,image->filename,
						    image->columns,image->rows))
			  {
			    status=MagickFail;
			    break;
			  }

                  }
                if (status == MagickFail)
                  break;
              }
            MagickFreeMemory(scanline);
            break;
          }
        case TiledMethod:
          {
            /*
              Write TIFF image as tiles
            */
            unsigned char
              *tile;

            uint32
              tile_columns,
              tile_rows;

            tsize_t
              stride,
              tile_size,
              tile_size_max;

            int
              max_sample,
              quantum_samples,
              sample;

            QuantumType
              quantum_type;

            unsigned long
              tile_total_pixels;
        
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "Using tiled %s write method with %u bits "
				    "per sample",
                                    PhotometricTagToString(photometric),
				    bits_per_sample);

            /*
              Determine tile size
            */
            {
              const char *
                value;

              tile_columns=0;
              tile_rows=0;
              
              /*
                Enable tiled output (with default size, or specified separately)
                -define tiff:tile
                
                Use an exact tile size in rows & columns
                -define tiff:tile-geometry=128x128
                
                Use a specific tile width (pixels)
                -define tiff:tile-width=128
                
                Use a specific tile height (pixels)
                -define tiff:tile-height=128
              */
              if ((value=AccessDefinition(image_info,"tiff","tile-geometry")))
                {
                  double
                    width,
                    height;

                  if (GetMagickDimension(value,&width,&height,NULL,NULL) == 2)
                    {
                      tile_rows=(uint32) height;
                      tile_columns=(uint32) width;
                    }
                }
              if ((value=AccessDefinition(image_info,"tiff","tile-width")))
                {
                  tile_columns=MagickAtoL(value);
                }
              if ((value=AccessDefinition(image_info,"tiff","tile-height")))
                {
                  tile_rows=MagickAtoL(value);
                }
              
              TIFFDefaultTileSize(tiff,&tile_columns,&tile_rows);
            }

            if (!TIFFSetField(tiff,TIFFTAG_TILEWIDTH,tile_columns))
              status=MagickFail;
            if (!TIFFSetField(tiff,TIFFTAG_TILELENGTH,tile_rows))
              status=MagickFail;
            /*
              Obtain the maximum number of bytes required to contain a tile.
            */
            tile_size_max=TIFFTileSize(tiff);
            /*
              Compute the total number of pixels in one tile
            */
            tile_total_pixels=tile_columns*tile_rows;
            if (logging)
              {
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "TIFF tile geometry %ux%u, %lu pixels",
                                      (unsigned int)tile_columns,
                                      (unsigned int)tile_rows,
                                      tile_total_pixels);
              }
            /*
              Allocate tile buffer
            */
            tile=MagickAllocateMemory(unsigned char *, (size_t) tile_size_max);
            if (tile == (unsigned char *) NULL)
              ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
            /*
              Prepare for separate/contiguous retrieval.
            */
            max_sample=1;
            if (planar_config == PLANARCONFIG_SEPARATE)
              {
                if (QuantumTransferMode(image,photometric,sample_format,
                                        samples_per_pixel,PLANARCONFIG_CONTIG,
                                        0,&quantum_type,&quantum_samples,
                                        &image->exception)
                    == MagickPass)
                  max_sample=quantum_samples;
              }
            /*
              Obtain per-row stride.
            */
            stride=TIFFTileRowSize(tiff);

	    /*
	      Process each plane.
	    */
	    for (sample=0; sample < max_sample; sample++)
	      {
		/*
		  Determine quantum parse method.
		*/
		if (QuantumTransferMode(image,photometric,sample_format,
                                        samples_per_pixel,planar_config,
                                        sample,&quantum_type,&quantum_samples,
                                        &image->exception)
                    == MagickFail)
		  {
		    status=MagickFail;
		    break;
		  }
		for (y=0; y < image->rows; y+=tile_rows)
		  {
		    for (x=0; x < image->columns; x+=tile_columns)
		      {
			const PixelPacket
			  *p;
			
			long
			  tile_set_columns,
			  tile_set_rows;
			
			unsigned char
			  *q;
			
			register long
			  yy;
			
			/*
			  Compute image region corresponding to tile.
			*/
			if (x+tile_columns > image->columns)
			  tile_set_columns=(tile_columns-(x+tile_columns-image->columns));
			else
			  tile_set_columns=tile_columns;
			if (y+tile_rows > image->rows)
			  tile_set_rows=(tile_rows-(y+tile_rows-image->rows));
			else
			  tile_set_rows=tile_rows;
			
                        q=tile;
                        for (yy=y; yy < (long) y+tile_set_rows; yy++)
                          {
                            /*
                              Obtain pixel region corresponding to tile row.
                            */
                            if ((image->matte) && (alpha_type == AssociatedAlpha))
                              p=GetImagePixels(image,x,yy,tile_set_columns,1);
                            else
                              p=AcquireImagePixels(image,x,yy,tile_set_columns,
						   1,&image->exception);
                            if (p == (const PixelPacket *) NULL)
                              {
                                status=MagickFail;
                                break;
                              }
                            /*
                              Convert to associated alpha if necessary.
                            */
                            if ((sample == 0) && (image->matte) &&
                                (alpha_type == AssociatedAlpha))
                              AssociateAlphaRegion(image);
                            /*
                              Export tile row
                            */
                            if (ExportImagePixelArea(image,quantum_type,
                                                     bits_per_sample,q,
                                                     &export_options,&export_info)
                                == MagickFail)
                              {
                                status=MagickFail;
                                break;
                              }
                            /*
                              Enforce that we did not overrun our buffer.
                            */
                            assert(export_info.bytes_exported <= (size_t) scanline_size);
                            q += stride;
                          } /* for yy */
                        if (status == MagickFail)
                          break;
                        /*
                          Write tile.
                        */
#if !defined(WORDS_BIGENDIAN)
                        if (24 == bits_per_sample)
                          SwabDataToNativeEndian(bits_per_sample,tile,tile_size_max);
#endif
                        if ((tile_size=TIFFWriteTile(tiff,tile,x,y,0,sample)) == -1)
                          {
                            status=MagickFail;
                          }
			if (status == MagickFail)
			  break;
		      } /* for x */
		    /*
		      Progress indicator.
		    */
		    if (image->previous == (Image *) NULL)
		      if (QuantumTick((y+sample*image->rows)/tile_rows,
				      (image->rows*max_sample)/tile_rows))
			if (!MagickMonitorFormatted((y+sample*image->rows)/tile_rows,
						    (image->rows*max_sample)/tile_rows,
						    &image->exception,
						    SaveImageText,image->filename,
						    image->columns,image->rows))
			  status=MagickFail;
		    
		    if (status == MagickFail)
		      break;
		  } /* for y */
	      } /* for sample */
            MagickFreeMemory(tile);
            break;
          }
        }
      if (image_info->verbose > 1)
        TIFFPrintDirectory(tiff,stdout,MagickFalse);
      if(!TIFFWriteDirectory(tiff))
	{
	  status=MagickFail;
	  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
				"TIFFWriteDirectory returns failed status!");
	}
      if (image->next == (Image *) NULL)
        break;
      image=SyncNextImageInList(image);
      if ((status &= MagickMonitorFormatted(scene++,
                                            GetImageListLength(image),
                                            &image->exception,
                                            SaveImagesText,
                                            image->filename)) == MagickFail)
        break;
    } while (image_info->adjoin);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  TIFFClose(tiff); /* Should implicity invoke CloseBlob(image) */

  if (status == MagickFail)
    {
      /*
        Handle write failure.
      */
			    
      if (unlink(filename) != -1)
	(void) LogMagickEvent(CoderEvent,GetMagickModule(),
			      "Removed broken output file \"%s\"",filename);
      return (MagickFail);
    }

  return(MagickPass);
}
#endif
