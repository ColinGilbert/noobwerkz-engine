/*
% Copyright (C) 2003 - 2016 GraphicsMagick Group
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
%                         BBBB   L       OOO   BBBB                           %
%                         B   B  L      O   O  B   B                          %
%                         BBBB   L      O   O  BBBB                           %
%                         B   B  L      O   O  B   B                          %
%                         BBBB   LLLLL   OOO   BBBB                           %
%                                                                             %
%                                                                             %
%                  GraphicsMagick Binary Large OBject Methods                 %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1999                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#if defined(MSWINDOWS) || defined(__CYGWIN__)
# include "magick/nt_feature.h"
#endif
#include "magick/blob.h"
#include "magick/confirm_access.h"
#include "magick/constitute.h"
#include "magick/delegate.h"
#include "magick/enum_strings.h"
#include "magick/log.h"
#include "magick/list.h"
#include "magick/map.h"
#include "magick/magick.h"
#include "magick/magick_endian.h"
#include "magick/module.h"
#include "magick/pixel_cache.h"
#include "magick/resource.h"
#include "magick/semaphore.h"
#include "magick/tempfile.h"
#include "magick/utility.h"
#if defined(HasZLIB)
#  include "zlib.h"
#endif
#if defined(HasBZLIB)
#  include "bzlib.h"
#endif

/*
  Define declarations.
*/
#define DefaultBlobQuantum  65541


/*
  Enum declarations.
*/
typedef enum
{
  UndefinedStream,  /* Closed or open error */
  FileStream,       /* Opened with stdio fopen() or via image_info->file */
  StandardStream,   /* Stdin or stdout (filename "-") */
  PipeStream,       /* Command pipe stream opened via popen() */
  ZipStream,        /* Opened with zlib's gzopen() */
  BZipStream,       /* Opened with bzlib's BZ2_bzopen() */
  BlobStream        /* Memory mapped, or in allocated RAM */
} StreamType;

/*
  Typedef declarations.
*/

typedef union _MagickFileHandle
{
    FILE    *std;       /* stdio handle */
#if defined(HasBZLIB)
    BZFILE  *bz;        /* bzip handle */
#endif
#if defined(HasZLIB)
    gzFile   gz;        /* zlib handle */
#endif
} MagickFileHandle;

struct _BlobInfo
{
  size_t
    block_size,         /* I/O block size */
    length,             /* The current size of the BLOB data. */
    extent,             /* The amount of backing store currently allocated */
    quantum;            /* The amount by which to increase the size of the backing store */

  unsigned int
    mapped,             /* True if backing store is a memory mapped file. */
    eof;                /* True if input data has been entirely read. */

  magick_off_t
    offset,             /* Current offset (I/O point) as would be returned by TellBlob() */
    size;               /* Size of the underlying file, or the BLOB */

  MagickBool
    exempt,             /* True if file descriptor should not be closed.*/
    temporary;          /* Associated file is a temporary file */

  unsigned int
    status;             /* Error status. 0 == good */

  StreamType
    type;               /* Classification for how BLOB I/O is implemented. */

  MagickFileHandle
    handle;             /* Handle for I/O (if any) */

  BlobMode
    mode;               /* Blob open mode */

  unsigned char
    *data;              /* Blob or memory mapped data. */

  MagickBool
    fsync;              /* Fsync on close if true */

  SemaphoreInfo
    *semaphore;         /* Lock for reference_count access */

  long
    reference_count;    /* Number of times this blob is referenced. */

  unsigned long
    signature;          /* Numeric value used to evaluate structure integrity. */
};

typedef union _MagickInt32Union
{
  magick_uint32_t
    uint32;

  magick_int32_t
    int32;

} MagickInt32Union;

typedef union _MagickInt16Union
{
  magick_uint16_t
    uint16;

  magick_int16_t
    int16;

} MagickInt16Union;

/*
  Forward Declarations
*/
static int SyncBlob(Image *image);

/*
  Some systems have unlocked versions of getc & putc which are faster
  when multi-threading is enabled.  Blobs do not require multi-thread
  support since Images are only allowed to be accessed by one thread at
  a time. Using the unlocked version improves performance by about 30%.
*/
#if defined(HAVE_PTHREAD)
#  if defined(HAVE_GETC_UNLOCKED)
#    undef getc
#    define getc getc_unlocked
#  endif
#  if defined(HAVE_PUTC_UNLOCKED)
#    undef putc
#    define putc putc_unlocked
#  endif
#endif

