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
%                  M   M   AAA    GGGG  IIIII   CCCC  K   K                   %
%                  MM MM  A   A  G        I    C      K  K                    %
%                  M M M  AAAAA  G GGG    I    C      KKK                     %
%                  M   M  A   A  G   G    I    C      K  K                    %
%                  M   M  A   A   GGGG  IIIII   CCCC  K   K                   %
%                                                                             %
%                                                                             %
%            Methods to Read or List GraphicsMagick Image formats             %
%                                                                             %
%                                                                             %
%                            Software Design                                  %
%                            Bob Friesenhahn                                  %
%                              John Cristy                                    %
%                             November 1998                                   %
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
#if defined(MSWINDOWS) || defined(__CYGWIN__)
# include "magick/nt_feature.h"
#endif
#include "magick/blob.h"
#include "magick/color_lookup.h"
#include "magick/command.h"
#include "magick/constitute.h"
#include "magick/delegate.h"
#include "magick/log.h"
#include "magick/magic.h"
#include "magick/magick.h"
#include "magick/module.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/random.h"
#include "magick/registry.h"
#include "magick/resource.h"
#include "magick/render.h"
#include "magick/semaphore.h"
#include "magick/tempfile.h"
#include "magick/utility.h"
#include "magick/version.h"
#if defined(HasX11)
#include "magick/xwindow.h"
#endif
/* watch out here - this include code for simple spinlock semaphore */
#include "magick/spinlock.h"

/*
  Global declarations.
*/
typedef enum  {
  InitDefault,
  InitUninitialized,
  InitInitialized
} MagickInitializationState;

static void
  DestroyMagickInfoList(void);

static SemaphoreInfo
  *magick_semaphore = (SemaphoreInfo *) NULL;

#if defined(SupportMagickModules)
static SemaphoreInfo
  *module_semaphore = (SemaphoreInfo *) NULL;
#endif /* #if defined(SupportMagickModules) */

static MagickInfo
  *magick_list = (MagickInfo *) NULL;

static unsigned int panic_signal_handler_call_count = 0;
static unsigned int quit_signal_handler_call_count = 0;

static MagickInitializationState MagickInitialized = InitDefault;
static CoderClass MinimumCoderClass = UnstableCoderClass;

static void DestroyMagickInfo(MagickInfo** magick_info);
static void DestroyMagickInfoList(void);

static MagickPassFail InitializeMagickInfoList(void);

/*
  Block size to use when accessing filesystem.

  Adjust via MAGICK_IOBUF_SIZE environment variable.
*/

static size_t filesystem_blocksize=16384;

/*
  Get blocksize to use when accessing the filesystem.
*/
size_t
MagickGetFileSystemBlockSize(void)
{
  return filesystem_blocksize;
}

