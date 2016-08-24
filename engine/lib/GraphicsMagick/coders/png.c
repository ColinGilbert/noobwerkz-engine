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
%                            PPPP   N   N   GGGG                              %
%                            P   P  NN  N  G                                  %
%                            PPPP   N N N  G  GG                              %
%                            P      N  NN  G   G                              %
%                            P      N   N   GGG                               %
%                                                                             %
%                                                                             %
%             Read/Write Portable Network Graphics Image Format.              %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                           Glenn Randers-Pehrson                             %
%                               November 1997                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/* GraphicsMagick differences */

#define LoadImageTag LoadImageText
#define SaveImageTag SaveImageText
#define LoadImagesTag LoadImagesText
#define SaveImagesTag SaveImagesText
#define ParseMetaGeometry GetMagickGeometry
#define AcquireUniqueFilename AcquireTemporaryFileName
#define LiberateUniqueFileResource LiberateTemporaryFile
#define first_scene subimage
#define number_scenes subrange

#if !defined(RGBColorMatchExact)
#define RGBColorMatchExact(color,target) \
   (((color).red == (target).red) && \
    ((color).green == (target).green) && \
    ((color).blue == (target).blue))
#endif

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/analyze.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/channel.h"
#include "magick/color.h"
#include "magick/colormap.h"
#include "magick/constitute.h"
#include "magick/enhance.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/profile.h"
#include "magick/quantize.h"
#include "magick/resource.h"
#include "magick/semaphore.h"
#include "magick/static.h"
#include "magick/tempfile.h"
#include "magick/transform.h"
#include "magick/utility.h"
#if defined(HasPNG)

/* Suppress libpng pedantic warnings */
#define PNG_DEPRECATED  /* Use of this function is deprecated */
#define PNG_USE_RESULT  /* The result of this function must be checked */
#define PNG_NORETURN    /* This function does not return */
/* #define PNG_ALLOCATED */ /* The result of the function is new memory */
#define PNG_DEPSTRUCT   /* access to this struct member is deprecated */

#include "png.h"
#include "zlib.h"


#if PNG_LIBPNG_VER > 10011
/*
  Optional declarations. Define or undefine them as you like.
*/
/* #define PNG_DEBUG -- turning this on breaks VisualC compiling */

/*
  Features under construction.  Define these to work on them.
*/
#undef MNG_OBJECT_BUFFERS
#undef MNG_BASI_SUPPORTED
#define MNG_COALESCE_LAYERS /* In 5.4.4, this interfered with MMAP'ed files. */
#define MNG_INSERT_LAYERS   /* Troublesome, but seem to work as of 5.4.4 */
#define GMPNG_BUILD_PALETTE   /* This works as of 5.4.3. */
#if defined(HasJPEG)
#  define JNG_SUPPORTED /* Not finished as of 5.5.2.  See "To do" comments. */
#endif

/*
  Establish thread safety.
  setjmp/longjmp is claimed to be safe on these platforms:
  setjmp/longjmp is alleged to be unsafe on these platforms:
*/
#ifdef PNG_SETJMP_SUPPORTED
# ifndef SETJMP_IS_THREAD_SAFE
#   define GMPNG_SETJMP_NOT_THREAD_SAFE
# endif

# if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
static SemaphoreInfo
  *png_semaphore = (SemaphoreInfo *) NULL;
# endif
#endif

/*
  This is temporary until I set up malloc'ed object attributes array.
  Recompile with MNG_MAX_OBJECTS=65536L to avoid this limit but
  waste more memory.
*/
#define MNG_MAX_OBJECTS 256

/*
  If this is not defined, spec is interpreted strictly.  If it is
  defined, an attempt will be made to recover from some errors,
  including
      o global PLTE too short
*/
#undef MNG_LOOSE

/*
  Don't try to define PNG_MNG_FEATURES_SUPPORTED here.  Make sure
  it's defined in libpng/pngconf.h, version 1.0.9 or later.  It won't work
  with earlier versions of libpng.  From libpng-1.0.3a to libpng-1.0.8,
  PNG_READ|WRITE_EMPTY_PLTE were used but those have been deprecated in
  libpng in favor of PNG_MNG_FEATURES_SUPPORTED, so we set them here.
  PNG_MNG_FEATURES_SUPPORTED is disabled by default in libpng-1.0.9 and
  will be enabled by default in libpng-1.2.0.
*/
#ifdef PNG_MNG_FEATURES_SUPPORTED
#  ifndef PNG_READ_EMPTY_PLTE_SUPPORTED
#    define PNG_READ_EMPTY_PLTE_SUPPORTED
#  endif
#  ifndef PNG_WRITE_EMPTY_PLTE_SUPPORTED
#    define PNG_WRITE_EMPTY_PLTE_SUPPORTED
#  endif
#endif

/*
  Maximum valid unsigned long in PNG/MNG chunks is (2^31)-1
  This macro is only defined in libpng-1.0.3a and later.
*/
#ifndef PNG_MAX_UINT
#define PNG_MAX_UINT (png_uint_32) 0x7fffffffL
#endif

/*
  Constant strings for known chunk types.  If you need to add a chunk,
  add a string holding the name here.   To make the code more
  portable, we use ASCII numbers like this, not characters.
*/

static png_byte const mng_MHDR[5]={ 77,  72,  68,  82, '\0'};
static png_byte const mng_BACK[5]={ 66,  65,  67,  75, '\0'};
static png_byte const mng_BASI[5]={ 66,  65,  83,  73, '\0'};
static png_byte const mng_CLIP[5]={ 67,  76,  73,  80, '\0'};
static png_byte const mng_CLON[5]={ 67,  76,  79,  78, '\0'};
static png_byte const mng_DEFI[5]={ 68,  69,  70,  73, '\0'};
static png_byte const mng_DHDR[5]={ 68,  72,  68,  82, '\0'};
static png_byte const mng_DISC[5]={ 68,  73,  83,  67, '\0'};
static png_byte const mng_ENDL[5]={ 69,  78,  68,  76, '\0'};
static png_byte const mng_FRAM[5]={ 70,  82,  65,  77, '\0'};
static png_byte const mng_IEND[5]={ 73,  69,  78,  68, '\0'};
static png_byte const mng_IHDR[5]={ 73,  72,  68,  82, '\0'};
static png_byte const mng_JHDR[5]={ 74,  72,  68,  82, '\0'};
static png_byte const mng_LOOP[5]={ 76,  79,  79,  80, '\0'};
static png_byte const mng_MAGN[5]={ 77,  65,  71,  78, '\0'};
static png_byte const mng_MEND[5]={ 77,  69,  78,  68, '\0'};
static png_byte const mng_MOVE[5]={ 77,  79,  86,  69, '\0'};
static png_byte const mng_PAST[5]={ 80,  65,  83,  84, '\0'};
static png_byte const mng_PLTE[5]={ 80,  76,  84,  69, '\0'};
static png_byte const mng_SAVE[5]={ 83,  65,  86,  69, '\0'};
static png_byte const mng_SEEK[5]={ 83,  69,  69,  75, '\0'};
static png_byte const mng_SHOW[5]={ 83,  72,  79,  87, '\0'};
static png_byte const mng_TERM[5]={ 84,  69,  82,  77, '\0'};
static png_byte const mng_bKGD[5]={ 98,  75,  71,  68, '\0'};
static png_byte const mng_cHRM[5]={ 99,  72,  82,  77, '\0'};
static png_byte const mng_gAMA[5]={103,  65,  77,  65, '\0'};
static png_byte const mng_iCCP[5]={105,  67,  67,  80, '\0'};
static png_byte const mng_nEED[5]={110,  69,  69,  68, '\0'};
static png_byte const mng_pHYg[5]={112,  72,  89, 103, '\0'};
static png_byte const mng_pHYs[5]={112,  72,  89, 115, '\0'};
static png_byte const mng_sBIT[5]={115,  66,  73,  84, '\0'};
static png_byte const mng_sRGB[5]={115,  82,  71,  66, '\0'};
static png_byte const mng_tRNS[5]={116,  82,  78,  83, '\0'};

#if defined(JNG_SUPPORTED)
static png_byte const mng_IDAT[5]={ 73,  68,  65,  84, '\0'};
static png_byte const mng_JDAT[5]={ 74,  68,  65,  84, '\0'};
static png_byte const mng_JDAA[5]={ 74,  68,  65,  65, '\0'};
static png_byte const mng_JdAA[5]={ 74, 100,  65,  65, '\0'};
static png_byte const mng_JSEP[5]={ 74,  83,  69,  80, '\0'};
static png_byte const mng_oFFs[5]={111,  70,  70, 115, '\0'};
#endif

/*
static png_byte const mng_hIST[5]={104,  73,  83,  84, '\0'};
static png_byte const mng_iCCP[5]={105,  67,  67,  80, '\0'};
static png_byte const mng_iTXt[5]={105,  84,  88, 116, '\0'};
static png_byte const mng_sPLT[5]={115,  80,  76,  84, '\0'};
static png_byte const mng_tEXt[5]={116,  69,  88, 116, '\0'};
static png_byte const mng_tIME[5]={116,  73,  77,  69, '\0'};
static png_byte const mng_zTXt[5]={122,  84,  88, 116, '\0'};
*/

typedef struct _UShortPixelPacket
{
  unsigned short
    red,
    green,
    blue,
    opacity,
    index;
} UShortPixelPacket;

typedef struct _MngBox
{
  long
    left,
    right,
    top,
    bottom;
} MngBox;

typedef struct _MngPair
{
  volatile long
    a,
    b;
} MngPair;

#ifdef MNG_OBJECT_BUFFERS
typedef struct _MngBuffer
{

  unsigned long
    height,
    width;

  Image
    *image;

  png_color
    plte[256];

  int
    reference_count;

  unsigned char
    alpha_sample_depth,
    compression_method,
    color_type,
    concrete,
    filter_method,
    frozen,
    image_type,
    interlace_method,
    pixel_sample_depth,
    plte_length,
    sample_depth,
    viewable;
} MngBuffer;
#endif

typedef struct _MngInfo
{

#ifdef MNG_OBJECT_BUFFERS
  MngBuffer
    *ob[MNG_MAX_OBJECTS];
#endif

  Image *
    image;

  RectangleInfo
    page;

  int
    adjoin,
#ifndef PNG_READ_EMPTY_PLTE_SUPPORTED
    bytes_in_read_buffer,
    found_empty_plte,
#endif
    equal_backgrounds,
    equal_chrms,
    equal_gammas,
#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) || \
    defined(PNG_MNG_FEATURES_SUPPORTED)
    equal_palettes,
#endif
    equal_physs,
    equal_srgbs,
    framing_mode,
    have_global_bkgd,
    have_global_chrm,
    have_global_gama,
    have_global_phys,
    have_global_sbit,
    have_global_srgb,
    have_saved_bkgd_index,
    have_write_global_chrm,
    have_write_global_gama,
    have_write_global_plte,
    have_write_global_srgb,
    need_fram,
    object_id,
    old_framing_mode,
    saved_bkgd_index;

  int
    new_number_colors;

  long
    image_found,
    loop_count[256],
    loop_iteration[256],
    scenes_found,
    x_off[MNG_MAX_OBJECTS],
    y_off[MNG_MAX_OBJECTS];

  MngBox
    clip,
    frame,
    image_box,
    object_clip[MNG_MAX_OBJECTS];

  unsigned char
    /* These flags could be combined into one byte */
    exists[MNG_MAX_OBJECTS],
    frozen[MNG_MAX_OBJECTS],
    loop_active[256],
    invisible[MNG_MAX_OBJECTS],
    viewable[MNG_MAX_OBJECTS];

  ExtendedSignedIntegralType
    loop_jump[256];

  png_colorp
    global_plte;

  png_color_8
    global_sbit;

  png_byte
#ifndef PNG_READ_EMPTY_PLTE_SUPPORTED
    read_buffer[8],
#endif
    global_trns[256];

  float
    global_gamma;

  ChromaticityInfo
    global_chrm;

  RenderingIntent
    global_srgb_intent;

  unsigned long
    delay,
    global_plte_length,
    global_trns_length,
    global_x_pixels_per_unit,
    global_y_pixels_per_unit,
    mng_width,
    mng_height,
    ticks_per_second;

  unsigned int
    IsPalette,
    global_phys_unit_type,
    basi_warning,
    clon_warning,
    dhdr_warning,
    jhdr_warning,
    magn_warning,
    past_warning,
    phyg_warning,
    phys_warning,
    sbit_warning,
    show_warning,
    mng_type,
    write_mng,
    write_png_colortype,
    write_png_depth,
    write_png8,
    write_png24,
    write_png32,
    write_png48,
    write_png64;

#ifdef MNG_BASI_SUPPORTED
  unsigned long
    basi_width,
    basi_height;

  unsigned int
    basi_depth,
    basi_color_type,
    basi_compression_method,
    basi_filter_type,
    basi_interlace_method,
    basi_red,
    basi_green,
    basi_blue,
    basi_alpha,
    basi_viewable;
#endif

  png_uint_16
    magn_first,
    magn_last,
    magn_mb,
    magn_ml,
    magn_mr,
    magn_mt,
    magn_mx,
    magn_my,
    magn_methx,
    magn_methy;

  PixelPacket
    mng_global_bkgd;

  unsigned char
    *png_pixels;

  Quantum
    *quantum_scanline;

} MngInfo;
#endif /* VER */

/*
  Forward declarations.
*/
static unsigned int
  WritePNGImage(const ImageInfo *,Image *);
static unsigned int
  WriteMNGImage(const ImageInfo *,Image *);
#if defined(JNG_SUPPORTED)
static unsigned int
  WriteJNGImage(const ImageInfo *,Image *);
#endif
static const char* PngColorTypeToString(const unsigned int color_type)
{
  const char
    *result = "Unknown";

  switch (color_type)
    {
    case PNG_COLOR_TYPE_GRAY:
      result = "Gray";
      break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      result = "Gray+Alpha";
      break;
    case PNG_COLOR_TYPE_PALETTE:
      result = "Palette";
      break;
    case PNG_COLOR_TYPE_RGB:
      result = "RGB";
      break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
      result = "RGB+Alpha";
      break;
    }

  return result;
}
#endif /* HasPNG */

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s M N G                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMNG returns True if the image format type, identified by the
%  magick string, is MNG.
%
%  The format of the IsMNG method is:
%
%      unsigned int IsMNG(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsMNG returns True if the image format type is MNG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static MagickPassFail IsMNG(const unsigned char *magick,const size_t length)
{
  if (length < 8)
    return(MagickFail);
  if (memcmp(magick,"\212MNG\r\n\032\n",8) == 0)
    return(MagickPass);
  return(MagickFail);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s J N G                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsJNG returns True if the image format type, identified by the
%  magick string, is JNG.
%
%  The format of the IsJNG method is:
%
%      unsigned int IsJNG(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsJNG returns True if the image format type is JNG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static MagickPassFail IsJNG(const unsigned char *magick,const size_t length)
{
  if (length < 8)
    return(MagickFail);
  if (memcmp(magick,"\213JNG\r\n\032\n",8) == 0)
    return(MagickPass);
  return(MagickFail);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s P N G                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsPNG returns True if the image format type, identified by the
%  magick string, is PNG.
%
%  The format of the IsPNG method is:
%
%      unsigned int IsPNG(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsPNG returns True if the image format type is PNG.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static MagickPassFail IsPNG(const unsigned char *magick,const size_t length)
{
  if (length < 8)
    return(MagickFail);
  if (memcmp(magick,"\211PNG\r\n\032\n",8) == 0)
    return(MagickPass);
  return(MagickFail);
}

#if defined(HasPNG)
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if (PNG_LIBPNG_VER > 10011)
static size_t WriteBlobMSBULong(Image *image,const unsigned long value)
{
  unsigned char
    buffer[4];

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  buffer[0]=(unsigned char) (value >> 24);
  buffer[1]=(unsigned char) (value >> 16);
  buffer[2]=(unsigned char) (value >> 8);
  buffer[3]=(unsigned char) value;
  return(WriteBlob(image,4,buffer));
}

static void PNGLong(png_bytep p,png_uint_32 value)
{
  *p++=(png_byte) ((value >> 24) & 0xff);
  *p++=(png_byte) ((value >> 16) & 0xff);
  *p++=(png_byte) ((value >> 8) & 0xff);
  *p++=(png_byte) (value & 0xff);
}

static void PNGsLong(png_bytep p,png_int_32 value)
{
  *p++=(png_byte) ((value >> 24) & 0xff);
  *p++=(png_byte) ((value >> 16) & 0xff);
  *p++=(png_byte) ((value >> 8) & 0xff);
  *p++=(png_byte) (value & 0xff);
}

static void PNGShort(png_bytep p,png_uint_16 value)
{
  *p++=(png_byte) ((value >> 8) & 0xff);
  *p++=(png_byte) (value & 0xff);
}

static void PNGType(png_bytep p,png_byte const * type)
{
  (void) memcpy(p,type,4*sizeof(png_byte));
}

static void LogPNGChunk(int logging, png_byte const * type, unsigned long length)
{
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Writing %c%c%c%c chunk, length: %lu",
        type[0],type[1],type[2],type[3],length);
}
#endif /* PNG_LIBPNG_VER > 10011 */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#if PNG_LIBPNG_VER > 10011
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d P N G I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadPNGImage reads a Portable Network Graphics (PNG) or
%  Multiple-image Network Graphics (MNG) image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image or set of images.
%
%  MNG support written by Glenn Randers-Pehrson, randeg@alum.rpi.edu.
%
%  The format of the ReadPNGImage method is:
%
%      Image *ReadPNGImage(const ImageInfo *image_info,
%                          ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadPNGImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%  To do, more or less in chronological order (as of version 5.5.2,
%   November 26, 2002 -- glennrp -- see also "To do" under WriteMNGImage):
%
%    Get 16-bit cheap transparency working.
%
%    (At this point, PNG decoding is supposed to be in full MNG-LC compliance)
%
%    Preserve all unknown and not-yet-handled known chunks found in input
%    PNG file and copy them into output PNG files according to the PNG
%    copying rules.
%
%    (At this point, PNG encoding should be in full MNG compliance)
%
%    Provide options for choice of background to use when the MNG BACK
%    chunk is not present or is not mandatory (i.e., leave transparent,
%    user specified, MNG BACK, PNG bKGD)
%
%    Implement LOOP/ENDL [done, but could do discretionary loops more
%    efficiently by linking in the duplicate frames.].
%
%    Decode and act on the MHDR simplicity profile (offer option to reject
%    files or attempt to process them anyway when the profile isn't LC or VLC).
%
%    Upgrade to full MNG without Delta-PNG.
%
%        o  BACK [done a while ago except for background image ID]
%        o  MOVE [done 15 May 1999]
%        o  CLIP [done 15 May 1999]
%        o  DISC [done 19 May 1999]
%        o  SAVE [partially done 19 May 1999 (marks objects frozen)]
%        o  SEEK [partially done 19 May 1999 (discard function only)]
%        o  SHOW
%        o  PAST
%        o  BASI
%        o  MNG-level tEXt/iTXt/zTXt
%        o  pHYg
%        o  pHYs
%        o  sBIT
%        o  bKGD
%        o  iTXt (wait for libpng implementation).
%
%    Use the scene signature to discover when an identical scene is
%    being reused, and just point to the original image->pixels instead
%    of storing another set of pixels.  This is not specific to MNG
%    but could be applied generally.
%
%    Upgrade to full MNG with Delta-PNG.
%
%    JNG tEXt/iTXt/zTXt
%
%    We will not attempt to read files containing the CgBI chunk.
%    They are really Xcode files meant for display on the iPhone.
%    These are not valid PNG files and it is impossible to recover
%    the orginal PNG from files that have been converted to Xcode-PNG,
%    since irretrievable loss of color data has occurred due to the
%    use of premultiplied alpha.
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  This is the function that does the actual reading of data.  It is
  the same as the one supplied in libpng, except that it receives the
  datastream from the ReadBlob() function instead of standard input.
*/
static void png_get_data(png_structp png_ptr,png_bytep data,png_size_t length)
{
  Image
    *image;

  image=(Image *) png_get_io_ptr(png_ptr);
  if (length)
    {
      png_size_t
        check;
    
      if (length > 0x7fffffff)
        png_warning(png_ptr, "chunk length > 2G");
      check=(png_size_t) ReadBlob(image,(size_t) length,(char *) data);
      if (check != length)
        {
          char
            msg[MaxTextExtent];
        
            (void) sprintf(msg,"Expected %lu bytes; found %lu bytes",
                           (unsigned long) length,(unsigned long) check);
          png_warning(png_ptr,msg);
          png_error(png_ptr,"Read Exception");
        }
    }
}

#if !defined(PNG_READ_EMPTY_PLTE_SUPPORTED) && \
    !defined(PNG_MNG_FEATURES_SUPPORTED)
/* We use mng_get_data() instead of png_get_data() if we have a libpng
 * older than libpng-1.0.3a, which was the first to allow the empty
 * PLTE, or a newer libpng in which PNG_MNG_FEATURES_SUPPORTED was
 * ifdef'ed out.  Earlier versions would crash if the bKGD chunk was
 * encountered after an empty PLTE, so we have to look ahead for bKGD
 * chunks and remove them from the datastream that is passed to libpng,
 * and store their contents for later use.
 */
static void mng_get_data(png_structp png_ptr,png_bytep data,png_size_t length)
{
  MngInfo
    *mng_info;

  Image
    *image;

  png_size_t
    check;

  register long
    i;

  i=0;
  mng_info=(MngInfo *) png_get_io_ptr(png_ptr);
  image=(Image *) mng_info->image;
  while (mng_info->bytes_in_read_buffer && length)
    {
      data[i]=mng_info->read_buffer[i];
      mng_info->bytes_in_read_buffer--;
      length--;
      i++;
    }
  if (length)
    {
      check=(png_size_t) ReadBlob(image,(size_t) length,(char *) data);
      if (check != length)
        png_error(png_ptr,"Read Exception");
      if (length == 4)
        {
          if ((data[0] == 0) && (data[1] == 0) && (data[2] == 0) &&
              (data[3] == 0))
            {
              check=(png_size_t) ReadBlob(image,(size_t) length,
                                          (char *) mng_info->read_buffer);
              mng_info->read_buffer[4]=0;
              mng_info->bytes_in_read_buffer=4;
              if (!memcmp(mng_info->read_buffer,mng_PLTE,4))
                mng_info->found_empty_plte=MagickTrue;
              if (!memcmp(mng_info->read_buffer,mng_IEND,4))
                {
                  mng_info->found_empty_plte=MagickFalse;
                  mng_info->have_saved_bkgd_index=MagickFalse;
                }
            }
          if ((data[0] == 0) && (data[1] == 0) && (data[2] == 0) &&
              (data[3] == 1))
            {
              check=(png_size_t) ReadBlob(image,(size_t) length,
                                          (char *) mng_info->read_buffer);
              mng_info->read_buffer[4]=0;
              mng_info->bytes_in_read_buffer=4;
              if (!memcmp(mng_info->read_buffer,mng_bKGD,4))
                if (mng_info->found_empty_plte)
                  {
                    /*
                      Skip the bKGD data byte and CRC.
                    */
                    check=(png_size_t)
                      ReadBlob(image,5,(char *) mng_info->read_buffer);
                    check=(png_size_t) ReadBlob(image,(size_t) length,
                                                (char *)
                                                mng_info->read_buffer);
                    mng_info->saved_bkgd_index=mng_info->read_buffer[0];
                    mng_info->have_saved_bkgd_index=MagickTrue;
                    mng_info->bytes_in_read_buffer=0;
                  }
            }
        }
    }
}
#endif

static void png_put_data(png_structp png_ptr,png_bytep data,png_size_t length)
{
  Image
    *image;

  image=(Image *) png_get_io_ptr(png_ptr);
  if (length)
    {
      png_size_t
        check;
    
      check=(png_size_t) WriteBlob(image,(unsigned long) length,(char *) data);
      if (check != length)
        png_error(png_ptr,"WriteBlob Failed");
    }
}

static void png_flush_data(png_structp png_ptr)
{
  ARG_NOT_USED(png_ptr);

  /* There is currently no safe API to "flush" a blob. */
#if 0
  Image
    *image;

  image=(Image *) png_get_io_ptr(png_ptr);
  (void) SyncBlob(image);
#endif
}

#ifdef PNG_WRITE_EMPTY_PLTE_SUPPORTED
static int PalettesAreEqual(Image *a,Image *b)
{
  long
    i;

  if ((a == (Image *) NULL) || (b == (Image *) NULL))
    return(MagickFail);
  if (a->storage_class!=PseudoClass || b->storage_class!=PseudoClass)
    return(MagickFail);
  if (a->colors != b->colors)
    return(MagickFail);
  for (i=0; i < (long) a->colors; i++)
    {
      if ((a->colormap[i].red != b->colormap[i].red) ||
          (a->colormap[i].green != b->colormap[i].green) ||
          (a->colormap[i].blue != b->colormap[i].blue))
        return(MagickFail);
    }
  return((int) MagickTrue);
}
#endif

static void MngInfoDiscardObject(MngInfo *mng_info,int i)
{
  if (i && (i < MNG_MAX_OBJECTS) && (mng_info != (MngInfo *) NULL) &&
      mng_info->exists[i] && !mng_info->frozen[i])
    {
#ifdef MNG_OBJECT_BUFFERS
      if (mng_info->ob[i] != (MngBuffer *) NULL)
        {
          if (mng_info->ob[i]->reference_count > 0)
            mng_info->ob[i]->reference_count--;
          if (mng_info->ob[i]->reference_count == 0)
            {
              if (mng_info->ob[i]->image != (Image *) NULL)
                DestroyImage(mng_info->ob[i]->image);
              MagickFreeMemory(mng_info->ob[i]);
            }
        }
      mng_info->ob[i]=(MngBuffer *) NULL;
#endif
      mng_info->exists[i]=MagickFalse;
      mng_info->invisible[i]=MagickFalse;
      mng_info->viewable[i]=MagickFalse;
      mng_info->frozen[i]=MagickFalse;
      mng_info->x_off[i]=0;
      mng_info->y_off[i]=0;
      mng_info->object_clip[i].left=0;
      mng_info->object_clip[i].right=PNG_MAX_UINT;
      mng_info->object_clip[i].top=0;
      mng_info->object_clip[i].bottom=PNG_MAX_UINT;
    }
}

static void MngInfoFreeStruct(MngInfo *mng_info,int *have_mng_structure)
{
  if (*have_mng_structure && (mng_info != (MngInfo *) NULL))
    {
      register long
        i;
    
      for (i=1; i < MNG_MAX_OBJECTS; i++)
        MngInfoDiscardObject(mng_info,i);
      MagickFreeMemory(mng_info->global_plte);
      MagickFreeMemory(mng_info);
      *have_mng_structure=MagickFalse;
    }
}

static MngBox mng_minimum_box(MngBox box1,MngBox box2)
{
  MngBox
    box;

  box=box1;
  if (box.left < box2.left)
    box.left=box2.left;
  if (box.top < box2.top)
    box.top=box2.top;
  if (box.right > box2.right)
    box.right=box2.right;
  if (box.bottom > box2.bottom)
    box.bottom=box2.bottom;
  return box;
}

static MngBox mng_read_box(MngBox previous_box,char delta_type,
                           unsigned char *p)
{
  MngBox
    box;

  /*
    Read clipping boundaries from DEFI, CLIP, FRAM, or PAST chunk.
  */
  box.left=(long) ((p[0]  << 24) | (p[1]  << 16) | (p[2]  << 8) | p[3]);
  box.right=(long) ((p[4]  << 24) | (p[5]  << 16) | (p[6]  << 8) | p[7]);
  box.top=(long) ((p[8]  << 24) | (p[9]  << 16) | (p[10] << 8) | p[11]);
  box.bottom=(long) ((p[12] << 24) | (p[13] << 16) | (p[14] << 8) | p[15]);
  if (delta_type != 0)
    {
      box.left+=previous_box.left;
      box.right+=previous_box.right;
      box.top+=previous_box.top;
      box.bottom+=previous_box.bottom;
    }
  return(box);
}

static MngPair mng_read_pair(MngPair previous_pair,int delta_type,
  unsigned char *p)
{
  MngPair
    pair;
  /*
    Read two longs from CLON, MOVE or PAST chunk
  */
  pair.a=(long) ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
  pair.b=(long) ((p[4] << 24) | (p[5] << 16) | (p[6] << 8) | p[7]);
  if (delta_type != 0)
    {
      pair.a+=previous_pair.a;
      pair.b+=previous_pair.b;
    }
  return(pair);
}

static long mng_get_long(unsigned char *p)
{
  return((long) ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]));
}

static void PNGErrorHandler(png_struct *ping,png_const_charp message) MAGICK_FUNC_NORETURN;

static void PNGErrorHandler(png_struct *ping,png_const_charp message)
{
  Image
    *image;

  image=(Image *) png_get_error_ptr(ping);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  libpng-%.1024s error: %.1024s",
                        PNG_LIBPNG_VER_STRING, message);
  (void) ThrowException2(&image->exception,CoderError,message,image->filename);
#if (PNG_LIBPNG_VER < 10500)
  longjmp(ping->jmpbuf,1);
#else
  png_longjmp(ping,1);
#endif
  SignalHandlerExit(1); /* Avoid GCC warning about non-exit function which does exit */
}

static void PNGWarningHandler(png_struct *ping,png_const_charp message)
{
  Image
    *image;

  if (LocaleCompare(message, "Missing PLTE before tRNS") == 0)
    png_error(ping, message);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  libpng-%.1024s warning: %.1024s",
                        PNG_LIBPNG_VER_STRING, message);
  image=(Image *) png_get_error_ptr(ping);
  (void) ThrowException2(&image->exception,CoderWarning,message,
                         image->filename);
}

#ifdef PNG_USER_MEM_SUPPORTED
#if PNG_LIBPNG_VER >= 14000
static png_voidp png_IM_malloc(png_structp png_ptr,png_alloc_size_t size)
#else
static png_voidp png_IM_malloc(png_structp png_ptr,png_size_t size)
#endif
{
  (void) png_ptr;
  return MagickAllocateMemory(png_voidp,(size_t) size);
}

/*
  Free a pointer.  It is removed from the list at the same time.
*/
static png_free_ptr png_IM_free(png_structp png_ptr,png_voidp ptr)
{
  (void) png_ptr;
  MagickFreeMemory(ptr);
  return((png_free_ptr) NULL);
}
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

static MagickPassFail
png_read_raw_profile(Image *image, const ImageInfo *image_info,
                     png_textp text,int ii)
{
  char
    profile_description[MaxTextExtent],
    profile_name[MaxTextExtent];

  unsigned char
    *info;

  register long
    i;

  register unsigned char
    *dp;

  register png_charp
    sp;

  png_uint_32
    length,
    nibbles;

  unsigned char
    unhex[103]={0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,1, 2,3,4,5,6,7,8,9,0,0,
                0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,10,11,12,
                13,14,15};

  sp=text[ii].text+1;
  /* look for newline */
  while (*sp != '\n')
    sp++;
  /* look for length */
  while (*sp == '\0' || *sp == ' ' || *sp == '\n')
    sp++;
  length=MagickAtoL(sp);
  while (*sp != ' ' && *sp != '\n')
    sp++;
  /* allocate space */
  if (length == 0)
    {
      (void) ThrowException2(&image->exception,CoderWarning,
                             "invalid profile length",(char *) NULL);
      return (MagickFail);
    }
  info=MagickAllocateMemory(unsigned char *,length);
  if (info == (unsigned char *) NULL)
    {
      (void) ThrowException2(&image->exception,CoderWarning,
                             "unable to copy profile",(char *) NULL);
      return (MagickFail);
    }
  /* copy profile, skipping white space and column 1 "=" signs */
  dp=info;
  nibbles=length*2;
  for (i=0; i < (long) nibbles; i++)
    {
      while (*sp < '0' || (*sp > '9' && *sp < 'a') || *sp > 'f')
        {
          if (*sp == '\0')
            {
              MagickFreeMemory(info);
              (void) ThrowException2(&image->exception,CoderWarning,
                                     "ran out of profile data",(char *) NULL);
              return (MagickFail);
            }
          sp++;
        }
      if (i%2 == 0)
        *dp=16*unhex[(int) *sp++];
      else
        (*dp++)+=unhex[(int) *sp++];
    }

  /* We have already read "Raw profile type " */
  if(!memcmp(&text[ii].key[17], "iptc\0",5))
    {
      strlcpy(profile_name,"IPTC",sizeof(profile_name));
      strlcpy(profile_description,"IPTC profile.",sizeof(profile_description));
    }
  else if (!memcmp(&text[ii].key[17], "icm\0",4))
    {
      strlcpy(profile_name,"ICM",sizeof(profile_name));
      strlcpy(profile_description,"ICM (ICCP) profile.",
              sizeof(profile_description));
    }
  else
    {
      strlcpy(profile_name,&text[ii].key[17],sizeof(profile_name));
      strlcpy(profile_description,"generic profile, type ",
              sizeof(profile_description));
      strlcat(profile_description,&text[ii].key[17],
              sizeof(profile_description));
    }
  if (image_info->verbose)
    (void) printf(" Found a %.1024s\n",profile_description);
  if(SetImageProfile(image,profile_name,info,length) == MagickFail)
    {
      MagickFreeMemory(info);
      (void) ThrowException(&image->exception,ResourceLimitError,
                            MemoryAllocationFailed,"unable to copy profile");
    }
  MagickFreeMemory(info);
  return MagickTrue;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d O n e P N G I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadOnePNGImage reads a Portable Network Graphics (PNG) image file
%  (minus the 8-byte signature)  and returns it.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.
%
%  The format of the ReadOnePNGImage method is:
%
%      Image *ReadOnePNGImage(MngInfo *mng_info, const ImageInfo *image_info,
%         ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadOnePNGImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o mng_info: Specifies a pointer to a MngInfo structure.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
*/
static Image *ReadOnePNGImage(MngInfo *mng_info,
                              const ImageInfo *image_info,
                              ExceptionInfo *exception)
{
  /* Read one PNG image */

  Image
    *image;

  int
    logging,
    num_text,
    num_passes,
    pass,
    ping_bit_depth,
    ping_colortype,
    ping_file_depth,
    ping_interlace_method,
    ping_compression_method,
    ping_filter_method,
    ping_num_trans;

  LongPixelPacket
    transparent_color;

  png_bytep
     ping_trans_alpha;

  png_color_16p
     ping_background,
     ping_trans_color;

  png_info
    *end_info,
    *ping_info;

  png_struct
    *ping;

  png_uint_32
    ping_rowbytes,
    ping_width,
    ping_height;

  png_textp
    text;

  long
    y;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register PixelPacket
    *q;

  unsigned long
    length,
    row_offset;

#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
  png_byte unused_chunks[]=
    {
      104,  73,  83,  84, '\0',   /* hIST */
      105,  84,  88, 116, '\0',   /* iTXt */
      112,  67,  65,  76, '\0',   /* pCAL */
      115,  67,  65,  76, '\0',   /* sCAL */
      115,  80,  76,  84, '\0',   /* sPLT */
      116,  73,  77,  69, '\0',   /* tIME */
#ifdef PNG_APNG_SUPPORTED /* libpng was built with APNG patch; */
                          /* ignore the APNG chunks */
       97,  99,  84,  76, '\0',   /* acTL */
      102,  99,  84,  76, '\0',   /* fcTL */
      102, 100,  65,  84, '\0',   /* fdAT */
#endif
    };
#endif

  logging=LogMagickEvent(CoderEvent,GetMagickModule(),
                         "  enter ReadOnePNGImage()");

  /* Set to an out-of-range color unless tRNS chunk is present */
  transparent_color.red=65537;
  transparent_color.green=65537;
  transparent_color.blue=65537;
  transparent_color.opacity=65537;

#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
  LockSemaphoreInfo(png_semaphore);
#endif

#if (PNG_LIBPNG_VER < 10012)
  if (image_info->verbose)
    printf("Your PNG library (libpng-%s) is rather old.\n",
           PNG_LIBPNG_VER_STRING);
#endif

  image=mng_info->image;

  /*
    Allocate the PNG structures
  */
#ifdef PNG_USER_MEM_SUPPORTED
  ping=png_create_read_struct_2(PNG_LIBPNG_VER_STRING, image,
                                PNGErrorHandler,PNGWarningHandler, NULL,
                                (png_malloc_ptr) png_IM_malloc,
                                (png_free_ptr) png_IM_free);
#else
  ping=png_create_read_struct(PNG_LIBPNG_VER_STRING,image,
                              PNGErrorHandler,PNGWarningHandler);
#endif
  if (ping == (png_struct *) NULL)
    {
#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
      UnlockSemaphoreInfo(png_semaphore);
#endif
      ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
    }

  mng_info->png_pixels=(unsigned char *) NULL;
  mng_info->quantum_scanline=(Quantum *) NULL;

  if (setjmp(png_jmpbuf(ping)))
    {
      /*
        PNG image is corrupt.
      */
      png_destroy_read_struct(&ping,&ping_info,&end_info);
      MagickFreeMemory(mng_info->quantum_scanline);
      MagickFreeMemory(mng_info->png_pixels);
#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
      UnlockSemaphoreInfo(png_semaphore);
#endif
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "  exit ReadOnePNGImage() with error.");
      if (image != (Image *) NULL)
        {
          if (image->exception.severity > exception->severity)
            CopyException(exception,&image->exception);
          image->columns=0;
        }
      return(image);
    }

/* { From here through end of ReadOnePNGImage(), use png_error(), not Throw() */

  ping_info=png_create_info_struct(ping);
  if (ping_info == (png_info *) NULL)
    {
      png_destroy_read_struct(&ping,(png_info **) NULL,(png_info **) NULL);
      png_error(ping,"Could not allocate PNG info struct");
    }

  end_info=png_create_info_struct(ping);
  if (end_info == (png_info *) NULL)
    {
      png_destroy_read_struct(&ping,&ping_info,(png_info **) NULL);
      png_error(ping,"Could not allocate PNG end_info struct");
    }

#ifdef PNG_BENIGN_ERRORS_SUPPORTED
  /* Allow benign errors */
  png_set_benign_errors(ping, 1);
#endif

#ifdef PNG_SET_USER_LIMITS_SUPPORTED
  /* Reject images with too many rows or columns */
  png_set_user_limits(ping,
    (png_uint_32) Min(0x7fffffffL, GetMagickResourceLimit(WidthResource)),
    (png_uint_32) Min(0x7fffffffL, GetMagickResourceLimit(HeightResource)));
#endif /* PNG_SET_USER_LIMITS_SUPPORTED */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    PNG width limit: %lu, height limit: %lu",
    (unsigned long) Min(0x7fffffffL, GetMagickResourceLimit(WidthResource)),
    (unsigned long) Min(0x7fffffffL, GetMagickResourceLimit(HeightResource)));
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    PNG pixels limit: %lu",
    (unsigned long) GetMagickResourceLimit(PixelsResource));

  /*
    Prepare PNG for reading.
  */
  mng_info->image_found++;
  png_set_sig_bytes(ping,8);
  if (LocaleCompare(image_info->magick,"MNG") == 0)
    {
#if defined(PNG_MNG_FEATURES_SUPPORTED)
      (void) png_permit_mng_features(ping,PNG_ALL_MNG_FEATURES);
      png_set_read_fn(ping,image,png_get_data);
#else
#if defined(PNG_READ_EMPTY_PLTE_SUPPORTED)
      png_permit_empty_plte(ping,MagickTrue);
      png_set_read_fn(ping,image,png_get_data);
#else
      mng_info->image=image;
      mng_info->bytes_in_read_buffer=0;
      mng_info->found_empty_plte=MagickFalse;
      mng_info->have_saved_bkgd_index=MagickFalse;
      png_set_read_fn(ping,mng_info,mng_get_data);
#endif
#endif
    }
  else
    png_set_read_fn(ping,image,png_get_data);

#if defined(PNG_UNKNOWN_CHUNKS_SUPPORTED)
  /* Ignore unknown chunks */
#if PNG_LIBPNG_VER < 10700 /* Avoid a libpng16 warning */
  png_set_keep_unknown_chunks(ping, 2, NULL, 0);
#else
  png_set_keep_unknown_chunks(ping, 1, NULL, 0);
#endif
  /* Ignore unused chunks */
  png_set_keep_unknown_chunks(ping, 1, unused_chunks,
                              (int)sizeof(unused_chunks)/5);
#endif

#ifdef PNG_READ_CHECK_FOR_INVALID_INDEX_SUPPORTED
    /* Disable new libpng-1.5.10 feature while reading */
    png_set_check_for_invalid_index (ping, 0);
#endif

#if defined(PNG_USE_PNGGCCRD) && defined(PNG_ASSEMBLER_CODE_SUPPORTED)  \
  && (PNG_LIBPNG_VER >= 10200)
  /* Disable thread-unsafe features of pnggccrd */
  if (png_access_version_number() >= 10200)
    {
      png_uint_32 mmx_disable_mask=0;
      png_uint_32 asm_flags;

      mmx_disable_mask |= ( PNG_ASM_FLAG_MMX_READ_COMBINE_ROW  \
                            | PNG_ASM_FLAG_MMX_READ_FILTER_SUB   \
                            | PNG_ASM_FLAG_MMX_READ_FILTER_AVG   \
                            | PNG_ASM_FLAG_MMX_READ_FILTER_PAETH );
      asm_flags=png_get_asm_flags(ping);
      png_set_asm_flags(ping, asm_flags & ~mmx_disable_mask);
    }
#endif

  png_read_info(ping,ping_info);

  (void) png_get_IHDR(ping,ping_info,
                      &ping_width,
                      &ping_height,
                      &ping_bit_depth,
                      &ping_colortype,
                      &ping_interlace_method,
                      &ping_compression_method,

                      &ping_filter_method);

#if (QuantumDepth == 8)
  if (ping_bit_depth > 8)
  {
#  if defined(PNG_READ_STRIP_16_TO_8_SUPPORTED) || \
      defined(PNG_READ_16_TO_8_SUPPORTED)
    png_set_strip_16(ping);
#  else
#    ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
    png_set_scale_16(ping);
#    endif
#  endif
    ping_bit_depth=8;
    image->depth=8;
  }
#else /* QuantumDepth > 8 */
  if (ping_bit_depth > 8)
    image->depth=16;
  else
    image->depth=8;
#endif

  ping_file_depth = ping_bit_depth;

  /* Save bit-depth and color-type in case we later want to write a PNG00 */
  {
      char
        msg[MaxTextExtent];

      (void) FormatString(msg,"%d",(int) ping_colortype);
      (void) SetImageAttribute(image,"png:IHDR.color-type-orig",msg);

      (void) FormatString(msg,"%d",(int) ping_bit_depth);
      (void) SetImageAttribute(image,"png:IHDR.bit-depth-orig",msg);
  }

  (void) png_get_tRNS(ping, ping_info, &ping_trans_alpha, &ping_num_trans,
                      &ping_trans_color);

  (void) png_get_bKGD(ping, ping_info, &ping_background);

  if (logging)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    PNG width: %lu, height: %lu",
                            (unsigned long)ping_width,
                            (unsigned long)ping_height);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    PNG color_type: %d, bit_depth: %d",
                            ping_colortype, ping_bit_depth);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    PNG compression_method: %d",
                            ping_compression_method);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    PNG interlace_method: %d, filter_method: %d",
                            ping_interlace_method,
                            ping_filter_method);
    }

  /* Too big? */
  if (ping_width >
     (magick_uint64_t) GetMagickResourceLimit(PixelsResource)/ping_height)
  {
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "Number of pixels exceeds resource limit");
    png_error(ping, "Number of pixels exceeds resource limit");
  }

  if (ping_bit_depth < 8)
    {
       png_set_packing(ping);
       ping_bit_depth=8;
       image->depth=8;
    }

