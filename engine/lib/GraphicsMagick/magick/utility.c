/*
% Copyright (C) 2003 - 2016 GraphicsMagick Group
% Copyright (c) 2000 Markus Friedl.  All rights reserved.
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
%             U   U  TTTTT  IIIII  L      IIIII  TTTTT  Y   Y                 %
%             U   U    T      I    L        I      T     Y Y                  %
%             U   U    T      I    L        I      T      Y                   %
%             U   U    T      I    L        I      T      Y                   %
%              UUU     T    IIIII  LLLLL  IIIII    T      Y                   %
%                                                                             %
%                                                                             %
%                     GraphicsMagick Utility Methods                          %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                               John Cristy                                   %
%                              January 1993                                   %
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
#include "magick/blob.h"
#include "magick/color.h"
#include "magick/confirm_access.h"
#include "magick/enum_strings.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/pixel_cache.h"
#include "magick/random.h"
#include "magick/signature.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

#if defined(HAVE_MACH_O_DYLD_H)
/* Needed for _NSGetExecutablePath */
# include <mach-o/dyld.h>
#endif

#if defined(HAVE_SPAWNVP) && defined(HAVE_PROCESS_H)
#  include <process.h>
#endif

/*
  Compute a value which is the next kilobyte power of 2 larger than
  the requested value or MaxTextExtent, whichever is larger.

  The objective is to round up the size quickly (and in repeatable
  steps) in order to reduce the number of memory copies due to realloc
  for strings which grow rapidly, while producing a reasonable size
  for smaller strings.
*/
#define MagickRoundUpStringLength(size) \
{ \
  size_t \
    _rounded; \
 \
  for (_rounded=256U; _rounded < (Max(size,256)); _rounded *= 2); \
  size=_rounded; \
}

/*
  Static declarations.
*/
static const char
  Base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const unsigned char
  AsciiMap[] =
  {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
    0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
    0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
    0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73,
    0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b,
    0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81, 0x82, 0x83,
    0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b,
    0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3,
    0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xe1, 0xe2, 0xe3, 0xe4, 0xc5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb,
    0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3,
    0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb,
    0xfc, 0xfd, 0xfe, 0xff,
  };

/*
  Forward declaration.
*/
static int
  IsDirectory(const char *);

static int
  MagickStrToD(const char *start,char **end,double *value);

static MagickPassFail
  MagickStrToInt64(const char *start,char **end,magick_int64_t *value);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A c q u i r e S t r i n g                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AcquireString() allocates memory for a string and copies the source string
