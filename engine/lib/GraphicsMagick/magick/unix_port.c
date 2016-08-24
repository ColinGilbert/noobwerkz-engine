/*
% Copyright (C) 2007-2016 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                       U   U  N   N  IIIII  X   X                            %
%                       U   U  NN  N    I     X X                             %
%                       U   U  N N N    I      X                              %
%                       U   U  N  NN    I     X X                             %
%                        UUU   N   N  IIIII  X   X                            %
%                                                                             %
%                                                                             %
%                       GraphicsMagick UNIX Methods                           %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                             Bob Friesenhahn                                 %
%                                July 2007                                    %
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
#include "magick/confirm_access.h"
#if defined(POSIX)
/* some of these may have already been included by studio.h */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <utime.h>
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t M M U P a g e S i z e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetMMUPageSize() returns the VM pagesize used by the MMU. The VM
%  pagesize is the number of bytes retrieved due to one page fault.
%
%  The format of the MagickGetMMUPageSize method is:
%
%      long MagickGetMMUPageSize()
%
*/
MagickExport long MagickGetMMUPageSize(void)
{
  static long
    pagesize = -1;

  if (pagesize <= 0)
    {
#if defined(HAVE_SYSCONF) && defined(_SC_PAGE_SIZE)
      pagesize=sysconf(_SC_PAGE_SIZE);
#endif /* defined(HAVE_SYSCONF) && defined(_SC_PAGE_SIZE) */
#if defined(HAVE_GETPAGESIZE)
      if (pagesize <= 0)
	pagesize=getpagesize();
#endif /* defined(HAVE_GETPAGESIZE) */
      if (pagesize <= 0)
	pagesize=16384;
    }

  return pagesize;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k G e t F i l e A t t r i b u t e s                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickGetFileAttributes() returns the file attributes for a specified
%  file in a structure of type MagickStatStruct_t.
%
%  The format of the MagickGetFileAttributes method is:
%
%      int MagickGetFileAttributes(const char *filename,
%                                  MagickStatStruct_t *statbuf)
%
%  A description of each parameter follows:
%
%    o filename:  Path to the file
%
%    o statbuf: A structure of type MagickStatStruct_t to populate.
%
%
*/
MagickExport int MagickGetFileAttributes(const char *filename,
                                         MagickStatStruct_t *statbuf)
{
  if (MagickStat(filename, statbuf) != 0)
    return -1;

  return 0;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k S e t F i l e A t t r i b u t e s                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickSetFileAttributes() sets the access and modification time file
%  attributes based on values provided via in a structure of type
%  MagickStatStruct_t.
%
%  The format of the MagickGetFileAttributes method is:
%
%      int MagickSetFileAttributes(const char *filename,
%                                  const MagickStatStruct_t *statbuf)
%
%  A description of each parameter follows:
%
%    o filename:  Path to the file
%
%    o statbuf: A structure of type MagickStatStruct_t to populate.
%
%
*/
MagickExport int MagickSetFileAttributes(const char *filename,
                                         const MagickStatStruct_t *statbuf)
{
  struct utimbuf
    utbuf;

  utbuf.actime = statbuf->st_atime;
  utbuf.modtime = statbuf->st_mtime;

  if (utime(filename, &utbuf) != 0)
    return -1;

  return 0;
}

#endif /* defined(POSIX) */