#if defined(PNG_READ_iCCP_SUPPORTED)
    if (png_get_valid(ping, ping_info, PNG_INFO_iCCP))
    {
      int
        compression;

#if (PNG_LIBPNG_VER < 10500)
      png_charp
        info;
#else
      png_bytep
        info;
#endif

      png_charp
        name;

      png_uint_32
        profile_length;

      (void) png_get_iCCP(ping,ping_info,&name,(int *) &compression,&info,
                          &profile_length);
      if (profile_length)
        {
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "    Reading PNG iCCP chunk.");
          /* libpng will destroy name and info */
          if (SetImageProfile(image,"ICM",(const unsigned char *) info,
                              (size_t) profile_length) == MagickFail)
            {
              MagickError3(ResourceLimitError,MemoryAllocationFailed,
                           UnableToAllocateICCProfile);
            }
        }
    }
#endif /* #if defined(PNG_READ_iCCP_SUPPORTED) */
#if defined(PNG_READ_sRGB_SUPPORTED)
  {
    int
      intent;

    if (!png_get_sRGB(ping,ping_info,&intent))
      {
        if (mng_info->have_global_srgb)
          {
            png_set_sRGB(ping,ping_info,(mng_info->global_srgb_intent+1));
          }
      }
    if (png_get_sRGB(ping,ping_info,&intent))
      {
        image->rendering_intent=(RenderingIntent) (intent+1);
        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "    Reading PNG sRGB chunk:"
                                " rendering_intent: %d",
                                intent+1);
      }
  }
#endif
  {
    double
      file_gamma;

    if (!png_get_gAMA(ping,ping_info,&file_gamma))
      {
        if (mng_info->have_global_gama)
          png_set_gAMA(ping,ping_info,mng_info->global_gamma);
      }
    if (png_get_gAMA(ping,ping_info,&file_gamma))
      {
        image->gamma=(float) file_gamma;
        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "    Reading PNG gAMA chunk: gamma: %f",
                                file_gamma);
      }
  }
  if (!png_get_valid(ping, ping_info, PNG_INFO_cHRM))
    {
      if (mng_info->have_global_chrm)
        (void) png_set_cHRM(ping,ping_info,
                     mng_info->global_chrm.white_point.x,
                     mng_info->global_chrm.white_point.y,
                     mng_info->global_chrm.red_primary.x,
                     mng_info->global_chrm.red_primary.y,
                     mng_info->global_chrm.green_primary.x,
                     mng_info->global_chrm.green_primary.y,
                     mng_info->global_chrm.blue_primary.x,
                     mng_info->global_chrm.blue_primary.y);
    }
  if (png_get_valid(ping, ping_info, PNG_INFO_cHRM))
    {
      (void) png_get_cHRM(ping,ping_info,
                          &image->chromaticity.white_point.x,
                          &image->chromaticity.white_point.y,
                          &image->chromaticity.red_primary.x,
                          &image->chromaticity.red_primary.y,
                          &image->chromaticity.green_primary.x,
                          &image->chromaticity.green_primary.y,
                          &image->chromaticity.blue_primary.x,
                          &image->chromaticity.blue_primary.y);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "    Reading PNG cHRM chunk.");
    }
  if (image->rendering_intent)
    {
      image->gamma=0.45455f;
      image->chromaticity.red_primary.x=0.6400f;
      image->chromaticity.red_primary.y=0.3300f;
      image->chromaticity.green_primary.x=0.3000f;
      image->chromaticity.green_primary.y=0.6000f;
      image->chromaticity.blue_primary.x=0.1500f;
      image->chromaticity.blue_primary.y=0.0600f;
      image->chromaticity.white_point.x=0.3127f;
      image->chromaticity.white_point.y=0.3290f;
    }
#if defined(PNG_oFFs_SUPPORTED)
  if (mng_info->mng_type == 0 && (png_get_valid(ping, ping_info,
                                                PNG_INFO_oFFs)))
    {
      image->page.x=png_get_x_offset_pixels(ping, ping_info);
      image->page.y=png_get_y_offset_pixels(ping, ping_info);
      if (logging)
        if (image->page.x || image->page.y)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "    Reading PNG oFFs chunk: x: %ld, y: %ld.",
                                image->page.x,
                                image->page.y);
    }
#endif
#if defined(PNG_pHYs_SUPPORTED)
  if (!png_get_valid(ping, ping_info, PNG_INFO_pHYs))
    {
      if (mng_info->have_global_phys)
        {
          png_set_pHYs(ping,ping_info,
                       mng_info->global_x_pixels_per_unit,
                       mng_info->global_y_pixels_per_unit,
                       mng_info->global_phys_unit_type);
        }
    }
  if (png_get_valid(ping, ping_info, PNG_INFO_pHYs))
    {
      int
        unit_type;

      png_uint_32
        x_resolution,
        y_resolution;

      /*
        Set image resolution.
      */
      (void) png_get_pHYs(ping,ping_info,&x_resolution,&y_resolution,
                          &unit_type);
      image->x_resolution=(float) x_resolution;
      image->y_resolution=(float) y_resolution;
      if (unit_type == PNG_RESOLUTION_METER)
        {
          image->units=PixelsPerCentimeterResolution;
          image->x_resolution=(double) x_resolution/100.0;
          image->y_resolution=(double) y_resolution/100.0;
        }
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "    Reading PNG pHYs chunk:"
                              " xres: %lu, yres: %lu, units: %d.",
                              (unsigned long)x_resolution,
                              (unsigned long)y_resolution,
                              unit_type);
    }
#endif
  if (png_get_valid(ping, ping_info, PNG_INFO_PLTE))
    {
      int
        number_colors;

      png_colorp
        palette;

      (void) png_get_PLTE(ping,ping_info,&palette,&number_colors);
      if (number_colors == 0 && ping_colortype ==
          PNG_COLOR_TYPE_PALETTE)
        {
          if (mng_info->global_plte_length)
            {
              png_set_PLTE(ping,ping_info,mng_info->global_plte,
                           (int) mng_info->global_plte_length);
              if (!(png_get_valid(ping, ping_info, PNG_INFO_tRNS)))
                if (mng_info->global_trns_length)
                  {
                    if (mng_info->global_trns_length >
                        mng_info->global_plte_length)
                       png_error(ping, "global tRNS has more entries"
                                             " than global PLTE");
                    png_set_tRNS(ping,ping_info,mng_info->global_trns,
                                 (int) mng_info->global_trns_length,NULL);
                  }
#if defined(PNG_READ_bKGD_SUPPORTED)
              if (
#ifndef PNG_READ_EMPTY_PLTE_SUPPORTED
                  mng_info->have_saved_bkgd_index ||
#endif
                  png_get_valid(ping, ping_info, PNG_INFO_bKGD))
                {
                  png_color_16
                    background;

#ifndef PNG_READ_EMPTY_PLTE_SUPPORTED
                  if (mng_info->have_saved_bkgd_index)
                    background.index=mng_info->saved_bkgd_index;
#endif
                  if (png_get_valid(ping, ping_info, PNG_INFO_bKGD))
                    background.index=ping_background->index;
                  background.red=(png_uint_16)
                    mng_info->global_plte[background.index].red;
                  background.green=(png_uint_16)
                    mng_info->global_plte[background.index].green;
                  background.blue=(png_uint_16)
                    mng_info->global_plte[background.index].blue;
                  png_set_bKGD(ping,ping_info,&background);
                }
#endif
            }
          else
            png_error (ping, "No global PLTE in file");
        }
    }

#if defined(PNG_READ_bKGD_SUPPORTED)
  if (mng_info->have_global_bkgd && 
              !(png_get_valid(ping,ping_info, PNG_INFO_bKGD)))
    image->background_color=mng_info->mng_global_bkgd;
  if (png_get_valid(ping, ping_info, PNG_INFO_bKGD))
    {
      /*
        Set image background color.
      */

      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "    Reading PNG bKGD chunk.");

      if (ping_bit_depth == QuantumDepth)
        {
          image->background_color.red  = ping_background->red;
          image->background_color.green= ping_background->green;
          image->background_color.blue = ping_background->blue;
        }
      else /* Scale background components to 16-bit */
        {
          unsigned int
            bkgd_scale;

          if (logging != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    raw ping_background=(%d,%d,%d).",ping_background->red,
              ping_background->green,ping_background->blue);

          bkgd_scale = 1;
          if (ping_file_depth == 1)
             bkgd_scale = 255;
          else if (ping_file_depth == 2)
             bkgd_scale = 85;
          else if (ping_file_depth == 4)
             bkgd_scale = 17;
          if (ping_file_depth <= 8)
             bkgd_scale *= 257;

          ping_background->red *= bkgd_scale;
          ping_background->green *= bkgd_scale;
          ping_background->blue *= bkgd_scale;

          if (logging != MagickFalse)
            {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    bkgd_scale=%d.",bkgd_scale);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    ping_background=(%d,%d,%d).",ping_background->red,
              ping_background->green,ping_background->blue);
            }

          image->background_color.red=
            ScaleShortToQuantum(ping_background->red);
          image->background_color.green=
            ScaleShortToQuantum(ping_background->green);
          image->background_color.blue=
            ScaleShortToQuantum(ping_background->blue);
          image->background_color.opacity=OpaqueOpacity;

          if (logging != MagickFalse)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "    image->background_color=(%d,%d,%d).",
              image->background_color.red,
              image->background_color.green,image->background_color.blue);
        }
    }
#endif

  if (png_get_valid(ping, ping_info, PNG_INFO_tRNS))
    {
      int
        bit_mask;

      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    Reading PNG tRNS chunk.");

      bit_mask = (1 << ping_file_depth) - 1;

      /*
        Image has a transparent background.
      */

      transparent_color.red=
        (unsigned long)(ping_trans_color->red & bit_mask);
      transparent_color.green=
        (unsigned long) (ping_trans_color->green & bit_mask);
      transparent_color.blue=
        (unsigned long) (ping_trans_color->blue & bit_mask);
      transparent_color.opacity=
        (unsigned long) (ping_trans_color->gray & bit_mask);

      if (ping_colortype == PNG_COLOR_TYPE_GRAY)
        {
#if (QuantumDepth == 8)
          if (ping_file_depth < QuantumDepth)
#endif
            transparent_color.opacity=(unsigned long) (
                ping_trans_color->gray *
                (65535L/((1UL << ping_file_depth)-1)));

#if (QuantumDepth == 8)
          else
            transparent_color.opacity=(unsigned long) (
                (ping_trans_color->gray * 65535L)/
                ((1UL << ping_file_depth)-1));
#endif
          if (logging != MagickFalse)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "    Raw tRNS graylevel is %d.",ping_trans_color->gray);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "    scaled graylevel is %d.",
                   (int) transparent_color.opacity);
            }
          transparent_color.red=transparent_color.opacity;
          transparent_color.green=transparent_color.opacity;
          transparent_color.blue=transparent_color.opacity;
        }
    }
#if defined(PNG_READ_sBIT_SUPPORTED)
  if (!png_get_valid(ping, ping_info, PNG_INFO_sBIT))
    if (mng_info->have_global_sbit)
        png_set_sBIT(ping,ping_info,&mng_info->global_sbit);
#endif
  num_passes=png_set_interlace_handling(ping);

  png_read_update_info(ping,ping_info);

  ping_rowbytes=(png_uint_32) png_get_rowbytes(ping,ping_info);

  /*
    Initialize image structure.
  */
  mng_info->image_box.left=0;
  mng_info->image_box.right=(long) ping_width;
  mng_info->image_box.top=0;
  mng_info->image_box.bottom=(long) ping_height;
  if (mng_info->mng_type == 0)
    {
      mng_info->mng_width=ping_width;
      mng_info->mng_height=ping_height;
      mng_info->frame=mng_info->image_box;
      mng_info->clip=mng_info->image_box;
    }
  else
    {
      image->page.y=mng_info->y_off[mng_info->object_id];
    }
  image->compression=ZipCompression;
  image->columns=ping_width;
  image->rows=ping_height;
  if ((ping_colortype == PNG_COLOR_TYPE_PALETTE) ||
      (ping_colortype == PNG_COLOR_TYPE_GRAY_ALPHA) ||
      (ping_colortype == PNG_COLOR_TYPE_GRAY))
    {
      image->storage_class=PseudoClass;
      image->colors=1 << ping_file_depth;
#if (QuantumDepth == 8)
      if (image->colors > 256)
        image->colors=256;
#else
      if (image->colors > 65536L)
        image->colors=65536L;
#endif
      if (ping_colortype == PNG_COLOR_TYPE_PALETTE)
        {
          int
            number_colors;

          png_colorp
            palette;

          (void) png_get_PLTE(ping,ping_info,&palette,&number_colors);
          image->colors=number_colors;
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "    Reading PNG PLTE chunk:"
                                  " number_colors: %d.",number_colors);
        }
    }

  if (image->storage_class == PseudoClass)
    {
      /*
        Initialize image colormap.
      */
      if (!AllocateImageColormap(image,image->colors))
        png_error(ping, "Could not allocate image colormap");
      if (ping_colortype == PNG_COLOR_TYPE_PALETTE)
        {
          int
            number_colors;

          png_colorp
            palette;

          (void) png_get_PLTE(ping,ping_info,&palette,&number_colors);
          for (i=0; i < (long) image->colors; i++)
            {
              image->colormap[i].red=ScaleCharToQuantum(palette[i].red);
              image->colormap[i].green=ScaleCharToQuantum(palette[i].green);
              image->colormap[i].blue=ScaleCharToQuantum(palette[i].blue);
            }
        }
      else
        {
          unsigned long
            scale;

          scale=(MaxRGB/((1 << ping_file_depth)-1));
          if (scale < 1)
            scale=1;
          for (i=0; i < (long) image->colors; i++)
            {
              image->colormap[i].red=(Quantum) (i*scale);
              image->colormap[i].green=(Quantum) (i*scale);
              image->colormap[i].blue=(Quantum) (i*scale);
            }
        }
    }
  if (image->delay != 0)
    mng_info->scenes_found++;
  if (image_info->ping && ((mng_info->mng_type == 0) ||
     ((image_info->number_scenes != 0) &&
      mng_info->scenes_found > (long) (image_info->first_scene+
                                       image_info->number_scenes))))
    {
      /* This happens later in non-ping decodes */
      if (png_get_valid(ping,ping_info,PNG_INFO_tRNS))
        image->storage_class=DirectClass;

      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "    Skipping PNG image data for scene %ld",
                              mng_info->scenes_found-1);
      png_destroy_read_struct(&ping,&ping_info,&end_info);
      MagickFreeMemory(mng_info->quantum_scanline);
      MagickFreeMemory(mng_info->png_pixels);
#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
      UnlockSemaphoreInfo(png_semaphore);
#endif
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "  exit ReadOnePNGImage().");
      return (image);
    }

  /*
    Read image scanlines.
  */
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "    Reading PNG IDAT chunk(s)");
  if (num_passes > 1)
    mng_info->png_pixels=MagickAllocateMemory(unsigned char *,
                                    ping_rowbytes*image->rows);
  else
    mng_info->png_pixels=MagickAllocateMemory(unsigned char *, ping_rowbytes);
  if (mng_info->png_pixels == (unsigned char *) NULL)
    png_error(ping, "Could not allocate png_pixels array");

  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "    Converting PNG pixels to pixel packets");
  /*
    Convert PNG pixels to pixel packets.
  */
  if (image->storage_class == DirectClass)
    for (pass=0; pass < num_passes; pass++)
      {
        /*
          Convert image to DirectClass pixel packets.
        */
#if (QuantumDepth == 8)
        int
          depth;

        depth=(long) ping_bit_depth;
#endif
        image->matte=((ping_colortype == PNG_COLOR_TYPE_RGB_ALPHA) ||
                      (ping_colortype == PNG_COLOR_TYPE_GRAY_ALPHA) ||
                      (png_get_valid(ping, ping_info, PNG_INFO_tRNS)));

        for (y=0; y < (long) image->rows; y++)
          {
            if (num_passes > 1)
              row_offset=ping_rowbytes*y;

            else
              row_offset=0;

            png_read_row(ping,mng_info->png_pixels+row_offset,NULL);

	    if (!SetImagePixels(image,0,y,image->columns,1))
              break;

            if (pass < num_passes-1)
              continue;

#if (QuantumDepth == 8)
            if (depth == 16)
              {
                register Quantum
                  *p,
                  *r;

                r=mng_info->png_pixels+row_offset;
                p=r;
                if (ping_colortype == PNG_COLOR_TYPE_GRAY)
                  {
                    for (x=(long) image->columns; x > 0; x--)
                      {
                        *r++=*p++;
                        p++;
                        if ((png_get_valid(ping, ping_info, PNG_INFO_tRNS)) &&
                            ((unsigned long) ((*(p-2) << 8)|*(p-1))
                            == transparent_color.opacity))
                          {
                            /* Cheap transparency */
                            *r++=TransparentOpacity;
                          }
                        else
                          *r++=OpaqueOpacity;
                      }
                  }
                else if (ping_colortype == PNG_COLOR_TYPE_RGB)
                  {
                    if (png_get_valid(ping, ping_info, PNG_INFO_tRNS))
                      for (x=(long) image->columns; x > 0; x--)
                        {
                          *r++=*p++;
                          p++;
                          *r++=*p++;
                          p++;
                          *r++=*p++;
                          p++;
                          if (((unsigned long) ((*(p-6) << 8)|*(p-5)) ==
                              transparent_color.red) &&
                              ((unsigned long) ((*(p-4) << 8)|*(p-3)) ==
                              transparent_color.green) &&
                              ((unsigned long) ((*(p-2) << 8)|*(p-1)) ==
                              transparent_color.blue))
                            {
                              /* Cheap transparency */
                              *r++=TransparentOpacity;
                            }
                          else
                            *r++=OpaqueOpacity;
                        }
                    else
                      for (x=(long) image->columns; x > 0; x--)
                        {
                          *r++=*p++;
                          p++;
                          *r++=*p++;
                          p++;
                          *r++=*p++;
                          p++;
                          *r++=OpaqueOpacity;
                        }
                  }
                else if (ping_colortype == PNG_COLOR_TYPE_RGB_ALPHA)
                  for (x=(long) (4*image->columns); x > 0; x--)
                    {
                      *r++=*p++;
                      p++;
                    }
                else if (ping_colortype == PNG_COLOR_TYPE_GRAY_ALPHA)
                  for (x=(long) (2*image->columns); x > 0; x--)
                    {
                      *r++=*p++;
                      p++;
                    }
              }
            if (depth == 8 && ping_colortype == PNG_COLOR_TYPE_GRAY)
              (void) ImportImagePixelArea(image,(QuantumType) GrayQuantum,
                                          image->depth,mng_info->png_pixels+
                                          row_offset,0,0);
            else if (ping_colortype == PNG_COLOR_TYPE_GRAY)
              {
                image->depth=8;
                (void) ImportImagePixelArea(image,
                                           (QuantumType) GrayQuantum,
                                            image->depth,mng_info->png_pixels+
                                            row_offset,0,0);
                image->depth=depth;
              }
            else if (ping_colortype == PNG_COLOR_TYPE_GRAY_ALPHA)
              {
                image->depth=8;
                (void) ImportImagePixelArea(image,
                                           (QuantumType) GrayAlphaQuantum,
                                            image->depth,mng_info->png_pixels+
                                            row_offset,0,0);
                image->depth=depth;
              }
            else if (depth == 8 && ping_colortype == PNG_COLOR_TYPE_RGB)
              (void) ImportImagePixelArea(image,(QuantumType) RGBQuantum,
                                          image->depth,mng_info->png_pixels+
                                          row_offset,0,0);
            else if (ping_colortype == PNG_COLOR_TYPE_RGB)
              {
                image->depth=8;
                (void) ImportImagePixelArea(image,(QuantumType) RGBQuantum,
                                            image->depth,mng_info->png_pixels+
                                            row_offset,0,0);
                image->depth=depth;
              }
            else if (ping_colortype == PNG_COLOR_TYPE_RGB_ALPHA)
              {
                image->depth=8;
                (void) ImportImagePixelArea(image,(QuantumType) RGBAQuantum,
                                            image->depth,mng_info->png_pixels+
                                            row_offset,0,0);
                image->depth=depth;
              }
            else if (ping_colortype == PNG_COLOR_TYPE_PALETTE)
              (void) ImportImagePixelArea(image,(QuantumType) IndexQuantum,
                                          ping_bit_depth,mng_info->png_pixels+
                                          row_offset,0,0);
                                          /* FIXME, sample size ??? */
#else /* (QuantumDepth != 8) */

            if (ping_colortype == PNG_COLOR_TYPE_GRAY)
              (void) ImportImagePixelArea(image,(QuantumType) GrayQuantum,
                                          image->depth,mng_info->png_pixels+
                                          row_offset,0,0);
            else if (ping_colortype == PNG_COLOR_TYPE_GRAY_ALPHA)
              (void) ImportImagePixelArea(image,(QuantumType) GrayAlphaQuantum,
                                          image->depth,mng_info->png_pixels+
                                          row_offset,0,0);
            else if (ping_colortype == PNG_COLOR_TYPE_RGB_ALPHA)
              (void) ImportImagePixelArea(image,(QuantumType) RGBAQuantum,
                                          image->depth,mng_info->png_pixels+
                                          row_offset,0,0);
            else if (ping_colortype == PNG_COLOR_TYPE_PALETTE)
              (void) ImportImagePixelArea(image,(QuantumType) IndexQuantum,
                                          ping_bit_depth,mng_info->png_pixels+
                                          row_offset,0,0);
                                          /* FIXME, sample size ??? */
            else
              (void) ImportImagePixelArea(image,(QuantumType) RGBQuantum,
                                          image->depth,mng_info->png_pixels+
                                          row_offset,0,0);
#endif
            if (!SyncImagePixels(image))
              break;
          }

        if (image->previous == (Image *) NULL)
          if (QuantumTick(pass, num_passes))
            if (!MagickMonitorFormatted(pass,num_passes,exception,
                                        LoadImageTag,image->filename,
				        image->columns,image->rows))
               break;
      }

  else /* image->storage_class != DirectClass */
  {
   image->matte=ping_colortype == PNG_COLOR_TYPE_GRAY_ALPHA;
   mng_info->quantum_scanline=MagickAllocateMemory(Quantum *,
      (image->matte ?  2 : 1) * image->columns*sizeof(Quantum));
   if (mng_info->quantum_scanline == (Quantum *) NULL)
     png_error(ping, "Could not allocate quantum_scanline");
   (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                         "      Allocated quantum_scanline");

    for (pass=0; pass < num_passes; pass++)
      {
        register Quantum
          *r;

        /*
          Convert grayscale image to PseudoClass pixel packets.
        */
        for (y=0; y < (long) image->rows; y++)
          {
            register unsigned char
              *p;

            if (num_passes > 1)
              row_offset=ping_rowbytes*y;

            else
              row_offset=0;

            png_read_row(ping,mng_info->png_pixels+row_offset,NULL);
            q=SetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;

            if (pass < num_passes-1)
              continue;

            indexes=AccessMutableIndexes(image);
            p=mng_info->png_pixels+row_offset;
            r=mng_info->quantum_scanline;
            switch (ping_bit_depth)
              {
              case 8:
                {
                  if (ping_colortype == 4)
                    for (x=(long) image->columns; x > 0; x--)
                      {
                        *r++=*p++;
                        /* In image.h, OpaqueOpacity is 0
                         * TransparentOpacity is MaxRGB
                         * In a PNG datastream, Opaque is MaxRGB
                         * and Transparent is 0.
                         */
                        q->opacity=ScaleCharToQuantum(255-(*p++));
                        q++;
                      }
                  else
                    for (x=(long) image->columns; x > 0; x--)
                      *r++=*p++;
                  break;
                }
              case 16:
                {
                  for (x=(long) image->columns; x > 0; x--)
                    {
#if (QuantumDepth == 16)
                      if (image->colors > 256)
                        *r=((*p++) << 8);
                      else
                        *r=0;
                      *r|=(*p++);
                      r++;
                      if (ping_colortype == 4)
                        {
                          q->opacity=((*p++) << 8);
                          q->opacity|=(*p++);
                          q->opacity=(Quantum) (MaxRGB-q->opacity);
                          q++;
                        }
#else
#if (QuantumDepth == 32)
                      if (image->colors > 256)
                        *r=((*p++) << 8);
                      else
                        *r=0;
                      *r|=(*p++);
                      r++;
                      if (ping_colortype == 4)
                        {
                          q->opacity=((*p++) << 8);
                          q->opacity|=(*p++);
                          q->opacity*=65537L;
                          q->opacity=(Quantum) (MaxRGB-q->opacity);
                          q++;
                        }
#else /* QuantumDepth == 8 */
                      *r++=(*p++);
                      p++; /* strip low byte */
                      if (ping_colortype == 4)
                        {
                          q->opacity=(Quantum) (MaxRGB-(*p++));
                          p++;
                          q++;
                        }
#endif
#endif
                    }
                  break;
                }
              default:
                break;
              }
            /*
              Transfer image scanline.
            */
            r=mng_info->quantum_scanline;

            for (x=0; x < (long) image->columns; x++)
              indexes[x]=(*r++);

            if (!SyncImagePixels(image))
              break;
            }

        if (image->previous == (Image *) NULL)
          if (QuantumTick(pass, num_passes))
            if (!MagickMonitorFormatted(pass,num_passes,exception,LoadImageTag,
                                        image->filename,
            			        image->columns,image->rows))
              break;
      }

      MagickFreeMemory(mng_info->quantum_scanline);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "      Free'ed quantum_scanline after last pass");
  }

  if (image->storage_class == PseudoClass)
    (void) SyncImage(image);
  png_read_end(ping,ping_info);

  if (image_info->number_scenes != 0 && mng_info->scenes_found-1 <
      (long) image_info->first_scene && image->delay != 0.)
    {
      png_destroy_read_struct(&ping,&ping_info,&end_info);
      MagickFreeMemory(mng_info->quantum_scanline);
      MagickFreeMemory(mng_info->png_pixels);
      image->colors=2;
      (void) SetImage(image,TransparentOpacity);
#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
      UnlockSemaphoreInfo(png_semaphore);
#endif
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "  exit ReadOnePNGImage().");
      return (image);
    }
  if (png_get_valid(ping, ping_info, PNG_INFO_tRNS))
    {
      ClassType
        storage_class;

      /*
        Image has a transparent background.
      */
      storage_class=image->storage_class;
      image->matte=MagickTrue;
      for (y=0; y < (long) image->rows; y++)
        {
          image->storage_class=storage_class;
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=AccessMutableIndexes(image);

          if (storage_class == PseudoClass)
            {
              IndexPacket
                index;

              if (ping_colortype == PNG_COLOR_TYPE_PALETTE)
                for (x=0; x < (long) image->columns; x++)
                  {
                    index=indexes[x];
                    if (index < (unsigned int) ping_num_trans)
                      q->opacity=
                        ScaleCharToQuantum(255-ping_trans_alpha[index]);
		    else
		      q->opacity=OpaqueOpacity;
                    q++;
                  }
              else if (ping_colortype == PNG_COLOR_TYPE_GRAY)
                for (x=0; x < (long) image->columns; x++)
                  {
                    index=indexes[x];
                    q->red=image->colormap[index].red;
                    q->green=image->colormap[index].green;
                    q->blue=image->colormap[index].blue;
                    if ((unsigned long) ScaleQuantumToShort(q->red) ==
                        transparent_color.opacity)
                      q->opacity=TransparentOpacity;
		    else
		      q->opacity=OpaqueOpacity;
                    q++;
                  }
            }
          else
            for (x=(long) image->columns; x > 0; x--)
              {
                if ((unsigned long) ScaleQuantumToShort(q->red) ==
                    transparent_color.red &&
                    (unsigned long) ScaleQuantumToShort(q->green) ==
                    transparent_color.green &&
                    (unsigned long) ScaleQuantumToShort(q->blue) ==
                    transparent_color.blue)
                  q->opacity=TransparentOpacity;
		else
		  q->opacity=OpaqueOpacity;
                q++;
              }
          if (!SyncImagePixels(image))
            break;
        }
      image->storage_class=DirectClass;
    }