%  to that memory location (and returns it). This method is best used to
%  allocate constant strings since only enough memory to support the data
%  is allocated.
%
%  The format of the AcquireString method is:
%
%      char *AcquireString(const char *source)
%
%  A description of each parameter follows:
%
%    o allocated_string:  Method AcquireString returns a copy of the source
%      string.
%
%    o source: A character string.
%
%
*/
MagickExport char *AcquireString(const char *source)
{
  char
    *destination;

  size_t
    length;

  assert(source != (const char *) NULL);
  length=strlen(source);
  destination=MagickAllocateMemory(char *,length+1);
  if (destination == (char *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateString);
  if (length != 0)
    (void) memcpy(destination,source,length);
  destination[length]='\0';
  return(destination);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A l l o c a t e S t r i n g                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AllocateString() allocates memory for a string and copies the source string
%  to that memory location (and returns it). Additional memory is allocated
%  so that subsequent concatenations to the string are most efficient.
%
%  The format of the AllocateString method is:
%
%      char *AllocateString(const char *source)
%
%  A description of each parameter follows:
%
%    o allocated_string:  Method AllocateString returns a copy of the source
%      string.
%
%    o source: A character string.
%
%
*/
MagickExport char *AllocateString(const char *source)
{
  char
    *destination;

  size_t
    allocation_length,
    source_length;

  allocation_length=MaxTextExtent;
  source_length=0;
  if (source != (char *) NULL)
    {
      source_length=strlen(source);
      allocation_length=source_length+1;
      MagickRoundUpStringLength(allocation_length);
    }
  destination=MagickAllocateMemory(char *,allocation_length);
  if (destination == (char *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateString);
  if (source_length != 0)
    (void) memcpy(destination,source,source_length);
  destination[source_length]='\0';
  return(destination);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  A p p e n d I m a g e F o r m a t                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AppendImageFormat() appends the image format type to the filename.  If an
%  extension to the file already exists, it is first removed.
%
%  The format of the AppendImageFormat method is:
%
%      void AppendImageFormat(const char *format,char *filename)
%
%  A description of each parameter follows.
%
%   o  format:  Specifies a pointer to an array of characters.  This is the
%      format of the image.
%
%   o  filename:  Specifies a pointer to an array of characters.  The unique
%      file name is returned in this array.
%
%
*/
MagickExport void AppendImageFormat(const char *format,char *filename)
{
  char
    root[MaxTextExtent];

  assert(format != (char *) NULL);
  assert(filename != (char *) NULL);
  if ((*format == '\0') || (*filename == '\0'))
    return;
  if (LocaleCompare(filename,"-") == 0)
    {
      char
        message[MaxTextExtent];

      FormatString(message,"%.1024s:%.1024s",format,filename);
      (void) strlcpy(filename,message,MaxTextExtent);
      return;
    }
  GetPathComponent(filename,RootPath,root);
  FormatString(filename,"%.1024s.%.1024s",root,format);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B a s e 6 4 D e c o d e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Base64Decode() decodes Base64-encoded text and returns its binary
%  equivalent.  NULL is returned if the text is not valid base64 data, or a
%  memory allocation failure occurs.
%
%  Contributed by Bob Friesenhahn.
%
%  The format of the Base64Decode method is:
%
%      unsigned char *Base64Decode(const char *source,length_t *length)
%
%  A description of each parameter follows:
%
%    o source:  A pointer to a Base64-encoded string.
%
%    o length: The number of bytes decoded.
%
*/
MagickExport unsigned char *Base64Decode(const char *source,size_t *length)
{
  int
    state;

  register const char
    *p,
    *q;

  register size_t
    i;

  size_t
    max_length;

  unsigned char
    *decode;

  assert(source != (char *) NULL);
  assert(length != (size_t *) NULL);
  *length=0;
  max_length=3*strlen(source)/4+1;
  decode=MagickAllocateMemory(unsigned char *,max_length);
  if (decode == (unsigned char *) NULL)
    return((unsigned char *) NULL);
  i=0;
  state=0;
  for (p=source; *p != '\0'; p++)
  {
    if (isspace((int)(unsigned char) *p))
      continue;
    if (*p == '=')
      break;
    q=strchr(Base64,*p);
    if (q == (char *) NULL)
      {
        MagickFreeMemory(decode);
        return((unsigned char *) NULL);  /* non-base64 character */
      }
    switch (state)
    {
      case 0:
      {
        decode[i]=(q-Base64) << 2;
        state++;
        break;
      }
      case 1:
      {
        decode[i++]|=(q-Base64) >> 4;
        decode[i]=((q-Base64) & 0x0f) << 4;
        state++;
        break;
      }
      case 2:
      {
        decode[i++]|=(q-Base64) >> 2;
        decode[i]=((q-Base64) & 0x03) << 6;
        state++;
        break;
      }
      case 3:
      {
        decode[i++]|=(q-Base64);
        state=0;
        break;
      }
    }
  }
  /*
    Verify Base-64 string has proper terminal characters.
  */
  if (*p != '=')
    {
      if (state != 0)
        {
          MagickFreeMemory(decode);
          return((unsigned char *) NULL);
        }
    }
  else
    {
      p++;
      switch (state)
      {
        case 0:
        case 1:
        {
          /*
            Unrecognized '=' character.
          */
          MagickFreeMemory(decode);
          return((unsigned char *) NULL);
        }
        case 2:
        {
          for ( ; *p != '\0'; p++)
            if (!isspace((int)(unsigned char)*p))
              break;
          if (*p != '=')
            {
              MagickFreeMemory(decode);
              return((unsigned char *) NULL);
            }
          p++;
          break;
        }
        case 3:
        {
          for ( ; *p != '\0'; p++)
            if (!isspace((int)(unsigned char) *p))
              {
                MagickFreeMemory(decode);
                return((unsigned char *) NULL);
              }
          if (decode[i] != 0)
            {
              MagickFreeMemory(decode);
              return((unsigned char *) NULL);
            }
          break;
        }
      }
    }
  *length=i;
  assert(*length < max_length);
  return(decode);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B a s e 6 4 E n c o d e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Base64Encode() encodes arbitrary binary data to base64 encoded format as
%  described by the "Base64 Content-Transfer-Encoding" section of RFC 2045 and
%  returns the result as a null-terminated ASCII string.  NULL is returned if
%  a memory allocation failure occurs.
%
%  Contributed by Bob Friesenhahn.
%
%  The format of the Base64Encode method is:
%
%      char *Base64Encode(const unsigned char *blob,const size_t blob_length,
%        size_t *encode_length)
%
%  A description of each parameter follows:
%
%    o blob:           A pointer to binary data to encode.
%
%    o blob_length:    The number of bytes to encode.
%
%    o encode_length:  The number of bytes encoded.
%
*/
#define Index64(index) ((index) & 0x3f)
MagickExport char *Base64Encode(const unsigned char *blob,
  const size_t blob_length,size_t *encode_length)
{
  char
    *encode;

  register const unsigned char
    *p;

  register size_t
    i;

  size_t
    max_length,
    remaining;

  assert(blob != (const unsigned char *) NULL);
  assert(blob_length != 0);
  assert(encode_length != (size_t *) NULL);
  *encode_length=0;
  max_length=MagickArraySize(4U,blob_length)/3U;
  if (max_length)
    max_length += 4U;
  encode=MagickAllocateMemory(char *,max_length);
  if (encode == (char *) NULL)
    return((char *) NULL);
  i=0;
  for (p=blob; p < (blob+blob_length-2); p+=3)
  {
    encode[i++]=Base64[Index64(*p >> 2)];
    encode[i++]=Base64[Index64(((*p & 0x03) << 4)+(*(p+1) >> 4))];
    encode[i++]=Base64[Index64(((*(p+1) & 0x0f) << 2)+(*(p+2) >> 6))];
    encode[i++]=Base64[Index64(*(p+2))];
  }
  remaining=blob_length % 3;
  if (remaining != 0)
    {
      long
        j;

      unsigned char
        code[3];

      code[0]='\0';
      code[1]='\0';
      code[2]='\0';
      for (j=0; j < (long) remaining; j++)
        code[j]=(*p++);
      encode[i++]=Base64[Index64(code[0] >> 2)];
      encode[i++]=Base64[Index64(((code[0] & 0x03) << 4)+(code[1] >> 4))];
      if (remaining == 1)
        encode[i++]='=';
      else
        encode[i++]=Base64[Index64(((code[1] & 0x0f) << 2)+(code[2] >> 6))];
      encode[i++]='=';
    }
  *encode_length=i;
  encode[i++]='\0';
  assert(i <= max_length);
  return(encode);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e S t r i n g                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CloneString allocates memory for the destination string and copies
%  the source string to that memory location.
%
%  The format of the CloneString method is:
%
%      MagickPassFail CloneString(char **destination,const char *source)
%
%  A description of each parameter follows:
%
%    o status:  Method CloneString returns MagickPass if the string is cloned,
%      otherwise MagickFail is returned.
%
%    o destination:  A pointer to a character string.
%
%    o source: A character string.
%
%
*/
MagickExport MagickPassFail CloneString(char **destination,const char *source)
{
  size_t
    allocation_length,
    string_length;

  assert(destination != (char **) NULL);
  if (source == (const char *) NULL)
    {
      MagickFreeMemory(*destination);
      return(MagickPass);
    }
  string_length=strlen(source);
  allocation_length=string_length+1;
  MagickRoundUpStringLength(allocation_length);
  MagickReallocMemory(char *,*destination,allocation_length);
  if (*destination == (char *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateString);
  if (0 != string_length)
    (void) memcpy(*destination,source,string_length);
  (*destination)[string_length]='\0';
  return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n c a t e n a t e S t r i n g                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ConcatenateString appends a copy of string source, including
%  the terminating null character, to the end of string destination.
%
%  The format of the ConcatenateString method is:
%
%      MagickPassFail ConcatenateString(char **destination,const char *source)
%
%  A description of each parameter follows:
%
%    o status:  Method ConcatenateString returns MagickPass if the string is cloned,
%      otherwise MagickFail is returned.
%
%    o destination:  A pointer to a character string.
%
%    o source: A character string.
%
%
*/
MagickExport MagickPassFail ConcatenateString(char **destination,
  const char *source)
{
  size_t
    allocation_length,
    destination_length=0,
    source_length;

  assert(destination != (char **) NULL);
  if (source == (const char *) NULL)
    return(MagickPass);
  source_length=strlen(source);
  if (*destination != (char *) NULL)
    destination_length=strlen(*destination);
  allocation_length=destination_length+source_length+1;
  MagickRoundUpStringLength(allocation_length);
  MagickReallocMemory(char *,(*destination),allocation_length);
  if (*destination == (char *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToConcatenateString);
  if (0 != source_length)
    (void) memcpy(&(*destination)[destination_length],source,source_length);
  (*destination)[destination_length+source_length]='\0';
  return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e f i n e C l i e n t N a m e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DefineClientName() this is a helper function that parses the passed string
%    in order to define the name of the client application.
%
%  The format of the DefineClientName method is:
%
%      void DefineClientName(const char *path)
%
%  A description of each parameter follows:
%
%    o path: A string that can also be a full path that contains the name of
%            application
%
*/
MagickExport void DefineClientName(const char *path)
{
  if ((path != (char *) NULL) && (*path != '\0'))
    {
      char
        component[MaxTextExtent];

      GetPathComponent(path,BasePath,component);
      (void) SetClientName(component);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e f i n e C l i e n t P a t h A n d N a m e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DefineClientPathAndName() this is a helper function that parses the passed
%  string in order to define several global settings related to the location of
%  the application. It sets the path, the filename, and the display name of the
%  client application based on the input string which is assumed to be the full
%  and valid path to the client.
%
%  The format of the DefineClientPathAndName method is:
%
%      void DefineClientPathAndName(const char *path)
%
%  A description of each parameter follows:
%
%    o path: A string that must be a full path that contains the name of
%            application
%
*/
MagickExport void DefineClientPathAndName(const char *path)
{
  if ((path != (char *) NULL) && (*path != '\0'))
    {
      char
        component[MaxTextExtent];

      /* This is the path only - inluding the parent folder */
      GetPathComponent(path,HeadPath,component);
      (void) SetClientPath(component);
      /* This is the file name AND the extension - of present */
      GetPathComponent(path,TailPath,component);
      (void) SetClientFilename(component);
      /* The last step is to define a human readable name for
         the help and error logging systems. */
      DefineClientName(component);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E s c a p e S t r i n g                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EscapeString() allocates memory for a backslash-escaped version of a
%  source text string, copies the escaped version of the text to that
%  memory location while adding backslash characters, and returns the
%  escaped string.
%
%  The format of the EscapeString method is:
%
%      char *EscapeString(const char *source,const char escape)
%
%  A description of each parameter follows:
%
%    o allocated_string:  Method EscapeString returns the escaped string.
%
%    o source: A character string.
%
%    o escape: The quoted string termination character to escape (e.g. '"').
%
*/
MagickExport char *EscapeString(const char *source,const char escape)
{
  char
    *destination;

  register char
    *q;

  register const char
    *p;

  size_t
    length;

  assert(source != (const char *) NULL);

  length=strlen(source)+1;
  for (p=source; *p; p++)
    if ((*p == '\\') || (*p == escape))
      length++;
  destination=MagickAllocateMemory(char *,length);
  if (destination == (char *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToEscapeString);
  *destination='\0';
  q=destination;
  for (p=source; *p; p++)
    {
      if ((*p == '\\') || (*p == escape))
        *q++= '\\';
      *q++=(*p);
    }
  *q=0;
  return(destination);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x p a n d F i l e n a m e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ExpandFilename expands '~' in a filename.
%
%  The format of the ExpandFilename function is:
%
%      ExpandFilename(char *filename)
%
%  A description of each parameter follows:
%
%    o filename: Specifies a pointer to a character array that contains the
%      filename.
%
%
*/
MagickExport void ExpandFilename(char *filename)
{
  char
    expanded_filename[MaxTextExtent];

  register char
    *p;

  if (filename == (char *) NULL)
    return;
  if (*filename != '~')
    return;
  (void) strlcpy(expanded_filename,filename,MaxTextExtent);
  if (*(filename+1) == '/')
    {
      /*
        Substitute ~ with $HOME.
      */
      p=(char *) getenv("HOME");
      if (p == (char *) NULL)
        p=(char *) ".";
      (void) strlcpy(expanded_filename,p,MaxTextExtent);
      (void) strlcat(expanded_filename,filename+1,MaxTextExtent);
    }
  else
    {
#if defined(POSIX)
      char
        username[MaxTextExtent];

      struct passwd
        *entry;

      /*
        Substitute ~ with home directory from password file.
      */
      (void) strlcpy(username,filename+1,MaxTextExtent);
      p=strchr(username,'/');
      if (p != (char *) NULL)
        *p='\0';
      entry=getpwnam(username);
      if (entry == (struct passwd *) NULL)
        return;
      (void) strncpy(expanded_filename,entry->pw_dir,MaxTextExtent-1);
      if (p != (char *) NULL)
        {
          (void) strcat(expanded_filename,"/");
          (void) strlcat(expanded_filename,p+1,MaxTextExtent);
        }
#endif
    }
  (void) strlcpy(filename,expanded_filename,MaxTextExtent);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   E x p a n d F i l e n a m e s                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ExpandFilenames checks each argument of the command line vector and
%  expands it if they have a wildcard character.  For example, *.jpg might
%  expand to:  bird.jpg rose.jpg tiki.jpg.
%
%  The format of the ExpandFilenames function is:
%
%      status=ExpandFilenames(int *argc,char ***argv)
%
%  A description of each parameter follows:
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.  The existing argument list is replaced.
%
%
*/
MagickExport MagickPassFail ExpandFilenames(int *argc,char ***argv)
{
  const
    size_t prealloc_entries = 1024;

  char
    current_directory[MaxTextExtent],
    *option,
    **vector;

  long
    count,
    number_files;

  register long
    i,
    j;

  MagickBool
    first;

  /*
    Allocate argument vector.
  */
  assert(argc != (int *) NULL);
  assert(argv != (char ***) NULL);
  vector=MagickAllocateMemory(char **,(*argc+prealloc_entries)*sizeof(char *));
  if (vector == (char **) NULL)
    return(MagickFail);
  /*
    Expand any wildcard filenames.
  */
  current_directory[0]='\0';
  count=0;
  for (i=0; i < *argc; i++)
    {
      char
	**filelist,
	filename[MaxTextExtent],
	magick[MaxTextExtent],
	path[MaxTextExtent],
	subimage[MaxTextExtent];

      option=(*argv)[i];
      /* Never throw options away, so copy here, then perhaps modify later */
      vector[count++]=AcquireString(option);

      /*
	Don't expand or process any VID: argument since the VID coder
	does its own expansion
      */
      if (LocaleNCompare("VID:",option,4) == 0)
	continue;

      /*
	Don't attempt to expand the argument to these options.
      */
      if ((LocaleNCompare("+define",option,7) == 0) ||
	  (LocaleNCompare("+profile",option,8) == 0) ||
	  (LocaleNCompare("-comment",option,8) == 0) ||
	  (LocaleNCompare("-convolve",option,9) == 0) ||
	  (LocaleNCompare("-draw",option,5) == 0) ||
	  (LocaleNCompare("-font",option,5) == 0) ||
	  (LocaleNCompare("-format",option,7) == 0) ||
	  (LocaleNCompare("-label",option,6) == 0))
	{
	  i++;
	  if (i == *argc)
            continue;
	  option=(*argv)[i];
	  vector[count++]=AcquireString(option);
	  continue;
	}

      /* Pass quotes through to the command-line parser */
      if ((*option == '"') || (*option == '\''))
	continue;

      /*
	Expand @filename to a list of arguments.
      */
      j=0;
      if (option[0] == '@')
	{
	  FILE
	    *file;

	  file=fopen(option+1,"r");
	  if (file != (FILE *) NULL)
	    {

	      first=MagickTrue;
	      number_files=0;
	      while (fgets(filename,sizeof(filename),file) != (char *) NULL)
		{
		  for (j=0; filename[j] != '\0'; j++)
		    if (filename[j] == '\n')
		      filename[j] = '\0';
	      
		  if (filename[0] != '\0')
		    {
		      if ((number_files % prealloc_entries) == 0)
			{
			  MagickReallocMemory(char **,vector,
					      (*argc+count+prealloc_entries)*
					      sizeof(char *));
			  if (vector == (char **) NULL)
			    {
			      fclose(file);
			      return(MagickFail);
			    }
			}

		      if (first)
			{
			  /* Deallocate original option assigned above */
			  --count;
			  MagickFreeMemory(vector[count]);
			  first=MagickFalse;
			}
		      number_files++;
		      vector[count++]=AcquireString(filename);
		    }
		}

	      fclose(file);
	    }
	}

      /* 
	 Fast cycle options that are not expandable filename patterns.
	 ListFiles only expands patterns in the filename.  We also check
	 if the full option resolves to a file since ListFiles() obtains
	 a list of all the files in the directory and is thus very slow
	 if there are thousands of files.
      */
      GetPathComponent(option,TailPath,filename);
      if ((!IsGlob(filename)) || IsAccessibleNoLogging(option))
        continue;

      /* Chop the option to get its other filename components. */
      GetPathComponent(option,MagickPath,magick);
      GetPathComponent(option,HeadPath,path);
      GetPathComponent(option,SubImagePath,subimage);

      /* GetPathComponent throws away the colon */
      if (*magick != '\0')
	(void) strlcat(magick,":",sizeof(magick));
      ExpandFilename(path);

      if ('\0' == current_directory[0])
	if (getcwd(current_directory,MaxTextExtent-1) == NULL)
          MagickFatalError(ConfigureFatalError,UnableToGetCurrentDirectory,
                           NULL);

      /* Get the list of matching file names. */
      filelist=ListFiles(*path=='\0' ? current_directory : path,
			 filename,&number_files);

      if (filelist != (char **) NULL)
	for (j=0; j < number_files; j++)
	  if (IsDirectory(filelist[j]) <= 0)
	    break;

      /* ListFiles() may change current directory without restoring. */
      if ((strlen(current_directory) > 0) && (chdir(current_directory) != 0))
        {
          for (j=0; j < number_files; j++)
	    MagickFreeMemory(filelist[j]);
          MagickFreeMemory(filelist);
          MagickFatalError(ConfigureFatalError,UnableToRestoreCurrentDirectory,
                           NULL);
        }

      if (filelist == 0)
	continue;

      if (j == number_files)
	{
	  /*
	    Bourne/Bash shells passes through unchanged any glob patterns
	    not matching anything (abc* and there's no file starting with
	    abc). Do the same for behaviour consistent with that.
	  */
	  for (j=0; j < number_files; j++)
	    MagickFreeMemory(filelist[j]);
	  MagickFreeMemory(filelist);
	  continue;
	}

      /*
	There's at least one matching filename.
	Transfer file list to argument vector.
      */
      MagickReallocMemory(char **,vector,
			  (*argc+count+number_files+prealloc_entries)*sizeof(char *));
      if (vector == (char **) NULL)
	return(MagickFail);

      first=MagickTrue;
      for (j=0; j < number_files; j++)
	{
	  char
	    filename_buffer[MaxTextExtent];

	  *filename_buffer='\0';
	  if (strlcat(filename_buffer,path,sizeof(filename_buffer))
	      >= sizeof(filename_buffer))
	    MagickFatalError2(ResourceLimitFatalError,"Path buffer overflow",
			      filename_buffer);
	  if (*path != '\0')
	    {
	      if (strlcat(filename_buffer,DirectorySeparator,sizeof(filename_buffer))
		  >= sizeof(filename_buffer))
		MagickFatalError2(ResourceLimitFatalError,"Path buffer overflow",
				  filename_buffer);
	    }
	  if (strlcat(filename_buffer,filelist[j],sizeof(filename_buffer))
	      >= sizeof(filename_buffer))
	    MagickFatalError2(ResourceLimitFatalError,"Path buffer overflow",
			      filename_buffer);
	  /* If it's a filename (not a directory) ... */
	  if (IsDirectory(filename_buffer) == 0) 
	    {
	      char
		formatted_buffer[MaxTextExtent];

	      *formatted_buffer='\0';
	      if (strlcat(formatted_buffer,magick,sizeof(formatted_buffer))
		  >= sizeof(formatted_buffer))
		MagickFatalError2(ResourceLimitFatalError,"Path buffer overflow",
				  formatted_buffer);
	      if (strlcat(formatted_buffer,filename_buffer,sizeof(formatted_buffer))
		  >= sizeof(formatted_buffer))
		MagickFatalError2(ResourceLimitFatalError,"Path buffer overflow",
				  formatted_buffer);
	      if (strlcat(formatted_buffer,subimage,sizeof(formatted_buffer))
		  >= sizeof(formatted_buffer))
		MagickFatalError2(ResourceLimitFatalError,"Path buffer overflow",
				  formatted_buffer);
	      if (first)
		{
		  /* Deallocate original option assigned above */
		  --count;
		  MagickFreeMemory(vector[count]);
		  first=MagickFalse;
		}
	      vector[count++]=AcquireString(formatted_buffer);
	    }
	  MagickFreeMemory(filelist[j]);
	}
      MagickFreeMemory(filelist);
    }
  *argc=count;
  *argv=vector;
  return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  F o r m a t S i z e                                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  FormatSize() converts a size to a human readable format, for example,
%  14kb, 234mb, 2.7gb, or 3.0tb.  Scaling is done by repetitively dividing by
%  1024.
%
%  The format of the FormatSize method is:
%
%      char *FormatSize(const magick_int64_t size,char *format)
%
%  A description of each parameter follows:
%
%    o size:  convert this size to a human readable format.
%
%    o format:  human readable format.
%
%
*/
MagickExport void FormatSize(const magick_int64_t size,char *format)
{
  double
    length;

  register unsigned int
    i;

  length=size;
  for (i=0; length > 1024; i++)
    length/=1024.0;
  if (i > 0)
    FormatString(format,"%.1f",length);
  else
    FormatString(format,"%.0f",length);
  switch (i)
    {
    default: break;
    case 0: break;
    case 1: (void) strcat(format,"Ki"); break; /* kilo, 10^3 */
    case 2: (void) strcat(format,"Mi"); break; /* mega, 10^6 */
    case 3: (void) strcat(format,"Gi"); break; /* giga, 10^9 */
    case 4: (void) strcat(format,"Ti"); break; /* tera, 10^12 */
    case 5: (void) strcat(format,"Pi"); break; /* peta, 10^15 */
    case 6: (void) strcat(format,"Ei"); break; /* exa,  10^18 */
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  F o r m a t S t r i n g                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method FormatString prints formatted output of a variable argument list.
%
%  The format of the FormatString method is:
%
%      void FormatString(char *string,const char *format,...)
%
%  A description of each parameter follows.
%
%   o  string:  Method FormatString returns the formatted string in this
%      character buffer. Buffer must be at least MaxTextExtent characters
%      in size.
%
%   o  format:  A string describing the format to use to write the remaining
%      arguments.
%
%
*/
MagickExport void FormatStringList(char *string,const char *format, 
                                   va_list operands)
{
#if defined(HAVE_VSNPRINTF)
  (void) vsnprintf(string,MaxTextExtent,format,operands);
#else
  (void) vsprintf(string,format,operands);
#endif
}
MagickExport void FormatString(char *string,const char *format,...)
{
  va_list
    operands;

  va_start(operands,format);
  FormatStringList(string, format, operands);
  va_end(operands);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t E x e c u t i o n P a t h                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetExecutionPath() returns the pathname of the executable that started
%  the process. The user-supplied buffer must be at least MaxTextExtent
%  bytes long.  On success True is returned, otherwise False.
%
%  The format of the GetExecutionPath method is:
%
%      unsigned int GetExecutionPath(char *path)
%
%  A description of each parameter follows:
%
%    o path: The pathname of the executable that started the process.
%
*/
MagickExport MagickPassFail GetExecutionPath(char *path)
{
  *path='\0';
#if defined(MSWINDOWS)
  /*
    Microsoft Windows provides an means to obtain the path to the
    currently executing executable or DLL.
  */
  return(NTGetExecutionPath(path));
#endif
#if defined(HAVE_GETEXECNAME)
  {
    /*
      Sun's Solaris provides a getexecname() for obtaining the path to
      this executable.
    */
    const char
      *execution_path;

    execution_path=(const char *) getexecname();
    if (execution_path != (const char *) NULL)
      {
        if (*execution_path != *DirectorySeparator)
          {
            if (getcwd(path,MaxTextExtent-1) == NULL)
              MagickFatalError(ConfigureFatalError,UnableToGetCurrentDirectory,
                               NULL);
            (void) strlcat(path,"/",MaxTextExtent);
          }
        (void) strlcat(path,execution_path,MaxTextExtent);
        if (IsAccessible(path))
          return(MagickPass);
      }
  }
#endif
#if defined(HAVE__NSGETEXECUTABLEPATH) && defined(HAVE_REALPATH)
  {
    /*
      Apple OS-X provides a _NSGetExecutablePath function for
      obtaining the path to the executable.  The returned path may be
      a symbolic link to the actual executable so use realpath() to
      resolve the real path.  It is claimed that _NSGetExecutablePath
      could possibly return a path longer than MAXPATHLEN.  Inspection
      of <sys/param.h> on an OS-X system reveals that MAXPATHLEN is
      defined to be PATH_MAX. So we will provide for PATH_MAX*2.
    */
    uint32_t
      bufsize;

    char
      executable_path[PATH_MAX*2],
      real_path[PATH_MAX+1];
    
    bufsize=sizeof(executable_path);
    if (_NSGetExecutablePath(executable_path,&bufsize) == 0)
      if (realpath(executable_path,real_path) != NULL)
        if (strlcpy(path,real_path,MaxTextExtent) < MaxTextExtent)
          if (IsAccessible(path))
            return(MagickPass);
  }
#endif
#if defined(HAVE_GETPID) && defined(HAVE_READLINK)
  {
    /*
      On Linux, the symbolic link "/proc/PID/exe" (where 'PID' is the
      integer process ID) points to the physical executable.  FreeBSD
      is similar except that the link path is "/proc/PID/file".  Maybe
      some other systems use compatible schemes.
    */
    int
      length;

    long
      pid;

    char 
      link_path[MaxTextExtent],
      real_path[PATH_MAX+1];

    pid=(long) getpid();
    /* Linux format */
    FormatString(link_path,"/proc/%ld/exe",pid);
    length=readlink(link_path, real_path, PATH_MAX);
    if (length == -1)
      {
        /* Try FreeBSD format */
        FormatString(link_path,"/proc/%ld/file",pid);
        length=readlink(link_path, real_path, PATH_MAX);
      }
    if ((length > 0) && (length <= PATH_MAX))
      {
        real_path[length]=0;
        if (strlcpy(path,real_path,MaxTextExtent) < MaxTextExtent)
          if (IsAccessible(path))
            return(MagickPass);
      }
  }
#endif
  return(MagickFail);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t E x e c u t i o n P a t h U s i n g N a m e                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetExecutionPathUsingName() replaces the provided path with the full
%  path to the directory containing the executable.  The replaced path
%  is terminated by a directory separator.  The provided path may be
%  a bare executable name, a relative path to the executable, or the
%  full path to the executable.  The provided path is usually obtained
%  from the argv[0] argument to main. If the path is a bare executable
%  name, then the executable is located via the executable search path.
%  If the path is replaced, then MagickPass is returned, otherwise
%  MagickFail is returned.
%
%  The format of the GetExecutionPathUsingName method is:
%
%      unsigned int GetExecutionPathUsingName(char *path)
%
%  A description of each parameter follows:
%
%    o path: The path (partial or complete) to the executable.
%
*/
MagickExport MagickPassFail GetExecutionPathUsingName(char *path)
{
  char
    execution_path[MaxTextExtent],
    original_cwd[MaxTextExtent],
    temporary_path[MaxTextExtent],
    *p;

  execution_path[0]='\0';

  /*
    Save original working directory so it can be restored later.
  */
  if (getcwd(original_cwd,sizeof(original_cwd)-1) == NULL)
              MagickFatalError(ConfigureFatalError,UnableToGetCurrentDirectory,
                               NULL);

  /*
    Check to see if path is a valid relative path from current
    directory.
  */
  if (IsAccessibleNoLogging(path))
    {
      /*
        If we can change directory to the path, then capture the full
        path to it.  Otherwise, remove any trailing path component
        (typically the program name) and try again.
      */
      if ((strlen(path) >0 ) && (chdir(path) == 0))
        {
          if (getcwd(execution_path,sizeof(execution_path)-2) == NULL)
            MagickFatalError(ConfigureFatalError,UnableToGetCurrentDirectory,
                             NULL);
        }
      else
        {
          (void) strlcpy(temporary_path,path,sizeof(execution_path));
          p=strrchr(temporary_path,DirectorySeparator[0]);
          if (p)
            *p='\0';
          if ((strlen(temporary_path) > 0) && (chdir(temporary_path) == 0))
            {
              if (getcwd(execution_path,sizeof(execution_path)-2) == NULL)
                MagickFatalError(ConfigureFatalError,UnableToGetCurrentDirectory,
                                 NULL);
            }
        }
    }
  /*
    Otherwise, check to see if bare program name is available via the
    executable search path.
  */
  if ((execution_path[0] == 0) && (strchr(path,DirectorySeparator[0]) == NULL ))
  {
    const char
      *search_path;

    search_path=getenv("PATH");
    if ( search_path )
      {
        const char
          *end = NULL,
          *start = search_path;
        
        end=start+strlen(start);
        while ( start < end )
          {
            const char
              *separator;
            
            int
              length;
            
            separator = strchr(start,DirectoryListSeparator);
            if (separator)
              length=separator-start;
            else
              length=end-start;
            if (length > MaxTextExtent-1)
              length = MaxTextExtent-1;
            (void) strlcpy(temporary_path,start,length+1);
            if ((strlen(temporary_path) > 0) && (chdir(temporary_path) == 0))
              {
                if (temporary_path[length-1] != DirectorySeparator[0])
                  (void) strlcat(temporary_path,DirectorySeparator,sizeof(temporary_path));
                (void) strlcat(temporary_path,path,sizeof(temporary_path));
                if (IsAccessibleNoLogging(temporary_path))
                  {
                    if (getcwd(execution_path,sizeof(execution_path)-2) == NULL)
                      MagickFatalError(ConfigureFatalError,UnableToGetCurrentDirectory,
                                       NULL);
                    break;
                  }
              }
            start += length+1;
          }
      }
  }

  /*
    Restore original working directory.
  */
  if ((strlen(original_cwd) > 0) && (chdir(original_cwd) != 0))
    return(MagickFail);

  if (execution_path[0] != '\0')
    {
      (void) strlcat(execution_path,DirectorySeparator,sizeof(execution_path));
      (void) strlcpy(path,execution_path,MaxTextExtent);
      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
                            "Path \"%.1024s\" is usable.",path);
      errno=0;
      return (MagickPass);
    }

  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
                        "Path \"%.1024s\" is not valid.",path);
  return(MagickFail);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t G e o m e t r y                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetGeometry() parses a geometry specification and returns the width
%  height, x, and y values.  It also returns flags that indicates which
%  of the four values (width, height, x, y) were located in the string, and
%  whether the x and y values are negative.  In addition, there are flags to
%  report any meta characters (%, !, <,  >, @, and ^).
%
%  The format of the GetGeometry method is:
%
%      int GetGeometry(const char *image_geometry,long *x,long *y,
%        unsigned long *width,unsigned long *height)
%
%  A description of each parameter follows:
%
%    o flags:  Method GetGeometry returns a bitmask that indicates
%      which of the four values were located in the geometry string.
%
%    o image_geometry:  Specifies a character string representing the geometry
%      specification.
%
%    o x,y:  The x and y offset as determined by the geometry specification is
%      returned here.
%
%    o width,height:  The width and height as determined by the geometry
%      specification is returned here.
%
%
*/
MagickExport int GetGeometry(const char *image_geometry,long *x,long *y,
  unsigned long *width,unsigned long *height)
{
  const char
    *c;

  char
    geometry[MaxTextExtent],
    *p,
    *q;

  int
    count,
    flags,
    i;

  double
    double_val;

  RectangleInfo
    bounds;

  /*
    Ensure the image geometry is valid.
  */
  assert(x != (long *) NULL);
  assert(y != (long *) NULL);
  assert(width != (unsigned long *) NULL);
  assert(height != (unsigned long *) NULL);
  if ((image_geometry == (char *) NULL) || (*image_geometry == '\0') ||
      (strlen(image_geometry) > MaxTextExtent-1))
    return(NoValue);

  /*
    Transfer base geometry while recording and stripping flags
  */
  i=0;
  q=geometry;
  flags=NoValue;

  for (c=image_geometry; *c != 0 ; c++)
    {
      if (isspace((int)(unsigned char) (*c)))
        {
          continue;
        }
      else
        switch (*c)
          {
          case '%':
            {
              flags|=PercentValue;
              break;
            }
          case '!':
            {
              flags|=AspectValue;
              break;
            }
          case '<':
            {
              flags|=LessValue;
              break;
            }
          case '>':
            {
              flags|=GreaterValue;
              break;
            }
          case '@':
            {
              flags|=AreaValue;
              break;
            }
          case '^':
            {
              flags|=MinimumValue;
              break;
            }
          case '+':
          case '-':
          case '.':
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
          case 'X':
          case 'x':
            {
              /* Check for too many characters. */
              i++;
              if (i == sizeof(geometry))
                return NoValue;

              *q=*c;
              q++;
              break;
            }
          default:
            {
              /* Illegal character fails entire geometry translation */
              return NoValue;
            }
          }
    }
  *q='\0';

  /*
    Parse width/height/x/y.  At this point resize qualifiers have been
    truncated from the original geometry string.
  */
  bounds.width=0;
  bounds.height=0;
  bounds.x=0;
  bounds.y=0;
  p=geometry;
  while (isspace((int)(unsigned char) *p))
    p++;
  if (*p == '\0')
    return(flags);
  if (*p == '=')
    p++;
  if ((*p != '+') && (*p != '-') && (*p != 'x') && (*p != 'X'))
    {
      /*
        Parse width.
      */
      q=p;

      count=MagickStrToD(p,&q,&double_val);
      if (count)
        {
          bounds.width=(unsigned long) floor(double_val+0.5);
          flags|=WidthValue;
        }
      if ((*q == 'x') || (*q == 'X') || ((flags & AreaValue) && (*q == '\0')))
        p=q;
      else
        {
          count=MagickStrToD(p,&p,&double_val);
          if (count)
            {
              bounds.width=(unsigned long) floor(double_val+0.5);
              bounds.height=bounds.width;
              flags|=HeightValue;
            }
        }
    }
  if ((*p == 'x') || (*p == 'X'))
    {
      /*
        Parse height.
      */
      p++;
      q=p;
      count=MagickStrToD(p,&p,&double_val);
      if (count)
        {
          bounds.height=(unsigned long) floor(double_val+0.5);
          flags|=HeightValue;
        }
    }
  if ((*p == '+') || (*p == '-'))
    {
      /*
        Parse x value.
      */
      if (*p == '+')
        {
          p++;
          q=p;
          count=MagickStrToD(p,&p,&double_val);
          bounds.x=(long) ceil(double_val-0.5);
        }
      else
        {
          p++;
          q=p;
          count=MagickStrToD(p,&p,&double_val);
          if (count)
            {
              bounds.x=(long) ceil(-double_val-0.5);
              flags|=XNegative;
            }
        }
      if (count)
        flags|=XValue;
      if ((*p == '+') || (*p == '-'))
        {
          /*
            Parse y value.
          */
          if (*p == '+')
            {
              p++;
              q=p;
              count=MagickStrToD(p,&p,&double_val);
              bounds.y=(long) ceil(double_val-0.5);
            }
          else
            {
              p++;
              q=p;
              count=MagickStrToD(p,&p,&double_val);
              if (count)
                {
                  bounds.y=(long) ceil(-double_val-0.5);
                  flags|=YNegative;
                }
            }
          if (count)
            flags|=YValue;
        }
    }
  if (*p != '\0')
    return(flags);
  if (flags & XValue)
    *x=bounds.x;
  if (flags & YValue)
    *y=bounds.y;
  if (flags & WidthValue)
    *width=bounds.width;
  if (flags & HeightValue)
    *height=bounds.height;
  return(flags);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t M a g i c k D i m e n s i o n                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickDimension() parses a string in the scanf form %lfx%lf+%lf+%lf
%  to obtain WIDTHxHEIGHT+XOFF+YOFF values and returns the number of values
%  successfully parsed. This function exists to overcome a new behavior of
%  ANSI C'99 which supports hex parsing.
%
%  The format of the GetMagickDimension method is:
%
%      int GetMagickDimension(const char *str,double *width,double *height,
%                             double *xoff,double *yoff)
%
%  A description of each parameter follows:
%
%    o str:    String to parse
%
%    o width:  First double value
%
%    o height: Second double value
%
%    o xoff:   Third double value (usually "x offset").  May be NULL.
%
%    o yoff:   Fourth double value (usually "y offset"). May be NULL.
%
*/
static int MagickStrToD(const char *start,char **end,double *value)
{
  const char
    *p;

  char
    buff[MaxTextExtent],
    *endptr;

  int
    i,
    n=0;
  
  p=start;
  
  for (i=0; (*p != 0) && (*p != 'x') && (*p != ',') && (i < MaxTextExtent-2); i++)
    buff[i]=*p++;
  buff[i]=0;
  errno=0;
  *value=strtod(buff,&endptr);
  if ((errno == 0) && (buff != endptr))
    n++;
  *end=(char *) start+(endptr-buff);

  return (n);
}
MagickExport int
GetMagickDimension(const char *str,double *width,double *height,
                   double *xoff,double *yoff)
{
  int
    n,
    parsed;

  const char
    *start=str;

  char
    *end;

  n=MagickStrToD(start,&end,width);
  if (n == 0)
    return n;
  start=end;
  if (*start == '%')
    start++;
  if ((*start != 'x') && (*start != 'X'))
    return n;
  start++;
  parsed = MagickStrToD(start,&end,height);
  if (parsed == 0)
    return n;
  n += parsed;
  start=end;
  if (xoff != (double *) NULL)
    {
      if ((*start != '+') && (*start != '-'))
        return n;
      parsed = MagickStrToD(start,&end,xoff);
      if (parsed == 0)
        return n;
      n += parsed;
      if (*(start -1) == '-')
        *xoff=-*xoff;
      start=end;
    }
  if (yoff != (double *) NULL)
    {
      if ((*start != '+') && (*start != '-'))
        return n;
      parsed = MagickStrToD(start,&end,yoff);
      if (parsed == 0)
        return n;
      n += parsed;
      if (*(start -1) == '-')
        *yoff=-*yoff;
      start=end;
    }

  return (n);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t M a g i c k G e o m e t r y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickGeometry() is similar to GetGeometry() except that existing
%  'width', and 'height' input values are modified by the geometry
%  influenced by the meta characters:  %, @, !, <, and >, as well as
%  +x, and +y offsets.  The geometry string has the form:
%
%      <width>x<height>{+-}<x>{+-}<y>{%}{@} {!}{<}{>}
%
%  For example, the string "640x480>" is a valid geometry string.
%
%  The interpretation of the extra geometry string parameters are as
%  follows:
%    %: (PercentValue) The geometry width and height parameters are
%       interpreted as a percentage of the supplied width and height
%       parameters.
%    @: (AreaValue) The geometry parameter represents the desired total
%       area (e.g. "307520@") or an area equivalent to a specified
%       width x height (e.g. "640x480@"), of the final image.
%    !: (AspectValue) Force the width and height values to be absolute
%       values.  The original image aspect ratio is not maintained.
%    <: (LessValue) Update the provided width and height parameters if
%       its dimensions are less than the geometry specification.
%    >: (GreaterValue) Update the provided width and height parameters
%       if its dimensions are greater than the geometry specification.
%    ^: (MinimumValue) Width and height are increased as required to
%       preserve aspect ratio while ensuring that width and height are
%       no less than specified.
%
%  Any supplied offset parameters are used to adjust the image width,
%  height, and x/y offset values as required to center the scaled image
%  into the region specified by the supplied width and height.
%
%  If only the width is specified, the width assumes the value and the
%  height is chosen to maintain the aspect ratio of the image. Similarly,
%  if only the height is specified (e.g., -geometry x256), the width is
%  chosen to maintain the aspect ratio.
%
%  The format of the GetMagickGeometry method is:
%
%      int GetMagickGeometry(const char *geometry,long *x,long *y,
%        unsigned long *width,unsigned long *height)
%
%  A description of each parameter follows:
%
%    o flags:  Method GetMagickGeometry returns a bitmask that indicates
%      which of the twelve values (XValue, YValue, WidthValue, HeightValue,
%      XNegative, YNegative, PercentValue, AspectValue, LessValue,
%      GreaterValue, AreaValue, MinimumValue) were located in the geometry
%      string.
%
%    o image_geometry:  Specifies a character string representing the geometry
%      specification.
%
%    o x,y:  A pointer to an integer.  The x and y offset as determined by
%      the geometry specification is returned here.
%
%    o width,height:  A pointer to an unsigned integer.  The width and height
%      as determined by the geometry specification is returned here.
%
%
*/
MagickExport int GetMagickGeometry(const char *geometry,long *x,long *y,
                                   unsigned long *width,unsigned long *height)
{
  int
    flags;

  long
    count;

  unsigned long
    former_height,
    former_width;

  /*
    Ensure the image geometry is valid.
  */
  assert(x != (long *) NULL);
  assert(y != (long *) NULL);
  assert(width != (unsigned long *) NULL);
  assert(height != (unsigned long *) NULL);
  if ((geometry == (char *) NULL) || (*geometry == '\0'))
    return(NoValue);
  /*
    Parse geometry using GetGeometry.
  */
  former_width=(*width);
  former_height=(*height);
  flags=GetGeometry(geometry,x,y,width,height);

  if ((former_width == 0UL) || (former_height == 0UL))
    return(flags);

#if 0
      fprintf(stderr,"WidthValue=%u, HeightValue=%u width=%lu, height=%lu\n",
              flags & WidthValue ? 1 : 0,
              flags & HeightValue ? 1 : 0,
              *width,*height);
#endif

  if (flags & AreaValue) /* @  */
    {
      double
        scale_factor,
        original_area,
        target_area;

      MagickBool
        resize;

      /*
        Geometry is an area in pixels.
      */
      target_area=0.0;
      if (flags & WidthValue)
        target_area=(double) (*width);
      if (flags & HeightValue)
        target_area *= (double) (*height);
      original_area=(double) former_width*former_height;
      resize=MagickFalse;
      if (flags & GreaterValue)
        {
          /* Resize if image area greater than target */
          if (original_area > target_area)
            resize=MagickTrue;
        }
      else if (flags & LessValue)
        {
          /* Resize if image area less than target */
          if (original_area < target_area)
            resize=MagickTrue;
        }
      else
        {
          /* Always resize */
          resize=MagickTrue;
        }

      if (resize)
        {
          scale_factor=1.0/sqrt(original_area/target_area);
          *width=(unsigned long) floor(former_width*scale_factor+0.25);
          *height=(unsigned long) floor(former_height*scale_factor+0.25);
        }
      else
        {
          *width=former_width;
          *height=former_height;
        }
    }
  else
    {
      /*
        Deal with width or height being missing from geometry.  Supply
        missing value required to preserve current image aspect ratio.
      */
      if (((flags & WidthValue) && !(flags & HeightValue)))
        *height=(unsigned long) floor(((double) former_height/former_width)*
                                      (*width)+0.5);
      else if ((!(flags & WidthValue) &&  (flags & HeightValue)))
        *width=(unsigned long) floor(((double) former_width/former_height)*
                                     (*height)+0.5);
#if 0
      fprintf(stderr,"Geometry Bounds: %lux%lu\n",*width,*height);
#endif
      if (flags & PercentValue)
        {
          double
            x_scale,
            y_scale;

          /*
            Geometry is a percentage of the image size.
          */
          x_scale=(*width);
          y_scale=(*height);
          count=GetMagickDimension(geometry,&x_scale,&y_scale,NULL,NULL);
          if (count == 1)
            y_scale=x_scale;
          *width=(unsigned long) floor((x_scale*former_width/100.0)+0.5);
          *height=(unsigned long) floor((y_scale*former_height/100.0)+0.5);
          former_width=(*width);
          former_height=(*height);
        }
      if (!(flags & AspectValue) && /* ! */
          ((*width != former_width) || (*height != former_height)))
        {
          double
            scale_factor;

          /*
            Respect aspect ratio of the image but assure that it is no
            larger than specified.
          */
          if ((former_width == 0) || (former_height == 0))
            {
              scale_factor=1.0;
            }
          else
            {
              double
                scale_height,
                scale_width;

              scale_height=(double) *height/former_height;
              scale_width=(double) *width/former_width;

              scale_factor=scale_width;
              if ((flags & MinimumValue) != 0)
                {
                  /* Width and height are minimum values */
                  if (scale_width < scale_height)
                    scale_factor=scale_height;
                }
              else
                {
                  /* Width and height are maximum values */
                  if (scale_width > scale_height)
                    scale_factor=scale_height;
                }
            }
          *width=(unsigned long) floor(scale_factor*former_width+0.5);
          *height=(unsigned long) floor(scale_factor*former_height+0.5);
        }
      if (flags & GreaterValue) /* > */
        {
          if (former_width < *width)
            *width=former_width;
          if (former_height < *height)
            *height=former_height;
        }
      if (flags & LessValue) /* < */
        {
          if (former_width > *width)
            *width=former_width;
          if (former_height > *height)
            *height=former_height;
        }
    }

  return(flags);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  G e t P a g e G e o m e t r y                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetPageGeometry() returns a new geometry string, with any embedded page
%  mneumonic in the original geometry string replaced with the equivalent
%  size in picas. For example, the geometry string "A4+36+36" is translated
%  to "595x842+36+36" in the returned value.
%
%  The format of the GetPageGeometry method is:
%
%      char *GetPageGeometry(const char *page_geometry)
%
%  A description of each parameter follows.
%
%   o  page_geometry:  The geometry string to translate.
%
%
*/
MagickExport char *GetPageGeometry(const char *page_geometry)
{
#define PAGE_SIZE(name, geometry) { name, sizeof(name)-1, geometry }
  static const struct
  {
    char
      *name;

    size_t
       name_length;

    char
       *geometry;
  }
  PageSizes[] =
    {
      PAGE_SIZE("4x6",  "288x432"),
      PAGE_SIZE("5x7",  "360x504"),
      PAGE_SIZE("7x9",  "504x648"),
      PAGE_SIZE("8x10", "576x720"),
      PAGE_SIZE("9x11",  "648x792"),
      PAGE_SIZE("9x12",  "648x864"),
      PAGE_SIZE("10x13",  "720x936"),
      PAGE_SIZE("10x14",  "720x1008"),
      PAGE_SIZE("11x17",  "792x1224"),
      PAGE_SIZE("A0",  "2384x3370"),
      PAGE_SIZE("A1",  "1684x2384"),
      PAGE_SIZE("A10", "73x105"),
      PAGE_SIZE("A2",  "1191x1684"),
      PAGE_SIZE("A3",  "842x1191"),
      PAGE_SIZE("A4",  "595x842"),
      PAGE_SIZE("A4SMALL", "595x842"),
      PAGE_SIZE("A5",  "420x595"),
      PAGE_SIZE("A6",  "297x420"),
      PAGE_SIZE("A7",  "210x297"),
      PAGE_SIZE("A8",  "148x210"),
      PAGE_SIZE("A9",  "105x148"),
      PAGE_SIZE("ARCHA", "648x864"),
      PAGE_SIZE("ARCHB", "864x1296"),
      PAGE_SIZE("ARCHC", "1296x1728"),
      PAGE_SIZE("ARCHD", "1728x2592"),
      PAGE_SIZE("ARCHE", "2592x3456"),
      PAGE_SIZE("B0",  "2920x4127"),
      PAGE_SIZE("B1",  "2064x2920"),
      PAGE_SIZE("B10", "91x127"),
      PAGE_SIZE("B2",  "1460x2064"),
      PAGE_SIZE("B3",  "1032x1460"),
      PAGE_SIZE("B4",  "729x1032"),
      PAGE_SIZE("B5",  "516x729"),
      PAGE_SIZE("B6",  "363x516"),
      PAGE_SIZE("B7",  "258x363"),
      PAGE_SIZE("B8",  "181x258"),
      PAGE_SIZE("B9",  "127x181"),
      PAGE_SIZE("C0",  "2599x3676"),
      PAGE_SIZE("C1",  "1837x2599"),
      PAGE_SIZE("C2",  "1298x1837"),
      PAGE_SIZE("C3",  "918x1296"),
      PAGE_SIZE("C4",  "649x918"),
      PAGE_SIZE("C5",  "459x649"),
      PAGE_SIZE("C6",  "323x459"),
      PAGE_SIZE("C7",  "230x323"),
      PAGE_SIZE("EXECUTIVE", "540x720"),
      PAGE_SIZE("FLSA", "612x936"),
      PAGE_SIZE("FLSE", "612x936"),
      PAGE_SIZE("FOLIO",  "612x936"),
      PAGE_SIZE("HALFLETTER", "396x612"),
      PAGE_SIZE("ISOB0", "2835x4008"),
      PAGE_SIZE("ISOB1", "2004x2835"),
      PAGE_SIZE("ISOB10", "88x125"),
      PAGE_SIZE("ISOB2", "1417x2004"),
      PAGE_SIZE("ISOB3", "1001x1417"),
      PAGE_SIZE("ISOB4", "709x1001"),
      PAGE_SIZE("ISOB5", "499x709"),
      PAGE_SIZE("ISOB6", "354x499"),
      PAGE_SIZE("ISOB7", "249x354"),
      PAGE_SIZE("ISOB8", "176x249"),
      PAGE_SIZE("ISOB9", "125x176"),
      PAGE_SIZE("LEDGER",  "1224x792"),
      PAGE_SIZE("LEGAL",  "612x1008"),
      PAGE_SIZE("LETTER", "612x792"),
      PAGE_SIZE("LETTERSMALL",  "612x792"),
      PAGE_SIZE("QUARTO",  "610x780"),
      PAGE_SIZE("STATEMENT",  "396x612"),
      PAGE_SIZE("TABLOID",  "792x1224"),
    };

  char
    page[MaxTextExtent];

  register size_t
    i;

  assert(page_geometry != (char *) NULL);
  strlcpy(page,page_geometry,MaxTextExtent);
  for (i=0; i < sizeof(PageSizes)/sizeof(PageSizes[0]); i++)
    if (LocaleNCompare(PageSizes[i].name,page_geometry,
                       PageSizes[i].name_length) == 0)
      {
        int
          flags;

        RectangleInfo
          geometry;

        /*
          Replace mneumonic with the equivalent size in dots-per-inch.
        */
        FormatString(page,"%s%.80s", PageSizes[i].geometry,
                     page_geometry+PageSizes[i].name_length);
        flags=GetGeometry(page,&geometry.x,&geometry.y,&geometry.width,
          &geometry.height);
        if (!(flags & GreaterValue))
          (void) strcat(page,">");
        break;
      }
  return (AcquireString(page));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t P a t h C o m p o n e n t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetPathComponent returns the parent directory name, filename,
%  basename, or extension of a file path.  File paths are in the form:
%
%     magick:base.ext[subimage]
%
%  Similar to
%
%      jpg:bar
%      jpg:foo.bar
%      pdf:foo.pdf[2-3]
%      foo.pdf[2-3]
%      C:\path\foo.bar
%      /path/foo.bar
%      pdf:C:\path\foo.pdf[2-3]
%
%  Note that Windows drive letters may be part of paths on Windows systems
%  and such paths include a colon.  Path components in real file paths may
%  include a colon and the file might not exist yet (or ever) so testing
%  for it is not reliable.  This means that determination of the "magick"
%  part is ambiguous and not reliable.
%
%  The format of the GetPathComponent function is:
%
%      GetPathComponent(const char *path,PathType type,char *component)
%
%  A description of each parameter follows:
%
%    o path: Specifies a pointer to a character array that contains the
%      file path.
%
%    o type: Specififies which file path component to return (RootPath,
%              HeadPath, TailPath, BasePath, ExtensionPath, MagickPath,
%              SubImagePath, or FullPath).
%
%    o component: The selected file path component is returned here.
%
*/
static inline unsigned int IsFrame(const char *point)
{
  char
    *p;

  long
    long_val;

  long_val = strtol(point,&p,10);
  (void) long_val;
  return(p != point);
}

MagickExport void GetPathComponent(const char *path,PathType type,
  char *component)
{
  register char
    *p;

  char
    magick[MaxTextExtent],
    subimage[MaxTextExtent];

  /*
    Get basename of client.
  */
  assert(path != (const char *) NULL);
  assert(component != (const char *) NULL);
  if (strlcpy(component,path,MaxTextExtent) >= MaxTextExtent)
    MagickFatalError2(ResourceLimitFatalError,"Path buffer overflow",
                      path);
  if (*path == '\0')
    return;
  subimage[0]=magick[0]='\0';

  /*
    Remove magic and subimage spec. from the path to make
    it easier to extract filename parts.
  */
  for (p=component; (*p != '\0') && (*p != ':'); p++)
    ;
  if (*p == ':')
    {
      (void) strncpy(magick,component,(size_t)(p-component)+1);
      magick[p-component+1]='\0';
      if (IsMagickConflict(magick))
        {
          magick[0]='\0';
        }
      else
        {
          register char
            *q;

          /*
            Safe-copy the remaining component part on top of the magic part.
          */
          magick[p-component]='\0';
          p++;
          q=component;
          while ((*q++=*p++))
            ;
        }
    }

  p=component+strlen(component);
  if ((p > component) && (*--p == ']'))
    {
      /* Look for a '[' matching the ']' */
      p--;
      while ((p > component) &&
             (*p != '[') &&
             (strchr("0123456789xX,-+ ", (int)(unsigned char)*p) != 0) )
        p--;

      /* Copy to subimage and remove from component */
      if ((p > component) && (*p == '[') && IsFrame(p+1))
        {
          (void) strlcpy(subimage, p,sizeof(subimage));
          *p='\0';
        }
    }

  /* first locate the spot were the filename begins */
  for (p=component+strlen(component); p > component; p--)
    if (IsBasenameSeparator(*p))
      break;

  switch (type)
  {
    case MagickPath:
    {
      /* this only includes the magick override prefix (if any) */
      (void) strlcpy(component,magick,MaxTextExtent);
      break;
    }
    case SubImagePath:
    {
      /* this returns only the subimage specification, including
         bracketing '[]', (if any) */
      (void) strlcpy(component,subimage,MaxTextExtent);
      break;
    }
    case FullPath:
    {
      /* this returns the full path except magic and sub-image spec. */
      break;
    }
    case RootPath:
    {
      /* this returns the path including the file part with no extension */
      for (p=component+strlen(component); p > component; p--)
        if (*p == '.')
          break;
      if (*p == '.')
        *p='\0';
      break;
    }
    case HeadPath:
    {
      /* this returns the path only with no trailing separator */
      *p='\0';
      break;
    }
    case TailPath:
    {
      /* this returns the filename and extension only */
      if (IsBasenameSeparator(*p))
        {
          char
            scratch[MaxTextExtent];

          (void) strlcpy(scratch,p+1,MaxTextExtent);
          (void) strlcpy(component,scratch,MaxTextExtent);
        }
      break;
    }
    case BasePath:
    {
      /* this returns just the filename with no extension */
      if (IsBasenameSeparator(*p))
        {
          char
            scratch[MaxTextExtent];

          (void) strlcpy(scratch,p+1,MaxTextExtent);
          (void) strlcpy(component,scratch,MaxTextExtent);
        }
      for (p=component+strlen(component); p > component; p--)
        if (*p == '.')
          {
            *p='\0';
            break;
          }
      break;
    }
    case ExtensionPath:
    {
      /* this returns the file extension only */
      if (IsBasenameSeparator(*p))
        {
          char
            scratch[MaxTextExtent];

          (void) strlcpy(scratch,p+1,MaxTextExtent);
          (void) strlcpy(component,scratch,MaxTextExtent);
        }
      for (p=component+strlen(component); p > component; p--)
        if (*p == '.')
          break;
      *component='\0';
      if (*p == '.')
        {
          char
            scratch[MaxTextExtent];

          (void) strlcpy(scratch,p+1,MaxTextExtent);
          (void) strlcpy(component,scratch,MaxTextExtent);
        }
      break;
    }
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t T o k e n                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetToken gets a token from the token stream.  A token is defined
%  as sequence of characters delimited by whitespace (e.g. clip-path), a
%  sequence delimited with quotes (.e.g "Quote me"), or a sequence enclosed
%  in parenthesis (e.g. rgb(0,0,0)).  The output to token is constrained to
%  not overflow a buffer of size MaxTextExtent so it should be allocated with
%  that size.
%
%  The format of the GetToken method is:
%
%      void GetToken(const char *start,char **end,char *token)
%
%  A description of each parameter follows:
%
%    o start: the start of the token sequence.
%
%    o end: point to the end of the token sequence (may be NULL).
%
%    o token: copy the token to this buffer.
%
%
*/
MagickExport void GetToken(const char *start,char **end,char *token)
{
  (void) MagickGetToken(start,end,token, MaxTextExtent);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G l o b E x p r e s s i o n                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GlobExpression returns True if the expression matches the pattern.
%  The pattern specification syntax is roughly similar to that supported by
%  POSIX.2 glob().
%
%  Please note that there is a conflict between glob patterns and subimage
%  specifications.  For example, video.mpg[50] or video.mpg[50-75] might
%  refer to video.mpg (page 51, or pages 51-76) or there might be a file
%  named 'video.mpg[50]'.  The IsSubimage() function is used to quickly test
%  if the specification may be a subimage specification.  If the pattern is
%  ultimately determined to be a subimage specification, then globbing is
%  not performed.
%  
%
%  The format of the GlobExpression function is:
%
%      int GlobExpression(const char *expression,const char *pattern)
%
%  A description of each parameter follows:
%
%    o expression: Specifies a pointer to a text string containing a file name.
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%            Patterns supported are roughly equivalent to those supported by
%            POSIX.2 glob().
%
%
*/
MagickExport int GlobExpression(const char *expression,const char *pattern)
{
  unsigned int
    done;

  /*
    Return on empty pattern or '*'.
  */
  if (pattern == (char *) NULL)
    return(True);
  if (strlen(pattern) == 0)
    return(True);
  if (LocaleCompare(pattern,"*") == 0)
    return(True);
  /*
    Evaluate glob expression.
  */
  done=False;
  while ((*pattern != '\0') && !done)
  {
    if (*expression == '\0')
      if ((*pattern != '{') && (*pattern != '*'))
        break;
    switch (*pattern)
    {
      case '\\':
      {
        pattern++;
        if (*pattern != '\0')
          pattern++;
        break;
      }
      case '*':
      {
        int
          status;

        pattern++;
        status=False;
        while ((*expression != '\0') && !status)
          status=GlobExpression((char *) expression++,pattern);
        if (status)
          {
            while (*expression != '\0')
              expression++;
            while (*pattern != '\0')
              pattern++;
          }
        break;
      }
      case '[':
      {
        char
          c;

        pattern++;
        for ( ; ; )
        {
          if ((*pattern == '\0') || (*pattern == ']'))
            {
              done=True;
              break;
            }
          if (*pattern == '\\')
            {
              pattern++;
              if (*pattern == '\0')
                {
                  done=True;
                  break;
                }
             }
          if (*(pattern+1) == '-')
            {
              c=(*pattern);
              pattern+=2;
              if (*pattern == ']')
                {
                  done=True;
                  break;
                }
              if (*pattern == '\\')
                {
                  pattern++;
                  if (*pattern == '\0')
                    {
                      done=True;
                      break;
                    }
                }
              if ((*expression < c) || (*expression > *pattern))
                {
                  pattern++;
                  continue;
                }
            }
          else
            if (*pattern != *expression)
              {
                pattern++;
                continue;
              }
          pattern++;
          while ((*pattern != ']') && (*pattern != '\0'))
          {
            if ((*pattern == '\\') && (*(pattern+1) != '\0'))
              pattern++;
            pattern++;
          }
          if (*pattern != '\0')
            {
              pattern++;
              expression++;
            }
          break;
        }
        break;
      }
      case '?':
      {
        pattern++;
        expression++;
        break;
      }
      case '{':
      {
        int
          match;

        register const char
          *p;

        pattern++;
        while ((*pattern != '}') && (*pattern != '\0'))
        {
          p=expression;
          match=True;
          while ((*p != '\0') && (*pattern != '\0') &&
                 (*pattern != ',') && (*pattern != '}') && match)
          {
            if (*pattern == '\\')
              pattern++;
            match=(*pattern == *p);
            p++;
            pattern++;
          }
          if (*pattern == '\0')
            {
              match=False;
              done=True;
              break;
            }
          else
            if (match)
              {
                expression=p;
                while ((*pattern != '}') && (*pattern != '\0'))
                {
                  pattern++;
                  if (*pattern == '\\')
                    {
                      pattern++;
                      if (*pattern == '}')
                        pattern++;
                    }
                }
              }
            else
              {
                while ((*pattern != '}') && (*pattern != ',') &&
                       (*pattern != '\0'))
                {
                  pattern++;
                  if (*pattern == '\\')
                    {
                      pattern++;
                      if ((*pattern == '}') || (*pattern == ','))
                        pattern++;
                    }
                }
              }
            if (*pattern != '\0')
              pattern++;
          }
        break;
      }
      default:
      {
        if (*expression != *pattern)
          done=True;
        else
          {
            expression++;
            pattern++;
          }
      }
    }
  }
  while ((*pattern != '\0') && (*pattern == '*'))
    pattern++;
  return((*expression == '\0') && (*pattern == '\0'));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s A c c e s s i b l e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsAccessible() returns MagickTrue if the file as defined by path exists
%  and is readable by the user.
%
%  The format of the IsAccessible method is:
%
%      MagickBool IsAccessible(const char *path)
%
%  A description of each parameter follows.
%
%    o status:  Method IsAccessible returns MagickTrue if the file as defined by
%      path exists and is readable by the user, otherwise MagickFalse is returned.
%
%    o path:  A pointer to an array of characters containing the path.
%
%
*/
MagickExport MagickBool IsAccessible(const char *path)
{
  if ((path == (const char *) NULL) || (*path == '\0'))
    return(MagickFalse);

  if ((access(path,R_OK)) != 0)
    {
      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
        "Tried: %.1024s [%.1024s]",path,strerror(errno));
      return(MagickFalse);
    }
  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
                        "Found: %.1024s",path);
  return (MagickTrue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s A c c e s s i b l e N o L o g g i n g                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsAccessibleNoLogging() returns MagickTrue if the file as defined by path
%  exists and is accessable by the user. This version is used internally to
%  avoid using the error logging of the normal version.
%
%  The format of the IsAccessibleNoLogging method is:
%
%      MagickBool IsAccessibleNoLogging(const char *path)
%
%  A description of each parameter follows.
%
%    o status:  Method IsAccessibleNoLogging returns MagickTrue if the file as
%      defined by path exists and is a regular file, otherwise Magick False is
%      returned.
%
%    o path:  A pointer to an array of characters containing the path.
%
%
*/
MagickExport MagickBool IsAccessibleNoLogging(const char *path)
{
  if ((path == (const char *) NULL) || (*path == '\0'))
    return(MagickFalse);

  if ((access(path,R_OK)) != 0)
    return(MagickFalse);
  return (MagickTrue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s A c c e s s i b l e A n d N o t E m p t y                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsAccessibleAndNotEmpty() returns MagickTrue if the file as defined by path
%  exists, is a regular file, and contains at least one byte of data.
%
%  The format of the IsAccessibleAndNotEmpty method is:
%
%      MagickBool IsAccessibleAndNotEmpty(const char *path)
%
%  A description of each parameter follows.
%
%    o status:  Method IsAccessibleAndNotEmpty returns MagickTrue if the file as
%      defined by path exists, is a regular file, and contains content,
%      otherwise MagickFalse is returned.
%
%    o path:  A pointer to an array of characters containing the path.
%
%
*/
MagickExport MagickBool IsAccessibleAndNotEmpty(const char *path)
{
  int
    status;

  MagickStatStruct_t
    file_info;

  if ((path == (const char *) NULL) || (*path == '\0'))
    return(MagickFalse);
  status=MagickStat(path,&file_info);

  if ((status == 0) && S_ISREG(file_info.st_mode) && (file_info.st_size > 0))
    return (MagickTrue);

  return (MagickFalse);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  I s D i r e c t o r y                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsDirectory() returns -1 if the path does not exist, 0 if the
%  path represents a file, and 1 if the path represents a directory.
%
%  The format of the IsDirectory method is:
%
%      int IsDirectory(const char *path)
%
%  A description of each parameter follows.
%
%   o  status:  Method IsDirectory returns -1 if the path does not exist,
%      0 if the path represents a file, and 1 if the path represents
%      a directory.
%
%   o  path:  Path to file or directory.
%
%
*/
static int IsDirectory(const char *path)
{
  MagickStatStruct_t
    file_info;

  if ((path == (const char *) NULL) || (*path == '\0'))
    return(False);

  if ((MagickStat(path,&file_info)) == 0)
    {
      if (S_ISREG(file_info.st_mode))
        return 0;
      if (S_ISDIR(file_info.st_mode))
        return 1;
    }
  return -1;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+     I s G e o m e t r y                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsGeometry returns MagickTrue if the geometry specification is valid
%  as determined by GetGeometry.
%
%  The format of the IsGeometry method is:
%
%      MagickBool IsGeometry(const char *geometry)
%
%  A description of each parameter follows:
%
%    o status: Method IsGeometry returns MagickTrue if the geometry specification
%      is valid otherwise MagickFalse is returned.
%
%    o geometry: This string is the geometry specification.
%
%
*/
MagickExport MagickBool IsGeometry(const char *geometry)
{
  long
    x,
    y;

  unsigned int
    flags;

  unsigned long
    height,
    width;

  if (geometry == (const char *) NULL)
    return(False);
  flags=GetGeometry(geometry,&x,&y,&width,&height);
  return(flags != NoValue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
+     I s G l o b                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsGlob() returns MagickTrue if the path specification contains a globbing
%  pattern as supported by GlobExpression().  Glob patterns supported are
%  roughly equivalent to those supported by POSIX.2 glob().
%
%  The format of the IsGlob method is:
%
%      MagickBool IsGlob(const char *path)
%
%  A description of each parameter follows:
%
%    o status: Returns MagickTrue if the path specification contains
%      a globbing patten.
%
%    o path: The path.
%
%
*/
MagickExport MagickBool IsGlob(const char *path)
{
  register const char
    *p;

  MagickBool
    status = MagickFalse;

  for (p = path; *p != '\0'; p++)
    {
      switch (*p)
        {
        case '*':
        case '?':
        case '{':
        case '}':
        case '[':
        case ']':
          {
            status = MagickTrue;
            break;
          }
        }
    }

  return status ;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  I s W r i t a b l e                                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IsWritable() returns True if the file as defined by path exists
%  and is writeable by the user.
%
%  The format of the IsWriteable method is:
%
%      MagickBool IsWriteable(const char *path)
%
%  A description of each parameter follows.
%
%    o status:  Method IsWriteable returns MagickTrue if the file as defined
%      by path exists and is writeable by the user, otherwise MagickFalse is
%      returned.
%
%    o path:  A pointer to an array of characters containing the path.
%
%
*/
MagickExport MagickBool IsWriteable(const char *path)
{
  if ((path == (const char *) NULL) || (*path == '\0'))
    return(MagickFalse);

  if ((access(path,W_OK)) != 0)
    {
      return(MagickFalse);
    }
  return (MagickTrue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L i s t F i l e s                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListFiles reads the directory specified and returns a list
%  of filenames contained in the directory sorted in ascending alphabetic
%  order.
%
%  The format of the ListFiles function is:
%
%      char **ListFiles(const char *directory,const char *pattern,
%        long *number_entries)
%
%  A description of each parameter follows:
%
%    o filelist: Method ListFiles returns a list of filenames contained
%      in the directory.  If the directory specified cannot be read or it is
%      a file a NULL list is returned.
%
%    o directory: Specifies a pointer to a text string containing a directory
%      name.
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%
%    o number_entries:  This integer returns the number of filenames in the
%      list.
%
%
*/

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

static int FileCompare(const void *x,const void *y)
{
  register char
    **p,
    **q;

  p=(char **) x;
  q=(char **) y;
  return(LocaleCompare(*p,*q));
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

MagickExport char **ListFiles(const char *directory,const char *pattern,
  long *number_entries)
{
  char
    **filelist,
    filename[MaxTextExtent];

  DIR
    *current_directory;

  int
    status;

  struct dirent
    *entry;

  unsigned int
    max_entries;

  /*
    Open directory.
  */
  assert(directory != (const char *) NULL);
  assert(pattern != (char *) NULL);
  assert(number_entries != (long *) NULL);
  *number_entries=0;
  status=chdir(directory);
  if (status != 0)
    return((char **) NULL);
  if (getcwd(filename,MaxTextExtent-1) == NULL)
    MagickFatalError(ConfigureFatalError,UnableToGetCurrentDirectory,
                     NULL);
  current_directory=opendir(filename);
  if (current_directory == (DIR *) NULL)
    return((char **) NULL);
  if (chdir(filename) != 0)
    {
      (void) closedir(current_directory);
      return((char **) NULL);
    }
  /*
    Allocate filelist.
  */
  max_entries=2048U;
  filelist=MagickAllocateArray(char **,max_entries,sizeof(char *));
  if (filelist == (char **) NULL)
    {
      (void) closedir(current_directory);
      return((char **) NULL);
    }
  /*
    Save the current and change to the new directory.
  */
  entry=readdir(current_directory);
  while (entry != (struct dirent *) NULL)
  {
    if (*entry->d_name == '.')
      {
        entry=readdir(current_directory);
        continue;
      }
    if ((IsDirectory(entry->d_name) > 0) ||
        GlobExpression(entry->d_name,pattern))
      {
        if (*number_entries >= (int) max_entries)
          {
            /*
              Extend the file list.
            */
            max_entries<<=1;
            MagickReallocMemory(char **,filelist,max_entries*sizeof(char *));
            if (filelist == (char **) NULL)
              {
                (void) closedir(current_directory);
                /*
                  We simply bail here since our memory reallocator has
                  just leaked lots of memory and returning does not
                  solve the problem.
                */
                MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
                                  UnableToAllocateString);
              }
          }
        {
          size_t
            entry_length;

          entry_length=strlen(entry->d_name)+1;
          if (IsDirectory(entry->d_name) > 0)
            entry_length+=strlen(DirectorySeparator);

          filelist[*number_entries]=MagickAllocateMemory(char *,entry_length);
          if (filelist[*number_entries] == (char *) NULL)
            {
              break;
            }
          (void) strlcpy(filelist[*number_entries],entry->d_name,entry_length);
          if (IsDirectory(entry->d_name) > 0)
            (void) strlcat(filelist[*number_entries],DirectorySeparator,entry_length);
        }
        (*number_entries)++;
      }
    entry=readdir(current_directory);
  }
  (void) closedir(current_directory);
  /*
    Sort filelist in ascending order.
  */
  qsort((void *) filelist,*number_entries,sizeof(filelist[0]),FileCompare);
  return(filelist);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o c a l e C o m p a r e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LocaleCompare performs a case-insensitive comparison of two strings
%  byte-by-byte, according to the ordering of the current locale encoding.
%  LocaleCompare returns an integer greater than, equal to, or less than 0,
%  if the string pointed to by p is greater than, equal to, or less than the
%  string pointed to by q respectively.  The sign of a non-zero return value
%  is determined by the sign of the difference between the values of the first
%  pair of bytes that differ in the strings being compared.
%
%  The format of the LocaleCompare method is:
%
%      int LocaleCompare(const char *p,const char *q)
%
%  A description of each parameter follows:
%
%    o p: A pointer to a character string.
%
%    o q: A pointer to a character string to compare to p.
%
%
*/
MagickExport int LocaleCompare(const char *p,const char *q)
{
  int
    result=0;

  register size_t
    i;

  if (p == (char *) NULL)
    result=-1;
  else if (q == (char *) NULL)
    result=1;
  else
    for (i=0; ; i++)
      {
        if (((result=
              AsciiMap[(unsigned int) ((unsigned char *) p)[i] & 0xff] -
              AsciiMap[(unsigned int) ((unsigned char *) q)[i] & 0xff]) != 0)
            || (p[i] == 0) || (q[i] == 0))
          break;
      }
  return result;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o c a l e L o w e r                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LocaleLower transforms all of the characters in the supplied
%  null-terminated string, changing all uppercase letters to lowercase.
%
%  The format of the LocaleLower method is:
%
%      void LocaleLower(char *string)
%
%  A description of each parameter follows:
%
%    o string: A pointer to the string to convert to lower-case Locale.
%
%
*/
MagickExport void LocaleLower(char *string)
{
  register char
    *q;

  assert(string != (char *) NULL);
  for (q=string; *q != '\0'; q++)
    *q=(char) tolower((int) *q);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o c a l e N C o m p a r e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LocaleNCompare performs a case-insensitive comparison of two
%  strings byte-by-byte, according to the ordering of the current locale
%  encoding. LocaleNCompare returns an integer greater than, equal to, or
%  less than 0, if the string pointed to by p is greater than, equal to, or
%  less than the string pointed to by q respectively.  The sign of a non-zero
%  return value is determined by the sign of the difference between the
%  values of the first pair of bytes that differ in the strings being
%  compared.  The LocaleNCompare method makes the same comparison as
%  LocaleCompare but looks at a maximum of n bytes.  Bytes following a
%  null byte are not compared.
%
%  The format of the LocaleNCompare method is:
%
%      int LocaleNCompare(const char *p,const char *q,const size_t n)
%
%  A description of each parameter follows:
%
%    o p: A pointer to a character string.
%
%    o q: A pointer to a character string to compare to p.
%
%    o length: The number of characters to compare in strings p & q.
%
%
*/
MagickExport int LocaleNCompare(const char *p,const char *q,const size_t length)
{
  int
    result=0;

  register size_t
    i;

  if (p == (char *) NULL)
    result = -1;
  else if (q == (char *) NULL)
    result = 1;
  else
    for (i=0; i < length; i++)
      {
        if (((result=
              AsciiMap[(unsigned int) ((unsigned char *) p)[i] & 0xff] -
              AsciiMap[(unsigned int) ((unsigned char *) q)[i] & 0xff]) != 0)
            || (p[i] == 0) || (q[i] == 0))
          break;
      }
  return result;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L o c a l e U p p e r                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method LocaleUpper transforms all of the characters in the supplied
%  null-terminated string, changing all lowercase letters to uppercase.
%
%  The format of the LocaleUpper method is:
%
%      void LocaleUpper(char *string)
%
%  A description of each parameter follows:
%
%    o string: A pointer to the string to convert to upper-case Locale.
%
%
*/
MagickExport void LocaleUpper(char *string)
{
  register char
    *q;

  assert(string != (char *) NULL);
  for (q=string; *q != '\0'; q++)
    *q=(char) toupper((int) *q);
}

MagickExport MagickPassFail MagickAtoFChk(const char *str, double *value)
{
  char *estr=0;
  *value=strtod(str,&estr);
  if (str == estr)
    *value=0.0;
  return (str == estr ? MagickFail : MagickPass);
}

MagickExport MagickPassFail MagickAtoIChk(const char *str, int *value)
{
  char *estr=0;
  long lvalue;
  lvalue=strtol(str,&estr, 10);
  if ((str == estr) || ((int) lvalue != lvalue))
    lvalue=0;
  *value=(int) lvalue;
  return (str == estr ? MagickFail : MagickPass);
}

MagickExport MagickPassFail MagickAtoUIChk(const char *str, unsigned int *value)
{
  char *estr=0;
  long lvalue;
  lvalue=strtol(str,&estr, 10);
  if ((str == estr) || ((long) ((unsigned int) lvalue) != lvalue))
    lvalue=0U;
  *value=(unsigned int) lvalue;
  return (((str == estr) || ((long) ((unsigned int) lvalue) != lvalue))
          ? MagickFail : MagickPass);
}

MagickExport MagickPassFail MagickAtoLChk(const char *str, long *value)
{
  char *estr=0;
  *value=strtol(str,&estr, 10);
  if (str == estr)
    *value=0L;
  return (str == estr ? MagickFail : MagickPass);
}

MagickExport MagickPassFail MagickAtoULChk(const char *str, unsigned long *value)
{
  char *estr=0;
  long lvalue;
  lvalue=strtol(str,&estr, 10);
  if ((str == estr) || ((long) ((unsigned long) lvalue) != lvalue))
    lvalue=0L;
  *value=(unsigned long) lvalue;
  return (((str == estr) || ((long) ((unsigned long) lvalue) != lvalue)) ?
          MagickFail : MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M a g i c k F m i n                                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickFmin emulates C'99 fmin(), which returns the minimum of two
%  double values.  This is implemented as a function rather than a macro.
%
%  The format of the MagickFmin method is:
%
%      double MagickFmin(const double x, const double y)
%
%  A description of each parameter follows.
%
%   o  x: first input value
%
%   o  y: second input value
%
%   o  returns: minimum of values x or y
%
%
*/
double MagickFmin(const double x, const double y)
{
  return Min(x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M a g i c k F m a x                                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickFmax emulates C'99 fmax(), which returns the maximum of two
%  double values.  This is implemented as a function rather than a macro.
%
%  The format of the MagickFmax method is:
%
%      double MagickFmax(const double x, const double y)
%
%  A description of each parameter follows.
%
%   o  x: first input value
%
%   o  y: second input value
%
%   o  returns: maximum of values x or y
%
%
*/
double MagickFmax(const double x, const double y)
{
  return Max(x,y);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M a g i c k F o r m a t S t r i n g                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickFormatString prints formatted output of a variable
%  argument list buffer, limiting its output to a specified buffer size.
%
%  The format of the MagickFormatString method is:
%
%      void MagickFormatString(char *string,const size_t length,
%                              const char *format,...)
%
%  A description of each parameter follows.
%
%   o  string:  Method FormatString returns the formatted string in this
%      character buffer. Buffer must be at least MaxTextExtent characters
%      in size.
%
%   o  length:  Maximum number of characters to write into buffer, not
%      including a terminating null byte.  The result is always terminated
%      with a null byte.
%
%   o  format:  A string describing the format to use to write the remaining
%      arguments.
%
%
*/
MagickExport void MagickFormatStringList(char *string,
                                         const size_t length,
                                         const char *format, 
                                         va_list operands)
{
#if defined(HAVE_VSNPRINTF)
  (void) vsnprintf(string,length,format,operands);
#else
  (void) vsprintf(string,format,operands);
#endif
}
MagickExport void MagickFormatString(char *string,
                                     const size_t length,
                                     const char *format,...)
{
  va_list
    operands;

  va_start(operands,format);
  MagickFormatStringList(string, length, format, operands);
  va_end(operands);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k G e t T o k e n                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickGetToken gets a token from the token stream.  A token is defined
%  as sequence of characters delimited by whitespace (e.g. clip-path), a
%  sequence delimited with quotes (.e.g "Quote me"), or a sequence enclosed
%  in parenthesis (e.g. rgb(0,0,0)).
%
%  The format of the MagickGetToken method is:
%
%      void MagickGetToken(const char *start,char **end,char *token,
%                          const size_t buffer_length)
%
%  A description of each parameter follows:
%
%    o start: the start of the token sequence.
%
%    o end: point to the end of the token sequence (may be NULL).
%
%    o token: copy the token to this buffer.
%
%    o buffer_length: size of the token buffer.  If the token exceeds
%             the buffer size, the token will be truncated, but the
%             parser will still update the end pointer as if the
%             truncation did not occur.
%
%    o returns: Size of the consumed token, not including a terminating
%             null character.  If this is larger or equal to the buffer
%             size then truncation has occured.
%
*/
MagickExport size_t MagickGetToken(const char *start,char **end,char *token,
                                   const size_t buffer_length)
{
  register char
    *p;

  register size_t
    i;

  register size_t
    length = buffer_length - 1;

  double
    double_val;

  assert(start != (const char *) NULL);
  assert(token != (char *) NULL);

  i=0;
  p=(char *) start;

  if (*p != '\0')
  {
    while (isspace((int)(unsigned char) (*p)) && (*p != '\0'))
      p++;
    switch (*p)
    {
      case '"':
      case '\'':
      case '{':
      {
        register char
          escape;

        escape=(*p);
        if (escape == '{')
          escape='}';
        for (p++; *p != '\0'; p++)
        {
          if ((*p == '\\') && ((*(p+1) == escape) || (*(p+1) == '\\')))
            p++;
          else
            if (*p == escape)
              {
                p++;
                break;
              }
          if (i < length)
            token[i++]=(*p);
        }
        break;
      }
      default:
      {
        char
          *q;

        double_val=strtod(p,&q);
        (void) double_val;
        if (p != q)
          {
            for ( ; p < q; p++)
              if (i < length)
                token[i++]=(*p);
            if (*p == '%')
              if (i < length)
                {
                  token[i++]=(*p);
                  p++;
                }
            break;
          }
        if ((*p != '\0') && !isalpha((int) *p) && (*p != *DirectorySeparator) &&
	    (*p != '#') && (*p != '<'))
          {
            if (i < length)
              {
                token[i++]=(*p);
                p++;
              }
            break;
          }
        for ( ; *p != '\0'; p++)
        {
          if ((isspace((int)(unsigned char) *p) || (*p == '=')) && (*(p-1) != '\\'))
            break;
          if (i < length)
            token[i++]=(*p);
          if (*p == '(')
            for (p++; *p != '\0'; p++)
            {
              if (i < length)
                token[i++]=(*p);
              if ((*p == ')') && (*(p-1) != '\\'))
                break;
            }
        }
        break;
      }
    }
  }
  token[i]='\0';
  {
    char
      *r;

    /*
      Parse token in form "url(#%s)"
    */
    if ((LocaleNCompare(token,"url(#",5) == 0) &&
        ((r = strrchr(token,')')) != NULL))
    {
      *r='\0';
      (void) memmove(token,token+5,r-token+1);
    }
  }
  if (end != (char **) NULL)
    *end=p;
  return (p-start+1);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k R a n d R e e n t r a n t                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickRandReentrant() is a reentrant version of the standard
%  rand() function but which allows the user to pass a pointer to the
%  'seed'.  Values returned are in the range of 0 - RAND_MAX.
%
%  This function is deprecated, and scheduled for eventual removal.
%
%  The format of the MagickRandReentrant method is:
%
%      int MagickRandReentrant(unsigned int *seed)
%
%  A description of each parameter follows:
%
%    o seed: The random sequence seed value.  Initialized by the user
%            once (e.g. with output from MagickRandNewSeed()) and then
%            passed via pointer thereafter.  If seed is NULL then
%            this function behaves identically to rand(), using the
%            global seed value set via srand().
%
*/
MagickExport int MagickRandReentrant(unsigned int *seed)
{
  ARG_NOT_USED(seed);

  return (int) ((double) RAND_MAX*MagickRandomReal()+0.5);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k R a n d N e w S e e d                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickRandNewSeed() returns a semi-random initial seed value for
%  use with MagickRandReentrant() or rand().
%
%  This function is deprecated, and scheduled for eventual removal.
%
%  The format of the MagickRandNewSeed method is:
%
%      unsigned int MagickRandNewSeed(void)
%
*/
MagickExport unsigned int MagickRandNewSeed(void)
{
  return (unsigned int) MagickRandomInteger();
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S i z e S t r T o I n t 6 4                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSizeStrToInt64() converts a numeric string expressed using a scaling
%  suffix (e.g. "100K" is 100 kilo) to a 64-bit integer type.  Even though
%  this function returns a signed type, it is intended to be used to obtain
%  positive size values so a negative return value indicates an error.
%  Specfically, -1 is returned if there is known to be an conversion error.
%
%  Binary Prefixes: http://en.wikipedia.org/wiki/Binary_prefix
%
%  SI Prefixes: http://en.wikipedia.org/wiki/SI_prefix
%
%  The format of the MagickSizeStrToInt64 method is:
%
%      magick_int64_t MagickSizeStrToInt64(const char *str,
%                                          const unsigned int kilo)
%
%  A description of each parameter follows:
%
%    o str: Input string to convert
%
%    o kilo: The unit of "kilo".  Should be either 1000 (SI units)
%            or 1024 (Binary units).
%
*/
MagickExport magick_int64_t MagickSizeStrToInt64(const char *str,
                                                 const unsigned int kilo)
{
  char
    *end;

  magick_int64_t
    result;

  MagickPassFail
    status;

  result=-1;
  end=(char *) NULL;
  if ((status=MagickStrToInt64(str,&end,&result)) == MagickPass)
    {
      int
        c,
        mult;

      c='\0';
      if (end != (char *) NULL)
        c=*end;
      mult=0;

      switch (tolower(c))
        {
        default: break;
        case 'k': mult=1; break; /* kilo, 10^3 */
        case 'm': mult=2; break; /* mega, 10^6 */
        case 'g': mult=3; break; /* giga, 10^9 */
        case 't': mult=4; break; /* tera, 10^12 */
        case 'p': mult=5; break; /* peta, 10^15 */
        case 'e': mult=6; break; /* exa,  10^18 */
        }

      while (mult-- > 0)
        result *= kilo;
    }

  return result;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S p a w n V P                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSpawnVP() executes an external command with arguments provided by
%  an argument vector.  The return status of the executed command is returned
%  if it is executed, or -1 is returned if the command could not be executed.
%  Executed commands will normally return zero if they execute without error.
%
%  The format of the MagickSpawnVP method is:
%
%      int MagickSpawnVP(const char *file, char *const argv[])
%
%  A description of each parameter follows:
%
%    o file:  Name of the command to execute.
%
%    o argv:  Argument vector. First argument in the vector should be
%             the name of the command.  The argument vector is terminated
%             via a NULL pointer.
%
*/
MagickExport int
MagickSpawnVP(const unsigned int verbose,const char *file, char *const argv[])
{
  int
    status;

  char
    message[MaxTextExtent];


  status = -1;
  message[0]='\0';
  errno=0;

  assert(file != (const char *) NULL);

  if (strlen(file) == 0)
    return -1;

  {
    /*
      Verify that we are allowed to run this program.
    */
    ExceptionInfo
      exception;
    
    GetExceptionInfo(&exception);
    if (MagickConfirmAccess(FileExecuteConfirmAccessMode,argv[0],&exception)
	== MagickFail)
      {
	errno=EPERM;
	DestroyExceptionInfo(&exception);
	return -1;
      }
  }

#if defined(HAVE_SPAWNVP)
  {
    /* int spawnvp(int mode, const char *path, const char * const *argv); */
    status = spawnvp(_P_WAIT, file, (const char * const *) argv);
  }
#else
  {
    pid_t
      child_pid;

    child_pid = fork( );
    if ( (pid_t)-1 == child_pid)
      {
	/* Failed to fork, errno contains reason */
	status = -1;
	FormatString(message,"fork failed: %.1024s", strerror(errno));
      }
    else if ( 0 == child_pid )
      {
	/* We are the child process, exec program with arguments. */
	status = execvp(file, argv);

	/* If we get here, then execvp must have failed. */
	(void) fprintf(stderr, "execvp failed, errno = %d (%s)\n",errno,strerror(errno));

	/* If there is an execvp error, then call _exit() */
	_exit(1);
      }
    else
      {
	/* We are the parent process, wait for child. */
	pid_t waitpid_status;
	int child_status = 0;
	waitpid_status = waitpid(child_pid, &child_status, 0);
	if ( (pid_t)-1 == waitpid_status )
	  {
	    /* Waitpid error */
	    status = -1;
	    FormatString(message, "waitpid failed: %.1024s", strerror(errno));
	  }
	else if ( waitpid_status == child_pid )
	  {
	    /* Status is available for child process */
	    if ( WIFEXITED( child_status ) )
	      {
		status =  WEXITSTATUS( child_status );
	      }
	    else if ( WIFSIGNALED( child_status ) )
	      {
		int sig_num = WTERMSIG( child_status );
		status = -1;
		FormatString(message, "child process quit due to signal %d", sig_num);
	      }
	  }
      }
  }
#endif

  /*
    Provide a verbose/dignostic message in a form which is easy for
    the user to understand.
  */
  if (verbose || (status != 0))
    {
      const char
	*message_p = (const char *) NULL;

      char
	*command;

      unsigned int
	i;

      command = AllocateString((const char*) NULL);
      for (i = 0; argv[i] != (const char*) NULL; i++)
	{
	  char
	    buffer[MaxTextExtent];

	  FormatString(buffer,"\"%.1024s\"", argv[i]);

	  if (0 != i)
	    (void) ConcatenateString(&command," ");

	  (void) ConcatenateString(&command,buffer);
	}
      if (message[0] != '\0')
	message_p = message;
      MagickError2(DelegateError,command,message_p);
      MagickFreeMemory(command);
    }

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S t r i p S p a c e s F r o m S t r i n g                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickStripSpacesFromString() compacts a NULL-terminated string in-place
%  by removing white space (space and tab) characters) from a string.
%
%  The format of the MagickStripSpacesFromString method is:
%
%      size_t MagickStripSpacesFromString(char *string)
%
%  A description of each parameter follows:
%
%    o string: String buffer to compact.
%
%    o returns: New string length
%
*/
MagickExport size_t
MagickStripSpacesFromString(char *string)
{
  register char
    *w;

  register const char
    *r;

  for (w=string, r=string; *r != '\0'; )
    {
      if ((*r == ' ') || (*r == '\t'))
        {
          r++;
          continue;
        }
      if (r != w)
        *w = *r;
      r++;
      w++;
    }
  *w='\0';
  return (size_t) (w - string);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S t r T o I n t 6 4                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickStrToInt64() converts a string to a 64-bit integer type.  MagickPass
%  is returned if the conversion succeeds, and MagickFail is returned if
%  the conversion fails.
%
%  The format of the MagickStrToInt64 method is:
%
%      MagickPassFail MagickStrToInt64(const char *start,char **end,
%                                      magick_int64_t *value)
%
%  A description of each parameter follows:
%
%    o start: Start of string
%
%    o end: Pointer to update with address where parsing stopped.
%
%    o value: Pointer to value to update
%
*/
static MagickPassFail MagickStrToInt64(const char *start,char **end,
                                       magick_int64_t *value)
{
  magick_int64_t
    result;

  errno=0;
  result=MagickStrToL64(start,end,10);
  if (errno == 0)
    *value=result;

  return (errno == 0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C r e a t e D i r e c t o r y P a t h                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickCreateDirectoryPath() creates the specified directory path, creating
%  parent directories as required.  MagickPass is returned on success, and
%  MagickFail is returned if there is a failure, with error information set
%  in the user-provided ExceptionInfo structure.
%
%  The format of the MagickCreateDirectoryPath method is:
%
%      MagickPassFail MagickCreateDirectoryPath(const char *dir,
%                                               ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o dir: Path to create.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport MagickPassFail MagickCreateDirectoryPath(const char *dir,
                                                      ExceptionInfo *exception)
{
  char
    path[MaxTextExtent];

  size_t
    dir_len;

  int
    status = MagickPass;

  const char
    *end,
    *p;

  unsigned int
    directory_mode;

#if defined(S_IRWXU)
  directory_mode = S_IRWXU;
#else
  directory_mode = 0777;
#endif
#if defined(S_IRGRP)
  directory_mode |= S_IRGRP;
#endif
#if defined(S_IXGRP)
  directory_mode |= S_IXGRP;
#endif
#if defined(S_IROTH)
  directory_mode |= S_IROTH;
#endif
#if defined(S_IXOTH)
  directory_mode |= S_IXOTH;
#endif

  dir_len = strlen(dir);
  end = dir + dir_len;

  /*
    Walk back to find part of path which already exists.
  */
  for (p = end; p > dir ; p--)
    {
      if ((p == end) || (DirectorySeparator[0] == *p))
        {
          (void) strlcpy(path,dir,p-dir+1);
          if (IsAccessibleNoLogging(path))
            break;
        }
    }

  if (p != end)
    {
      /*
        Create part of path which does not already exist.
      */
      for ( p++; p <= end ; p++)
        {
          if ((*p == '\0') || (DirectorySeparator[0] == *p))
            {
              (void) strlcpy(path,dir,p-dir+1);
              if (-1 == mkdir(path,directory_mode))
                {
                  if (EEXIST != errno)
                    {
                      /*
                        Throw exception.
                      */
                      ThrowException2(exception,FileOpenError,dir,strerror(errno));
                      status = MagickFail;
                      break;
                    }
                }
              errno = 0;
            }
        }
    }
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S c e n e F i l e N a m e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSceneFileName() uses a filename template and scene number
%  in order to produce a unique filename for the scene.  If force is
%  MagickFalse, then a substition is only performed if the template
%  contains a valid substitution specification.  If force is MagickTrue,
%  then the additional scene template is applied so that the generated
%  filename is assured to be distinguished by the scene number.
%
%  The format of the MagickSceneFileName method is:
%
%      MagickBool MagickSceneFileName(char *filename, const char* template,
%                                     unsigned long scene)
%
%  A description of each parameter follows:
%
%    o filename: Buffer to update with generated filename.  Buffer must
%                be at least MaxTextExtent bytes long.
%
%    o filename_template: Filename generation template (e.g. "image%02d.miff").
%
%    o scene_template: Template for scene part which is appended to
%                template if template does not contain a scene format
%                specification  (e.g. ".%lu" or "[%lu]").
%
%    o force:    If there is no embedded template in filename, then apply
%                the scene template.
%
%    o scene:    Scene number.
%
*/
MagickExport MagickBool MagickSceneFileName(char *filename,
                                            const char* filename_template,
                                            const char* scene_template,
                                            const MagickBool force,
                                            unsigned long scene)
{
  const char
    *p;

  MagickBool
    status;

  status = MagickFalse;
  (void) strlcpy(filename,filename_template,MaxTextExtent);
  p=strchr(filename_template,'%');
  if ((p != (char *) NULL) && ((strchr(p+1,'%') == (char *) NULL)))
    {
      for ( p=p+1; *p ; p++)
        {
          register const int c = *p;
          if ('d' == c)
            {
              FormatString(filename,filename_template,scene);
              break;
            }
          if (!isdigit(c))
            {
              status = MagickFalse;
              break;
            }
        }
    }

  if ((force) && (LocaleCompare(filename,filename_template) == 0))
    {
      char format[MaxTextExtent];
      (void) strlcpy(format,"%.1024s",sizeof(format));
      (void) strlcat(format,scene_template,sizeof(format));
      FormatString(filename,format,filename_template,scene);
    }
  if (LocaleCompare(filename,filename_template) != 0)
    status = MagickTrue;

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M a g i c k S t r l C a t                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickStrlCat appends the NULL-terminated string src to the end
%  of dst.  It will append at most size - strlen(dst) - 1 bytes, NULL-
%  terminating the result. If size is zero, then the result is not NULL
%  terminated. The total length of the string which would have been created
%  given sufficient buffer size (may be longer than size) is returned.  This
%  function substitutes for strlcat() which is available under FreeBSD,
%  Apple's OS-X, and Solaris 8.
%
%  Buffer overflow can be checked as  follows:
%
%    if (MagickStrlCat(dst, src, dstsize) >= dstsize)
%      return -1;
%
%  The format of the MagickStrlCat method is:
%
%      size_t MagickStrlCat(char *dst, const char *src, size_t size)
%
%  A description of each parameter follows.
%
%   o  dst:  Destination string.
%
%   o  src:  Source string.
%
%   o  size: Maximum string length, including the terminating null.
%
*/
MagickExport size_t MagickStrlCat(char *dst, const char *src, const size_t size)
{
  size_t
    length;

  const char
    *q;

  assert(dst != NULL);
  assert(src != (const char *) NULL);
  assert(size >= 1);

  length=strlen(dst);

  /*
    Copy remaining characters from src while constraining length to
    size - 1.
  */
  q = src;
  if (size >= 1)
    {
      char
	*p;

      for ( p = dst + length ;
	    (*q != 0) && (length < size - 1) ;
	    length++, p++, q++ )
	*p = *q;
      
      dst[length]='\0';
    }

  /*
    Add remaining length of src to length.
  */
  while (*q++)
    length++;

  return length;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M a g i c k S t r l C p y                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickStrlCpy copies up to size - 1 characters from the NULL-
%  terminated string src to dst, NULL-terminating the result. If size is
%  zero, then the result is not NULL terminated.  The total length of the
%  string which would have been created given sufficient buffer size (may
%  be longer than size) is returned. This function is simlar to strlcpy()
%  which is available under FreeBSD, Apple's OS-X, and Solaris 8 except
%  that it is assured to work with overlapping objects.  FreeBSD does not
%  document if strlcpy() handles overlapping objects, but Solaris strlcpy()
%  does not.
%
%  Buffer overflow can be checked as  follows:
%
%    if (MagickStrlCpy(dst, src, dstsize) >= dstsize)
%      return -1;
%
%  The format of the MagickStrlCpy method is:
%
%      size_t MagickStrlCpy(char *dst, const char *src, size_t size)
%
%  A description of each parameter follows.
%
%   o  dst:  Destination string.
%
%   o  src:  Source string.
%
%   o  size: Maximum string length, including the terminating null.
%
*/
MagickExport size_t MagickStrlCpy(char *dst, const char *src, const size_t size)
{
  size_t
    length;

  const char
    *q;

  assert(dst != NULL);
  assert(src != (const char *) NULL);
  assert(size >= 1);
  /* assert(((dst+size) <= src) || (dst >= (src+size))); */


  /*
    Copy src to dst within bounds of size-1.
  */
  length=0;
  q=src;
  if (size >= 1)
    {
      char
	*p;

      for ( p=dst ;
	    (*q != 0) && (length < size-1) ;
	    length++, p++, q++ )
	*p = *q;
      
      dst[length]='\0';
    }

  /*
    Add remaining length of src to length.
  */
  while (*q++)
    length++;

  return length;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M a g i c k S t r l C p y T r u n c                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickStrlCpyTrunc copies up to size - 1 characters from the NULL-
%  terminated string src to dst, NULL-terminating the result.  If size is
%  zero, then the result is not NULL terminated.  The number of bytes copied
%  (not including the terminating NULL) is returned.  This function is a
%  useful alternative to using MagickStrlCpy() when the actual size copied
%  is more useful than knowledge that truncation occured.
%
%  The format of the MagickStrlCat method is:
%
%      size_t MagickStrlCpyTrunc(char *dst, const char *src, size_t size)
%
%  A description of each parameter follows.
%
%   o  dst:  Destination string.
%
%   o  src:  Source string.
%
%   o  size: Maximum string length, including the terminating null.
%
*/
MagickExport size_t MagickStrlCpyTrunc(char *dst, const char *src, const size_t size)
{
  size_t
    length;

  const char
    *q;

  assert(dst != NULL);
  assert(src != (const char *) NULL);
  assert(size >= 1);

  /*
    Copy src to dst within bounds of size-1.
  */
  length=0;
  if (size >= 1)
    {
      char
	*p;

      for ( p=dst, q=src;
	    (*q != 0) && (length < size-1) ;
	    length++, p++, q++ )
	*p = *q;
      
      dst[length]='\0';
    }

  return length;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M u l t i l i n e C e n s u s                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MultilineCensus returns the number of lines within a label.  A line
%  is represented by a \n character.
%
%  The format of the MultilineCenus method is:
%
%      unsigned long MultilineCensus(const char *label)
%
%  A description of each parameter follows.
%
%   o  label:  This character string is the label.
%
%
*/
MagickExport unsigned long MultilineCensus(const char *label)
{
  long
    number_lines;

  /*
    Determine the number of lines within this label.
  */
  if (label == (char *) NULL)
    return(0);
  for (number_lines=1; *label != '\0'; label++)
    if (*label == '\n')
      number_lines++;
  return(number_lines);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t C l i e n t F i l e n a m e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetClientFilename sets the client filename if the name is specified.
%  Otherwise the current client filename is returned. On a UNIX system the
%  client name and filename are often the same since file extensions are not
%  very important, but on windows the distinction if very important.
%
%  The format of the SetClientFilename method is:
%
%      char *SetClientFilname(const char *name)
%
%  A description of each parameter follows:
%
%    o client_name: Method SetClientFilename returns the current client name.
%
%    o status: Specifies the new client name.
%
%
*/
MagickExport const char *GetClientFilename(void)
{
  return(SetClientFilename((char *) NULL));
}

MagickExport const char *SetClientFilename(const char *name)
{
  static char
    client_filename[MaxTextExtent] = "";

  if ((name != (char *) NULL) && (*name != '\0'))
    {
      (void) strlcpy(client_filename,name,sizeof(client_filename));
      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
        "Client Filename was set to: %s",client_filename);
    }
  return(client_filename);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t C l i e n t N a m e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetClientName sets the client name if the name is specified.
%  Otherwise the current client name is returned.
%
%  The format of the SetClientName method is:
%
%      char *SetClientName(const char *name)
%
%  A description of each parameter follows:
%
%    o client_name: Method SetClientName returns the current client name.
%
%    o status: Specifies the new client name.
%
%
*/
MagickExport const char *GetClientName(void)
{
  return(SetClientName((char *) NULL));
}

MagickExport const char *SetClientName(const char *name)
{
  static char
    client_name[MaxTextExtent] = "Magick";

  if ((name != (char *) NULL) && (*name != '\0'))
    {
      (void) strlcpy(client_name,name,sizeof(client_name));
      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
        "Client Name was set to: %s",client_name);
    }
  return(client_name);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t C l i e n t P a t h                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetClientPath sets the client path if the name is specified.
%  Otherwise the current client path is returned. A zero-length string
%  is returned if the client path has never been set.
%
%  The format of the SetClientPath method is:
%
%      char *SetClientPath(const char *path)
%
%  A description of each parameter follows:
%
%    o client_path: Method SetClientPath returns the current client path.
%
%    o status: Specifies the new client path.
%
%
*/
MagickExport const char *GetClientPath(void)
{
  return(SetClientPath((char *) NULL));
}

MagickExport const char *SetClientPath(const char *path)
{
  static char
    client_path[MaxTextExtent] = "";

  if ((path != (char *) NULL) && (*path != '\0'))
    {
      (void) strlcpy(client_path,path,sizeof(client_path));
      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
        "Client Path was set to: %s",path);
    }
  return(client_path);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t G e o m e t r y                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetGeometry sets a grometry to its default values.
%
%  The format of the SetGeometry method is:
%
%      SetGeometry(const Image *image,RectangleInfo *geometry)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o geometry: The geometry.
%
%
*/
MagickExport void SetGeometry(const Image *image,RectangleInfo *geometry)
{
  assert(image != (Image *) NULL);
  assert(geometry != (RectangleInfo *) NULL);
  (void) memset(geometry,0,sizeof(RectangleInfo));
  geometry->width=image->columns;
  geometry->height=image->rows;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  S t r i n g T o A r g v                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method StringToArgv converts a text string into command line arguments.
%
%  The format of the StringToArgv method is:
%
%      char **StringToArgv(const char *text,int *argc)
%
%  A description of each parameter follows:
%
%    o argv:  Method StringToArgv returns the string list unless an error
%      occurs, otherwise NULL.
%
%    o text:  Specifies the string to segment into a list.
%
%    o argc:  This integer pointer returns the number of arguments in the
%      list.
%
%
*/
MagickExport char **StringToArgv(const char *text,int *argc)
{
  char
    **argv;

  register char
    *p,
    *q;

  register long
    i;

  *argc=0;
  if (text == (char *) NULL)
    return((char **) NULL);
  /*
    Determine the number of arguments.
  */
  for (p=(char *) text; *p != '\0'; )
  {
    while (isspace((int)(unsigned char) (*p)))
      p++;
    (*argc)++;
    if (*p == '"')
      for (p++; (*p != '"') && (*p != '\0'); p++);
    if (*p == '\'')
      for (p++; (*p != '\'') && (*p != '\0'); p++);
    while (!isspace((int)(unsigned char) (*p)) && (*p != '\0'))
      p++;
  }
  (*argc)++;
  argv=MagickAllocateMemory(char **,(*argc+1)*sizeof(char *));
  if (argv == (char **) NULL)
    {
      MagickError3(ResourceLimitError,MemoryAllocationFailed,
        UnableToConvertStringToTokens);
      return((char **) NULL);
    }
  /*
    Convert string to an ASCII list.
  */
  argv[0]=AllocateString("magick");
  p=(char *) text;
  for (i=1; i < *argc; i++)
  {
    while (isspace((int)(unsigned char) (*p)))
      p++;
    q=p;
    if (*q == '"')
      {
        p++;
        for (q++; (*q != '"') && (*q != '\0'); q++);
      }
    else
      if (*q == '\'')
        {
          for (q++; (*q != '\'') && (*q != '\0'); q++);
          q++;
        }
      else
        while (!isspace((int)(unsigned char) (*q)) && (*q != '\0'))
          q++;
    argv[i]=MagickAllocateMemory(char *,(size_t) (q-p+MaxTextExtent));
    if (argv[i] == (char *) NULL)
      {
        int
          j;

        MagickError3(ResourceLimitError,MemoryAllocationFailed,
          UnableToConvertStringToTokens);

        /*
          Deallocate allocated data and return.
        */
        for (j=0; j<i; j++)
          MagickFreeMemory(argv[j]);
        MagickFreeMemory(argv);
        return((char **) NULL);
      }
    (void) strlcpy(argv[i],p,q-p+1);
    p=q;
    while (!isspace((int)(unsigned char) (*p)) && (*p != '\0'))
      p++;
  }
  argv[i]=(char *) NULL;
  return(argv);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  S t r i n g T o D o u b l e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method StringToDouble() converts a text string to a double.  If the string
%  contains a percent sign (e.g. 50%) that percentage of the interval is
%  returned.
%
%  The format of the StringToDouble method is:
%
%      double StringToDouble(const char *text,const double interval)
%
%  A description of each parameter follows:
%
%    o value:  Method StringToDouble returns the converted value.
%
%    o text:  Specifies the string to segment into a list.
%
%    o interval:  Specifies the interval; used for obtaining a percentage.
%
%
*/
MagickExport double StringToDouble(const char *text,const double interval)
{
  char
    *q;

  double
    value;

  if (MagickStrToD(text,&q,&value) == 0)
    return 0.0;
  if (strchr(q,'%') != (char *) NULL)
    value*=interval/100.0;
  return(value);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  S t r i n g T o L i s t                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method StringToList converts a text string into a list by segmenting the
%  text string at each carriage return discovered.  The list is converted to
%  HEX characters if any control characters are discovered within the text
%  string.
%
%  The format of the StringToList method is:
%
%      char **StringToList(const char *text)
%
%  A description of each parameter follows:
%
%    o list:  Method StringToList returns the string list unless an error
%      occurs, otherwise NULL.
%
%    o text:  Specifies the string to segment into a list.
%
%
*/
MagickExport char **StringToList(const char *text)
{
  char
    **textlist;

  register char
    *q;

  register const char
    *p;

  register long
    i;

  size_t
    lines;

  if (text == (char *) NULL)
    return((char **) NULL);
  for (p=text; *p != '\0'; p++)
    if (((unsigned char) *p < 32) && !isspace((int)(unsigned char) (*p)))
      break;
  if (*p == '\0')
    {
      /*
        Convert string to an ASCII list.
      */
      lines=1;
      for (p=text; *p != '\0'; p++)
        if (*p == '\n')
          lines++;
      textlist=MagickAllocateMemory(char **,(lines+MaxTextExtent)*sizeof(char *));
      if (textlist == (char **) NULL)
        MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
          UnableToConvertText);
      p=text;
      for (i=0; i < (long) lines; i++)
      {
        for (q=(char *) p; *q != '\0'; q++)
          if ((*q == '\r') || (*q == '\n'))
            break;
        textlist[i]=MagickAllocateMemory(char *,(size_t) (q-p+MaxTextExtent));
        if (textlist[i] == (char *) NULL)
          MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
            UnableToConvertText);
	/*
	  Don't use strlcpy here because it spends too much time
	  looking for the trailing null in order to report the
	  characters not copied.
	*/
	(void) strncpy(textlist[i],p,q-p);
        textlist[i][q-p]='\0';
        if (*q == '\r')
          q++;
        p=q+1;
      }
    }
  else
    {
      char
        hex_string[MaxTextExtent];

      register long
        j;

      /*
        Convert string to a HEX list.
      */
      lines=(strlen(text)/0x14)+1;
      textlist=MagickAllocateMemory(char **,(lines+MaxTextExtent)*sizeof(char *));
      if (textlist == (char **) NULL)
        MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
          UnableToConvertText);
      p=text;
      for (i=0; i < (long) lines; i++)
      {
        textlist[i]=MagickAllocateMemory(char *,2*MaxTextExtent);
        if (textlist[i] == (char *) NULL)
          MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
            UnableToConvertText);
        FormatString(textlist[i],"0x%08lx: ",0x14*i);
        q=textlist[i]+strlen(textlist[i]);
        for (j=1; j <= (long) Min(strlen(p),0x14); j++)
        {
          FormatString(hex_string,"%02x",*(p+j));
          (void) strlcpy(q,hex_string,MaxTextExtent);
          q+=2;
          if ((j % 0x04) == 0)
            *q++=' ';
        }
        for (; j <= 0x14; j++)
        {
          *q++=' ';
          *q++=' ';
          if ((j % 0x04) == 0)
            *q++=' ';
        }
        *q++=' ';
        for (j=1; j <= (long) Min(strlen(p),0x14); j++)
        {
          if (isprint((int)(unsigned char)(*p)))
            *q++=(*p);
          else
            *q++='-';
          p++;
        }
        *q='\0';
      }
    }
  textlist[i]=(char *) NULL;
  return(textlist);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S t r i p                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Strip strips any whitespace or quotes from the beginning and end of
%  a string of characters.
%
%  The format of the Strip method is:
%
%      void Strip(char *message)
%
%  A description of each parameter follows:
%
%    o message: Specifies an array of characters.
%
%
*/
MagickExport void Strip(char *message)
{
  register char
    *p,
    *q;

  assert(message != (char *) NULL);
  if (*message == '\0')
    return;
  if (strlen(message) == 1)
    return;
  p=message;
  while (isspace((int)(unsigned char) (*p)))
    p++;
  if ((*p == '\'') || (*p == '"'))
    p++;
  q=message+strlen(message)-1;
  while (isspace((int)(unsigned char) (*q)) && (q > p))
    q--;
  if (q > p)
    if ((*q == '\'') || (*q == '"'))
      q--;
  (void) memmove(message,p,q-p+1);
  message[q-p+1]='\0';
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S u b s t i t u t e S t r i n g                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SubstituteString() performs string substitution on a buffer, replacing
%  the buffer with the substituted version. Buffer must be allocated from
%  the heap since it may be reallocated (as required). MagickTrue is returned
%  if a replacement was made.
%
%  The format of the SubstituteString method is:
%
%      MagickBool SubstituteString(char **buffer,const char* search,
%        const char *replace)
%
%  A description of each parameter follows:
%
%    o buffer: The buffer to perform replacements on. Replaced with new
%      allocation if a replacement is made.
%
%    o search: String to search for.
%
%    o replace: Replacement string.
%
*/
MagickExport MagickBool
SubstituteString(char **buffer,const char *search,const char *replace)
{
  register char
    *p=*buffer;

  register size_t
    i;

  size_t
    search_len=0,
    replace_len=0;

  MagickBool
    replaced=MagickFalse;

  search_len=strlen(search);
  p=*buffer;
  for (i=0; p[i] != '\0'; i++)
    {
      if ((p[i] == search[0]) && (strncmp(&p[i],search,search_len) == 0))
	{
	  if (0 == replace_len)
	    replace_len=strlen(replace);
	  if (replace_len > search_len)
	    {
	      size_t
		allocation_len;

	      allocation_len=strlen(p)+(replace_len-search_len)+1;
	      MagickRoundUpStringLength(allocation_len);
	      MagickReallocMemory(char *,p,allocation_len);
	      *buffer=p;
	      if (p == (char *) NULL)
		MagickFatalError3(ResourceLimitFatalError,
				  MemoryAllocationFailed,
				  UnableToAllocateString);
	    }
	  if (search_len != replace_len)
	    (void) MagickCloneMemory(&p[i+replace_len],&p[i+search_len],
				     strlen(&p[i+search_len])+1);
	  (void) MagickCloneMemory(&p[i],replace,replace_len);
	  i += replace_len;
	  replaced=MagickTrue;
	}
    }
  return replaced;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S y s t e m C o m m a n d                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SystemCommand executes the specified command and waits until it
%  terminates.  The returned value is the exit status of the command.
%
%  The format of the SystemCommand method is:
%
%      int SystemCommand(const unsigned int verbose,const char *command)
%
%  A description of each parameter follows:
%
%    o status: Method SystemCommand returns False if the command is
%      executed successfully.
%
%    o verbose: An unsigned integer other than 0 prints the executed
%      command before it is invoked.
%
%    o command: This string is the command to execute.
%
%
*/
MagickExport int SystemCommand(const unsigned int verbose,const char *command)
{
  int
    status;

#if defined(POSIX)
  char
    message[MaxTextExtent];
#endif /* POSIX */

  const char
    *message_p = (const char *) NULL;

  {
    /*
      Verify that we are allowed to run this program.
    */
    ExceptionInfo
      exception;

    char
      *end,
      program[MaxTextExtent];

    GetExceptionInfo(&exception);
    end=(char *) NULL;
    program[0]='\0';
    MagickGetToken(command,&end,program,MaxTextExtent);
    if (MagickConfirmAccess(FileExecuteConfirmAccessMode,program,&exception)
	== MagickFail)
      {
	errno=EPERM;
	DestroyExceptionInfo(&exception);
	return -1;
      }
  }

  errno=0;
#if defined(POSIX)
  status=system(command);
  if (status == 1)
    {
      (void) strlcpy(message,strerror(status),sizeof(message));
      message_p=message;
    }
  else if (WIFSIGNALED(status))
    {
      FormatString(message,"terminated due to signal %d",
                   WTERMSIG(status));
      message[sizeof(message)-1]='\0';
      message_p=message;
    }
#elif defined(MSWINDOWS)
  status=NTSystemComman(command);
  if (!status)
    message_p=strerror(status);
#else
#  error Do not know how to run system commands.
#endif
  if (verbose || (status != 0))
    MagickError2(DelegateError,command,message_p);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T o k e n i z e r                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method Tokenizer is a generalized, finite state token parser.  It extracts
%  tokens one at a time from a string of characters.  The characters used for
%  white space, for break characters, and for quotes can be specified.  Also,
%  characters in the string can be preceded by a specifiable escape character
%  which removes any special meaning the character may have.
%
%  Here is some terminology:
%
%    o token: A single unit of information in the form of a group of
%      characters.
%
%    o white space: Space that gets ignored (except within quotes or when
%      escaped), like blanks and tabs. in addition, white space terminates a
%      non-quoted token.
%
%    o break set: One or more characters that separates non-quoted tokens.
%      Commas are a common break character. The usage of break characters to
%      signal the end of a token is the same as that of white space, except
%      multiple break characters with nothing or only white space between
%      generate a null token for each two break characters together.
%
%      For example, if blank is set to be the white space and comma is set to
%      be the break character, the line
%
%        A, B, C ,  , DEF
%
%        ... consists of 5 tokens:
%
%        1)  "A"
%        2)  "B"
%        3)  "C"
%        4)  "" (the null string)
%        5)  "DEF"
%
%    o Quote character: A character that, when surrounding a group of other
%      characters, causes the group of characters to be treated as a single
%      token, no matter how many white spaces or break characters exist in
%      the group. Also, a token always terminates after the closing quote.
%      For example, if ' is the quote character, blank is white space, and
%      comma is the break character, the following string
%
%        A, ' B, CD'EF GHI
%
%        ... consists of 4 tokens:
%
%        1)  "A"
%        2)  " B, CD" (note the blanks & comma)
%        3)  "EF"
%        4)  "GHI"
%
%      The quote characters themselves do not appear in the resultant
%      tokens.  The double quotes are delimiters i use here for
%      documentation purposes only.
%
%    o Escape character: A character which itself is ignored but which
%      causes the next character to be used as is.  ^ and \ are often used
%      as escape characters. An escape in the last position of the string
%      gets treated as a "normal" (i.e., non-quote, non-white, non-break,
%      and non-escape) character. For example, assume white space, break
%      character, and quote are the same as in the above examples, and
%      further, assume that ^ is the escape character. Then, in the string
%
%        ABC, ' DEF ^' GH' I ^ J K^ L ^
%
%        ... there are 7 tokens:
%
%        1)  "ABC"
%        2)  " DEF ' GH"
%        3)  "I"
%        4)  " "     (a lone blank)
%        5)  "J"
%        6)  "K L"
%        7)  "^"     (passed as is at end of line)
%
%  The format of the Tokenizer method is:
%
%      int Tokenizer(TokenInfo *token_info,unsigned flag,char *token,
%        size_t max_token_length,char *line,char *white,char *break_set,
%        char *quote,char escape,char *breaker,int *next,char *quoted)
%
%  A description of each parameter follows:
%
%    o flag: right now, only the low order 3 bits are used.
%
%        1 => convert non-quoted tokens to upper case
%        2 => convert non-quoted tokens to lower case
%        0 => do not convert non-quoted tokens
%
%    o token: a character string containing the returned next token
%
%    o max_token_length: the maximum size of "token".  Characters beyond
%      "max_token_length" are truncated.
%
%    o string: the string to be parsed.
%
%    o white: a string of the valid white spaces.  example:
%
%        char whitesp[]={" \t"};
%
%      blank and tab will be valid white space.
%
%    o break: a string of the valid break characters. example:
%
%        char breakch[]={";,"};
%
%      semicolon and comma will be valid break characters.
%
%    o quote: a string of the valid quote characters. An example would be
%
%        char whitesp[]={"'\"");
%
%      (this causes single and double quotes to be valid) Note that a
%      token starting with one of these characters needs the same quote
%      character to terminate it.
%
%      for example:
%
%        "ABC '
%
%      is unterminated, but
%
%        "DEF" and 'GHI'
%
%      are properly terminated.  Note that different quote characters
%      can appear on the same line; only for a given token do the quote
%      characters have to be the same.
%
%    o escape: the escape character (NOT a string ... only one
%      allowed). Use zero if none is desired.
%
%    o breaker: the break character used to terminate the current
%      token.  If the token was quoted, this will be the quote used.  If
%      the token is the last one on the line, this will be zero.
%
%    o next: this variable points to the first character of the
%      next token.  it gets reset by "tokenizer" as it steps through the
%      string.  Set it to 0 upon initialization, and leave it alone
%      after that.  You can change it if you want to jump around in the
%      string or re-parse from the beginning, but be careful.
%
%    o quoted: set to True if the token was quoted and False
%      if not.  You may need this information (for example:  in C, a
%      string with quotes around it is a character string, while one
%      without is an identifier).
%
%    o result: 0 if we haven't reached EOS (end of string), and 1
%      if we have.
%
*/

#define IN_WHITE 0
#define IN_TOKEN 1
#define IN_QUOTE 2
#define IN_OZONE 3

static long sindex(char c,char *string)
{
  register char
    *p;

  for (p=string; *p; p++)
    if (c == (*p))
      return(p-string);
  return(-1);
}

static void StoreToken(TokenInfo *token_info,char *string,
  size_t max_token_length,char c)
{
  register long
    i;

  if ((token_info->offset < 0) ||
      ((size_t) token_info->offset >= (max_token_length-1)))
    return;
  i=token_info->offset++;
  string[i]=c;
  if (token_info->state == IN_QUOTE)
    return;
  switch (token_info->flag & 0x03)
  {
    case 1:
    {
      string[i]=toupper((int) c);
      break;
    }
    case 2:
    {
      string[i]=tolower((int) c);
      break;
    }
    default:
      break;
  }
}

MagickExport int Tokenizer(TokenInfo *token_info,unsigned flag,char *token,
  size_t max_token_length,char *line,char *white,char *break_set,char *quote,
  char escape,char *breaker,int *next,char *quoted)
{
  char
    c;

  register long
    i;

  *breaker=False;
  *quoted=False;
  if (!line[*next])
    return(1);
  token_info->state=IN_WHITE;
  token_info->quote=False;
  token_info->flag=flag;
  for (token_info->offset=0; line[*next] != 0; (*next)++)
  {
    c=line[*next];
    i=sindex(c,break_set);
    if (i >= 0)
      {
        switch (token_info->state)
        {
          case IN_WHITE:
          case IN_TOKEN:
          case IN_OZONE:
          {
            (*next)++;
            *breaker=break_set[i];
            token[token_info->offset]=0;
            return(0);
          }
          case IN_QUOTE:
          {
            StoreToken(token_info,token,max_token_length,c);
            break;
          }
        }
        continue;
      }
    i=sindex(c,quote);
    if (i >= 0)
      {
        switch(token_info->state)
        {
          case IN_WHITE:
          {
            token_info->state=IN_QUOTE;
            token_info->quote=quote[i];
            *quoted=True;
            break;
          }
          case IN_QUOTE:
          {
            if (quote[i] != token_info->quote)
              StoreToken(token_info,token,max_token_length,c);
            else
              {
                token_info->state=IN_OZONE;
                token_info->quote=0;
              }
            break;
          }
          case IN_TOKEN:
          case IN_OZONE:
          {
            *breaker=c;
            token[token_info->offset]=0;
            return(0);
          }
        }
        continue;
      }
    i=sindex(c,white);
    if (i >= 0)
      {
        switch(token_info->state)
        {
          case IN_WHITE:
          case IN_OZONE:
            break;
          case IN_TOKEN:
          {
            token_info->state=IN_OZONE;
            break;
          }
          case IN_QUOTE:
          {
            StoreToken(token_info,token,max_token_length,c);
            break;
          }
        }
        continue;
      }
    if (c == escape)
      {
        if (line[(*next)+1] == 0)
          {
            *breaker=0;
            StoreToken(token_info,token,max_token_length,c);
            (*next)++;
            token[token_info->offset]=0;
            return(0);
          }
        switch(token_info->state)
        {
          case IN_WHITE:
          {
            (*next)--;
            token_info->state=IN_TOKEN;
            break;
          }
          case IN_TOKEN:
          case IN_QUOTE:
          {
            (*next)++;
            c=line[*next];
            StoreToken(token_info,token,max_token_length,c);
            break;
          }
          case IN_OZONE:
          {
            token[token_info->offset]=0;
            return(0);
          }
        }
        continue;
      }
    switch(token_info->state)
    {
      case IN_WHITE:
        {
          token_info->state=IN_TOKEN;
          StoreToken(token_info,token,max_token_length,c);
          break;
        }
      case IN_TOKEN:
      case IN_QUOTE:
      {
        StoreToken(token_info,token,max_token_length,c);
        break;
      }
      case IN_OZONE:
      {
        token[token_info->offset]=0;
        return(0);
      }
    }
  }
  token[token_info->offset]=0;
  return(0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T r a n s l a t e T e x t                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TranslateText replaces any embedded formatting characters with
%  the appropriate image attribute and returns the translated text.  See
%  the documentation for TranslateTextEx() for the available translations.
%
%  The format of the TranslateText method is:
%
%      char *TranslateText(const ImageInfo *image_info,Image *image,
%        const char *formatted_text)
%
%  A description of each parameter follows:
%
%    o translated_text:  Method TranslateText returns the translated
%      text string.
%
%    o image_info: The imageInfo (may be NULL!).
%
%    o image: The image.
%
%    o formatted_text: The address of a character string containing the embedded
%      formatting characters.
%
*/

MagickExport char *TranslateText(const ImageInfo *image_info,
                                 Image *image,
                                 const char *formatted_text)
{
  return TranslateTextEx(image_info,image,formatted_text,MagickStrlCpyTrunc);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T r a n s l a t e T e x t E x                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method TranslateTextEx replaces any embedded formatting characters with
%  the appropriate image attribute and returns the translated text, while
%  applying a text transformation for each substitution via a user provided
%  translation function.  The translation function should behave similar to
%  strlcpy() but may translate text while it is copied.
%
%  The currently supported translations are:
%
%    %b   file size
%    %c   comment
%    %d   directory
%    %e   filename extension
%    %f   filename
%    %g   page dimensions and offsets
%    %h   height
%    %i   input filename
%    %k   number of unique colors
%    %l   label
%    %m   magick
%    %n   number of scenes
%    %o   output filename
%    %p   page number
%    %q   image bit depth
%    %r   image type description
%    %s   scene number
%    %t   top of filename
%    %u   first unique temporary filename
%    %w   width
%    %x   horizontal resolution
%    %y   vertical resolution
%    %z   second unique temporary filename
%    %A   transparency supported
%    %C   compression type
%    %D   GIF disposal method
%    %G   Original width and height
%    %H   page height
%    %M   original filename specification
%    %O   page offset (x,y)
%    %P   page dimensions (width,height)
%    %Q   compression quality
%    %T   time delay (in centi-seconds)
%    %U   resolution units
%    %W   page width
%    %X   page horizontal offset (x)
%    %Y   page vertical offset (y)
%    %@   trim bounding box
%    %[a] named attribute 'a'
%    %#   signature
%    \n   newline
%    \r   carriage return
%    %%   % (literal)
%
%  The format of the TranslateTextEx method is:
%
%      char *TranslateTextEx(const ImageInfo *image_info,Image *image,
%        const char *formatted_text, MagickTextTranslate translate)
%
%  A description of each parameter follows:
%
%    o translated_text:  Method TranslateText returns the translated
%      text string.
%
%    o image_info: The imageInfo (may be NULL!).
%
%    o image: The image.
%
%    o formatted_text: The address of a character string containing the embedded
%      formatting characters.
%
%    o translate: text translation callback function
%
*/
MagickExport char *TranslateTextEx(const ImageInfo *image_info,
                                   Image *image,
                                   const char *formatted_text,
                                   MagickTextTranslate translate)
{
  char
    buffer[MaxTextExtent],
    *text,
    *translated_text;

  const ImageAttribute
    *attribute;

  register char
    *p,
    *q;

  register long
    i;

  size_t
    length,
    offset;

  assert(image != (Image *) NULL);
  if ((formatted_text == (const char *) NULL) || (*formatted_text == '\0'))
    return((char *) NULL);
  text=(char *) formatted_text;
  /*
    If text starts with '@' then try to replace it with the content of
    the file name which follows.
  */
  if ((*text == '@') && IsAccessible(text+1))
    {
      text=(char *) FileToBlob(text+1,&length,&image->exception);
      if (text == (char *) NULL)
        return((char *) NULL);
    }
  /*
    Translate any embedded format characters.
  */
  length=strlen(text)+MaxTextExtent;
  translated_text=MagickAllocateMemory(char *,length);
  if (translated_text == (char *) NULL)
    return NULL;
  /*
    FIXME: Overlapping memory detected here where memory should not be overlapping.
  */
  (void) strlcpy(translated_text,text,length);
  /* (void) memmove(translated_text,text,strlen(text)+1); */
  p=text;
  for (q=translated_text; *p != '\0'; p++)
  {
    *q='\0';
    if ((size_t) (q-translated_text+MaxTextExtent) >= length)
      {
        length<<=1;
        MagickReallocMemory(char *,translated_text,length);
        if (translated_text == (char *) NULL)
          break;
        q=translated_text+strlen(translated_text);
      }
    /*
      Process formatting characters in text.
    */
    if ((*p == '\\') && (*(p+1) == 'r'))
      {
        *q++='\r';
        p++;
        continue;
      }
    if ((*p == '\\') && (*(p+1) == 'n'))
      {
        *q++='\n';
        p++;
        continue;
      }
    if (*p != '%')
      {
        *q++=(*p);
        continue;
      }
    p++;
    switch (*p)
    {
      case 'b':
      {
        /* File size */
        FormatSize(GetBlobSize(image),buffer);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'c':
      {
        /* Comment */
        attribute=GetImageAttribute(image,"comment");
        if (attribute != (ImageAttribute *) NULL)
          {
            /* Comments may be larger than MaxTextExtent so make sure
               there is sufficient memory allocated. Make sure that
               there is at least MaxTextExtent left available after we
               have concatenated our part. */
            offset=q-translated_text;
            if ((size_t) (offset+attribute->length+1+MaxTextExtent) >= length)
              {
                length += (attribute->length+1+2*MaxTextExtent);
                MagickReallocMemory(char *,translated_text,length);
                if (translated_text == (char *) NULL)
                  break;
                q=translated_text+offset;
              }
            q+=(translate)(q,attribute->value,attribute->length+1+MaxTextExtent);
          }
        break;
      }
      case 'd':
      case 'e':
      case 'f':
      case 't':
      {
        /*
          Label segment is the base of the filename.
        */
        if (strlen(image->magick_filename) != 0)
          {
            (void) strlcpy(buffer,"",MaxTextExtent);
            switch (*p)
              {
              case 'd':
                {
                  /* Directory */
                  GetPathComponent(image->magick_filename,HeadPath,buffer);
                  break;
                }
              case 'e':
                {
                  /* Filename extension */
                  GetPathComponent(image->magick_filename,ExtensionPath,buffer);
                  break;
                }
              case 'f':
                {
                  /* Filename */
                  GetPathComponent(image->magick_filename,TailPath,buffer);
                  break;
                }
              case 't':
                {
                  /* Top of filename */
                  GetPathComponent(image->magick_filename,BasePath,buffer);
                  break;
                }
              }
            q+=(translate)(q,buffer,MaxTextExtent);
          }
        break;
      }
      case 'g':
      {
        /* page dimensions and offsets */
        FormatString(buffer,"%lux%lu%+ld%+ld",
                     image->page.width,image->page.height,
                     image->page.x,image->page.y);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'h':
      {
        /* Image height */
        FormatString(buffer,"%lu",image->rows);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'i':
      {
        /* Input filename */
        q+=(translate)(q,image->filename,MaxTextExtent);
        break;
      }
      case 'k':
      {
        /* Number of unique colors */
        if (GetPixelCachePresent(image))
          {
            FormatString(buffer,"%lu",GetNumberColors(image,(FILE *) NULL,
                                                      &image->exception));
            q+=(translate)(q,buffer,MaxTextExtent);
          }
        else
          {
            *q++='%';
            *q++=(*p);
          }
        break;
      }
      case 'l':
      {
        /* Label */
        attribute=GetImageAttribute(image,"label");
        if (attribute != (ImageAttribute *) NULL)
          q+=(translate)(q,attribute->value,MaxTextExtent);
        break;
      }
      case 'm':
      {
        /* File format "magick" */
        q+=(translate)(q,image->magick,MaxTextExtent);
        break;
      }
      case 'n':
      {
        /* Number of scenes */
        FormatString(buffer,"%lu",(unsigned long) GetImageListLength(image));
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'o':
      {
        /* Output filename */
        if (image_info != (const ImageInfo *) NULL)
          q+=(translate)(q,image_info->filename,MaxTextExtent);
        break;
      }
      case 'p':
      {
        /* Page number */
        register const Image
          *frame;

        unsigned long
          page;

        frame=image;
        for (page=1; frame->previous != (Image *) NULL; page++)
          frame=frame->previous;
        FormatString(buffer,"%lu",page);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'q':
      {
        /* Quantum depth */
        if (GetPixelCachePresent(image))
          FormatString(buffer,"%lu",GetImageDepth(image,&image->exception));
        else
          FormatString(buffer,"%u",image->depth);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'r':
      {
        /* Image type */
        q+=(translate)(q,ImageTypeToString(GetImageType(image,&image->exception)),
                       MaxTextExtent);
        break;
      }
      case 's':
      {
        /* Scene number */
        FormatString(buffer,"%lu",image->scene);
        if (image_info != (const ImageInfo *) NULL)
          if (image_info->subrange != 0)
            FormatString(buffer,"%lu",image_info->subimage);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'u':
      {
        /* Unique temporary filename */
        if (image_info != (const ImageInfo *) NULL)
          {
            if (strlcpy(buffer,image_info->unique,MaxTextExtent) == 0)
              if (!AcquireTemporaryFileName(buffer))
                break;
            q+=(translate)(q,buffer,MaxTextExtent);
          }
        break;
      }
      case 'w':
      {
        /* Image width */
        FormatString(buffer,"%lu",image->columns);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'x':
      {
        /* Horizontal resolution */
        FormatString(buffer,"%g",image->x_resolution);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'y':
      {
        /* Vertical resolution */
        FormatString(buffer,"%g",image->y_resolution);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'z':
      {
        /* Second unique temporary filename */
        if (image_info != (const ImageInfo *) NULL)
          {
            if (strlcpy(buffer,image_info->zero,MaxTextExtent) == 0)
              if (!AcquireTemporaryFileName(buffer))
                break;
            q+=(translate)(q,buffer,MaxTextExtent);
          }
        break;
      }
      case 'A':
      {
        /* Transparency supported */
        FormatString(buffer,"%s",image->matte? "true" : "false");
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'C':
      {
        /* Image compression type */
        FormatString(buffer,"%s",CompressionTypeToString(image->compression));
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'D':
      {
        /* GIF disposal method */
        FormatString(buffer,"%d",image->dispose);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'G':
      {
        /* Original image width and height */
        FormatString(buffer,"%lux%lu",image->magick_columns,image->magick_rows);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'H':
      {
        /* page height */
        FormatString(buffer,"%lu",image->page.height);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'M':
      {
        /* Original filename specification */
        q+=(translate)(q,image->magick_filename,MaxTextExtent);
        break;
      }
      case 'O':
      {
        /* page offset */
        FormatString(buffer,"%+ld%+ld",image->page.x,image->page.y);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'P':
      {
        /* page dimensions */
        FormatString(buffer,"%lux%lu",image->page.width,image->page.height);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'Q':
      {
        /* Compression quality */
        attribute=GetImageAttribute(image,"JPEG-Quality");
        if (attribute != (ImageAttribute *) NULL)
          {
            q+=(translate)(q,attribute->value,MaxTextExtent);
            break;
          }
        if (image_info != (const ImageInfo *) NULL)
          {
            FormatString(buffer,"%lu",image_info->quality);
            q+=(translate)(q,buffer,MaxTextExtent);
            break;
          }
        FormatString(buffer,"%lu",DefaultCompressionQuality);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'T':
      {
        /* time delay (in centi-seconds) */
        FormatString(buffer,"%lu",image->delay);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'U':
      {
        /* resolution units */
        FormatString(buffer,"%s",ResolutionTypeToString(image->units));
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'W':
      {
        /* page width */
        FormatString(buffer,"%lu",image->page.width);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'X':
      {
        /* page x offset */
        FormatString(buffer,"%+ld",image->page.x);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case 'Y':
      {
        /* page y offset */
        FormatString(buffer,"%+ld",image->page.y);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case '@':
      {
        /* trim bounding box */
        RectangleInfo bounds = GetImageBoundingBox(image,
                                                   &image->exception);
        FormatString(buffer,"%lux%lu%+ld%+ld",bounds.width,bounds.height,
                     bounds.x,bounds.y);
        q+=(translate)(q,buffer,MaxTextExtent);
        break;
      }
      case '[':
      {
        /* Image attribute */
        char
          key[MaxTextExtent];

        /* Extract attribute key string. */
        p++;
        for (i=0; (i < MaxTextExtent) && (*p) && (*p != ']'); i++)
          {
          key[i]=(*p++);
          }
        if (']' != *p)
          break;
        key[i]='\0';

        /* Try to get the attibute from image */
        attribute=GetImageAttribute(image,key);

        /* Try to get the attribute from image_info */
        if (attribute == (const ImageAttribute *) NULL)
          if (image_info != (const ImageInfo *) NULL)
            attribute=GetImageInfoAttribute(image_info,image,key);

        if (attribute != (const ImageAttribute *) NULL)
          {
            /* Attributes may be larger than MaxTextExtent so make
               sure there is sufficient memory allocated. Make sure
               that there is at least MaxTextExtent left available
               after we have concatenated our part. */
            offset=q-translated_text;
            if ((size_t) (offset+attribute->length+1+MaxTextExtent) >= length)
              {
                length += (attribute->length+1+2*MaxTextExtent);
                MagickReallocMemory(char *,translated_text,length);
                if (translated_text == (char *) NULL)
                  break;
                q=translated_text+offset;
              }
            q+=(translate)(q,attribute->value,attribute->length+1+MaxTextExtent);
          }
        break;
      }
      case '#':
      {
        if (GetPixelCachePresent(image))
          {
            (void) SignatureImage(image);
            attribute=GetImageAttribute(image,"signature");
            if (attribute != (ImageAttribute *) NULL)
              q+=(translate)(q,attribute->value,MaxTextExtent);
          }
        else
          {
            *q++='%';
            *q++=(*p);
          }
        break;
      }
      case '%':
      {
        /* Pass through literal % */
        *q++=(*p);
        break;
      }
      default:
      {
        /* Pass through unknown codes */
        *q++='%';
        *q++=(*p);
        break;
      }
    }
    if (*p == '\0')
      break;
  }
  *q='\0';
  if (text != (char *) formatted_text)
    MagickFreeMemory(text);
  return(translated_text);
}