/*
  Set blocksize to use when accessing the filesystem.
*/
void
MagickSetFileSystemBlockSize(const size_t block_size)
{
  assert(block_size > 0);
  filesystem_blocksize=block_size;
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M a g i c k                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyMagick() destroys the GraphicsMagick environment, releasing all
%  allocated semaphores, memory, and temporary files.  This function
%  should be invoked in the primary (original) thread of the application's
%  process while shutting down, and only after any threads which might be
%  using GraphicsMagick functions have terminated.  Since GraphicsMagick
%  uses threads internally via OpenMP, it is also necessary for any function
%  calls into GraphicsMagick to have already returned so that OpenMP worker
%  threads are quiesced and won't be accessing any semaphores or data
%  structures which are destroyed by this function.
%
%  The format of the DestroyMagick function is:
%
%      DestroyMagick(void)
%
%
*/
MagickExport void
DestroyMagick(void)
{
  /* Acquire destruction lock */
  SPINLOCK_WAIT;

  if (MagickInitialized == InitUninitialized)
    {
      /* Release destruction lock */
      SPINLOCK_RELEASE;
      return;
    }

  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
			"Destroy Magick");

  MagickDestroyCommandInfo();   /* Command parser */
#if defined(HasX11)
  MagickXDestroyX11Resources();
#endif
  DestroyColorInfo();           /* Color database */
  DestroyDelegateInfo();        /* External delegate information */
  DestroyTypeInfo();            /* Font information */
  DestroyMagicInfo();           /* File format detection */
  DestroyMagickInfoList();      /* Coder registrations + modules */
  DestroyConstitute();          /* Constitute semaphore */
  DestroyMagickRegistry();      /* Registered images */
  DestroyMagickResources();     /* Resource semaphore */
  DestroyMagickRandomGenerator(); /* Random number generator */
  DestroyTemporaryFiles();      /* Temporary files */
#if defined(MSWINDOWS)
  NTGhostscriptUnLoadDLL();     /* Ghostscript DLL */
#endif /* defined(MSWINDOWS) */
  /*
    Destroy logging last since some components log their destruction.
  */
  DestroyLogInfo();             /* Logging configuration */
  DestroySemaphore();           /* Semaphores framework */

  /* Now uninitialized */
  MagickInitialized=InitUninitialized;

  /* Release destruction lock */
  SPINLOCK_RELEASE;
}
/*
  Destroy MagickInfo structure.
*/

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M a g i c k I n f o                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyMagickInfo() destroys a MagickInfo structure.
%
%  The format of the DestroyMagickInfoList method is:
%
%      void DestroyMagickInfo(MagickInfo** magick_info)
%
*/
static void
DestroyMagickInfo(MagickInfo** magick_info)
{
  MagickInfo
    *p;

  p=*magick_info;
  if (p)
    {
      p->name=0;
      p->description=0;
      p->version=0;
      p->note=0;
      p->module=0;
      MagickFreeMemory(p);
      *magick_info=p;
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D e s t r o y M a g i c k I n f o L i s t                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DestroyMagickInfo() deallocates memory associated with the MagickInfo list.
%
%  The format of the DestroyMagickInfoList method is:
%
%      void DestroyMagickInfoList(void)
%
*/
static void
DestroyMagickInfoList(void)
{
  MagickInfo
    *magick_info;

  register MagickInfo
    *p;

#if defined(SupportMagickModules)
  DestroyMagickModules();
#endif /* defined(SupportMagickModules) */

#if !defined(BuildMagickModules)
  UnregisterStaticModules();
#endif /* !defined(BuildMagickModules) */

  /*
    At this point, the list should be empty, but check for remaining
    entries anyway.
  */
#if defined(BuildMagickModules)
  if (magick_list != (MagickInfo *) NULL)
    (void) printf("Warning: module registrations are still present!\n");
#endif /* defined(BuildMagickModules) */
  for (p=magick_list; p != (MagickInfo *) NULL; )
  {
    magick_info=p;
    p=p->next;

#if !defined(BuildMagickModules)
    (void) printf("Warning: module registration for \"%s\" from module \"%s\" still present!\n",
                  magick_info->name, magick_info->module);
#endif /* !defined(BuildMagickModules) */
    DestroyMagickInfo(&magick_info);
  }
  magick_list=(MagickInfo *) NULL;
  DestroySemaphoreInfo(&magick_semaphore);

#if defined(SupportMagickModules)
  DestroySemaphoreInfo(&module_semaphore);
#endif /* #if defined(SupportMagickModules) */
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t I m a g e M a g i c k                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetImageMagick() searches for an image format that matches the specified
%  magick string.  If one is found the name is returned otherwise NULL.
%
%  The format of the GetImageMagick method is:
%
%      const char *GetImageMagick(const unsigned char *magick,
%        const size_t length)
%
%  A description of each parameter follows:
%
%    o magick: The image format we are searching for.
%
%    o length: The length of the binary string.
%
%
*/
MagickExport const char *
GetImageMagick(const unsigned char *magick,const size_t length)
{
  register MagickInfo
    *p;

  assert(magick != (const unsigned char *) NULL);
  LockSemaphoreInfo(magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
    if (p->magick && p->magick(magick,length))
      break;
  UnlockSemaphoreInfo(magick_semaphore);
  if (p != (MagickInfo *) NULL)
    return(p->name);
  return((char *) NULL);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickInfo() returns a pointer MagickInfo structure that matches
%  the specified name.  If name is NULL, the head of the image format list
%  is returned. It is not safe to traverse the list by using the previous and
%  next pointers in the MagickInfo structure since the list contents or order
%  may be altered while the list is being traversed. If the list must be
%  traversed, access it via the GetMagickInfoArray function instead.
%
%  If GraphicsMagick has not been initialized via InitializeMagick()
%  then this function will not work.
%
%  The format of the GetMagickInfo method is:
%
%     const MagickInfo *GetMagickInfo(const char *name,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o name: The image format we are looking for.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static MagickInfo *
GetMagickInfoEntryLocked(const char *name)
{
  register MagickInfo
    *p;

  LockSemaphoreInfo(magick_semaphore);

  p=magick_list;

  if ((name != (const char *) NULL) && (name[0] != '*'))
    {
      for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
        if (LocaleCompare(p->name,name) == 0)
          break;
      
      if (p != (MagickInfo *) NULL)
        if (p != magick_list)
          {
            /*
              Self-adjusting list.
            */
            if (p->previous != (MagickInfo *) NULL)
              p->previous->next=p->next;
            if (p->next != (MagickInfo *) NULL)
              p->next->previous=p->previous;
            p->previous=(MagickInfo *) NULL;
            p->next=magick_list;
            magick_list->previous=p;
            magick_list=p;
          }
    }

  UnlockSemaphoreInfo(magick_semaphore);

  return p;
}
MagickExport const MagickInfo *
GetMagickInfo(const char *name,ExceptionInfo *exception)
{
  const MagickInfo
    *magick_info=(const MagickInfo *) NULL;

#if defined(SupportMagickModules)
  if ((name != (const char *) NULL) &&
      (name[0] != '\0'))
    {
      LockSemaphoreInfo(module_semaphore);
      if (name[0] == '*')
	{
	  /*
	    If all modules are requested, then use OpenModules to load all
	    modules.
	  */
	  (void) OpenModules(exception);
	}
      else
	{
	  magick_info=GetMagickInfoEntryLocked(name);
	  if (magick_info == (const MagickInfo *) NULL)
	    {
	      /*
		Try to load a supporting module.
	      */
	      (void) OpenModule(name,exception);
	    }
	}
      UnlockSemaphoreInfo(module_semaphore);
    }
#else
  ARG_NOT_USED(exception);
#endif /* #if defined(SupportMagickModules) */

  /*
    Return whatever we've got
  */
  if (magick_info == (const MagickInfo *) NULL)
    magick_info=GetMagickInfoEntryLocked(name);

  return magick_info;
}
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k I n f o A r r a y                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickInfoArray() returns a sorted null-terminated array of MagickInfo
%  pointers corresponding to the available format registrations. If necessarly
%  all modules are loaded in order to return a complete list. This function
%  should be used to access the entire list rather than GetMagickInfo since
%  the list returned by GetMagickInfo may be re-ordered every time it is
%  invoked. Once the returned array is no longer needed, the allocated array
%  should be deallocated. Do not attempt to deallocate the MagickInfo
%  structures based on pointers in the array!
%
%  The format of the GetMagickList method is:
%
%      MagickInfo **GetMagickInfoArray(ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
/*
  Compare two MagickInfo structures based on their name
*/
static int
MagickInfoCompare(const void *x, const void *y)
{
  const MagickInfo
    *xx=*((const MagickInfo **) x),
    *yy=*((const MagickInfo **) y);

  return (strcmp(xx->name, yy->name));
}
MagickExport MagickInfo **
GetMagickInfoArray(ExceptionInfo *exception)
{
  MagickInfo
    **array;

  MagickInfo
    *p;

  MagickInfo
    *list;

  size_t
    entries=0;

  int
    i;

  /*
    Load all modules and obtain pointer to head of list
  */
  (void) GetMagickInfo("*",exception);
  if (!magick_list)
    return ((MagickInfo **) NULL);

  LockSemaphoreInfo(magick_semaphore);

  list=magick_list;

  /*
    Count number of list entries
  */
  for (p=list; p != 0; p=p->next)
    entries++;

  /*
    Allocate array memory
  */
  array=MagickAllocateArray(MagickInfo **,sizeof(MagickInfo *),(entries+1));
  if (!array)
    {
      UnlockSemaphoreInfo(magick_semaphore);
      ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,0);
      return ((MagickInfo **) NULL);
    }

  /*
    Add entries to array
  */
  i=0;
  for (p=list; p != 0; p=p->next)
    array[i++]=p;
  array[i]=(MagickInfo *) NULL;

  UnlockSemaphoreInfo(magick_semaphore);

  /*
    Sort array entries
  */
  qsort((void *) array, entries, sizeof(MagickInfo *), MagickInfoCompare);

  return (array);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n i t i a l i z e M a g i c k                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InitializeMagick() initializes the GraphicsMagick environment.
%  InitializeMagick() MUST be invoked by the using program before making
%  use of GraphicsMagick functions or else the library will be unusable.
%
%  This function should be invoked in the primary (original) thread of the
%  application's process, and before starting any OpenMP threads, as part
%  of program initialization.
%
%  The format of the InitializeMagick function is:
%
%      InitializeMagick(const char *path)
%
%  A description of each parameter follows:
%
%    o path: The execution path of the current GraphicsMagick client.
%
%
*/

/*
  Due to legacy compilers, POSIX.1 actually allows the signal function
  to return 'int' or 'void'. The compiler is not so lenient so we must
  ensure that the return type matches the prototype in the headers.
*/
#if !defined(RETSIGTYPE)
# define RETSIGTYPE void
#endif
typedef RETSIGTYPE Sigfunc(int);

#if !defined(SIG_ERR)
# define SIG_ERR (Sigfunc *)-1
#endif
#if !defined(SIG_DFL)
# define SIG_DFL (Sigfunc *)0
#endif

static RETSIGTYPE MagickPanicSignalHandler(int signo);
static RETSIGTYPE MagickSignalHandler(int signo);

/*
  Signal function which prevents interrupted system calls from
  automatically being restarted. From W. Richard Stevens "Advanced
  Programming in the UNIX Environment", Chapter 10.14.
*/
static Sigfunc *
MagickSignal(int signo, Sigfunc *func)
{
#if defined(HAVE_SIGACTION) && defined(HAVE_SIGEMPTYSET)
  struct sigaction
    act,
    oact;

  act.sa_handler=func;
  (void) sigemptyset(&act.sa_mask);
  act.sa_flags=0;
#  if defined(SA_INTERRUPT)  /* SunOS */
  act.sa_flags |= SA_INTERRUPT;
#  endif
  if (sigaction(signo, &act, &oact) < 0)
    return (SIG_ERR);
  return (oact.sa_handler);
#else
  return signal(signo, func);
#endif
}

/*
  Signal function which only sets the signal handler if it its handling
  is still set to SIG_DFL. This ensures that signal handlers are not
  registered for signals which are being ignored, or that the API
  user has already registered before invoking InitializeMagick.

  If an API user registers its own signal hander, then it is responsible
  for invoking DestroyMagick when a signal is received.
*/
static Sigfunc *
MagickCondSignal(int signo, Sigfunc *func)
{
  Sigfunc *
    o_handler;

  if ((o_handler=MagickSignal(signo,func)) != SIG_ERR)
    {
      /*
        If handler is not the default, then restore the previous
        setting.
      */
      if (o_handler != SIG_DFL)
        (void) MagickSignal(signo,o_handler);
      else
        (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
          "Registered signal handler for signal ID %d",signo);
    }
  return (o_handler);
}

/*
  Signal handler which does nothing.
*/
#if 0
static RETSIGTYPE
MagickIgnoreSignalHandler(int signo)
{
  fprintf(stderr,"Caught ignored signal %d\n", signo);
}
#endif

/*
  Write a message about the signal to stderr.
*/
static void
MagickSignalHandlerMessage(const int signo, const char *subtext)
{
  /*
    Signals we might handle, to map to their common description.

    May use strsignal() on POSIX.1-2008-compliant systems, but it is
    not documented to be async-safe.

  */
  static const struct {
    const int signo;
    const char *name;
    const char *descr;
  } signal_descr[] = {
#if defined(SIGHUP)
    { SIGHUP, "SIGHUP","Hangup" },
#endif
#if defined(SIGINT)
    { SIGINT, "SIGINT", "Interrupt" },
#endif
#if defined(SIGQUIT)
    { SIGQUIT, "SIGQUIT", "Quit" },
#endif
#if defined(SIGILL)
    { SIGILL, "SIGILL", "Illegal Instruction" },
#endif
#if defined(SIGABRT)
    { SIGABRT, "SIGABRT", "Abort" },
#endif
#if defined(SIGFPE)
    { SIGFPE, "SIGFPE", "Arithmetic Exception" },
#endif
#if defined(SIGBUS)
    { SIGBUS, "SIGBUS", "Bus Error" },
#endif
#if defined(SIGSEGV)
    { SIGSEGV, "SIGSEGV", "Segmentation Fault" },
#endif
#if defined(SIGPIPE)
    { SIGPIPE, "SIGPIPE", "Broken Pipe" },
#endif
#if defined(SIGALRM)
    { SIGALRM, "SIGALRM", "Alarm Clock" },
#endif
#if defined(SIGTERM)
    { SIGTERM, "SIGTERM", "Terminated" },
#endif
#if defined(SIGCHLD)
    { SIGCHLD, "SIGCHLD", "Child Status Changed" },
#endif
#if defined(SIGXCPU)
    { SIGXCPU, "SIGXCPU", "CPU time limit exceeded" },
#endif
#if defined(SIGXFSZ)
    { SIGXFSZ, "SIGXFSZ", "File size limit exceeded" }
#endif
  };

  static char
    message[128];

  size_t
    i;

  int
    num=signo;

  /*
    Output messages like:

    gm convert: quitting due to signal 2 (SIGINT) "Interrupt"...
    gm convert: abort due to signal 11 (SIGSEGV) "Segmentation Fault"...
  */
  (void) strlcpy(message, GetClientName(), sizeof(message));
  (void) strlcat(message,": ", sizeof(message));
  (void) strlcat(message,subtext,sizeof(message));
  (void) strlcat(message," due to signal ",sizeof(message));
  i=strlen(message);
  for ( ; (num != 0) && (i < sizeof(message)-1) ; i++)
    {
      int rem = num % 10;
      message[i] = rem + '0';
      num = num/10;
    }
  message[i] = '\0';
  for ( i=0; i < (sizeof(signal_descr)/sizeof(signal_descr[0])); i++)
    {
      if (signo == signal_descr[i].signo)
        {
          (void) strlcat(message," (",sizeof(message));
          (void) strlcat(message,signal_descr[i].name,sizeof(message));
          (void) strlcat(message,") \"",sizeof(message));
          (void) strlcat(message,signal_descr[i].descr,sizeof(message));
          (void) strlcat(message,"\"",sizeof(message));
        }
    }
  (void) strlcat(message,"...\n",sizeof(message));

  if (write(STDERR_FILENO,message,
            (MAGICK_POSIX_IO_SIZE_T) strlen(message)) == -1)
    {
      /* Exists to quench warning */
    }
}


/*
  Signal handler to ensure that DestroyMagick is invoked in case the
  user aborts the program.

  The WIN32 documentation says that SIGINT is not supported under any
  version of Windows. It also says that a new thread is created to
  handle the interrupt caused by CNTRL+C. The Windows signal
  documentation also says that it is unsafe to do anything useful from
  within a signal handler.  This means that malloc/free and any system
  call should not be executed. Theoretically DestroyMagick() should
  not be executed since its purpose is to invoke unsafe functions.
  SIGILL, SIGSEGV, and SIGTERM are documented to never be generated by
  Windows NT.

  While support for signal is definitely broken under Windows, the good
  news is that it seems to be unlikely to generate a signal we care about.
*/

static RETSIGTYPE
MagickPanicSignalHandler(int signo)
{
  ARG_NOT_USED(signo);

  panic_signal_handler_call_count++;

  /*
    Only handle signal one time.
  */
  if (1 == panic_signal_handler_call_count)
    {
      /*
        Release persistent resources
      */
      PanicDestroyMagick();

      /*
        Produce a useful message for the user
      */
      MagickSignalHandlerMessage(signo,"abort");

      /*
	Call abort so that we quit with core dump.
      */
      abort();
    }
}

static MagickBool QuitProgressMonitor(const char *task,
				      const magick_int64_t quantum,
				      const magick_uint64_t span,
				      ExceptionInfo *exception)
{
  ARG_NOT_USED(task);
  ARG_NOT_USED(quantum);
  ARG_NOT_USED(span);

  /* Report an error message */
  if (exception->severity < FatalErrorException)
    ThrowException(exception,MonitorFatalError,UserRequestedTerminationBySignal,0);

  return MagickFail;
}

static RETSIGTYPE
MagickSignalHandler(int signo)
{
  /* fprintf(stderr,"Caught signal %d\n", signo); */

  quit_signal_handler_call_count++;

  /*
    Only handle signal one time.
  */
  if (1 == quit_signal_handler_call_count)
    {
      if (MagickInitialized == InitInitialized)
	{

	  /*
	    Set progress monitor handler to one which always returns
	    MagickFail.
	  */
	  (void) SetMonitorHandler(QuitProgressMonitor);

          /*
            Release persistent resources
          */
          PanicDestroyMagick();

          /*
            Produce a useful message for the user
          */
          if (signo != SIGINT)
            MagickSignalHandlerMessage(signo,"quitting");
	}

      /*
	Invoke _exit(signo) (or equivalent) which avoids invoking
	registered atexit() functions.
      */
      SignalHandlerExit(signo);
    }
}

/*
   The goal of this routine is to determine whether the passed
   string is a valid and complete path to a file within the 
   filesystem
 */
#if !defined(UseInstalledMagick)
static unsigned int
IsValidFilesystemPath(const char *path)
{
  if ((path != (const char *) NULL) && (*path != '\0'))
    {
#if defined(POSIX)
      /* For POSIX we check the first character to see of it is a file
          system path seperator. If not then we ignore the passed data
      */
      if ((*path == *DirectorySeparator))
        return IsAccessibleNoLogging(path);
#elif defined(MSWINDOWS)
      /* For Windows we check to see if the path passed seems to be a 
         pathof any kind (contains delimiters) or seem to be either UNC
         path or one with a drive letter spec in it: \\Server\share, C:\
      */
      if (((*path == *DirectorySeparator) && (*(path+1) == *DirectorySeparator)) ||
            (*(path+1) == ':') ||
              (strchr(path,*DirectorySeparator) != (char *) NULL))
        return IsAccessibleNoLogging(path);
#else
      /* In any other case, we just look to see if it has path delimiters */
      if ((strchr(path,*DirectorySeparator) != (char *) NULL))
        return IsAccessibleNoLogging(path);
#endif
    }
  return False;
}
#endif /* !defined(UseInstalledMagick) */

/*
  Try and figure out the path and name of the client
 */
MagickExport void
InitializeMagickClientPathAndName(const char *path)
{
#if !defined(UseInstalledMagick)
  const char
    *spath;

  char
    execution_path[MaxTextExtent];

  /* the following is to make the logging more readable later on */
  spath = path;
  if (spath == (char *) NULL)
    spath = "NULL";
  if (*spath == '\0')
    spath = "EMPTY";

  *execution_path='\0';
  if (!IsValidFilesystemPath(path))
    {
      /*
        If the path passed is NULL or invalid then we try to ask the
        OS what the name of the executable is. Callers will often pass
        NULL in order to invoke this functionality.
      */
      if (GetExecutionPath(execution_path))
        {
          /*
            The call to the OS worked so we can do all the settings
          */
          (void) DefineClientPathAndName(execution_path);
          (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
            "Invalid path \"%s\" passed - asking OS worked: \"%s\"",spath,
              execution_path);
        }
      else
        {
          /*
            The call to the OS failed so we try to compute a usable
            path based on the current working directory or by
            searching the executable search path.
          */
          if (path)
            (void) strlcpy(execution_path,path,MaxTextExtent);
          if (GetExecutionPathUsingName(execution_path))
            {
              (void) DefineClientPathAndName(execution_path);
              (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
                "Invalid path \"%s\" passed - asking OS failed, getcwd worked: \"%s\"",spath,
                  execution_path);
            }
          else
            {
              /*
                At this point we are totally hosed and have nothing to
                go on for the path, so all we can do is log the error
              */
              (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
                "Invalid path \"%s\" passed - asking OS failed, getcwd also failed",spath);
            }
        }
    }
  else
    {
      /*
        This is the easy one. The caller gave us the correct and
        working path to the application, so we just use it
      */
      (void) strlcpy(execution_path,path,MaxTextExtent);
      (void) DefineClientPathAndName(execution_path);
      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),"Valid path \"%s\"",spath);
    }
#else
  ARG_NOT_USED(path);
#endif
}

/*
  Establish signal handlers for common signals

*/
MagickExport void
InitializeMagickSignalHandlers(void)
{
#if 0
  /* termination of child process */
#if defined(SIGCHLD)
  (void) MagickCondSignal(SIGCHLD,MagickIgnoreSignalHandler);
#endif
#endif

  /* hangup, default terminate */
#if defined(SIGHUP)
  (void) MagickCondSignal(SIGHUP,MagickSignalHandler);
#endif
  /* interrupt (CONTROL-c), default terminate */
#if defined(SIGINT)
  (void) MagickCondSignal(SIGINT,MagickSignalHandler);
#endif
  /* quit (CONTROL-\), default terminate with core */
#if defined(SIGQUIT)
  (void) MagickCondSignal(SIGQUIT,MagickPanicSignalHandler);
#endif
  /* software-triggered abort, default terminate with core */
#if defined(SIGABRT)
  (void) MagickCondSignal(SIGABRT,MagickPanicSignalHandler);
#endif
  /* floating point exception, default terminate with core */
#if defined(SIGFPE)
  (void) MagickCondSignal(SIGFPE,MagickPanicSignalHandler);
#endif
  /* software termination signal from kill, default terminate */
#if defined(SIGTERM)
  (void) MagickCondSignal(SIGTERM,MagickSignalHandler);
#endif
  /* Bus Error */
#if defined(SIGBUS)
  (void) MagickCondSignal(SIGBUS,MagickPanicSignalHandler);
#endif
  /* segmentation fault */
#if defined(SIGSEGV)
  (void) MagickCondSignal(SIGSEGV,MagickPanicSignalHandler);
#endif
  /* exceeded cpu limit, default terminate with core */
#if defined(SIGXCPU)
  (void) MagickCondSignal(SIGXCPU,MagickPanicSignalHandler);
#endif
  /* exceeded file size limit, default terminate with core */
#if defined(SIGXFSZ)
  (void) MagickCondSignal(SIGXFSZ,MagickPanicSignalHandler);
#endif
}

MagickExport void
InitializeMagick(const char *path)
{
  /*
    NOTE: This routine sets up the path to the client which needs to
    be determined before almost anything else works right. This also
    includes logging!!! So we can't start logging until the path is
    actually saved. As soon as we know what the path is we make the
    same call to DefineClientSettings to set it up. Please make sure
    that this rule is followed in any future updates the this code!!!
  */

  const char
    *p;

  /* Acquire initialization lock */
  SPINLOCK_WAIT;

  if (MagickInitialized == InitInitialized)
    {
      /* Release initialization lock */
      SPINLOCK_RELEASE;
      return;
    }
  
#if defined(MSWINDOWS)
# if defined(_DEBUG) && !defined(__BORLANDC__)
  {
    int
      debug;
    
    debug=_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    debug|=_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_DELAY_FREE_MEM_DF |
      _CRTDBG_LEAK_CHECK_DF;
    /* debug=_CrtSetDbgFlag(debug); */
    /* _ASSERTE(_CrtCheckMemory()); */
  }
# endif /* defined(_DEBUG) */
#endif /* defined(MSWINDOWS) */

  /* Initialize semaphores */
  InitializeSemaphore();

  /* Initialize logging */
  InitializeLogInfo();

  /* Initialize our random number generator */
  InitializeMagickRandomGenerator();

  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
			"Initialize Magick");

  /*
    Set the filesystem block size.
  */
  {
    size_t
      block_size=16384;

    if ((p=getenv("MAGICK_IOBUF_SIZE")) != (const char *) NULL)
      {
        long
          block_size_long;
    
        block_size_long = MagickAtoL(p);
        if ((block_size_long > 0L) && (block_size_long <= 2097152L))
          block_size=(size_t) block_size_long;
        else
          (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
                                "Ignoring unreasonable MAGICK_IOBUF_SIZE of "
                                "%ld bbytes", block_size_long);
      }
    
    MagickSetFileSystemBlockSize(block_size);
  }

  /*
    Establish the path, filename, and display name of the client app
  */
  InitializeMagickClientPathAndName(path);
  /*
    If the client name did not get setup for any reason, we take one
    last shot at it using the data the caller passed us.
  */
  if (GetClientName() == (const char *) NULL)
    DefineClientName(path);

  /*
    Initialize any logging configuration which could not complete
    since we did not know the installation directory yet
  */
  InitializeLogInfoPost();

  /*
    Adjust minimum coder class if requested.
  */
  if ((p=getenv("MAGICK_CODER_STABILITY")) != (const char *) NULL)
    {
      if (LocaleCompare(p,"BROKEN") == 0)
	MinimumCoderClass=BrokenCoderClass;
      else if (LocaleCompare(p,"UNSTABLE") == 0)
	MinimumCoderClass=UnstableCoderClass;
      else if (LocaleCompare(p,"STABLE") == 0)
	MinimumCoderClass=StableCoderClass;
      else if (LocaleCompare(p,"PRIMARY") == 0)
	MinimumCoderClass=PrimaryCoderClass;
    }

#if defined(MSWINDOWS)
  NTInitializeExceptionHandlers();  /* WIN32 Exceptions */
#endif /* defined(MSWINDOWS) */
  InitializeMagickSignalHandlers(); /* Signal handlers */
  InitializeTemporaryFiles();       /* Temporary files */
  InitializeMagickResources();      /* Resources */
  InitializeMagickRegistry();       /* Image/blob registry */
  InitializeConstitute();           /* Constitute semaphore */
  InitializeMagickInfoList();       /* Coder registrations + modules */
  InitializeMagicInfo();            /* File format detection */
  InitializeTypeInfo();             /* Font information */
  InitializeDelegateInfo();         /* External delegate information */
  InitializeColorInfo();            /* Color database */
  MagickInitializeCommandInfo();    /* Command parser */

  /* Let's log the three important setting as we exit this routine */
  (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
			"Path: \"%s\" Name: \"%s\" Filename: \"%s\"",
			GetClientPath(),GetClientName(),GetClientFilename());

  /* Now initialized */
  MagickInitialized=InitInitialized;

  /* Release initialization lock */
  SPINLOCK_RELEASE;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n i t i a l i z e M a g i c k I n f o L i s t                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InitializeMagickInfoList initializes the magick info facility
%
%  The format of the InitializeMagickInfoList method is:
%
%      MagickPassFail InitializeMagickInfoList(void)
%
%
*/
MagickPassFail
InitializeMagickInfoList(void)
{
  assert(magick_semaphore == (SemaphoreInfo *) NULL);
  magick_semaphore=AllocateSemaphoreInfo();

#if defined(SupportMagickModules)
  assert(module_semaphore == (SemaphoreInfo *) NULL);
  module_semaphore=AllocateSemaphoreInfo();
#endif /* #if defined(SupportMagickModules) */

#if !defined(BuildMagickModules)
  RegisterStaticModules();          /* Register all static modules */
#endif /* !defined(BuildMagickModules) */

#if defined(SupportMagickModules)
  InitializeMagickModules();        /* Module loader */
#endif /* defined(SupportMagickModules) */

  return MagickPass;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s M a g i c k C o n f l i c t                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsMagickConflict returns true if the image format conflicts with
%  a logical drive (.e.g. X:).
%
%  The format of the IsMagickConflict method is:
%
%      MagickBool IsMagickConflict(const char *magick)
%
%  A description of each parameter follows:
%
%    o status: Method IsMagickConflict returns true if the image format
%      conflicts with a logical drive.
%
%    o magick: Specifies the image format.
%
%
*/
MagickExport MagickBool
IsMagickConflict(const char *magick)
{
  assert(magick != (char *) NULL);
#if defined(MSWINDOWS) || defined(__CYGWIN__)
  return(NTIsMagickConflict(magick));
#endif
  return(MagickFalse);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M a g i c k I n f o                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ListMagickInfo() lists the image formats to a file.
%
%  The format of the ListMagickInfo method is:
%
%      MagickPassFail ListMagickInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file: A file handle.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickPassFail
ListMagickInfo(FILE *file,ExceptionInfo *exception)
{

  MagickInfo
    **magick_array;

  int
    i;

  if (file == (FILE *) NULL)
    file=stdout;

  magick_array=GetMagickInfoArray(exception);
  if (!magick_array)
    return False;

  (void) fprintf(file,"   Format L  Mode  Description\n");
  (void) fprintf(file,"--------------------------------------------------------"
    "------------------------\n");
  for (i=0; magick_array[i] != 0; i++)
  {
    if (magick_array[i]->stealth)
      continue;
    (void) fprintf(file,"%9s %c  %c%c%c",
                   magick_array[i]->name ? magick_array[i]->name : "",
                   (magick_array[i]->coder_class == PrimaryCoderClass ? 'P' :
                    (magick_array[i]->coder_class == StableCoderClass ? 'S' :
                     'U')),
                   (magick_array[i]->decoder ? 'r' : '-'),
                   (magick_array[i]->encoder ? 'w' : '-'),
                   ((magick_array[i]->encoder && magick_array[i]->adjoin) ? '+' : '-'));
    if (magick_array[i]->description != (char *) NULL)
      (void) fprintf(file,"  %.1024s",magick_array[i]->description);
    if (magick_array[i]->version != (char *) NULL)
      (void) fprintf(file," (%.1024s)",magick_array[i]->version);
    (void) fprintf(file,"\n");
    if (magick_array[i]->note != (char *) NULL)
      {
        char
          **text;

        text=StringToList(magick_array[i]->note);
        if (text != (char **) NULL)
          {
            register long
              j;

            for (j=0; text[j] != (char *) NULL; j++)
              {
                (void) fprintf(file,"            %.1024s\n",text[j]);
                MagickFreeMemory(text[j]);
              }
            MagickFreeMemory(text);
          }
      }
  }
  (void) fprintf(file,"\n Meaning of 'L': P=Primary, S=Stable, U=Unstable\n");
  (void) fflush(file);
  MagickFreeMemory(magick_array);
  return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M o d u l e M a p                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListModuleMap lists the module alias info to a file in the XML
%  format used by modules.mgk. True is returned on success.
%
%  The format of the ListModuleMap method is:
%
%      MagickPassFail ListModuleMap(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickPassFail
ListModuleMap(FILE *file,ExceptionInfo *exception)
{
  MagickInfo
    **magick_array;

  int
    i;

  if (file == (const FILE *) NULL)
    file=stdout;

   magick_array=GetMagickInfoArray(exception);
   if (!magick_array)
     return MagickFail;

   (void) fprintf(file, "<?xml version=\"1.0\"?>\n");
   (void) fprintf(file, "<!-- %s -->\n",GetMagickCopyright());
   (void) fprintf(file, "<!-- Magick Module Alias Map (modules.mgk) -->\n");
   (void) fprintf(file, "<modulemap>\n");

   for (i=0; magick_array[i] != 0; i++)
     {
       if (LocaleCompare(magick_array[i]->name,magick_array[i]->module) != 0)
         {
           (void) fprintf(file, "  <module magick=\"%s\" name=\"%s\" />\n",
                          magick_array[i]->name,
			  (magick_array[i]->module == NULL ? "(null)" :
			   magick_array[i]->module));
         }
     }
   (void) fprintf(file, "</modulemap>\n");
   (void) fflush(file);

   MagickFreeMemory(magick_array);

   return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  M a g i c k T o M i m e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickToMime returns the officially registered (or de facto) MIME
%  media-type corresponding to a magick string.  If there is no registered
%  media-type, then the string "image/x-magick" (all lower case) is returned.
%  The returned string must be deallocated by the user.
%
%  The format of the MagickToMime method is:
%
%      char *MagickToMime(const char *magick)
%
%  A description of each parameter follows.
%
%   o  magick:  GraphicsMagick format specification "magick" tag.
%
%
*/
MagickExport char *
MagickToMime(const char *magick)
{
  typedef struct _MediaType
  {
    const char
      *magick,
      *media;
  } MediaType;

  char
    media[MaxTextExtent];

  MediaType
    *entry;

  static MediaType
    MediaTypes[] =
    {
      { "avi",   "video/avi" },
      { "cgm",   "image/cgm;Version=4;ProfileId=WebCGM" }, /* W3 WebCGM */
      { "dcm",   "application/dicom" }, /* Incomplete.  See RFC 3240 */
      { "epdf",  "application/pdf" },
      { "epi",   "application/postscript" },
      { "eps",   "application/postscript" },
      { "eps2",  "application/postscript" },
      { "eps3",  "application/postscript" },
      { "epsf",  "application/postscript" },
      { "ept",   "application/postscript" },
      { "fax",   "image/g3fax" },
      { "fpx",   "image/vnd.fpx" },
      { "g3",    "image/g3fax" },
      { "gif",   "image/gif" },
      { "gif87", "image/gif" },
      { "jpeg",  "image/jpeg" },
      { "mng",   "video/x-mng" },
      { "mpeg",  "video/mpeg" },
      { "png",   "image/png" },
      { "pdf",   "application/pdf" },
      { "ps",    "application/postscript" },
      { "ps2",   "application/postscript" },
      { "ps3",   "application/postscript" },
      { "svg",   "image/svg+xml" },
      { "tif",   "image/tiff" },
      { "tiff",  "image/tiff" },
      { "wbmp",  "image/vnd.wap.wbmp" },
      { (char *) NULL, (char *) NULL }
    };

  for (entry=MediaTypes; entry->magick != (char *) NULL; entry++)
    if (LocaleCompare(entry->magick,magick) == 0)
      return(AllocateString(entry->media));
  FormatString(media,"image/x-%.1024s",magick);
  LocaleLower(media+8);
  return(AllocateString(media));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r M a g i c k I n f o                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RegisterMagickInfo() adds attributes for a particular image format to the
%  list of supported formats.  The attributes include the image format name,
%  a method to read and/or write the format, whether the format supports the
%  saving of more than one frame to the same file or blob, whether the format
%  supports native in-memory I/O, and a brief description of the format.
%
%  The format of the RegisterMagickInfo method is:
%
%      MagickInfo *RegisterMagickInfo(MagickInfo *magick_info)
%
%  A description of each parameter follows:
%
%    o magick_info: The magick info.
%
*/
MagickExport MagickInfo *
RegisterMagickInfo(MagickInfo *magick_info)
{
  assert(magick_info != (MagickInfo *) NULL);
  assert(magick_info->signature == MagickSignature);

  /*
    Remove any existing entry.
  */
  (void) UnregisterMagickInfo(magick_info->name);

  /*
    Verify that coder stability level is sufficient.
  */
  if (magick_info->coder_class >= MinimumCoderClass)
    {
      /*
        Add to front of list.
      */
      LockSemaphoreInfo(magick_semaphore);
      magick_info->previous=(MagickInfo *) NULL;
      magick_info->next=magick_list;
      if (magick_info->next != (MagickInfo *) NULL)
        magick_info->next->previous=magick_info;
      magick_list=magick_info;
      UnlockSemaphoreInfo(magick_semaphore);
      return(magick_info);
    }

  /*
    Discard registration and return NULL.
  */
  DestroyMagickInfo(&magick_info);
  return(magick_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P a n i c D e s t r o y M a g i c k                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PanicDestroyMagick() destroys only persistent allocations such as
%  temporary files.  Other allocations (e.g. semaphores and heap memory)
%  remain allocated.  This function is an alternative to DestroyMagick()
%  which is async-safe so it may be invoked from signal handers, and
%  may be invoked from thread context.  No semaphores are taken and no
%  additional heap memory is allocated by this function.  The program
%  must quit immediately after invoking this function.
%
%  The format of the PanicDestroyMagick function is:
%
%      void PanicDestroyMagick(void)
%
%
*/
/*
  Set to 1 to break the memory allocation functions.  This is a
  debugging aid.  In a multi-threaded program, other threads may
  still be allocating and deallocating memory while a signal
  handler is called.
*/
#define MAGICK_BREAK_HEAP 0
#if MAGICK_BREAK_HEAP
static void
PanicFreeFunc(void *ptr)
{
  char err_str[] = "Attempt to free memory in PanicDestroyMagick()\n";
  ARG_NOT_USED(ptr);
  if (write(STDERR_FILENO,err_str,sizeof(err_str)-1) == -1)
    {
      /* Exists to quench warning */
    }
}
static void *
PanicMallocFunc(size_t size)
{
  char err_str[] = "Attempt to malloc memory in PanicDestroyMagick()\n";
  ARG_NOT_USED(size);
  if (write(STDERR_FILENO,err_str,sizeof(err_str)-1) == -1)
    {
      /* Exists to quench warning */
    }
  return (void *) NULL;
}
static void *
PanicReallocFunc(void *ptr, size_t size)
{
  char err_str[] = "Attempt to realloc memory in PanicDestroyMagick()\n";
  ARG_NOT_USED(ptr);
  ARG_NOT_USED(size);
  if (write(STDERR_FILENO,err_str,sizeof(err_str)-1) == -1)
    {
      /* Exists to quench warning */
    }
  return (void *) NULL;
}
#endif /* if MAGICK_BREAK_HEAP */
MagickExport void
PanicDestroyMagick(void)
{
  if (MagickInitialized == InitInitialized)
    {
#if MAGICK_BREAK_HEAP
      /*
        Enforce that we don't use the memory allocation functions by
        setting them all to dummy functions.
      */
      MagickAllocFunctions(PanicFreeFunc,PanicMallocFunc,PanicReallocFunc);
#endif /* if MAGICK_BREAK_HEAP */
      /*
        Release persistent resources
      */
      PurgeTemporaryFilesAsyncSafe();
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t M a g i c k I n f o                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SetMagickInfo allocates a MagickInfo structure and initializes the
%  members to default values.
%
%  The format of the SetMagickInfo method is:
%
%      MagickInfo *SetMagickInfo(const char *name)
%
%  A description of each parameter follows:
%
%    o magick_info: Method SetMagickInfo returns the allocated and initialized
%      MagickInfo structure.
%
%    o name: a character string that represents the image format associated
%      with the MagickInfo structure.
%
%
*/
MagickExport MagickInfo *
SetMagickInfo(const char *name)
{
  MagickInfo
    *magick_info;

  assert(name != (const char *) NULL);
  magick_info=MagickAllocateMemory(MagickInfo *,sizeof(MagickInfo));
  if (magick_info == (MagickInfo *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToAllocateMagickInfo);
  (void) memset(magick_info,0,sizeof(MagickInfo));
  magick_info->name=name;
  magick_info->adjoin=MagickTrue;
  magick_info->raw=MagickFalse;
  magick_info->stealth=MagickFalse;
  magick_info->seekable_stream=MagickFalse;
  magick_info->blob_support=MagickTrue;
  magick_info->thread_support=MagickTrue;
  magick_info->coder_class=StableCoderClass;
  magick_info->extension_treatment=HintExtensionTreatment;
  magick_info->signature=MagickSignature;
  return(magick_info);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r M a g i c k I n f o                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterMagickInfo removes a name from the magick info list.  It
%  returns MagickFail if the name does not exist in the list otherwise
%  MagickPass.
%
%  The format of the UnregisterMagickInfo method is:
%
%      unsigned int UnregisterMagickInfo(const char *name)
%
%  A description of each parameter follows:
%
%    o status: Method UnregisterMagickInfo returns False if the name does not
%      exist in the list otherwise True.
%
%    o name: a character string that represents the image format we are
%      looking for.
%
*/
MagickExport MagickPassFail
UnregisterMagickInfo(const char *name)
{
  MagickInfo
    *magick_info;

  register MagickInfo
    *p;

  unsigned int
    status;

  assert(name != (const char *) NULL);
  status=MagickFail;
  LockSemaphoreInfo(magick_semaphore);
  for (p=magick_list; p != (MagickInfo *) NULL; p=p->next)
  {
    if (LocaleCompare(p->name,name) != 0)
      continue;
    if (p->next != (MagickInfo *) NULL)
      p->next->previous=p->previous;
    if (p->previous != (MagickInfo *) NULL)
      p->previous->next=p->next;
    else
      magick_list=p->next;
    magick_info=p;
    DestroyMagickInfo(&magick_info);
    status=MagickPass;
    break;
  }
  UnlockSemaphoreInfo(magick_semaphore);
  return(status);
}