#if (QuantumDepth == 8)
  if (image->depth > 8)
    image->depth=8;
#endif
  if (png_get_text(ping,ping_info,&text,&num_text) != 0)
    for (i=0; i < (long) num_text; i++)
      {
        /* Check for a profile */

        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "    Reading PNG text chunk");
        if (strlen(text[i].key) > 16 &&
            !memcmp(text[i].key, "Raw profile type ",17))
          {
            if (png_read_raw_profile(image,image_info,text,(int) i) ==
              MagickFail)
            break;
          }
        else
          {
            char
              *value;

            length=(unsigned long) text[i].text_length;
            value=MagickAllocateMemory(char *,length+1);
            if (value == (char *) NULL)
              {
                png_warning(ping, "Could not allocate memory for text chunk");
                break;
              }
            *value='\0';
            (void) strlcat(value,text[i].text,length+1);
            (void) SetImageAttribute(image,text[i].key,value);
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "      Keyword: %s",text[i].key);
            MagickFreeMemory(value);
          }
      }
#ifdef MNG_OBJECT_BUFFERS
  /*
    Store the object if necessary.
  */
  if (object_id && !mng_info->frozen[object_id])
    {
      if (mng_info->ob[object_id] == (MngBuffer *) NULL)
        {
          /*
            create a new object buffer.
          */
          mng_info->ob[object_id]=MagickAllocateMemory(MngBuffer *,
                                                       sizeof(MngBuffer));
          if (mng_info->ob[object_id] != (MngBuffer *) NULL)
            {
              mng_info->ob[object_id]->image=(Image *) NULL;
              mng_info->ob[object_id]->reference_count=1;
            }
        }
      if ((mng_info->ob[object_id] == (MngBuffer *) NULL) ||
          mng_info->ob[object_id]->frozen)
        {
          if (mng_info->ob[object_id] == (MngBuffer *) NULL)
            png_error(ping, "Could not allocate MNG object");
          if (mng_info->ob[object_id]->frozen)
            png_error(ping, "Could not overwrite MNG frozen object buffer");
        }
      else
        {
          png_uint_32
            width,
            height;

          int
            bit_depth,
            color_type,
            interlace_method,
            compression_method,
            filter_method;

          if (mng_info->ob[object_id]->image != (Image *) NULL)
            DestroyImage(mng_info->ob[object_id]->image);
          mng_info->ob[object_id]->image=CloneImage(image,0,0,MagickTrue,
                                                    &image->exception);
          if (mng_info->ob[object_id]->image != (Image *) NULL)
            mng_info->ob[object_id]->image->file=(FILE *) NULL;
          else
            png_error(ping, "Cloning image for object buffer failed");
          png_get_IHDR(ping,ping_info,&width,&height,&bit_depth,&color_type,
                       &interlace_method,&compression_method,&filter_method);
          if (width > 250000L || height > 250000L)
            png_error(ping,"PNG Image dimensions are too large.");
          mng_info->ob[object_id]->width=width;
          mng_info->ob[object_id]->height=height;
          mng_info->ob[object_id]->color_type=color_type;
          mng_info->ob[object_id]->sample_depth=bit_depth;
          mng_info->ob[object_id]->interlace_method=interlace_method;
          mng_info->ob[object_id]->compression_method=compression_method;
          mng_info->ob[object_id]->filter_method=filter_method;
          if (png_get_valid(ping, ping_info, PNG_INFO_PLTE))
            {
              int
                number_colors;

              png_colorp
                plte;

              /*
                Copy the PLTE to the object buffer.
              */
              png_get_PLTE(ping,ping_info,&plte,&number_colors);
              mng_info->ob[object_id]->plte_length=number_colors;
              for (i=0; i < number_colors; i++)
                {
                  mng_info->ob[object_id]->plte[i]=plte[i];
                }
            }
          else
            mng_info->ob[object_id]->plte_length=0;
        }
    }
#endif
  /*
    Free memory.
  */
  png_destroy_read_struct(&ping,&ping_info,&end_info);
  MagickFreeMemory(mng_info->quantum_scanline);
  MagickFreeMemory(mng_info->png_pixels);

#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
  UnlockSemaphoreInfo(png_semaphore);
#endif

  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  exit ReadOnePNGImage()");
  return (image);

  /* End of setjmp-controlled block } */

  /* end of reading one PNG image */
}

static Image *ReadPNGImage(const ImageInfo *image_info,
                           ExceptionInfo *exception)
{
  Image
    *image;

  MngInfo
    *mng_info;

  char
    magic_number[MaxTextExtent];

  int
    have_mng_structure,
    logging;

  unsigned int
    status;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"enter ReadPNGImage()");
  image=AllocateImage(image_info);
  mng_info=(MngInfo *) NULL;
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Verify PNG signature.
  */
  (void) ReadBlob(image,8,magic_number);
  if (memcmp(magic_number,"\211PNG\r\n\032\n",8) != 0)
  {
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  }
  /*
    Allocate a MngInfo structure.
  */
  have_mng_structure=MagickFalse;
  mng_info=MagickAllocateMemory(MngInfo *,sizeof(MngInfo));
  if (mng_info == (MngInfo *) NULL)
  {
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  }
  /*
    Initialize members of the MngInfo structure.
  */
  (void) memset(mng_info,0,sizeof(MngInfo));
  mng_info->image=image;
  have_mng_structure=MagickTrue;

  image=ReadOnePNGImage(mng_info,image_info,exception);
  MngInfoFreeStruct(mng_info,&have_mng_structure);
  if (image == (Image *) NULL)
    {
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "exit ReadPNGImage() with error");
      return((Image *) NULL);
    }
  CloseBlob(image);
  if (image->columns == 0 || image->rows == 0)
    {
      if (image->exception.severity > exception->severity)
        CopyException(exception,&image->exception);
      DestroyImageList(image);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "exit ReadPNGImage() with error.");
      return((Image *) NULL);
    }
  if (LocaleCompare(image_info->magick,"PNG8") == 0)
    {
      (void) SetImageType(image,PaletteType);
      if (image->matte)
        {
          /* To do: Reduce to binary transparency */
        }
    }
  if (LocaleCompare(image_info->magick,"PNG24") == 0)
    {
      (void) SetImageType(image,TrueColorType);
      image->matte=MagickFalse;
    }
  if (LocaleCompare(image_info->magick,"PNG32") == 0)
    (void) SetImageType(image,TrueColorMatteType);
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"exit ReadPNGImage()");
  return (image);
}



#if defined(JNG_SUPPORTED)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d O n e J N G I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadOneJNGImage reads a JPEG Network Graphics (JNG) image file
%  (minus the 8-byte signature)  and returns it.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.
%
%  JNG support written by Glenn Randers-Pehrson, randeg@alum.rpi.edu.
%
%  The format of the ReadOneJNGImage method is:
%
%      Image *ReadOneJNGImage(MngInfo *mng_info, const ImageInfo *image_info,
%         ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadOneJNGImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o mng_info: Specifies a pointer to a MngInfo structure.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
*/
static Image *ReadOneJNGImage(MngInfo *mng_info,
                              const ImageInfo *image_info,
                              ExceptionInfo *exception)
{
  Image
    *alpha_image,
    *color_image,
    *image,
    *jng_image;

  ImageInfo
    *alpha_image_info,
    *color_image_info;

  long
    y;

  unsigned long
    jng_height,
    jng_width;

  png_byte
    jng_color_type,
    jng_image_sample_depth,
    jng_image_compression_method,
    jng_image_interlace_method,
    jng_alpha_sample_depth,
    jng_alpha_compression_method,
    jng_alpha_filter_method,
    jng_alpha_interlace_method;

  register const PixelPacket
    *s;

  register long
    x;

  register PixelPacket
    *q;

  register unsigned char
    *p;

  unsigned int
    logging,
    read_JSEP,
    reading_idat,
    status;

  unsigned long
    length;

  jng_alpha_compression_method=0;
  jng_alpha_sample_depth=8;
  jng_color_type=0;
  jng_height=0;
  jng_width=0;
  alpha_image=(Image *) NULL;
  color_image=(Image *) NULL;
  alpha_image_info=(ImageInfo *) NULL;
  color_image_info=(ImageInfo *) NULL;

  logging=LogMagickEvent(CoderEvent,GetMagickModule(),
                         "  enter ReadOneJNGImage()");

  image=mng_info->image;
  if (AccessMutablePixels(image) != (PixelPacket *) NULL)
    {
      /*
        Allocate next image structure.
      */
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "  AllocateNextImage()");
      AllocateNextImage(image_info,image);
      if (image->next == (Image *) NULL)
        return((Image *) NULL);
      image=SyncNextImageInList(image);
    }
  mng_info->image=image;

  /*
    Signature bytes have already been read.
  */

  read_JSEP=MagickFalse;
  reading_idat=MagickFalse;
  for (;;)
    {
      char
        type[MaxTextExtent];

      unsigned char
        *chunk;

      unsigned int
        count;

      /*
        Read a new JNG chunk.
      */

      if (QuantumTick(TellBlob(image),2*GetBlobSize(image)))
        if (!MagickMonitorFormatted(TellBlob(image),2*GetBlobSize(image),
                                    exception,LoadImagesTag,image->filename))
          break;

      type[0]='\0';
      (void) strcat(type,"errr");
      length=ReadBlobMSBLong(image);
      count=(unsigned int) ReadBlob(image,4,type);

      if (logging)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                " Reading JNG chunk type %c%c%c%c, length: %lu",
                                type[0],type[1],type[2],type[3],length);
      
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "   count=%u\n",count);
        }

      if (length > PNG_MAX_UINT || count == 0)
        {
          if (color_image_info != (ImageInfo *)NULL)
            {
              DestroyImageInfo(color_image_info);
            }
          if (alpha_image_info != (ImageInfo *)NULL)
            {
              DestroyImageInfo(alpha_image_info);
            }
          ThrowReaderException(CorruptImageError,CorruptImage,image);
        }

      chunk=(unsigned char *) NULL;
      p=NULL;
      if (length)
        {
          chunk=MagickAllocateMemory(unsigned char *,length);
          if (chunk == (unsigned char *) NULL)
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                 image);
          if (ReadBlob(image,length,chunk) < length)
            {
              if (color_image_info != (ImageInfo *)NULL)
                {
                  DestroyImageInfo(color_image_info);
                }
              if (alpha_image_info != (ImageInfo *)NULL)
                {
                  DestroyImageInfo(alpha_image_info);
                }
              ThrowReaderException(CorruptImageError,CorruptImage,image);
            }
          p=chunk;
        }
      (void) ReadBlobMSBLong(image);  /* read crc word */

      if (!memcmp(type,mng_JHDR,4))
        {
          if (length != 16)
            {
              (void) ThrowException2(&image->exception,CoderWarning,
                             "Invalid JHDR chunk length",(char *) NULL);
              return (MagickFail);
            }

          jng_width=(unsigned long) ((p[0] << 24) | (p[1] << 16) |
                                     (p[2] << 8) | p[3]);
          jng_height=(unsigned long) ((p[4] << 24) | (p[5] << 16) |
                                      (p[6] << 8) | p[7]);
          jng_color_type=p[8];
          jng_image_sample_depth=p[9];
          jng_image_compression_method=p[10];
          jng_image_interlace_method=p[11];
          jng_alpha_sample_depth=p[12];
          jng_alpha_compression_method=p[13];
          jng_alpha_filter_method=p[14];
          jng_alpha_interlace_method=p[15];
          if (logging)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "    jng_width:      %16lu",
                                    jng_width);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "    jng_height:     %16lu",
                                    jng_height);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "    jng_color_type: %16d",
                                    jng_color_type);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "    jng_image_sample_depth:      %3d",
                                    jng_image_sample_depth);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "    jng_image_compression_method:%3d",
                                    jng_image_compression_method);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "    jng_image_interlace_method:  %3d",
                                    jng_image_interlace_method);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "    jng_alpha_sample_depth:      %3d",
                                jng_alpha_sample_depth);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "    jng_alpha_compression_method:%3d",
                                    jng_alpha_compression_method);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "    jng_alpha_filter_method:     %3d",
                                    jng_alpha_filter_method);
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "    jng_alpha_interlace_method:  %3d",
                                    jng_alpha_interlace_method);
            }

          if (length)
            MagickFreeMemory(chunk);

          if (jng_width > 65535 || jng_height > 65535 ||
               (long) jng_width > GetMagickResourceLimit(WidthResource) ||
               (long) jng_height > GetMagickResourceLimit(HeightResource))
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "    JNG width or height too large: (%lu x %lu)",
                  jng_width, jng_height);
              ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
            }

          continue;
        }


      if (!reading_idat && !read_JSEP && (!memcmp(type,mng_JDAT,4) ||
                                          !memcmp(type,mng_JdAA,4) ||
                                          !memcmp(type,mng_IDAT,4) ||
                                          !memcmp(type,mng_JDAA,4)))
        {
          /*
            o create color_image
            o open color_blob, attached to color_image
            o if (color type has alpha)
            open alpha_blob, attached to alpha_image
          */

          color_image_info=MagickAllocateMemory(ImageInfo *,sizeof(ImageInfo));
          if (color_image_info == (ImageInfo *) NULL)
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                 image);
          GetImageInfo(color_image_info);
          color_image=AllocateImage(color_image_info);
          if (color_image == (Image *) NULL)
            ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                 image);

          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "    Creating color_blob.");
          (void) AcquireUniqueFilename(color_image->filename);
          status=OpenBlob(color_image_info,color_image,WriteBinaryBlobMode,
                          exception);
          if (status == MagickFalse)
            ThrowReaderException(CoderError,UnableToOpenBlob,color_image);

          if (!image_info->ping && jng_color_type >= 12)
            {
              alpha_image_info=MagickAllocateMemory(ImageInfo *,
                                                    sizeof(ImageInfo));
              if (alpha_image_info == (ImageInfo *) NULL)
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
              GetImageInfo(alpha_image_info);
              alpha_image=AllocateImage(alpha_image_info);
              if (alpha_image == (Image *) NULL)
                {
                  DestroyImage(alpha_image);
                  ThrowReaderException(ResourceLimitError,
                                       MemoryAllocationFailed,
                                       alpha_image);
                }
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "    Creating alpha_blob.");
              (void) AcquireUniqueFilename(alpha_image->filename);
              status=OpenBlob(alpha_image_info,alpha_image,WriteBinaryBlobMode,
                              exception);
              if (status == MagickFalse)
                ThrowReaderException(CoderError,UnableToOpenBlob,image);
              if (jng_alpha_compression_method == 0)
                {
                  unsigned char
                    data[18];

                  if (logging)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "    Writing IHDR chunk"
                                          " to alpha_blob.");
                  (void) WriteBlob(alpha_image,8,"\211PNG\r\n\032\n");
                  (void) WriteBlobMSBULong(alpha_image,13L);
                  PNGType(data,mng_IHDR);
                  LogPNGChunk(logging,mng_IHDR,13L);
                  PNGLong(data+4,jng_width);
                  PNGLong(data+8,jng_height);
                  data[12]=jng_alpha_sample_depth;
                  data[13]=0; /* color_type gray */
                  data[14]=0; /* compression method 0 */
                  data[15]=0; /* filter_method 0 */
                  data[16]=0; /* interlace_method 0 */
                  (void) WriteBlob(alpha_image,17,data);
                  (void) WriteBlobMSBULong(alpha_image,crc32(0,data,17));
                }
            }
          reading_idat=MagickTrue;
        }

      if (!memcmp(type,mng_JDAT,4))
        {
          /*
            Copy chunk to color_image->blob
          */

          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "    Copying JDAT chunk data"
                                  " to color_blob.");
          if (color_image != (Image *)NULL)
            (void) WriteBlob(color_image,length,(char *) chunk);
          if (length)
            MagickFreeMemory(chunk);
          continue;
        }

      if (!memcmp(type,mng_IDAT,4))
        {
          png_byte
            data[5];

          /*
            Copy IDAT header and chunk data to alpha_image->blob
          */

          if (alpha_image != NULL && image_info->ping == MagickFalse)
            {
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "    Copying IDAT chunk data"
                                      " to alpha_blob.");

              (void) WriteBlobMSBULong(alpha_image,(unsigned long) length);
              PNGType(data,mng_IDAT);
              LogPNGChunk(logging,mng_IDAT,length);
              (void) WriteBlob(alpha_image,4,(char *) data);
              (void) WriteBlob(alpha_image,length,(char *) chunk);
              (void) WriteBlobMSBULong(alpha_image,
                                       crc32(crc32(0,data,4),chunk,length));
            }
          if (length)
            MagickFreeMemory(chunk);
          continue;
        }

      if (!memcmp(type,mng_JDAA,4) || !memcmp(type,mng_JdAA,4))
        {
          /*
            Copy chunk data to alpha_image->blob
          */

          if (alpha_image != NULL && image_info->ping == MagickFalse)
            {
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "    Copying JDAA chunk data"
                                      " to alpha_blob.");

              (void) WriteBlob(alpha_image,length,(char *) chunk);
            }
          if (length)
            MagickFreeMemory(chunk);
          continue;
        }

      if (!memcmp(type,mng_JSEP,4))
        {
          read_JSEP=MagickTrue;
          if (length)
            MagickFreeMemory(chunk);
          continue;
        }

      if (!memcmp(type,mng_bKGD,4))
        {
          if (length == 2)
            {
              image->background_color.red=ScaleCharToQuantum(p[1]);
              image->background_color.green=image->background_color.red;
              image->background_color.blue=image->background_color.red;
            }
          if (length == 6)
            {
              image->background_color.red=ScaleCharToQuantum(p[1]);
              image->background_color.green=ScaleCharToQuantum(p[3]);
              image->background_color.blue=ScaleCharToQuantum(p[5]);
            }
          MagickFreeMemory(chunk);
          continue;
        }

      if (!memcmp(type,mng_gAMA,4))
        {
          if (length == 4)
            image->gamma=((float) mng_get_long(p))*0.00001;
          MagickFreeMemory(chunk);
          continue;
        }

      if (!memcmp(type,mng_cHRM,4))
        {
          if (length == 32)
            {
              image->chromaticity.white_point.x=0.00001*mng_get_long(p);
              image->chromaticity.white_point.y=0.00001*mng_get_long(&p[4]);
              image->chromaticity.red_primary.x=0.00001*mng_get_long(&p[8]);
              image->chromaticity.red_primary.y=0.00001*mng_get_long(&p[12]);
              image->chromaticity.green_primary.x=0.00001*mng_get_long(&p[16]);
              image->chromaticity.green_primary.y=0.00001*mng_get_long(&p[20]);
              image->chromaticity.blue_primary.x=0.00001*mng_get_long(&p[24]);
              image->chromaticity.blue_primary.y=0.00001*mng_get_long(&p[28]);
            }
          MagickFreeMemory(chunk);
          continue;
        }

      if (!memcmp(type,mng_sRGB,4))
        {
          if (length == 1)
            {
              image->rendering_intent=(RenderingIntent) (p[0]+1);
              image->gamma=0.45455f;
              image->chromaticity.red_primary.x=0.6400f;
              image->chromaticity.red_primary.y=0.3300f;
              image->chromaticity.green_primary.x=0.3000f;
              image->chromaticity.green_primary.y=0.6000f;
              image->chromaticity.blue_primary.x=0.1500f;
              image->chromaticity.blue_primary.y=0.0600f;
              image->chromaticity.white_point.x=0.3127f;
              image->chromaticity.white_point.y=0.3290f;
            }
          MagickFreeMemory(chunk);
          continue;
        }

      if (!memcmp(type,mng_oFFs,4))
        {
          if (length > 8)
            {
              image->page.x=mng_get_long(p);
              image->page.y=mng_get_long(&p[4]);
              if ((int) p[9] != 0)
                {
                  image->page.x/=10000;
                  image->page.y/=10000;
                }
            }
          if (length)
            MagickFreeMemory(chunk);
          continue;
        }

      if (!memcmp(type,mng_pHYs,4))
        {
          if (length > 8)
            {
              image->x_resolution=(double) mng_get_long(p);
              image->y_resolution=(double) mng_get_long(&p[4]);
              if ((int) p[8] == PNG_RESOLUTION_METER)
                {
                  image->units=PixelsPerCentimeterResolution;
                  image->x_resolution=image->x_resolution/100.0f;
                  image->y_resolution=image->y_resolution/100.0f;
                }
            }
          MagickFreeMemory(chunk);
          continue;
        }

#if 0
      if (!memcmp(type,mng_iCCP,4))
        {
          /* To do. */
          if (length)
            MagickFreeMemory(chunk);
          continue;
        }
#endif

      if (length)
        MagickFreeMemory(chunk);

      if (memcmp(type,mng_IEND,4))
        continue;
      break;
    }


  /* IEND found */

  /*
    Finish up reading image data:

    o read main image from color_blob.

    o close color_blob.

    o if (color_type has alpha)
    if alpha_encoding is PNG
    read secondary image from alpha_blob via ReadPNG
    if alpha_encoding is JPEG
    read secondary image from alpha_blob via ReadJPEG

    o close alpha_blob.

    o copy intensity of secondary image into
    opacity samples of main image.

    o destroy the secondary image.
  */

  if (color_image != (Image *)NULL)
    {
      CloseBlob(color_image);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "    Reading jng_image from color_blob.");

      FormatString(color_image_info->filename,"%.1024s",color_image->filename);

      color_image_info->ping=MagickFalse;   /* To do: avoid this */
      jng_image=ReadImage(color_image_info,exception);
      (void) LiberateUniqueFileResource(color_image->filename);
      DestroyImage(color_image);
      color_image = (Image *)NULL;
      DestroyImageInfo(color_image_info);
      color_image_info = (ImageInfo *)NULL;

      if (jng_image == (Image *) NULL)
        {
          /*
            Don't throw exception here since ReadImage() will already
            have thrown it.
          */
          return (Image *) NULL;
        }

      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "    Copying jng_image pixels to main image.");
      image->rows=jng_height;
      image->columns=jng_width;
      length=image->columns*sizeof(PixelPacket);
      for (y=0; y < (long) image->rows; y++)
        {
          s=AcquireImagePixels(jng_image,0,y,image->columns,1,&image->exception);
          q=SetImagePixels(image,0,y,image->columns,1);
          (void) memcpy(q,s,length);
          if (!SyncImagePixels(image))
            break;
        }
      DestroyImage(jng_image);
      jng_image = (Image *)NULL;
      if (alpha_image != (Image *)NULL && !image_info->ping)
        {
          if (jng_color_type >= 12)
            {
              if (jng_alpha_compression_method == 0)
                {
                  png_byte
                    data[5];
                  (void) WriteBlobMSBULong(alpha_image,0x00000000L);
                  PNGType(data,mng_IEND);
                  LogPNGChunk(logging,mng_IEND,0L);
                  (void) WriteBlob(alpha_image,4,(char *) data);
                  (void) WriteBlobMSBULong(alpha_image,crc32(0,data,4));
                }
              CloseBlob(alpha_image);
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "    Reading opacity from alpha_blob.");

              FormatString(alpha_image_info->filename,"%.1024s",
                           alpha_image->filename);

              jng_image=ReadImage(alpha_image_info,exception);

              for (y=0; y < (long) image->rows; y++)
                {
                  s=AcquireImagePixels(jng_image,0,y,image->columns,1,
                                       &image->exception);
                  if (image->matte)
                    {
                      q=SetImagePixels(image,0,y,image->columns,1);
                      for (x=(long) image->columns; x > 0; x--,q++,s++)
                        q->opacity=(Quantum) MaxRGB-s->red;
                    }
                  else
                    {
                      q=SetImagePixels(image,0,y,image->columns,1);
                      for (x=(long) image->columns; x > 0; x--,q++,s++)
                        {
                          q->opacity=(Quantum) MaxRGB-s->red;
                          if (q->opacity != OpaqueOpacity)
                            image->matte=MagickTrue;
                        }
                    }
                  if (!SyncImagePixels(image))
                    break;
                }
              (void) LiberateUniqueFileResource(alpha_image->filename);
              DestroyImage(alpha_image);
              alpha_image = (Image *)NULL;
              DestroyImageInfo(alpha_image_info);
              alpha_image_info = (ImageInfo *)NULL;
              DestroyImage(jng_image);
              jng_image = (Image *)NULL;
            }
        }

      /*
        Read the JNG image.
      */
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          " Read the JNG image");
      image->page.width=jng_width;
      image->page.height=jng_height;
      image->page.x=mng_info->x_off[mng_info->object_id];
      image->page.y=mng_info->y_off[mng_info->object_id];
      mng_info->image_found++;
      if (QuantumTick(2*GetBlobSize(image),2*GetBlobSize(image)))
        (void) MagickMonitorFormatted(2*GetBlobSize(image),2*GetBlobSize(image),
                                      exception,LoadImagesTag,image->filename);
    }

  /* Clean up in case we didn't earlier */
  if (alpha_image != (Image *)NULL)
    {
      (void) LiberateUniqueFileResource(alpha_image->filename);
      DestroyImage(alpha_image);
    }
  if (alpha_image_info != (ImageInfo *)NULL)
    DestroyImageInfo(alpha_image_info);

  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  exit ReadOneJNGImage()");
  return (image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d J N G I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadJNGImage reads a JPEG Network Graphics (JNG) image file
%  (including the 8-byte signature)  and returns it.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.
%
%  JNG support written by Glenn Randers-Pehrson, randeg@alum.rpi.edu.
%
%  The format of the ReadJNGImage method is:
%
%      Image *ReadJNGImage(const ImageInfo *image_info, ExceptionInfo
%         *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadJNGImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
*/

static Image *ReadJNGImage(const ImageInfo *image_info,
                           ExceptionInfo *exception)
{
  Image
    *image;

  MngInfo
    *mng_info;

  char
    magic_number[MaxTextExtent];

  int
    have_mng_structure,
    logging;

  unsigned int
    status;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"enter ReadJNGImage()");
  image=AllocateImage(image_info);
  mng_info=(MngInfo *) NULL;
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  if (LocaleCompare(image_info->magick,"JNG") != 0)
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  /*
    Verify JNG signature.
  */
  if ((ReadBlob(image,8,magic_number) != 8) ||
      (memcmp(magic_number,"\213JNG\r\n\032\n",8) != 0))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  /*
    Allocate a MngInfo structure.
  */
  have_mng_structure=MagickFalse;
  mng_info=MagickAllocateMemory(MngInfo *,sizeof(MngInfo));
  if (mng_info == (MngInfo *) NULL)
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  /*
    Initialize members of the MngInfo structure.
  */
  (void) memset(mng_info,0,sizeof(MngInfo));
  have_mng_structure=MagickTrue;

  mng_info->image=image;
  image=ReadOneJNGImage(mng_info,image_info,exception);
  MngInfoFreeStruct(mng_info,&have_mng_structure);
  if (image == (Image *) NULL)
    {
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "exit ReadJNGImage() with error");
      return((Image *) NULL);
    }
  CloseBlob(image);
  if (image->columns == 0 || image->rows == 0)
    {
      DestroyImageList(image);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "exit ReadJNGImage() with error");
      return((Image *) NULL);
    }
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"exit ReadJNGImage()");
  return (image);
}
#endif

static Image *ReadMNGImage(const ImageInfo *image_info,
                           ExceptionInfo *exception)
{
  char
    page_geometry[MaxTextExtent];

  Image
    *image,
    *previous;

  int
    have_mng_structure;

  volatile int
    first_mng_object,
    logging,
    object_id,
    term_chunk_found,
    skip_to_iend;

  volatile long
    image_count=0;

  MngInfo
    *mng_info;

  MngBox
    default_fb,
    fb,
    previous_fb;

#ifdef MNG_INSERT_LAYERS
  PixelPacket
    mng_background_color;
#endif

  register long
    i;

  size_t
    count;

  short
    loop_level,
    loops_active = 0;

  volatile short
    skipping_loop;

  unsigned int
#ifdef MNG_INSERT_LAYERS
    mandatory_back=0,
#endif
    status;

  volatile unsigned int
#ifdef MNG_OBJECT_BUFFERS
    mng_background_object=0,
#endif
    mng_type=0;   /* 0: PNG or JNG; 1: MNG; 2: MNG-LC; 3: MNG-VLC */

  unsigned long
    default_frame_timeout,
    frame_timeout,
#ifdef MNG_INSERT_LAYERS
    image_height,
    image_width,
#endif
    length;

  volatile unsigned long
    default_frame_delay,
    final_delay,
    final_image_delay,
    frame_delay,
#ifdef MNG_INSERT_LAYERS
    insert_layers,
#endif
    mng_iterations=1,
    simplicity=0,
    subframe_height=0,
    subframe_width=0;
  previous_fb.top=0;
  previous_fb.bottom=0;
  previous_fb.left=0;
  previous_fb.right=0;
  default_fb.top=0;
  default_fb.bottom=0;
  default_fb.left=0;
  default_fb.right=0;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"enter ReadMNGImage()");
  image=AllocateImage(image_info);
  mng_info=(MngInfo *) NULL;
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  first_mng_object=MagickFalse;
  skipping_loop=(-1);
  have_mng_structure=MagickFalse;
  /*
    Allocate a MngInfo structure.
  */
  mng_info=MagickAllocateMemory(MngInfo *,sizeof(MngInfo));
  if (mng_info == (MngInfo *) NULL)
    ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
  /*
    Initialize members of the MngInfo structure.
  */
  (void) memset(mng_info,0,sizeof(MngInfo));
  mng_info->image=image;
  have_mng_structure=MagickTrue;

  if (LocaleCompare(image_info->magick,"MNG") == 0)
    {
      char
        magic_number[MaxTextExtent];

      /*
        Verify MNG signature.
      */
      (void) ReadBlob(image,8,magic_number);
      if (memcmp(magic_number,"\212MNG\r\n\032\n",8) != 0)
        {
          MngInfoFreeStruct(mng_info,&have_mng_structure);
          ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
        }
      /*
        Initialize some nonzero members of the MngInfo structure.
      */
      for (i=0; i < MNG_MAX_OBJECTS; i++)
        {
          mng_info->object_clip[i].right=PNG_MAX_UINT;
          mng_info->object_clip[i].bottom=PNG_MAX_UINT;
        }
      mng_info->exists[0]=MagickTrue;
    }
  first_mng_object=MagickTrue;
  mng_type=0;