static const char *BlobStreamTypeToString(StreamType stream_type)
{
  const char
    *type_string="Undefined";

  switch (stream_type)
  {
  case UndefinedStream:
    type_string="Undefined";
    break;
  case FileStream:
    type_string="File";
    break;
  case StandardStream:
    type_string="Standard";
    break;
  case PipeStream:
    type_string="Pipe";
    break;
  case ZipStream:
    type_string="Zip";
    break;
  case BZipStream:
    type_string="BZip";
    break;
  case BlobStream:
    type_string="Blob";
    break;
  }
  return type_string;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b S t r e a m                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadBlobStream() allocates the requested data size, or the amount
%  remaining (whichever is smaller) from a BlobStream.  This function
%  should only be invoked for Blobs of type 'BlobStream'.  The number of
%  bytes available from the requested length is returned.  If fewer bytes
%  are available than requested, the Blob EOF flag is set True.  A user
%  provided pointer is updated with the address of the data.  This pointer
%  is only valid while the BlobStream remains mapped or allocated.
%
%  The format of the ReadBlobStream method is:
%
%      size_t ReadBlobStream(Image *image,const size_t length,void **data)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o length: The requested amount of data.
%
%    o data: A pointer to where the address of the data should be returned.
%
*/
static inline size_t ReadBlobStream(Image *image,const size_t length,
                                    void **data)
{
  size_t
    available;

  if (image->blob->offset >= (magick_off_t) image->blob->length)
    {
      image->blob->eof=MagickTrue;
      return 0;
    }
  *data=(void *)(image->blob->data+image->blob->offset);
  available=Min(length,image->blob->length-image->blob->offset);
  image->blob->offset+=available;
  if (available < length)
    image->blob->eof=True;
  return available;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b S t r e a m                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteBlobStream() writes data to an in-memory Blob  This function
%  should only be invoked for Blobs of type 'BlobStream'.  The number of
%  bytes written is returned.
%
%  The format of the WriteBlobStream method is:
%
%      size_t WriteBlobStream(Image *image,const size_t length,
%                             const void *data)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o length: The requested amount of data.
%
%    o data: A pointer to where the data resides.
%
*/
static void *ExtendBlobWriteStream(Image *image,const size_t length)
{
  if ((image->blob->offset+length) >= image->blob->extent)
    {
      /* In-memory Blob */
      image->blob->quantum<<=1;
      image->blob->extent+=length+image->blob->quantum;
      MagickReallocMemory(unsigned char *,image->blob->data,image->blob->extent+1);
      (void) SyncBlob(image);
      if (image->blob->data == (unsigned char *) NULL)
        {
          DetachBlob(image->blob);
          return 0;
        }
    }
  return image->blob->data+image->blob->offset;
}
static inline size_t WriteBlobStream(Image *image,const size_t length,
                                     const void *data)
{
  void
    *dest;

  dest=image->blob->data+image->blob->offset;
  if ((image->blob->offset+length) >= image->blob->extent)
    if ((dest=ExtendBlobWriteStream(image,length)) == (void *) NULL)
      return 0;

  (void) memcpy(dest,data,length);
  image->blob->offset+=length;
  if (image->blob->offset > (magick_off_t) image->blob->length)
    image->blob->length=image->blob->offset;
  return length;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A t t a c h B l o b                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AttachBlob() attaches a blob to the BlobInfo structure.
%
%  The format of the AttachBlob method is:
%
%      void AttachBlob(BlobInfo *blob_info,const void *blob,const size_t length)
%
%  A description of each parameter follows:
%
%    o blob_info: Specifies a pointer to a BlobInfo structure.
%
%    o blob: The address of a character stream in one of the image formats
%      understood by GraphicsMagick.
%
%    o length: This size_t integer reflects the length in bytes of the blob.
%
%
*/
MagickExport void AttachBlob(BlobInfo *blob_info,const void *blob,
  const size_t length)
{
  assert(blob_info != (BlobInfo *) NULL);
  blob_info->length=length;
  blob_info->extent=length;
  blob_info->quantum=DefaultBlobQuantum;
  blob_info->offset=0;
  blob_info->type=BlobStream;
  blob_info->handle.std=(FILE *) NULL;
#if defined(HasBZLIB)
  blob_info->handle.bz=(BZFILE *) NULL;
#endif
#if defined(HasZLIB)
  blob_info->handle.gz=(gzFile) NULL;
#endif
  blob_info->data=(unsigned char *) blob;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B l o b I s S e e k a b l e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BlobIsSeekable() returns MagickTrue if the blob supports seeks
%  (SeekBlob() is functional).
%
%  The format of the BlobIsSeekable method is:
%
%      MagickBool BlobIsSeekable(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: Image to query
%
%
*/
MagickExport MagickBool BlobIsSeekable(const Image *image)
{
  assert(image != (const Image *) NULL);
  assert(image->blob != (const BlobInfo *) NULL);

  return ((image->blob->type == FileStream) ||
          (image->blob->type == BlobStream));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B l o b R e s e r v e S i z e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BlobReserveSize() sets the output size of the blob or file.  This is used
%  as a means to minimize memory or filesystem fragmentation if the final
%  output size is known in advance.  While it is possible that file
%  fragmentation is reduced, it is also possible that file write
%  performance is reduced by changing a write operation to a read, modify,
%  write operation.
%
%  The format of the BlobReserveSize method is:
%
%      MagickPassFail BlobReserveSize(Image *image, magick_off_t size)
%
%  A description of each parameter follows:
%
%    o image: Image to update
%
%    o size: New output size.
%
*/
MagickExport MagickPassFail BlobReserveSize(Image *image, magick_off_t size)
{
  MagickPassFail
    status;

  status=MagickPass;

  if ((FileStream == image->blob->type) ||
      ((BlobStream == image->blob->type) &&
       (image->blob->mapped) && (image->blob->handle.std != (FILE *) NULL)))
    {
#if defined(HAVE_POSIX_FALLOCATE)
      /*
        FIXME: Solaris 11.2 documentation says that posix_fallocate()
        reports EINVAL for anything but UFS */
      int
        err_status;

      if ((err_status=posix_fallocate(fileno(image->blob->handle.std),
                                      0UL, size)) != 0)
        {
          /* ThrowException(&image->exception,BlobError,UnableToWriteBlob,strerror(err_status)); */
          /* status=MagickFail; */
        }
#endif /* HAVE_POSIX_FALLOCATE */
    }

  if (BlobStream == image->blob->type)
  {
    /*
      In-memory blob
    */
    image->blob->extent=size;
    MagickReallocMemory(unsigned char *,image->blob->data,image->blob->extent+1);
    (void) SyncBlob(image);
        
    if (image->blob->data == (unsigned char *) NULL)
      {
        ThrowException(&image->exception,ResourceLimitError,MemoryAllocationFailed,
                       NULL);

        DetachBlob(image->blob);
        status=MagickFail;
      }
  }
  if (image->logging)
    (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                          "Request to reserve %" MAGICK_OFF_F "u output bytes %s",
                          size,
                          (status == MagickFail ? "failed" : "succeeded"));

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B l o b T o F i l e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BlobToFile() writes a blob to a file.  It returns MagickFail if an error
%  occurs otherwise MagickPass.
%
%  The format of the BlobToFile method is:
%
%      MagickPassFail BlobToFile(const char *filename,const void *blob,
%        const size_t length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status:  BlobToFile returns MagickPass on success; otherwise,  it
%      returns MagickFail if an error occurs.
%
%    o filename: Write the blob to this file.
%
%    o blob: The address of a blob.
%
%    o length: This length in bytes of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickPassFail BlobToFile(const char *filename,const void *blob,
				       const size_t length,ExceptionInfo *exception)
{
  ssize_t
    count;

  int
    file;

  register size_t
    i;

  MagickPassFail
    status=MagickPass;

  assert(filename != (const char *) NULL);
  assert(blob != (const void *) NULL);
  (void) LogMagickEvent(BlobEvent,GetMagickModule(),
			"Copying memory BLOB to file %s",filename);
  if (MagickConfirmAccess(FileWriteConfirmAccessMode,filename,exception)
      == MagickFail)
    return MagickFail;
  file=open(filename,O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,0777);
  if (file == -1)
    {
      ThrowException(exception,BlobError,UnableToWriteBlob,filename);
      status=MagickFail;
    }
  if (status != MagickFail)
    {
      const char
	*env = NULL;

      size_t
        block_size;

      block_size=MagickGetFileSystemBlockSize();

      /*
	Write data to file.
      */
      for (i=0; i < length; i+=count)
	{
          size_t
            remaining;

          MAGICK_POSIX_IO_SIZE_T
            amount;

          remaining=length - i;
          if (remaining > block_size)
            amount=(MAGICK_POSIX_IO_SIZE_T) block_size;
          else
            amount=(MAGICK_POSIX_IO_SIZE_T) remaining;

	  count=write(file,(char *) blob+i,amount);
	  if (count <= 0)
	    break;
	}

      if (i < length)
	{
	  if (status != MagickFail)
	    ThrowException(exception,BlobError,UnableToWriteBlob,filename);
	  status=MagickFail;
	}

      /*
	Explicitly synchronize file to disk if requested.
      */
      env = getenv("MAGICK_IO_FSYNC");
      if ((env != (const char *) NULL) && (LocaleCompare(env,"TRUE") == 0))
	{
	  if (fsync(file) == -1)
	    {
	      if (status != MagickFail)
		ThrowException(exception,BlobError,UnableToWriteBlob,filename);
	      status=MagickFail;
	    }
	}
      if (close(file) == -1)
	{
	  if (status != MagickFail)
	    ThrowException(exception,BlobError,UnableToWriteBlob,filename);
	  status=MagickFail;
	}
    }
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B l o b T o I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BlobToImage() implements direct to memory image formats.  It returns the
%  blob as an image.
%
%  The format of the BlobToImage method is:
%
%      Image *BlobToImage(const ImageInfo *image_info,const void *blob,
%        const size_t length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o blob: The address of a character stream in one of the image formats
%      understood by GraphicsMagick.
%
%    o length: This size_t integer reflects the length in bytes of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *BlobToImage(const ImageInfo *image_info,const void *blob,
				const size_t length,ExceptionInfo *exception)
{
  const MagickInfo
    *magick_info;

  Image
    *image;

  ImageInfo
    *clone_info;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);

  image=(Image *) NULL;
  (void) LogMagickEvent(BlobEvent,GetMagickModule(), "Entering BlobToImage");
  if ((blob == (const void *) NULL) || (length == 0))
    {
      ThrowException(exception,OptionError,NullBlobArgument,
		     image_info->magick);
      (void) LogMagickEvent(BlobEvent,GetMagickModule(),
			    "Leaving BlobToImage");
      return((Image *) NULL);
    }
  clone_info=CloneImageInfo(image_info);
  clone_info->blob=(void *) blob;
  clone_info->length=length;
  /*
    Set file magick based on file name or file header if magick was
    not provided.
  */
  if (clone_info->magick[0] == '\0')
    (void) SetImageInfo(clone_info,SETMAGICK_READ,exception);
  magick_info=GetMagickInfo(clone_info->magick,exception);
  if (magick_info == (const MagickInfo *) NULL)
    {
      DestroyImageInfo(clone_info);
      (void) LogMagickEvent(BlobEvent,GetMagickModule(),
			    "Leaving BlobToImage");
      return((Image *) NULL);
    }
  if (magick_info->blob_support)
    {
      /*
        Native blob support for this image format.
      */
      (void) LogMagickEvent(BlobEvent,GetMagickModule(),
			    "Using native BLOB support");
      (void) strlcpy(clone_info->filename,image_info->filename,
		     MaxTextExtent);
      (void) strlcpy(clone_info->magick,image_info->magick,MaxTextExtent);
      image=ReadImage(clone_info,exception);
      if (image != (Image *) NULL)
        DetachBlob(image->blob);
      DestroyImageInfo(clone_info);
      (void) LogMagickEvent(BlobEvent,GetMagickModule(),
			    "Leaving BlobToImage");
      return(image);
    }
  /*
    Write blob to a temporary file on disk.
  */
  {
    char
      temporary_file[MaxTextExtent];

      (void) LogMagickEvent(BlobEvent,GetMagickModule(),
			    "Using temporary file");
    clone_info->blob=(void *) NULL;
    clone_info->length=0;
    
    if(!AcquireTemporaryFileName(temporary_file))
      {
	ThrowException(exception,FileOpenError,UnableToCreateTemporaryFile,
		       clone_info->filename);
      }
    else
      {
	if (BlobToFile(temporary_file,blob,length,exception) != MagickFail)
	  {
	    clone_info->filename[0]='\0';
	    if (clone_info->magick[0] != '\0')
	      {
		(void) strlcpy(clone_info->filename,clone_info->magick,sizeof(clone_info->filename));
		(void) strlcat(clone_info->filename,":",sizeof(clone_info->filename));
	      }
	    (void) strlcat(clone_info->filename,temporary_file,sizeof(clone_info->filename));
	    image=ReadImage(clone_info,exception);
	    /*
	      Restore original user-provided file name field to images
	      in list so that user does not see a temporary file name.
	    */
	    if (image != (Image *) NULL)
	      {
		Image
		  *list_image;

		list_image = GetFirstImageInList(image);
		while (list_image != (Image *) NULL)
		  {
		    (void) strlcpy(list_image->magick_filename,image_info->filename,sizeof(list_image->magick_filename));
		    (void) strlcpy(list_image->filename,image_info->filename,sizeof(list_image->filename));
		    list_image = GetNextImageInList(list_image);
		  }
	      }
	  }
	(void) LiberateTemporaryFile(temporary_file);
      }
  }
  DestroyImageInfo(clone_info);
  (void) LogMagickEvent(BlobEvent,GetMagickModule(), "Leaving BlobToImage");
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C l o n e B l o b I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloneBlobInfo() makes a duplicate of the given blob info structure, or if
%  blob info is NULL, a new one.
%
%  The format of the CloneBlobInfo method is:
%
%      BlobInfo *CloneBlobInfo(const BlobInfo *blob_info)
%
%  A description of each parameter follows:
%
%    o clone_info: Method CloneBlobInfo returns a duplicate of the given
%      blob info, or if blob info is NULL a new one.
%
%    o quantize_info: a structure of type info.
%
%
*/
MagickExport BlobInfo *CloneBlobInfo(const BlobInfo *blob_info)
{
  BlobInfo
    *clone_info;

  SemaphoreInfo
    *semaphore;

  clone_info=MagickAllocateMemory(BlobInfo *,sizeof(BlobInfo));
  if (clone_info == (BlobInfo *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToCloneBlobInfo);
  GetBlobInfo(clone_info);
  if (blob_info == (BlobInfo *) NULL)
    return(clone_info);
  semaphore=clone_info->semaphore;
  (void) memcpy(clone_info,blob_info,sizeof(BlobInfo));
  clone_info->semaphore=semaphore;
  LockSemaphoreInfo(clone_info->semaphore);
  clone_info->reference_count=1;
  UnlockSemaphoreInfo(clone_info->semaphore);
  return(clone_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C l o s e B l o b                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CloseBlob() closes a stream associated with the image.
%
%  If the blob 'exempt' member is MagickTrue, then any passed file descriptor
%  is left open, otherwise it is closed.
%
%  The format of the CloseBlob method is:
%
%      void CloseBlob(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport void CloseBlob(Image *image)
{
  int
    status;

  /*
    Close image file.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  /*
    If blob was not allocated, is UndefinedStream (closed) then it
    doesn't need to be closed.
  */
  if ((image->blob == (BlobInfo *) NULL) ||
      (image->blob->type == UndefinedStream))
    return;

  if (image->logging)
    (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                          "Closing %sStream blob: image %p, blob %p, ref %lu",
                          BlobStreamTypeToString(image->blob->type),
                          image,image->blob,image->blob->reference_count);

  status=0;
  switch (image->blob->type)
    {
    case UndefinedStream:
      break;
    case FileStream:
    case StandardStream:
    case PipeStream:
      {
        if (image->blob->fsync)
          {
            (void) fflush(image->blob->handle.std);
            (void) fsync(fileno(image->blob->handle.std));
          }
        status=ferror(image->blob->handle.std);
        break;
      }
    case ZipStream:
      {
#if defined(HasZLIB)
        (void) gzerror(image->blob->handle.gz,&status);
#endif
        break;
      }
    case BZipStream:
      {
#if defined(HasBZLIB)
        (void) BZ2_bzerror(image->blob->handle.bz,&status);
#endif
        break;
      }
    case BlobStream:
      break;
    }
  errno=0;
  image->taint=MagickFalse;
  image->blob->size=GetBlobSize(image);
  image->blob->eof=MagickFalse;
  image->blob->status=status < 0;
  image->blob->mode=UndefinedBlobMode;

  /*
    If we are allowed to close the stream and detatch (destroy)
    the blob.
  */
  if (! image->blob->exempt )
    {
      /*
        Close the underlying stream.
      */
      switch (image->blob->type)
        {
        case UndefinedStream:
          break;
        case FileStream:
        case StandardStream:
          {
            status=fclose(image->blob->handle.std);
            break;
          }
        case PipeStream:
          {
#if defined(HAVE_PCLOSE)
            status=pclose(image->blob->handle.std);
#endif /* defined(HAVE_PCLOSE) */
            break;
          }
        case ZipStream:
          {
#if defined(HasZLIB)
            status=gzclose(image->blob->handle.gz);
#endif
            break;
          }
        case BZipStream:
          {
#if defined(HasBZLIB)
            BZ2_bzclose(image->blob->handle.bz);
#endif
            break;
          }
        case BlobStream:
          {
            break;
          }
        }
      /*
        Detatch (destroy) the blob.
      */
      DetachBlob(image->blob);
    }
  image->blob->type=UndefinedStream;
  image->blob->status=(status != 0);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y B l o b                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyBlob() deallocates memory associated with a blob.  The blob is
%  a reference counted object so the object is only destroyed once its
%  reference count decreases to zero.
%
%  The format of the DestroyBlob method is:
%
%      void DestroyBlob(Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport void DestroyBlob(Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->blob != (BlobInfo *) NULL)
    {
      MagickBool
        destroy;

      assert(image->blob->signature == MagickSignature);
      LockSemaphoreInfo(image->blob->semaphore);
      if (image->logging)
        (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                              "Destroy blob (ref counted): image %p, blob %p,"
                              " ref %lu, filename \"%s\"",
                              image,image->blob,image->blob->reference_count,
                              image->filename);
      image->blob->reference_count--;
      assert(image->blob->reference_count >= 0);
      destroy=(image->blob->reference_count > 0 ? MagickFalse : MagickTrue);
      UnlockSemaphoreInfo(image->blob->semaphore);
      if (destroy)
        {
          /*
            Destroy blob object.
          */
          if (image->logging)
            (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                                  "  Destroy blob (real): image %p, blob %p,"
                                  " ref %lu, filename \"%s\"",
                                  image,image->blob,
                                  image->blob->reference_count,image->filename);
          if (image->blob->type != UndefinedStream)
            CloseBlob(image);
          if (image->blob->mapped)
            (void) UnmapBlob(image->blob->data,image->blob->length);
	  DestroySemaphoreInfo(&image->blob->semaphore);
          (void) memset((void *) image->blob,0xbf,sizeof(BlobInfo));
          MagickFreeMemory(image->blob);
        }
      image->blob=(BlobInfo *) NULL;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y B l o b I n f o                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyBlobInfo() deallocates memory associated with an BlobInfo structure.
%  The blob is a reference counted object so the object is only destroyed once
%  its reference count decreases to zero. Use of DestroyBlob is preferred over
%  this function since it assures that the blob is closed prior to destruction.
%
%  This function is no longer used within GraphicsMagick.
%
%  The format of the DestroyBlobInfo method is:
%
%      void DestroyBlobInfo(BlobInfo *blob)
%
%  A description of each parameter follows:
%
%    o blob: Specifies a pointer to a BlobInfo structure.
%
%
*/
MagickExport void DestroyBlobInfo(BlobInfo *blob)
{
  if (blob != (BlobInfo *) NULL)
    {
      MagickBool
        destroy;

      assert(blob->signature == MagickSignature);
      LockSemaphoreInfo(blob->semaphore);
      (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                            "Destroy blob info: blob %p, ref %lu",
                            blob,blob->reference_count);
      blob->reference_count--;
      assert(blob->reference_count >= 0);
      destroy=(blob->reference_count > 0 ? MagickFalse : MagickTrue);
      UnlockSemaphoreInfo(blob->semaphore);
      if (destroy)
        {
          if (blob->mapped)
            (void) UnmapBlob(blob->data,blob->length);
	  DestroySemaphoreInfo(&blob->semaphore);
          (void) memset((void *)blob,0xbf,sizeof(BlobInfo));
          MagickFreeMemory(blob);
        }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e t a c h B l o b                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DetachBlob() detaches a blob from the BlobInfo structure.
%
%  The format of the DetachBlob method is:
%
%      void DetachBlob(BlobInfo *blob_info)
%
%  A description of each parameter follows:
%
%    o blob_info: Specifies a pointer to a BlobInfo structure.
%
%
*/
MagickExport void DetachBlob(BlobInfo *blob_info)
{
  (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                        "Detach (reset) blob: blob %p, ref %lu",
                        blob_info,blob_info->reference_count);
 assert(blob_info != (BlobInfo *) NULL);
  if (blob_info->mapped)
    {
      (void) UnmapBlob(blob_info->data,blob_info->length);
      LiberateMagickResource(MapResource,blob_info->length);
    }
  blob_info->mapped=MagickFalse;
  blob_info->length=0;
  blob_info->offset=0;
  blob_info->eof=MagickFalse;
  blob_info->exempt=MagickFalse;
  blob_info->type=UndefinedStream;
  blob_info->handle.std=(FILE *) NULL;
#if defined(HasBZLIB)
  blob_info->handle.bz=(BZFILE *) NULL;
#endif
#if defined(HasZLIB)
  blob_info->handle.gz=(gzFile) NULL;
#endif
  blob_info->data=(unsigned char *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  E O F B l o b                                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EOFBlob() returns a non-zero value when EOF has been detected reading from
%  a blob or file.
%
%  The format of the EOFBlob method is:
%
%      int EOFBlob(const Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method EOFBlob returns 0 on success; otherwise,  it
%      returns -1 and set errno to indicate the error.
%
%    o image: The image.
%
%
*/
MagickExport int EOFBlob(const Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  switch (image->blob->type)
  {
    case UndefinedStream:
      break;
    case FileStream:
    case StandardStream:
    case PipeStream:
    {
      image->blob->eof=feof(image->blob->handle.std);
      break;
    }
    case ZipStream:
    {
      image->blob->eof=MagickFalse;
      break;
    }
    case BZipStream:
    {
#if defined(HasBZLIB)
      int
        status;

      (void) BZ2_bzerror(image->blob->handle.bz,&status);
      image->blob->eof=status == BZ_UNEXPECTED_EOF;
#endif
      break;
    }
    case BlobStream:
      break;
  }
  return(image->blob->eof);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   F i l e T o B l o b                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  FileToBlob() returns the contents of a file in a buffer allocated via
%  MagickMalloc() (which is equivalent to the system malloc() by default).
%  The character '\0' is appended to the buffer in case the buffer will be
%  accessed as a string.  The length of the buffer (not including the extra
%  terminating '\0' character) is returned via the 'length' parameter.
%  If an error occurs, a NULL pointer is returned.  The returned buffer
%  must be freed by the user in a matter compatible with MagickMalloc()
%  (e.g. via MagickFree()).
%
%  The format of the FileToBlob method is:
%
%      void *FileToBlob(const char *filename,size_t *length,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o blob:  FileToBlob() returns the contents of a file as a blob.  If
%      an error occurs NULL is returned.
%
%    o filename: The filename.
%
%    o length: This pointer to a size_t integer sets the initial length of the
%      blob.  On return, it reflects the actual length of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport void *FileToBlob(const char *filename,size_t *length,
                              ExceptionInfo *exception)
{
  FILE
    *file;

  unsigned char
    *blob;

  assert(filename != (const char *) NULL);
  assert(length != (size_t *) NULL);
  assert(exception != (ExceptionInfo *) NULL);

  blob=(unsigned char *) NULL;
  /* Open file */
  if ((file=fopen(filename,"rb")) != (FILE *) NULL)
    {
      /* Set buffering size (best effort) */
      size_t
        vbuf_size;

      vbuf_size=MagickGetFileSystemBlockSize();
      if (0 != vbuf_size)
        (void) setvbuf(file,NULL,_IOFBF,vbuf_size);

      /* Get file length */
      if (MagickFseek(file,0L,SEEK_END) != -1)
        {
          magick_off_t
            offset;

          if ((offset=MagickFtell(file)) != -1)
            {
              *length=(size_t) offset;
              if ((magick_off_t) *length == offset)
                {
                  /* Restore position to beginning of file */
                  if (MagickFseek(file,0L,SEEK_SET) != -1)
                    {
                      /* Allocate memory */
                      if ((blob=MagickAllocateMemory(unsigned char *,*length+1))
                          != (unsigned char *) NULL)
                        {
                          /* Read data from file */
                          if (fread(blob,1,*length,file) == *length)
                            {
                              /* Add terminating null byte */
                              blob[*length]='\0';
                            }
                          else
                            {
                              /* Failed to read all the data */
                              MagickFreeMemory(blob);
                              ThrowException3(exception,BlobError,
                                              UnableToReadToOffset,
                                              UnableToCreateBlob);
                            }
                        }
                      else
                        {
                          /* Failed to allocate the memory */
                          ThrowException(exception,ResourceLimitError,
                                         MemoryAllocationFailed,
                                         MagickMsg(BlobError,
                                                   UnableToCreateBlob));
                        }
                    }
                  else
                    {
                      /* Failed to seek back to beginning of file */
                      ThrowException3(exception,BlobError,UnableToSeekToOffset,
                                      UnableToCreateBlob);
                    }
                }
              else
                {
                  /* File is too large for size_t */
                  ThrowException(exception,ResourceLimitError,
                                 MemoryAllocationFailed,
                                 MagickMsg(BlobError,UnableToCreateBlob));
                }
            }
          else
            {
              /* Failed to get EOF offset */
              ThrowException3(exception,BlobError,UnableToSeekToOffset,
                              UnableToCreateBlob);
            }
        }
      else
        {
          /* Failed to seek to end of file */
          ThrowException3(exception,BlobError,UnableToSeekToOffset,
                          UnableToCreateBlob);
        }
      (void) fclose(file);
      file=(FILE *) NULL;
    }
  else
    {
      /* Failed to open the file */
      ThrowException(exception,BlobError,UnableToOpenFile,filename);
    }
  return(blob);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t B l o b F i l e H a n d l e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetBlobFileHandle() returns the stdio file handle associated with the
%  image blob.  If there is no associated file handle, then a null pointer
%  is returned.
%
%  The format of the GetBlobFileHandle method is:
%
%      FILE *GetBlobFileHandle(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: Image to query
%
%
*/
MagickExport FILE *GetBlobFileHandle(const Image *image)
{
  assert(image != (const Image *) NULL);
  assert(image->blob != (const BlobInfo *) NULL);
  return (image->blob->handle.std);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t B l o b I n f o                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetBlobInfo() initializes the BlobInfo structure.
%
%  The format of the GetBlobInfo method is:
%
%      void GetBlobInfo(BlobInfo *blob_info)
%
%  A description of each parameter follows:
%
%    o blob_info: Specifies a pointer to a BlobInfo structure.
%
%
*/
MagickExport void GetBlobInfo(BlobInfo *blob_info)
{
  assert(blob_info != (BlobInfo *) NULL);
  (void) memset(blob_info,0,sizeof(BlobInfo));
  blob_info->quantum=DefaultBlobQuantum;
  blob_info->fsync=MagickFalse;
  blob_info->semaphore=AllocateSemaphoreInfo();
  LockSemaphoreInfo(blob_info->semaphore);
  blob_info->reference_count=1;
  UnlockSemaphoreInfo(blob_info->semaphore);
  blob_info->signature=MagickSignature;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  G e t B l o b I s O p e n                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetBlobIsOpen() returns MagickTrue if the blob is currently open or
%  MagickFalse if it is currently closed.
%
%  The format of the GetBlobSize method is:
%
%      MagickBool GetBlobIsOpen(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport MagickBool GetBlobIsOpen(const Image *image)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);

  return (image->blob->type != UndefinedStream ? MagickTrue : MagickFalse);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  G e t B l o b S i z e                                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetBlobSize() returns the current length of the image file or blob; zero is
%  returned if the size cannot be determined.  If BLOB is no longer open, then
%  return the size when the BLOB was closed.
%
%  The format of the GetBlobSize method is:
%
%      magick_off_t GetBlobSize(const Image *image)
%
%  A description of each parameter follows:
%
%    o size:  Method GetBlobSize returns the current length of the image file
%      or blob.
%
%    o image: The image.
%
%
*/
MagickExport magick_off_t GetBlobSize(const Image *image)
{
  MagickStatStruct_t
    attributes;

  magick_off_t
    offset;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);

  offset=0;
  switch (image->blob->type)
    {
    case UndefinedStream:
      offset=image->blob->size;
      break;
    case FileStream:
      {
	offset=(MagickFstat(fileno(image->blob->handle.std),&attributes) < 0 ? 0 :
		attributes.st_size);
	break;
      }
    case StandardStream:
    case PipeStream:
      break;
    case ZipStream:
    case BZipStream:
      {
	offset=(MagickStat(image->filename,&attributes) < 0 ? 0 :
		attributes.st_size);
	break;
      }
    case BlobStream:
      {
	offset=image->blob->length;
	break;
      }
    }
  return(offset);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t B l o b S t a t u s                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetBlobStatus() returns the blob error status.
%
%  The format of the GetBlobStatus method is:
%
%      int GetBlobStatus(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport int GetBlobStatus(const Image *image)
{
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  return(image->blob->status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t B l o b S t r e a m D a t a                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetBlobStreamData() returns the stream data for the image. The data is only
%  available if the data is stored on the heap, or is memory mapped.
%  Otherwise a NULL value is returned.
%
%  The format of the GetBlobStreamData method is:
%
%      unsigned char *GetBlobStreamData(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%
*/
MagickExport unsigned char *GetBlobStreamData(const Image *image)
{
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->blob->type != BlobStream)
    return 0;
  return(image->blob->data);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t B l o b T e m p o r a r y                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetBlobTemporary() returns MagickTrue if the file associated with the blob
%  is a temporary file and should be removed when the associated image is
%  destroyed.
%
%  The format of the GetBlobTemporary method is:
%
%      MagickBool GetBlobTemporary(const Image *image)
%
%  A description of each parameter follows:
%
%    o image: Image to query
%
%
*/
MagickExport MagickBool GetBlobTemporary(const Image *image)
{
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (const BlobInfo *) NULL);
  assert(image->blob->signature == MagickSignature);
  return (image->blob->temporary != MagickFalse);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  G e t C o n f i g u r e B l o b                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetConfigureBlob() returns the specified configure file as a blob.
%
%  The format of the GetConfigureBlob method is:
%
%      void *GetConfigureBlob(const char *filename,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o filename: The configure file name.
%
%    o path: return the full path information of the configure file.
%
%    o length: This pointer to a size_t integer sets the initial length of the
%      blob.  On return, it reflects the actual length of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

#if !defined(UseInstalledMagick) && defined(POSIX)
static void ChopPathComponents(char *path,const unsigned long components)
{
  long
    count;

  register char
    *p;

  if (*path == '\0')
    return;
  p=path+strlen(path);
  if (*p == *DirectorySeparator)
    *p='\0';
  for (count=0; (count < (long) components) && (p > path); p--)
    if (*p == *DirectorySeparator)
      {
        *p='\0';
        count++;
      }
}
#endif

static void AddConfigurePath(MagickMap path_map, unsigned int *path_index,
  const char *path,ExceptionInfo *exception)
{
  char
    key[MaxTextExtent];

  FormatString(key,"%u",*path_index);
  (void) MagickMapAddEntry(path_map,key,(void *)path,0,exception);
  (*path_index)++;
}

MagickExport void *GetConfigureBlob(const char *filename,char *path,
  size_t *length,ExceptionInfo *exception)
{
  MagickMap
    path_map;

  MagickMapIterator
    path_map_iterator;

  const char
    *key;

  unsigned char
    *blob=0;

  unsigned int
    logging,
    path_index=0;

  assert(filename != (const char *) NULL);
  assert(path != (char *) NULL);
  assert(length != (size_t *) NULL);
  assert(exception != (ExceptionInfo *) NULL);

  logging=IsEventLogging();

  (void) strlcpy(path,filename,MaxTextExtent);
  path_map=MagickMapAllocateMap(MagickMapCopyString,MagickMapDeallocateString);

  {
    /*
      Allow the configuration file search path to be explicitly
      specified.
    */
    const char
      *magick_configure_path = getenv("MAGICK_CONFIGURE_PATH");
    if ( magick_configure_path )
      {
        const char
          *end = NULL,
          *start = magick_configure_path;
        
        end=start+strlen(start);
        while ( start < end )
          {
            char
              buffer[MaxTextExtent];
            
            const char
              *separator;
            
            int
              string_length;
            
            separator = strchr(start,DirectoryListSeparator);
            if (separator)
              string_length=separator-start;
            else
              string_length=end-start;
            if (string_length > MaxTextExtent-1)
              string_length = MaxTextExtent-1;
            (void) strlcpy(buffer,start,string_length+1);
            if (buffer[string_length-1] != DirectorySeparator[0])
              (void) strlcat(buffer,DirectorySeparator,sizeof(buffer));
            AddConfigurePath(path_map,&path_index,buffer,exception);
            start += string_length+1;
          }
      }
  }

#if defined(UseInstalledMagick)

# if defined(MagickShareConfigPath)
  AddConfigurePath(path_map,&path_index,MagickShareConfigPath,exception);
# endif /* defined(MagickShareConfigPath) */

# if defined(MagickLibConfigPath)
  AddConfigurePath(path_map,&path_index,MagickLibConfigPath,exception);
# endif /* defined(MagickLibConfigPath) */

# if defined(MSWINDOWS) && !(defined(MagickLibConfigPath) || defined(MagickShareConfigPath))
  {
    char
      *registry_key,
      *key_value;

    /*
      Locate file via registry key.
    */
    registry_key="ConfigurePath";
    key_value=NTRegistryKeyLookup(registry_key);
    if (key_value == (char *) NULL)
      {
        ThrowException(exception,ConfigureError,RegistryKeyLookupFailed,registry_key);
        return 0;
      }

    FormatString(path,"%.1024s%s",key_value,DirectorySeparator);
    AddConfigurePath(path_map,&path_index,path,exception);
  }
#  endif /* defined(MSWINDOWS) */

#else /* !defined(UseInstalledMagick) */

  {
    const char
      *magick_home;
    
    /*
      Search under MAGICK_HOME.
    */
    magick_home=getenv("MAGICK_HOME");
    if (magick_home)
      {
#if defined(POSIX)
        FormatString(path,"%.1024s/share/%s/",magick_home,
          MagickShareConfigSubDir);
        AddConfigurePath(path_map,&path_index,path,exception);

        FormatString(path,"%.1024s/lib/%s/",magick_home,
          MagickLibConfigSubDir);
        AddConfigurePath(path_map,&path_index,path,exception);
#else
        FormatString(path,"%.1024s%s",magick_home,
          DirectorySeparator);
        AddConfigurePath(path_map,&path_index,path,exception);
#endif /* defined(POSIX) */
      }
    }

  if (getenv("HOME") != (char *) NULL)
    {
      /*
        Search $HOME/.magick.
      */
      FormatString(path,"%.1024s%s%s",getenv("HOME"),
        *getenv("HOME") == '/' ? "/.magick" : "",DirectorySeparator);
      AddConfigurePath(path_map,&path_index,path,exception);
    }

  if (*SetClientPath((char *) NULL) != '\0')
    {
#if defined(POSIX)
      char
        prefix[MaxTextExtent];

      /*
        Search based on executable directory if directory is known.
      */
      (void) strlcpy(prefix,SetClientPath((char *) NULL),MaxTextExtent);
      ChopPathComponents(prefix,1);

      FormatString(path,"%.1024s/lib/%s/",prefix,MagickLibConfigSubDir);
      AddConfigurePath(path_map,&path_index,path,exception);

      FormatString(path,"%.1024s/share/%s/",prefix,MagickShareConfigSubDir);
      AddConfigurePath(path_map,&path_index,path,exception);
#else /* defined(POSIX) */
      FormatString(path,"%.1024s%s",SetClientPath((char *) NULL),
        DirectorySeparator);
      AddConfigurePath(path_map,&path_index,path,exception);
#endif /* !defined(POSIX) */
    }

  /*
    Search current directory.
  */
  AddConfigurePath(path_map,&path_index,"",exception);
#endif /* !defined(UseInstalledMagick) */

  path_map_iterator=MagickMapAllocateIterator(path_map);

  if (logging)
    {
      char
        list_separator[2],
        *search_path=0;

      list_separator[0]=DirectoryListSeparator;
      list_separator[1]='\0';
      while(MagickMapIterateNext(path_map_iterator,&key))
        {
          if (search_path)
            (void) ConcatenateString(&search_path,list_separator);
          (void) ConcatenateString(&search_path,
            (const char *) MagickMapDereferenceIterator(path_map_iterator,0));
        }
      
      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
         "Searching for file \"%s\" in path \"%s\"",filename,search_path);

      MagickFreeMemory(search_path);
      MagickMapIterateToFront(path_map_iterator);
    }

  while(MagickMapIterateNext(path_map_iterator,&key))
    {
      char
        test_path[MaxTextExtent];

      FILE
        *file;

      FormatString(test_path,"%.1024s%.256s",
        (const char *)MagickMapDereferenceIterator(path_map_iterator,0),
        filename);

      file=fopen(test_path,"rb");
      if (file )
        {
          if (logging)
            (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
              "Found: %.1024s",test_path);
          (void) strcpy(path,test_path);
          (void) MagickFseek(file,0L,SEEK_END);
          *length=MagickFtell(file); /* FIXME: ftell returns long, but size_t may be unsigned */
          if (*length > 0)
            {
              (void) MagickFseek(file,0L,SEEK_SET);
              blob=MagickAllocateMemory(unsigned char *,(*length)+1);
              if (blob)
                {
                  *length=fread((void  *)blob, 1, *length, file);
                  blob[*length]='\0';
                }
            }
          (void) fclose(file);
          if (blob)
            break;
        }

      if (logging)
	{
	  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
				"Tried: %.1024s [%.1024s]",test_path,
				strerror(errno));
	  errno=0;
	}
    }
  MagickMapDeallocateIterator(path_map_iterator);
  MagickMapDeallocateMap(path_map);

  if (blob)
    return(blob);

#if defined(MSWINDOWS)
  {
    void
      *resource;

    resource=NTResourceToBlob(filename);
    if (resource)
      return resource;
  }
#endif /* defined(MSWINDOWS) */

  ThrowException(exception,ConfigureError,UnableToAccessConfigureFile,
    filename);

  return 0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m a g e T o B l o b                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImageToBlob() implements direct to memory image formats.  It returns the
%  image as a formatted blob and its length.  The magick member of the Image
%  structure determines the format of the returned blob (GIF, JPEG, PNG,
%  etc.).  This function is the equivalent of WriteImage(), but writes the
%  formatted "file" to a memory buffer rather than to an actual file.
%
%  The format of the ImageToBlob method is:
%
%      void *ImageToBlob(const ImageInfo *image_info,Image *image,
%        size_t *length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o image: The image.
%
%    o length: This pointer to a size_t integer sets the initial length of the
%      blob.  On return, it reflects the actual length of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport void *ImageToBlob(const ImageInfo *image_info,Image *image,
			       size_t *length,ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    unique[MaxTextExtent];

  const MagickInfo
    *magick_info;

  ImageInfo
    *clone_info;

  unsigned char
    *blob;

  unsigned int
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);

  image->logging=IsEventLogging();
  if (image->logging)
    (void) LogMagickEvent(BlobEvent,GetMagickModule(),"Entering ImageToBlob");
  /* SetExceptionInfo(exception,UndefinedException); */
  clone_info=CloneImageInfo(image_info);
  (void) strlcpy(clone_info->magick,image->magick,MaxTextExtent);
  magick_info=GetMagickInfo(clone_info->magick,exception);
  if (magick_info == (const MagickInfo *) NULL)
    {
      ThrowException(exception,MissingDelegateError,
		     NoDecodeDelegateForThisImageFormat,clone_info->magick);
      DestroyImageInfo(clone_info);
      if (image->logging)
	(void) LogMagickEvent(BlobEvent,GetMagickModule(),
			      "Exiting ImageToBlob");
      return((void *) NULL);
    }
  if (magick_info->blob_support)
    {
      /*
        Native blob support for this image format.
      */
      clone_info->blob=MagickAllocateMemory(void *,65535L);
      if (clone_info->blob == (void *) NULL)
        {
          ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,
			 MagickMsg(BlobError,UnableToCreateBlob));
          DestroyImageInfo(clone_info);
          if (image->logging)
            (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                                  "Exiting ImageToBlob");
          return((void *) NULL);
        }
      /* Blob length is initially zero */
      clone_info->length=0;
      /* Blob file descriptor should not be closed */
      image->blob->exempt=True;
      /* There is no filename for a memory blob */
      *image->filename='\0';
      /* Write the image to the blob */
      status=WriteImage(clone_info,image);
      if (status == MagickFalse)
        {
	  /* Only assert our own exception if an exception was not already reported. */
	  if (image->exception.severity == UndefinedException)
	    ThrowException(exception,BlobError,UnableToWriteBlob,
			   clone_info->magick);
          MagickFreeMemory(image->blob->data);
          DestroyImageInfo(clone_info);
          if (image->logging)
            (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                                  "Exiting ImageToBlob");
          return((void *) NULL);
        }
      /* Request to truncate memory allocation down to memory actually used. */
      MagickReallocMemory(unsigned char *,image->blob->data,image->blob->length+1);
      /* Pass blob data and length to user parameters */
      blob=image->blob->data;
      *length=image->blob->length;
      /* Reset BlobInfo to original state (without freeing blob data). */
      DetachBlob(image->blob);
      DestroyImageInfo(clone_info);
      if (image->logging)
        (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                              "Exiting ImageToBlob");
      return(blob);
    }
  /*
    Write file to disk in blob image format.
  */
  (void) strlcpy(filename,image->filename,MaxTextExtent);
  if (!AcquireTemporaryFileName(unique))
    {
      ThrowException(exception,FileOpenError,UnableToCreateTemporaryFile,
		     unique);
      DestroyImageInfo(clone_info);
      return((void *) NULL);
    }
  if (image->logging)
    (void) LogMagickEvent(BlobEvent,GetMagickModule(),
			  "Allocated temporary file \"%s\"",unique);
  FormatString(image->filename,"%.1024s:%.1024s",image->magick,unique);
  status=WriteImage(clone_info,image);
  DestroyImageInfo(clone_info);
  if (status == MagickFail)
    {
      (void) LiberateTemporaryFile(unique);
      ThrowException(exception,BlobError,UnableToWriteBlob,image->filename);
      if (image->logging)
        (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                              "Exiting ImageToBlob");
      return((void *) NULL);
    }
  /*
    Read image from disk as blob.
  */
  blob=(unsigned char *) FileToBlob(image->filename,length,exception);
  if (image->logging)
    (void) LogMagickEvent(BlobEvent,GetMagickModule(),
			  "Liberating temporary file \"%s\"",image->filename);
  (void) LiberateTemporaryFile(image->filename);
  (void) strlcpy(image->filename,filename,MaxTextExtent);
  if (blob == (unsigned char *) NULL)
    {
      ThrowException(exception,BlobError,UnableToReadFile,filename);
      if (image->logging)
        (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                              "Exiting ImageToBlob");
      return((void *) NULL);
    }
  if (image->logging)
    (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                          "Exiting ImageToBlob");
  return(blob);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m a g e T o F i l e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImageToFile() copies the input image from an open blob stream to a file.
%  It returns False if an error occurs otherwise True.  This function is used
%  to handle coders which are unable to stream the data in using Blob I/O.
%  Instead of streaming the data in, the data is streammed to a temporary
%  file, and the coder accesses the temorary file directly.
%
%  The format of the ImageToFile method is:
%
%      MagickPassFail ImageToFile(Image *image,const char *filename,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status:  ImageToFile returns MagickPass on success; otherwise,  it
%      returns MagickFail if an error occurs.
%
%    o image: The image.
%
%    o filename: Write the image to this file.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickPassFail ImageToFile(Image *image,const char *filename,
  ExceptionInfo *exception)
{
  char
    *buffer;

  ssize_t
    count;

  int
    file;

  register size_t
    i;

  size_t
    block_size,
    length;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(filename != (const char *) NULL);

  if (image->logging)
    (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                          "Copying from Blob stream to file %s",filename);
  if (MagickConfirmAccess(FileWriteConfirmAccessMode,filename,exception)
      == MagickFail)
    return MagickFail;
  file=open(filename,O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,0777);
  if (file == -1)
    {
      ThrowException(exception,BlobError,UnableToWriteBlob,filename);
      return(MagickFail);
    }
  block_size=MagickGetFileSystemBlockSize();
  buffer=MagickAllocateMemory(char *,block_size);
  if (buffer == (char *) NULL)
    {
      (void) close(file);
      ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,
        filename);
      return(MagickFail);
    }
  for (i=0; (length=ReadBlob(image,block_size,buffer)) > 0; )
  {
    for (i=0; i < length; i+=count)
    {
      count=write(file,buffer+i,(MAGICK_POSIX_IO_SIZE_T) (length-i));
      if (count <= 0)
        break;
    }
    if (i < length)
      break;
  }
  (void) close(file);
  MagickFreeMemory(buffer);
  return(i < length);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  M a p B l o b                                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MapBlob() creates a mapping from a file to a binary large object.
%
%  The format of the MapBlob method is:
%
%      void *MapBlob(int file,const MapMode mode,off_t offset,size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method MapBlob returns the address of the blob as well as
%      its length in bytes.
%
%    o file: map this file descriptor.
%
%    o mode: ReadMode, WriteMode, or IOMode.
%
%    o offset: starting at this offset within the file.
%
%    o length: the length of the mapping is returned in this pointer.
%
%
*/
MagickExport void *MapBlob(int file,const MapMode mode,magick_off_t offset,
  size_t length)
{
#if defined(HAVE_MMAP_FILEIO)
  void
    *map;

  /*
    Map file.
  */
  if (file == -1)
    return((void *) NULL);
  switch (mode)
  {
    case ReadMode:
    default:
    {
      map=(void *) MagickMmap((char *) NULL,length,PROT_READ,MAP_PRIVATE,file,
        (off_t)offset);
#if 0
#if defined(HAVE_MADVISE)
      if (map != (void *) MAP_FAILED)
        {
#if defined(MADV_SEQUENTIAL)
          /* Note: It has been noticed that madvise() wastes time if
             the file has been accessed recently so pages are already
             in RAM. ... */
          (void) madvise(map,length,MADV_SEQUENTIAL);
#endif /* defined(MADV_SEQUENTIAL) */
#if defined(MADV_WILLNEED)
          (void) madvise(map,length,MADV_WILLNEED);
#endif /* defined(MADV_WILLNEED) */
        }
#endif /* defined(HAVE_MADVISE) */
#endif
      break;
    }
    case WriteMode:
    {
      map=(void *) MagickMmap((char *) NULL,length,PROT_WRITE,MAP_SHARED,file,(off_t)offset);
#if defined(MADV_SEQUENTIAL)
          (void) madvise(map,length,MADV_SEQUENTIAL);
#endif /* defined(MADV_SEQUENTIAL) */
      break;
    }
    case IOMode:
    {
      map=(void *) MagickMmap((char *) NULL,length,(PROT_READ | PROT_WRITE),
        MAP_SHARED,file,(off_t)offset);
      break;
    }
  }
  if (map == (void *) MAP_FAILED)
    {
      (void) LogMagickEvent(BlobEvent,GetMagickModule(),
			    "Failed to mmap fd %d using %s mode at offset %"
			    MAGICK_OFF_F "u and length %" MAGICK_OFF_F
			    "u (%d=\"%s\").",file,MapModeToString(mode),offset,
			    (magick_off_t) length,errno,strerror(errno));
      return((void *) NULL);
    }
  (void) LogMagickEvent(BlobEvent,GetMagickModule(),
			"Mmapped fd %d using %s mode at offset %" MAGICK_OFF_F
			"u and length %" MAGICK_OFF_F "u to address %p",
			file,MapModeToString(mode),offset,(magick_off_t) length,
			map);
  return((void *) map);
#else
  return((void *) NULL);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  M S B O r d e r L o n g                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBOrderLong converts a least-significant byte first buffer
%  of integers to most-significant byte first.
%
%  The format of the MSBOrderLong method is:
%
%      void MSBOrderLong(unsigned char *buffer,const size_t length)
%
%  A description of each parameter follows.
%
%   o  p:  Specifies a pointer to a buffer of integers.
%
%   o  length:  Specifies the length of the buffer.
%
%
*/
MagickExport void MSBOrderLong(unsigned char *buffer,const size_t length)
{
  int
    c;

  register unsigned char
    *p,
    *q;

  assert(buffer != (unsigned char *) NULL);
  q=buffer+length;
  while (buffer < q)
  {
    p=buffer+3;
    c=(*p);
    *p=(*buffer);
    *buffer++=(unsigned char) c;
    p=buffer+1;
    c=(*p);
    *p=(*buffer);
    *buffer++=(unsigned char) c;
    buffer+=2;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  M S B O r d e r S h o r t                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MSBOrderShort converts a least-significant byte first buffer of
%  integers to most-significant byte first.
%
%  The format of the MSBOrderShort method is:
%
%      void MSBOrderShort(unsigned char *p,const size_t length)
%
%  A description of each parameter follows.
%
%   o  p:  Specifies a pointer to a buffer of integers.
%
%   o  length:  Specifies the length of the buffer.
%
%
*/
MagickExport void MSBOrderShort(unsigned char *p,const size_t length)
{
  int
    c;

  register unsigned char
    *q;

  assert(p != (unsigned char *) NULL);
  q=p+length;
  while (p < q)
  {
    c=(*p);
    *p=(*(p+1));
    p++;
    *p++=(unsigned char) c;
  }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   O p e n B l o b                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  OpenBlob() opens a file associated with the image.  A file name of '-' sets
%  the file to stdin for type 'r' and stdout for type 'w'.  If the filename
%  suffix is '.gz' or '.Z', the image is decompressed for type 'r' and
%  compressed for type 'w'.
%
%  The format of the OpenBlob method is:
%
%      MagickPassFail OpenBlob(const ImageInfo *image_info,Image *image,
%        const BlobMode mode,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status:  Method OpenBlob returns MagickPass if the file is successfully
%      opened otherwise MagickFail.
%
%    o image_info: The image info.
%
%    o image: The image.
%
%    o mode: The mode for opening the file.
%
*/

static void FormMultiPartFilename(Image *image, const ImageInfo *image_info)
{
  char
    filename[MaxTextExtent];

  /*
    Form filename for multi-part images.
  */
  if (MagickSceneFileName(filename,image->filename,"",MagickFalse,
                          GetImageIndexInList(image)))
    (void) strlcpy(image->filename,filename,MaxTextExtent);

  if (!image_info->adjoin)
    if ((image->previous != (Image *) NULL) ||
        (image->next != (Image *) NULL))
      {
        /* Propagate magick to next image in list. */
        if (image->next != (Image *) NULL)
          (void) strlcpy(image->next->magick,image->magick,
                         MaxTextExtent);
      }
}

MagickExport MagickPassFail OpenBlob(const ImageInfo *image_info,Image *image,
                                     const BlobMode mode,ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent],
    *type;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->logging)
    (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                          "Opening blob stream: image %p, blob %p,"
                          " mode %s ...", image, image->blob,
                          BlobModeToString(mode));
  /*
    Cache I/O block size
  */
  image->blob->block_size=MagickGetFileSystemBlockSize();
  assert(image->blob->block_size > 0);
  /*
    Attach existing memory buffer for I/O and immediately return.
  */
  if (image_info->blob != (void *) NULL)
    {
      AttachBlob(image->blob,image_info->blob,image_info->length);
      if (image->logging)
        (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                              "  attached data blob (addr %p, len %"
                              MAGICK_SIZE_T_F "u) to image %p, blob %p",
                              image_info->blob,
                              (MAGICK_SIZE_T) image_info->length,
                              image,image->blob);
      return(MagickPass);
    }
  /*
    Reset BlobInfo to defaults.
  */
  DetachBlob(image->blob);
  image->blob->mode=mode;
  switch (mode)
    {
    default: type=(char *) "r"; break;
    case ReadBlobMode: type=(char *) "r"; break;
    case ReadBinaryBlobMode: type=(char *) "rb"; break;
    case WriteBlobMode: type=(char *) "w"; break;
    case WriteBinaryBlobMode: type=(char *) "w+b"; break;
    }

  /*
    Open image file.
  */
  (void) strlcpy(filename,image->filename,MaxTextExtent);
  if (LocaleCompare(filename,"-") == 0)
    {
      /*
        Handle stdin/stdout stream
      */
      if (*type == 'r')
        {
          image->blob->handle.std=stdin;
          if (image->logging)
            (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                                  "  using stdin as StandardStream image"
                                  " %p, blob %p",
                                  image,image->blob);
        }
      else
        {
          image->blob->handle.std=stdout;
          if (image->logging)
            (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                                  "  using stdout as StandardStream image"
                                  " %p, blob %p",
                                  image,image->blob);
        }
#if defined(MSWINDOWS)
      if (strchr(type,'b') != (char *) NULL)
        setmode(_fileno(image->blob->handle.std),_O_BINARY);
#endif
      image->blob->type=StandardStream;
      image->blob->exempt=True;
    }
  else
    {
      if (*type == 'w')
        {
          /*
            Form filename for multi-part images.
          */
          if (!image_info->adjoin)
            FormMultiPartFilename(image,image_info);
          (void) strcpy(filename,image->filename);
        }
#if defined(HasZLIB)
      if (((strlen(filename) > 2) &&
           (LocaleCompare(filename+strlen(filename)-2,".Z") == 0)) ||
          ((strlen(filename) > 3) &&
           (LocaleCompare(filename+strlen(filename)-3,".gz") == 0)) ||
          ((strlen(filename) > 5) &&
           (LocaleCompare(filename+strlen(filename)-5,".svgz") == 0)))
        {
          image->blob->handle.gz=(gzFile) NULL;
          if (MagickConfirmAccess((type[0] == 'r' ? FileReadConfirmAccessMode :
                                   FileWriteConfirmAccessMode),filename,
                                  exception) != MagickFail)
            image->blob->handle.gz=gzopen(filename,type);
          if (image->blob->handle.gz != (gzFile) NULL)
            {
              image->blob->type=ZipStream;
              if (image->logging)
                (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                                      "  opened file %s as ZipStream image"
                                      " %p, blob %p",
                                      filename,image,image->blob);
            }
        }
      else
#endif
#if defined(HasBZLIB)
        if ((strlen(filename) > 4) &&
            (LocaleCompare(filename+strlen(filename)-4,".bz2") == 0))
          {
            image->blob->handle.bz=(BZFILE *) NULL;
            if (MagickConfirmAccess((type[0] == 'r' ? FileReadConfirmAccessMode :
                                     FileWriteConfirmAccessMode),filename,
                                    exception) != MagickFail)
              image->blob->handle.bz=(BZFILE *) BZ2_bzopen(filename,type);
            if (image->blob->handle.bz != (BZFILE *) NULL)
              {
                image->blob->type=BZipStream;
                if (image->logging)
                  (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                                        "  opened file %s as BZipStream image"
                                        " %p, blob %p",
                                        filename,image,image->blob);
              }
          }
        else
#endif
          if (image_info->file != (FILE *) NULL)
            {
              image->blob->handle.std=image_info->file;
              image->blob->type=FileStream;
              image->blob->exempt=MagickTrue;
              if (image->logging)
                (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                                      "  opened image_info->file (%d) as"
                                      " FileStream image %p, blob %p",
                                      fileno(image_info->file),image,image->blob);
            }
          else
            {
              image->blob->handle.std=(FILE *) NULL;
              if (MagickConfirmAccess((type[0] == 'r' ? FileReadConfirmAccessMode :
                                       FileWriteConfirmAccessMode),filename,
                                      exception) != MagickFail)
                image->blob->handle.std=(FILE *) fopen(filename,type);
              if (image->blob->handle.std != (FILE *) NULL)
                {
                  char
                    *env = NULL;

                  unsigned char
                    magick[MaxTextExtent];

                  size_t
                    count;

                  size_t
                    vbuf_size;

                  vbuf_size=image->blob->block_size;
                  if (0 != vbuf_size)
                    {
                      if (setvbuf(image->blob->handle.std,NULL,_IOFBF,vbuf_size) != 0)
                        {
                          if (image->logging)
                            (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                                                  "  setvbuf of %" MAGICK_SIZE_T_F
                                                  "u bytes returns failure!",
                                                  (MAGICK_SIZE_T) vbuf_size);
                        }
                      else
                        {
                          if (image->logging)
                            (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                                                  "  I/O buffer set to %"
                                                  MAGICK_SIZE_T_F "u bytes",
                                                  (MAGICK_SIZE_T) vbuf_size);
                        }
                    }
                  /*
                    Enable fsync-on-close mode if requested.
                  */
                  if (((WriteBlobMode == mode) || (WriteBinaryBlobMode == mode)) &&
                      (env = getenv("MAGICK_IO_FSYNC")))
                    {
                      if (LocaleCompare(env,"TRUE") == 0)
                        {
                          image->blob->fsync=MagickTrue;
                          if (image->logging)
                            (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                                                  "  fsync() on close requested");
                        }
                    }
                  image->blob->type=FileStream;
                  if (image->logging)
                    (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                                          "  opened file \"%s\" as FileStream image %p, blob %p",
                                          filename,image,image->blob);

                  if ((ReadBlobMode == mode) || (ReadBinaryBlobMode == mode))
                    {
                      /*
                        Read file header and check magick bytes.
                      */
                      (void) memset((void *) magick,0,MaxTextExtent);
                      count=fread(magick,1,MaxTextExtent,image->blob->handle.std);
                      (void) MagickFseek(image->blob->handle.std,
                                         -(magick_off_t) count,SEEK_CUR);
#if defined(POSIX)
                      /*
                        Discard any buffered input and adjust the
                        file pointer such that the next input
                        operation accesses the byte after the last
                        one read. This avoids possible problems if
                        the fseek()/rewind() implementations do not
                        implicitly empty the stdio input buffer.
                      */
                      (void) fflush(image->blob->handle.std);
#endif /* defined(POSIX) */
                      if (image->logging)
                        (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                                              "  read %" MAGICK_SIZE_T_F
                                              "u magic header bytes",
                                              (MAGICK_SIZE_T) count);
#if defined(HasZLIB)
                      if ((magick[0] == 0x1FU) && (magick[1] == 0x8BU) &&
                          (magick[2] == 0x08U))
                        {
                          (void) fclose(image->blob->handle.std);
                          image->blob->handle.gz=gzopen(filename,type);
                          if (image->blob->handle.gz != (gzFile) NULL)
                            {
                              image->blob->type=ZipStream;
                              if (image->logging)
                                (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                                                      "  reopened file \"%s\""
                                                      "as ZipStream image %p, blob %p",
                                                      filename,image,image->blob);
                            }
                        }
#endif
#if defined(HasBZLIB)
                      if (strncmp((char *) magick,"BZh",3) == 0)
                        {
                          (void) fclose(image->blob->handle.std);
                          image->blob->handle.bz=BZ2_bzopen(filename,type);
                          if (image->blob->handle.bz != (BZFILE *) NULL)
                            {
                              image->blob->type=BZipStream;
                              if (image->logging)
                                (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                                                      "  reopened file %s as"
                                                      " BZipStream image %p, blob %p",
                                                      filename,image,image->blob);
                            }
                        }
#endif
                    }
                }
            }
      if (image->blob->type == FileStream)
        {
          const char* env_val;

          if (*type == 'r')
            {
              /*
                Support reading from a file using memory mapping.
                  
                This code was used for years and definitely speeds
                re-reading of the same file, but it has been
                discovered that some operating systems (e.g. FreeBSD
                and Apple's OS-X) fail to perform automatic
                read-ahead for network files.  It will be disabled
                by default until we add a way to force read-ahead.
              */
              if (((env_val = getenv("MAGICK_MMAP_READ")) != NULL) &&
                  (LocaleCompare(env_val,"TRUE") == 0))
                {
                  const MagickInfo
                    *magick_info;
                
                  MagickStatStruct_t
                    attributes;

                  magick_info=GetMagickInfo(image_info->magick,&image->exception);
                  if ((magick_info != (const MagickInfo *) NULL) &&
                      magick_info->blob_support)
                    {
                      if ((MagickFstat(fileno(image->blob->handle.std),&attributes) >= 0) &&
                          (attributes.st_size > MinBlobExtent) &&
                          (attributes.st_size == (off_t) ((size_t) attributes.st_size)))
                        {
                          size_t
                            length;
                      
                          void
                            *blob;
                      
                          length=(size_t) attributes.st_size;

                          if (AcquireMagickResource(MapResource,length))
                            {
                              blob=MapBlob(fileno(image->blob->handle.std),ReadMode,0,length);
                              if (blob != (void *) NULL)
                                {
                                  /*
                                    Format supports blobs-- use memory-mapped I/O.
                                  */
                                  if (image_info->file != (FILE *) NULL)
                                    image->blob->exempt=MagickFalse;
                                  else
                                    {
                                      (void) fclose(image->blob->handle.std);
                                      image->blob->handle.std=(FILE *) NULL;
                                    }
                                  AttachBlob(image->blob,blob,length);
                                  image->blob->mapped=True;
                                }
                              else
                                {
                                  LiberateMagickResource(MapResource,length);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
  image->blob->status=MagickFalse;
  if (image->blob->type != UndefinedStream)
    image->blob->size=GetBlobSize(image);
  if (*type == 'r')
    {
      image->next=(Image *) NULL;
      image->previous=(Image *) NULL;
    }
  if (UndefinedStream == image->blob->type)
    {
      if (UndefinedException == exception->severity)
	ThrowException(exception,FileOpenError,UnableToOpenFile,filename);
      return MagickFail;
    }
  return MagickPass;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i n g B l o b                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PingBlob() returns all the attributes of an image or image sequence except
%  for the pixels.  It is much faster and consumes far less memory than
%  BlobToImage().  On failure, a NULL image is returned and exception
%  describes the reason for the failure.
%
%
%  The format of the PingBlob method is:
%
%      Image *PingBlob(const ImageInfo *image_info,const void *blob,
%        const size_t length,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o blob: The address of a character stream in one of the image formats
%      understood by GraphicsMagick.
%
%    o length: This size_t integer reflects the length in bytes of the blob.
%
%    o exception: Return any errors or warnings in this structure.
%
%
%
*/
MagickExport Image *PingBlob(const ImageInfo *image_info,const void *blob,
  const size_t length,ExceptionInfo *exception)
{
  Image
    *image;

  ImageInfo
    *clone_info;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  clone_info=CloneImageInfo(image_info);
  clone_info->ping=MagickTrue;
  image=BlobToImage(clone_info,blob,length,exception);
  DestroyImageInfo(clone_info);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadBlob() reads data from the blob or image file and returns it.  It
%  returns the number of bytes read.
%
%  The format of the ReadBlob method is:
%
%      size_t ReadBlob(Image *image,const size_t length,void *data)
%
%  A description of each parameter follows:
%
%    o count:  Method ReadBlob returns the number of bytes read.
%
%    o image: The image.
%
%    o length:  Specifies an integer representing the number of bytes
%      to read from the file.
%
%    o data:  Specifies an area to place the information requested from
%      the file.
%
%
*/
MagickExport size_t ReadBlob(Image *image,const size_t length,void *data)
{
  size_t
    count;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  assert(data != (void *) NULL);

  count=0;
  switch (image->blob->type)
  {
    case UndefinedStream:
      break;
    case FileStream:
    case StandardStream:
    case PipeStream:
    {
      if (length == 1)
        {
          int
            c;

          if ((c=getc(image->blob->handle.std)) != EOF)
            {
              *((unsigned char *)data)=(unsigned char) c;
              count=1;
            }
          else
            {
              count=0;
            }
        }
      else
        {
          count=fread(data,1,length,image->blob->handle.std);
        }
      break;
    }
    case ZipStream:
    {
#if defined(HasZLIB)
      size_t
        i;

      for (i=0; i < length; i+=count)
        {
          size_t
            remaining;

          unsigned int
            amount;

          remaining=length - i;
          if (remaining > image->blob->block_size)
            amount=(unsigned int) image->blob->block_size;
          else
            amount=(unsigned int) remaining;

          count=gzread(image->blob->handle.gz,
                       (void *) ((unsigned char *) data+i),amount);
          if (count <= 0)
            break;
        }
      count=i;
#endif
      break;
    }
    case BZipStream:
    {
#if defined(HasBZLIB)
      size_t
        i;

      for (i=0; i < length; i+=count)
        {
          size_t
            remaining;

          int
            amount;

          remaining=length - i;
          if (remaining > image->blob->block_size)
            amount=(int) image->blob->block_size;
          else
            amount=(int) remaining;

          count=BZ2_bzread(image->blob->handle.bz,
                           (void *) ((unsigned char *) data+i),amount);
          if (count <= 0)
            break;
        }
      count=i;
#endif
      break;
    }
    case BlobStream:
    {
      void
        *source_void = 0;

      const unsigned char
        *source;

      count=ReadBlobStream(image,length,&source_void);
      source=source_void;
      if (count <= 10)
        {
          register size_t
            i;

          register unsigned char
            *target=(unsigned char*) data;

          for(i=count; i > 0; i--)
            {
              *target=*source;
              target++;
              source++;
            }
        }
      else 
        (void) memcpy(data,source,count);
      break;
    }
  }
  return(count);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b Z C                                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadBlobZC() reads data from the blob or image file and returns it.  It
%  returns the number of bytes read.  Provision is made for a "zero-copy"
%  transfer if the blob data is already in memory.
%
%  This method is currently EXPERIMENTAL!
%
%  The format of the ReadBlob method is:
%
%      size_t ReadBlob(Image *image,const size_t length,void *data)
%
%  A description of each parameter follows:
%
%    o count:  Method ReadBlob returns the number of bytes read.
%
%    o image: The image.
%
%    o length:  Specifies an integer representing the number of bytes
%      to read from the file.
%
%    o data:  Specifies an area to place the information requested from
%      the file.  If the data may be accessed without a copy, then
%      the provided pointer is updated to point to the location of
%      the data in memory, and no copy is performed.
%
%
*/
MagickExport  size_t ReadBlobZC(Image *image,const size_t length,void **data)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  assert(data != (void *) NULL);

  if (image->blob->type == BlobStream)
    return (ReadBlobStream(image,length,data));

  assert(*data != (void *) NULL);
  return ReadBlob(image,length,*data);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b B y t e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobByte reads a single byte from the image file and returns it.
%  An EOF value is returned if there are no more bytes available on the input
%  stream (similar to stdio's fgetc()).
%
%  The format of the ReadBlobByte method is:
%
%      int ReadBlobByte(Image *image)
%
%  A description of each parameter follows.
%
%    o value: Method ReadBlobByte returns an integer read from the file.
%
%    o image: The image.
%
%
*/
MagickExport int ReadBlobByte(Image *image)
{
  unsigned char
    c;
  
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  
  switch (image->blob->type)
    {
    case FileStream:
    case StandardStream:
    case PipeStream:
      {
        return getc(image->blob->handle.std);
      }
    case BlobStream:
      {
        if (image->blob->offset < (magick_off_t) image->blob->length)
          {
            c=*((unsigned char *)image->blob->data+image->blob->offset);
            image->blob->offset++;
            return (c);
          }
        image->blob->eof=True;
        break;
      }
    default:
      {
        /* Do things the slow way */
        if (ReadBlob(image,1,&c) == 1)
          return (c);
      }
    }
  return(EOF);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b L S B D o u b l e                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobLSBDouble reads a double value as a 64 bit quantity in
%  least-significant byte first order.  If insufficient octets are available
%  to compose the value, then zero is returned, and EOFBlob() may be used to
%  detect that the input is in EOF state.
%
%  The format of the ReadBlobLSBDouble method is:
%
%      double ReadBlobLSBDouble(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobLSBDouble returns a double read from
%      the file.  Zero is returned if insufficient data is available.
%
%    o image: The image.
%
%
*/
MagickExport double ReadBlobLSBDouble(Image * image)
{
  union
  {
    double d;
    unsigned char chars[8];
  } dbl_buffer;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(sizeof(dbl_buffer) == sizeof(double));

  if (ReadBlob(image, 8, dbl_buffer.chars) != 8)
    dbl_buffer.d = 0.0;

#if defined(WORDS_BIGENDIAN)
  MagickSwabDouble(&dbl_buffer.d);
#endif

  return (dbl_buffer.d);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b L S B D o u b l e s                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobLSBDoubles reads an array of little-endian 64-bit "double"
%  values from the file or BLOB and returns them in native order.
%
%  The format of the ReadBlobLSBDoubles method is:
%
%      size_t ReadBlobLSBDoubles(Image *image, size_t octets, double *data)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobLSBDoubles returns the number of octets
%        which were actually read.
%
%    o image: The image.
%
%    o octets: The number of bytes of data to read.
%
%    o data: The address of a user-supplied buffer in which to write
%        the decoded data.  The buffer must be suitably aligned for the
%        data type.
%
*/
MagickExport size_t ReadBlobLSBDoubles(Image *image, size_t octets, double *data)
{
  size_t
    octets_read;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(data != (double *) NULL);

  octets_read=ReadBlob(image,octets,data);
#if defined(WORDS_BIGENDIAN)
  if (octets_read >= sizeof(double))
    MagickSwabArrayOfDouble(data,(octets_read+sizeof(double)-1)/sizeof(double));
#endif

  return octets_read;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b L S B L o n g                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobLSBLong reads an unsigned 32 bit value in least-significant
%  byte first order.  If insufficient octets are available to compose the
%  value, then zero is returned, and EOFBlob() may be used to detect that
%  the input is in EOF state.
%
%  The format of the ReadBlobLSBLong method is:
%
%      magick_uint32_t ReadBlobLSBLong(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobLSBLong returns an unsigned 32-bit value from
%      the file.  Zero is returned if insufficient data is available.
%
%    o image: The image.
%
%
*/
MagickExport magick_uint32_t ReadBlobLSBLong(Image *image)
{
  unsigned char
    buffer[4];

  magick_uint32_t
    value;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if (ReadBlob(image,4,buffer) != 4)
    return(0U);

  value=buffer[3] << 24;
  value|=buffer[2] << 16;
  value|=buffer[1] << 8;
  value|=buffer[0];
  return(value & 0xffffffff);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b L S B S i g n e d L o n g                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobLSBSignedLong reads an signed 32 bit value in least-significant
%  byte first order.  If insufficient octets are available to compose the
%  value, then zero is returned, and EOFBlob() may be used to detect that
%  the input is in EOF state.
%
%  The format of the ReadBlobLSBSignedLong method is:
%
%      magick_uint32_t ReadBlobLSBSignedLong(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobLSBSignedLong returns an signed 32-bit value from
%      the file.  Zero is returned if insufficient data is available.
%
%    o image: The image.
%
%
*/
MagickExport magick_int32_t ReadBlobLSBSignedLong(Image *image)
{
  unsigned char
    buffer[4];

  MagickInt32Union
    value;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if (ReadBlob(image,4,buffer) != 4)
    return(0U);

  value.uint32=buffer[3] << 24;
  value.uint32|=buffer[2] << 16;
  value.uint32|=buffer[1] << 8;
  value.uint32|=buffer[0];
  value.uint32&=0xffffffff;
  return value.int32;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b L S B L o n g s                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobLSBLongs reads an array of little-endian 32-bit "long"
%  values from the file or BLOB and returns them in native order.
%
%  The format of the ReadBlobLSBLongs method is:
%
%      size_t ReadBlobLSBLongs(Image *image, size_t octets,
%                              magick_uint32_t *data)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobLSBLongs returns the number of octets
%        which were actually read.
%
%    o image: The image.
%
%    o octets: The number of bytes of data to read.
%
%    o data: The address of a user-supplied buffer in which to write
%        the decoded data.  The buffer must be suitably aligned for the
%        data type.
%
*/
MagickExport size_t ReadBlobLSBLongs(Image *image, size_t octets,
                                     magick_uint32_t *data)
{
  size_t
    octets_read;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(data != (magick_uint32_t *) NULL);

  octets_read=ReadBlob(image,octets,data);
#if defined(WORDS_BIGENDIAN)
  if (octets_read >= sizeof(magick_uint32_t))
    MagickSwabArrayOfUInt32(data,(octets_read+sizeof(magick_uint32_t)-1)/sizeof(magick_uint32_t));
#endif

  return octets_read;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b L S B S h o r t                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobLSBShort reads a 16-bit unsigned value in least-significant
%  byte first order.  If insufficient octets are available to compose the
%  value, then zero is returned, and EOFBlob() may be used to detect that
%  the input is in EOF state.
%
%  The format of the ReadBlobLSBShort method is:
%
%      magick_uint16_t ReadBlobLSBShort(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobLSBShort returns an unsigned 16-bit value
%      read from the file.  Zero is returned if insufficient data is available.
%
%    o image: The image.
%
%
*/
MagickExport magick_uint16_t ReadBlobLSBShort(Image *image)
{
  unsigned char
    buffer[2];

  magick_uint16_t
    value;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if (ReadBlob(image,2,buffer) != 2)
    return(0U);

  value=buffer[1] << 8;
  value|=buffer[0];
  return(value & 0xffff);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b L S B S i g n e d S h o r t                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobLSBSignedShort reads a 16-bit signed value in
%  least-significant byte first order.  If insufficient octets are available
%  to compose the value, then zero is returned, and EOFBlob() may be used to
%  detect that the input is in EOF state.
%
%  The format of the ReadBlobLSBSignedShort method is:
%
%      magick_int16_t ReadBlobLSBSignedShort(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobLSBSignedShort returns an signed 16-bit value
%      read from the file.  Zero is returned if insufficient data is available.
%
%    o image: The image.
%
%
*/
MagickExport magick_int16_t ReadBlobLSBSignedShort(Image *image)
{
  unsigned char
    buffer[2];

  MagickInt16Union
    value;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if (ReadBlob(image,2,buffer) != 2)
    return(0U);

  value.uint16=buffer[1] << 8;
  value.uint16|=buffer[0];
  value.uint16&=0xffff;
  return value.int16;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b L S B F l o a t                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobLSBFloat reads a float value as a 32 bit quantity in
%  least-significant byte first order.  If insufficient octets are available
%  to compose the value, then zero is returned, and EOFBlob() may be used to
%  detect that the input is in EOF state.
%
%  The format of the ReadBlobLSBFloat method is:
%
%      float ReadBlobLSBFloat(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobLSBFloat returns a float read from
%      the file.  Zero is returned if insufficient data is available.
%
%    o image: The image.
%
%
*/
MagickExport float ReadBlobLSBFloat(Image * image)
{
  union
  {
    float f;
    unsigned char chars[4];
  } flt_buffer;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(sizeof(flt_buffer) == sizeof(float));

  if (ReadBlob(image, 4, flt_buffer.chars) != 4)
    flt_buffer.f = 0.0;

#if defined(WORDS_BIGENDIAN)
  MagickSwabFloat(&flt_buffer.f);
#endif

  return (flt_buffer.f);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b L S B F l o a t s                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobLSBFloats reads an array of little-endian 32-bit "float"
%  values from the file or BLOB and returns them in native order.
%
%  The format of the ReadBlobLSBFloats method is:
%
%      size_t ReadBlobLSBFloats(Image *image, size_t octets, float *data)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobLSBFloats returns the number of octets
%        which were actually read.
%
%    o image: The image.
%
%    o octets: The number of bytes of data to read.
%
%    o data: The address of a user-supplied buffer in which to write
%        the decoded data.  The buffer must be suitably aligned for the
%        data type.
%
*/
MagickExport size_t ReadBlobLSBFloats(Image *image, size_t octets, float *data)
{
  size_t
    octets_read;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(data != (float *) NULL);

  octets_read=ReadBlob(image,octets,data);
#if defined(WORDS_BIGENDIAN)
  if (octets_read >= sizeof(float))
    MagickSwabArrayOfFloat(data,(octets_read+sizeof(float)-1)/sizeof(float));
#endif

  return octets_read;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b M S B F l o a t                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobMSBFloat reads a float value as a 32 bit quantity in
%  most-significant byte first order.  If insufficient octets are available
%  to compose the value, then zero is returned, and EOFBlob() may be used to
%  detect that the input is in EOF state.
%
%  The format of the ReadBlobMSBFloat method is:
%
%      float ReadBlobMSBFloat(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobMSBFloat returns a float read from
%      the file.  Zero is returned if insufficient data is available.
%
%    o image: The image.
%
%
*/
MagickExport float ReadBlobMSBFloat(Image * image)
{
  union
  {
    float f;
    unsigned char chars[4];
  } flt_buffer;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(sizeof(flt_buffer) == sizeof(float));

  if (ReadBlob(image, 4, flt_buffer.chars) != 4)
    flt_buffer.f = 0.0;

#if !defined(WORDS_BIGENDIAN)
  MagickSwabFloat(&flt_buffer.f);
#endif

  return (flt_buffer.f);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b M S B F l o a t s                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobMSBFloats reads an array of big-endian 32-bit "float"
%  values from the file or BLOB and returns them in native order.
%
%  The format of the ReadBlobMSBFloats method is:
%
%      size_t ReadBlobMSBFloats(Image *image, size_t octets, float *data)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobMSBFloats returns the number of octets
%        which were actually read.
%
%    o image: The image.
%
%    o octets: The number of bytes of data to read.
%
%    o data: The address of a user-supplied buffer in which to write
%        the decoded data.  The buffer must be suitably aligned for the
%        data type.
%
*/
MagickExport size_t ReadBlobMSBFloats(Image *image, size_t octets, float *data)
{
  size_t
    octets_read;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(data != (float *) NULL);

  octets_read=ReadBlob(image,octets,data);
#if !defined(WORDS_BIGENDIAN)
  if (octets_read >= sizeof(float))
    MagickSwabArrayOfFloat(data,(octets_read+sizeof(float)-1)/sizeof(float));
#endif

  return octets_read;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b M S B D o u b l e                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobMSBDouble reads a double value as a 64 bit quantity in
%  most-significant byte first order.  If insufficient octets are available
%  to compose the value, then zero is returned, and EOFBlob() may be used
%  to detect that the input is in EOF state.
%
%  The format of the ReadBlobMSBDouble method is:
%
%      double ReadBlobMSBDouble(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobMSBDouble returns a double read from
%      the file.  Zero is returned if insufficient data is available.
%
%    o image: The image.
%
%
*/
MagickExport double ReadBlobMSBDouble(Image * image)
{
  union
  {
    double d;
    unsigned char chars[8];
  } dbl_buffer;
  
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(sizeof(dbl_buffer) == sizeof(double));

  if (ReadBlob(image, 8, dbl_buffer.chars) != 8)
    dbl_buffer.d = 0.0;

#if !defined(WORDS_BIGENDIAN)
  MagickSwabDouble(&dbl_buffer.d);
#endif

  return (dbl_buffer.d);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b M S B D o u b l e s                                        %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobMSBDoubles reads an array of big-endian 64-bit "double"
%  values from the file or BLOB and returns them in native order.
%
%  The format of the ReadBlobMSBDoubles method is:
%
%      size_t ReadBlobMSBDoubles(Image *image, size_t octets, double *data)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobMSBDoubles returns the number of octets
%        which were actually read.
%
%    o image: The image.
%
%    o octets: The number of bytes of data to read.
%
%    o data: The address of a user-supplied buffer in which to write
%        the decoded data.  The buffer must be suitably aligned for the
%        data type.
%
*/
MagickExport size_t ReadBlobMSBDoubles(Image *image, size_t octets, double *data)
{
  size_t
    octets_read;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(data != (double *) NULL);

  octets_read=ReadBlob(image,octets,data);
#if !defined(WORDS_BIGENDIAN)
  if (octets_read > 0)
    MagickSwabArrayOfDouble(data,(octets_read+sizeof(double)-1)/sizeof(double));
#endif

  return octets_read;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b M S B L o n g                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadBlobMSBLong() reads a 32 bit unsigned value in most-significant byte
%  first order.  If insufficient octets are available to compose the value,
%  then zero is returned, and EOFBlob() may be used to detect that the input
%  is in EOF state.
%
%  The format of the ReadBlobMSBLong method is:
%
%      magick_uint32_t ReadBlobMSBLong(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobMSBLong returns an unsigned 32-bit value
%      read from the file.  Zero is returned if insufficient data is available.
%
%    o image: The image.
%
%
%
*/
MagickExport magick_uint32_t ReadBlobMSBLong(Image *image)
{
  unsigned char
    buffer[4];

  magick_uint32_t
    value;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if (ReadBlob(image,4,buffer) != 4)
    return(0U);

  value=buffer[0] << 24;
  value|=buffer[1] << 16;
  value|=buffer[2] << 8;
  value|=buffer[3];
  return(value & 0xffffffff);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b M S B S i g n e d L o n g                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadBlobMSBSignedLong() reads a 32 bit signed value in most-significant byte
%  first order.  If insufficient octets are available to compose the value,
%  then zero is returned, and EOFBlob() may be used to detect that the input
%  is in EOF state.
%
%  The format of the ReadBlobMSBSignedLong method is:
%
%      magick_int32_t ReadBlobMSBSignedLong(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobMSBSignedLong returns a signed 32-bit value
%      read from the file.  Zero is returned if insufficient data is available.
%
%    o image: The image.
%
%
%
*/
MagickExport magick_int32_t ReadBlobMSBSignedLong(Image *image)
{
  unsigned char
    buffer[4];

  MagickInt32Union
    value;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if (ReadBlob(image,4,buffer) != 4)
    return(0);

  value.uint32=buffer[0] << 24;
  value.uint32|=buffer[1] << 16;
  value.uint32|=buffer[2] << 8;
  value.uint32|=buffer[3];
  value.uint32&=0xffffffff;
  return value.int32;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b M S B S h o r t                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobMSBShort reads a 16 bit unsigned value in most-significant
%  byte first order.  If insufficient octets are available to compose the
%  value, then zero is returned, and EOFBlob() may be used to detect that
%  the input is in EOF state.
%
%  The format of the ReadBlobMSBShort method is:
%
%      magick_uint16_t ReadBlobMSBShort(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobMSBShort returns an unsigned 16-bit value read
%      from the file.  Zero is returned if insufficient data is available.
%
%    o image: The image.
%
%
*/
MagickExport magick_uint16_t ReadBlobMSBShort(Image *image)
{
  unsigned char
    buffer[2];

  magick_uint16_t
    value;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if (ReadBlob(image,2,buffer) != 2)
    return(0U);

  value=buffer[0] << 8;
  value|=buffer[1];
  return(value & 0xffff);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b M S B S i g n e d S h o r t                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobMSBSignedShort reads a 16 bit signed value in
%  most-significant byte first order.  If insufficient octets are available
%  to compose the value, then zero is returned, and EOFBlob() may be used
%  to detect that the input is in EOF state.
%
%  The format of the ReadBlobMSBSignedShort method is:
%
%      magick_uint16_t ReadBlobMSBSignedShort(Image *image)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobMSBSignedShort returns an signed 16-bit value read
%      from the file.  Zero is returned if insufficient data is available.
%
%    o image: The image.
%
%
*/
MagickExport magick_int16_t ReadBlobMSBSignedShort(Image *image)
{
  unsigned char
    buffer[2];

  MagickInt16Union
    value;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  if (ReadBlob(image,2,buffer) != 2)
    return(0U);

  value.uint16=buffer[0] << 8;
  value.uint16|=buffer[1];
  value.uint16&=0xffff;
  return value.int16;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b M S B S h o r t s                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobMSBShorts reads an array of big-endian 16-bit "short"
%  values from the file or BLOB and returns them in native order.
%
%  The format of the ReadBlobMSBShorts method is:
%
%      size_t ReadBlobMSBShorts(Image *image, size_t octets,
%                               magick_uint16_t *data)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobMSBShorts returns the number of octets
%        which were actually read.
%
%    o image: The image.
%
%    o octets: The number of bytes of data to read.
%
%    o data: The address of a user-supplied buffer in which to write
%        the decoded data.  The buffer must be suitably aligned for the
%        data type.
%
*/
MagickExport size_t ReadBlobMSBShorts(Image *image, size_t octets,
                                      magick_uint16_t *data)
{
  size_t
    octets_read;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(data != (magick_uint16_t *) NULL);

  octets_read=ReadBlob(image,octets,data);
#if !defined(WORDS_BIGENDIAN)
  if (octets_read >= sizeof(magick_uint32_t))
    MagickSwabArrayOfUInt16(data,(octets_read+sizeof(magick_uint16_t)-1)/sizeof(magick_uint16_t));
#endif

  return octets_read;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d B l o b S t r i n g                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadBlobString() reads characters from a blob or file until a newline
%  character is read or an end-of-file condition is encountered.  Any
%  terminating newline ('\n') or carriage return ('\r') characters will be
%  removed.  The string buffer is assured to be null terminated.  NULL
%  is returned if zero bytes were read, otherwise a pointer to the string
%  buffer is returned.
%
%  The format of the ReadBlobString method is:
%
%      char *ReadBlobString(Image *image,char *string)
%
%  A description of each parameter follows:
%
%    o status:  Method ReadBlobString returns the string on success, otherwise,
%      a null is returned.
%
%    o image: The image.
%
%    o string: The address of a character buffer, which must be at least
%              MaxTextExtent bytes long.
%
%
*/
MagickExport char *ReadBlobString(Image *image,char *string)
{
  int
    c;

  register unsigned int
    i;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  for (i=0; i < (MaxTextExtent-1); i++)
  {
    c=ReadBlobByte(image);
    if (c == EOF)
      {
        if (i == 0)
          return((char *) NULL);
        break;
      }
    string[i]=c;
    if ((string[i] == '\n') || (string[i] == '\r'))
      break;
  }
  string[i]='\0';
  return(string);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e f e r e n c e B l o b                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReferenceBlob() increments the reference count associated with the pixel
%  blob, returning a pointer to the blob.
%
%  The format of the ReferenceBlob method is:
%
%      BlobInfo ReferenceBlob(BlobInfo *blob_info)
%
%  A description of each parameter follows:
%
%    o blob_info: The blob_info.
%
%
*/
MagickExport BlobInfo *ReferenceBlob(BlobInfo *blob)
{
  assert(blob != (BlobInfo *) NULL);
  assert(blob->signature == MagickSignature);
  LockSemaphoreInfo(blob->semaphore);
  blob->reference_count++;
  (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                        "Reference blob: blob %p, ref %lu",
                        blob,blob->reference_count);
  UnlockSemaphoreInfo(blob->semaphore);
  return(blob);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  S e e k B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SeekBlob() sets the offset in bytes from the beginning of a blob or file
%  and returns the resulting offset.
%
%  The format of the SeekBlob method is:
%
%      magick_off_t SeekBlob(Image *image,const magick_off_t offset,
%                            const int whence)
%
%  A description of each parameter follows:
%
%    o offset:  Method SeekBlob returns the offset from the beginning
%      of the file or blob.
%
%    o image: The image.
%
%    o offset:  Specifies an integer representing the offset in bytes.
%
%    o whence:  Specifies an integer representing how the offset is
%      treated relative to the beginning of the blob as follows:
%
%        SEEK_SET  Set position equal to offset bytes.
%        SEEK_CUR  Set position to current location plus offset.
%        SEEK_END  Set position to EOF plus offset.
%
%
*/
MagickExport magick_off_t SeekBlob(Image *image,const magick_off_t offset,
  const int whence)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  switch (image->blob->type)
  {
    case UndefinedStream:
      break;
    case FileStream:
    {
      if (MagickFseek(image->blob->handle.std,offset,whence) < 0)
        return(-1);
      image->blob->offset=TellBlob(image);
      break;
    }
    case StandardStream:
    case PipeStream:
      return(-1);
    case ZipStream:
    {
#if defined(HasZLIB)
      if (gzseek(image->blob->handle.gz,(off_t) offset,whence) < 0)
        return(-1);
#endif
      image->blob->offset=TellBlob(image);
      break;
    }
    case BZipStream:
      return(-1);
    case BlobStream:
    {
      switch (whence)
      {
        case SEEK_SET:
        default:
        {
          if (offset < 0)
            return(-1);
          image->blob->offset=offset;
          break;
        }
        case SEEK_CUR:
        {
          if ((image->blob->offset+offset) < 0)
            return(-1);
          image->blob->offset+=offset;
          break;
        }
        case SEEK_END:
        {
          if ((magick_off_t)
              (image->blob->offset+image->blob->length+offset) < 0)
            return(-1);
          image->blob->offset=image->blob->length+offset;
          break;
        }
      }
      if (image->blob->offset <= (magick_off_t) image->blob->length)
        image->blob->eof=MagickFalse;
      else
        if (image->blob->mapped)
          return(-1);
        else
          {
            image->blob->extent=image->blob->offset+image->blob->quantum;
            MagickReallocMemory(unsigned char *,image->blob->data,image->blob->extent+1);
            (void) SyncBlob(image);
            if (image->blob->data == (unsigned char *) NULL)
              {
                DetachBlob(image->blob);
                return(-1);
              }
          }
      break;
    }
  }
  return(image->blob->offset);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t B l o b C l o s a b l e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetBlobClosable() enables closing the blob if MagickTrue is passed, and
%  exempts the blob from being closed if False is passed.  Blobs are closable
%  by default (default MagickTrue).
%
%  The format of the SetBlobClosable method is:
%
%      void SetBlobClosable(Image *image, MagickBool closeable)
%
%  A description of each parameter follows:
%
%    o image: Image to update
%
%    o closeable: Set to FALSE in order to disable closing the blob.
%
*/
MagickExport void SetBlobClosable(Image *image, MagickBool closeable)
{
  assert(image != (const Image *) NULL);
  assert(image->blob != (const BlobInfo *) NULL);
  image->blob->exempt = (closeable != MagickFalse);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t B l o b T e m p o r a r y                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetBlobTemporary() sets a boolean flag (default False) to specify if
%  the file associated with the blob is a temporary file and should be
%  removed when the associated image is destroyed. 
%
%  The format of the SetBlobTemporary method is:
%
%      void SetBlobTemporary(Image *image, MagickBool isTemporary)
%
%  A description of each parameter follows:
%
%    o image: Image to update
%
%    o isTemporary: Set to True to indicate that the file associated with
%        the blob is temporary.
%
*/
MagickExport void SetBlobTemporary(Image *image, MagickBool isTemporary)
{
  assert(image != (const Image *) NULL);
  assert(image->blob != (const BlobInfo *) NULL);
  image->blob->temporary = isTemporary;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  S y n c B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SyncBlob() flushes the datastream if it is a file or synchonizes the data
%  attributes if it is an blob.
%
%  The format of the SyncBlob method is:
%
%      int SyncBlob(Image *image)
%
%  A description of each parameter follows:
%
%    o status:  Method SyncBlob returns 0 on success; otherwise,  it
%      returns -1 and set errno to indicate the error.
%
%    o image: The image.
%
%
*/
static int SyncBlob(Image *image)
{
  int
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);

#if 0
  {
    register Image
      *p;

    /*
      FIXME: It is not clear why doing a sync on the blob stream
      should try to propogate the blob stream object across the whole
      image list.  Note that code below is not yet using the blob from
      the current image (as desired).
    */
    for ( p=GetFirstImageInList(image); p != (Image *) NULL;
	  p=SyncNextImageInList(p));
  }
#endif

  status=0;
  switch (image->blob->type)
  {
    case UndefinedStream:
      break;
    case FileStream:
    case StandardStream:
    case PipeStream:
    {
      status=fflush(image->blob->handle.std);
      break;
    }
    case ZipStream:
    {
#if defined(HasZLIB)
      status=gzflush(image->blob->handle.gz,Z_SYNC_FLUSH);
#endif
      break;
    }
    case BZipStream:
    {
#if defined(HasBZLIB)
      status=BZ2_bzflush(image->blob->handle.bz);
#endif
      break;
    }
    case BlobStream:
      break;
  }
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  T e l l B l o b                                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TellBlob() obtains the current value of the blob or file position.
%
%  The format of the TellBlob method is:
%
%      magick_off_t TellBlob(const Image *image)
%
%  A description of each parameter follows:
%
%    o offset:  Method TellBlob returns the current value of the blob or
%      file position success; otherwise, it returns -1 and sets errno to
%      indicate the error.
%
%    o image: The image.
%
%
*/
MagickExport magick_off_t TellBlob(const Image *image)
{
  magick_off_t
    offset;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  offset=(-1);
  switch (image->blob->type)
  {
    case UndefinedStream:
      break;
    case FileStream:
    {
      offset=MagickFtell(image->blob->handle.std);
      break;
    }
    case StandardStream:
    case PipeStream:
    case ZipStream:
    {
#if defined(HasZLIB)
      offset=gztell(image->blob->handle.gz);
#endif
      break;
    }
    case BZipStream:
      break;
    case BlobStream:
    {
      offset=image->blob->offset;
      break;
    }
  }
  return(offset);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  U n m a p B l o b                                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  UnmapBlob() deallocates the binary large object previously allocated with
%  the MapBlob method.
%
%  The format of the UnmapBlob method is:
%
%      MagickPassFail UnmapBlob(void *map,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method UnmapBlob returns MagickPass on success; otherwise,
%      it returns MagickFail and sets errno to indicate the error.
%
%    o map: The address  of the binary large object.
%
%    o length: The length of the binary large object.
%
%
*/
MagickExport MagickPassFail UnmapBlob(void *map,const size_t length)
{
#if defined(HAVE_MMAP_FILEIO)
  int
    status;

  (void) LogMagickEvent(BlobEvent,GetMagickModule(),
    "Munmap file mapping at address %p and length %"
                        MAGICK_SIZE_T_F "u",
    map,(MAGICK_SIZE_T) length);
  status=MagickMunmap(map,length);
  return(status == 0);
#else
  return(MagickFail);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b                                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteBlob() writes data to a blob or image file.  It returns the number of
%  bytes written.
%
%  The format of the WriteBlob method is:
%
%      size_t WriteBlob(Image *image,const size_t length,const void *data)
%
%  A description of each parameter follows:
%
%    o count:  Method WriteBlob returns the number of bytes written to the
%      blob.
%
%    o image: The image.
%
%    o length:  Specifies an integer representing the number of bytes to
%      write to the file.
%
%    o data:  The address of the data to write to the blob or file.
%
%
*/
MagickExport size_t WriteBlob(Image *image,const size_t length,const void *data)
{
  size_t
    count;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(data != (const char *) NULL);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  count=length;
  switch (image->blob->type)
  {
    case UndefinedStream:
      break;
    case FileStream:
    case StandardStream:
    case PipeStream:
    {
      if (length == 1)
        {
          if((putc((int)*((unsigned char *)data),image->blob->handle.std)) != EOF)
            count=1;
          else
            count=0;
        }
      else
        {
          count=fwrite((char *) data,1,length,image->blob->handle.std);
        }
      break;
    }
    case ZipStream:
    {
#if defined(HasZLIB)
      size_t
        i;

      for (i=0; i < length; i+=count)
        {
          size_t
            remaining;

          unsigned int
            amount;

          remaining=length - i;
          if (remaining > image->blob->block_size)
            amount=(unsigned int) image->blob->block_size;
          else
            amount=(unsigned int) remaining;

          count=gzwrite(image->blob->handle.gz,
                        (void *) ((unsigned char *) data+i),amount);
          if (count <= 0)
            break;
        }
      count=i;
#endif
      break;
    }
    case BZipStream:
    {
#if defined(HasBZLIB)
      size_t
        i;

      for (i=0; i < length; i+=count)
        {
          size_t
            remaining;

          int
            amount;

          remaining=length - i;
          if (remaining > image->blob->block_size)
            amount=(int) image->blob->block_size;
          else
            amount=(int) remaining;

          count=BZ2_bzwrite(image->blob->handle.gz,
                            (void *) ((unsigned char *) data+i),amount);
          if (count <= 0)
            break;
        }
      count=i;
#endif
      break;
    }
    case BlobStream:
    {
      count=WriteBlobStream(image,length,data);
      break;
    }
  }
  return(count);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b B y t e                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobByte writes an integer to a blob.  It returns the number of
%  bytes written (either 0 or 1);
%
%  The format of the WriteBlobByte method is:
%
%      size_t WriteBlobByte(Image *image,const unsigned int value)
%
%  A description of each parameter follows.
%
%    o count:  Method WriteBlobByte returns the number of bytes written.
%
%    o image: The image.
%
%    o value: Specifies the value to write.
%
%
*/
MagickExport size_t WriteBlobByte(Image *image,const magick_uint8_t value)
{
  unsigned char
    c;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  switch (image->blob->type)
    {
    case FileStream:
    case StandardStream:
    case PipeStream:
      {
        if(putc((int) value,image->blob->handle.std) != EOF)
          return 1;
        return 0;
      }
      /* case BlobStream: TBD */
    default:
      {
        c=(unsigned char) value;
        return(WriteBlob(image,1,&c));
      }
    }

}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b F i l e                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobFile writes the content of a disk file to a blob stream.
%  MagickPass is returned if the file is copied successfully.
%
%  The format of the WriteBlobFile method is:
%
%     MagickPassFail WriteBlobFile(Image *image,const char *filename)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o filename: The filename to copy to blob.
%
%
*/
MagickExport MagickPassFail WriteBlobFile(Image *image,const char *filename)
{
  int
    file;

  MagickPassFail
    status=MagickFail;

  if (MagickConfirmAccess(FileReadConfirmAccessMode,filename,
			  &image->exception) == MagickFail)
    return MagickFail;
  file=open(filename,O_RDONLY | O_BINARY,0777);
  if (file != -1)
    {
      MagickStatStruct_t
        attributes;

      /* st_size has type off_t */
      if ((MagickFstat(file,&attributes) == 0) &&
          (attributes.st_size == (off_t) ((size_t) attributes.st_size)) &&
          (attributes.st_size > (off_t) ((size_t) 0)))
        {
          unsigned char
            *buffer;

          size_t
            block_size,
            length;

          register size_t
            i;

          MAGICK_POSIX_IO_SIZE_T
            count;

          block_size=image->blob->block_size;
          length=(size_t) attributes.st_size;

          if (length < block_size)
            count = (MAGICK_POSIX_IO_SIZE_T) length;
          else
            count = (MAGICK_POSIX_IO_SIZE_T) block_size;

          buffer=MagickAllocateMemory(unsigned char *,count);
          i=0;
          if (buffer != (unsigned char *) NULL)
            {
              ssize_t
                result;

              for (i=0; i < length; i+=result)
                {
                  result=read(file,buffer,count);
                  if (result <= 0)
                    break;
                  if (WriteBlob(image,result,buffer) != (size_t) result)
                    break;
                }
              MagickFreeMemory(buffer);
            }
          if (i == length)
            status = MagickPass;
        }
      (void) close(file);
    }
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b L S B L o n g                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobLSBLong writes a 32 bit quantity in least-significant byte
%  first order.
%
%  The format of the WriteBlobLSBLong method is:
%
%      size_t WriteBlobLSBLong(Image *image,const magick_uint32_t value)
%
%  A description of each parameter follows.
%
%    o count: Method WriteBlobLSBLong returns the number of bytes written
%             (should be 4).
%
%    o image: The image.
%
%    o value: Specifies the value to write.
%
%
*/
MagickExport size_t WriteBlobLSBLong(Image *image,const magick_uint32_t value)
{
  unsigned char
    buffer[4];

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  buffer[0]=(unsigned char) value;
  buffer[1]=(unsigned char) (value >> 8);
  buffer[2]=(unsigned char) (value >> 16);
  buffer[3]=(unsigned char) (value >> 24);
  return(WriteBlob(image,4,buffer));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b L S B S i g n e d L o n g                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobLSBSignedLong writes a 32 bit signed quantity in
%  least-significant byte first order.
%
%  The format of the WriteBlobLSBSignedLong method is:
%
%      size_t WriteBlobLSBSignedLong(Image *image,const magick_int32_t value)
%
%  A description of each parameter follows.
%
%    o count: Method WriteBlobLSBLong returns the number of bytes written
%             (should be 4).
%
%    o image: The image.
%
%    o value: Specifies the value to write.
%
%
*/
MagickExport size_t WriteBlobLSBSignedLong(Image *image,const magick_int32_t value)
{
  unsigned char
    buffer[4];

  MagickInt32Union
    uvalue;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  uvalue.int32=value;
  buffer[0]=(unsigned char) uvalue.uint32;
  buffer[1]=(unsigned char) (uvalue.uint32 >> 8);
  buffer[2]=(unsigned char) (uvalue.uint32 >> 16);
  buffer[3]=(unsigned char) (uvalue.uint32 >> 24);
  return(WriteBlob(image,4,buffer));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   W r i t e B l o b L S B S h o r t                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobLSBShort writes a 16 bit value in least-significant byte
%  first order.
%
%  The format of the WriteBlobLSBShort method is:
%
%      size_t WriteBlobLSBShort(Image *image,const magick_uint16_t value)
%
%  A description of each parameter follows.
%
%    o count: Method WriteBlobLSBShort returns the number of bytes written
%             (should be 2).
%
%    o image: The image.
%
%    o value:  Specifies the value to write.
%
%
*/
MagickExport size_t WriteBlobLSBShort(Image *image,const magick_uint16_t value)
{
  unsigned char
    buffer[2];

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  buffer[0]=(unsigned char) value;
  buffer[1]=(unsigned char) (value >> 8);
  return(WriteBlob(image,2,buffer));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   W r i t e B l o b L S B S i g n e d S h o r t                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobLSBSignedShort writes a 16 bit signed value in
%  least-significant byte first order.
%
%  The format of the WriteBlobLSBSignedShort method is:
%
%      size_t WriteBlobLSBSignedShort(Image *image,const magick_int16_t value)
%
%  A description of each parameter follows.
%
%    o count: Method WriteBlobLSBSignedShort returns the number of bytes written
%             (should be 2).
%
%    o image: The image.
%
%    o value:  Specifies the value to write.
%
%
*/
MagickExport size_t WriteBlobLSBSignedShort(Image *image,const magick_int16_t value)
{
  unsigned char
    buffer[2];

  MagickInt16Union
    uvalue;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  uvalue.int16=value;
  buffer[0]=(unsigned char) uvalue.uint16;
  buffer[1]=(unsigned char) (uvalue.uint16 >> 8);
  return(WriteBlob(image,2,buffer));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b L S B S h o r t s                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobLSBShorts reads an array of little-endian 16-bit "short"
%  values from the file or BLOB and returns them in native order.
%
%  The format of the ReadBlobLSBShorts method is:
%
%      size_t ReadBlobLSBShorts(Image *image, size_t octets,
%                               magick_uint16_t *data)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobLSBShorts returns the number of octets
%        which were actually read.
%
%    o image: The image.
%
%    o octets: The number of bytes of data to read.
%
%    o data: The address of a user-supplied buffer in which to write
%        the decoded data.  The buffer must be suitably aligned for the
%        data type.
%
*/
MagickExport size_t ReadBlobLSBShorts(Image *image, size_t octets,
                                      magick_uint16_t *data)
{
  size_t
    octets_read;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(data != (magick_uint16_t *) NULL);

  octets_read=ReadBlob(image,octets,data);
#if defined(WORDS_BIGENDIAN)
  if (octets_read >= sizeof(magick_uint16_t))
    MagickSwabArrayOfUInt16(data,(octets_read+sizeof(magick_uint16_t)-1)/sizeof(magick_uint16_t));
#endif

  return octets_read;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b M S B L o n g                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobMSBLong writes a 32 bit value in most-significant byte
%  first order.
%
%  The format of the WriteBlobMSBLong method is:
%
%      size_t WriteBlobMSBLong(Image *image,const magick_uint32_t value)
%
%  A description of each parameter follows.
%
%    o count: Method WriteBlobMSBLong returns the number of bytes written.
%
%    o value:  Specifies the value to write.
%
%    o image: The image.
%
%
*/
MagickExport size_t WriteBlobMSBLong(Image *image,const magick_uint32_t value)
{
  size_t
    count;

  unsigned char
    buffer[4];

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  buffer[0]=(unsigned char) (value >> 24);
  buffer[1]=(unsigned char) (value >> 16);
  buffer[2]=(unsigned char) (value >> 8);
  buffer[3]=(unsigned char) value;

  if (image->blob->type == BlobStream)
    count=WriteBlobStream(image,4,buffer);
  else
    count=WriteBlob(image,4,buffer);
  return count;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b M S B S i g n e d L o n g                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobMSBSignedLong writes a 32 bit signed value in
%  most-significant byte first order.
%
%  The format of the WriteBlobMSBSignedLong method is:
%
%      size_t WriteBlobMSBSignedLong(Image *image,const magick_int32_t value)
%
%  A description of each parameter follows.
%
%    o count: Method WriteBlobMSBLong returns the number of bytes written
%             (should be 4).
%
%    o value:  Specifies the value to write.
%
%    o image: The image.
%
%
*/
MagickExport size_t WriteBlobMSBSignedLong(Image *image,const magick_int32_t value)
{
  size_t
    count;

  unsigned char
    buffer[4];

  MagickInt32Union
    uvalue;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  uvalue.int32=value;
  buffer[0]=(unsigned char) (uvalue.uint32 >> 24);
  buffer[1]=(unsigned char) (uvalue.uint32 >> 16);
  buffer[2]=(unsigned char) (uvalue.uint32 >> 8);
  buffer[3]=(unsigned char) uvalue.uint32;

  if (image->blob->type == BlobStream)
    count=WriteBlobStream(image,4,buffer);
  else
    count=WriteBlob(image,4,buffer);
  return count;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  R e a d B l o b M S B L o n g s                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadBlobMSBLongs reads an array of big-endian 32-bit "long"
%  values from the file or BLOB and returns them in native order.
%
%  The format of the ReadBlobMSBLongs method is:
%
%      size_t ReadBlobMSBLongs(Image *image, size_t octets,
%                              magick_uint32_t *data)
%
%  A description of each parameter follows.
%
%    o value:  Method ReadBlobMSBLongs returns the number of octets
%        which were actually read.
%
%    o image: The image.
%
%    o octets: The number of bytes of data to read.
%
%    o data: The address of a user-supplied buffer in which to write
%        the decoded data.  The buffer must be suitably aligned for the
%        data type.
%
*/
MagickExport size_t ReadBlobMSBLongs(Image *image, size_t octets,
                                     magick_uint32_t *data)
{
  size_t
    octets_read;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(data != (magick_uint32_t *) NULL);

  octets_read=ReadBlob(image,octets,data);
#if !defined(WORDS_BIGENDIAN)
  if (octets_read >= sizeof(magick_uint32_t))
    MagickSwabArrayOfUInt32(data,(octets_read+sizeof(magick_uint32_t)-1)/sizeof(magick_uint32_t));
#endif

  return octets_read;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b M S B S h o r t                                          %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobMSBShort writes a 16 bit value in most-significant byte
%  first order.
%
%  The format of the WriteBlobMSBShort method is:
%
%      size_t WriteBlobMSBShort(Image *image,const magick_uint16_t value)
%
%  A description of each parameter follows.
%
%    o count: Method WriteBlobMSBShort returns the number of bytes written
%             (should be 2).
%
%   o  value:  Specifies the value to write.
%
%   o  file:  Specifies the file to write the data to.
%
%
*/
MagickExport size_t WriteBlobMSBShort(Image *image,const magick_uint16_t value)
{
  unsigned char
    buffer[2];

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  buffer[0]=(unsigned char) (value >> 8);
  buffer[1]=(unsigned char) value;
  return(WriteBlob(image,2,buffer));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b M S B S i g n e d S h o r t                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobMSBSignedShort writes a 16 bit signed value in
%  most-significant byte first order.
%
%  The format of the WriteBlobMSBSignedShort method is:
%
%      size_t WriteBlobMSBSignedShort(Image *image,const magick_int16_t value)
%
%  A description of each parameter follows.
%
%    o count: Method WriteBlobMSBSignedShort returns the number of bytes written
%             (should be 2).
%
%   o  value:  Specifies the value to write.
%
%   o  file:  Specifies the file to write the data to.
%
%
*/
MagickExport size_t WriteBlobMSBSignedShort(Image *image,const magick_int16_t value)
{
  unsigned char
    buffer[2];

  MagickInt16Union
    uvalue;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  uvalue.int16=value;
  buffer[0]=(unsigned char) (uvalue.uint16 >> 8);
  buffer[1]=(unsigned char) uvalue.uint16;
  return(WriteBlob(image,2,buffer));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  W r i t e B l o b S t r i n g                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method WriteBlobString write a string to a blob.  It returns the number of
%  characters written.
%
%  The format of the WriteBlobString method is:
%
%      size_t WriteBlobString(Image *image,const char *string)
%
%  A description of each parameter follows.
%
%    o count:  Method WriteBlobString returns the number of characters written.
%
%    o image: The image.
%
%    o string: Specifies the string to write.
%
%
*/
MagickExport size_t WriteBlobString(Image *image,const char *string)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(string != (const char *) NULL);
  return(WriteBlob(image,strlen(string),string));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  D i s a s s o c i a t e B l o b                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%   Checks if the blob of the specified image is referenced by other images. If
%   the reference count is higher then 1 a new blob is assigned to the image.
%
%  The format of the DisassociateBlob method is:
%
%      void DisassociateBlob(Image *image)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%
*/
MagickExport void DisassociateBlob(Image *image)
{
  BlobInfo
    *blob;

  int
    clone;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->signature == MagickSignature);
  clone=MagickFalse;
  LockSemaphoreInfo(image->blob->semaphore);
  if (image->logging)
    (void) LogMagickEvent(BlobEvent,GetMagickModule(),
                          "Disassociate blob: image=%p, blob=%p, ref=%lu",
                          image,image->blob,image->blob->reference_count);
  assert(image->blob->reference_count >= 0);
  if (image->blob->reference_count > 1)
    clone=MagickTrue;
  UnlockSemaphoreInfo(image->blob->semaphore);
  if (clone == MagickFalse)
    return;
  blob=CloneBlobInfo(image->blob);
  DestroyBlob(image);
  image->blob=blob;
}