#ifdef MNG_INSERT_LAYERS
  insert_layers=MagickFalse;  /* should be False if converting or mogrifying */
#endif
  default_frame_delay=0;
  default_frame_timeout=0;
  frame_delay=0;
  final_delay=1;
  mng_info->ticks_per_second=100;
  object_id=0;
  skip_to_iend=MagickFalse;
  term_chunk_found=MagickFalse;
  mng_info->framing_mode=1;
#ifdef MNG_INSERT_LAYERS
  mandatory_back=MagickFalse;
#endif
#ifdef MNG_INSERT_LAYERS
  mng_background_color=image->background_color;
#endif
  do
    {
      char
        type[MaxTextExtent];

      if (LocaleCompare(image_info->magick,"MNG") == 0)
        {
          register unsigned char
            *p;

          unsigned char
            *chunk;

          /*
            Read a new chunk.
          */
          type[0]='\0';
          (void) strcat(type,"errr");
          length=ReadBlobMSBLong(image);
          count=ReadBlob(image,4,type);
          if (count < 4)
            {
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "  Reading MNG chunk, count: %"
                                      MAGICK_SIZE_T_F  "u",
                                      (MAGICK_SIZE_T) count);
              MngInfoFreeStruct(mng_info,&have_mng_structure);
              ThrowReaderException(CorruptImageError,CorruptImage,image);
            }

          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "  Reading MNG chunk type %c%c%c%c,"
                                  " length: %lu",
                                  type[0],type[1],type[2],type[3],length);

          if (length > PNG_MAX_UINT)
            status=MagickFalse;
          p=NULL;
          chunk=(unsigned char *) NULL;
          if (length)
            {
              chunk=MagickAllocateMemory(unsigned char *,length);
              if (chunk == (unsigned char *) NULL)
                ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                                     image);
              if (ReadBlob(image,length,chunk) < length)
                {
                  MngInfoFreeStruct(mng_info,&have_mng_structure);
                  ThrowReaderException(CorruptImageError,CorruptImage,image);
                }
              p=chunk;
            }
          (void) ReadBlobMSBLong(image);  /* read crc word */

#if !defined(JNG_SUPPORTED)
          if (!memcmp(type,mng_JHDR,4))
            {
              skip_to_iend=MagickTrue;
              if (!mng_info->jhdr_warning)
                (void) ThrowException(&image->exception,CoderError,
                                      JNGCompressionNotSupported,
                                      image->filename);
              mng_info->jhdr_warning++;
            }
#endif
          if (!memcmp(type,mng_DHDR,4))
            {
              skip_to_iend=MagickTrue;
              if (!mng_info->dhdr_warning)
                (void) ThrowException(&image->exception,CoderError,
                                      DeltaPNGNotSupported,image->filename);
              mng_info->dhdr_warning++;
            }
          if (!memcmp(type,mng_MEND,4))
            break;
          if (skip_to_iend)
            {
              if (!memcmp(type,mng_IEND,4))
                skip_to_iend=MagickFalse;
              if (length)
                MagickFreeMemory(chunk);
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "  Skip to IEND.");
              continue;
            }
          if (!memcmp(type,mng_MHDR,4))
            {
              if (length < 16)
                {
                  ThrowReaderException(CorruptImageError,CorruptImage,image);
                }
#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
              /* To quiet a Coverity complaint */
              LockSemaphoreInfo(png_semaphore);
#endif
              mng_info->mng_width=(unsigned long) ((p[0] << 24) |
                                                   (p[1] << 16) |
                                                   (p[2] << 8) | p[3]);
              mng_info->mng_height=(unsigned long) ((p[4] << 24) |
                                                    (p[5] << 16) |
                                                    (p[6] << 8) | p[7]);
              if (logging)
                {
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "  MNG width: %lu",
                                        mng_info->mng_width);
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "  MNG height: %lu",
                                        mng_info->mng_height);
                }
#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
              UnlockSemaphoreInfo(png_semaphore);
#endif
              if ((long) mng_info->mng_width >
                  GetMagickResourceLimit(WidthResource) ||
                  (long) mng_info->mng_height >
                  GetMagickResourceLimit(HeightResource))
                {
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                      "   MNG width or height too large: %lu, %lu",
                      mng_info->mng_width,mng_info->mng_height);
                  ThrowReaderException(CorruptImageError,ImproperImageHeader,
                      image);
                }

              p+=8;
              mng_info->ticks_per_second=mng_get_long(p);
              if (mng_info->ticks_per_second == 0)
                default_frame_delay=0;
              else
                default_frame_delay=100/mng_info->ticks_per_second;
              frame_delay=default_frame_delay;
              simplicity=0;
              if (length >= 28)
                {
                  p+=16;
                  simplicity=mng_get_long(p);
                }
              if (logging)
                {
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "  MNG simplicity: %lx",simplicity);
                }
              mng_type=1;    /* Full MNG */
              if ((simplicity != 0) && ((simplicity | 11) == 11))
                mng_type=2; /* LC */
              if ((simplicity != 0) && ((simplicity | 9) == 9))
                mng_type=3; /* VLC */
#ifdef MNG_INSERT_LAYERS
              if (mng_type != 3 && !image_info->ping)
                insert_layers=MagickTrue;
#endif
              if (AccessMutablePixels(image) != (PixelPacket *) NULL)
                {
                  /*
                    Allocate next image structure.
                  */
                  AllocateNextImage(image_info,image);
                  if (image->next == (Image *) NULL)
                    return((Image *) NULL);
                  image=SyncNextImageInList(image);
                  mng_info->image=image;
                }

              if ((mng_info->mng_width > 65535L) || (mng_info->mng_height
                                                     > 65535L))
                (void) ThrowException(&image->exception,ImageError,
                                      WidthOrHeightExceedsLimit,
                                      image->filename);
              FormatString(page_geometry,"%lux%lu+0+0",mng_info->mng_width,
                           mng_info->mng_height);
              mng_info->frame.left=0;
              mng_info->frame.right=(long) mng_info->mng_width;
              mng_info->frame.top=0;
              mng_info->frame.bottom=(long) mng_info->mng_height;
#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
              /* To quiet a Coverity complaint */
              LockSemaphoreInfo(png_semaphore);
#endif
              mng_info->clip=default_fb=previous_fb=mng_info->frame;
#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
              UnlockSemaphoreInfo(png_semaphore);
#endif
              for (i=0; i < MNG_MAX_OBJECTS; i++)
                mng_info->object_clip[i]=mng_info->frame;
              MagickFreeMemory(chunk);
              continue;
            }

          if (!memcmp(type,mng_TERM,4))
            {
              int
                repeat=0;

              if (length)
                repeat=p[0];
              if (repeat == 3 && length > 8)
                {
                  final_delay=(png_uint_32) mng_get_long(&p[2]);
                  mng_iterations=(png_uint_32) mng_get_long(&p[6]);
                  if (mng_iterations == PNG_MAX_UINT)
                    mng_iterations=0;
                  image->iterations=mng_iterations;
                  term_chunk_found=MagickTrue;
                }
              if (logging)
                {
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "    repeat=%d",repeat);
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "    final_delay=%ld",final_delay);
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "    image->iterations=%ld",
                                        image->iterations);
                }
              MagickFreeMemory(chunk);
              continue;
            }
          if (!memcmp(type,mng_DEFI,4))
            {
              if (mng_type == 3)
                (void) ThrowException2(&image->exception,CoderError,
                                       "DEFI chunk found in MNG-VLC"
                                       " datastream",
                                       (char *) NULL);
              object_id=(p[0] << 8) | p[1];
              if (mng_type == 2 && object_id != 0)
                (void) ThrowException2(&image->exception,CoderError,
                                       "Nonzero object_id in MNG-LC"
                                       " datastream",
                                       (char *) NULL);
              if (object_id > MNG_MAX_OBJECTS)
                {
                  /*
                    Instead of issuing a warning we should allocate a larger
                    MngInfo structure and continue.
                  */
                  (void) ThrowException2(&image->exception,CoderError,
                                         "object id too large",(char *) NULL);
                  object_id=MNG_MAX_OBJECTS;
                }
              if (mng_info->exists[object_id])
                if (mng_info->frozen[object_id])
                  {
                    MagickFreeMemory(chunk);
                    (void) ThrowException2(&image->exception,CoderError,
                                           "DEFI cannot redefine a frozen"
                                           " MNG object",
                                           (char *) NULL);
                    continue;
                  }
              mng_info->exists[object_id]=MagickTrue;
              if (length > 2)
                mng_info->invisible[object_id]=p[2];
              /*
                Extract object offset info.
              */
              if (length > 11)
                {
                  mng_info->x_off[object_id]=(long) ((p[4] << 24) |
                                                     (p[5] << 16) |
                                                     (p[6] << 8) | p[7]);
                  mng_info->y_off[object_id]=(long) ((p[8] << 24) |
                                                     (p[9] << 16) |
                                                     (p[10] << 8) | p[11]);
                  if (logging)
                    {
                      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                            "  x_off[%d]: %lu",object_id,
                                            mng_info->x_off[object_id]);
                      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                            "  y_off[%d]: %lu",object_id,
                                            mng_info->y_off[object_id]);
                    }
                }
              /*
                Extract object clipping info.
              */
              if (length > 27)
                mng_info->object_clip[object_id]
                   =mng_read_box( mng_info->frame,0,&p[12]);
              MagickFreeMemory(chunk);
              continue;
            }
          if (!memcmp(type,mng_bKGD,4))
            {
              mng_info->have_global_bkgd=MagickFalse;
              if (length > 5)
                {
                  mng_info->mng_global_bkgd.red
                    =ScaleShortToQuantum(((p[0] << 8) | p[1]));
                  mng_info->mng_global_bkgd.green
                    =ScaleShortToQuantum(((p[2] << 8) | p[3]));
                  mng_info->mng_global_bkgd.blue
                    =ScaleShortToQuantum(((p[4] << 8) | p[5]));
                  mng_info->have_global_bkgd=MagickTrue;
                }
              MagickFreeMemory(chunk);
              continue;
            }
          if (!memcmp(type,mng_BACK,4))
            {
#ifdef MNG_INSERT_LAYERS
              if (length > 6)
                mandatory_back=p[6];
              else
                mandatory_back=0;
              if (mandatory_back && length > 5)
                {
                  mng_background_color.red=
                    ScaleShortToQuantum(((p[0] << 8) | p[1]));
                  mng_background_color.green=
                    ScaleShortToQuantum(((p[2] << 8) | p[3]));
                  mng_background_color.blue=
                    ScaleShortToQuantum(((p[4] << 8) | p[5]));
                  mng_background_color.opacity=OpaqueOpacity;
                }
#ifdef MNG_OBJECT_BUFFERS
              if (length > 8)
                mng_background_object=(p[7] << 8) | p[8];
#endif
#endif
              MagickFreeMemory(chunk);
              continue;
            }
          if (!memcmp(type,mng_PLTE,4))
            {
              /*
                Read global PLTE.
              */
              if (length && (length < 769))
                {
                  if (mng_info->global_plte == (png_colorp) NULL)
                    mng_info->global_plte=
                      MagickAllocateMemory(png_colorp,256*sizeof(png_color));
                  for (i=0; i < (long) (length/3); i++)
                    {
                      mng_info->global_plte[i].red=p[3*i];
                      mng_info->global_plte[i].green=p[3*i+1];
                      mng_info->global_plte[i].blue=p[3*i+2];
                    }
                  mng_info->global_plte_length=length/3;
                }
#ifdef MNG_LOOSE
              for ( ; i < 256; i++)
                {
                  mng_info->global_plte[i].red=i;
                  mng_info->global_plte[i].green=i;
                  mng_info->global_plte[i].blue=i;
                }
              if (length)
                mng_info->global_plte_length=256;
#endif
              else
                mng_info->global_plte_length=0;
              MagickFreeMemory(chunk);
              continue;
            }
          if (!memcmp(type,mng_tRNS,4))
            {
              /* read global tRNS */

              if (length < 257)
                for (i=0; i < (long) length; i++)
                  mng_info->global_trns[i]=p[i];

#ifdef MNG_LOOSE
              for ( ; i < 256; i++)
                mng_info->global_trns[i]=255;
#endif
              mng_info->global_trns_length=length;
              MagickFreeMemory(chunk);
              continue;
            }
          if (!memcmp(type,mng_gAMA,4))
            {
              if (length == 4)
                {
                  long
                    igamma;

                  igamma=mng_get_long(p);
                  mng_info->global_gamma=((float) igamma)*0.00001;
                  mng_info->have_global_gama=MagickTrue;
                }
              else
                mng_info->have_global_gama=MagickFalse;
              MagickFreeMemory(chunk);
              continue;
            }

          if (!memcmp(type,mng_cHRM,4))
            {
              /*
                Read global cHRM
              */
              if (length == 32)
                {
                  mng_info->global_chrm.white_point.x=0.00001*
                    mng_get_long(p);
                  mng_info->global_chrm.white_point.y=0.00001*
                    mng_get_long(&p[4]);
                  mng_info->global_chrm.red_primary.x=0.00001*
                    mng_get_long(&p[8]);
                  mng_info->global_chrm.red_primary.y=0.00001*
                    mng_get_long(&p[12]);
                  mng_info->global_chrm.green_primary.x=0.00001*
                    mng_get_long(&p[16]);
                  mng_info->global_chrm.green_primary.y=0.00001*
                    mng_get_long(&p[20]);
                  mng_info->global_chrm.blue_primary.x=0.00001*
                    mng_get_long(&p[24]);
                  mng_info->global_chrm.blue_primary.y=0.00001*
                    mng_get_long(&p[28]);
                  mng_info->have_global_chrm=MagickTrue;
                }
              else
                mng_info->have_global_chrm=MagickFalse;
              MagickFreeMemory(chunk);
              continue;
            }
          if (!memcmp(type,mng_sRGB,4))
            {
              /*
                Read global sRGB.
              */
              if (length)
                {
                  mng_info->global_srgb_intent=(RenderingIntent) (p[0]+1);
                  mng_info->have_global_srgb=MagickTrue;
                }
              else
                mng_info->have_global_srgb=MagickFalse;
              MagickFreeMemory(chunk);
              continue;
            }
          if (!memcmp(type,mng_iCCP,4))
            {
              /* To do. */

              /*
                Read global iCCP.
              */
              if (length)
                MagickFreeMemory(chunk);
              continue;
            }
          if (!memcmp(type,mng_FRAM,4))
            {
              if (mng_type == 3)
                (void) ThrowException2(&image->exception,CoderError,
                                       "FRAM chunk found in MNG-VLC"
                                       " datastream",
                                       (char *) NULL);
              if ((mng_info->framing_mode == 2) ||
                  (mng_info->framing_mode == 4))
                image->delay=frame_delay;
              frame_delay=default_frame_delay;
              frame_timeout=default_frame_timeout;
              fb=default_fb;
              if (length)
                if (p[0])
                  mng_info->framing_mode=p[0];
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "    Framing_mode=%d",
                                      mng_info->framing_mode);
              if (length > 6)
                {
                  /*
                    Note the delay and frame clipping boundaries.
                  */
                  p++; /* framing mode */
                  while (*p && ((p-chunk) < (long) length))
                    p++;  /* frame name */
                  p++;  /* frame name terminator */
                  if ((p-chunk) < (long) (length-4))
                    {
                      int
                        change_delay,
                        change_timeout,
                        change_clipping;

                      change_delay=(*p++);
                      change_timeout=(*p++);
                      change_clipping=(*p++);
                      p++; /* change_sync */
                      if (change_delay && (p-chunk) < (ssize_t) (length-4))
                        {
                          frame_delay=(100*(mng_get_long(p))/
                                       mng_info->ticks_per_second);
                          if (change_delay == 2)
                            default_frame_delay=frame_delay;
                          p+=4;
                          if (logging)
                            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                                  "    Framing_delay=%ld",
                                                  frame_delay);
                        }
                      if (change_timeout && (p-chunk) < (ssize_t) (length-4))
                        {
                          frame_timeout=
                            (100*(mng_get_long(p))/mng_info->ticks_per_second);
                          if (change_timeout == 2)
                            default_frame_timeout=frame_timeout;
                          p+=4;
                          if (logging)
                            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                                  "    Framing_timeout=%ld",
                                                  frame_timeout);
                        }
                      if (change_clipping && (p-chunk) < (ssize_t) (length-17))
                        {
                          fb=mng_read_box(previous_fb,p[0],&p[1]);
                          p+=17;
                          previous_fb=fb;
                          if (logging)
                            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                                  "    Frame_clipping:"
                                                  " L=%ld R=%ld T=%ld B=%ld",
                                                  fb.left, fb.right,fb.top,
                                                  fb.bottom);
                          if (change_clipping == 2)
                            default_fb=fb;
                        }
                    }
                }
#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
              /* To quiet a Coverity complaint */
              LockSemaphoreInfo(png_semaphore);
#endif
              mng_info->clip=fb;
              mng_info->clip=mng_minimum_box(fb,mng_info->frame);
#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
              UnlockSemaphoreInfo(png_semaphore);
#endif
              subframe_width=(mng_info->clip.right-mng_info->clip.left);
              subframe_height=(mng_info->clip.bottom-mng_info->clip.top);
              /*
                Insert a background layer behind the frame
                if framing_mode is 4.
              */
#ifdef MNG_INSERT_LAYERS
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "   subframe_width=%lu,"
                                      " subframe_height=%lu",
                                      subframe_width, subframe_height);
              if (insert_layers && (mng_info->framing_mode == 4) &&
                  (subframe_width) && (subframe_height))
                {
                  /*
                    Allocate next image structure.
                  */
                  if (AccessMutablePixels(image) != (PixelPacket *) NULL)
                    {
                      AllocateNextImage(image_info,image);
                      if (image->next == (Image *) NULL)
                        {
                          DestroyImageList(image);
                          MngInfoFreeStruct(mng_info,&have_mng_structure);
                          return((Image *) NULL);
                        }
                      image=SyncNextImageInList(image);
                    }
                  mng_info->image=image;
                  if (term_chunk_found)
                    {
                      image->start_loop=MagickTrue;
                      image->iterations=mng_iterations;
                      term_chunk_found=MagickFalse;
                    }
                  else
                    image->start_loop=MagickFalse;
                  image->columns=subframe_width;
                  image->rows=subframe_height;
                  image->page.width=subframe_width;
                  image->page.height=subframe_height;
                  image->page.x=mng_info->clip.left;
                  image->page.y=mng_info->clip.top;
                  image->background_color=mng_background_color;
                  image->matte=MagickFalse;
                  image->delay=0;
                  (void) SetImage(image,OpaqueOpacity);
                  if (logging)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "  Inserted background layer,"
                                          " L=%ld, R=%ld, T=%ld, B=%ld",
                                          mng_info->clip.left,
                                          mng_info->clip.right,
                                          mng_info->clip.top,
                                          mng_info->clip.bottom);
                }
#endif
              MagickFreeMemory(chunk);
              continue;
            }
          if (!memcmp(type,mng_CLIP,4))
            {
              unsigned int
                first_object,
                last_object;

              /*
                Read CLIP.
              */
              if (length > 3)
                {
                  first_object=(p[0] << 8) | p[1];
                  last_object=(p[2] << 8) | p[3];
                  p+=4;

                  for (i=(int) first_object; i <= (int) last_object; i++)
                    {
                      if (mng_info->exists[i] && !mng_info->frozen[i])
                        {
                          MngBox
                            box;

                          box=mng_info->object_clip[i];
                          if ((p-chunk) < (ssize_t) (length-17))
                            mng_info->object_clip[i]=
                                mng_read_box(box,p[0],&p[1]);
                        }
                    }
                }
              MagickFreeMemory(chunk);
              continue;
            }
          if (!memcmp(type,mng_SAVE,4))
            {
              for (i=1; i < MNG_MAX_OBJECTS; i++)
                if (mng_info->exists[i])
                  {
                    mng_info->frozen[i]=MagickTrue;
#ifdef MNG_OBJECT_BUFFERS
                    if (mng_info->ob[i] != (MngBuffer *) NULL)
                      mng_info->ob[i]->frozen=MagickTrue;
#endif
                  }
              if (length)
                MagickFreeMemory(chunk);
              continue;
            }

          if (!memcmp(type,mng_DISC,4) || !memcmp(type,mng_SEEK,4))
            {
              /*
                Read DISC or SEEK.
              */
              if ((length == 0) || !memcmp(type,mng_SEEK,4))
                {
                  for (i=1; i < MNG_MAX_OBJECTS; i++)
                    MngInfoDiscardObject(mng_info,i);
                }
              else
                {
                  register long
                    j;

                  for (j=0; j < (long) length; j+=2)
                    {
                      i=p[j] << 8 | p[j+1];
                      MngInfoDiscardObject(mng_info,i);
                    }
                }
              if (length)
                MagickFreeMemory(chunk);
              continue;
            }
          if (!memcmp(type,mng_MOVE,4))
            {
              unsigned long
                first_object,
                last_object;

              /*
                read MOVE
              */

              if (length > 3)
                {
                  first_object=(p[0] << 8) | p[1];
                  last_object=(p[2] << 8) | p[3];
                  p+=4;

                  for (i=(long) first_object; i <= (long) last_object; i++)
                    {
                      if (mng_info->exists[i] && !mng_info->frozen[i] &&
                          (p-chunk) < (ssize_t) (length-8))
                        {
                          MngPair
                            new_pair;

                          MngPair
                            old_pair;

                          old_pair.a=mng_info->x_off[i];
                          old_pair.b=mng_info->y_off[i];
                          new_pair=mng_read_pair(old_pair,(int) p[0],&p[1]);
                          mng_info->x_off[i]=new_pair.a;
                          mng_info->y_off[i]=new_pair.b;
                        }
                    }
                }
              MagickFreeMemory(chunk);
              continue;
            }

          if (!memcmp(type,mng_LOOP,4))
            {
              long loop_iters=1;

              if (length > 0) /* To do: check spec, if empty LOOP is allowed */
                {
                  loop_level=chunk[0];
                  loops_active++;
                  mng_info->loop_active[loop_level]=1;  /* mark loop active */
                  /*
                    Record starting point.
                  */
                  loop_iters=mng_get_long(&chunk[1]);
                  if (loop_iters == 0)
                    skipping_loop=loop_level;
                  else
                    {
                      mng_info->loop_jump[loop_level]=TellBlob(image);
                      mng_info->loop_count[loop_level]=loop_iters;
                    }
                  mng_info->loop_iteration[loop_level]=0;
                }
              MagickFreeMemory(chunk);
              continue;
            }
          if (!memcmp(type,mng_ENDL,4))
            {
              loop_level=chunk[0];
              if (skipping_loop > 0)
                {
                  if (skipping_loop == loop_level)
                    {
                      /*
                        Found end of zero-iteration loop.
                      */
                      skipping_loop=(-1);
                      loops_active--;
                      mng_info->loop_active[loop_level]=0;
                    }
                }
              else
                {
                  if (mng_info->loop_active[loop_level] == 1)
                    {
                      mng_info->loop_count[loop_level]--;
                      mng_info->loop_iteration[loop_level]++;
                      if (mng_info->loop_count[loop_level] != 0)
                        (void) SeekBlob(image,mng_info->loop_jump[loop_level],
                                        SEEK_SET);
                      else
                        {
                          short
                            last_level;

                          /*
                            Finished loop.
                          */
                          loops_active--;
                          mng_info->loop_active[loop_level]=0;
                          last_level=(-1);
                          for (i=0; i < loop_level; i++)
                            if (mng_info->loop_active[i] == 1)
                              last_level=(short) i;
                          loop_level=last_level;
                        }
                    }
                }
              MagickFreeMemory(chunk);
              continue;
            }
          if (!memcmp(type,mng_CLON,4))
            {
              if (!mng_info->clon_warning)
                (void) ThrowException2(&image->exception,CoderError,
                                       "CLON is not implemented yet",
                                       image->filename);
              mng_info->clon_warning++;
            }
          if (!memcmp(type,mng_MAGN,4))
            {
              png_uint_16
                magn_first,
                magn_last,
                magn_mb,
                magn_ml,
                magn_mr,
                magn_mt,
                magn_mx,
                magn_my,
                magn_methx,
                magn_methy;

              if (length > 1)
                magn_first=(p[0] << 8) | p[1];
              else
                magn_first=0;
              if (length > 3)
                magn_last=(p[2] << 8) | p[3];
              else
                magn_last=magn_first;
#ifndef MNG_OBJECT_BUFFERS
              if (magn_first || magn_last)
                if (!mng_info->magn_warning)
                  {
                    (void) ThrowException2(&image->exception,CoderError,
                                           "MAGN is not implemented yet"
                                           " for nonzero objects",
                                           image->filename);
                    mng_info->magn_warning++;
                  }
#endif
              if (length > 4)
                magn_methx=p[4];
              else
                magn_methx=0;

              if (length > 6)
                magn_mx=(p[5] << 8) | p[6];
              else
                magn_mx=1;
              if (magn_mx == 0)
                magn_mx=1;

              if (length > 8)
                magn_my=(p[7] << 8) | p[8];
              else
                magn_my=magn_mx;
              if (magn_my == 0)
                magn_my=1;

              if (length > 10)
                magn_ml=(p[9] << 8) | p[10];
              else
                magn_ml=magn_mx;
              if (magn_ml == 0)
                magn_ml=1;

              if (length > 12)
                magn_mr=(p[11] << 8) | p[12];
              else
                magn_mr=magn_mx;
              if (magn_mr == 0)
                magn_mr=1;

              if (length > 14)
                magn_mt=(p[13] << 8) | p[14];
              else
                magn_mt=magn_my;
              if (magn_mt == 0)
                magn_mt=1;

              if (length > 16)
                magn_mb=(p[15] << 8) | p[16];
              else
                magn_mb=magn_my;
              if (magn_mb == 0)
                magn_mb=1;

              if (length > 17)
                magn_methy=p[17];
              else
                magn_methy=magn_methx;

              if (magn_methx > 5 || magn_methy > 5)
                if (!mng_info->magn_warning)
                  {
                    (void) ThrowException2(&image->exception,CoderError,
                                           "Unknown MAGN method in"
                                           " MNG datastream",
                                           image->filename);
                    mng_info->magn_warning++;
                  }
#ifdef MNG_OBJECT_BUFFERS
              /* Magnify existing objects in the range magn_first
                 to magn_last */
#endif
              if (magn_first == 0 || magn_last == 0)
                {
                  /* Save the magnification factors for object 0 */
                  mng_info->magn_mb=magn_mb;
                  mng_info->magn_ml=magn_ml;
                  mng_info->magn_mr=magn_mr;
                  mng_info->magn_mt=magn_mt;
                  mng_info->magn_mx=magn_mx;
                  mng_info->magn_my=magn_my;
                  mng_info->magn_methx=magn_methx;
                  mng_info->magn_methy=magn_methy;
                }
            }
          if (!memcmp(type,mng_PAST,4))
            {
              if (!mng_info->past_warning)
                (void) ThrowException2(&image->exception,CoderError,
                                       "PAST is not implemented yet",
                                       image->filename);
              mng_info->past_warning++;
            }
          if (!memcmp(type,mng_SHOW,4))
            {
              if (!mng_info->show_warning)
                (void) ThrowException2(&image->exception,CoderError,
                                       "SHOW is not implemented yet",
                                       image->filename);
              mng_info->show_warning++;
            }
          if (!memcmp(type,mng_sBIT,4))
            {
              if (length < 4)
                mng_info->have_global_sbit=MagickFalse;
              else
                {
                  mng_info->global_sbit.gray=p[0];
                  mng_info->global_sbit.red=p[0];
                  mng_info->global_sbit.green=p[1];
                  mng_info->global_sbit.blue=p[2];
                  mng_info->global_sbit.alpha=p[3];
                  mng_info->have_global_sbit=MagickTrue;
                }
            }
          if (!memcmp(type,mng_pHYs,4))
            {
              if (length > 8)
                {
                  mng_info->global_x_pixels_per_unit=mng_get_long(p);
                  mng_info->global_y_pixels_per_unit=mng_get_long(&p[4]);
                  mng_info->global_phys_unit_type=p[8];
                  mng_info->have_global_phys=MagickTrue;
                }
              else
                mng_info->have_global_phys=MagickFalse;
            }
          if (!memcmp(type,mng_pHYg,4))
            {
              if (!mng_info->phyg_warning)
                (void) ThrowException2(&image->exception,CoderError,
                                       "pHYg is not implemented.",
                                       image->filename);
              mng_info->phyg_warning++;
            }
          if (!memcmp(type,mng_BASI,4))
            {
              skip_to_iend=MagickTrue;
              if (!mng_info->basi_warning)
                (void) ThrowException2(&image->exception,CoderError,
                                       "BASI is not implemented yet",
                                       image->filename);
              mng_info->basi_warning++;
#ifdef MNG_BASI_SUPPORTED
              basi_width=(unsigned long) ((p[0] << 24) | (p[1] << 16) |
                                          (p[2] << 8) | p[3]);
              basi_height=(unsigned long) ((p[4] << 24) | (p[5] << 16) |
                                           (p[6] << 8) | p[7]);
              basi_color_type=p[8];
              basi_compression_method=p[9];
              basi_filter_type=p[10];
              basi_interlace_method=p[11];
              if (length > 11)
                basi_red=(p[12] << 8) & p[13];
              else
                basi_red=0;
              if (length > 13)
                basi_green=(p[14] << 8) & p[15];
              else
                basi_green=0;
              if (length > 15)
                basi_blue=(p[16] << 8) & p[17];
              else
                basi_blue=0;
              if (length > 17)
                basi_alpha=(p[18] << 8) & p[19];
              else
                {
                  if (basi_sample_depth == 16)
                    basi_alpha=65535L;
                  else
                    basi_alpha=255;
                }
              if (length > 19)
                basi_viewable=p[20];
              else
                basi_viewable=0;
#endif
              MagickFreeMemory(chunk);
              continue;
            }
          if (memcmp(type,mng_IHDR,4)
#if defined(JNG_SUPPORTED)
              && memcmp(type,mng_JHDR,4)
#endif
              )
            {
              /* Not an IHDR or JHDR chunk */
              if (length)
                MagickFreeMemory(chunk);
              continue;
            }
          /* Process IHDR */
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "  Processing %c%c%c%c chunk",
                                  type[0],type[1],type[2],type[3]);
          mng_info->exists[object_id]=MagickTrue;
          mng_info->viewable[object_id]=MagickTrue;
          if (mng_info->invisible[object_id])
            {
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "  Skipping invisible object");
              skip_to_iend=MagickTrue;
              MagickFreeMemory(chunk);
              continue;
            }
#ifdef MNG_INSERT_LAYERS
          if (length < 8)
            ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
          image_width=mng_get_long(p);
          image_height=mng_get_long(&p[4]);
#endif
          MagickFreeMemory(chunk);

          /*
            Insert a transparent background layer behind the entire animation
            if it is not full screen.
          */
#ifdef MNG_INSERT_LAYERS
          if (insert_layers && mng_type && first_mng_object)
            {
              if ((mng_info->clip.left > 0) || (mng_info->clip.top > 0) ||
                  (image_width < mng_info->mng_width) ||
                  (mng_info->clip.right < (long) mng_info->mng_width) ||
                  (image_height < mng_info->mng_height) ||
                  (mng_info->clip.bottom < (long) mng_info->mng_height))
                {
                  if (AccessMutablePixels(image) != (PixelPacket *) NULL)
                    {
                      /*
                        Allocate next image structure.
                      */
                      AllocateNextImage(image_info,image);
                      if (image->next == (Image *) NULL)
                        {
                          DestroyImageList(image);
                          MngInfoFreeStruct(mng_info,&have_mng_structure);
                          return((Image *) NULL);
                        }
                      image=SyncNextImageInList(image);
                    }
                  mng_info->image=image;
                  if (term_chunk_found)
                    {
                      image->start_loop=MagickTrue;
                      image->iterations=mng_iterations;
                      term_chunk_found=MagickFalse;
                    }
                  else
                    image->start_loop=MagickFalse;
                  /*
                    Make a background rectangle.
                  */
                  image->delay=0;
                  image->columns=mng_info->mng_width;
                  image->rows=mng_info->mng_height;
                  image->page.width=mng_info->mng_width;
                  image->page.height=mng_info->mng_height;
                  image->page.x=0;
                  image->page.y=0;
                  image->background_color=mng_background_color;
                  (void) SetImage(image,TransparentOpacity);
                  if (logging)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "  Inserted transparent background"
                                          " layer, W=%lud, H=%lud",
                                          mng_info->mng_width,
                                          mng_info->mng_height);
                }
            }
          /*
            Insert a background layer behind the upcoming image if
            framing_mode is 3, and we haven't already inserted one.
          */
          if (insert_layers && (mng_info->framing_mode == 3) &&
              (subframe_width) && (subframe_height) && (simplicity == 0 ||
                                                        (simplicity & 0x08)))
            {
              if (AccessMutablePixels(image) != (PixelPacket *) NULL)
                {
                  /*
                    Allocate next image structure.
                  */
                  AllocateNextImage(image_info,image);
                  if (image->next == (Image *) NULL)
                    {
                      DestroyImageList(image);
                      MngInfoFreeStruct(mng_info,&have_mng_structure);
                      return((Image *) NULL);
                    }
                  image=SyncNextImageInList(image);
                }
              mng_info->image=image;
              if (term_chunk_found)
                {
                  image->start_loop=MagickTrue;
                  image->iterations=mng_iterations;
                  term_chunk_found=MagickFalse;
                }
              else
                image->start_loop=MagickFalse;
              image->delay=0;
              image->columns=subframe_width;
              image->rows=subframe_height;
              image->page.width=subframe_width;
              image->page.height=subframe_height;
              image->page.x=mng_info->clip.left;
              image->page.y=mng_info->clip.top;
              image->background_color=mng_background_color;
              image->matte=MagickFalse;
              (void) SetImage(image,OpaqueOpacity);
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "  Inserted background layer,"
                                      " L=%ld, R=%ld, T=%ld, B=%ld",
                                      mng_info->clip.left,
                                      mng_info->clip.right,
                                      mng_info->clip.top,
                                      mng_info->clip.bottom);
            }
#endif /* MNG_INSERT_LAYERS */
          first_mng_object=MagickFalse;
          if (AccessMutablePixels(image) != (PixelPacket *) NULL)
            {
              /*
                Allocate next image structure.
              */
              AllocateNextImage(image_info,image);
              if (image->next == (Image *) NULL)
                {
                  DestroyImageList(image);
                  MngInfoFreeStruct(mng_info,&have_mng_structure);
                  return((Image *) NULL);
                }
              image=SyncNextImageInList(image);
            }
          mng_info->image=image;
          if (QuantumTick(TellBlob(image),GetBlobSize(image)))
            if (!MagickMonitorFormatted(TellBlob(image),GetBlobSize(image),
                                        exception,LoadImagesTag,
                                        image->filename))
            break;
          if (term_chunk_found)
            {
              image->start_loop=MagickTrue;
              term_chunk_found=MagickFalse;
            }
          else
            image->start_loop=MagickFalse;
          if (mng_info->framing_mode == 1 || mng_info->framing_mode == 3)
            {
              image->delay=frame_delay;
              frame_delay=default_frame_delay;
            }
          else
            image->delay=0;
          image->page.width=mng_info->mng_width;
          image->page.height=mng_info->mng_height;
          image->page.x=mng_info->x_off[object_id];
          image->page.y=mng_info->y_off[object_id];
          image->iterations=mng_iterations;
          /*
            Seek back to the beginning of the IHDR or JHDR chunk's
            length field.
          */
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "  Seeking back to beginning of"
                                  " %c%c%c%c chunk",
                                  type[0],type[1],
                                  type[2],type[3]);
          (void) SeekBlob(image,-((long) length+12),SEEK_CUR);
        }

      previous=image;
      mng_info->image=image;
      mng_info->mng_type=mng_type;
      mng_info->object_id=object_id;

      if (!memcmp(type,mng_IHDR,4))
        image=ReadOnePNGImage(mng_info,image_info,exception);
#if defined(JNG_SUPPORTED)
      else
        image=ReadOneJNGImage(mng_info,image_info,exception);
#endif

      if (image == (Image *) NULL)
        {
          DestroyImageList(previous);
          CloseBlob(previous);
          MngInfoFreeStruct(mng_info,&have_mng_structure);
          return((Image *) NULL);
        }
      if (image->columns == 0 || image->rows == 0)
        {
          CloseBlob(image);
          DestroyImageList(image);
          MngInfoFreeStruct(mng_info,&have_mng_structure);
          return((Image *) NULL);
        }
      mng_info->image=image;

      if (mng_type)
        {
          MngBox
            crop_box;

          if (mng_info->magn_methx || mng_info->magn_methy)
            {
              png_uint_32
                magnified_height,
                magnified_width;

              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "  Processing MNG MAGN chunk");

              if (mng_info->magn_methx == 1)
                {
                  magnified_width=mng_info->magn_ml;
                  if (image->columns > 1)
                    magnified_width += mng_info->magn_mr;
                  if (image->columns > 2)
                    magnified_width += (image->columns-2)*(mng_info->magn_mx);
                }
              else
                {
                  magnified_width=image->columns;
                  if (image->columns > 1)
                    magnified_width += mng_info->magn_ml-1;
                  if (image->columns > 2)
                    magnified_width += mng_info->magn_mr-1;
                  if (image->columns > 3)
                    magnified_width += (image->columns-3)*
                       (mng_info->magn_mx-1);
                }
              if (mng_info->magn_methy == 1)
                {
                  magnified_height=mng_info->magn_mt;
                  if (image->rows > 1)
                    magnified_height += mng_info->magn_mb;
                  if (image->rows > 2)
                    magnified_height += (image->rows-2)*(mng_info->magn_my);
                }
              else
                {
                  magnified_height=image->rows;
                  if (image->rows > 1)
                    magnified_height += mng_info->magn_mt-1;
                  if (image->rows > 2)
                    magnified_height += mng_info->magn_mb-1;
                  if (image->rows > 3)
                    magnified_height += (image->rows-3)*(mng_info->magn_my-1);
                }
              if (magnified_height > image->rows ||
                  magnified_width > image->columns)
                {
                  Image
                    *large_image;

                  int
                    yy;

                  long
                    m,
                    y;

                  register long
                    x;

                  register PixelPacket
                    *n,
                    *p,
                    *q;

                  PixelPacket
                    *next,
                    *prev;

                  size_t
                    row_length;

                  png_uint_16
                    magn_methx,
                    magn_methy;

                  /*
                    Allocate next image structure.
                  */
                  if (logging)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "    Allocate magnified image");
                  AllocateNextImage(image_info,image);
                  if (image->next == (Image *) NULL)
                    {
                      DestroyImageList(image);
                      MngInfoFreeStruct(mng_info,&have_mng_structure);
                      return((Image *) NULL);
                    }

                  large_image=SyncNextImageInList(image);

                  large_image->columns=magnified_width;
                  large_image->rows=magnified_height;

                  magn_methx=mng_info->magn_methx;
                  magn_methy=mng_info->magn_methy;

#if (QuantumDepth == 32)
#define QM unsigned short
                  if (magn_methx != 1 || magn_methy != 1)
                    {
                      /*
                        Scale pixels to unsigned shorts to prevent
                        overflow of intermediate values of interpolations
                      */
                      for (y=0; y < (long) image->rows; y++)
                        {
                          q=GetImagePixels(image,0,y,image->columns,1);
                          for (x=(long) image->columns; x > 0; x--)
                            {
                              q->red=ScaleQuantumToShort(q->red);
                              q->green=ScaleQuantumToShort(q->green);
                              q->blue=ScaleQuantumToShort(q->blue);
                              q->opacity=ScaleQuantumToShort(q->opacity);
                              q++;
                            }
                          if (!SyncImagePixels(image))
                            break;
                        }
                    }
#else
#define QM Quantum
#endif

                  if (image->matte)
                    (void) SetImage(large_image,TransparentOpacity);
                  else
                    {
                      large_image->background_color.opacity=OpaqueOpacity;
                      (void) SetImage(large_image,OpaqueOpacity);
                      if (magn_methx == 4)
                        magn_methx=2;
                      if (magn_methx == 5)
                        magn_methx=3;
                      if (magn_methy == 4)
                        magn_methy=2;
                      if (magn_methy == 5)
                        magn_methy=3;
                    }

                  /* magnify the rows into the right side of the large image */

                  if (logging)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "    Magnify the rows to %lu",
                                          large_image->rows);
                  m=mng_info->magn_mt;
                  yy=0;
                  row_length=(size_t) (image->columns*sizeof(PixelPacket));
                  next=MagickAllocateMemory(PixelPacket *,row_length);
                  prev=MagickAllocateMemory(PixelPacket *,row_length);
                  if ((prev == (PixelPacket *) NULL) ||
                      (next == (PixelPacket *) NULL))
                    {
                      DestroyImageList(image);
                      MngInfoFreeStruct(mng_info,&have_mng_structure);
                      ThrowReaderException(ResourceLimitError,
                                           MemoryAllocationFailed,image)
                    }
                  n=GetImagePixels(image,0,0,image->columns,1);
                  (void) memcpy(next,n,row_length);
                  for (y=0; y < (long) image->rows; y++)
                    {
                      if (y == 0)
                        m=mng_info->magn_mt;
                      else if (magn_methy > 1 && y == (long) image->rows-2)
                        m=mng_info->magn_mb;
                      else if (magn_methy <= 1 && y == (long) image->rows-1)
                        m=mng_info->magn_mb;
                      else if (magn_methy > 1 && y == (long) image->rows-1)
                        m=1;
                      else
                        m=mng_info->magn_my;
                      n=prev;
                      prev=next;
                      next=n;
                      if (y < (long) image->rows-1)
                        {
                          n=GetImagePixels(image,0,y+1,image->columns,1);
                          (void) memcpy(next,n,row_length);
                        }
                      for (i=0; i < m; i++, yy++)
                        {
                          assert(yy < (long) large_image->rows);
                          p=prev;
                          n=next;
                          q=SetImagePixels(large_image,0,yy,
                                           large_image->columns,1);
                          q+=(large_image->columns-image->columns);
                          for (x=(long) image->columns; x > 0; x--)
                            {
                              /* TO DO: get color as function of indexes[x] */
                              /*
                                if (image->storage_class == PseudoClass)
                                {
                                }
                              */

                              if (magn_methy <= 1)
                                {
                                  *q=(*p); /* replicate previous */
                                }
                              else if (magn_methy == 2 || magn_methy == 4)
                                {
                                  if (i == 0)
                                    *q=(*p);
                                  else
                                    {
                                      /* Interpolate */
                                      (*q).red=(QM) (((long) (2*i*((*n).red
                                               -(*p).red)+m))/
                                               ((long) (m*2))+(*p).red);
                                      (*q).green=(QM) (((long) (2*i*((*n).green
                                                 -(*p).green)+m))/
                                                 ((long) (m*2))+(*p).green);
                                      (*q).blue=(QM) (((long) (2*i*((*n).blue
                                                     -(*p).blue)+m))/
                                                     ((long) (m*2))+(*p).blue);
                                      if (image->matte)
                                        (*q).opacity=(QM) (((long)
                                                      (2*i*((*n).opacity-
                                                      (*p).opacity)+m))
                                                      /((long) (m*2))+
                                                      (*p).opacity);
                                    }
                                  if (magn_methy == 4)
                                    {
                                      /* Replicate nearest */
                                      if (i <= ((m+1) << 1))
                                        (*q).opacity=(*p).opacity+0;
                                      else
                                        (*q).opacity=(*n).opacity+0;
                                    }
                                }
                              else /* if (magn_methy == 3 ||
                                          magn_methy == 5) */
                                {
                                  /* Replicate nearest */
                                  if (i <= ((m+1) << 1))
                                    *q=(*p);
                                  else
                                    *q=(*n);
                                  if (magn_methy == 5)
                                    {
                                      (*q).opacity=(QM) (
                                             ((long) (2*i*((*n).opacity
                                             -(*p).opacity)+m))/
                                             ((long) (m*2))+
                                             (*p).opacity);
                                    }
                                }
                              n++;
                              q++;
                              p++;
                            } /* x */
                          if (!SyncImagePixels(large_image))
                            break;
                        } /* i */
                    } /* y */
                  MagickFreeMemory(prev);
                  MagickFreeMemory(next);

                  row_length=image->columns;

                  if (logging)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "    Delete original image");

                  DeleteImageFromList(&image);

                  image=large_image;

                  mng_info->image=image;

                  /* magnify the columns */
                  if (logging)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "    Magnify the columns to %lu",
                                          image->columns);

                  for (y=0; y < (long) image->rows; y++)
                    {
                      q=GetImagePixels(image,0,y,image->columns,1);
                      p=q+(image->columns-row_length);
                      n=p+1;
                      for (x=(long) (image->columns-row_length);
                           x < (long) image->columns; x++)
                        {
                          if (x == (long) (image->columns-row_length))
                            m=mng_info->magn_ml;
                          else if (magn_methx > 1 &&
                                   x == (long) image->columns-2)
                            m=mng_info->magn_mr;
                          else if (magn_methx <= 1 &&
                                   x == (long) image->columns-1)
                            m=mng_info->magn_mr;
                          else if (magn_methx > 1 &&
                                   x == (long) image->columns-1)
                            m=1;
                          else
                            m=mng_info->magn_mx;
                          for (i=0; i < m; i++)
                            {
                              if (magn_methx <= 1)
                                {
                                  /* replicate previous */
                                  *q=(*p);
                                }
                              else if (magn_methx == 2 || magn_methx == 4)
                                {
                                  if (i == 0)
                                    *q=(*p);
                                  else
                                    {
                                      /* Interpolate */
                                      (*q).red=(QM) ((2*i*((*n).red-
                                                     (*p).red)+m)
                                                     /((long) (m*2))+
                                                     (*p).red);
                                      (*q).green=(QM) ((2*i*((*n).green-
                                                       (*p).green)
                                                       +m)/((long) (m*2))+
                                                       (*p).green);
                                      (*q).blue=(QM) ((2*i*((*n).blue-
                                                      (*p).blue)+m)
                                                      /((long) (m*2))+
                                                      (*p).blue);
                                      if (image->matte)
                                        (*q).opacity=(QM) ((2*i*((*n).opacity
                                                           -(*p).opacity)+m)/
                                                           ((long) (m*2))
                                                           +(*p).opacity);
                                    }
                                  if (magn_methx == 4)
                                    {
                                      /* Replicate nearest */
                                      if (i <= ((m+1) << 1))
                                        (*q).opacity=(*p).opacity+0;
                                      else
                                        (*q).opacity=(*n).opacity+0;
                                    }
                                }
                              else /* if (magn_methx == 3 ||
                                      magn_methx == 5) */
                                {
                                  /* Replicate nearest */
                                  if (i <= ((m+1) << 1))
                                    *q=(*p);
                                  else
                                    *q=(*n);
                                  if (magn_methx == 5)
                                    {
                                      /* Interpolate */
                                      (*q).opacity=(QM) ((2*i*((*n).opacity
                                                         -(*p).opacity)+m)/
                                                         ((long) (m*2))
                                                         +(*p).opacity);
                                    }
                                }
                              q++;
                            }
                          n++;
                          p++;
                        }
                      if (!SyncImagePixels(image))
                        break;
                    }
#if (QuantumDepth == 32)
                  if (magn_methx != 1 || magn_methy != 1)
                    {
                      /*
                        Rescale pixels to Quantum
                      */
                      for (y=0; y < (long) image->rows; y++)
                        {
                          q=GetImagePixels(image,0,y,image->columns,1);
                          for (x=(long) image->columns; x > 0; x--)
                            {
                              q->red=ScaleShortToQuantum(q->red);
                              q->green=ScaleShortToQuantum(q->green);
                              q->blue=ScaleShortToQuantum(q->blue);
                              q->opacity=ScaleShortToQuantum(q->opacity);
                              q++;
                            }
                          if (!SyncImagePixels(image))
                            break;
                        }
                    }
#endif
                  if (logging)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "  Finished MAGN processing");
                }
            }

          /*
            Crop_box is with respect to the upper left corner of the MNG.
          */
          crop_box.left=mng_info->image_box.left+mng_info->x_off[object_id];
          crop_box.right=mng_info->image_box.right+mng_info->x_off[object_id];
          crop_box.top=mng_info->image_box.top+mng_info->y_off[object_id];
          crop_box.bottom=mng_info->image_box.bottom+
                          mng_info->y_off[object_id];
          crop_box=mng_minimum_box(crop_box,mng_info->clip);
          crop_box=mng_minimum_box(crop_box,mng_info->frame);
          crop_box=mng_minimum_box(crop_box,mng_info->object_clip[object_id]);
          if ((crop_box.left != (mng_info->image_box.left
                                 +mng_info->x_off[object_id])) ||
              (crop_box.right != (mng_info->image_box.right
                                  +mng_info->x_off[object_id])) ||
              (crop_box.top != (mng_info->image_box.top
                                +mng_info->y_off[object_id])) ||
              (crop_box.bottom != (mng_info->image_box.bottom
                                   +mng_info->y_off[object_id])))
            {
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "  Crop the PNG image");
              if ((crop_box.left < crop_box.right) &&
                  (crop_box.top < crop_box.bottom))
                {
                  Image
                    *p;

                  RectangleInfo
                    crop_info;

                  /*
                    Crop_info is with respect to the upper left corner of
                    the image.
                  */
                  crop_info.x=(crop_box.left-mng_info->x_off[object_id]);
                  crop_info.y=(crop_box.top-mng_info->y_off[object_id]);
                  crop_info.width=(crop_box.right-crop_box.left);
                  crop_info.height=(crop_box.bottom-crop_box.top);
                  image->page.width=image->columns;
                  image->page.height=image->rows;
                  image->page.x=0;
                  image->page.y=0;
                  p=CropImage(image,&crop_info,exception);
                  if (p != (Image *) NULL)
                    {
                      image->columns=p->columns;
                      image->rows=p->rows;
                      DestroyImage(p);
                      image->page.width=image->columns;
                      image->page.height=image->rows;
                      image->page.x=crop_box.left;
                      image->page.y=crop_box.top;
                    }
                }
              else
                {
                  /*
                    No pixels in crop area.  The MNG spec still requires
                    a layer, though, so make a single transparent pixel in
                    the top left corner.
                  */
                  image->columns=1;
                  image->rows=1;
                  image->colors=2;
                  (void) SetImage(image,TransparentOpacity);
                  image->page.width=1;
                  image->page.height=1;
                  image->page.x=0;
                  image->page.y=0;
                }
            }
#ifndef PNG_READ_EMPTY_PLTE_SUPPORTED
          image=mng_info->image;
#endif
        }

      /*
        Transfer most significant exception to exception argument
        FIXME: should status be used to terminate processing?
      */
      GetImageException(image,exception);
      if (image_info->number_scenes != 0)
        {
          if (mng_info->scenes_found > (long) (image_info->first_scene+
                                               image_info->number_scenes))
            break;
        }
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "  Finished reading image datastream.");
    } while (LocaleCompare(image_info->magick,"MNG") == 0);
  CloseBlob(image);
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  Finished reading all image datastreams.");
#ifdef MNG_INSERT_LAYERS
  if (insert_layers && !mng_info->image_found && (mng_info->mng_width) &&
      (mng_info->mng_height))
    {
      /*
        Insert a background layer if nothing else was found.
      */
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "  No images found.  Inserting a"
                              " background layer.");
      if (AccessMutablePixels(image) != (PixelPacket *) NULL)
        {
          /*
            Allocate next image structure.
          */
          AllocateNextImage(image_info,image);
          if (image->next == (Image *) NULL)
            {
              DestroyImageList(image);
              MngInfoFreeStruct(mng_info,&have_mng_structure);
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "  Allocation failed, returning NULL.");
              return((Image *) NULL);
            }
          image=SyncNextImageInList(image);
        }
      image->columns=mng_info->mng_width;
      image->rows=mng_info->mng_height;
      image->page.width=mng_info->mng_width;
      image->page.height=mng_info->mng_height;
      image->page.x=0;
      image->page.y=0;
      image->background_color=mng_background_color;
      image->matte=MagickFalse;
      if (!image_info->ping)
        (void) SetImage(image,OpaqueOpacity);
      mng_info->image_found++;
    }
#endif
  image->iterations=mng_iterations;
  if (mng_iterations == 1)
    image->start_loop=MagickTrue;
  while (image->previous != (Image *) NULL)
    {
      image_count++;
      if (image_count > 10*mng_info->image_found)
        {
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "  No beginning");
          MngInfoFreeStruct(mng_info,&have_mng_structure);
          (void) ThrowException2(&image->exception,(ExceptionType) CoderError,
                                 "Linked list is corrupted,"
                                 " beginning of list not found",
                                 image_info->filename);
          return((Image *) NULL);
        }
      image=image->previous;
      if (image->next == (Image *) NULL)
        {
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "  Corrupt list");
          (void) ThrowException2(&image->exception,(ExceptionType) CoderError,
                                 "Linked list is corrupted;"
                                 " next_image is NULL",
                                 image_info->filename);
        }
    }
  if (mng_info->ticks_per_second && mng_info->image_found > 1 && image->next ==
      (Image *) NULL)
    {
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "  First image null");
      (void) ThrowException2(&image->exception,(ExceptionType) CoderError,
                             "image->next for first image is NULL but"
                             " shouldn't be.",
                             image_info->filename);
    }
  if (!mng_info->image_found)
    {
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "  No visible images found.");
      (void) ThrowException2(&image->exception,(ExceptionType) CoderError,
                             "No visible images in file",image_info->filename);
      if (image != (Image *) NULL)
        DestroyImageList(image);
      MngInfoFreeStruct(mng_info,&have_mng_structure);
      return((Image *) NULL);
    }

  if (mng_info->ticks_per_second)
    final_delay=100*final_delay/mng_info->ticks_per_second;
  else
    image->start_loop=MagickTrue;
  /* Find final nonzero image delay */
  final_image_delay=0;
  while (image->next != (Image *) NULL)
    {
      if (image->delay)
        final_image_delay=image->delay;
      image=image->next;
    }
  if (final_delay < final_image_delay)
    final_delay=final_image_delay;
  image->delay=final_delay;
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  image->delay=%lu, final_delay=%lu",
                          image->delay,final_delay);
  if (logging)
    {
      int
        scene;

      scene=0;
      while (image->previous != (Image *) NULL)
        image=image->previous;
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "  Before coalesce:");
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    scene 0 delay=%lu",image->delay);
      while (image->next != (Image *) NULL)
        {
          image=image->next;
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "    scene %d delay=%lu",++scene,image->delay);
        }
    }

  while (image->previous != (Image *) NULL)
    image=image->previous;
#ifdef MNG_COALESCE_LAYERS
  if (insert_layers)
    {
      Image
        *next_image,
        *next;

      unsigned long
        scene;

      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "  Coalesce Images");
      scene=image->scene;
      next_image=CoalesceImages(image,&image->exception);
      if (next_image == (Image *) NULL)
        MagickFatalError2(image->exception.severity,image->exception.reason,
                          image->exception.description);
      DestroyImageList(image);
      image=next_image;
      for (next=image; next != (Image *) NULL; next=next_image)
        {
          next->page.width=mng_info->mng_width;
          next->page.height=mng_info->mng_height;
          next->page.x=0;
          next->page.y=0;
          next->scene=scene++;
          next_image=next->next;
          if (next_image == (Image *) NULL)
            break;
          if (next->delay == 0)
            {
              scene--;
              next_image->previous=next->previous;
              if (next->previous == (Image *) NULL)
                image=next_image;
              else
                next->previous->next=next_image;
              DestroyImage(next);
            }
        }
    }
#endif

  while (image->next != (Image *) NULL)
    image=image->next;
  image->dispose=BackgroundDispose;

  if (logging)
    {
      int
        scene;

      scene=0;
      while (image->previous != (Image *) NULL)
        image=image->previous;
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "  After coalesce:");
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    scene 0 delay=%lu dispose=%d",
                            image->delay,(int) image->dispose);
      while (image->next != (Image *) NULL)
        {
          image=image->next;
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "    scene %d delay=%lu dispose=%d",++scene,
                                image->delay,(int) image->dispose);
        }
    }
  while (image->previous != (Image *) NULL)
    image=image->previous;
  MngInfoFreeStruct(mng_info,&have_mng_structure);
  have_mng_structure=MagickFalse;
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"exit ReadMNGImage()");
  return(image);
}
#else /* PNG_LIBPNG_VER > 10011 */
static Image *ReadPNGImage(const ImageInfo *image_info,
                           ExceptionInfo *exception)
{
  printf("Your PNG library is too old: You have libpng-%s\n",
     PNG_LIBPNG_VER_STRING);
  (void) ThrowException2(exception,CoderError,"PNG library is too old",
    image_info->filename);
  return (Image *) NULL;
}
static Image *ReadMNGImage(const ImageInfo *image_info,
                           ExceptionInfo *exception)
{
  return (ReadPNGImage(image_info,exception));
}
#endif /* PNG_LIBPNG_VER > 10011 */
#endif

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r P N G I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterPNGImage adds attributes for the PNG image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterPNGImage method is:
%
%      RegisterPNGImage(void)
%
*/
ModuleExport void RegisterPNGImage(void)
{
  static char
    version[MaxTextExtent];

  MagickInfo
    *entry;

  static const char
    *PNGNote=
    {
      "See http://www.libpng.org/ for information on PNG.."
    };

  static const char
    *JNGNote=
    {
      "See http://www.libpng.org/pub/mng/ for information on JNG."
    };

  static const char
    *MNGNote=
    {
      "See http://www.libpng.org/pub/mng/ for information on MNG."
    };

      *version='\0';
#if defined(PNG_LIBPNG_VER_STRING)
      (void) strlcat(version,"libpng ",MaxTextExtent);
      (void) strlcat(version,PNG_LIBPNG_VER_STRING,MaxTextExtent);
#if (PNG_LIBPNG_VER > 10005)
      if (LocaleCompare(PNG_LIBPNG_VER_STRING,png_get_header_ver(NULL)) != 0)
        {
          (void) strlcat(version,",",MaxTextExtent);
          (void) strlcat(version,png_get_libpng_ver(NULL),MaxTextExtent);
        }
#endif
#endif

#if defined(ZLIB_VERSION)
      if (*version != '\0')
        (void) strlcat(version,", ",MaxTextExtent);
      (void) strlcat(version,"zlib ",MaxTextExtent);
      (void) strlcat(version,ZLIB_VERSION,MaxTextExtent);
      if (LocaleCompare(ZLIB_VERSION,zlib_version) != 0)
        {
          (void) strlcat(version,",",MaxTextExtent);
          (void) strlcat(version,zlib_version,MaxTextExtent);
        }
#endif

      entry=SetMagickInfo("MNG");
      entry->seekable_stream=MagickTrue;  /* To do: eliminate this. */
      entry->thread_support=MagickTrue;
#if defined(HasPNG)
      entry->decoder=(DecoderHandler) ReadMNGImage;
      entry->encoder=(EncoderHandler) WriteMNGImage;
#endif
      entry->magick=(MagickHandler) IsMNG;
      entry->description="Multiple-image Network Graphics";
      if (*version != '\0')
        entry->version=version;
      entry->module="PNG";
      entry->coder_class=PrimaryCoderClass;
      entry->note=MNGNote;
      (void) RegisterMagickInfo(entry);

      entry=SetMagickInfo("PNG");
#if defined(HasPNG)
      entry->decoder=(DecoderHandler) ReadPNGImage;
      entry->encoder=(EncoderHandler) WritePNGImage;
#endif
      entry->magick=(MagickHandler) IsPNG;
      entry->adjoin=MagickFalse;
      entry->thread_support=MagickTrue;
      entry->description="Portable Network Graphics";
      if (*version != '\0')
        entry->version=version;
      entry->note=PNGNote;
      entry->module="PNG";
      entry->coder_class=PrimaryCoderClass;
      (void) RegisterMagickInfo(entry);

      entry=SetMagickInfo("PNG8");
#if defined(HasPNG)
      entry->decoder=(DecoderHandler) ReadPNGImage;
      entry->encoder=(EncoderHandler) WritePNGImage;
#endif
      entry->magick=(MagickHandler) IsPNG;
      entry->adjoin=MagickFalse;
      entry->thread_support=MagickTrue;
      entry->description="8-bit indexed PNG, binary transparency only";
      if (*version != '\0')
        entry->version=version;
      entry->module="PNG";
      entry->coder_class=PrimaryCoderClass;
      (void) RegisterMagickInfo(entry);

      entry=SetMagickInfo("PNG24");
#if defined(HasPNG)
      entry->decoder=(DecoderHandler) ReadPNGImage;
      entry->encoder=(EncoderHandler) WritePNGImage;
#endif
      entry->magick=(MagickHandler) IsPNG;
      entry->adjoin=MagickFalse;
      entry->thread_support=MagickTrue;
      entry->description="24-bit RGB PNG, opaque only";
      if (*version != '\0')
        entry->version=version;
      entry->module="PNG";
      entry->coder_class=PrimaryCoderClass;
      (void) RegisterMagickInfo(entry);

      entry=SetMagickInfo("PNG32");
#if defined(HasPNG)
      entry->decoder=(DecoderHandler) ReadPNGImage;
      entry->encoder=(EncoderHandler) WritePNGImage;
#endif
      entry->magick=(MagickHandler) IsPNG;
      entry->adjoin=MagickFalse;
      entry->thread_support=MagickTrue;
      entry->description="32-bit RGBA PNG, semitransparency OK";
      if (*version != '\0')
        entry->version=version;
      entry->module="PNG";
      entry->coder_class=PrimaryCoderClass;
      (void) RegisterMagickInfo(entry);

      entry=SetMagickInfo("PNG48");

#if defined(HasPNG)
      entry->decoder=(DecoderHandler) ReadPNGImage;
      entry->encoder=(EncoderHandler) WritePNGImage;
#endif

      entry->magick=(MagickHandler) IsPNG;
      entry->adjoin=MagickFalse;
      entry->thread_support=MagickTrue;
      entry->description="opaque or binary transparent 48-bit RGB";
      if (*version != '\0')
         entry->version=version;
      entry->module="PNG";
      entry->coder_class=PrimaryCoderClass;
      (void) RegisterMagickInfo(entry);

      entry=SetMagickInfo("PNG64");

#if defined(HasPNG)
      entry->decoder=(DecoderHandler) ReadPNGImage;
      entry->encoder=(EncoderHandler) WritePNGImage;
#endif

      entry->magick=(MagickHandler) IsPNG;
      entry->adjoin=MagickFalse;
      entry->thread_support=MagickTrue;
      entry->description="opaque or transparent 64-bit RGBA";
      if (*version != '\0')
        entry->version=version;
      entry->module="PNG";
      entry->coder_class=PrimaryCoderClass;
      (void) RegisterMagickInfo(entry);

      entry=SetMagickInfo("PNG00");

#if defined(HasPNG)
      entry->decoder=(DecoderHandler) ReadPNGImage;
      entry->encoder=(EncoderHandler) WritePNGImage;
#endif

      entry->magick=(MagickHandler) IsPNG;
      entry->adjoin=MagickFalse;
      entry->thread_support=MagickTrue;
      entry->description="PNG that inherits type and depth from original";
      if (*version != '\0')
        entry->version=version;
      entry->module="PNG";
      entry->coder_class=PrimaryCoderClass;
      (void) RegisterMagickInfo(entry);

      entry=SetMagickInfo("JNG");
#if defined(JNG_SUPPORTED)
#if defined(HasPNG)
      entry->decoder=(DecoderHandler) ReadJNGImage;
      entry->encoder=(EncoderHandler) WriteJNGImage;
#endif
#endif
      entry->magick=(MagickHandler) IsJNG;
      entry->adjoin=MagickFalse;
      entry->thread_support=MagickTrue;
      entry->description="JPEG Network Graphics";
      entry->note=JNGNote;
      if (*version != '\0')
        entry->version=version;
      entry->module="PNG";
      entry->coder_class=PrimaryCoderClass;
      (void) RegisterMagickInfo(entry);

#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
      png_semaphore=AllocateSemaphoreInfo();
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r P N G I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterPNGImage removes format registrations made by the
%  PNG module from the list of supported formats.
%
%  The format of the UnregisterPNGImage method is:
%
%      UnregisterPNGImage(void)
%
*/
ModuleExport void UnregisterPNGImage(void)
{
  (void) UnregisterMagickInfo("MNG");
  (void) UnregisterMagickInfo("PNG");
  (void) UnregisterMagickInfo("PNG8");
  (void) UnregisterMagickInfo("PNG24");
  (void) UnregisterMagickInfo("PNG32");
  (void) UnregisterMagickInfo("PNG48");
  (void) UnregisterMagickInfo("PNG64");
  (void) UnregisterMagickInfo("PNG00");
  (void) UnregisterMagickInfo("JNG");

#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
  DestroySemaphoreInfo(&png_semaphore);
#endif
}

#if defined(HasPNG)
#if PNG_LIBPNG_VER > 10011
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e M N G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteMNGImage writes an image in the Portable Network Graphics
%  Group's "Multiple-image Network Graphics" encoded image format.
%
%  MNG support written by Glenn Randers-Pehrson, randeg@alum.rpi.edu
%
%  The format of the WriteMNGImage method is:
%
%      unsigned int WriteMNGImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteMNGImage return True if the image is written.
%      MagickFalse is returned is there is a memory shortage or if the
%      image file fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
%  To do (as of version 5.5.2, November 26, 2002 -- glennrp -- see also
%    "To do" under ReadPNGImage):
%
%    Fix problem with palette sorting (when PNG_SORT_PALETTE is enabled,
%    some GIF animations don't convert properly)
%
%    Preserve all unknown and not-yet-handled known chunks found in input
%    PNG file and copy them  into output PNG files according to the PNG
%    copying rules.
%
%    Write the iCCP chunk at MNG level when (image->color_profile.length > 0)
%
%    Improve selection of color type (use indexed-colour or indexed-colour
%    with tRNS when 256 or fewer unique RGBA values are present).
%
%    Figure out what to do with "dispose=<restore-to-previous>" (dispose == 3)
%    This will be complicated if we limit ourselves to generating MNG-LC
%    files.  For now we ignore disposal method 3 and simply overlay the next
%    image on it.
%
%    Check for identical PLTE's or PLTE/tRNS combinations and use a
%    global MNG PLTE or PLTE/tRNS combination when appropriate.
%    [mostly done 15 June 1999 but still need to take care of tRNS]
%
%    Check for identical sRGB and replace with a global sRGB (and remove
%    gAMA/cHRM if sRGB is found; check for identical gAMA/cHRM and
%    replace with global gAMA/cHRM (or with sRGB if appropriate; replace
%    local gAMA/cHRM with local sRGB if appropriate).
%
%    Check for identical sBIT chunks and write global ones.
%
%    Provide option to skip writing the signature tEXt chunks.
%
%    Use signatures to detect identical objects and reuse the first
%    instance of such objects instead of writing duplicate objects.
%
%    Use a smaller-than-32k value of compression window size when
%    appropriate.
%
%    Encode JNG datastreams.  Mostly done as of 5.5.2; need to write
%    ancillary text chunks and save profiles.
%
%    Provide an option to force LC files (to ensure exact framing rate)
%    instead of VLC.
%
%    Provide an option to force VLC files instead of LC, even when offsets
%    are present.  This will involve expanding the embedded images with a
%    transparent region at the top and/or left.
*/


static void
png_write_raw_profile(const ImageInfo *image_info,png_struct *ping,
                      png_info *ping_info, const char *profile_type,
                      const char *profile_description,
                      const unsigned char *profile_data,
                      png_uint_32 length)
{
  png_textp
    text;

  register long
    i;

  const unsigned char
    *sp;

  png_charp
    dp;

  png_uint_32
    allocated_length,
    description_length;

  unsigned char
    hex[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

  if (image_info->verbose)
    {
      (void) printf("writing raw profile: type=%.1024s, length=%lu\n",
                    profile_type, (unsigned long)length);
    }
#if PNG_LIBPNG_VER >= 14000
  text=(png_textp) png_malloc(ping,(png_alloc_size_t) sizeof(png_text));
#else
  text=(png_textp) png_malloc(ping,(png_size_t) sizeof(png_text));
#endif
  description_length=(png_uint_32) strlen((const char *) profile_description);
  allocated_length=(png_uint_32) (length*2 + (length >> 5) + 20
                                  + description_length);
#if PNG_LIBPNG_VER >= 14000
   text[0].text=(png_charp) png_malloc(ping,
      (png_alloc_size_t) allocated_length);
   text[0].key=(png_charp) png_malloc(ping, (png_alloc_size_t) 80);
#else
   text[0].text=(png_charp) png_malloc(ping, (png_size_t) allocated_length);
   text[0].key=(png_charp) png_malloc(ping, (png_size_t) 80);
#endif
  text[0].key[0]='\0';
  (void) strcat(text[0].key, "Raw profile type ");
  (void) strncat(text[0].key, (const char *) profile_type, 61);
  sp=profile_data;
  dp=text[0].text;
  *dp++='\n';
  (void) strcpy(dp,(const char *) profile_description);
  dp+=description_length;
  *dp++='\n';
  (void) sprintf(dp,"%8lu ",(unsigned long)length);
  dp+=8;
  for (i=0; i < (long) length; i++)
    {
      if (i%36 == 0)
        *dp++='\n';
      *(dp++)=hex[((*sp >> 4) & 0x0f)];
      *(dp++)=hex[((*sp++ ) & 0x0f)];
    }
  *dp++='\n';
  *dp='\0';
  text[0].text_length=dp-text[0].text;
  text[0].compression=image_info->compression == NoCompression ||
    (image_info->compression == UndefinedCompression &&
     text[0].text_length < 128) ? -1 : 0;
  if (text[0].text_length <= allocated_length)
    png_set_text(ping,ping_info,text,1);
  png_free(ping,text[0].text);
  png_free(ping,text[0].key);
  png_free(ping,text);
}

static MagickPassFail WriteOnePNGImage(MngInfo *mng_info,
                                       const ImageInfo *image_info,Image *imagep)
{
  Image
    * volatile imagev = imagep,  /* Use only 'imagev' before setjmp() */
    *image;                      /* Use only 'image' after setjmp() */

  /* Write one PNG image */
  const ImageAttribute
    *attribute;

  char
    s[2];

  int
    num_passes,
    pass,
    ping_bit_depth = 0,
    ping_colortype = 0,
    ping_interlace_method = 0,
    ping_compression_method = 0;

  volatile int
    ping_filter_method = 0,
    ping_num_trans = 0,
    ping_valid_trns = 0;

  volatile png_bytep
    ping_trans_alpha = NULL;

  png_colorp
    palette;

  png_color_16
    ping_background,
    ping_trans_color;

  png_info
    *ping_info;

  png_struct
    *ping;

  png_uint_32
    ping_width,
    ping_height;

  long
    y;

  register const IndexPacket
    *indexes;

  register long
    i,
    x;

  unsigned char
    *png_pixels;

  unsigned int
    image_colors,
    image_depth,
    image_matte,
    logging,
    matte;

  unsigned long
    quantum_size,  /* depth for ExportImage */
    rowbytes,
    save_image_depth;

  ImageCharacteristics
    characteristics;

  logging=LogMagickEvent(CoderEvent,GetMagickModule(),
                         "  enter WriteOnePNGImage()");

  /* Initialize some stuff */
  ping_background.red = 0;
  ping_background.green = 0;
  ping_background.blue = 0;
  ping_background.gray = 0;
  ping_background.index = 0;

  ping_trans_color.red=0;
  ping_trans_color.green=0;
  ping_trans_color.blue=0;
  ping_trans_color.gray=0;

  /*
    Allocate the PNG structures
  */
#ifdef PNG_USER_MEM_SUPPORTED
  ping=png_create_write_struct_2(PNG_LIBPNG_VER_STRING,imagev,
                                 PNGErrorHandler,PNGWarningHandler,
                                 (void *) NULL,
                                 (png_malloc_ptr) png_IM_malloc,
                                 (png_free_ptr) png_IM_free);
#else
  ping=png_create_write_struct(PNG_LIBPNG_VER_STRING,imagev,
                               PNGErrorHandler,PNGWarningHandler);
#endif
  if (ping == (png_struct *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,imagev);
  ping_info=png_create_info_struct(ping);
  if (ping_info == (png_info *) NULL)
    {
      png_destroy_write_struct(&ping,(png_info **) NULL);
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,imagev);
    }
  png_set_write_fn(ping,imagev,png_put_data,png_flush_data);
  png_pixels=(unsigned char *) NULL;

#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
  LockSemaphoreInfo(png_semaphore);
#endif

  if (setjmp(png_jmpbuf(ping)))
    {
      /*
        PNG write failed.
      */
#ifdef PNG_DEBUG
      if (image_info->verbose)
        (void) printf("PNG write has failed.\n");
#endif
      png_destroy_write_struct(&ping,&ping_info);
      MagickFreeMemory(png_pixels);
#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
      UnlockSemaphoreInfo(png_semaphore);
#endif
      return(MagickFail);
    }

/* { For navigation to end of setjmp-controlled block */

#ifdef PNG_BENIGN_ERRORS_SUPPORTED
  /* Allow benign errors */
  png_set_benign_errors(ping, 1);
#endif

  image=imagev;  /* Use 'image' after this point for optimization */

  /*
    Make sure that image is in an RGB type space.
  */
  if (!TransformColorspace(image,RGBColorspace))
    {
      CloseBlob(image);
#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
      UnlockSemaphoreInfo(png_semaphore);
#endif
      return MagickFail;
    }

  /*
    Analyze image to be written.
  */
  if (!GetImageCharacteristics(image,&characteristics,
                               (OptimizeType == image_info->type),
                               &image->exception))
    {
      CloseBlob(image);
#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
      UnlockSemaphoreInfo(png_semaphore);
#endif
      return MagickFail;
    }

  if (logging)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "  Image Characteristics:\n"
                            "    CMYK:       %u\n"
                            "    Grayscale:  %u\n"
                            "    Monochrome: %u\n"
                            "    Opaque:     %u\n"
                            "    Palette:    %u",
                            characteristics.cmyk,
                            characteristics.grayscale ,
                            characteristics.monochrome,
                            characteristics.opaque,
                            characteristics.palette);
    }

  if (image->storage_class == PseudoClass)
    image_colors=image->colors;
  else
    image_colors=0;

  image_depth=image->depth;
  image_matte=image->matte;

  if (image->storage_class == PseudoClass &&
      image_colors <= 256)
    {
      mng_info->IsPalette=MagickTrue;
      image_depth=8;
    }

  if (mng_info->write_png8 || mng_info->write_png24 || mng_info->write_png32)
    image_depth=8;

  else if (mng_info->write_png48 || mng_info->write_png64)
      image_depth=16;

  else if (image_depth < 8)
    image_depth=8;

  else if (image_depth > 8)
    image_depth=16;

  /*
    Prepare PNG for writing.
  */

#if defined(PNG_MNG_FEATURES_SUPPORTED)
  if (mng_info->write_mng)
  {
# ifdef PNG_WRITE_CHECK_FOR_INVALID_INDEX_SUPPORTED
    /* Disable new libpng-1.5.10 feature when writing a MNG */
    png_set_check_for_invalid_index (ping, 0);
# endif
    (void) png_permit_mng_features(ping,PNG_ALL_MNG_FEATURES);
  }
#else
# ifdef PNG_WRITE_EMPTY_PLTE_SUPPORTED
  if (mng_info->write_mng)
    png_permit_empty_plte(ping,MagickTrue);
# endif
#endif
  x=0;
  ping_width=image->columns;
  ping_height=image->rows;
  if (logging)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    width=%lu",
                            (unsigned long)ping_width);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    height=%lu",
                            (unsigned long)ping_height);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    image->depth=%u",image_depth);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "  image_colors=%u",image_colors);
    }


  quantum_size=(image_depth > 8) ? 16:8;

  save_image_depth=image_depth;
  ping_bit_depth=(png_byte) save_image_depth;
  if (logging)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    ping_bit_depth=%u",
                            ping_bit_depth);
    }
  /*
    Select the color type.
  */
  matte=image_matte;
  if (mng_info->write_png8)
    {
      ping_colortype=PNG_COLOR_TYPE_PALETTE;
      ping_bit_depth=8;
      {
        /* TO DO: make this a function cause it's used twice, except
           for reducing the sample depth from 8. */

        QuantizeInfo
          quantize_info;

        unsigned long
          number_colors;

        number_colors=image_colors;
        if ((number_colors == 0) || (number_colors > 256))
          {
            GetQuantizeInfo(&quantize_info);
            quantize_info.dither=image_info->dither;
            quantize_info.number_colors=256;
            (void) QuantizeImage(&quantize_info,image);
            number_colors=image->colors;
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "    Colors quantized to %ld",
                                    number_colors);
            image_colors=image->colors;
          }

        /*
          Set image palette.
        */

        ping_colortype=PNG_COLOR_TYPE_PALETTE;

        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "  Setting up PLTE chunk with %d colors",
                                (int) number_colors);
        palette=MagickAllocateMemory(png_color *,
                                     number_colors*sizeof(png_color));
        if (palette == (png_color *) NULL)
          png_error(ping, "Could not allocate palette");
        else
        {
          for (i=0; i < (long) number_colors; i++)
            {
              palette[i].red=ScaleQuantumToChar(image->colormap[i].red);
              palette[i].green=ScaleQuantumToChar(image->colormap[i].green);
              palette[i].blue=ScaleQuantumToChar(image->colormap[i].blue);
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
#if QuantumDepth == 8
                                      "    %3ld (%3d,%3d,%3d)",
#else
                                      "    %5ld (%5d,%5d,%5d)",
#endif
                                      i,palette[i].red,palette[i].green,
                                      palette[i].blue);

            }
          png_set_PLTE(ping,ping_info,palette,(int) number_colors);
          MagickFreeMemory(palette);
        }
        /*
          Identify which colormap entry is transparent.
        */
        ping_trans_alpha=MagickAllocateMemory(unsigned char *, number_colors);
        if (ping_trans_alpha == (unsigned char *) NULL)
          png_error(ping, "Could not allocate ping_trans_alpha");
        else
          {
            assert(number_colors <= 256);
            for (i=0; i < (long) number_colors; i++)
              ping_trans_alpha[i]=255;
            for (y=0; y < (long) image->rows; y++)
              {
                register const PixelPacket
                  *p=NULL;

                p=AcquireImagePixels(image,0,y,image->columns,1,
                                     &image->exception);
                if (p == (const PixelPacket *) NULL)
                  break;
                indexes=AccessImmutableIndexes(image);
                for (x=0; x < (long) image->columns; x++)
                  {
                    if (p->opacity != OpaqueOpacity)
                      {
                        IndexPacket
                          index;

                        index=indexes[x];
                        assert((unsigned long) index < number_colors);
                        ping_trans_alpha[index]=(png_byte) (255-
                                                ScaleQuantumToChar(p->opacity));
                      }
                    p++;
                  }
              }
            ping_num_trans=0;
            for (i=0; i < (long) number_colors; i++)
              if (ping_trans_alpha[i] != 255)
                ping_num_trans=(unsigned short) (i+1);
            if (ping_num_trans != 0)
              ping_valid_trns = 1;
            MagickFreeMemory(ping_trans_alpha);
          }
        /*
          Identify which colormap entry is the background color.
        */
        for (i=0; i < (long) Max(number_colors-1,1); i++)
          if (RGBColorMatchExact(ping_background,image->colormap[i]))
            break;
        ping_background.index=(png_uint_16) i;
      }
      if (image_matte)
        {
          /* TO DO: reduce to binary transparency */
        }
    }
  else if (mng_info->write_png24 || mng_info->write_png48)
    {
      image_matte=MagickFalse;
      ping_colortype=PNG_COLOR_TYPE_RGB;
    }
  else if (mng_info->write_png32 || mng_info->write_png64)
    {
      image_matte=MagickTrue;
      ping_colortype=PNG_COLOR_TYPE_RGB_ALPHA;
    }
  else
    {
      if (ping_bit_depth < 8)
        ping_bit_depth=8;

      ping_colortype=PNG_COLOR_TYPE_RGB;
      if (characteristics.monochrome)
        {
          if (characteristics.opaque)
            {
              ping_colortype=PNG_COLOR_TYPE_GRAY;
              ping_bit_depth=1;
            }
          else
            {
              ping_colortype=PNG_COLOR_TYPE_GRAY_ALPHA;
            }
        }
      else if (characteristics.grayscale)
        {
          if (characteristics.opaque)
            ping_colortype=PNG_COLOR_TYPE_GRAY;
          else
            ping_colortype=PNG_COLOR_TYPE_GRAY_ALPHA;
        }
      else if (characteristics.palette && image_colors <= 256)
        {
          ping_colortype=PNG_COLOR_TYPE_PALETTE;
          ping_bit_depth=8;
          mng_info->IsPalette=MagickTrue;
        }
      else
        {
          if (characteristics.opaque)
            ping_colortype=PNG_COLOR_TYPE_RGB;
          else
            ping_colortype=PNG_COLOR_TYPE_RGB_ALPHA;
        }
      if (image_info->type == BilevelType)
        {
          if (characteristics.monochrome)
            {
              if (!image_matte)
                ping_bit_depth=1;
            }
        }
      if (image_info->type == GrayscaleType)
        ping_colortype=PNG_COLOR_TYPE_GRAY;
      if (image_info->type == GrayscaleMatteType)
        ping_colortype=PNG_COLOR_TYPE_GRAY_ALPHA;

      if (logging)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "    Tentative PNG color type: %s (%d)",
                                PngColorTypeToString(ping_colortype),
                                ping_colortype);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "    image_info->type: %d",image_info->type);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "    image->depth: %u",image_depth);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "    ping_bit_depth: %d",
                                ping_bit_depth);
        }

      if (matte && mng_info->IsPalette)
        {
          register const PixelPacket
            *p=NULL;

          if (characteristics.opaque)
            {
              /*
                No transparent pixels are present.  Change 4 or 6 to 0 or 2,
                and unset the ping_valid_trns flag.
              */
              image_matte=MagickFalse;
              ping_colortype&=0x03;
              ping_valid_trns=0;
            }
          else
            {
              unsigned int
                mask;

              mask=0xffff;
              if (ping_bit_depth == 8)
                mask=0x00ff;
              if (ping_bit_depth == 4)
                mask=0x000f;
              if (ping_bit_depth == 2)
                mask=0x0003;
              if (ping_bit_depth == 1)
                mask=0x0001;

              /*
                Find a transparent color.
              */
              for (y=0; y < (long) image->rows; y++)
                {
                  p=AcquireImagePixels(image,0,y,image->columns,1,
                                       &image->exception);
                  if (p == (const PixelPacket *) NULL)
                    break;
                  for (x=(long) image->columns; x > 0; x--)
                    {
                      if (p->opacity != OpaqueOpacity)
                        break;
                      p++;
                    }
                  if (p->opacity != OpaqueOpacity)
                    break;
                }
              if ((p != (const PixelPacket *) NULL) &&
                  (p->opacity != OpaqueOpacity))
                {
                  ping_trans_color.red=ScaleQuantumToShort(p->red)&mask;
                  ping_trans_color.green=ScaleQuantumToShort(p->green)
                                                &mask;
                  ping_trans_color.blue=ScaleQuantumToShort(p->blue)
                                               &mask;
                  ping_trans_color.gray=
                    (png_uint_16) ScaleQuantumToShort(PixelIntensity(p))&mask;
                  ping_trans_color.index=(unsigned char)
                    (ScaleQuantumToChar(MaxRGB-p->opacity));
                  ping_valid_trns=1;
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                    "  Found transparent color: (%d,%d,%d)",
                    ping_trans_color.red, ping_trans_color.green,
                    ping_trans_color.blue);
                }
            }
          if (ping_valid_trns != 0)
            {
              /*
                Determine if there is one and only one transparent color
                and if so if it is fully transparent.
              */
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "  Determine if there is exactly one transparent color");
              for (y=0; y < (long) image->rows; y++)
                {
                  p=AcquireImagePixels(image,0,y,image->columns,1,
                                       &image->exception);
                  x=0;
                  if (p == (const PixelPacket *) NULL)
                    break;
                  for (x=(long) image->columns; x > 0; x--)
                    {
                      if (p->opacity != OpaqueOpacity)
                        {
                          if (!RGBColorMatchExact(ping_trans_color,*p))
                            {
                              break;  /* Can't use RGB + tRNS for multiple
                                         transparent colors.  */
                            }
                          if (p->opacity != TransparentOpacity)
                            {
                              break;  /* Can't use RGB + tRNS for
                                         semitransparency. */
                            }
                        }
                      else
                        {
                          if (RGBColorMatchExact(ping_trans_color,*p))
                            break; /* Can't use RGB + tRNS when another pixel
                                      having the same RGB samples is
                                      transparent. */
                        }
                      p++;
                    }
                  if (x != 0)
                    break;
                } 
              if (x != 0)
                {
                  ping_valid_trns = 0;
                }
              else if (logging)
                {
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "Single transparent color (%d,%d,%d index=%d)",
                                        ping_trans_color.red,ping_trans_color.green,
                                        ping_trans_color.blue,ping_trans_color.index);
                }
            }
          if (ping_valid_trns != 0)
            {
              ping_colortype &= 0x03;  /* changes 4 or 6 to 0 or 2 */
              if (image_depth == 8)
                {
                  ping_trans_color.red&=0xff;
                  ping_trans_color.green&=0xff;
                  ping_trans_color.blue&=0xff;
                  ping_trans_color.gray&=0xff;
                }
            }
        }
      matte=image_matte;
      if (ping_valid_trns != 0)
        image_matte=MagickFalse;
      if (mng_info->IsPalette &&
          characteristics.grayscale && (!image_matte || image_depth >= 8))
        {
          if (image_matte)
            ping_colortype=PNG_COLOR_TYPE_GRAY_ALPHA;
          else
            {
              ping_colortype=PNG_COLOR_TYPE_GRAY;
              if (save_image_depth == 16 && image_depth == 8)
                ping_trans_color.gray*=0x0101;
            }
          if (image_depth > QuantumDepth)
            image_depth=QuantumDepth;
          if (image_colors == 0 || image_colors-1 > MaxRGB)
            image_colors=1 << image_depth;
          if (image_depth > 8)
            ping_bit_depth=16;
          else
            {
              if (ping_colortype == PNG_COLOR_TYPE_PALETTE)
                {
                  ping_bit_depth=1;
                  while ((int) (1 << ping_bit_depth) <
                                (long) image_colors)
                    ping_bit_depth <<= 1;
                }
            }
        }
      else
        if (mng_info->IsPalette)
          {
            if (image_depth <= 8)
              {
                unsigned long
                  number_colors;

                number_colors=image_colors;
                /*
                  Set image palette.
                */
                ping_colortype=PNG_COLOR_TYPE_PALETTE;
                if (mng_info->have_write_global_plte && !matte)
                  {
                    png_set_PLTE(ping,ping_info,NULL,0);
                    if (logging)
                      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                            "  Setting up empty PLTE chunk");
                  }
                else
                  {
                    palette=MagickAllocateArray(png_color *,
                                                number_colors,
                                                sizeof(png_color));
                    if (palette == (png_color *) NULL)
                      png_error(ping, "Could not allocate palette");
                    for (i=0; i < (long) number_colors; i++)
                      {
                        palette[i].red=ScaleQuantumToChar
                                       (image->colormap[i].red);
                        palette[i].green=ScaleQuantumToChar
                                       (image->colormap[i].green);
                        palette[i].blue=ScaleQuantumToChar
                                       (image->colormap[i].blue);
                      }
                    if (logging)
                      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                           "  Setting up PLTE chunk with %d colors",
                           (int) number_colors);
                    png_set_PLTE(ping,ping_info,palette,(int) number_colors);
                    MagickFreeMemory(palette);
                  }
                ping_bit_depth=1;
                while ((1UL << ping_bit_depth) < number_colors)
                  ping_bit_depth <<= 1;
                ping_num_trans=0;
                if (matte)
                  {
                    int
                      num_alpha=0,
                      trans_alpha[256];

                    /*
                      Identify which colormap entry is transparent.
                    */
                    assert(number_colors <= 256);

                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                     "  Look for transparent colormap entry, number_colors=%d",
                        (int) number_colors);

                    for (i=0; i < (long) number_colors; i++)
                      trans_alpha[i]=256;

                    for (y=0; y < (long) image->rows; y++)
                      {
                        register const PixelPacket
                          *p=NULL;

                        p=AcquireImagePixels(image,0,y,image->columns,1,
                                             &image->exception);
                        if (p == (const PixelPacket *) NULL)
                          break;
                        indexes=AccessImmutableIndexes(image);
                        for (x=0; x < (long) image->columns; x++)
                          {
                            if (p->opacity != OpaqueOpacity)
                              {
                                IndexPacket
                                  index;

                                index=indexes[x];
                                assert((unsigned long) index < number_colors);
                                if (trans_alpha[index] == 256)
                                  {
                                    (void) LogMagickEvent(CoderEvent,
                                      GetMagickModule(),
                                      "  Index=%d is transparent",(int) index);
                                    trans_alpha[index]=(png_byte) (255-
                                      ScaleQuantumToChar(p->opacity));
                                    num_alpha++;
                                  }
                              }
                            p++;
                          }
                      }

                      (void) LogMagickEvent(CoderEvent, GetMagickModule(),
                         "  num_alpha = %d",num_alpha);
                      if (num_alpha == 1)
                         ping_valid_trns=1;
                      else
                      {
                         ping_colortype=PNG_COLOR_TYPE_RGB_ALPHA;
                         ping_num_trans=0;
                         if (ping_bit_depth < 8)
                            ping_bit_depth=8;
                         ping_valid_trns = 0;
                         png_set_invalid(ping, ping_info, PNG_INFO_PLTE);
                         mng_info->IsPalette=MagickFalse;
                         (void) SyncImage(image);
                         if (logging)
                           (void) LogMagickEvent(CoderEvent,
                                                 GetMagickModule(),
                                                 "    Cannot write image"
                                                 " as indexed PNG,"
                                                 " writing RGBA.");
                       }

                    if (ping_valid_trns != 0)
                      {
                        for (i=0; i < (long) number_colors; i++)
                          {
                            if (trans_alpha[i] == 256)
                              trans_alpha[i]=255;
                            if (trans_alpha[i] != 255)
                              ping_num_trans=(unsigned short) (i+1);
                          }
                      (void) LogMagickEvent(CoderEvent, GetMagickModule(),
                         "  ping_num_trans = %d",ping_num_trans);
                      }
                    if (ping_num_trans == 0)
                       ping_valid_trns = 0;
                    else
                       ping_valid_trns = 1;
                    if (ping_valid_trns == 0)
                       ping_num_trans=0;

                    if (ping_valid_trns != 0)
                      {
                        ping_trans_alpha=MagickAllocateMemory(
                          unsigned char *, number_colors);
                        if (ping_trans_alpha == (unsigned char *) NULL)
                          png_error(ping, "Could not allocate trans_alpha");

                        for (i=0; i<(int) number_colors; i++)
                          if (trans_alpha[i] == 256)
                             ping_trans_alpha[i]=255;
                          else
                             ping_trans_alpha[i]=(png_byte) trans_alpha[i];
                         (void) LogMagickEvent(CoderEvent, GetMagickModule(),
                            "    Alpha[%d]=%d",(int) i, (int) trans_alpha[i]);
                      }
                  }

                /*
                  Identify which colormap entry is the background color.
                */
                for (i=0; i < (long) Max(number_colors-1,1); i++)
                  if (RGBColorMatchExact(ping_background,
                                         image->colormap[i]))
                    break;
                ping_background.index=(png_uint_16) i;
              }
          }
        else
          {
            if (image_depth < 8)
              image_depth=8;
            if ((save_image_depth == 16) && (image_depth == 8))
              {
                ping_trans_color.red*=0x0101;
                ping_trans_color.green*=0x0101;
                ping_trans_color.blue*=0x0101;
                ping_trans_color.gray*=0x0101;
              }
          }

      /*
        Adjust background and transparency samples in sub-8-bit
        grayscale files.
      */
      if (ping_bit_depth < 8 && ping_colortype ==
          PNG_COLOR_TYPE_GRAY)
        {
          png_uint_16
            maxval;

          maxval=(1 << ping_bit_depth)-1;

          ping_trans_color.gray=(png_uint_16)(maxval*
                                ping_trans_color.gray/
                                MaxRGB);
        }
    }

  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "    PNG color type: %s (%d)",
                          PngColorTypeToString(ping_colortype),
                          ping_colortype);
  /*
    Initialize compression level and filtering.
  */
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  Setting up deflate compression");
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "    Compression buffer size: 32768");
  png_set_compression_buffer_size(ping,32768L);
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "    Compression mem level: 9");
  png_set_compression_mem_level(ping, 9);
  if (image_info->quality > 9)
    {
      int
        level;

      level=(int) Min(image_info->quality/10,9);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "    Compression level: %d",level);
      png_set_compression_level(ping,level);
    }
  else
    {
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "    Compression strategy: Z_HUFFMAN_ONLY");
      png_set_compression_strategy(ping, Z_HUFFMAN_ONLY);
      png_set_compression_level(ping,2);
    }
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  Setting up filtering");
#if defined(PNG_MNG_FEATURES_SUPPORTED) && defined(PNG_INTRAPIXEL_DIFFERENCING)

  /* This became available in libpng-1.0.9.  Output must be a MNG. */
  if (mng_info->write_mng && ((image_info->quality % 10) == 7))
    {
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "    Filter_type: PNG_INTRAPIXEL_DIFFERENCING");
      ping_filter_method=PNG_INTRAPIXEL_DIFFERENCING;
    }
  else
    if (logging)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    Filter_type: 0");
#endif
  {
    int
      base_filter;

    if ((image_info->quality % 10) > 5)
      base_filter=PNG_ALL_FILTERS;
    else
      if ((image_info->quality % 10) != 5)
        base_filter=(int) image_info->quality % 10;
      else
        if ((ping_colortype == PNG_COLOR_TYPE_GRAY) ||
            (ping_colortype == PNG_COLOR_TYPE_PALETTE) ||
            (image_info->quality < 50))
          base_filter=PNG_NO_FILTERS;
        else
          base_filter=PNG_ALL_FILTERS;
    if (logging)
      {
        if (base_filter == PNG_ALL_FILTERS)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "    Base filter method: ADAPTIVE");
        else
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "    Base filter method: NONE");
      }
    png_set_filter(ping,PNG_FILTER_TYPE_BASE,base_filter);
  }

  ping_interlace_method=(image_info->interlace == LineInterlace);

  if (mng_info->write_mng)
    png_set_sig_bytes(ping,8);

  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  Writing PNG header chunks");

  png_set_IHDR(ping,ping_info,
               ping_width,
               ping_height,
               ping_bit_depth,
               ping_colortype,
               ping_interlace_method,
               ping_compression_method,
               ping_filter_method);

  if (ping_colortype == 3 && ping_valid_trns != 0)
     {
        if (logging)
           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Setting up tRNS chunk");
        (void) png_set_tRNS(ping, ping_info,
                           ping_trans_alpha,
                           ping_num_trans,
                           &ping_trans_color);
     }

  MagickFreeMemory(ping_trans_alpha);

  if (image_matte && (!mng_info->adjoin || !mng_info->equal_backgrounds))
    {
      if (logging)
         (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  Setting up bKGD chunk");
      if (ping_bit_depth < 8 && ping_colortype == PNG_COLOR_TYPE_GRAY)
        {
          png_uint_16
            maxval;

          png_color_16
            background;

          maxval=(1 << ping_bit_depth)-1;


          background.gray=(png_uint_16)
            (maxval*(PixelIntensity(&image->background_color))/MaxRGB);

          png_set_bKGD(ping,ping_info,&background);
        }

      else
        {
          png_color_16
            background;

          if (image_depth < QuantumDepth)
            {
              int
                maxval;

              maxval=(1 << image_depth)-1;
              background.red=(png_uint_16)
                (maxval*image->background_color.red/MaxRGB);
              background.green=(png_uint_16)
                (maxval*image->background_color.green/MaxRGB);
              background.blue=(png_uint_16)
                (maxval*image->background_color.blue/MaxRGB);
              background.gray=(png_uint_16)
                (maxval*PixelIntensity(&image->background_color)/MaxRGB);
            }
          else
            {
              background.red=image->background_color.red;
              background.green=image->background_color.green;
              background.blue=image->background_color.blue;
              background.gray=
                (png_uint_16) PixelIntensity(&image->background_color);
            }
          background.index=(png_byte) background.gray;
          png_set_bKGD(ping,ping_info,&background);
        }
    }

#if defined(PNG_pHYs_SUPPORTED)
  if ((image->x_resolution != 0) && (image->y_resolution != 0) &&
      (!mng_info->write_mng || !mng_info->equal_physs))
    {
      int
        unit_type;

      png_uint_32
        x_resolution,
        y_resolution;

      if (image->units == PixelsPerInchResolution)
        {
          unit_type=PNG_RESOLUTION_METER;
          x_resolution=(png_uint_32) ((100.0*image->x_resolution+0.5)/2.54);
          y_resolution=(png_uint_32) ((100.0*image->y_resolution+0.5)/2.54);
        }
      else if (image->units == PixelsPerCentimeterResolution)
        {
          unit_type=PNG_RESOLUTION_METER;
          x_resolution=(png_uint_32) (100.0*image->x_resolution+0.5);
          y_resolution=(png_uint_32) (100.0*image->y_resolution+0.5);
        }
      else
        {
          unit_type=PNG_RESOLUTION_UNKNOWN;
          x_resolution=(png_uint_32) image->x_resolution;
          y_resolution=(png_uint_32) image->y_resolution;
        }

      png_set_pHYs(ping,ping_info,x_resolution,y_resolution,unit_type);

      if (logging)
      {
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "    Setting up pHYs chunk");
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "      x_resolution=%lu",(unsigned long) x_resolution);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "      y_resolution=%lu",(unsigned long) y_resolution);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "      unit_type=%lu",(unsigned long) unit_type);
      }
    }
#endif

#if defined(PNG_oFFs_SUPPORTED)
  if (mng_info->write_mng == 0 && (image->page.x || image->page.y))
    {
      png_set_oFFs(ping,ping_info,(png_int_32) image->page.x,
                   (png_int_32) image->page.y, 0);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "    Setting up oFFs chunk");
    }
#endif

  {
    ImageProfileIterator
      *profile_iterator;

    profile_iterator=AllocateImageProfileIterator(image);
    if (profile_iterator)
      {
        const char
          *profile_name;

        const unsigned char
          *profile_info;

        size_t
          profile_length;

        while (NextImageProfile(profile_iterator,&profile_name,&profile_info,
                                &profile_length) != MagickFail)
          {
            if (LocaleCompare(profile_name,"ICM") == 0)
              {
#if defined(PNG_WRITE_iCCP_SUPPORTED)
                {
                  if (logging)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "  Setting up iCCP chunk");

                  png_set_iCCP(ping,ping_info,(png_charp) "icm",
                               (int) 0,
#if (PNG_LIBPNG_VER < 10500)
                               (png_charp) profile_info,
#else
                               (png_const_bytep) profile_info,
#endif

                               (png_uint_32) profile_length);
                }
#else
                {
                  if (logging)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "  Setting up text chunk with"
                                          " iCCP Profile");
                  png_write_raw_profile(image_info,ping,ping_info,
                                        "icm",
                                        "ICC Profile",
                                        profile_info,
                                        (png_uint_32) profile_length);
                }
#endif
              }
            else if (LocaleCompare(profile_name,"IPTC") == 0)
              {
                if (logging)
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "  Setting up text chunk with"
                                        " iPTC Profile");
                png_write_raw_profile(image_info,ping,ping_info,
                                      "iptc",
                                      "IPTC profile",
                                      profile_info,
                                      (png_uint_32) profile_length);
              }
            else
              {
                if (logging)
                  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                        "  Setting up text chunk with"
                                        " %s profile",
                                        profile_name);
                png_write_raw_profile(image_info,ping,ping_info,
                                      profile_name,
                                      "generic profile",
                                      profile_info,
                                      (png_uint_32) profile_length);
              }
          }

        DeallocateImageProfileIterator(profile_iterator);
      }
  }

#if defined(PNG_WRITE_sRGB_SUPPORTED)
  if (!mng_info->have_write_global_srgb &&
      ((image->rendering_intent != UndefinedIntent) ||
       image_info->colorspace == sRGBColorspace))
    {
      /*
        Note image rendering intent.
      */
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "  Setting up sRGB chunk");
      if (image->rendering_intent != UndefinedIntent)
        (void) png_set_sRGB(ping,ping_info,(int) (image->rendering_intent-1));
      else
        (void) png_set_sRGB(ping,ping_info,PerceptualIntent);
      png_set_gAMA(ping,ping_info,0.45455);
    }
  if ((!mng_info->write_mng) || 
       !png_get_valid(ping, ping_info, PNG_INFO_sRGB))
#endif
    {
      if (!mng_info->have_write_global_gama && (image->gamma != 0.0))
        {
          /*
            Note image gamma.
            To do: check for cHRM+gAMA == sRGB, and write sRGB instead.
          */
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "  Setting up gAMA chunk");
          png_set_gAMA(ping,ping_info,image->gamma);
        }
      if (!mng_info->have_write_global_chrm &&
          (image->chromaticity.red_primary.x != 0.0))
        {
          /*
            Note image chromaticity.
            To do: check for cHRM+gAMA == sRGB, and write sRGB instead.
          */
          PrimaryInfo
            bp,
            gp,
            rp,
            wp;

          wp=image->chromaticity.white_point;
          rp=image->chromaticity.red_primary;
          gp=image->chromaticity.green_primary;
          bp=image->chromaticity.blue_primary;

          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "  Setting up cHRM chunk");
          png_set_cHRM(ping,ping_info,wp.x,wp.y,rp.x,rp.y,gp.x,gp.y,
                       bp.x,bp.y);
        }
    }

  png_write_info(ping,ping_info);

#if (PNG_LIBPNG_VER == 10206)
  /* avoid libpng-1.2.6 bug by setting PNG_HAVE_IDAT flag */
#define PNG_HAVE_IDAT               0x04
  ping->mode |= PNG_HAVE_IDAT;
#undef PNG_HAVE_IDAT
#endif

  png_set_packing(ping);
  /*
    Allocate memory.
  */
  rowbytes=image->columns;
  if (image_depth <= 8)
    {
      if (mng_info->write_png24)
        rowbytes*=3;
      else if (mng_info->write_png32)
        rowbytes*=4;
      else if (!mng_info->write_png8 && (mng_info->IsPalette &&
                IsGrayImage(image,&image->exception)))
        rowbytes*=(image_matte ? 2 : 1);
      else
        {
          if (!mng_info->IsPalette)
            rowbytes*=(image_matte ? 4 : 3);
        }
    }
  else
    {
      if (mng_info->write_png48)
        rowbytes*=6;

      else if (mng_info->write_png64)
        rowbytes*=8;

      else if (mng_info->IsPalette &&
          IsGrayImage(image,&image->exception))
        rowbytes*=(image_matte ? 4 : 2);

      else
        rowbytes*=(image_matte ? 8 : 6);
    }
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  Allocating %lu bytes of memory for pixels",
                          rowbytes);
  png_pixels=MagickAllocateMemory(unsigned char *,rowbytes);
  if (png_pixels == (unsigned char *) NULL)
    png_error(ping, "Could not allocate png_pixels");
  /*
    Initialize image scanlines.
  */
  num_passes=png_set_interlace_handling(ping);
  if ((!mng_info->write_png8 && !mng_info->write_png24 &&
       !mng_info->write_png32 && !mng_info->write_png48 &&
       !mng_info->write_png64) && (mng_info->IsPalette ||
       image_info->type == BilevelType) &&
       !image_matte &&
       IsMonochromeImage(image,&image->exception))
    for (pass=0; pass < num_passes; pass++)
      {
        /*
          Convert PseudoClass image to a PNG monochrome image.
        */
        if (logging)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "  pass %d, Image Is Monochrome",pass);
        for (y=0; y < (long) image->rows; y++)
          {
            if (!AcquireImagePixels(image,0,y,image->columns,1,
                                    &image->exception))
              break;
            if (mng_info->IsPalette)
              (void) ExportImagePixelArea(image,(QuantumType) GrayQuantum,
                                          quantum_size,png_pixels,0,0);
            else
              (void) ExportImagePixelArea(image,(QuantumType) RedQuantum,
                                          quantum_size,png_pixels,0,0);
            for (i=0; i < (long) image->columns; i++)
              *(png_pixels+i)=(*(png_pixels+i) > 128) ? 255 : 0;
            png_write_row(ping,png_pixels);
            if (image->previous == (Image *) NULL)
              if (QuantumTick((magick_uint64_t) y*(pass+1),
                              (magick_uint64_t) image->rows * num_passes))
                if (!MagickMonitorFormatted((magick_uint64_t) y*(pass+1),
                                            (magick_uint64_t) image->rows*
                                            num_passes,
                                            &image->exception,
                                            SaveImageTag,
                                            image->filename,
					    image->columns,image->rows))
                  break;

          }
      }
  else
    for (pass=0; pass < num_passes; pass++)
      {
        if ((!mng_info->write_png8 && !mng_info->write_png24 &&
             !mng_info->write_png32 && !mng_info->write_png48 &&
             !mng_info->write_png64) &&
            (!image_matte || (ping_bit_depth >= QuantumDepth)) &&
            mng_info->IsPalette &&
            IsGrayImage(image,&image->exception))
          {
            if (logging)
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "  pass %d, Image Is Gray",pass);
            for (y=0; y < (long) image->rows; y++)
              {
                if (!AcquireImagePixels(image,0,y,image->columns,1,
                                        &image->exception))
                  break;
                if (ping_colortype == PNG_COLOR_TYPE_GRAY)
                  {
                    if (mng_info->IsPalette)
                      (void) ExportImagePixelArea(image,
                                                  (QuantumType) GrayQuantum,
                                                  quantum_size,png_pixels,0,0);
                    else
                      (void) ExportImagePixelArea(image,
                                                  (QuantumType) RedQuantum,
                                                  quantum_size,
                                                 png_pixels,0,0);
                  }
                else if (ping_colortype == PNG_COLOR_TYPE_PALETTE)
                  {
                    (void) ExportImagePixelArea(image,(QuantumType)
                                                IndexQuantum,
                                                quantum_size,
                                                png_pixels,0,0);
                  }
                else /* PNG_COLOR_TYPE_GRAY_ALPHA */
                  {
                    (void) ExportImagePixelArea(image,(QuantumType)
                                                GrayAlphaQuantum,
                                                quantum_size,
                                                png_pixels,0,0);
                  }
                png_write_row(ping,png_pixels);
                if (image->previous == (Image *) NULL)
                  if (QuantumTick((magick_uint64_t) y*(pass+1),
                                  (magick_uint64_t) image->rows * num_passes))
                    if (!MagickMonitorFormatted((magick_uint64_t) y*(pass+1),
                                                (magick_uint64_t) image->rows*
                                                num_passes,
                                                &image->exception,SaveImageTag,
                                                image->filename,
					        image->columns,image->rows))
                      break;
              }
          }
        else
          for (pass=0; pass < num_passes; pass++)
            {
              if ((image_depth > 8) ||
                  (mng_info->write_png24 || mng_info->write_png32 ||
                   mng_info->write_png48 || mng_info->write_png64 ||
                   (!mng_info->write_png8 && !mng_info->IsPalette)))
                {
                  if (logging)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "  pass %d, Image Is RGB,"
                                          " PNG colortype is %s (%d)",pass,
                                          PngColorTypeToString(
                                             ping_colortype),
                                          ping_colortype);
                  for (y=0; y < (long) image->rows; y++)
                    {
                      if (!AcquireImagePixels(image,0,y,image->columns,1,
                                              &image->exception))
                        break;
                      if (ping_colortype == PNG_COLOR_TYPE_GRAY)
                        {
                          if (image->storage_class == DirectClass)
                            (void) ExportImagePixelArea(image,(QuantumType)
                                                        RedQuantum,
                                                        quantum_size,
                                                        png_pixels,0,0);
                          else
                            (void) ExportImagePixelArea(image,(QuantumType)
                                                        GrayQuantum,
                                                        quantum_size,
                                                        png_pixels,0,0);
                        }
                      else if (ping_colortype ==
                               PNG_COLOR_TYPE_GRAY_ALPHA)
                        (void) ExportImagePixelArea(image,(QuantumType)
                                                    GrayAlphaQuantum,
                                                    quantum_size,
                                                    png_pixels,0,0);
                      else if (image_matte)
                        (void) ExportImagePixelArea(image,(QuantumType)
                                                    RGBAQuantum,
                                                    quantum_size,
                                                    png_pixels,0,0);
                      else
                        {
                          (void) ExportImagePixelArea(image,(QuantumType)
                                                      RGBQuantum,
                                                      quantum_size,
                                                      png_pixels,0,0);
                        }
                      png_write_row(ping,png_pixels);
                      if (image->previous == (Image *) NULL)
                      if (QuantumTick((magick_uint64_t) y * (pass+1),
                                      (magick_uint64_t) image->rows *
                                      num_passes))
                        if (!MagickMonitorFormatted((magick_uint64_t)
                                                    y*(pass+1),
                                                    (magick_uint64_t)
                                                    image->rows*num_passes,
                                                    &image->exception,
                                                    SaveImageTag,
                                                    image->filename,
						    image->columns,image->rows))
                          break;
                    }
                  if (logging)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "  pass %d done",pass);
                }
              else
                {
                  if (logging)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "  pass %d, Image Is RGB,"
                                          " 16-bit GRAY, or GRAY_ALPHA",pass);
                  for (y=0; y < (long) image->rows; y++)
                    {
                      if (!AcquireImagePixels(image,0,y,image->columns,1,
                                              &image->exception))
                        break;
                      if (ping_colortype == PNG_COLOR_TYPE_GRAY)
                        (void) ExportImagePixelArea(image,(QuantumType)
                                                    GrayQuantum,
                                                    quantum_size,
                                                    png_pixels,0,0);
                      else if (ping_colortype ==
                               PNG_COLOR_TYPE_GRAY_ALPHA)
                        (void) ExportImagePixelArea(image,(QuantumType)
                                                    GrayAlphaQuantum,
                                                    quantum_size,
                                                    png_pixels,0,0);
                      else
                        (void) ExportImagePixelArea(image,(QuantumType)
                                                    IndexQuantum,
                                                    quantum_size,
                                                    png_pixels,0,0);
                      png_write_row(ping,png_pixels);
                      if (image->previous == (Image *) NULL)
                        if (QuantumTick((magick_uint64_t) y * (pass+1),
                                        (magick_uint64_t) image->rows *
                                        num_passes))
                          if (!MagickMonitorFormatted((magick_uint64_t)
                                                      y*(pass+1),
                                                      (magick_uint64_t)
                                                      image->rows*num_passes,
                                                      &image->exception,
                                                      SaveImageTag,
                                                      image->filename,
						      image->columns,
                                                      image->rows))
                            break;
                    }
                }
            }
      }

  if (logging)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "  Writing PNG image data");
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    Width: %lu",
                            (unsigned long)ping_width);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    Height: %lu",
                            (unsigned long)ping_height);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    PNG sample depth: %d",ping_bit_depth);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    PNG color type: %s (%d)",
                            PngColorTypeToString(ping_colortype),
                            ping_colortype);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    PNG Interlace method: %d",
                            ping_interlace_method);
    }
  /*
    Generate text chunks.
  */
  attribute=GetImageAttribute(image,(char *) NULL);
  for ( ; attribute != (const ImageAttribute *) NULL;
        attribute=attribute->next)
    {
      png_textp
        text;

      if (*attribute->key == '[')
        continue; 
      if (LocaleCompare(attribute->key,"png:IHDR.color-type-orig") == 0 ||
          LocaleCompare(attribute->key,"png:IHDR.bit-depth-orig") == 0)
        continue; 
#if PNG_LIBPNG_VER >= 14000
            text=(png_textp) png_malloc(ping,
                 (png_alloc_size_t) sizeof(png_text));
#else
            text=(png_textp) png_malloc(ping,(png_size_t) sizeof(png_text));
#endif
      text[0].key=attribute->key;
      text[0].text=attribute->value;
      text[0].text_length=strlen(attribute->value);
      text[0].compression=image_info->compression == NoCompression ||
        (image_info->compression == UndefinedCompression &&
         text[0].text_length < 128) ? -1 : 0;
      if (logging)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "  Setting up text chunk");
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "    keyword: %s",text[0].key);
        }
      png_set_text(ping,ping_info,text,1);
      png_free(ping,text);
    }
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  Writing PNG end info");
  png_write_end(ping,ping_info);
  if (mng_info->need_fram && (int) image->dispose == BackgroundDispose)
    {
      if (mng_info->page.x || mng_info->page.y || (ping_width !=
                                                   mng_info->page.width) ||
          (ping_height != mng_info->page.height))
        {
          unsigned char
            chunk[32];

          /*
            Write FRAM 4 with clipping boundaries followed by FRAM 1.
          */
          (void) WriteBlobMSBULong(image,27L);  /* data length=27 */
          PNGType(chunk,mng_FRAM);
          LogPNGChunk(logging,mng_FRAM,27L);
          chunk[4]=4;
          chunk[5]=0;  /* frame name separator (no name) */
          chunk[6]=1;  /* flag for changing delay, for next frame only */
          chunk[7]=0;  /* flag for changing frame timeout */
          chunk[8]=1;  /* flag for changing frame clipping for next frame */
          chunk[9]=0;  /* flag for changing frame sync_id */
          PNGLong(chunk+10,(png_uint_32) (0L)); /* temporary 0 delay */
          chunk[14]=0; /* clipping boundaries delta type */
          PNGLong(chunk+15,(png_uint_32) (mng_info->page.x)); /* left cb */
          PNGLong(chunk+19,(png_uint_32) (mng_info->page.x +
                                          ping_width));
          PNGLong(chunk+23,(png_uint_32) (mng_info->page.y)); /* top cb */
          PNGLong(chunk+27,(png_uint_32) (mng_info->page.y +
                                          ping_height));
          (void) WriteBlob(image,31,(char *) chunk);
          (void) WriteBlobMSBULong(image,crc32(0,chunk,31));
          mng_info->old_framing_mode=4;
          mng_info->framing_mode=1;
        }
      else
        mng_info->framing_mode=3;
    }
  if (mng_info->write_mng && !mng_info->need_fram &&
      ((int) image->dispose == 3))
    png_error(ping, "Cannot convert GIF with disposal method 3 to MNG-LC");
  image->depth=save_image_depth;

  /* Save depth actually written */

  s[0]=(char) ping_bit_depth;
  s[1]='\0';

  (void) SetImageAttribute(image,"[png:bit-depth-written]",s);

  /*
    Free PNG resources.
  */
  png_destroy_write_struct(&ping,&ping_info);

  MagickFreeMemory(png_pixels);

#if defined(GMPNG_SETJMP_NOT_THREAD_SAFE)
  UnlockSemaphoreInfo(png_semaphore);
#endif


  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  exit WriteOnePNGImage()");
  return (MagickPass);

  /* } end of setjmp-controlled block */

  /*  End write one PNG image */
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e P N G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WritePNGImage() writes a Portable Network Graphics (PNG) or
%  Multiple-image Network Graphics (MNG) image file.
%
%  MNG support written by Glenn Randers-Pehrson, glennrp@image...
%
%  The format of the WritePNGImage method is:
%
%      MagickPassFail WritePNGImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: the image info.
%
%    o image:  The image.
%
%  Returns MagickPass on success, MagickFail on failure.
%
%  While the datastream written is always in PNG format and normally
%  would be given the "png" file extension, this method also writes
%  the following pseudo-formats which are subsets of PNG:
%
%    o PNG8:  An 8-bit indexed PNG datastream is written.
%             If transparency is present, the tRNS chunk
%             must only have values 0 and 255 (i.e., transparency is binary:
%             fully opaque or fully transparent).  The pixels contain 8-bit
%             indices even if they could be represented with 1, 2, or 4 bits.
%             Note: grayscale images will be written as indexed PNG files
%             even though the PNG grayscale type might be slightly more
%             efficient.
%
%    o PNG24: An 8-bit per sample RGB PNG datastream is written.  The tRNS
%             chunk can be present to convey binary transparency by naming
%             one of the colors as transparent.
%
%    o PNG32: An 8-bit per sample RGBA PNG is written.  Partial
%             transparency is permitted, i.e., the alpha sample for
%             each pixel can have any value from 0 to 255. The alpha
%             channel is present even if the image is fully opaque. 
%
%    o PNG48:   A 16-bit per sample RGB PNG datastream is written.  The tRNS
%               chunk can be present to convey binary transparency by naming
%               one of the colors as transparent.  If the image has more
%               than one transparent color, has semitransparent pixels, or
%               has an opaque pixel with the same RGB components as the
%               transparent color, an image is not written.
%
%    o PNG64:   A 16-bit per sample RGBA PNG is written.  Partial
%               transparency is permitted, i.e., the alpha sample for
%               each pixel can have any value from 0 to 65535. The alpha
%               channel is present even if the image is fully opaque.
%
%    o PNG00:   A PNG that inherits its colortype and bit-depth from the input
%               image, if the input was a PNG, is written.  If these values
%               cannot be found, then "PNG00" falls back to the regular "PNG"
%               format.
%
%  If the image cannot be written in the requested PNG8|24|32|48|64
%  format without loss, a PNG file will not be written, and MagickFail
%  will be returned.  Since image encoders should not be responsible for
%  the "heavy lifting", the user should make sure that GraphicsMagick has
%  already reduced the image depth and number of colors and limit
%  transparency to binary transparency prior to attempting to write the
%  image in a format that is subject to depth, color, or transparency
%  limitations.
%
%  TODO: Enforce the previous paragraph.
%
%  TODO: Allow all other PNG subformats to be requested via new
%        "-define png:bit-depth -define png:color-type" options.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
static MagickPassFail WritePNGImage(const ImageInfo *image_info,Image *image)
{
  MngInfo
    *mng_info;

  int
    have_mng_structure;

  unsigned int
    logging,
    status;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"enter WritePNGImage()");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);

  /*
    Allocate a MngInfo structure.
  */
  have_mng_structure=MagickFalse;
  mng_info=MagickAllocateMemory(MngInfo *,sizeof(MngInfo));
  if (mng_info == (MngInfo *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  /*
    Initialize members of the MngInfo structure.
  */
  (void) memset(mng_info,0,sizeof(MngInfo));
  mng_info->image=image;
  have_mng_structure=MagickTrue;
  mng_info->write_png8=LocaleCompare(image_info->magick,"PNG8") == 0;
  mng_info->write_png24=LocaleCompare(image_info->magick,"PNG24") == 0;
  mng_info->write_png32=LocaleCompare(image_info->magick,"PNG32") == 0;
  mng_info->write_png48=LocaleCompare(image_info->magick,"PNG48") == 0;
  mng_info->write_png64=LocaleCompare(image_info->magick,"PNG64") == 0;

        if (LocaleCompare(image_info->magick,"png00") == 0)
          {
          const ImageAttribute
            *attribute;

          /* Retrieve png:IHDR.bit-depth-orig and png:IHDR.color-type-orig */
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
             "  Format=%s",image_info->magick);

          attribute=GetImageAttribute(image,"png:IHDR.bit-depth-orig");

          if (attribute != (ImageAttribute *) NULL)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "  png00 inherited bit depth=%s",attribute->value);

              if (LocaleCompare(attribute->value,"1") == 0)
                mng_info->write_png_depth = 1;

              else if (LocaleCompare(attribute->value,"1") == 0)
                mng_info->write_png_depth = 2;

              else if (LocaleCompare(attribute->value,"2") == 0)
                mng_info->write_png_depth = 4;

              else if (LocaleCompare(attribute->value,"8") == 0)
                mng_info->write_png_depth = 8;

              else if (LocaleCompare(attribute->value,"16") == 0)
                mng_info->write_png_depth = 16;
            }

          attribute=GetImageAttribute(image,"png:IHDR.color-type-orig");

          if (attribute != (ImageAttribute *) NULL)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "  png00 inherited color type=%s",attribute->value);

              if (LocaleCompare(attribute->value,"0") == 0)
                mng_info->write_png_colortype = 1;

              else if (LocaleCompare(attribute->value,"2") == 0)
                mng_info->write_png_colortype = 3;

              else if (LocaleCompare(attribute->value,"3") == 0)
                mng_info->write_png_colortype = 4;

              else if (LocaleCompare(attribute->value,"4") == 0)
                mng_info->write_png_colortype = 5;

              else if (LocaleCompare(attribute->value,"6") == 0)
                mng_info->write_png_colortype = 7;
            }
        }

  status=WriteOnePNGImage(mng_info,image_info,image);

  CloseBlob(image);

  MngInfoFreeStruct(mng_info,&have_mng_structure);
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"exit WritePNGImage()");
  return (status);
}

#if defined(JNG_SUPPORTED)

/* Write one JNG image */
static MagickPassFail WriteOneJNGImage(MngInfo *mng_info,
                                     const ImageInfo *image_info,Image *image)
{
  Image
    *jpeg_image;

  ImageInfo
    *jpeg_image_info;

  char
    *blob;

  size_t
    length;

  unsigned char
    chunk[80],
    *p;

  unsigned int
    jng_alpha_compression_method,
    jng_alpha_sample_depth,
    jng_color_type,
    logging,
    status,
    transparent;

  unsigned long
    jng_quality;

  logging=LogMagickEvent(CoderEvent,GetMagickModule(),
                         "  enter WriteOneJNGImage()");

  if (image->columns > 65535 || image->rows > 65535)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "  JNG dimensions too large");
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
                             image);
    }

  blob=(char *) NULL;
  jpeg_image=(Image *) NULL;
  jpeg_image_info=(ImageInfo *) NULL;

  status=MagickTrue;
  transparent=image_info->type==GrayscaleMatteType ||
    image_info->type==TrueColorMatteType;
  jng_color_type=10;
  jng_alpha_sample_depth=0;
  jng_quality=image_info->quality;
  jng_alpha_compression_method=0;

  if (image->matte)
    {
      /* if any pixels are transparent */
      transparent=MagickTrue;
      if (image_info->compression==JPEGCompression)
        jng_alpha_compression_method=8;
    }

  if (transparent)
    {
      jng_color_type=14;
      /* Create JPEG blob, image, and image_info */
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "  Creating jpeg_image_info for opacity.");
      jpeg_image_info=(ImageInfo *) CloneImageInfo(image_info);
      if (jpeg_image_info == (ImageInfo *) NULL)
        ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
                             image);
      if (logging)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                              "  Creating jpeg_image.");
      jpeg_image=CloneImage(image,0,0,MagickTrue,&image->exception);
      if (jpeg_image == (Image *) NULL)
        {
          if (jpeg_image_info != (ImageInfo *)NULL)
            {
              DestroyImageInfo(jpeg_image_info);
            }
          ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
                               image);
        }
      DestroyBlob(jpeg_image);
      jpeg_image->blob=CloneBlobInfo((BlobInfo *) NULL);
      status=ChannelImage(jpeg_image,OpacityChannel);
      if (status != MagickFalse)
        status=NegateImage(jpeg_image,MagickFalse);
      if (status == MagickFalse)
        {
          DestroyImage(jpeg_image);
          DestroyImageInfo(jpeg_image_info);
          ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
                               image);
        }
      jpeg_image->matte=MagickFalse;
      if (jng_quality >= 1000)
        jpeg_image_info->quality=jng_quality/1000;
      else
        jpeg_image_info->quality=jng_quality;
      jpeg_image_info->type=GrayscaleType;
      (void) SetImageType(jpeg_image,GrayscaleType);
      (void) AcquireUniqueFilename(jpeg_image->filename);
      FormatString(jpeg_image_info->filename,"%.1024s",
                   jpeg_image->filename);
    }

  /* To do: check bit depth of PNG alpha channel */

  /* Check if image is grayscale. */
  if (image_info->type != TrueColorMatteType && image_info->type !=
      TrueColorType && IsGrayImage(image,&image->exception))
    jng_color_type-=2;

  if (transparent)
    {
      if (jng_alpha_compression_method==0)
        {
          const ImageAttribute
            *attribute;

          /* Encode opacity as a grayscale PNG blob */
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "  Creating PNG blob.");
          status=OpenBlob(jpeg_image_info,jpeg_image,WriteBinaryBlobMode,
                          &image->exception);
          if (status != MagickFalse)
            {
              length=0;

              (void) strlcpy(jpeg_image_info->magick,"PNG",MaxTextExtent);
              (void) strlcpy(jpeg_image->magick,"PNG",MaxTextExtent);
              jpeg_image_info->interlace=NoInterlace;

              blob=(char *) ImageToBlob(jpeg_image_info,jpeg_image,&length,
                                        &image->exception);

              /* Retrieve sample depth used */
              attribute=GetImageAttribute(jpeg_image,"[png:bit-depth-written]");
              if ((attribute != (const ImageAttribute *) NULL) &&
                  (attribute->value != (char *) NULL))
                jng_alpha_sample_depth= (unsigned int) attribute->value[0];
            }
        }
      else
        {
          /* Encode opacity as a grayscale JPEG blob */

          status=OpenBlob(jpeg_image_info,jpeg_image,WriteBinaryBlobMode,
                          &image->exception);
          if (status != MagickFalse)
            {
              (void) strlcpy(jpeg_image_info->magick,"JPEG",MaxTextExtent);
              (void) strlcpy(jpeg_image->magick,"JPEG",MaxTextExtent);
              jpeg_image_info->interlace=NoInterlace;
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "  Creating blob.");
              blob=(char *) ImageToBlob(jpeg_image_info,jpeg_image,&length,
                                        &image->exception);
              jng_alpha_sample_depth=8;
              if (logging)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "  Successfully read jpeg_image into a"
                                      " blob, length=%lu.",
                                      (unsigned long) length);

            }
        }
      /* Destroy JPEG image and image_info */
      (void) LiberateUniqueFileResource(jpeg_image_info->filename);
      DestroyImage(jpeg_image);
      DestroyImageInfo(jpeg_image_info);
      if (status == MagickFalse)
        ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
                             image);
    }

  /* Write JHDR chunk */
  (void) WriteBlobMSBULong(image,16L);  /* chunk data length=16 */
  PNGType(chunk,mng_JHDR);
  LogPNGChunk(logging,mng_JHDR,16L);
  PNGLong(chunk+4,image->columns);
  PNGLong(chunk+8,image->rows);
  chunk[12]=jng_color_type;
  chunk[13]=8;  /* sample depth */
  chunk[14]=8; /*jng_image_compression_method */
  chunk[15]=(image_info->interlace == LineInterlace) ? 8 : 0;
  chunk[16]=jng_alpha_sample_depth;
  chunk[17]=jng_alpha_compression_method;
  chunk[18]=0; /*jng_alpha_filter_method */
  chunk[19]=0; /*jng_alpha_interlace_method */
  (void) WriteBlob(image,20,(char *) chunk);
  (void) WriteBlobMSBULong(image,crc32(0,chunk,20));
  if (logging)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    JNG width:%15lu",image->columns);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    JNG height:%14lu",image->rows);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    JNG color type:%10d",jng_color_type);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    JNG sample depth:%8d",8);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    JNG compression:%9d",8);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    JNG interlace:%11d",0);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    JNG alpha depth:%9d",jng_alpha_sample_depth);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    JNG alpha compression:%3d",
                            jng_alpha_compression_method);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    JNG alpha filter:%8d",0);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    JNG alpha interlace:%5d",0);
    }

  /*
    Write leading ancillary chunks
  */

  if (transparent)
    {
      /*
        Write JNG bKGD chunk
      */

      char
        blue,
        green,
        red;

      long
        num_bytes;

      if (jng_color_type == 8 || jng_color_type == 12)
        num_bytes=6L;
      else
        num_bytes=10L;
      (void) WriteBlobMSBULong(image,num_bytes-4L);
      PNGType(chunk,mng_bKGD);
      LogPNGChunk(logging,mng_bKGD,num_bytes-4L);
      red=ScaleQuantumToChar(image->background_color.red);
      green=ScaleQuantumToChar(image->background_color.green);
      blue=ScaleQuantumToChar(image->background_color.blue);
      *(chunk+4)=0;
      *(chunk+5)=red;
      *(chunk+6)=0;
      *(chunk+7)=green;
      *(chunk+8)=0;
      *(chunk+9)=blue;
      (void) WriteBlob(image,num_bytes,(char *) chunk);
      (void) WriteBlobMSBULong(image,crc32(0,chunk,num_bytes));
    }

  if ((image_info->colorspace == sRGBColorspace || image->rendering_intent))
    {
      /*
        Write JNG sRGB chunk
      */
      (void) WriteBlobMSBULong(image,1L);
      PNGType(chunk,mng_sRGB);
      LogPNGChunk(logging,mng_sRGB,1L);
      if (image->rendering_intent != UndefinedIntent)
        chunk[4]=(int) image->rendering_intent-1;
      else
        chunk[4]=(int) PerceptualIntent-1;
      (void) WriteBlob(image,5,(char *) chunk);
      (void) WriteBlobMSBULong(image,crc32(0,chunk,5));
    }
  else
    {
      if (image->gamma)
        {
          /*
            Write JNG gAMA chunk
          */
          (void) WriteBlobMSBULong(image,4L);
          PNGType(chunk,mng_gAMA);
          LogPNGChunk(logging,mng_gAMA,4L);
          PNGLong(chunk+4,(unsigned long) (100000*image->gamma+0.5));
          (void) WriteBlob(image,8,(char *) chunk);
          (void) WriteBlobMSBULong(image,crc32(0,chunk,8));
        }
      if (!mng_info->equal_chrms && image->chromaticity.red_primary.x != 0.0)
        {
          PrimaryInfo
            primary;

          /*
            Write JNG cHRM chunk
          */
          (void) WriteBlobMSBULong(image,32L);
          PNGType(chunk,mng_cHRM);
          LogPNGChunk(logging,mng_cHRM,32L);
          primary=image->chromaticity.white_point;
          PNGLong(chunk+4,(unsigned long) (100000*primary.x+0.5));
          PNGLong(chunk+8,(unsigned long) (100000*primary.y+0.5));
          primary=image->chromaticity.red_primary;
          PNGLong(chunk+12,(unsigned long) (100000*primary.x+0.5));
          PNGLong(chunk+16,(unsigned long) (100000*primary.y+0.5));
          primary=image->chromaticity.green_primary;
          PNGLong(chunk+20,(unsigned long) (100000*primary.x+0.5));
          PNGLong(chunk+24,(unsigned long) (100000*primary.y+0.5));
          primary=image->chromaticity.blue_primary;
          PNGLong(chunk+28,(unsigned long) (100000*primary.x+0.5));
          PNGLong(chunk+32,(unsigned long) (100000*primary.y+0.5));
          (void) WriteBlob(image,36,(char *) chunk);
          (void) WriteBlobMSBULong(image,crc32(0,chunk,36));
        }
    }
  if (image->x_resolution && image->y_resolution && !mng_info->equal_physs)
    {
      /*
        Write JNG pHYs chunk
      */
      (void) WriteBlobMSBULong(image,9L);
      PNGType(chunk,mng_pHYs);
      LogPNGChunk(logging,mng_pHYs,9L);
      if (image->units == PixelsPerInchResolution)
        {
          PNGLong(chunk+4,(unsigned long)
                  (image->x_resolution*100.0/2.54+0.5));
          PNGLong(chunk+8,(unsigned long)
                  (image->y_resolution*100.0/2.54+0.5));
          chunk[12]=1;
        }
      else
        {
          if (image->units == PixelsPerCentimeterResolution)
            {
              PNGLong(chunk+4,(unsigned long)
                      (image->x_resolution*100.0+0.5));
              PNGLong(chunk+8,(unsigned long)
                      (image->y_resolution*100.0+0.5));
              chunk[12]=1;
            }
          else
            {
              PNGLong(chunk+4,(unsigned long) (image->x_resolution+0.5));
              PNGLong(chunk+8,(unsigned long) (image->y_resolution+0.5));
              chunk[12]=0;
            }
        }
      (void) WriteBlob(image,13,(char *) chunk);
      (void) WriteBlobMSBULong(image,crc32(0,chunk,13));
    }

  if (mng_info->write_mng == 0 && (image->page.x || image->page.y))
    {
      /*
        Write JNG oFFs chunk
      */
      (void) WriteBlobMSBULong(image,9L);
      PNGType(chunk,mng_oFFs);
      LogPNGChunk(logging,mng_oFFs,9L);
      PNGsLong(chunk+4,(long) (image->page.x));
      PNGsLong(chunk+8,(long) (image->page.y));
      chunk[12]=0;
      (void) WriteBlob(image,13,(char *) chunk);
      (void) WriteBlobMSBULong(image,crc32(0,chunk,13));
    }

  if (transparent)
    {
      if (jng_alpha_compression_method==0)
        {
          unsigned long
            len;

          register long
            i;

          /* Write IDAT chunk header */
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "  Write IDAT chunks from blob, length=%lu.",
                                  (unsigned long) length);

          /* Copy IDAT chunks */
          len=0;
          p=(unsigned char *) (blob+8);
          for (i=8; i<(long) length; i+=len+12)
            {
              len=((*(p    ) & 0xff) << 24) +
                  ((*(p + 1) & 0xff) << 16) +
                  ((*(p + 2) & 0xff) <<  8) +
                  ((*(p + 3) & 0xff)      ) ;
              p+=4;
              if (*(p)==73 && *(p+1)==68 && *(p+2)==65 && *(p+3)==84)
                {
                  /* Found an IDAT chunk. */
                  (void) WriteBlobMSBULong(image,(unsigned long) len);
                  LogPNGChunk(logging,mng_IDAT,len);
                  (void) WriteBlob(image,len+4,(char *) p);
                  (void) WriteBlobMSBULong(image,
                                           crc32(0,p,len+4));
                }
              else
                {
                  if (logging)
                    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                          "    Skipping %c%c%c%c chunk,"
                                          " length=%lu.",
                                          *(p),*(p+1),*(p+2),*(p+3),len);
                }
              p+=(8+len);
            }
        }
      else
        {
          /* Write JDAA chunk header */
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "  Write JDAA chunk, length=%lu.",
                                  (unsigned long) length);
          (void) WriteBlobMSBULong(image,(unsigned long) length);
          PNGType(chunk,mng_JDAA);
          LogPNGChunk(logging,mng_JDAA,(unsigned long) length);
          /* Write JDAT chunk(s) data */
          (void) WriteBlob(image,4,(char *) chunk);
          (void) WriteBlob(image,length,(char *) blob);
          (void) WriteBlobMSBULong(image,crc32(crc32(0,chunk,4),
                                               (unsigned char *) blob,(uInt) length));
        }
      MagickFreeMemory(blob);
    }

  /* Encode image as a JPEG blob */
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  Creating jpeg_image_info.");
  jpeg_image_info=(ImageInfo *) CloneImageInfo(image_info);
  if (jpeg_image_info == (ImageInfo *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
                         image);

  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  Creating jpeg_image.");

  jpeg_image=CloneImage(image,0,0,MagickTrue,&image->exception);
  if (jpeg_image == (Image *) NULL)
    {
      DestroyImageInfo(jpeg_image_info);
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
                           image);
    }
  DestroyBlob(jpeg_image);
  jpeg_image->blob=CloneBlobInfo((BlobInfo *) NULL);
  (void) AcquireUniqueFilename(jpeg_image->filename);
  FormatString(jpeg_image_info->filename,"%.1024s",jpeg_image->filename);

  status=OpenBlob(jpeg_image_info,jpeg_image,WriteBinaryBlobMode,
                  &image->exception);
  if (status == MagickFalse)
    {
      if (jpeg_image != (Image *)NULL)
        DestroyImage(jpeg_image);
      if (jpeg_image_info != (ImageInfo *)NULL)
        DestroyImageInfo(jpeg_image_info);
      ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,
                           image);
    }

  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  Created jpeg_image, %lu x %lu.",
                          jpeg_image->columns,
                          jpeg_image->rows);

  if (jng_color_type == 8 || jng_color_type == 12)
    jpeg_image_info->type=GrayscaleType;
  jpeg_image_info->quality=jng_quality%1000;
  (void) strlcpy(jpeg_image_info->magick,"JPEG",MaxTextExtent);
  (void) strlcpy(jpeg_image->magick,"JPEG",MaxTextExtent);
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  Creating blob.");
  blob=(char *) ImageToBlob(jpeg_image_info,jpeg_image,&length,
                            &image->exception);
  if (logging)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "  Successfully read jpeg_image into a blob,"
                            " length=%lu.",
                            (unsigned long) length);

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "  Write JDAT chunk, length=%lu.",
                            (unsigned long) length);
    }
  /* Write JDAT chunk(s) */
  (void) WriteBlobMSBULong(image,(unsigned long) length);
  PNGType(chunk,mng_JDAT);
  LogPNGChunk(logging,mng_JDAT,(unsigned long) length);
  (void) WriteBlob(image,4,(char *) chunk);
  (void) WriteBlob(image,length,(char *) blob);
  (void) WriteBlobMSBULong(image,crc32(crc32(0,chunk,4),(unsigned char *)
                                       blob,(uInt)length));

  (void) LiberateUniqueFileResource(jpeg_image_info->filename);
  DestroyImage(jpeg_image);
  DestroyImageInfo(jpeg_image_info);
  MagickFreeMemory(blob);

  /* Write IEND chunk */
  (void) WriteBlobMSBULong(image,0L);
  PNGType(chunk,mng_IEND);
  LogPNGChunk(logging,mng_IEND,0);
  (void) WriteBlob(image,4,(char *) chunk);
  (void) WriteBlobMSBULong(image,crc32(0,chunk,4));

  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "  exit WriteOneJNGImage()");
  return(status);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e J N G I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteJNGImage() writes a JPEG Network Graphics (JNG) image file.
%
%  JNG support written by Glenn Randers-Pehrson, glennrp@image...
%
%  The format of the WriteJNGImage method is:
%
%      MagickPassFail WriteJNGImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows:
%
%    o image_info: the image info.
%
%    o image:  The image.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/
static MagickPassFail WriteJNGImage(const ImageInfo *image_info,Image *image)
{
  MngInfo
    *mng_info;

  int
    have_mng_structure;

  unsigned int
    logging,
    status;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"enter WriteJNGImage()");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);

  /*
    Allocate a MngInfo structure.
  */
  have_mng_structure=MagickFalse;
  mng_info=MagickAllocateMemory(MngInfo *,sizeof(MngInfo));
  if (mng_info == (MngInfo *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  /*
    Initialize members of the MngInfo structure.
  */
  (void) memset(mng_info,0,sizeof(MngInfo));
  mng_info->image=image;
  have_mng_structure=MagickTrue;

  (void) WriteBlob(image,8,"\213JNG\r\n\032\n");

  status=WriteOneJNGImage(mng_info,image_info,image);
  CloseBlob(image);

  (void) CatchImageException(image);
  MngInfoFreeStruct(mng_info,&have_mng_structure);
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"exit WriteJNGImage()");
  return (status);
}
#endif



static unsigned int WriteMNGImage(const ImageInfo *image_info,Image *image)
{
  Image
    *next_image;

  MngInfo
    *mng_info;

  int
    have_mng_structure,
    image_count,
    need_iterations,
    need_matte;

  volatile int
#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) ||  \
  defined(PNG_MNG_FEATURES_SUPPORTED)
    need_local_plte,
#endif
    all_images_are_gray,
    logging,
    need_defi,
    build_palette,
    use_global_plte;

  register long
    i;

#if 1
  size_t
    chunk_length=0;
#endif

  unsigned int
    status;

  volatile unsigned int
    write_jng,
    write_mng;

  volatile unsigned long
    scene;

  unsigned long
    final_delay=0,
    initial_delay;

#if (PNG_LIBPNG_VER < 10200)
  if (image_info->verbose)
    printf("Your PNG library (libpng-%s) is rather old.\n",
           PNG_LIBPNG_VER_STRING);
#endif

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"enter WriteMNGImage()");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);

  /*
    Allocate a MngInfo structure.
  */
  have_mng_structure=MagickFalse;
  mng_info=MagickAllocateMemory(MngInfo *,sizeof(MngInfo));
  if (mng_info == (MngInfo *) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  /*
    Initialize members of the MngInfo structure.
  */
  (void) memset(mng_info,0,sizeof(MngInfo));
  mng_info->image=image;
  have_mng_structure=MagickTrue;

  write_mng=LocaleCompare(image_info->magick,"MNG") == 0;
  mng_info->write_png8=LocaleCompare(image_info->magick,"PNG8") == 0;
  mng_info->write_png24=LocaleCompare(image_info->magick,"PNG24") == 0;
  mng_info->write_png32=LocaleCompare(image_info->magick,"PNG32") == 0;
  mng_info->write_png48=LocaleCompare(image_info->magick,"PNG48") == 0;
  mng_info->write_png64=LocaleCompare(image_info->magick,"PNG64") == 0;

  write_jng=MagickFalse;
  if (image_info->compression==JPEGCompression)
    write_jng=MagickTrue;
  else
    {
      Image
        *p;

      for (p=image; p != (Image *) NULL; p=p->next)
        {
          if (p->compression==JPEGCompression)
            write_jng=MagickTrue;
        }
    }

  mng_info->adjoin=image_info->adjoin && (image->next != (Image *) NULL) &&
    write_mng;

#ifdef GMPNG_BUILD_PALETTE
  if (mng_info->write_png8)
    build_palette=MagickTrue;
  else if (mng_info->write_png24 || mng_info->write_png32 ||
      mng_info->write_png48 || mng_info->write_png64)
    build_palette=MagickFalse;
  else
    build_palette=(image_info->type == UndefinedType);
#endif

  if (logging)
    {
      /* Log some info about the input */
      Image
        *p;

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "  Checking input image(s)");

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    Image_info depth: %ld",
                            image_info->depth);
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                            "    Type: %d",image_info->type);

      scene=0;
      for (p=image; p != (Image *) NULL; p=p->next)
        {
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "    Scene: %ld",scene++);
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                "      Image depth: %u",p->depth);
          if (p->matte)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "      Matte: True");
          else
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "      Matte: False");
          if (p->storage_class == PseudoClass)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "      Storage class: PseudoClass");
          else
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "      Storage class: DirectClass");
          if (p->colors)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "      Number of colors: %u",p->colors);
          else
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "      Number of colors: unspecified");
          if (!mng_info->adjoin)
            break;
        }
    }

  /*
    Sometimes we get PseudoClass images whose RGB values don't match
    the colors in the colormap.  This code syncs the RGB values.
  */
  {
    Image
      *p;

    for (p=image; p != (Image *) NULL; p=p->next)
      {
        if (p->taint && p->storage_class == PseudoClass)
          (void) SyncImage(p);
        if (!mng_info->adjoin)
          break;
      }
  }

#ifdef GMPNG_BUILD_PALETTE
  if (build_palette)
    {
      /*
        Sometimes we get DirectClass images that have 256 colors or fewer.
        This code will convert them to PseudoClass and build a colormap.
      */
      Image
        *p;

      for (p=image; p != (Image *) NULL; p=p->next)
        {
          if (p->storage_class != PseudoClass)
            {
              p->colors=GetNumberColors(p,(FILE *) NULL,&p->exception);
              if (p->colors <= 256)
                {
                  p->colors=0;
                  if (p->matte)
                    (void) SetImageType(p,PaletteMatteType);
                  else
                    (void) SetImageType(p,PaletteType);
                }
            }
          if (!mng_info->adjoin)
            break;
        }
    }
#endif

  use_global_plte=MagickFalse;
  all_images_are_gray=MagickFalse;
#ifdef PNG_WRITE_EMPTY_PLTE_SUPPORTED
  need_local_plte=MagickTrue;
#endif
  need_defi=MagickFalse;
  need_matte=MagickFalse;
  mng_info->framing_mode=1;
  mng_info->old_framing_mode=1;

  if (write_mng)
    if (image_info->page != (char *) NULL)
      {
        /*
          Determine image bounding box.
        */
        SetGeometry(image,&mng_info->page);
        (void) ParseMetaGeometry(image_info->page,&mng_info->page.x,
                                 &mng_info->page.y,&mng_info->page.width,
                                 &mng_info->page.height);
      }
  if (write_mng)
    {
      unsigned char
        chunk[800];

      unsigned int
        need_geom;

      unsigned short
        red,
        green,
        blue;

      mng_info->page=image->page;
      need_geom=MagickTrue;
      if (mng_info->page.width || mng_info->page.height)
        need_geom=MagickFalse;
      /*
        Check all the scenes.
      */
      initial_delay=(long) image->delay;
      need_iterations=MagickFalse;
      mng_info->equal_chrms=image->chromaticity.red_primary.x != 0.0;
      mng_info->equal_physs=MagickTrue,
        mng_info->equal_gammas=MagickTrue;
      mng_info->equal_srgbs=MagickTrue;
      mng_info->equal_backgrounds=MagickTrue;
      image_count=0;
#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) ||  \
  defined(PNG_MNG_FEATURES_SUPPORTED)
      all_images_are_gray=MagickTrue;
      mng_info->equal_palettes=MagickFalse;
      need_local_plte=MagickFalse;
#endif
      for (next_image=image; next_image != (Image *) NULL; )
        {
          if (need_geom)
            {
              if ((next_image->columns+next_image->page.x) >
                   mng_info->page.width)
                mng_info->page.width=next_image->columns+next_image->page.x;
              if ((next_image->rows+next_image->page.y) >
                   mng_info->page.height)
                mng_info->page.height=next_image->rows+next_image->page.y;
            }
          if (next_image->page.x || next_image->page.y)
            need_defi=MagickTrue;
          if (next_image->matte)
            need_matte=MagickTrue;
          if ((int) next_image->dispose >= BackgroundDispose)
            if (next_image->matte || next_image->page.x ||
                next_image->page.y ||
                ((next_image->columns < mng_info->page.width) &&
                 (next_image->rows < mng_info->page.height)))
              mng_info->need_fram=MagickTrue;
          if (next_image->iterations)
            need_iterations=MagickTrue;
          final_delay=next_image->delay;
          if (final_delay != initial_delay || final_delay > 100)
            mng_info->need_fram=1;
#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) ||  \
  defined(PNG_MNG_FEATURES_SUPPORTED)
          /*
            check for global palette possibility.
          */
          if (image->matte)
            need_local_plte=MagickTrue;
          if (!need_local_plte)
            {
              if (!IsGrayImage(image,&image->exception))
                all_images_are_gray=MagickFalse;
              mng_info->equal_palettes=PalettesAreEqual(image,next_image);
              if (!use_global_plte)
                use_global_plte=mng_info->equal_palettes;
              need_local_plte=!mng_info->equal_palettes;
            }
#endif
          if (next_image->next != (Image *) NULL)
            {
              if (next_image->background_color.red !=
                  next_image->next->background_color.red ||
                  next_image->background_color.green !=
                  next_image->next->background_color.green ||
                  next_image->background_color.blue !=
                  next_image->next->background_color.blue)
                mng_info->equal_backgrounds=MagickFalse;
              if (next_image->gamma != next_image->next->gamma)
                mng_info->equal_gammas=MagickFalse;
              if (next_image->rendering_intent !=
                  next_image->next->rendering_intent)
                mng_info->equal_srgbs=MagickFalse;
              if ((next_image->units != next_image->next->units) ||
                  (next_image->x_resolution !=
                  next_image->next->x_resolution) ||
                  (next_image->y_resolution != next_image->next->y_resolution))
                mng_info->equal_physs=MagickFalse;
              if (mng_info->equal_chrms)
                {
                  if (next_image->chromaticity.red_primary.x !=
                      next_image->next->chromaticity.red_primary.x ||
                      next_image->chromaticity.red_primary.y !=
                      next_image->next->chromaticity.red_primary.y ||
                      next_image->chromaticity.green_primary.x !=
                      next_image->next->chromaticity.green_primary.x ||
                      next_image->chromaticity.green_primary.y !=
                      next_image->next->chromaticity.green_primary.y ||
                      next_image->chromaticity.blue_primary.x !=
                      next_image->next->chromaticity.blue_primary.x ||
                      next_image->chromaticity.blue_primary.y !=
                      next_image->next->chromaticity.blue_primary.y ||
                      next_image->chromaticity.white_point.x !=
                      next_image->next->chromaticity.white_point.x ||
                      next_image->chromaticity.white_point.y !=
                      next_image->next->chromaticity.white_point.y)
                    mng_info->equal_chrms=MagickFalse;
                }
            }
          image_count++;
          next_image=next_image->next;
        }
      if (image_count < 2)
        {
          mng_info->equal_backgrounds=MagickFalse;
          mng_info->equal_chrms=MagickFalse;
          mng_info->equal_gammas=MagickFalse;
          mng_info->equal_srgbs=MagickFalse;
          mng_info->equal_physs=MagickFalse;
          use_global_plte=MagickFalse;
#ifdef PNG_WRITE_EMPTY_PLTE_SUPPORTED
          need_local_plte=MagickTrue;
#endif
          need_iterations=MagickFalse;
        }
      if (!mng_info->need_fram)
        {
          /*
            Only certain framing rates 100/n are exactly representable without
            the FRAM chunk but we'll allow some slop in VLC files
          */
          if (final_delay == 0)
            {
              if (need_iterations)
                {
                  /*
                    It's probably a GIF with loop; don't run it *too* fast.
                  */
                  final_delay=10;
                  (void) ThrowException2(&image->exception,CoderError,
                                         "input has zero delay between"
                                         " all frames; assuming 10 cs",
                                         (char *) NULL);
                }
              else
                mng_info->ticks_per_second=0;
            }
          if (final_delay)
            mng_info->ticks_per_second=100/final_delay;
          if (final_delay > 50)
            mng_info->ticks_per_second=2;
          if (final_delay > 75)
            mng_info->ticks_per_second=1;
          if (final_delay > 125)
            mng_info->need_fram=MagickTrue;
          if (need_defi && final_delay > 2 && (final_delay != 4) &&
              (final_delay != 5) && (final_delay != 10) &&
              (final_delay != 20) &&
              (final_delay != 25) && (final_delay != 50) &&
              (final_delay != 100))
            mng_info->need_fram=MagickTrue;  /* make it exact;
                                          cannot be VLC anyway */
        }
      if (mng_info->need_fram)
        mng_info->ticks_per_second=100;
      /*
        If pseudocolor, we should also check to see if all the
        palettes are identical and write a global PLTE if they are.
        ../glennrp Feb 99.
      */
      /*
        Write the MNG version 1.0 signature and MHDR chunk.
      */
      (void) WriteBlob(image,8,"\212MNG\r\n\032\n");
      (void) WriteBlobMSBULong(image,28L);  /* chunk data length=28 */
      PNGType(chunk,mng_MHDR);
      LogPNGChunk(logging,mng_MHDR,28L);
      PNGLong(chunk+4,mng_info->page.width);
      PNGLong(chunk+8,mng_info->page.height);
      PNGLong(chunk+12,mng_info->ticks_per_second);
      PNGLong(chunk+16,0L);  /* layer count=unknown */
      PNGLong(chunk+20,0L);  /* frame count=unknown */
      PNGLong(chunk+24,0L);  /* play time=unknown   */
      if (write_jng)
        {
          if (need_matte)
            {
              if (need_defi || mng_info->need_fram || use_global_plte)
                PNGLong(chunk+28,27L);  /* simplicity=LC+JNG */
              else
                PNGLong(chunk+28,25L);  /* simplicity=VLC+JNG */
            }
          else
            {
              if (need_defi || mng_info->need_fram || use_global_plte)
                PNGLong(chunk+28,19L);  /* simplicity=LC+JNG,
                                           no transparency */
              else
                PNGLong(chunk+28,17L);  /* simplicity=VLC+JNG,
                                           no transparency */
            }
        }
      else
        {
          if (need_matte)
            {
              if (need_defi || mng_info->need_fram || use_global_plte)
                PNGLong(chunk+28,11L);    /* simplicity=LC */
              else
                PNGLong(chunk+28,9L);    /* simplicity=VLC */
            }
          else
            {
              if (need_defi || mng_info->need_fram || use_global_plte)
                PNGLong(chunk+28,3L);    /* simplicity=LC, no transparency */
              else
                PNGLong(chunk+28,1L);    /* simplicity=VLC, no transparency */
            }
        }
      (void) WriteBlob(image,32,(char *) chunk);
      (void) WriteBlobMSBULong(image,crc32(0,chunk,32));
#if 1
      if (AccessDefinition(image_info,"mng","need-cacheoff"))
        {
          /*
            Add a "nEED CACHEOFF" request to disable frame caching in libmng.
            Unfortunately, standard conformant players will reject the stream
            if they do not support a nEED request.
          */
          PNGType(chunk,mng_nEED);
          chunk_length = (strlcpy((char *) chunk+4, "CACHEOFF",20));
          (void) WriteBlobMSBULong(image,(const unsigned long) chunk_length);
          LogPNGChunk(logging,mng_nEED,(unsigned long) chunk_length);
          chunk_length += 4;
          (void) WriteBlob(image,chunk_length,(char *) chunk);
          (void) WriteBlobMSBULong(image,crc32(0,chunk,(uInt) chunk_length));
        }
#endif
      if ((image->previous == (Image *) NULL) &&
          (image->next != (Image *) NULL) && (image->iterations != 1))
        {
          /*
            Write MNG TERM chunk
          */
          (void) WriteBlobMSBULong(image,10L);  /* data length=10 */
          PNGType(chunk,mng_TERM);
          LogPNGChunk(logging,mng_TERM,10L);
          chunk[4]=3;  /* repeat animation */
          chunk[5]=0;  /* show last frame when done */
          PNGLong(chunk+6,(png_uint_32) (mng_info->ticks_per_second*
                                         final_delay/100));
          if (image->iterations == 0)
            PNGLong(chunk+10,PNG_MAX_UINT);
          else
            PNGLong(chunk+10,(png_uint_32) image->iterations);
          if (logging)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                    "     TERM delay: %lu",
                                    (unsigned long)
                                    (mng_info->ticks_per_second*
                                    final_delay/100));
              if (image->iterations == 0)
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "     TERM iterations: %lu",
                                      (unsigned long)PNG_MAX_UINT);
              else
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                      "     Image iterations: %lu",
                                      image->iterations);
            }
          (void) WriteBlob(image,14,(char *) chunk);
          (void) WriteBlobMSBULong(image,crc32(0,chunk,14));
        }
      /*
        To do: check for cHRM+gAMA == sRGB, and write sRGB instead.
      */
      if ((image_info->colorspace == sRGBColorspace ||
           image->rendering_intent) && mng_info->equal_srgbs)
        {
          /*
            Write MNG sRGB chunk
          */
          (void) WriteBlobMSBULong(image,1L);
          PNGType(chunk,mng_sRGB);
          LogPNGChunk(logging,mng_sRGB,1L);
          if (image->rendering_intent != UndefinedIntent)
            chunk[4]=(int) image->rendering_intent-1;
          else
            chunk[4]=(int) PerceptualIntent-1;
          (void) WriteBlob(image,5,(char *) chunk);
          (void) WriteBlobMSBULong(image,crc32(0,chunk,5));
          mng_info->have_write_global_srgb=MagickTrue;
        }
      else
        {
          if (image->gamma && mng_info->equal_gammas)
            {
              /*
                Write MNG gAMA chunk
              */
              (void) WriteBlobMSBULong(image,4L);
              PNGType(chunk,mng_gAMA);
              LogPNGChunk(logging,mng_gAMA,4L);
              PNGLong(chunk+4,(unsigned long) (100000*image->gamma+0.5));
              (void) WriteBlob(image,8,(char *) chunk);
              (void) WriteBlobMSBULong(image,crc32(0,chunk,8));
              mng_info->have_write_global_gama=MagickTrue;
            }
          if (mng_info->equal_chrms)
            {
              PrimaryInfo
                primary;

              /*
                Write MNG cHRM chunk
              */
              (void) WriteBlobMSBULong(image,32L);
              PNGType(chunk,mng_cHRM);
              LogPNGChunk(logging,mng_cHRM,32L);
              primary=image->chromaticity.white_point;
              PNGLong(chunk+4,(unsigned long) (100000*primary.x+0.5));
              PNGLong(chunk+8,(unsigned long) (100000*primary.y+0.5));
              primary=image->chromaticity.red_primary;
              PNGLong(chunk+12,(unsigned long) (100000*primary.x+0.5));
              PNGLong(chunk+16,(unsigned long) (100000*primary.y+0.5));
              primary=image->chromaticity.green_primary;
              PNGLong(chunk+20,(unsigned long) (100000*primary.x+0.5));
              PNGLong(chunk+24,(unsigned long) (100000*primary.y+0.5));
              primary=image->chromaticity.blue_primary;
              PNGLong(chunk+28,(unsigned long) (100000*primary.x+0.5));
              PNGLong(chunk+32,(unsigned long) (100000*primary.y+0.5));
              (void) WriteBlob(image,36,(char *) chunk);
              (void) WriteBlobMSBULong(image,crc32(0,chunk,36));
              mng_info->have_write_global_chrm=MagickTrue;
            }
        }
      if (image->x_resolution && image->y_resolution && mng_info->equal_physs)
        {
          /*
            Write MNG pHYs chunk
          */
          (void) WriteBlobMSBULong(image,9L);
          PNGType(chunk,mng_pHYs);
          LogPNGChunk(logging,mng_pHYs,9L);
          if (image->units == PixelsPerInchResolution)
            {
              PNGLong(chunk+4,(unsigned long)
                      (image->x_resolution*100.0/2.54+0.5));
              PNGLong(chunk+8,(unsigned long)
                      (image->y_resolution*100.0/2.54+0.5));
              chunk[12]=1;
            }
          else
            {
              if (image->units == PixelsPerCentimeterResolution)
                {
                  PNGLong(chunk+4,(unsigned long)
                          (image->x_resolution*100.0+0.5));
                  PNGLong(chunk+8,(unsigned long)
                          (image->y_resolution*100.0+0.5));
                  chunk[12]=1;
                }
              else
                {
                  PNGLong(chunk+4,(unsigned long) (image->x_resolution+0.5));
                  PNGLong(chunk+8,(unsigned long) (image->y_resolution+0.5));
                  chunk[12]=0;
                }
            }
          (void) WriteBlob(image,13,(char *) chunk);
          (void) WriteBlobMSBULong(image,crc32(0,chunk,13));
        }
      /*
        Write MNG BACK chunk and global bKGD chunk, if the image is transparent
        or does not cover the entire frame.
      */
      if (write_mng && (image->matte || image->page.x > 0 ||
                        image->page.y > 0 ||
                        (image->page.width &&
                        (image->page.width+image->page.x <
                        mng_info->page.width))
                        || (image->page.height &&
                        (image->page.height+image->page.y <
                        mng_info->page.height))))
        {
          (void) WriteBlobMSBULong(image,6L);
          PNGType(chunk,mng_BACK);
          LogPNGChunk(logging,mng_BACK,6L);
          red=ScaleQuantumToShort(image->background_color.red);
          green=ScaleQuantumToShort(image->background_color.green);
          blue=ScaleQuantumToShort(image->background_color.blue);
          PNGShort(chunk+4,red);
          PNGShort(chunk+6,green);
          PNGShort(chunk+8,blue);
          (void) WriteBlob(image,10,(char *) chunk);
          (void) WriteBlobMSBULong(image,crc32(0,chunk,10));
          if (mng_info->equal_backgrounds)
            {
              (void) WriteBlobMSBULong(image,6L);
              PNGType(chunk,mng_bKGD);
              LogPNGChunk(logging,mng_bKGD,6L);
              (void) WriteBlob(image,10,(char *) chunk);
              (void) WriteBlobMSBULong(image,crc32(0,chunk,10));
            }
        }

#ifdef PNG_WRITE_EMPTY_PLTE_SUPPORTED
      if (!need_local_plte && image->storage_class == PseudoClass
          && !all_images_are_gray)
        {
          unsigned long
            data_length;

          /*
            Write MNG PLTE chunk
          */
          data_length=3*image->colors;
          (void) WriteBlobMSBULong(image,data_length);
          PNGType(chunk,mng_PLTE);
          LogPNGChunk(logging,mng_PLTE,data_length);
          for (i=0; i < (long) image->colors; i++)
            {
              chunk[4+i*3]=ScaleQuantumToChar(image->colormap[i].red) & 0xff;
              chunk[5+i*3]=ScaleQuantumToChar(image->colormap[i].green) & 0xff;
              chunk[6+i*3]=ScaleQuantumToChar(image->colormap[i].blue) & 0xff;
            }
          (void) WriteBlob(image,data_length+4,(char *) chunk);
          (void) WriteBlobMSBULong(image,crc32(0,chunk,(int) (data_length+4)));
          mng_info->have_write_global_plte=MagickTrue;
        }
#endif
    }
  scene=0;
  mng_info->delay=0;
#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) ||  \
  defined(PNG_MNG_FEATURES_SUPPORTED)
  mng_info->equal_palettes=MagickFalse;
#endif
  do
    {
      unsigned char
        chunk[800];

      if (mng_info->adjoin)
        {
#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) ||  \
  defined(PNG_MNG_FEATURES_SUPPORTED)
          /*
            If we aren't using a global palette for the entire MNG, check to
            see if we can use one for two or more consecutive images.
          */
          if (need_local_plte && use_global_plte && !all_images_are_gray)
            {
              if (mng_info->IsPalette)
                {
                  /*
                    When equal_palettes is true, this image has the
                    same palette as the previous PseudoClass image
                  */
                  mng_info->have_write_global_plte=mng_info->equal_palettes;
                  mng_info->equal_palettes=PalettesAreEqual(image,image->next);
                  if (mng_info->equal_palettes &&
                      !mng_info->have_write_global_plte)
                    {
                      /*
                        Write MNG PLTE chunk
                      */
                      unsigned long
                        data_length;

                      data_length=3*image->colors;
                      (void) WriteBlobMSBULong(image,data_length);
                      PNGType(chunk,mng_PLTE);
                      LogPNGChunk(logging,mng_PLTE,data_length);
                      for (i=0; i < (long) image->colors; i++)
                        {
                          chunk[4+i*3]=
                             ScaleQuantumToChar(image->colormap[i].red);
                          chunk[5+i*3]=
                             ScaleQuantumToChar(image->colormap[i].green);
                          chunk[6+i*3]=
                             ScaleQuantumToChar(image->colormap[i].blue);
                        }
                      (void) WriteBlob(image,data_length+4,(char *) chunk);
                      (void) WriteBlobMSBULong(image,
                                               crc32(0,chunk,(int)
                                               (data_length+4)));
                      mng_info->have_write_global_plte=MagickTrue;
                    }
                }
              else
                mng_info->have_write_global_plte=MagickFalse;
            }
#endif
          if (need_defi)
            {
              long
                previous_x,
                previous_y;

              if (scene)
                {
                  previous_x=mng_info->page.x;
                  previous_y=mng_info->page.y;
                }
              else
                {
                  previous_x=0;
                  previous_y=0;
                }
              mng_info->page=image->page;
              if ((mng_info->page.x !=  previous_x) || (mng_info->page.y !=
                                                        previous_y))
                {
                  (void) WriteBlobMSBULong(image,12L);  /* data length=12 */
                  PNGType(chunk,mng_DEFI);
                  LogPNGChunk(logging,mng_DEFI,12L);
                  chunk[4]=0; /* object 0 MSB */
                  chunk[5]=0; /* object 0 LSB */
                  chunk[6]=0; /* visible  */
                  chunk[7]=0; /* abstract */
                  PNGLong(chunk+8,mng_info->page.x);
                  PNGLong(chunk+12,mng_info->page.y);
                  (void) WriteBlob(image,16,(char *) chunk);
                  (void) WriteBlobMSBULong(image,crc32(0,chunk,16));
                }
            }
        }

      mng_info->write_mng=write_mng;

      if ((int) image->dispose >= 3)
        mng_info->framing_mode=3;

      if (mng_info->need_fram && mng_info->adjoin &&
          ((image->delay != mng_info->delay) ||
           (mng_info->framing_mode != mng_info->old_framing_mode)))
        {
          if (image->delay == mng_info->delay)
            {
              /*
                Write a MNG FRAM chunk with the new framing mode.
              */
              (void) WriteBlobMSBULong(image,1L);  /* data length=1 */
              PNGType(chunk,mng_FRAM);
              LogPNGChunk(logging,mng_FRAM,1L);
              chunk[4]=(unsigned char) mng_info->framing_mode;
              (void) WriteBlob(image,5,(char *) chunk);
              (void) WriteBlobMSBULong(image,crc32(0,chunk,5));
            }
          else
            {
              /*
                Write a MNG FRAM chunk with the delay.
              */
              (void) WriteBlobMSBULong(image,10L);  /* data length=10 */
              PNGType(chunk,mng_FRAM);
              LogPNGChunk(logging,mng_FRAM,10L);
              chunk[4]=(unsigned char) mng_info->framing_mode;
              chunk[5]=0;  /* frame name separator (no name) */
              chunk[6]=2;  /* flag for changing default delay */
              chunk[7]=0;  /* flag for changing frame timeout */
              chunk[8]=0;  /* flag for changing frame clipping */
              chunk[9]=0;  /* flag for changing frame sync_id */
              PNGLong(chunk+10,(png_uint_32)
                      ((mng_info->ticks_per_second*image->delay)/100));
              (void) WriteBlob(image,14,(char *) chunk);
              (void) WriteBlobMSBULong(image,crc32(0,chunk,14));
              mng_info->delay=(long) image->delay;
            }
          mng_info->old_framing_mode=mng_info->framing_mode;
        }

#if defined(JNG_SUPPORTED)
      if (image->compression == JPEGCompression)
        {
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "  Writing JNG object.");
          /* To do: specify the desired alpha compression method. */
          image->compression=UndefinedCompression;
          status=WriteOneJNGImage(mng_info,image_info,image);
        }
      else
#endif
        {
          if (logging)
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                                  "  Writing PNG object.");
          status=WriteOnePNGImage(mng_info,image_info,image);
        }

      if (!status)
        {
          MngInfoFreeStruct(mng_info,&have_mng_structure);
          CloseBlob(image);
          return (MagickFail);
        }
      (void) CatchImageException(image);
      if (image->next == (Image *) NULL)
        break;
      image=SyncNextImageInList(image);
      if (QuantumTick(scene,GetImageListLength(image)))
        if (!MagickMonitorFormatted(scene++,GetImageListLength(image),
                                    &image->exception,SaveImagesTag,
                                    image->filename))
          break;
    } while (mng_info->adjoin);
  if (write_mng)
    {
      unsigned char
        chunk[16];

      while (image->previous != (Image *) NULL)
        image=image->previous;
      /*
        Write the MEND chunk.
      */
      (void) WriteBlobMSBULong(image,0x00000000L);
      PNGType(chunk,mng_MEND);
      LogPNGChunk(logging,mng_MEND,0L);
      (void) WriteBlob(image,4,(char *) chunk);
      (void) WriteBlobMSBULong(image,crc32(0,chunk,4));
    }
  /*
    Free memory.
  */
  CloseBlob(image);
  MngInfoFreeStruct(mng_info,&have_mng_structure);
  if (logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"exit WriteMNGImage()");
  return(MagickPass);
}
#else /* PNG_LIBPNG_VER > 10011 */
static unsigned int WritePNGImage(const ImageInfo *image_info,Image *image)
{
  image=image;
  printf("Your PNG library is too old: You have libpng-%s\n",
         PNG_LIBPNG_VER_STRING);
  ThrowBinaryException(CoderError,PNGLibraryTooOld,image_info->filename);
}
static unsigned int WriteMNGImage(const ImageInfo *image_info,Image *image)
{
  return (WritePNGImage(image_info,image));
}
#endif /* PNG_LIBPNG_VER > 10011 */
#endif
