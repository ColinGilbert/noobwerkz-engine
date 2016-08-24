/*
% Copyright (C) 2003 - 2016 GraphicsMagick Group
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
%               CCCC   OOO   M   M  M   M   AAA   N   N   DDDD                %
%              C      O   O  MM MM  MM MM  A   A  NN  N   D   D               %
%              C      O   O  M M M  M M M  AAAAA  N N N   D   D               %
%              C      O   O  M   M  M   M  A   A  N  NN   D   D               %
%               CCCC   OOO   M   M  M   M  A   A  N   N   DDDD                %
%                                                                             %
%                                                                             %
%                        Image Command Methods                                %
%                                                                             %
%                           Software Design                                   %
%                             John Cristy                                     %
%                            Bill Radcliffe                                   %
%                              July 2002                                      %
%                            Bob Friesenhahn                                  %
%                              2003-2008                                      %
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
#include "magick/analyze.h"
#include "magick/attribute.h"
#include "magick/average.h"
#include "magick/cdl.h"
#include "magick/channel.h"
#include "magick/color.h"
#include "magick/color_lookup.h"
#include "magick/colormap.h"
#include "magick/confirm_access.h"
#include "magick/constitute.h"
#include "magick/command.h"
#include "magick/compare.h"
#include "magick/composite.h"
#include "magick/decorate.h"
#include "magick/delegate.h"
#include "magick/describe.h"
#include "magick/effect.h"
#include "magick/enhance.h"
#include "magick/enum_strings.h"
#include "magick/fx.h"
#include "magick/gem.h"
#include "magick/hclut.h"
#include "magick/log.h"
#include "magick/magic.h"
#include "magick/magick.h"
#include "magick/map.h"
#include "magick/module.h"
#include "magick/monitor.h"
#include "magick/montage.h"
#include "magick/operator.h"
#include "magick/paint.h"
#include "magick/pixel_cache.h"
#include "magick/profile.h"
#include "magick/quantize.h"
#include "magick/registry.h"
#include "magick/render.h"
#include "magick/resize.h"
#include "magick/resource.h"
#include "magick/shear.h"
#include "magick/semaphore.h"
#include "magick/transform.h"
#include "magick/utility.h"
#include "magick/version.h"
#include "magick/xwindow.h"

/*
  Typedef declarations.
*/
typedef enum
{
  SingleMode = 0x01,
  BatchMode = 0x02
} RunMode;

typedef enum
{
  OptionSuccess = 0,
  OptionHelp = -1,
  OptionUnknown = -2,
  OptionMissingValue = -3,
  OptionInvalidValue = -4
} OptionStatus;

typedef struct _CompositeOptions
{
  char
    *displace_geometry,
    *geometry,
    *unsharp_geometry,
    *watermark_geometry;

  CompositeOperator
    compose;

  GravityType
    gravity;

  double
    dissolve;

  long
    stegano;

  unsigned int
    stereo,
    tile;
} CompositeOptions;

typedef int (*CommandLineParser)(FILE *in, int acmax, char **av);

#define SIZE_OPTION_VALUE 256
typedef struct _BatchOptions {
  MagickBool        stop_on_error,
                    is_feedback_enabled,
                    is_echo_enabled;
  char              prompt[SIZE_OPTION_VALUE],
                    pass[SIZE_OPTION_VALUE],
                    fail[SIZE_OPTION_VALUE];
  CommandLineParser command_line_parser;
} BatchOptions;

typedef unsigned int (*CommandVectorHandler)(ImageInfo *image_info,
  int argc,char **argv,char **metadata,ExceptionInfo *exception);

typedef void (*UsageVectorHandler)(void);

typedef struct _CommandInfo
{
  const char           *command;
  const char           *description;
  CommandVectorHandler  command_vector;
  UsageVectorHandler    usage_vector;
  int                   pass_metadata;
  RunMode               support_mode;
} CommandInfo;

static void InitializeBatchOptions(MagickBool);
static MagickBool GMCommandSingle(int argc, char **argv);
static int ProcessBatchOptions(int argc, char **argv, BatchOptions *options);
static int ParseUnixCommandLine(FILE *in, int acmax, char **av);
static int ParseWindowsCommandLine(FILE *in, int acmax, char **av);

static void
#if defined(HasX11)
  AnimateUsage(void),
#endif /* HasX11 */
  BatchUsage(void),
  BenchmarkUsage(void),
  CompositeUsage(void),
  CompareUsage(void),
  ConjureUsage(void),
  ConvertUsage(void),
#if defined(HasX11)
  DisplayUsage(void),
#endif /* HasX11 */
  GMUsage(void),
  IdentifyUsage(void),
#if defined(HasX11)
  ImportUsage(void),
#endif /* HasX11 */
  LiberateArgumentList(const int argc,char **argv),
  MogrifyUsage(void),
  MontageUsage(void),
  SetUsage(void),
  TimeUsage(void);

static unsigned int
  HelpCommand(ImageInfo *image_info,int argc,char **argv,
              char **metadata,ExceptionInfo *exception),
#if defined(MSWINDOWS)
  RegisterCommand(ImageInfo *image_info,int argc,char **argv,
                 char **metadata,ExceptionInfo *exception),
#endif
  SetCommand(ImageInfo *image_info,int argc,char **argv,
                 char **metadata,ExceptionInfo *exception),
  VersionCommand(ImageInfo *image_info,int argc,char **argv,
                 char **metadata,ExceptionInfo *exception);

static const CommandInfo commands[] =
  {
#if defined(HasX11)
    { "animate", "animate a sequence of images",
      AnimateImageCommand, AnimateUsage, 0, SingleMode | BatchMode },
#endif
    { "batch", "issue multiple commands in interactive or batch mode",
      0, BatchUsage, 1, SingleMode },
    { "benchmark", "benchmark one of the other commands",
      BenchmarkImageCommand, BenchmarkUsage, 1, SingleMode | BatchMode },
    { "compare", "compare two images",
      CompareImageCommand, CompareUsage, 0, SingleMode | BatchMode },
    { "composite", "composite images together",
      CompositeImageCommand, CompositeUsage, 0, SingleMode | BatchMode },
    { "conjure", "execute a Magick Scripting Language (MSL) XML script",
      ConjureImageCommand, ConjureUsage, 0, SingleMode | BatchMode },
    { "convert", "convert an image or sequence of images",
      ConvertImageCommand, ConvertUsage, 0, SingleMode | BatchMode },
#if defined(HasX11)
    { "display", "display an image on a workstation running X",
      DisplayImageCommand, DisplayUsage, 0, SingleMode | BatchMode },
#endif
    { "help", "obtain usage message for named command",
      HelpCommand, GMUsage, 0, SingleMode | BatchMode },
    { "identify", "describe an image or image sequence",
      IdentifyImageCommand, IdentifyUsage, 1, SingleMode | BatchMode },
#if defined(HasX11)
    { "import", "capture an application or X server screen",
      ImportImageCommand, ImportUsage, 0, SingleMode | BatchMode },
#endif
    { "mogrify", "transform an image or sequence of images",
      MogrifyImageCommand, MogrifyUsage, 0, SingleMode | BatchMode },
    { "montage", "create a composite image (in a grid) from separate images",
      MontageImageCommand, MontageUsage, 0, SingleMode | BatchMode },
    { "set", "change batch mode option",
      SetCommand, SetUsage, 1, BatchMode },
    { "time", "time one of the other commands",
      TimeImageCommand, TimeUsage, 1, SingleMode | BatchMode },
    { "version", "obtain release version",
      VersionCommand, 0, 0, SingleMode | BatchMode },
#if defined(MSWINDOWS)
    { "register", "register this application as the source of messages",
      RegisterCommand, 0, 0, SingleMode | BatchMode },
#endif
    { 0, 0, 0, 0, 0, 0}
  };

static SemaphoreInfo
  *command_semaphore = (SemaphoreInfo *) NULL;

static RunMode run_mode = SingleMode;

static BatchOptions batch_options;

static const char *on_off_option_values[3] = { "off", "on", (char *) NULL };

static const char *escape_option_values[3] = { "unix", "windows", (char *)NULL };

#define MAX_PARAM_CHAR 4096
#define MAX_PARAM 256
static char commandline[MAX_PARAM_CHAR+2];

#define PrintVersionAndCopyright() { \
  (void) printf("%.1024s\n",GetMagickVersion((unsigned long *) NULL)); \
  (void) printf("%.1024s\n",GetMagickCopyright()); \
}

#define PrintUsageHeader() { \
  if (run_mode != BatchMode) \
    PrintVersionAndCopyright(); \
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C o m m a n d A c c e s s M o n i t o r                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CommandAccessMonitor displays the files and programs which are
%  attempted to be accessed.
%
%  The format of the CommandAccessMonitor method is:
%
%      MagickBool CommandAccessMonitor(const ConfirmAccessMode mode,
%                                      const char *path,
%                                      ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o mode: The type of access to be performed.
%
%    o path: The local path or URL requested to be accessed.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
static MagickBool CommandAccessMonitor(const ConfirmAccessMode mode,
				       const char *path,
				       ExceptionInfo *exception)
{
  const char
    *env;

  ARG_NOT_USED(mode);
  ARG_NOT_USED(path);
  ARG_NOT_USED(exception);

  if (((env=getenv("MAGICK_ACCESS_MONITOR")) != (const char *) NULL) &&
      (LocaleCompare(env,"TRUE") == 0))
    {
      (void) fprintf(stderr,"  %s %s\n",
		     ConfirmAccessModeToString(mode),path);
    }
  return MagickPass;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C o m m a n d P r o g r e s s M o n i t o r                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CommandProgressMonitor displays the progress a task is making in
%  completing a task.
%
%  The format of the CommandProgressMonitor method is:
%
%      MagickBool CommandProgressMonitor(const char *task,
%        const magick_int64_t quantum,const magick_uint64_t span,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o task: Identifies the task in progress.
%
%    o quantum: Specifies the quantum position within the span which represents
%      how much progress has been made in completing a task.
%
%    o span: Specifies the span relative to completing a task.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
static MagickBool CommandProgressMonitor(const char *task,
					 const magick_int64_t quantum,
					 const magick_uint64_t span,
					 ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);

  if ((span > 1) && (quantum >= 0) && ((magick_uint64_t) quantum < span))
    {
      const char
        *p;

      /* Skip over any preceding white space */
      for (p=task; (*p) && (isspace((int) *p)); p++);
      (void) fprintf(stderr,"  %3lu%% %s\r",
                     (unsigned long)
		     ((double) 100.0*quantum/
		      (
#ifdef _MSC_VER
# if _MSC_VER <= 1200 /*Older Visual Studio lacks UINT64 to double conversion*/
		       (magick_int64_t)
# endif
#endif
		       span-1)),
                     p);
      if ((magick_uint64_t) quantum == (span-1))
        (void) fprintf(stderr,"\n");
      (void) fflush(stderr);
    }
  return(MagickTrue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   N o r m a l i z e S a m p l i n g F a c t o r                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  NormalizeSamplingFactor() transforms industry-standard subsampling
%  specifications into the internal geometry-style form.
%
%  The format of the NormalizeSamplingFactor method is:
%
%      void NormalizeSamplingFactor(ImageInfo *image_info)
%
%  A description of each parameter follows:
%
%   o image_info: ImageInfo structure containing sampling factor to transform.
%
%
*/
static void NormalizeSamplingFactor(ImageInfo *image_info)
{
  int
    count;

  unsigned int
    factors[3],
    horizontal,
    vertical;

  char
    buffer[MaxTextExtent];

  if (image_info->sampling_factor == NULL)
    return;

  factors[0]=factors[1]=factors[2]=0;
  count=sscanf(image_info->sampling_factor,"%u:%u:%u",
               &factors[0], &factors[1], &factors[2]);
  if ((count != 3) || (factors[1] == 0))
    return;

  horizontal=factors[0]/factors[1];
  vertical=1;
  if (factors[2] == 0)
    vertical=2;

  FormatString(buffer,"%ux%u",horizontal,vertical);
  (void) CloneString(&image_info->sampling_factor,buffer);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A n i m a t e U s a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AnimateUsage() displays the program command syntax.
%
%  The format of the AnimateUsage method is:
%
%      void AnimateUsage()
%
%  A description of each parameter follows:
%
%
*/
#if defined(HasX11)
static void AnimateUsage(void)
{
  const char
    **p;

  static const char
    *buttons[]=
    {
      "Press any button to map or unmap the Command widget",
      (char *) NULL
    },
    *options[]=
    {
      "-authenticate value  decrypt image with this password",
      "-backdrop            display image centered on a backdrop",
      "-colormap type       Shared or Private",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     alternate image colorspace",
      "-crop geometry       preferred size and location of the cropped image",
      "-debug events        display copious debugging information",
      "-define values       Coder/decoder specific options",
      "-delay value         display the next image after pausing",
      "-density geometry    horizontal and vertical density of the image",
      "-depth value         image depth",
      "-display server      display image to this X server",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-gamma value         level of gamma correction",
      "-geometry geometry   preferred size and location of the Image window",
      "-help                print program options",
      "-interlace type      None, Line, Plane, or Partition",
      "-limit type value    Disk, File, Map, Memory, Pixels, Width, Height or",
      "                     Threads resource limit",
      "-log format          format of debugging information",
      "-matte               store matte channel if the image has one",
      "-map type            display image using this Standard Colormap",
      "-monitor             show progress indication",
      "-monochrome          transform image to black and white",
      "-noop                do not apply options to image",
      "-pause               seconds to pause before reanimating",
      "-remote command      execute a command in a remote display process",
      "-rotate degrees      apply Paeth rotation to the image",
      "-sampling-factor HxV[,...]",
      "                     horizontal and vertical sampling factors",
      "-scenes range        image scene range",
      "-size geometry       width and height of image",
      "-treedepth value     color tree depth",
      "-trim                trim image edges",
      "-type type           image type",
      "-verbose             print detailed information about the image",
      "-version             print version information",
      "-visual type         display image using this visual type",
      "-virtual-pixel method",
      "                     Constant, Edge, Mirror, or Tile",
      "-window id           display image to background of this window",
      (char *) NULL
    };

  PrintUsageHeader();
  (void) printf("Usage: %.1024s [options ...] file [ [options ...] file ...]\n",
    GetClientName());
  (void) printf("\nWhere options include: \n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf(
    "\nIn addition to those listed above, you can specify these standard X\n");
  (void) printf(
    "resources as command line options:  -background, -bordercolor,\n");
  (void) printf(
    "-borderwidth, -font, -foreground, -iconGeometry, -iconic, -name,\n");
  (void) printf("-mattecolor, -shared-memory, or -title.\n");
  (void) printf(
    "\nBy default, the image format of `file' is determined by its magic\n");
  (void) printf(
    "number.  To specify a particular image format, precede the filename\n");
  (void) printf(
    "with an image format name and a colon (i.e. ps:image) or specify the\n");
  (void) printf(
    "image type as the filename suffix (i.e. image.ps).  Specify 'file' as\n");
  (void) printf("'-' for standard input or output.\n");
  (void) printf("\nButtons: \n");
  for (p=buttons; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
}
#endif /* HasX11 */
MagickExport unsigned int AnimateImageCommand(ImageInfo *image_info,
  int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
#if defined(HasX11)
  char
    *option,
    *resource_value,
    *server_name;

  const char
    *client_name;

  Display
    *display;

  Image
    *image,
    *image_list,
    *loaded_image,
    *next_image;

  long
    first_scene,
    j,
    k,
    last_scene,
    scene,
    x;

  QuantizeInfo
    *quantize_info;

  register Image
    *p;

  register long
    i;

  unsigned int
    status;

  MagickXResourceInfo
    resource_info;

  XrmDatabase
    resource_database;

  /*
    Set defaults.
  */
  SetNotifyHandlers;
  display=(Display *) NULL;
  first_scene=0;
  image=(Image *) NULL;
  image_list=(Image *) NULL;
  last_scene=0;
  server_name=(char *) NULL;
  status=True;
  /*
    Check for server name specified on the command line.
  */
  for (i=1; i < argc; i++)
  {
    /*
      Check command line for server name.
    */
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      continue;
    if (LocaleCompare("display",option+1) == 0)
      {
        /*
          User specified server name.
        */
        i++;
        if (i == argc)
          MagickFatalError(OptionFatalError,MissingArgument,option);
        server_name=argv[i];
        break;
      }
    if (LocaleCompare("help",option+1) == 0)
      {
        AnimateUsage();
        return MagickPass;
      }
    if (LocaleCompare("version",option+1) == 0)
      {
        (void) VersionCommand(image_info,argc,argv,metadata,exception);
        return MagickPass;
      }
  }

  /*
    Expand argument list
  */
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickFatalError(ResourceLimitFatalError,MemoryAllocationFailed,
      (char *) NULL);

  /*
    Get user defaults from X resource database.
  */
  display=XOpenDisplay(server_name);
  if (display == (Display *) NULL)
    MagickFatalError(XServerFatalError,UnableToOpenXServer,
      XDisplayName(server_name));
  (void) XSetErrorHandler(MagickXError);
  client_name=GetClientName();
  resource_database=MagickXGetResourceDatabase(display,client_name);
  MagickXGetResourceInfo(resource_database,(char *) client_name,&resource_info);
  image_info=resource_info.image_info;
  quantize_info=resource_info.quantize_info;
  image_info->density=
    MagickXGetResourceInstance(resource_database,client_name,"density",(char *) NULL);
  if (image_info->density == (char *) NULL)
    image_info->density=MagickXGetScreenDensity(display);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"interlace","none");
  image_info->interlace=StringToInterlaceType(resource_value);
  if (image_info->interlace == UndefinedInterlace)
    MagickError(OptionFatalError,InvalidInterlaceType,resource_value);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"verbose","False");
  image_info->verbose=MagickIsTrue(resource_value);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"dither","True");
  quantize_info->dither=MagickIsTrue(resource_value);
  /*
    Parse command line.
  */
  j=1;
  k=0;
  for (i=1; i <= argc; i++)
  {
    if (i < argc)
      option=argv[i];
    else
      if (image != (Image *) NULL)
        break;
      else
        option=(char *) "logo:Untitled";
    if ((strlen(option) < 2) ||
	/* stdin + subexpression */
	((option[0] == '-') && (option[1] == '[')) ||
	((option[0] != '-') && option[0] != '+'))
      {
        /*
          Option is a file name.
        */
        k=i;
        for (scene=first_scene; scene <= last_scene ; scene++)
        {
          /*
            Read image.
          */
          (void) strlcpy(image_info->filename,option,MaxTextExtent);
          if (first_scene != last_scene)
            {
              char
                filename[MaxTextExtent];

              /*
                Form filename for multi-part images.
              */
              (void) MagickSceneFileName(filename,image_info->filename,"[%lu]",MagickTrue,scene);
              (void) strlcpy(image_info->filename,filename,MaxTextExtent);
            }
          image_info->colorspace=quantize_info->colorspace;
          image_info->dither=quantize_info->dither;
          next_image=ReadImage(image_info,exception);
          if (exception->severity > UndefinedException)
	    {
	      CatchException(exception);
	      DestroyExceptionInfo(exception);
	      GetExceptionInfo(exception);
	    }
          status&=next_image != (Image *) NULL;
          if (next_image == (Image *) NULL)
            continue;
          if (image == (Image *) NULL)
            {
              image=next_image;
              continue;
            }
          /*
            Link image into image list.
          */
          for (p=image; p->next != (Image *) NULL; p=p->next);
          next_image->previous=p;
          p->next=next_image;
        }
        continue;
      }
    if (j != (k+1))
      {
        status&=MogrifyImages(image_info,i-j,argv+j,&image);
        (void) CatchImageException(image);
        AppendImageToList(&image_list,image);
        image=(Image *) NULL;
        j=k+1;
      }
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("authenticate",option+1) == 0)
          {
            (void) CloneString(&image_info->authenticate,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) CloneString(&image_info->authenticate,argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'b':
      {
        if (LocaleCompare("backdrop",option+1) == 0)
          {
            resource_info.backdrop=(*option == '-');
            break;
          }
        if (LocaleCompare("background",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.background_color=argv[i];
                (void) QueryColorDatabase(argv[i],&image_info->background_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.border_color=argv[i];
                (void) QueryColorDatabase(argv[i],&image_info->border_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("borderwidth",option+1) == 0)
          {
            resource_info.border_width=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.border_width=MagickAtoI(argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'c':
      {
        if (LocaleCompare("colormap",option+1) == 0)
          {
            resource_info.colormap=PrivateColormap;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                resource_info.colormap=UndefinedColormap;
                if (LocaleCompare("private",option) == 0)
                  resource_info.colormap=PrivateColormap;
                if (LocaleCompare("shared",option) == 0)
                  resource_info.colormap=SharedColormap;
                if (resource_info.colormap == UndefinedColormap)
                  MagickFatalError(OptionFatalError,UnrecognizedColormapType,
                    option);
              }
            break;
          }
        if (LocaleCompare("colors",option+1) == 0)
          {
            quantize_info->number_colors=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                quantize_info->number_colors=MagickAtoL(argv[i]);
              }
            break;
          }
        if (LocaleCompare("colorspace",option+1) == 0)
          {
            quantize_info->colorspace=RGBColorspace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                quantize_info->colorspace=StringToColorspaceType(option);
                if (IsGrayColorspace(quantize_info->colorspace))
                  {
                    quantize_info->number_colors=256;
                    quantize_info->tree_depth=8;
                  }
                if (quantize_info->colorspace == UndefinedColorspace)
                  MagickFatalError(OptionFatalError,InvalidColorspaceType,
                    option);
              }
            break;
          }
        if (LocaleCompare("crop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            (void) SetLogEventMask("None");
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                (void) SetLogEventMask(argv[i]);
              }
            break;
          }
        if (LocaleCompare("define",option+1) == 0)
          {
            i++;
            if (i == argc)
              MagickFatalError(OptionFatalError,MissingArgument,option);
            if (*option == '+')
              (void) RemoveDefinitions(image_info,argv[i]);
            else
              (void) AddDefinitions(image_info,argv[i],exception);
            break;
          }
        if (LocaleCompare("delay",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            (void) CloneString(&image_info->density,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) CloneString(&image_info->density,argv[i]);
              }
            break;
          }
        if (LocaleCompare("depth",option+1) == 0)
          {
            image_info->depth=QuantumDepth;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                image_info->depth=MagickAtoL(argv[i]);
              }
            break;
          }
        if (LocaleCompare("display",option+1) == 0)
          {
            (void) CloneString(&image_info->server_name,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) CloneString(&image_info->server_name,argv[i]);
              }
            break;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            quantize_info->dither=(*option == '-');
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'f':
      {
        if (LocaleCompare("font",option+1) == 0)
          {
            (void) CloneString(&image_info->font,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) CloneString(&image_info->font,argv[i]);
              }
            if ((image_info->font == (char *) NULL) ||
                (*image_info->font != '@'))
              resource_info.font=AllocateString(image_info->font);
            break;
          }
        if (LocaleCompare("foreground",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                resource_info.foreground_color=argv[i];
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'g':
      {
        if (LocaleCompare("gamma",option+1) == 0)
          {
            i++;
            if (i == argc)
              MagickFatalError(OptionFatalError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("geometry",option+1) == 0)
          {
            resource_info.image_geometry=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.image_geometry=argv[i];
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'h':
      {
        if (LocaleCompare("help",option+1) == 0)
          {
            AnimateUsage();
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'i':
      {
        if (LocaleCompare("iconGeometry",option+1) == 0)
          {
            resource_info.icon_geometry=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.icon_geometry=argv[i];
              }
            break;
          }
        if (LocaleCompare("iconic",option+1) == 0)
          {
            resource_info.iconic=(*option == '-');
            break;
          }
        if (LocaleCompare("interlace",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                image_info->interlace=StringToInterlaceType(option);
                if (image_info->interlace == UndefinedInterlace)
                  MagickFatalError(OptionFatalError,InvalidInterlaceType,
                    option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'l':
      {
        if (LocaleCompare("limit",option+1) == 0)
          {
            if (*option == '-')
              {
                ResourceType
                  resource_type;

                char
                  *type;

                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                type=argv[i];
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_type=StringToResourceType(type);
                if (resource_type == UndefinedResource)
                  MagickFatalError(OptionFatalError,UnrecognizedResourceType,type);
                (void) SetMagickResourceLimit(resource_type,MagickSizeStrToInt64(argv[i],1024));
              }
            break;
          }
        if (LocaleCompare("log",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) SetLogFormat(argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'm':
      {
        if (LocaleCompare("map",option+1) == 0)
          {
            (void) strcpy(argv[i]+1,"sans");
            resource_info.map_type=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.map_type=argv[i];
              }
            break;
          }
        if (LocaleCompare("matte",option+1) == 0)
          break;
        if (LocaleCompare("mattecolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.matte_color=argv[i];
                (void) QueryColorDatabase(argv[i],&image_info->matte_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("monitor",option+1) == 0)
          {
            if (*option == '+')
	      {
		(void) SetMonitorHandler((MonitorHandler) NULL);
		(void) MagickSetConfirmAccessHandler((ConfirmAccessHandler) NULL);
	      }
            else
	      {
		(void) SetMonitorHandler(CommandProgressMonitor);
		(void) MagickSetConfirmAccessHandler(CommandAccessMonitor);
	      }
            break;
          }
        if (LocaleCompare("monochrome",option+1) == 0)
          {
            image_info->monochrome=(*option == '-');
            if (image_info->monochrome)
              {
                quantize_info->number_colors=2;
                quantize_info->tree_depth=8;
                quantize_info->colorspace=GRAYColorspace;
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'n':
      {
        if (LocaleCompare("name",option+1) == 0)
          {
            resource_info.name=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.name=argv[i];
              }
            break;
          }
        if (LocaleCompare("noop",option+1) == 0)
          break;
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'p':
      {
        if (LocaleCompare("pause",option+1) == 0)
          {
            resource_info.pause=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            resource_info.pause=MagickAtoI(argv[i]);
            break;
          }
        break;
      }
      case 'r':
      {
        if (LocaleCompare("remote",option+1) == 0)
          {
            i++;
            if (i == argc)
              MagickFatalError(OptionFatalError,MissingArgument,option);
            status=MagickXRemoteCommand(display,resource_info.window_id,argv[i]);
            Exit(!status);
          }
        if (LocaleCompare("rotate",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              MagickFatalError(OptionFatalError,MissingArgument,option);
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 's':
      {
        if (LocaleCompare("sampling-factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
                NormalizeSamplingFactor(image_info);
              }
            break;
          }
        if (LocaleCompare("scenes",option+1) == 0)
          {
            first_scene=0;
            last_scene=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                first_scene=MagickAtoL(argv[i]);
                last_scene=first_scene;
                (void) sscanf(argv[i],"%ld-%ld",&first_scene,&last_scene);
              }
            break;
          }
        if (LocaleCompare("shared-memory",option+1) == 0)
          {
            resource_info.use_shared_memory=(*option == '-');
            break;
          }
        if (LocaleCompare("size",option+1) == 0)
          {
            (void) CloneString(&image_info->size,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) CloneString(&image_info->size,argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 't':
      {
        if (LocaleCompare("text-font",option+1) == 0)
          {
            resource_info.text_font=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.text_font=argv[i];
              }
            break;
          }
        if (LocaleCompare("title",option+1) == 0)
          {
            resource_info.title=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.title=argv[i];
              }
            break;
          }
        if (LocaleCompare("treedepth",option+1) == 0)
          {
            quantize_info->tree_depth=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                quantize_info->tree_depth=MagickAtoI(argv[i]);
              }
            break;
          }
        if (LocaleCompare("trim",option+1) == 0)
          {
            break;
          }
        if (LocaleCompare("type",option+1) == 0)
          {
            resource_info.image_info->type=UndefinedType;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                resource_info.image_info->type=StringToImageType(option);
                if (resource_info.image_info->type == UndefinedType)
                  MagickFatalError(OptionFatalError,UnrecognizedImageType,
                                   option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose+=(*option == '-');
            break;
          }
        if (LocaleCompare("version",option+1) == 0)
          break;
        if (LocaleCompare("virtual-pixel",option+1) == 0)
          {
            if (*option == '-')
              {
                VirtualPixelMethod
                  virtual_pixel_method;

                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                option=argv[i];
                virtual_pixel_method=StringToVirtualPixelMethod(option);
                if (virtual_pixel_method == UndefinedVirtualPixelMethod)
                  MagickFatalError(OptionFatalError,UnrecognizedVirtualPixelMethod,
                    option);
              }
            break;
          }
        if (LocaleCompare("visual",option+1) == 0)
          {
            resource_info.visual_type=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                resource_info.visual_type=argv[i];
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'w':
      {
        if (LocaleCompare("window",option+1) == 0)
          {
            resource_info.window_id=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                resource_info.window_id=argv[i];
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case '?':
      {
        AnimateUsage();
        break;
      }
      default:
      {
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
    }
  }
  i--;
  if ((image == (Image *) NULL) && (image_list == (Image *) NULL))
    MagickFatalError(OptionFatalError,RequestDidNotReturnAnImage,(char *) NULL);
  if (image == (Image *) NULL)
    {
      status&=MogrifyImages(image_info,i-j,argv+j,&image_list);
      (void) CatchImageException(image_list);
    }
  else
    {
      status&=MogrifyImages(image_info,i-j,argv+j,&image);
      (void) CatchImageException(image);
      AppendImageToList(&image_list,image);
    }
  if (resource_info.window_id != (char *) NULL)
    MagickXAnimateBackgroundImage(display,&resource_info,image_list);
  else
    {
      /*
        Animate image to X server.
      */
      loaded_image=MagickXAnimateImages(display,&resource_info,argv,argc,image_list);
      while (loaded_image != (Image *) NULL)
      {
        image_list=loaded_image;
        loaded_image=
          MagickXAnimateImages(display,&resource_info,argv,argc,image_list);
      }
    }
  DestroyImageList(image_list);
  LiberateArgumentList(argc,argv);
  MagickXDestroyResourceInfo(&resource_info);
  MagickXDestroyX11Resources();
  (void) XCloseDisplay(display);
  return(status);
#else
  ARG_NOT_USED(image_info);
  ARG_NOT_USED(argc);
  ARG_NOT_USED(argv);
  ARG_NOT_USED(metadata);
  ARG_NOT_USED(exception);

  MagickFatalError(MissingDelegateError,XWindowLibraryIsNotAvailable,
    (char *) NULL);
  return(False);
#endif
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B a t c h C o m m a n d                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BatchCommand runs multiple commands in interactive or batch mode.
%
%  The format of the BatchCommand method is:
%
%      unsigned int BatchCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
static MagickBool BatchCommand(int argc, char **argv)
{
  int result;
  MagickBool hasInputFile;
  int ac;
  char *av[MAX_PARAM+1];

  {
    char client_name[MaxTextExtent];
    FormatString(client_name,"%.1024s %s", argv[0], argv[1]);
    (void) SetClientName(client_name);
  }

  {
    BatchOptions dummy;
    result = ProcessBatchOptions(argc-1, argv+1, &dummy);
    if (result < 0 )
      {
        BatchUsage();
        return result == OptionHelp;
      }
  }

  hasInputFile = (++result <= argc-1);
  if (result < argc-1)
    {
      (void) fprintf(stderr, "Error: unexpected parameter: %s\n", argv[result+1]);
      BatchUsage();
      return MagickFalse;
    }

  if (hasInputFile && (argv[result][0] != '-' || argv[result][1] != '\0'))
    if (freopen(argv[result], "r", stdin) == (FILE *)NULL)
      {
        perror(argv[result]);
        exit(1);
      }
      
  InitializeBatchOptions(!hasInputFile);
  result = ProcessBatchOptions(argc-1, argv+1, &batch_options);

  run_mode = BatchMode;
#if defined(MSWINDOWS)
  InitializeMagick((char *) NULL);
#else
  InitializeMagick(argv[0]);
#endif

  av[0] = argv[0];
  av[MAX_PARAM] = (char *)NULL;
  if (batch_options.prompt[0])
    {
      PrintVersionAndCopyright();
      (void) fflush(stdout);
    }

  for (;;)
    {
      if (batch_options.prompt[0])
        {
          (void) fputs(batch_options.prompt, stdout);
          (void) fflush(stdout);
        }

      ac = (batch_options.command_line_parser)(stdin, MAX_PARAM, av);
      if (ac < 0)
        {
          result = MagickTrue;
          break;
        };
      if (batch_options.is_echo_enabled)
        {
          int i;
          for (i = 1; i < ac; i++)
            {
              (void) fputs(av[i], stdout);
              (void) putchar(' ');
            }
          (void) putchar('\n');
          (void) fflush(stdout);
        }
      if (ac == 1)
        continue;
      if (ac > 0 && ac <= MAX_PARAM)
        result = GMCommandSingle(ac, av);
      else
        {
          if (ac == 0)
            (void) fprintf(stderr,
                           "Error: command line exceeded %d characters.\n",
                           MAX_PARAM_CHAR);
          else
            (void) fprintf(stderr,
                           "Error: command line exceeded %d parameters.\n",
                           MAX_PARAM);
          result = MagickFalse;
        }

      if (batch_options.is_feedback_enabled)
        {
          (void) fputs(result ? batch_options.pass : batch_options.fail, stdout);
          (void) fputc('\n', stdout);
        }
      (void) fflush(stderr);
      (void) fflush(stdout);

      if (batch_options.stop_on_error && !result)
        break;
    }

  if (batch_options.prompt[0])
    {
      (void) fputs("\n", stdout);
      (void) fflush(stdout);
    }
  DestroyMagick();
  return(result);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B a t c h O p t i o n U s a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BatchOptionUsage() displays the option detail for "batch" and "set" command.
%
%  The format of the BatchOptionUsage method is:
%
%      void BatchOptionUsage()
%
*/
static void BatchOptionUsage(void)
{
  static const char
    *options[]=
    {
      "-echo on|off         echo command back to standard out, default is off",
      "-escape unix|windows force use Unix or Windows escape format for command line",
      "                     argument parsing, default is platform dependent",
      "-fail text           when feedback is on, output the designated text if the",
      "                     command returns error, default is 'FAIL'",
      "-feedback on|off     print text (see -pass and -fail options) feedback after",
      "                     each command to indicate the result, default is off",
      "-help                print program options",
      "-pass text           when feedback is on, output the designated text if the",
      "                     command executed successfully, default is 'PASS'",
      "-prompt text         use the given text as command prompt. use text 'off' or",
      "                     empty string to turn off prompt. default to 'GM> ' if",
      "                     and only if batch mode was entered with no file argument",
      "-stop-on-error on|off",
      "                     when turned on, batch execution quits prematurely when",
      "                     any command returns error",
      (char *) NULL
    };

  const char
    **p;

  (void) printf("\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) puts("\nUnix escape allows the use backslash(\\), single quote(') and double quote(\") in");
  (void) puts("the command line. Windows escape only uses double quote(\").  For example,");

  (void) puts("\n    Orignal             Unix escape              Windows escape");
  (void) puts("    [a\\b\\c\\d]           [a\\\\b\\\\c\\\\d]             [a\\b\\c\\d]");
  (void) puts("    [Text with space]   [Text\\ with\\ space]      [\"Text with space\"]");
  (void) puts("    [Text with (\")]     ['Text with (\")']        [\"Text with (\"\")\"]");
  (void) puts("    [Mix: \"It's a (\\)\"] [\"Mix: \\\"It's a (\\\\)\\\"\"] [\"Mix: \"\"It's a (\\)\"\"\"]");
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B a t c h U s a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BatchUsage displays the program command syntax.
%
%  The format of the BatchUsage method is:
%
%      void BatchUsage()
%
*/
static void BatchUsage(void)
{
  PrintUsageHeader();
  (void) printf("Usage: %.1024s [options ...] [file|-]\n", GetClientName());
  BatchOptionUsage();
  (void) puts("\nUse '-' to read command from standard input without default prompt.");
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   B e n c h m a r k U s a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BenchmarkUsage() displays the program command syntax.
%
%  The format of the BenchmarkUsage method is:
%
%      void BenchmarkUsage()
%
%  A description of each parameter follows:
%
%
*/
static void BenchmarkUsage(void)
{
  static const char
    *options[]=
    {
      "-duration duration  duration to run each benchmark (in seconds)",
      "-iterations loops   number of command iterations",
      "-rawcsv             CSV output (threads,iterations,user_time,elapsed_time)",
      "-stepthreads step   step benchmark with increasing number of threads",
      (char *) NULL
    };

  const char
    **p;

  PrintUsageHeader();
  (void) printf("Usage: %.1024s options command ... ",GetClientName());
  (void) printf("\nWhere options include one of:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf("Followed by some other GraphicsMagick command\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  B e n c h m a r k I m a g e C o m m a n d                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BenchmarkImageCommand() executes a specified GraphicsMagick sub-command
%  (e.g. 'convert') for a specified number of interations or for a specified
%  elapsed time. When the command completes, various statistics are printed
%  including the number of iterations per second as a floating point value.
%
%  The format of the BenchmarkImageCommand method is:
%
%      unsigned int BenchmarkImageCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int ExecuteSubCommand(const ImageInfo *image_info,
  int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
  unsigned int
    status;

  ImageInfo
    *clone_info;

  char
    *text = (char *) NULL;;
  
  clone_info=CloneImageInfo(image_info);
  status=MagickCommand(clone_info,argc,argv,metadata,exception);
  if (metadata != (char **) NULL)
    text=*metadata;
  if (text != (char *) NULL)
    {
      if (strlen(text))
        {
          (void) fputs(text,stdout);
          (void) fputc('\n',stdout);
          (void) fflush(stdout);
        }
      MagickFreeMemory(text);
      *metadata=text;
    }
  DestroyImageInfo(clone_info);
  return status;
}
MagickExport unsigned int
BenchmarkImageCommand(ImageInfo *image_info,
		      int argc,char **argv,
		      char **metadata,ExceptionInfo *exception)
{
  MagickBool
    concurrent;

  MagickBool
    raw_csv,
    thread_bench;

  long
    current_threads,
    max_threads;

  double
    rate_total_st;

  double
    duration;

  long
    max_iterations,
    thread_step;

  unsigned int
    status=MagickTrue;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);

  if (argc < 2 || ((argc < 3) && (LocaleCompare("-help",argv[1]) == 0 ||
                                  LocaleCompare("-?",argv[1]) == 0)))
    {
      BenchmarkUsage();
      if (argc < 2)
	{
	  ThrowException(exception,OptionError,UsageError,NULL);
	  return MagickFail;
	}
      return MagickPass;
    }
  if (LocaleCompare("-version",argv[1]) == 0)
    {
      (void) VersionCommand(image_info,argc,argv,metadata,exception);
      return MagickPass;
    }

  /*
    Skip over our command name argv[0].
  */
  argc--;
  argv++;
  concurrent=MagickFalse;
  raw_csv=MagickFalse;
  thread_bench=MagickFalse;
  max_threads = (long) GetMagickResourceLimit(ThreadsResource);
  current_threads = 1;
  rate_total_st = 1.0;
  duration=-1.0;
  max_iterations=1L;
  thread_step=1L;

  while ((argc) && (*argv[0] == '-'))
    {
      if (LocaleCompare("-duration",argv[0]) == 0)
	{
	  argc--;
	  argv++;
	  if (argc)
	    {
	      duration=MagickAtoF(argv[0]);
	    }
	}
      else if (LocaleCompare("-iterations",argv[0]) == 0)
	{
	  argc--;
	  argv++;
	  if (argc)
	    {
	      max_iterations=MagickAtoL(argv[0]);
	    }
	}
      else if (LocaleCompare("-concurrent",argv[0]) == 0)
	{
	  concurrent=MagickTrue;
	}
      else if (LocaleCompare("-rawcsv",argv[0]) == 0)
	{
	  raw_csv=MagickTrue;
	}
      else if (LocaleCompare("-stepthreads",argv[0]) == 0)
	{
	  thread_bench=MagickTrue;
	  argc--;
	  argv++;
	  if (argc)
	    {
	      thread_step=MagickAtoL(argv[0]);
	    }
	}
      argc--;
      argv++;
    }

  if ((argc < 1) ||
      ((duration <= 0) && (max_iterations <= 0)))
    {
      BenchmarkUsage();
      ThrowException(exception,OptionError,UsageError,NULL);
      return MagickFail;
    }

  if (raw_csv)
    {
      /*
        Print a header line since spreadsheet programs (and Python's
        CSV parser) use the first line as the column titles.
      */
      (void) fprintf(stderr,"\"Threads\",\"Iterations\",\"User Time\",\"Elapsed Time\"\n");
    }

  do
    {
      char
	client_name[MaxTextExtent];

      long
	iteration=0;
    
      TimerInfo
	timer;

      if (thread_bench)
	(void) SetMagickResourceLimit(ThreadsResource,current_threads);


      (void) strlcpy(client_name,GetClientName(),sizeof(client_name));

      /*
	If doing a thread run-up, then warm things up first with one iteration.
      */
      if (thread_bench)
	status=ExecuteSubCommand(image_info,argc,argv,metadata,exception);

      GetTimerInfo(&timer);
    
      if (concurrent)
	{
	  MagickBool
	    quit = MagickFalse;

	  long
	    count = 0;

#if defined(HAVE_OPENMP)
	  omp_set_nested(MagickTrue);
#endif
	  if (duration > 0)
	    {
	      count=0;
#if defined(HAVE_OPENMP)
#  pragma omp parallel for shared(count, status, quit)
#endif
	      for (iteration=0; iteration < 1000000; iteration++)
		{
		  MagickPassFail
		    thread_status;

		  MagickBool
		    thread_quit;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_BenchmarkImageCommand)
#endif
		  thread_quit=quit;
		
		  if (thread_quit)
		    continue;

		  thread_status=ExecuteSubCommand(image_info,argc,argv,metadata,exception);
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_BenchmarkImageCommand)
#endif
		  {
		    count++;
		    if (!thread_status)
		      {
			status=thread_status;
			thread_quit=MagickTrue;
		      }
		    if (GetElapsedTime(&timer) > duration)
		      thread_quit=MagickTrue;
		    else
		      (void) ContinueTimer(&timer);
		    if (thread_quit)
		      quit=thread_quit;
		  }
		}
	    }
	  else if (max_iterations > 0)
	    {
#if defined(HAVE_OPENMP)
#  pragma omp parallel for shared(count, status, quit)
#endif
	      for (iteration=0; iteration < max_iterations; iteration++)
		{
		  MagickPassFail
		    thread_status;

		  MagickBool
		    thread_quit;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_BenchmarkImageCommand)
#endif
		  thread_quit=quit;

		  if (thread_quit)
		    continue;

		  thread_status=ExecuteSubCommand(image_info,argc,argv,metadata,exception);
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_BenchmarkImageCommand)
#endif
		  {
		    count++;
		    if (!thread_status)
		      {
			status=thread_status;
			thread_quit=MagickTrue;
		      }
		    if (thread_quit)
		      quit=thread_quit;
		  }
		}
	    }
	  iteration=count;
	}
      else
	{
	  if (duration > 0)
	    {
	      /*
		Loop until duration or max_iterations has been hit.
	      */
	      for (iteration=0; iteration < (LONG_MAX-1); )
		{
		  status=ExecuteSubCommand(image_info,argc,argv,metadata,exception);
		  iteration++;
		  if (!status)
		    break;
		  if (GetElapsedTime(&timer) > duration)
		    break;
		  (void) ContinueTimer(&timer);
		}
	    }
	  else if (max_iterations > 0)
	    {
	      /*
		Loop until max_iterations has been hit.
	      */
	      for (iteration=0; iteration < max_iterations; )
		{
		  status=ExecuteSubCommand(image_info,argc,argv,metadata,exception);
		  iteration++;
		  if (!status)
		    break;
		}
	    }
	}
      {
	double
	  rate_cpu,
	  rate_total,
	  /* resolution, */
	  user_time;

	double
	  elapsed_time;

	long
	  threads_limit;

	/* resolution=GetTimerResolution(); */
	user_time=GetUserTime(&timer);
	elapsed_time=GetElapsedTime(&timer);
	rate_total=(((double) iteration)/elapsed_time);
	rate_cpu=(((double) iteration)/user_time);
	threads_limit=(long) GetMagickResourceLimit(ThreadsResource);
	if (1 == threads_limit)
	  rate_total_st=rate_total;
	(void) fflush(stdout);
	if (raw_csv)
	  {
	    /* RAW CSV value output */
	    (void) fprintf(stderr,"\"%ld\",\"%ld\",\"%.2f\",\"%.3f\"",
			   threads_limit,iteration,user_time,elapsed_time);
	  }
	else
	  {
	    /* Formatted and summarized output */
	    (void) fprintf(stderr,
			   "Results: %ld threads %ld iter %.2fs user %.2fs total %.3f iter/s "
			   "%.3f iter/cpu",
			   threads_limit,iteration,user_time,elapsed_time,rate_total,rate_cpu);
	    if (thread_bench)
	      {
		double
		  karp_flatt_metric,
		  speedup;

		/* Speedup ratio */
		speedup=rate_total/rate_total_st;

		/* Karp-Flatt metric, http://en.wikipedia.org/wiki/Karp%E2%80%93Flatt_metric */
		karp_flatt_metric=1.0;
		if (threads_limit > 1)
		  karp_flatt_metric=((1.0/Min(threads_limit,speedup))-
				     (1.0/threads_limit))/(1.0-(1.0/threads_limit));
		(void) fprintf(stderr," %.2f speedup %.3f karp-flatt",speedup,karp_flatt_metric);
	      }
	  }
	(void) fprintf(stderr,"\n");
	(void) fflush(stderr);
      }
      /*
	Always measure with one thread, and then step up as if from zero.
      */
      if ((current_threads == 1) && (thread_step > 1))
	current_threads = thread_step;
      else
	current_threads += thread_step;
    }
  while ((thread_bench) && (current_threads <= max_threads));

  return status;
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C h e c k O p t i o n V a l u e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CheckOptionValue prints error message to stderr if value agrument is null
%  and returns OPtionMissingValue. Otherwise return OptionSuccess.
%
%  The format of the CheckOptionValue method is:
%
%      OptionStatus CheckOptionValue(const char *option, const char *value)
%
%  A description of each parameter follows:
%
%    o option: The option to check the value.
%
%    o value: The value to check for non-null.
%
*/
static OptionStatus CheckOptionValue(const char *option, const char *value)
{
  if (value == (char *) NULL)
    {
      fprintf(stderr, "Error: Missing value for %s option\n", option);
      return OptionMissingValue;
    }
  return OptionSuccess;
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  C o m p a r e I m a g e C o m m a n d                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CompareImageCommand() reads two images, compares them via a specified
%  comparison metric, and prints the results.
%
%  The format of the CompareImageCommand method is:
%
%      unsigned int CompareImageCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define ThrowCompareException(code,reason,description) \
{ \
  DestroyImageList(compare_image); \
  DestroyImageList(difference_image); \
  DestroyImageList(reference_image); \
  ThrowException(exception,code,reason,description); \
  LiberateArgumentList(argc,argv); \
  return(False); \
}
#define ThrowCompareException3(code,reason,description) \
{ \
  DestroyImageList(compare_image); \
  DestroyImageList(difference_image); \
  DestroyImageList(reference_image); \
  ThrowException3(exception,code,reason,description); \
  LiberateArgumentList(argc,argv); \
  return(False); \
}
MagickExport unsigned int 
CompareImageCommand(ImageInfo *image_info,
  int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
  const char
    *difference_filename;

  char
    *filename,
    message[MaxTextExtent],
    *option;

  double
    maximum_error=-1;

  Image
    *compare_image,
    *difference_image,
    *reference_image;

  MetricType
    metric=UndefinedMetric;

  DifferenceImageOptions
    difference_options;

  long
    x;

  register long
    i;

  unsigned int
    status;

  /*
    Set default.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  status=True;

  if (argc < 2 || ((argc < 3) && (LocaleCompare("-help",argv[1]) == 0 ||
      LocaleCompare("-?",argv[1]) == 0)))
    {
      CompareUsage();
      if (argc < 2)
	{
	  ThrowException(exception,OptionError,UsageError,NULL);
	  return MagickFail;
	}
      return MagickPass;
    }
  if (LocaleCompare("-version",argv[1]) == 0)
    {
      (void) VersionCommand(image_info,argc,argv,metadata,exception);
      return MagickPass;
    }

  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickFatalError(ResourceLimitFatalError,MemoryAllocationFailed,
    (char *) NULL);

  InitializeDifferenceImageOptions(&difference_options,exception);
  difference_image=NewImageList();
  reference_image=NewImageList();
  difference_filename=(const char *) NULL;
  filename=(char *) NULL;
  compare_image=NewImageList();
  (void) strlcpy(image_info->filename,argv[argc-1],MaxTextExtent);
  (void) SetImageInfo(image_info,SETMAGICK_WRITE,exception);

  status=True;
  /*
    Check command syntax.
  */
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) < 2) ||
	/* stdin + subexpression */
	((option[0] == '-') && (option[1] == '[')) ||
	((option[0] != '-') && option[0] != '+'))
      {
        /*
          Read input images.
        */
        filename=argv[i];
        (void) strlcpy(image_info->filename,filename,MaxTextExtent);
	DestroyExceptionInfo(exception);
	GetExceptionInfo(exception);
        if (reference_image == (Image *) NULL)
          {
            reference_image=ReadImage(image_info,exception);
            continue;
          }
        if (compare_image == (Image *) NULL)
          {
            compare_image=ReadImage(image_info,exception);
            continue;
          }
        continue;
      }
    switch(*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("authenticate",option+1) == 0)
          {
            (void) CloneString(&image_info->authenticate,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompareException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->authenticate,argv[i]);
              }
            break;
          }
        ThrowCompareException(OptionError,UnrecognizedOption,option)
      }
      case 'c':
      {
        if (LocaleCompare("colorspace",option+1) == 0)
          {
            image_info->colorspace=RGBColorspace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompareException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->colorspace=StringToColorspaceType(option);
                if (image_info->colorspace == UndefinedColorspace)
                  ThrowCompareException(OptionError,UnrecognizedColorspace,
                    option);
              }
            break;
          }
        ThrowCompareException(OptionError,UnrecognizedOption,option)
      }
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            (void) SetLogEventMask("None");
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompareException(OptionError,MissingArgument,
                    option);
                (void) SetLogEventMask(argv[i]);
              }
            break;
          }
        if (LocaleCompare("define",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowCompareException(OptionError,MissingArgument,
                option);
            if (*option == '+')
              (void) RemoveDefinitions(image_info,argv[i]);
            else
              (void) AddDefinitions(image_info,argv[i],exception);
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            (void) CloneString(&image_info->density,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompareException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->density,argv[i]);
              }
            break;
          }
        if (LocaleCompare("depth",option+1) == 0)
          {
            image_info->depth=QuantumDepth;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowCompareException(OptionError,MissingArgument,
                    option);
                image_info->depth=MagickAtoL(argv[i]);
              }
            break;
          }
        if (LocaleCompare("display",option+1) == 0)
          {
            (void) CloneString(&image_info->server_name,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompareException(OptionError,MissingArgument,
                    option);
                (void) CloneString(&image_info->server_name,argv[i]);
              }
            break;
          }
        ThrowCompareException(OptionError,UnrecognizedOption,option)
      }
      case 'e':
      {
        if (LocaleCompare("endian",option+1) == 0)
          {
            image_info->endian=UndefinedEndian;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompareException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->endian=StringToEndianType(option);
                if (image_info->endian == UndefinedEndian)
                  ThrowCompareException(OptionError,UnrecognizedEndianType,
                    option);
              }
            break;
          }
        ThrowCompareException(OptionError,UnrecognizedOption,option)
      }
    case 'f':
      {
        if (LocaleCompare("file",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompareException(OptionError,MissingArgument,
                    option);
                difference_filename=argv[i];
              }
            break;
          }
        ThrowCompareException(OptionError,UnrecognizedOption,option)
      }
      case 'h':
      {
        if (LocaleCompare("help",option+1) == 0)
          {
            CompareUsage();
	    break;
            /* ThrowCompareException(OptionError,UsageError,NULL); */
          }
        if ((LocaleCompare("highlight-color",option+1) == 0) ||
            (LocaleCompare("hilight-color",option+1) == 0))
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompareException(OptionError,MissingArgument,
                    option);
                (void) QueryColorDatabase(argv[i],&difference_options.highlight_color,
                  exception);
              }
            break;
          }
        if ((LocaleCompare("highlight-style",option+1) == 0) ||
            (LocaleCompare("hilight-style",option+1) == 0))
          {
            difference_options.highlight_style=UndefinedHighlightStyle;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompareException(OptionError,MissingArgument,option);
                option=argv[i];
                difference_options.highlight_style=StringToHighlightStyle(option);
                if (difference_options.highlight_style == UndefinedHighlightStyle)
                  ThrowCompareException(OptionError,UnrecognizedHighlightStyle,
                    option);
              }
            break;
          }
        ThrowCompareException(OptionError,UnrecognizedOption,option)
      }
      case 'i':
      {
        if (LocaleCompare("interlace",option+1) == 0)
          {
            image_info->interlace=UndefinedInterlace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompareException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->interlace=StringToInterlaceType(option);
                if (image_info->interlace == UndefinedInterlace)
                  ThrowCompareException(OptionError,UnrecognizedInterlaceType,
                    option);
              }
            break;
          }
        ThrowCompareException(OptionError,UnrecognizedOption,option)
      }
      case 'l':
      {
        if (LocaleCompare("limit",option+1) == 0)
          {
            if (*option == '-')
              {
                ResourceType
                  resource_type;

                char
                  *type;

                i++;
                if (i == argc)
                  ThrowCompareException(OptionError,MissingArgument,option);
                type=argv[i];
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowCompareException(OptionError,MissingArgument,option);
                resource_type=StringToResourceType(type);
                if (resource_type == UndefinedResource)
                  ThrowCompareException(OptionError,UnrecognizedResourceType,type);
                (void) SetMagickResourceLimit(resource_type,MagickSizeStrToInt64(argv[i],1024));
              }
            break;
          }
        if (LocaleCompare("log",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompareException(OptionError,MissingArgument,
                    option);
                (void) SetLogFormat(argv[i]);
              }
            break;
          }
        ThrowCompareException(OptionError,UnrecognizedOption,option)
      }
      case 'm':
      {
        if (LocaleCompare("matte",option+1) == 0)
	  break;
	if (LocaleCompare("maximum-error",option+1) == 0)
	  {
	    if (*option == '-')
              {
		i++;
		if (i == argc)
		  ThrowCompareException(OptionError,MissingArgument,
                                        option);
		maximum_error=MagickAtoF(argv[i]);
	      }
	    break;
	  }
        if (LocaleCompare("metric",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompareException(OptionError,MissingArgument,
                                        option);
                metric=StringToMetricType(argv[i]);
                if (metric == UndefinedMetric)
                  ThrowCompareException(OptionError,UnrecognizedMetric,
                                        option);
              }
            break;
          }
        if (LocaleCompare("monitor",option+1) == 0)
          {
            if (*option == '+')
	      {
		(void) SetMonitorHandler((MonitorHandler) NULL);
		(void) MagickSetConfirmAccessHandler((ConfirmAccessHandler) NULL);
	      }
            else
	      {
		(void) SetMonitorHandler(CommandProgressMonitor);
		(void) MagickSetConfirmAccessHandler(CommandAccessMonitor);
	      }
            break;
          }
        ThrowCompareException(OptionError,UnrecognizedOption,option)
      }
      case 's':
      {
        if (LocaleCompare("sampling-factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompareException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
                NormalizeSamplingFactor(image_info);
              }
            break;
          }
        if (LocaleCompare("size",option+1) == 0)
          {
            (void) CloneString(&image_info->size,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompareException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->size,argv[i]);
              }
            break;
          }
        ThrowCompareException(OptionError,UnrecognizedOption,option)
      }
      case 't':
      {
        if (LocaleCompare("type",option+1) == 0)
          {
            image_info->type=UndefinedType;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompareException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->type=StringToImageType(option);
                if (image_info->type == UndefinedType)
                  ThrowCompareException(OptionError,UnrecognizedImageType,
                    option);
              }
            break;
          }
        ThrowCompareException(OptionError,UnrecognizedOption,option)
      }
    case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose+=(*option == '-');
            break;
          }
        if (LocaleCompare("verbose",option+1) == 0)
          break;
        ThrowCompareException(OptionError,UnrecognizedOption,option)
      }
      case '?':
        break;
      default:
        ThrowCompareException(OptionError,UnrecognizedOption,option)
    }
  }
  if (compare_image == (Image *) NULL)
    {
      if (exception->severity == UndefinedException)
        ThrowCompareException(OptionError,RequestDidNotReturnAnImage,
          (char *) NULL);
      return(False);
    }
  if ((reference_image == (Image *) NULL) ||
      (compare_image == (Image *) NULL))
    ThrowCompareException(OptionError,MissingAnImageFilename,(char *) NULL);

  /*
    Apply any user settings to images prior to compare.
  */
  if (image_info->type != UndefinedType)
    {
      (void) SetImageType(reference_image,image_info->type);
      (void) SetImageType(compare_image,image_info->type);
    }

  if (image_info->colorspace != UndefinedColorspace)
    {
      (void) TransformColorspace(reference_image,image_info->colorspace);
      (void) TransformColorspace(compare_image,image_info->colorspace);
    }

  if (metric != UndefinedMetric)
  {
    /*
      Compute and print statistical differences based on metric.
    */
    DifferenceStatistics
      statistics;

    InitializeDifferenceStatistics(&statistics,exception);
    status&=GetImageChannelDifference(reference_image,compare_image,metric,
                                      &statistics,exception);
    fprintf(stdout,"Image Difference (%s):\n",MetricTypeToString(metric));
    if (metric == PeakSignalToNoiseRatioMetric)
      {
        fprintf(stdout, "           PSNR\n");
        fprintf(stdout, "          ======\n");
        fprintf(stdout,"     Red: %#-6.2f\n",statistics.red);
        fprintf(stdout,"   Green: %#-6.2f\n",statistics.green);
        fprintf(stdout,"    Blue: %#-6.2f\n",statistics.blue);
        if (reference_image->matte)
          fprintf(stdout," Opacity: %#-6.2f\n",statistics.opacity);
        fprintf(stdout,"   Total: %#-6.2f\n",statistics.combined);

	if ((maximum_error >= 0.0) && (statistics.combined < maximum_error))
	  {
	    status &= MagickFail;
	    FormatString(message,"%g",statistics.combined);
	    ThrowException(exception,ImageError,ImageDifferenceExceedsLimit,message);
	  }
      }
    else
      {
        fprintf(stdout, "           Normalized    Absolute\n");
        fprintf(stdout, "          ============  ==========\n");
        fprintf(stdout,"     Red: %#-12.10f % 10.1f\n",statistics.red,statistics.red*MaxRGBDouble);
        fprintf(stdout,"   Green: %#-12.10f % 10.1f\n",statistics.green,statistics.green*MaxRGBDouble);
        fprintf(stdout,"    Blue: %#-12.10f % 10.1f\n",statistics.blue,statistics.blue*MaxRGBDouble);
        if (reference_image->matte)
          fprintf(stdout," Opacity: %#-12.10f % 10.1f\n",statistics.opacity,statistics.opacity*MaxRGBDouble);
        fprintf(stdout,"   Total: %#-12.10f % 10.1f\n",statistics.combined,statistics.combined*MaxRGBDouble);

	if ((maximum_error >= 0.0) && (statistics.combined > maximum_error))
	  {
	    status &= MagickFail;
	    FormatString(message,"%g > %g",statistics.combined, maximum_error);
	    ThrowException(exception,ImageError,ImageDifferenceExceedsLimit,message);
	  }
      }
  }

  if ((difference_filename != (const char *) NULL) &&
      (difference_options.highlight_style != UndefinedHighlightStyle))
    {
      /*
        Generate an annotated difference image and write file.
      */

      difference_image=DifferenceImage(reference_image,compare_image,
                                       &difference_options,exception);
      if (difference_image != (Image *) NULL)
        {
          (void) strlcpy(difference_image->filename,difference_filename,
                         MaxTextExtent);
          if (WriteImage(image_info,difference_image) == MagickFail)
	    {
	      status &= MagickFail;
	      CopyException(exception,&difference_image->exception);
	    }
        }
    }

  DestroyImageList(difference_image);
  DestroyImageList(reference_image);
  DestroyImageList(compare_image);
  LiberateArgumentList(argc,argv);
  return(status);
}
#undef ThrowCompareException
#undef ThrowCompareException3


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p a r e U s a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CompareUsage() displays the program command syntax.
%
%  The format of the CompareUsage method is:
%
%      void CompareUsage()
%
%
*/
static void CompareUsage(void)
{
  const char
    **p;

  static const char
    *options[]=
    {
      "-authenticate value  decrypt image with this password",
      "-colorspace type     alternate image colorspace",
      "-debug events        display copious debugging information",
      "-define values       coder/decoder specific options",
      "-density geometry    horizontal and vertical density of the image",
      "-depth value         image depth",
      "-display server      get image or font from this X server",
      "-endian type         multibyte word order (LSB, MSB, or Native)",
      "-file filename       write difference image to this file",
      "-help                print program options",
      "-highlight-color color",
      "                     color to use when annotating difference pixels",
      "-highlight-style style",
      "                     pixel highlight style (assign, threshold, tint, xor)",
      "-interlace type      None, Line, Plane, or Partition",
      "-limit type value    Disk, File, Map, Memory, Pixels, Width, Height or",
      "                     Threads resource limit",
      "-log format          format of debugging information",
      "-matte               store matte channel if the image has one",
      "-maximum-error       maximum total difference before returning error",
      "-metric              comparison metric (MAE, MSE, PAE, PSNR, RMSE)",
      "-monitor             show progress indication",
      "-sampling-factor HxV[,...]",
      "                     horizontal and vertical sampling factors",
      "-size geometry       width and height of image",
      "-type type           image type",
      "-verbose             print detailed information about the image",
      "-version             print version information",
      (char *) NULL
    };

  PrintUsageHeader();
  (void) printf("Usage: %.1024s [options ...] reference [options ...] compare"
    " [options ...]\n",GetClientName());
  (void) printf("\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  C o m p o s i t e I m a g e C o m m a n d                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CompositeImageCommand() reads one or more images and an optional mask and
%  composites them into a new image.
%
%  The format of the CompositeImageCommand method is:
%
%      unsigned int CompositeImageCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int CompositeImageList(ImageInfo *image_info,Image **image,
  Image *composite_image,Image *mask_image,CompositeOptions *option_info,
  ExceptionInfo *exception)
{
  long
    x,
    y;

  unsigned int
    matte,
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image **) NULL);
  assert((*image)->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  status=True;
  if (composite_image != (Image *) NULL)
    {
      assert(composite_image->signature == MagickSignature);
      if (mask_image != (Image *) NULL)
        {
          assert(mask_image->signature == MagickSignature);
          (void) SetImageType(composite_image,TrueColorMatteType);
          if (!composite_image->matte)
            SetImageOpacity(composite_image,OpaqueOpacity);
          status&=CompositeImage(composite_image,CopyOpacityCompositeOp,
            mask_image,0,0);
          if (status == False)
            GetImageException(composite_image,exception);
        }
      if (option_info->compose == DissolveCompositeOp)
        {
          register PixelPacket
            *q;

          /*
            Create mattes for dissolve.
          */
          if (!composite_image->matte)
            SetImageOpacity(composite_image,OpaqueOpacity);
          for (y=0; y < (long) composite_image->rows; y++)
          {
            q=GetImagePixels(composite_image,0,y,composite_image->columns,1);
            if (q == (PixelPacket *) NULL)
              break;
            for (x=0; x < (long) composite_image->columns; x++)
            {
              q->opacity=(Quantum)
                ((((unsigned long) MaxRGB-q->opacity)*option_info->dissolve)/100.0);
              q++;
            }
            if (!SyncImagePixels(composite_image))
              break;
          }
        }
      if (option_info->compose == DisplaceCompositeOp)
        (void) CloneString(&composite_image->geometry,
          option_info->displace_geometry);
      if (option_info->compose == ModulateCompositeOp)
        (void) CloneString(&composite_image->geometry,
          option_info->watermark_geometry);
      if (option_info->compose == ThresholdCompositeOp)
        (void) CloneString(&composite_image->geometry,
          option_info->unsharp_geometry);
      /*
        Composite image.
      */
      matte=(*image)->matte;
      if (option_info->stegano != 0)
        {
          Image
            *stegano_image;

          (*image)->offset=option_info->stegano-1;
          stegano_image=SteganoImage(*image,composite_image,exception);
          if (stegano_image != (Image *) NULL)
            {
              DestroyImageList(*image);
              *image=stegano_image;
            }
        }
      else
        if (option_info->stereo)
          {
            Image
              *stereo_image;

            stereo_image=StereoImage(*image,composite_image,exception);
            if (stereo_image != (Image *) NULL)
              {
                DestroyImageList(*image);
                *image=stereo_image;
              }
          }
        else
          if (option_info->tile)
            {
              /*
                Tile the composite image.
              */
              for (y=0; y < (long) (*image)->rows; y+=composite_image->rows)
                for (x=0; x < (long) (*image)->columns; x+=composite_image->columns)
                {
                  status&=CompositeImage(*image,option_info->compose,
                    composite_image,x,y);
                  GetImageException(*image,exception);
                }
            }
          else
            {
              char
                composite_geometry[MaxTextExtent];

              RectangleInfo
                geometry;

              /*
                Digitally composite image.
              */
              geometry.x=0;
              geometry.y=0;
              (void) GetGeometry(option_info->geometry,&geometry.x,&geometry.y,
                &geometry.width,&geometry.height);
              FormatString(composite_geometry,"%lux%lu%+ld%+ld",
                composite_image->columns,composite_image->rows,geometry.x,
                geometry.y);
              (*image)->gravity=option_info->gravity;
              (void) GetImageGeometry(*image,composite_geometry,False,
                &geometry);
              status&=CompositeImage(*image,option_info->compose,
                composite_image,geometry.x,geometry.y);
              GetImageException(*image,exception);
            }
      if (option_info->compose != CopyOpacityCompositeOp)
        (*image)->matte=matte;
    }
  return(status);
}

static void LiberateCompositeOptions(CompositeOptions *option_info)
{
  MagickFreeMemory(option_info->displace_geometry);
  MagickFreeMemory(option_info->geometry);
  MagickFreeMemory(option_info->unsharp_geometry);
  MagickFreeMemory(option_info->watermark_geometry);
}

#define NotInitialized  (unsigned int) (~0)
#define ThrowCompositeException(code,reason,description) \
{ \
  LiberateCompositeOptions(&option_info); \
  DestroyImageList(image); \
  DestroyImageList(composite_image); \
  DestroyImageList(mask_image); \
  ThrowException(exception,code,reason,description); \
  LiberateArgumentList(argc,argv); \
  return(False); \
}
#define ThrowCompositeException3(code,reason,description) \
{ \
  LiberateCompositeOptions(&option_info); \
  DestroyImageList(image); \
  DestroyImageList(composite_image); \
  DestroyImageList(mask_image); \
  ThrowException3(exception,code,reason,description); \
  LiberateArgumentList(argc,argv); \
  return(False); \
}
MagickExport unsigned int CompositeImageCommand(ImageInfo *image_info,
  int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
  char
    *filename,
    *format,
    *option;

  CompositeOptions
    option_info;

  Image
    *composite_image,
    *image,
    *mask_image;

  long
    j,
    x;

  register long
    i;

  unsigned int
    status;

  /*
    Set default.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  status=MagickPass;

  if (argc < 2 || ((argc < 3) && (LocaleCompare("-help",argv[1]) == 0 ||
      LocaleCompare("-?",argv[1]) == 0)))
    {
      CompositeUsage();
      if (argc < 2)
	{
	  ThrowException(exception,OptionError,UsageError,NULL);
	  return MagickFail;
	}
      return MagickPass;
    }
  if (LocaleCompare("-version",argv[1]) == 0)
    {
      (void) VersionCommand(image_info,argc,argv,metadata,exception);
      return MagickPass;
    }

  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickFatalError(ResourceLimitFatalError,MemoryAllocationFailed,
    (char *) NULL);

  (void) memset(&option_info,0,sizeof(CompositeOptions));
  option_info.dissolve=0.0;
  option_info.compose=OverCompositeOp;
  composite_image=NewImageList();
  option_info.displace_geometry=(char *) NULL;
  filename=(char *) NULL;
  format=(char *) NULL;
  option_info.geometry=(char *) NULL;
  image=NewImageList();
  (void) strlcpy(image_info->filename,argv[argc-1],MaxTextExtent);
  (void) SetImageInfo(image_info,SETMAGICK_WRITE,exception);
  mask_image=NewImageList();
  option_info.stegano=0;
  option_info.stereo=False;
  option_info.tile=False;
  option_info.watermark_geometry=(char *) NULL;
  option_info.unsharp_geometry=(char *) NULL;
  status=True;
  /*
    Check command syntax.
  */
  j=1;
  for (i=1; i < (argc-1); i++)
  {
    option=argv[i];
    if ((strlen(option) < 2) ||
	/* stdin + subexpression */
	((option[0] == '-') && (option[1] == '[')) ||
	((option[0] != '-') && option[0] != '+'))
      {
        /*
          Read input images.
        */
        filename=argv[i];
        (void) strlcpy(image_info->filename,filename,MaxTextExtent);
        if (composite_image == (Image *) NULL)
          {
            composite_image=ReadImage(image_info,exception);
            if (composite_image != (Image *) NULL)
              {
                status&=MogrifyImages(image_info,i-j,argv+j,&composite_image);
                GetImageException(composite_image,exception);
              }
            j=i+1;
            continue;
          }
        if (mask_image != (Image *) NULL)
          ThrowCompositeException(OptionError,InputImagesAlreadySpecified,
            filename);
        if (image == (Image *) NULL)
          {
            image=ReadImage(image_info,exception);
            if (image != (Image *) NULL)
              {
                status&=MogrifyImages(image_info,i-j,argv+j,&image);
                GetImageException(image,exception);
              }
            j=i+1;
            continue;
          }
        mask_image=ReadImage(image_info,exception);
        status&=mask_image != (Image *) NULL;
        if (mask_image != (Image *) NULL)
          {
            status&=MogrifyImages(image_info,i-j,argv+j,&mask_image);
            GetImageException(mask_image,exception);
          }
        j=i+1;
        continue;
      }
    switch(*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("affine",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("authenticate",option+1) == 0)
          {
            (void) CloneString(&image_info->authenticate,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->authenticate,argv[i]);
              }
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'b':
      {
        if (LocaleCompare("background",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->background_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("blue-primary",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'c':
      {
        if (LocaleCompare("colors",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowCompositeException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("colorspace",option+1) == 0)
          {
            image_info->colorspace=RGBColorspace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->colorspace=StringToColorspaceType(option);
                if (image_info->colorspace == UndefinedColorspace)
                  ThrowCompositeException(OptionError,UnrecognizedColorspace,
                    option);
              }
            break;
          }
        if (LocaleCompare("comment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("compose",option+1) == 0)
          {
            option_info.compose=CopyCompositeOp;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
                option=argv[i];
                option_info.compose=StringToCompositeOperator(option);
                if (option_info.compose == UndefinedCompositeOp)
                  ThrowCompositeException(OptionError,UnrecognizedComposeOperator,
                    option);
              }
            break;
          }
        if (LocaleCompare("compress",option+1) == 0)
          {
            image_info->compression=NoCompression;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->compression=StringToCompressionType(option);
                if (image_info->compression == UndefinedCompression)
                  ThrowCompositeException(OptionError,UnrecognizedImageCompression,
                    option);
              }
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            (void) SetLogEventMask("None");
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
                (void) SetLogEventMask(argv[i]);
              }
            break;
          }
        if (LocaleCompare("define",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowCompositeException(OptionError,MissingArgument,
                option);
            if (*option == '+')
              (void) RemoveDefinitions(image_info,argv[i]);
            else
              (void) AddDefinitions(image_info,argv[i],exception);
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            (void) CloneString(&image_info->density,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->density,argv[i]);
              }
            break;
          }
        if (LocaleCompare("depth",option+1) == 0)
          {
            image_info->depth=QuantumDepth;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
                image_info->depth=MagickAtoL(argv[i]);
              }
            break;
          }
        if (LocaleCompare("displace",option+1) == 0)
          {
            (void) CloneString(&option_info.displace_geometry,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,MissingArgument,option);
                (void) CloneString(&option_info.displace_geometry,argv[i]);
                option_info.compose=DisplaceCompositeOp;
              }
            break;
          }
        if (LocaleCompare("display",option+1) == 0)
          {
            (void) CloneString(&image_info->server_name,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
                (void) CloneString(&image_info->server_name,argv[i]);
              }
            break;
          }
        if (LocaleCompare("dispose",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
                option=argv[i];
                if ((LocaleCompare("0",option) != 0) &&
                    (LocaleCompare("1",option) != 0) &&
                    (LocaleCompare("2",option) != 0) &&
                    (LocaleCompare("3",option) != 0) &&
                    (LocaleCompare("Undefined",option) != 0) &&
                    (LocaleCompare("None",option) != 0) &&
                    (LocaleCompare("Background",option) != 0) &&
                    (LocaleCompare("Previous",option) != 0))
                  ThrowCompositeException(OptionError,UnrecognizedDisposeMethod,
                    option);
              }
            break;
          }
        if (LocaleCompare("dissolve",option+1) == 0)
          {
            option_info.dissolve=0.0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
                option_info.dissolve=MagickAtoF(argv[i]);
                option_info.compose=DissolveCompositeOp;
              }
            break;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            image_info->dither=(*option == '-');
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'e':
      {
        if (LocaleCompare("encoding",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("endian",option+1) == 0)
          {
            image_info->endian=UndefinedEndian;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->endian=StringToEndianType(option);
                if (image_info->endian == UndefinedEndian)
                  ThrowCompositeException(OptionError,UnrecognizedEndianType,
                    option);
              }
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'f':
      {
        if (LocaleCompare("filter",option+1) == 0)
          {
            if (*option == '-')
              {
                FilterTypes
                  filter;

                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
                option=argv[i];
                filter=StringToFilterTypes(option);
                if (filter == UndefinedFilter)
                  ThrowCompositeException(OptionError,UnrecognizedImageFilter,
                    option);
              }
            break;
          }
        if (LocaleCompare("font",option+1) == 0)
          {
            (void) CloneString(&image_info->font,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->font,argv[i]);
              }
            break;
          }
        if (LocaleCompare("format",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
                format=argv[i];
              }
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'g':
      {
        if (LocaleCompare("geometry",option+1) == 0)
          {
            (void) CloneString(&option_info.geometry,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,MissingArgument,option);
                (void) CloneString(&option_info.geometry,argv[i]);
              }
            break;
          }
        if (LocaleCompare("gravity",option+1) == 0)
          {
            option_info.gravity=ForgetGravity;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
                option=argv[i];
                option_info.gravity=StringToGravityType(option);
                if (option_info.gravity == ForgetGravity)
                  ThrowCompositeException(OptionError,UnrecognizedGravityType,
                    option);
              }
            break;
          }
        if (LocaleCompare("green-primary",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'h':
      {
        if (LocaleCompare("help",option+1) == 0)
          {
            CompositeUsage();
	    break;
            /* ThrowCompositeException(OptionError,UsageError,NULL); */
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'i':
      {
        if (LocaleCompare("interlace",option+1) == 0)
          {
            image_info->interlace=UndefinedInterlace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->interlace=StringToInterlaceType(option);
                if (image_info->interlace == UndefinedInterlace)
                  ThrowCompositeException(OptionError,UnrecognizedInterlaceType,
                    option);
              }
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'l':
      {
        if (LocaleCompare("label",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("limit",option+1) == 0)
          {
            if (*option == '-')
              {
                ResourceType
                  resource_type;

                char
                  *type;

                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
                type=argv[i];
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowCompositeException(OptionError,MissingArgument,option);
                resource_type=StringToResourceType(type);
                if (resource_type == UndefinedResource)
                  ThrowCompositeException(OptionError,UnrecognizedResourceType,type);
                (void) SetMagickResourceLimit(resource_type,MagickSizeStrToInt64(argv[i],1024));
              }
            break;
          }
        if (LocaleCompare("log",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
                (void) SetLogFormat(argv[i]);
              }
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'm':
      {
        if (LocaleCompare("matte",option+1) == 0)
          break;
        if (LocaleCompare("monitor",option+1) == 0)
          {
            if (*option == '+')
	      {
		(void) SetMonitorHandler((MonitorHandler) NULL);
		(void) MagickSetConfirmAccessHandler((ConfirmAccessHandler) NULL);
	      }
            else
	      {
		(void) SetMonitorHandler(CommandProgressMonitor);
		(void) MagickSetConfirmAccessHandler(CommandAccessMonitor);
	      }
            break;
          }
        if (LocaleCompare("monochrome",option+1) == 0)
          break;
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'n':
      {
        if (LocaleCompare("negate",option+1) == 0)
          break;
        if (LocaleCompare("noop",option+1) == 0)
          {
            status&=CompositeImageList(image_info,&image,composite_image,
              mask_image,&option_info,exception);
            if (composite_image != (Image *) NULL)
              {
                DestroyImageList(composite_image);
                composite_image=NewImageList();
              }
            if (mask_image != (Image *) NULL)
              {
                DestroyImageList(mask_image);
                mask_image=NewImageList();
              }
            GetImageException(image,exception);
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'p':
      {
        if (LocaleCompare("page",option+1) == 0)
          {
            (void) CloneString(&image_info->page,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
                image_info->page=GetPageGeometry(argv[i]);
              }
            break;
          }
        if (LocaleCompare("process",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("profile",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowCompositeException(OptionError,MissingArgument,option);
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'q':
      {
        if (LocaleCompare("quality",option+1) == 0)
          {
            image_info->quality=DefaultCompressionQuality;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,MissingArgument,option);
                image_info->quality=MagickAtoL(argv[i]);
              }
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'r':
      {
        if (LocaleCompare("recolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("red-primary",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("render",option+1) == 0)
          break;
        if (LocaleCompare("repage",option+1) == 0)
          {
	    if (*option == '-')
	      {
		i++;
		if ((i == argc) || !IsGeometry(argv[i]))
		  ThrowCompositeException(OptionError,MissingArgument,option);
	      }
            break;
          }
        if (LocaleCompare("resize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("rotate",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowCompositeException(OptionError,MissingArgument,
                option);
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 's':
      {
        if (LocaleCompare("sampling-factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
                NormalizeSamplingFactor(image_info);
              }
            break;
          }
        if (LocaleCompare("scene",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("set",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowCompositeException(OptionError,MissingArgument,option);
            if (*option == '-')
              {
                /* -set attribute value */
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("sharpen",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowCompositeException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("size",option+1) == 0)
          {
            (void) CloneString(&image_info->size,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->size,argv[i]);
              }
            break;
          }
        if (LocaleCompare("stegano",option+1) == 0)
          {
            option_info.stegano=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowCompositeException(OptionError,MissingArgument,option);
                option_info.stegano=MagickAtoL(argv[i])+1;
              }
            break;
          }
        if (LocaleCompare("stereo",option+1) == 0)
          {
            option_info.stereo=(*option == '-');
            break;
          }
        if (LocaleCompare("strip",option+1) == 0)
	  {
	    break;
	  }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 't':
      {
        if (LocaleCompare("thumbnail",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("tile",option+1) == 0)
          {
            option_info.tile=(*option == '-');
            break;
          }
        if (LocaleCompare("transform",option+1) == 0)
          break;
        if (LocaleCompare("treedepth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowCompositeException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("type",option+1) == 0)
          {
            image_info->type=UndefinedType;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->type=StringToImageType(option);
                if (image_info->type == UndefinedType)
                  ThrowCompositeException(OptionError,UnrecognizedImageType,
                    option);
              }
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'u':
      {
        if (LocaleCompare("units",option+1) == 0)
          {
            image_info->units=UndefinedResolution;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->units=UndefinedResolution;
                if (LocaleCompare("PixelsPerInch",option) == 0)
                  image_info->units=PixelsPerInchResolution;
                if (LocaleCompare("PixelsPerCentimeter",option) == 0)
                  image_info->units=PixelsPerCentimeterResolution;
              }
            break;
          }
        if (LocaleCompare("unsharp",option+1) == 0)
          {
            (void) CloneString(&option_info.unsharp_geometry,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,MissingArgument,option);
                (void) CloneString(&option_info.unsharp_geometry,argv[i]);
                option_info.compose=ThresholdCompositeOp;
              }
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose+=(*option == '-');
            break;
          }
        if (LocaleCompare("verbose",option+1) == 0)
          break;
        if (LocaleCompare("virtual-pixel",option+1) == 0)
          {
            if (*option == '-')
              {
                VirtualPixelMethod
                  virtual_pixel_method;

                i++;
                if (i == argc)
                  ThrowCompositeException(OptionError,MissingArgument,option);
                option=argv[i];
                virtual_pixel_method=StringToVirtualPixelMethod(option);
                if (virtual_pixel_method == UndefinedVirtualPixelMethod)
                  ThrowCompositeException(OptionError,UnrecognizedVirtualPixelMethod,option);
              }
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case 'w':
      {
        if (LocaleCompare("watermark",option+1) == 0)
          {
            (void) CloneString(&option_info.watermark_geometry,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,MissingArgument,option);
                (void) CloneString(&option_info.watermark_geometry,argv[i]);
                option_info.compose=ModulateCompositeOp;
              }
            break;
          }
        if (LocaleCompare("white-point",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowCompositeException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("write",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowCompositeException(OptionError,MissingArgument,option);
            break;
          }
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
      }
      case '?':
        break;
      default:
        ThrowCompositeException(OptionError,UnrecognizedOption,option)
    }
  }
  if (image == (Image *) NULL)
    {
      if (exception->severity == UndefinedException)
        ThrowCompositeException(OptionError,RequestDidNotReturnAnImage,
          (char *) NULL);
      return(False);
    }
  if (i != (argc-1))
    ThrowCompositeException(OptionError,MissingAnImageFilename,(char *) NULL);
  status&=MogrifyImages(image_info,i-j,argv+j,&image);
  GetImageException(image,exception);
  status&=CompositeImageList(image_info,&image,composite_image,mask_image,
    &option_info,exception);
  /*
    Write composite images.
  */
  status&=WriteImages(image_info,image,argv[argc-1],exception);
  if (metadata != (char **) NULL)
    {
      char
        *text;

      text=TranslateText(image_info,image,(format != (char *) NULL) ? format : "%w,%h,%m");
      if (text == (char *) NULL)
        ThrowCompositeException(ResourceLimitError,MemoryAllocationFailed,
          MagickMsg(OptionError,UnableToFormatImageMetadata));
      (void) ConcatenateString(&(*metadata),text);
      (void) ConcatenateString(&(*metadata),"\n");
      MagickFreeMemory(text);
    }
  LiberateCompositeOptions(&option_info);
  DestroyImageList(composite_image);
  DestroyImageList(mask_image);
  DestroyImageList(image);
  LiberateArgumentList(argc,argv);
  return(status);
}
#undef ThrowCompositeException
#undef ThrowCompositeException3

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o m p o s i t e U s a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  CompositeUsage() displays the program command syntax.
%
%  The format of the CompositeUsage method is:
%
%      void CompositeUsage()
%
%
*/
static void CompositeUsage(void)
{
  const char
    **p;

  static const char
    *options[]=
    {
      "-affine matrix       affine transform matrix",
      "-authenticate value  decrypt image with this password",
      "-blue-primary point  chomaticity blue primary point",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     alternate image colorspace",
      "-comment string      annotate image with comment",
      "-compose operator    composite operator",
      "-compress type       image compression type",
      "-debug events        display copious debugging information",
      "-define values       Coder/decoder specific options",
      "-density geometry    horizontal and vertical density of the image",
      "-depth value         image depth",
      "-displace geometry   shift image pixels defined by a displacement map",
      "-display server      get image or font from this X server",
      "-dispose method      Undefined, None, Background, Previous",
      "-dissolve value      dissolve the two images a given percent",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-encoding type       text encoding type",
      "-endian type         multibyte word order (LSB, MSB, or Native)",
      "-filter type         use this filter when resizing an image",
      "-font name           render text with this font",
      "-geometry geometry   location of the composite image",
      "-gravity type        which direction to gravitate towards",
      "-green-primary point chomaticity green primary point",
      "-help                print program options",
      "-interlace type      None, Line, Plane, or Partition",
      "-label name          ssign a label to an image",
      "-limit type value    Disk, File, Map, Memory, Pixels, Width, Height or",
      "                     Threads resource limit",
      "-log format          format of debugging information",
      "-matte               store matte channel if the image has one",
      "-monitor             show progress indication",
      "-monochrome          transform image to black and white",
      "-negate              replace every pixel with its complementary color ",
      "+page                reset current page offsets to default",
      "-page geometry       size and location of an image canvas",
      "-profile filename    add ICM or IPTC information profile to image",
      "-quality value       JPEG/MIFF/PNG compression level",
      "-recolor matrix      apply a color translation matrix to image channels",
      "-red-primary point   chomaticity red primary point",
      "-rotate degrees      apply Paeth rotation to the image",
      "+repage              reset current page offsets to default",
      "-repage geometry     adjust current page offsets by geometry",
      "-resize geometry     resize the image",
      "-sampling-factor HxV[,...]",
      "                     horizontal and vertical sampling factors",
      "-scene value         image scene number",
      "-set attribute value set image attribute",
      "+set attribute       unset image attribute",
      "-sharpen geometry    sharpen the image",
      "-size geometry       width and height of image",
      "-stegano offset      hide watermark within an image",
      "-stereo              combine two image to create a stereo anaglyph",
      "-strip               strip all profiles and text attributes from image",
      "-thumbnail geometry  resize the image (optimized for thumbnails)",
      "-tile                repeat composite operation across image",
      "-transform           affine transform image",
      "-treedepth value     color tree depth",
      "-type type           image type",
      "-units type          PixelsPerInch, PixelsPerCentimeter, or Undefined",
      "-unsharp geometry    sharpen the image",
      "-verbose             print detailed information about the image",
      "-version             print version information",
      "-virtual-pixel method",
      "                     Constant, Edge, Mirror, or Tile",
      "-watermark geometry  percent brightness and saturation of a watermark",
      "-white-point point   chomaticity white point",
      "-write filename      write image to this file",
      (char *) NULL
    };

  PrintUsageHeader();
  (void) printf("Usage: %.1024s [options ...] image [options ...] composite\n"
    "  [ [options ...] mask ] [options ...] composite\n",GetClientName());
  (void) printf("\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n v e r t I m a g e C o m m a n d                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConvertImageCommand() reads one or more images, applies one or more image
%  processing operations, and writes out the image in the same or differing
%  format.
%
%  The format of the ConvertImageCommand method is:
%
%      unsigned int ConvertImageCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static unsigned int ConcatenateImages(const int argc,char **argv,
  ExceptionInfo *exception)
{
  FILE
    *input,
    *output;

  int
    c;

  register long
    i;

  /*
    Open output file.
  */
  output=fopen(argv[argc-1],"wb");
  if (output == (FILE *) NULL)
    {
      ThrowException(exception,FileOpenError,UnableToOpenFile,argv[argc-1]);
      return(False);
    }
  for (i=2; i < (argc-1); i++)
  {
    input=fopen(argv[i],"rb");
    if (input == (FILE *) NULL)
      {
        ThrowException(exception,FileOpenError,UnableToOpenFile,argv[i]);
        continue;
      }
    for (c=fgetc(input); c != EOF; c=fgetc(input))
      (void) fputc((char) c,output);
    (void) fclose(input);
    (void) remove(argv[i]);
  }
  (void) fclose(output);
  return(True);
}

#define NotInitialized  (unsigned int) (~0)

#define ThrowConvertException(code,reason,description) \
{ \
  status = MagickFail; \
  ThrowException(exception,code,reason,description); \
  goto convert_cleanup_and_return; \
}
#define ThrowConvertException3(code,reason,description) \
{ \
  status = MagickFail; \
  ThrowException3(exception,code,reason,description); \
  goto convert_cleanup_and_return; \
}

MagickExport unsigned int ConvertImageCommand(ImageInfo *image_info,
  int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
  char
    *filename,
    *format,
    *option;

  double
    sans;

  Image
    *image = (Image *) NULL,
    *image_list = (Image *) NULL,
    *next_image = (Image *) NULL;

  long
    j,
    k,
    x;

  register int
    i;

  unsigned int
    ping,
    status = 0;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);

  if (argc < 2 || ((argc < 3) && (LocaleCompare("-help",argv[1]) == 0 ||
      LocaleCompare("-?",argv[1]) == 0)))
    {
      ConvertUsage();
      if (argc < 2)
	{
	  ThrowException(exception,OptionError,UsageError,NULL);
	  return MagickFail;
	}
      return MagickPass;
    }
  if (LocaleCompare("-version",argv[1]) == 0)
    {
      (void) VersionCommand(image_info,argc,argv,metadata,exception);
      return MagickPass;
    }

  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickFatalError(ResourceLimitFatalError,MemoryAllocationFailed,
      (char *) NULL);

  /*
    Set defaults.
  */
  filename=(char *) NULL;
  format=(char *) NULL;
  image=NewImageList();
  image_list=(Image *) NULL;
  (void) strlcpy(image_info->filename,argv[argc-1],MaxTextExtent);
  (void) SetImageInfo(image_info,SETMAGICK_WRITE,exception);
  ping=False;
  option=(char *) NULL;
  status=True;
  /*
    Parse command-line arguments.
  */
  if ((argc > 2) && (LocaleCompare("-concatenate",argv[1]) == 0))
    return(ConcatenateImages(argc,argv,exception));
  j=1;
  k=0;
  for (i=1; i < (argc-1); i++)
  {
    option=argv[i];
    if ((strlen(option) < 2) ||
	/* stdin + subexpression */
	((option[0] == '-') && (option[1] == '[')) ||
	((option[0] != '-') && option[0] != '+'))
      {
        /*
          Read input image.
        */
        k=i;
        filename=argv[i];
        (void) strlcpy(image_info->filename,filename,MaxTextExtent);
        if (ping)
          next_image=PingImage(image_info,exception);
        else
          next_image=ReadImage(image_info,exception);
        status&=(next_image != (Image *) NULL) &&
          (exception->severity < ErrorException);
        if (next_image == (Image *) NULL)
          continue;
        if (image == (Image *) NULL)
          {
            image=next_image;
            continue;
          }
        AppendImageToList(&image,next_image);
        continue;
      }
    if ((image != (Image *) NULL) && (j != (k+1)))
      {
        status&=MogrifyImages(image_info,i-j,argv+j,&image);
        GetImageException(image,exception);
        AppendImageToList(&image_list,image);
        image=NewImageList();
        j=k+1;
      }
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("adjoin",option+1) == 0)
          {
            image_info->adjoin=(*option == '-');
            break;
          }
        if (LocaleCompare("affine",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("antialias",option+1) == 0)
          {
            image_info->antialias=(*option == '-');
            break;
          }
        if (LocaleCompare("append",option+1) == 0)
	  {
	    break;
	  }
	if (LocaleCompare("asc-cdl",option+1) == 0)
          {
	    if (*option == '-')
              {
		i++;
		if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
		  ThrowConvertException(OptionError,MissingArgument,option);
              }
	    break;
          }
        if (LocaleCompare("authenticate",option+1) == 0)
          {
            (void) CloneString(&image_info->authenticate,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->authenticate,argv[i]);
              }
            break;
          }
        if (LocaleCompare("auto-orient",option+1) == 0)
          break;
        if (LocaleCompare("average",option+1) == 0)
          break;
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'b':
      {
        if (LocaleCompare("background",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->background_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("black-threshold",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("blue-primary",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("blur",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowConvertException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("border",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->border_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("box",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'c':
      {
        if (LocaleCompare("channel",option+1) == 0)
          {
            if (*option == '-')
              {
                ChannelType
                  channel;

                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                channel=StringToChannelType(argv[i]);
                if (channel == UndefinedChannel)
                  ThrowConvertException(OptionError,UnrecognizedChannelType,
                    option);
              }
            break;
          }
        if (LocaleCompare("charcoal",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("chop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("clip",option+1) == 0)
          break;
        if (LocaleCompare("clippath",option+1) == 0)
         {
            i++;
            if (i == argc)
              ThrowConvertException(OptionError,MissingArgument,
                option);
            break;
          }
        if (LocaleCompare("coalesce",option+1) == 0)
          break;
        if (LocaleCompare("colorize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("compose",option+1) == 0)
          {
	    if (*option == '-')
              {
		i++;
		if (i == argc)
		  ThrowConvertException(OptionError,MissingArgument,option);
		option=argv[i];
		if ((StringToCompositeOperator(option)) == UndefinedCompositeOp)
		  ThrowConvertException(OptionError,UnrecognizedComposeOperator,
					option);
	      }
            break;
          }
        if (LocaleCompare("colors",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("colorspace",option+1) == 0)
          {
            if ((*option == '-') || (*option == '+'))
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->colorspace=StringToColorspaceType(option);
                if (image_info->colorspace == UndefinedColorspace)
                  ThrowConvertException(OptionError,UnrecognizedColorspace,option);
              }
            break;
          }
        if (LocaleCompare("comment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("compress",option+1) == 0)
          {
            image_info->compression=NoCompression;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->compression=StringToCompressionType(option);
                if (image_info->compression == UndefinedCompression)
                  ThrowConvertException(OptionError,UnrecognizedImageCompression,
                    option);
              }
            break;
          }
        if (LocaleCompare("contrast",option+1) == 0)
          break;
        if (LocaleCompare("convolve",option+1) == 0)
          {
           if (*option == '-')
             {
               i++;
               if (i == (argc-1))
                 ThrowConvertException(OptionError,MissingArgument,
                   option);
             }
           break;
         }
        if (LocaleCompare("crop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("cycle",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'd':
      {
        if (LocaleCompare("deconstruct",option+1) == 0)
          break;
        if (LocaleCompare("debug",option+1) == 0)
          {
            (void) SetLogEventMask("None");
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                (void) SetLogEventMask(argv[i]);
              }
            break;
          }
        if (LocaleCompare("define",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowConvertException(OptionError,MissingArgument,option);
            if (*option == '+')
              (void) RemoveDefinitions(image_info,argv[i]);
            else
              (void) AddDefinitions(image_info,argv[i],exception);
            break;
          }
        if (LocaleCompare("delay",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            (void) CloneString(&image_info->density,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->density,argv[i]);
              }
            break;
          }
        if (LocaleCompare("depth",option+1) == 0)
          {
            image_info->depth=QuantumDepth;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
                image_info->depth=MagickAtoL(argv[i]);
              }
            break;
          }
        if (LocaleCompare("despeckle",option+1) == 0)
          break;
        if (LocaleCompare("display",option+1) == 0)
          {
            (void) CloneString(&image_info->server_name,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->server_name,argv[i]);
              }
            break;
          }
        if (LocaleCompare("dispose",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
                option=argv[i];
                if ((LocaleCompare("0",option) != 0) &&
                    (LocaleCompare("1",option) != 0) &&
                    (LocaleCompare("2",option) != 0) &&
                    (LocaleCompare("3",option) != 0) &&
                    (LocaleCompare("Undefined",option) != 0) &&
                    (LocaleCompare("None",option) != 0) &&
                    (LocaleCompare("Background",option) != 0) &&
                    (LocaleCompare("Previous",option) != 0))
                  ThrowConvertException(OptionError,UnrecognizedDisposeMethod,
                    option);
              }
            break;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            image_info->dither=(*option == '-');
            break;
          }
        if (LocaleCompare("draw",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'e':
      {
        if (LocaleCompare("edge",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("emboss",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("encoding",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("endian",option+1) == 0)
          {
            image_info->endian=UndefinedEndian;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->endian=StringToEndianType(option);
                if (image_info->endian == UndefinedEndian)
                  ThrowConvertException(OptionError,UnrecognizedEndianType,
                    option);
              }
            break;
          }
        if (LocaleCompare("enhance",option+1) == 0)
          break;
        if (LocaleCompare("equalize",option+1) == 0)
          break;
        if (LocaleCompare("extent",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'f':
      {
        if (LocaleCompare("fill",option+1) == 0)
          {
            (void) QueryColorDatabase("none",&image_info->pen,exception);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                (void) QueryColorDatabase(argv[i],&image_info->pen,exception);
              }
            break;
          }
        if (LocaleCompare("filter",option+1) == 0)
          {
            if (*option == '-')
              {
                FilterTypes
                  filter;

                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                option=argv[i];
                filter=StringToFilterTypes(option);
                if (filter == UndefinedFilter)
                  ThrowConvertException(OptionError,UnrecognizedImageFilter,
                    option);
              }
            break;
          }
        if (LocaleCompare("flatten",option+1) == 0)
          break;
        if (LocaleCompare("flip",option+1) == 0)
          break;
        if (LocaleCompare("flop",option+1) == 0)
          break;
        if (LocaleCompare("font",option+1) == 0)
          {
            (void) CloneString(&image_info->font,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->font,argv[i]);
              }
            break;
          }
        if (LocaleCompare("format",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
                format=argv[i];
		/*
		  Add definition to defines for use by 'info' coder.
		*/
		(void) AddDefinition(image_info,"info","format",format,exception);
              }
            break;
          }
        if (LocaleCompare("frame",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("fuzz",option+1) == 0)
          {
            image_info->fuzz=0.0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
                image_info->fuzz=StringToDouble(argv[i],MaxRGB);
              }
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'g':
      {
        if (LocaleCompare("gamma",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowConvertException(OptionError,MissingArgument,option);
            break;
          }
        if ((LocaleCompare("gaussian",option+1) == 0) ||
            (LocaleCompare("gaussian-blur",option+1) == 0))
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowConvertException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("geometry",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("gravity",option+1) == 0)
          {
            GravityType
              gravity;

            gravity=ForgetGravity;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                option=argv[i];
		gravity=StringToGravityType(option);
                if (gravity == ForgetGravity)
                  ThrowConvertException(OptionError,UnrecognizedGravityType,
                    option);
              }
            break;
          }
        if (LocaleCompare("green-primary",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'h':
      {
	if (LocaleCompare("hald-clut",option+1) == 0)
          {
	    if (*option == '-')
              {
		i++;
		if (i == argc)
		  ThrowConvertException(OptionError,MissingArgument,option);
              }
	    break;
          }
        if (LocaleCompare("help",option+1) == 0)
          {
            ConvertUsage();
	    break;
            /* ThrowConvertException(OptionError,UsageError,NULL); */
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'i':
      {
        if (LocaleCompare("implode",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("intent",option+1) == 0)
          {
            if (*option == '-')
              {
                RenderingIntent
                  rendering_intent;

                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                option=argv[i];
                rendering_intent=UndefinedIntent;
                if (LocaleCompare("Absolute",option) == 0)
                  rendering_intent=AbsoluteIntent;
                if (LocaleCompare("Perceptual",option) == 0)
                  rendering_intent=PerceptualIntent;
                if (LocaleCompare("Relative",option) == 0)
                  rendering_intent=RelativeIntent;
                if (LocaleCompare("Saturation",option) == 0)
                  rendering_intent=SaturationIntent;
                if (rendering_intent == UndefinedIntent)
                  ThrowConvertException(OptionError,UnrecognizedIntentType,
                    option);
              }
            break;
          }
        if (LocaleCompare("interlace",option+1) == 0)
          {
            image_info->interlace=UndefinedInterlace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->interlace=StringToInterlaceType(option);
                if (image_info->interlace == UndefinedInterlace)
                  ThrowConvertException(OptionError,UnrecognizedInterlaceType,
                    option);
              }
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'l':
      {
        if (LocaleCompare("label",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("lat",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("level",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
              ThrowConvertException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("linewidth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("limit",option+1) == 0)
          {
            if (*option == '-')
              {
                ResourceType
                  resource_type;

                char
                  *type;

                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                type=argv[i];
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
                resource_type=StringToResourceType(type);
                if (resource_type == UndefinedResource)
                  ThrowConvertException(OptionError,UnrecognizedResourceType,type);
                (void) SetMagickResourceLimit(resource_type,MagickSizeStrToInt64(argv[i],1024));
              }
            break;
          }
        if (LocaleCompare("list",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                option=argv[i];
                switch (*option)
                {
                  case 'C':
                  case 'c':
                  {
                    if ((LocaleCompare("Color",option) == 0) ||
                        (LocaleCompare("Colors",option) == 0))
                      {
                        (void) ListColorInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowConvertException(OptionError,UnrecognizedListType,
                      option)
                  }
                  case 'D':
                  case 'd':
                  {
                    if ((LocaleCompare("Delegate",option) == 0) ||
                        (LocaleCompare("Delegates",option) == 0))
                      {
                        (void) ListDelegateInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowConvertException(OptionError,UnrecognizedListType,
                      option)
                  }
                  case 'F':
                  case 'f':
                  {
                    if ((LocaleCompare("Font",option) == 0) ||
                        (LocaleCompare("Fonts",option) == 0))
                      {
                        (void) ListTypeInfo((FILE *) NULL,exception);
                        break;
                      }
                    if ((LocaleCompare("Format",option) == 0) ||
                        (LocaleCompare("Formats",option) == 0))
                      {
                        (void) ListMagickInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowConvertException(OptionError,UnrecognizedListType,
                      option)
                  }
                  case 'M':
                  case 'm':
                  {
                    if (LocaleCompare("Magic",option) == 0)
                      {
                        (void) ListMagicInfo((FILE *) NULL,exception);
                        break;
                      }
#if defined(SupportMagickModules)
                    if ((LocaleCompare("Module",option) == 0) ||
                        (LocaleCompare("Modules",option) == 0))
                      {
                        (void) ListModuleInfo((FILE *) NULL,exception);
                        break;
                      }
#endif /* SupportMagickModules */
                    if (LocaleCompare("ModuleMap",option) == 0)
                      {
                        (void) ListModuleMap((FILE *) NULL,exception);
                        break;
                      }
                    ThrowConvertException(OptionError,UnrecognizedListType,
                      option)
                  }
                  case 'R':
                  case 'r':
                  {
                    if ((LocaleCompare("Resource",option) == 0) ||
                        (LocaleCompare("Resource",option)))
                      {
                        (void) ListMagickResourceInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowConvertException(OptionError,UnrecognizedListType,
                      option)
                  }
                  case 'T':
                  case 't':
                  {
                    if (LocaleCompare("Type",option) == 0)
                      {
                        (void) ListTypeInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowConvertException(OptionError,UnrecognizedListType,
                      option)
                  }
                  default:
                    ThrowConvertException(OptionError,UnrecognizedListType,
                      option)
                }
		status=MagickPass;
		goto convert_cleanup_and_return;
              }
            break;
          }
        if (LocaleCompare("log",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                (void) SetLogFormat(argv[i]);
              }
            break;
          }
        if (LocaleCompare("loop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'm':
      {
        if (LocaleCompare("magnify",option+1) == 0)
          break;
        if (LocaleCompare("map",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("mask",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("matte",option+1) == 0)
          break;
        if (LocaleCompare("mattecolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                (void) QueryColorDatabase(argv[i],&image_info->matte_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("median",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("minify",option+1) == 0)
          break;
        if (LocaleCompare("modulate",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("monitor",option+1) == 0)
          {
            if (*option == '+')
	      {
		(void) SetMonitorHandler((MonitorHandler) NULL);
		(void) MagickSetConfirmAccessHandler((ConfirmAccessHandler) NULL);
	      }
            else
	      {
		(void) SetMonitorHandler(CommandProgressMonitor);
		(void) MagickSetConfirmAccessHandler(CommandAccessMonitor);
	      }
            break;
          }
        if (LocaleCompare("monochrome",option+1) == 0)
          {
            image_info->monochrome=(*option == '-');
            break;
          }
        if (LocaleCompare("morph",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("mosaic",option+1) == 0)
          break;
        if (LocaleCompare("motion-blur",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowConvertException(OptionError,MissingArgument,option);
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'n':
      {
        if (LocaleCompare("negate",option+1) == 0)
          break;
        if (LocaleCompare("noise",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            if (*option == '+')
              {
		NoiseType
		  noise_type;

                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                option=argv[i];
		noise_type=StringToNoiseType(option);
		if (UndefinedNoise == noise_type)
                  ThrowConvertException(OptionError,UnrecognizedNoiseType,
                    option);
              }
            break;
          }
        if (LocaleCompare("noop",option+1) == 0)
          break;
        if (LocaleCompare("normalize",option+1) == 0)
          break;
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'o':
      {
        if (LocaleCompare("opaque",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("operator",option+1) == 0)
          {
            if (*option == '-')
              {
                ChannelType
                  channel;

                QuantumOperator
                  quantum_operator;

                double
                  rvalue;
                  
                /* channel */
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
                channel=StringToChannelType(argv[i]);
                if (channel == UndefinedChannel)
                  ThrowConvertException(OptionError,UnrecognizedChannelType,
                    option);

                /* operator id */
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
                quantum_operator=StringToQuantumOperator(argv[i]);
                if (quantum_operator == UndefinedQuantumOp)
                  ThrowConvertException(OptionError,UnrecognizedOperator,
                    option);

                /* rvalue */
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&rvalue))
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("ordered-dither",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("orient",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowConvertException(OptionError,MissingArgument,option);
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'p':
      {
        if (LocaleCompare("page",option+1) == 0)
          {
            (void) CloneString(&image_info->page,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
                image_info->page=GetPageGeometry(argv[i]);
              }
            break;
          }
        if (LocaleCompare("paint",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("ping",option+1) == 0)
          {
            ping=(*option == '-');
            break;
          }
        if (LocaleCompare("pointsize",option+1) == 0)
          {
            image_info->pointsize=12;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
                image_info->pointsize=MagickAtoF(argv[i]);
              }
            break;
          }
        if (LocaleCompare("preview",option+1) == 0)
          {
            image_info->preview_type=UndefinedPreview;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->preview_type=StringToPreviewType(option);
                if (image_info->preview_type == UndefinedPreview)
                  ThrowConvertException(OptionError,UnrecognizedPreviewType,
                     option);
              }
            break;
          }
        if (LocaleCompare("process",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("profile",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowConvertException(OptionError,MissingArgument,option);
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'q':
      {
        if (LocaleCompare("quality",option+1) == 0)
          {
            image_info->quality=DefaultCompressionQuality;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
                image_info->quality=MagickAtoL(argv[i]);
              }
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'r':
      {
        if (LocaleCompare("raise",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowConvertException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("random-threshold",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("recolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("red-primary",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("region",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("render",option+1) == 0)
          break;
        if (LocaleCompare("repage",option+1) == 0)
          {
	    if (*option == '-')
	      {
		i++;
		if ((i == argc) || !IsGeometry(argv[i]))
		  ThrowConvertException(OptionError,MissingArgument,option);
	      }
            break;
          }
        if (LocaleCompare("resample",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == (argc-1)) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("resize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("roll",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("rotate",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowConvertException(OptionError,MissingArgument,option);
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 's':
      {
        if (LocaleCompare("sample",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("sampling-factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
                NormalizeSamplingFactor(image_info);
              }
            break;
          }
        if (LocaleCompare("scale",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("scene",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("segment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("set",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowConvertException(OptionError,MissingArgument,option);
            if (*option == '-')
              {
                /* -set attribute value */
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("shade",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowConvertException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("sharpen",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowConvertException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("shave",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("shear",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowConvertException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("size",option+1) == 0)
          {
            (void) CloneString(&image_info->size,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->size,argv[i]);
              }
            break;
          }
        if (LocaleCompare("solarize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("spread",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("strip",option+1) == 0)
	  {
	    break;
	  }
        if (LocaleCompare("stroke",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("strokewidth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("swirl",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 't':
      {
        if (LocaleCompare("temporary",option+1) == 0)
          {
            image_info->temporary=(*option == '-');
            break;
          }
        if (LocaleCompare("texture",option+1) == 0)
          {
            (void) CloneString(&image_info->texture,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->texture,argv[i]);
              }
            break;
          }
        if (LocaleCompare("thumbnail",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("threshold",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("tile",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowConvertException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("transform",option+1) == 0)
          break;
        if (LocaleCompare("transparent",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("treedepth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("trim",option+1) == 0)
          break;
        if (LocaleCompare("type",option+1) == 0)
          {
            image_info->type=UndefinedType;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->type=StringToImageType(option);
                if (image_info->type == UndefinedType)
                  ThrowConvertException(OptionError,UnrecognizedImageType,option);
              }
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'u':
      {
        if (LocaleCompare("undercolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("units",option+1) == 0)
          {
            image_info->units=UndefinedResolution;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->units=UndefinedResolution;
                if (LocaleCompare("PixelsPerInch",option) == 0)
                  image_info->units=PixelsPerInchResolution;
                if (LocaleCompare("PixelsPerCentimeter",option) == 0)
                  image_info->units=PixelsPerCentimeterResolution;
              }
            break;
          }
        if (LocaleCompare("unsharp",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose+=(*option == '-');
            break;
          }
        if (LocaleCompare("verbose",option+1) == 0)
          break;
        if (LocaleCompare("view",option+1) == 0)
          {
            (void) CloneString(&image_info->view,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
                (void) CloneString(&image_info->view,argv[i]);
              }
            break;
          }
        if (LocaleCompare("virtual-pixel",option+1) == 0)
          {
            if (*option == '-')
              {
                VirtualPixelMethod
                  virtual_pixel_method;

                i++;
                if (i == argc)
                  ThrowConvertException(OptionError,MissingArgument,
                    option);
                option=argv[i];
                virtual_pixel_method=StringToVirtualPixelMethod(option);
                if (virtual_pixel_method == UndefinedVirtualPixelMethod)
                  ThrowConvertException(OptionError,UnrecognizedVirtualPixelMethod,option);
              }
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case 'w':
      {
        if (LocaleCompare("wave",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowConvertException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("white-point",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("white-threshold",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowConvertException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("write",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowConvertException(OptionError,MissingArgument,option);
            break;
          }
        ThrowConvertException(OptionError,UnrecognizedOption,option)
      }
      case '?':
        {
          ConvertUsage();
	  break;
          /* ThrowConvertException(OptionError,UsageError,NULL); */
        }
      default:
        {
          ThrowConvertException(OptionError,UnrecognizedOption,option);
        }
    }
  }
  if ((image == (Image *) NULL) && (image_list == (Image *) NULL))
    {
      if (exception->severity == UndefinedException)
        ThrowConvertException(OptionError,RequestDidNotReturnAnImage,
          (char *) NULL);
      status = MagickFail;
      goto convert_cleanup_and_return;
    }
  if (i != (argc-1))
    ThrowConvertException(OptionError,MissingAnImageFilename,(char *) NULL);
  if (image == (Image *) NULL)
    {
      status&=MogrifyImages(image_info,i-j,argv+j,&image_list);
      GetImageException(image_list,exception);
    }
  else
    {
      status&=MogrifyImages(image_info,i-j,argv+j,&image);
      GetImageException(image,exception);
      AppendImageToList(&image_list,image);
    }
  status&=WriteImages(image_info,image_list,argv[argc-1],exception);
  if (metadata != (char **) NULL)
    {
      char
        *text;

      /*
	Return formatted string with image characteristics if metadata
	is requested.
      */
      text=TranslateText(image_info,image_list,(format != (char *) NULL) ? format : "%w,%h,%m");
      if (text == (char *) NULL)
        ThrowConvertException(ResourceLimitError,MemoryAllocationFailed,
          MagickMsg(OptionError,UnableToFormatImageMetadata));
      (void) ConcatenateString(&(*metadata),text);
      (void) ConcatenateString(&(*metadata),"\n");
      MagickFreeMemory(text);
    }
 convert_cleanup_and_return:
  DestroyImageList(image_list);
  LiberateArgumentList(argc,argv);
  return(status);
}
#undef ThrowConvertException
#undef ThrowConvertException3

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n v e r t U s a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure ConvertUsage displays the program command syntax.
%
%  The format of the ConvertUsage method is:
%
%      void ConvertUsage()
%
*/
static void ConvertUsage(void)
{
  static const char
    *options[]=
    {
      "-adjoin              join images into a single multi-image file",
      "-affine matrix       affine transform matrix",
      "-antialias           remove pixel-aliasing",
      "-append              append an image sequence",
      "-asc-cdl spec        apply ASC CDL transform",
      "-authenticate value  decrypt image with this password",
      "-auto-orient         orient (rotate) image so it is upright",
      "-average             average an image sequence",
      "-background color    background color",
      "-black-threshold value",
      "                     pixels below the threshold become black",
      "-blue-primary point  chomaticity blue primary point",
      "-blur geometry       blur the image",
      "-border geometry     surround image with a border of color",
      "-bordercolor color   border color",
      "-box color           set the color of the annotation bounding box",
      "-channel type        extract a particular color channel from image",
      "-charcoal radius     simulate a charcoal drawing",
      "-chop geometry       remove pixels from the image interior",
      "-clip                apply first clipping path if the image has one",
      "-clippath            apply named clipping path if the image has one",
      "-coalesce            merge a sequence of images",
      "-colorize value      colorize the image with the fill color",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     alternate image colorspace",
      "-comment string      annotate image with comment",
      "-compose operator    composite operator",
      "-compress type       image compression type",
      "-contrast            enhance or reduce the image contrast",
      "-convolve kernel     convolve image with the specified convolution kernel",
      "-crop geometry       preferred size and location of the cropped image",
      "-cycle amount        cycle the image colormap",
      "-debug events        display copious debugging information",
      "-deconstruct         break down an image sequence into constituent parts",
      "-define values       Coder/decoder specific options",
      "-delay value         display the next image after pausing",
      "-density geometry    horizontal and vertical density of the image",
      "-depth value         image depth",
      "-despeckle           reduce the speckles within an image",
      "-display server      get image or font from this X server",
      "-dispose method      Undefined, None, Background, Previous",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-draw string         annotate the image with a graphic primitive",
      "-edge radius         apply a filter to detect edges in the image",
      "-emboss radius       emboss an image",
      "-encoding type       text encoding type",
      "-endian type         multibyte word order (LSB, MSB, or Native)",
      "-enhance             apply a digital filter to enhance a noisy image",
      "-equalize            perform histogram equalization to an image",
      "-extent              composite image on background color canvas image",
      "-fill color          color to use when filling a graphic primitive",
      "-filter type         use this filter when resizing an image",
      "-flatten             flatten a sequence of images",
      "-flip                flip image in the vertical direction",
      "-flop                flop image in the horizontal direction",
      "-font name           render text with this font",
      "-frame geometry      surround image with an ornamental border",
      "-fuzz distance       colors within this distance are considered equal",
      "-gamma value         level of gamma correction",
      "-gaussian geometry   gaussian blur an image",
      "-geometry geometry   perferred size or location of the image",
      "-green-primary point chomaticity green primary point",
      "-gravity type        horizontal and vertical text/object placement",
      "-hald-clut clut      apply a Hald CLUT to the image",
      "-help                print program options",
      "-implode amount      implode image pixels about the center",
      "-intent type         Absolute, Perceptual, Relative, or Saturation",
      "-interlace type      None, Line, Plane, or Partition",
      "-label name          assign a label to an image",
      "-lat geometry        local adaptive thresholding",
      "-level value         adjust the level of image contrast",
      "-limit type value    Disk, File, Map, Memory, Pixels, Width, Height or",
      "                     Threads resource limit",
      "-linewidth width     the line width for subsequent draw operations",
      "-list type           Color, Delegate, Format, Magic, Module, Resource,",
      "                     or Type",
      "-log format          format of debugging information",
      "-loop iterations     add Netscape loop extension to your GIF animation",
      "-magnify             interpolate image to double size",
      "-map filename        transform image colors to match this set of colors",
      "-mask filename       set the image clip mask",
      "-matte               store matte channel if the image has one",
      "-mattecolor color    specify the color to be used with the -frame option",
      "-median radius       apply a median filter to the image",
      "-minify              interpolate the image to half size",
      "-modulate value      vary the brightness, saturation, and hue",
      "-monitor             show progress indication",
      "-monochrome          transform image to black and white",
      "-morph value         morph an image sequence",
      "-mosaic              create a mosaic from an image sequence",
      "-motion-blur radiusxsigma+angle",
      "                     simulate motion blur",
      "-negate              replace every pixel with its complementary color ",
      "-noop                do not apply options to image",
      "-noise radius        add or reduce noise in an image",
      "-normalize           transform image to span the full range of colors",
      "-opaque color        change this color to the fill color",
      "-operator channel operator rvalue",
      "                     apply a mathematical or bitwise operator to channel",
      "-ordered-dither channeltype NxN",
      "                     ordered dither the image",
      "-orient orientation  set image orientation attribute",
      "+page                reset current page offsets to default",
      "-page geometry       size and location of an image canvas",
      "-paint radius        simulate an oil painting",
      "-ping                efficiently determine image attributes",
      "-pointsize value     font point size",
      "-preview type        image preview type",
      "-profile filename    add ICM or IPTC information profile to image",
      "-quality value       JPEG/MIFF/PNG compression level",
      "-raise value         lighten/darken image edges to create a 3-D effect",
      "-random-threshold channeltype LOWxHIGH",
      "                     random threshold the image",
      "-recolor matrix      apply a color translation matrix to image channels",
      "-red-primary point   chomaticity red primary point",
      "-region geometry     apply options to a portion of the image",
      "-render              render vector graphics",
      "+render              disable rendering vector graphics",
      "-resample geometry   resample to horizontal and vertical resolution",
      "+repage              reset current page offsets to default",
      "-repage geometry     adjust current page offsets by geometry",
      "-resize geometry     resize the image",
      "-roll geometry       roll an image vertically or horizontally",
      "-rotate degrees      apply Paeth rotation to the image",
      "-sample geometry     scale image with pixel sampling",
      "-sampling-factor HxV[,...]",
      "                     horizontal and vertical sampling factors",
      "-scale geometry      scale the image",
      "-scene value         image scene number",
      "-seed value          pseudo-random number generator seed value",
      "-segment values      segment an image",
      "-set attribute value set image attribute",
      "+set attribute       unset image attribute",
      "-shade degrees       shade the image using a distant light source",
      "-sharpen geometry    sharpen the image",
      "-shave geometry      shave pixels from the image edges",
      "-shear geometry      slide one edge of the image along the X or Y axis",
      "-size geometry       width and height of image",
      "-solarize threshold  negate all pixels above the threshold level",
      "-spread amount       displace image pixels by a random amount",
      "-stroke color        graphic primitive stroke color",
      "-strokewidth value   graphic primitive stroke width",
      "-strip               strip all profiles and text attributes from image",
      "-swirl degrees       swirl image pixels about the center",
      "-texture filename    name of texture to tile onto the image background",
      "-threshold value     threshold the image",
      "-thumbnail geometry  resize the image (optimized for thumbnails)",
      "-tile filename       tile image when filling a graphic primitive",
      "-transform           affine transform image",
      "-transparent color   make this color transparent within the image",
      "-treedepth value     color tree depth",
      "-trim                trim image edges",
      "-type type           image type",
      "-undercolor color    annotation bounding box color",
      "-units type          PixelsPerInch, PixelsPerCentimeter, or Undefined",
      "-unsharp geometry    sharpen the image",
      "-verbose             print detailed information about the image",
      "-version             print version information",
      "-view                FlashPix viewing transforms",
      "-virtual-pixel method",
      "                     Constant, Edge, Mirror, or Tile",
      "-wave geometry       alter an image along a sine wave",
      "-white-point point   chomaticity white point",
      "-white-threshold value",
      "                     pixels above the threshold become white",
      "-write filename      write image to this file",
      (char *) NULL
    };

  const char
    **p;

  PrintUsageHeader();
  (void) printf("Usage: %.1024s [options ...] file [ [options ...] "
    "file ...] [options ...] file\n",GetClientName());
  (void) printf("\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf(
    "\nBy default, the image format of `file' is determined by its magic\n");
  (void) printf(
    "number.  To specify a particular image format, precede the filename\n");
  (void) printf(
    "with an image format name and a colon (i.e. ps:image) or specify the\n");
  (void) printf(
    "image type as the filename suffix (i.e. image.ps).  Specify 'file' as\n");
  (void) printf("'-' for standard input or output.\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   C o n j u r e U s a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConjureUsage() displays the program command syntax.
%
%  The format of the ConjureUsage method is:
%
%      void ConjureUsage()
%
*/
static void ConjureUsage(void)
{
  static const char
    *options[]=
    {
      "-debug events        display copious debugging information",
      "-help                print program options",
      "-log format          format of debugging information",
      "-verbose             print detailed information about the image",
      "-version             print version information",
      (char *) NULL
    };

  const char
    **p;

  PrintUsageHeader();
  (void) printf("Usage: %.1024s [options ...] file [ [options ...] file ...]\n",
    GetClientName());
  (void) printf("\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf("\nIn additiion, define any key value pairs required by "
    "your script.  For\nexample,\n\n");
  (void) printf("    conjure -size 100x100 -color blue -foo bar script.msl\n");
}

MagickExport unsigned int ConjureImageCommand(ImageInfo *image_info,
  int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
  char
    *option;

  Image
    *image;

  register long
    i;

  unsigned int
    status=MagickPass;

  if (argc < 2 || ((argc < 3) && (LocaleCompare("-help",argv[1]) == 0 ||
      LocaleCompare("-?",argv[1]) == 0)))
    {
      ConjureUsage();
      if (argc < 2)
	{
	  ThrowException(exception,OptionError,UsageError,NULL);
	  return MagickFail;
	}
      return MagickPass;
    }
  if (LocaleCompare("-version",argv[1]) == 0)
    {
      (void) VersionCommand(image_info,argc,argv,metadata,exception);
      return MagickPass;
    }

  /*
    Expand argument list
  */
  status=ExpandFilenames(&argc,&argv);
  if (status == MagickFail)
    MagickFatalError(ResourceLimitFatalError,MemoryAllocationFailed,
      (char *) NULL);

  /*
    Validate command list
  */
  image_info=CloneImageInfo((ImageInfo *) NULL);
  image_info->attributes=AllocateImage(image_info);
  status=True;
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) != 1) && ((*option == '-') || (*option == '+')))
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) SetLogEventMask(argv[i]);
              }
            continue;
          }
        if (LocaleCompare("help",option+1) == 0 ||
            LocaleCompare("?",option+1) == 0)
          {
            if (*option == '-')
              ConjureUsage();
            continue;
          }
        if (LocaleCompare("log",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) SetLogFormat(argv[i]);
              }
            continue;
          }
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose+=(*option == '-');
            continue;
          }
        if (LocaleCompare("version",option+1) == 0)
          {
            PrintVersionAndCopyright();
            Exit(0);
            continue;
          }
        /*
          Persist key/value pair.
        */
        (void) SetImageAttribute(image_info->attributes,option+1,(char *) NULL);
        status&=SetImageAttribute(image_info->attributes,option+1,argv[i+1]);
        if (status == MagickFail)
          MagickFatalError(ImageFatalError,UnableToPersistKey,option);
        i++;
        continue;
      }
    /*
      Interpret MSL script.
    */
    (void) SetImageAttribute(image_info->attributes,"filename",(char *) NULL);
    status&=SetImageAttribute(image_info->attributes,"filename",argv[i]);
    if (status == MagickFail)
      MagickFatalError(ImageFatalError,UnableToPersistKey,argv[i]);
    (void) FormatString(image_info->filename,"msl:%.1024s",argv[i]);
    image=ReadImage(image_info,exception);
    if (exception->severity > UndefinedException)
      {
	CatchException(exception);
	DestroyExceptionInfo(exception);
	GetExceptionInfo(exception);
      }
    status&=(image != (Image *) NULL);
    if (image != (Image *) NULL)
      DestroyImageList(image);
  }
  DestroyImageInfo(image_info);
  LiberateArgumentList(argc,argv);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   D i s p l a y U s a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  DisplayUsage() displays the program command syntax.
%
%  The format of the DisplayUsage method is:
%
%      void DisplayUsage(void)
%
%  A description of each parameter follows:
%
%
*/
#if defined(HasX11)
static void DisplayUsage(void)
{
  const char
    **p;

  static const char
    *buttons[]=
    {
      "1    press to map or unmap the Command widget",
      "2    press and drag to magnify a region of an image",
      "3    press to load an image from a visual image directory",
      (char *) NULL
    },
    *options[]=
    {
      "-authenticate value  decrypt image with this password",
      "-backdrop            display image centered on a backdrop",
      "-border geometry     surround image with a border of color",
      "-colormap type       Shared or Private",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     alternate image colorspace",
      "-comment string      annotate image with comment",
      "-compress type       image compression type",
      "-contrast            enhance or reduce the image contrast",
      "-crop geometry       preferred size and location of the cropped image",
      "-debug events        display copious debugging information",
      "-define values       Coder/decoder specific options",
      "-delay value         display the next image after pausing",
      "-density geometry    horizontal and vertical density of the image",
      "-depth value         image depth",
      "-despeckle           reduce the speckles within an image",
      "-display server      display image to this X server",
      "-dispose method      Undefined, None, Background, Previous",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-edge factor         apply a filter to detect edges in the image",
      "-endian type         multibyte word order (LSB, MSB, or Native)",
      "-enhance             apply a digital filter to enhance a noisy image",
      "-filter type         use this filter when resizing an image",
      "-flip                flip image in the vertical direction",
      "-flop                flop image in the horizontal direction",
      "-frame geometry      surround image with an ornamental border",
      "-gamma value         level of gamma correction",
      "-geometry geometry   preferred size and location of the Image window",
      "-help                print program options",
      "-immutable           displayed image cannot be modified",
      "-interlace type      None, Line, Plane, or Partition",
      "-label name          assign a label to an image",
      "-limit type value    Disk, File, Map, Memory, Pixels, Width, Height or",
      "                     Threads resource limit",
      "-log format          format of debugging information",
      "-map type            display image using this Standard Colormap",
      "-matte               store matte channel if the image has one",
      "-monitor             show progress indication",
      "-monochrome          transform image to black and white",
      "-negate              replace every pixel with its complementary color",
      "-noop                do not apply options to image",
      "-page geometry       size and location of an image canvas",
      "+progress            disable progress monitor and busy cursor",
      "-quality value       JPEG/MIFF/PNG compression level",
      "-raise value         lighten/darken image edges to create a 3-D effect",
      "-remote command      execute a command in an remote display process",
      "-roll geometry       roll an image vertically or horizontally",
      "-rotate degrees      apply Paeth rotation to the image",
      "-sample geometry     scale image with pixel sampling",
      "-sampling-factor HxV[,...]",
      "                     horizontal and vertical sampling factors",
      "-scenes range        image scene range",
      "-segment value       segment an image",
      "-set attribute value set image attribute",
      "+set attribute       unset image attribute",
      "-sharpen geometry    sharpen the image",
      "-size geometry       width and height of image",
      "-texture filename    name of texture to tile onto the image background",
      "-treedepth value     color tree depth",
      "-trim                trim image edges",
      "-type type           image type",
      "-update seconds      detect when image file is modified and redisplay",
      "-verbose             print detailed information about the image",
      "-version             print version information",
      "-visual type         display image using this visual type",
      "-virtual-pixel method",
      "                     Constant, Edge, Mirror, or Tile",
      "-window id           display image to background of this window",
      "-window_group id     exit program when this window id is destroyed",
      "-write filename      write image to a file",
      (char *) NULL
    };

  PrintUsageHeader();
  (void) printf("Usage: %.1024s [options ...] file [ [options ...] file ...]\n",
    GetClientName());
  (void) printf("\nWhere options include: \n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf(
    "\nIn addition to those listed above, you can specify these standard X\n");
  (void) printf(
    "resources as command line options:  -background, -bordercolor,\n");
  (void) printf(
    "-borderwidth, -font, -foreground, -iconGeometry, -iconic, -mattecolor,\n");
  (void) printf("-name, -shared-memory, -usePixmap, or -title.\n");
  (void) printf(
    "\nBy default, the image format of `file' is determined by its magic\n");
  (void) printf(
    "number.  To specify a particular image format, precede the filename\n");
  (void) printf(
    "with an image format name and a colon (i.e. ps:image) or specify the\n");
  (void) printf(
    "image type as the filename suffix (i.e. image.ps).  Specify 'file' as\n");
  (void) printf("'-' for standard input or output.\n");
  (void) printf("\nButtons: \n");
  for (p=buttons; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
}
#endif /* HasX11 */

MagickExport unsigned int DisplayImageCommand(ImageInfo *image_info,
  int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
#if defined(HasX11)
  char
    *option,
    *resource_value,
    *server_name;

  const char
    *client_name;

  Display
    *display;

  double
    sans;

  Image
    *image,
    *next;

  long
    first_scene,
    image_number,
    j,
    k,
    last_scene,
    scene,
    x;

  QuantizeInfo
    *quantize_info;

  register long
    i;

  unsigned int
    *image_marker,
    last_image,
    status;

  unsigned long
    state;

  MagickXResourceInfo
    resource_info;

  XrmDatabase
    resource_database;

  if (argc < 3)
    {
      if ((LocaleCompare("-help",argv[1]) == 0) ||
          (LocaleCompare("-?",argv[1]) == 0))
        {
          DisplayUsage();
          return MagickPass;
        }
      else if (LocaleCompare("-version",argv[1]) == 0)
        {
          (void) VersionCommand(image_info,argc,argv,metadata,exception);
          return MagickPass;
        }
    }

  /*
    Expand Argument List
  */
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickFatalError(ResourceLimitFatalError,MemoryAllocationFailed,
      (char *) NULL);

  /*
    Set defaults.
  */
  SetNotifyHandlers;
  display=(Display *) NULL;
  first_scene=0;
  image_number=0;
  last_image=0;
  last_scene=0;
  image_marker=
    MagickAllocateMemory(unsigned int *,(argc+1)*sizeof(unsigned int));
  if (image_marker == (unsigned int *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
      UnableToDisplayImage);
  for (i=0; i <= argc; i++)
    image_marker[i]=argc;
  resource_database=(XrmDatabase) NULL;
  server_name=(char *) NULL;
  state=0;
  /*
    Check for server name specified on the command line.
  */
  for (i=1; i < argc; i++)
  {
    /*
      Check command line for server name.
    */
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      continue;
    if (LocaleCompare("display",option+1) == 0)
      {
        /*
          User specified server name.
        */
        i++;
        if (i == argc)
          MagickFatalError(OptionFatalError,MissingArgument,option);
        server_name=argv[i];
        break;
      }
  }
  /*
    Get user defaults from X resource database.
  */
  display=XOpenDisplay(server_name);
  if (display == (Display *) NULL)
    MagickFatalError(XServerFatalError,UnableToOpenXServer,
      XDisplayName(server_name));
  (void) XSetErrorHandler(MagickXError);
  client_name=GetClientName();
  resource_database=MagickXGetResourceDatabase(display,client_name);
  MagickXGetResourceInfo(resource_database,(char *) client_name,&resource_info);
  image_info=resource_info.image_info;
  quantize_info=resource_info.quantize_info;
  image_info->density=
    MagickXGetResourceInstance(resource_database,client_name,"density",(char *) NULL);
  if (image_info->density == (char *) NULL)
    image_info->density=MagickXGetScreenDensity(display);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"interlace","none");
  image_info->interlace=StringToInterlaceType(resource_value);
  if (image_info->interlace == UndefinedInterlace)
    MagickError(OptionError,UnrecognizedInterlaceType,resource_value);
  image_info->page=MagickXGetResourceInstance(resource_database,client_name,
    "pageGeometry",(char *) NULL);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"quality","75");
  image_info->quality=MagickAtoL(resource_value);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"verbose","False");
  image_info->verbose=MagickIsTrue(resource_value);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"dither","True");
  quantize_info->dither=MagickIsTrue(resource_value);
  /*
    Parse command line.
  */
  status=True;
  j=1;
  k=0;
  for (i=1; ((i <= argc) && !(state & ExitState)); i++)
  {
    if (i < argc)
      option=argv[i];
    else
      if (image_number != 0)
        break;
      else
        option=(char *) "logo:Untitled";
    if ((strlen(option) < 2) ||
	/* stdin + subexpression */
	((option[0] == '-') && (option[1] == '[')) ||
	((option[0] != '-') && option[0] != '+'))
      {
        /*
          Option is a file name.
        */
        k=i;
        for (scene=first_scene; scene <= last_scene ; scene++)
        {
          /*
            Read image.
          */
          (void) strlcpy(image_info->filename,option,MaxTextExtent);
          if (first_scene != last_scene)
            {
              char
                filename[MaxTextExtent];

              /*
                Form filename for multi-part images.
              */
              (void) MagickSceneFileName(filename,image_info->filename,".%lu",MagickTrue,scene);
              (void) strlcpy(image_info->filename,filename,MaxTextExtent);
            }
          (void) strcpy(image_info->magick,"MIFF");
          image_info->colorspace=quantize_info->colorspace;
          image_info->dither=quantize_info->dither;
	  DestroyExceptionInfo(exception);
	  GetExceptionInfo(exception);
          image=ReadImage(image_info,exception);
          if (exception->severity > UndefinedException)
	    {
	      CatchException(exception);
	      DestroyExceptionInfo(exception);
	      GetExceptionInfo(exception);
	    }
          status&=image != (Image *) NULL;
          if (image == (Image *) NULL)
            continue;
          status&=MogrifyImage(image_info,i-j,argv+j,&image);
	  (void) CatchImageException(image);
          do
          {
            /*
              Transmogrify image as defined by the image processing options.
            */
            resource_info.quantum=1;
            if (first_scene != last_scene)
              image->scene=scene;
            /*
              Display image to X server.
            */
            if (resource_info.window_id != (char *) NULL)
              {
                /*
                  Display image to a specified X window.
                */
                if (MagickXDisplayBackgroundImage(display,&resource_info,image))
                  state|=RetainColorsState;
              }
            else
              do
              {
                Image
                  *nexus;

                /*
                  Display image to X server.
                */
                nexus=
                  MagickXDisplayImage(display,&resource_info,argv,argc,&image,&state);
                if (nexus == (Image *) NULL)
                  break;
                while ((nexus != (Image *) NULL) && (!(state & ExitState)))
                {
                  if (nexus->montage != (char *) NULL)
                    {
                      /*
                        User selected a visual directory image (montage).
                      */
                      DestroyImageList(image);
                      image=nexus;
                      break;
                    }
                  if (first_scene != last_scene)
                    image->scene=scene;
                  next=MagickXDisplayImage(display,&resource_info,argv,argc,&nexus,
                    &state);
                  if ((next == (Image *) NULL) &&
                      (nexus->next != (Image *) NULL))
                    {
                      DestroyImageList(image);
                      image=nexus->next;
                      nexus=(Image *) NULL;
                    }
                  else
                    {
                      if (nexus != image)
                        DestroyImageList(nexus);
                      nexus=next;
                    }
                }
              } while (!(state & ExitState));
            if (resource_info.write_filename != (char *) NULL)
              {
                /*
                  Write image.
                */
                (void) strlcpy(image->filename,resource_info.write_filename,
                  MaxTextExtent);
                (void) SetImageInfo(image_info,SETMAGICK_WRITE,&image->exception);
                status&=WriteImage(image_info,image);
                (void) CatchImageException(image);
              }
            if (image_info->verbose)
              (void) DescribeImage(image,stderr,False);
            /*
              Proceed to next/previous image.
            */
            next=image;
            if (state & FormerImageState)
              for (k=0; k < resource_info.quantum; k++)
              {
                next=next->previous;
                if (next == (Image *) NULL)
                  break;
              }
            else
              for (k=0; k < resource_info.quantum; k++)
              {
                next=next->next;
                if (next == (Image *) NULL)
                  break;
              }
            if (next != (Image *) NULL)
              image=next;
          } while ((next != (Image *) NULL) && !(state & ExitState));
          /*
            Free image resources.
          */
          DestroyImageList(image);
          image=(Image *) NULL;
          if (!(state & FormerImageState))
            {
              last_image=image_number;
              image_marker[i]=image_number++;
            }
          else
            {
              /*
                Proceed to previous image.
              */
              for (i--; i > 0; i--)
                if ((int) image_marker[i] == (image_number-2))
                  break;
              image_number--;
            }
          if (state & ExitState)
            break;
        }
        /*
          Determine if we should proceed to the first image.
        */
        if (image_number < 0)
          {
            if (state & FormerImageState)
              {
                for (i=1; i < (argc-2); i++)
                  if (image_marker[i] == last_image)
                    break;
                image_number=image_marker[i]+1;
              }
            continue;
          }
        continue;
      }
    j=k+1;
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("authenticate",option+1) == 0)
          {
            (void) CloneString(&image_info->authenticate,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->authenticate,argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'b':
      {
        if (LocaleCompare("backdrop",option+1) == 0)
          {
            resource_info.backdrop=(*option == '-');
            break;
          }
        if (LocaleCompare("background",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.background_color=argv[i];
                (void) QueryColorDatabase(argv[i],&image_info->background_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("border",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.border_color=argv[i];
                (void) QueryColorDatabase(argv[i],&image_info->border_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("borderwidth",option+1) == 0)
          {
            resource_info.border_width=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.border_width=MagickAtoI(argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'c':
      {
        if (LocaleCompare("colormap",option+1) == 0)
          {
            resource_info.colormap=PrivateColormap;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                resource_info.colormap=UndefinedColormap;
                if (LocaleCompare("private",option) == 0)
                  resource_info.colormap=PrivateColormap;
                if (LocaleCompare("shared",option) == 0)
                  resource_info.colormap=SharedColormap;
                if (resource_info.colormap == UndefinedColormap)
                  MagickFatalError(OptionFatalError,UnrecognizedColormapType,
                    option);
              }
            break;
          }
        if (LocaleCompare("colors",option+1) == 0)
          {
            quantize_info->number_colors=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                quantize_info->number_colors=MagickAtoL(argv[i]);
              }
            break;
          }
        if (LocaleCompare("colorspace",option+1) == 0)
          {
            quantize_info->colorspace=RGBColorspace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                quantize_info->colorspace=StringToColorspaceType(option);
                if (IsGrayColorspace(quantize_info->colorspace))
                  {
                    quantize_info->number_colors=256;
                    quantize_info->tree_depth=8;
                  }
                if (quantize_info->colorspace == UndefinedColorspace)
                  MagickFatalError(OptionFatalError,InvalidColorspaceType,
                    option);
              }
            break;
          }
        if (LocaleCompare("comment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("compress",option+1) == 0)
          {
            image_info->compression=NoCompression;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                image_info->compression=StringToCompressionType(option);
                if (image_info->compression == UndefinedCompression)
                  MagickFatalError(OptionFatalError,UnrecognizedImageCompressionType,
                    option);
              }
            break;
          }
        if (LocaleCompare("contrast",option+1) == 0)
          break;
        if (LocaleCompare("crop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            (void) SetLogEventMask("None");
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) SetLogEventMask(argv[i]);
              }
            break;
          }
        if (LocaleCompare("define",option+1) == 0)
          {
            i++;
            if (i == argc)
              MagickFatalError(OptionFatalError,MissingArgument,option);
            if (*option == '+')
              (void) RemoveDefinitions(image_info,argv[i]);
            else
              (void) AddDefinitions(image_info,argv[i],exception);
            break;
          }
        if (LocaleCompare("delay",option+1) == 0)
          {
            resource_info.delay=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.delay=MagickAtoI(argv[i]);
              }
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            (void) CloneString(&image_info->density,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) CloneString(&image_info->density,argv[i]);
              }
            break;
          }
        if (LocaleCompare("depth",option+1) == 0)
          {
            image_info->depth=QuantumDepth;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                image_info->depth=MagickAtoL(argv[i]);
              }
            break;
          }
        if (LocaleCompare("despeckle",option+1) == 0)
          break;
        if (LocaleCompare("display",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                
                (void) CloneString(&image_info->server_name,argv[i]);
              }
            break;
          }
        if (LocaleCompare("dispose",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                if ((LocaleCompare("0",option) != 0) &&
                    (LocaleCompare("1",option) != 0) &&
                    (LocaleCompare("2",option) != 0) &&
                    (LocaleCompare("3",option) != 0) &&
                    (LocaleCompare("Undefined",option) != 0) &&
                    (LocaleCompare("None",option) != 0) &&
                    (LocaleCompare("Background",option) != 0) &&
                    (LocaleCompare("Previous",option) != 0))
                  MagickFatalError(OptionFatalError,UnrecognizedDisposeMethod,
                    option);
              }
            break;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            quantize_info->dither=(*option == '-');
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'e':
      {
        if (LocaleCompare("edge",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("endian",option+1) == 0)
          {
            image_info->endian=UndefinedEndian;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                image_info->endian=StringToEndianType(option);
                if (image_info->endian == UndefinedEndian)
                  MagickFatalError(OptionFatalError,InvalidEndianType,option);
              }
            break;
          }
        if (LocaleCompare("enhance",option+1) == 0)
          break;
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'f':
      {
        if (LocaleCompare("filter",option+1) == 0)
          {
            if (*option == '-')
              {
                FilterTypes
                  filter;

                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                filter=StringToFilterTypes(option);
                if (filter == UndefinedFilter)
                  MagickFatalError(OptionFatalError,UnrecognizedFilterType,
                    option);
              }
            break;
          }
        if (LocaleCompare("flip",option+1) == 0)
          break;
        if (LocaleCompare("flop",option+1) == 0)
          break;
        if (LocaleCompare("font",option+1) == 0)
          {
            (void) CloneString(&image_info->font,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                image_info->font=argv[i];
              }
            if ((image_info->font == (char *) NULL) ||
                (*image_info->font != '@'))
              resource_info.font=AllocateString(image_info->font);
            break;
          }
        if (LocaleCompare("foreground",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.foreground_color=argv[i];
              }
             break;
          }
        if (LocaleCompare("frame",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'g':
      {
        if (LocaleCompare("gamma",option+1) == 0)
          {
            i++;
            if (i == argc)
              MagickFatalError(OptionFatalError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("geometry",option+1) == 0)
          {
            MagickFreeMemory(resource_info.image_geometry);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.image_geometry=AcquireString(argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'h':
      {
        if (LocaleCompare("help",option+1) == 0)
	  {
	    DisplayUsage();
	    break;
	  }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'i':
      {
        if (LocaleCompare("iconGeometry",option+1) == 0)
          {
            resource_info.icon_geometry=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.icon_geometry=argv[i];
              }
            break;
          }
        if (LocaleCompare("iconic",option+1) == 0)
          {
            resource_info.iconic=(*option == '-');
            break;
          }
        if (LocaleCompare("immutable",option+1) == 0)
          {
            resource_info.immutable=(*option == '-');
            break;
          }
        if (LocaleCompare("interlace",option+1) == 0)
          {
            image_info->interlace=UndefinedInterlace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                image_info->interlace=StringToInterlaceType(option);
                if (image_info->interlace == UndefinedInterlace)
                  MagickFatalError(OptionFatalError,InvalidInterlaceType,
                    option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'l':
      {
        if (LocaleCompare("label",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("limit",option+1) == 0)
          {
            if (*option == '-')
              {
                ResourceType
                  resource_type;

                char
                  *type;

                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                type=argv[i];
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_type=StringToResourceType(type);
                if (resource_type == UndefinedResource)
                  MagickFatalError(OptionFatalError,UnrecognizedResourceType,type);
                (void) SetMagickResourceLimit(resource_type,MagickSizeStrToInt64(argv[i],1024));
              }
            break;
          }
        if (LocaleCompare("log",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) SetLogFormat(argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'm':
      {
        if (LocaleCompare("magnify",option+1) == 0)
          {
            resource_info.magnify=2;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.magnify=MagickAtoI(argv[i]);
              }
            break;
          }
        if (LocaleCompare("map",option+1) == 0)
          {
            (void) strcpy(argv[i]+1,"sans");
            resource_info.map_type=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.map_type=argv[i];
              }
            break;
          }
        if (LocaleCompare("matte",option+1) == 0)
          break;
        if (LocaleCompare("mattecolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.matte_color=argv[i];
                (void) QueryColorDatabase(argv[i],&image_info->matte_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("monitor",option+1) == 0)
          {
            if (*option == '+')
	      {
		(void) SetMonitorHandler((MonitorHandler) NULL);
		(void) MagickSetConfirmAccessHandler((ConfirmAccessHandler) NULL);
	      }
            else
	      {
		(void) SetMonitorHandler(CommandProgressMonitor);
		(void) MagickSetConfirmAccessHandler(CommandAccessMonitor);
	      }
            break;
          }
          if (LocaleCompare("monochrome",option+1) == 0)
          {
            image_info->monochrome=(*option == '-');
            if (image_info->monochrome)
              {
                quantize_info->number_colors=2;
                quantize_info->tree_depth=8;
                quantize_info->colorspace=GRAYColorspace;
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'n':
      {
        if (LocaleCompare("name",option+1) == 0)
          {
            resource_info.name=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.name=argv[i];
              }
            break;
          }
        if (LocaleCompare("negate",option+1) == 0)
          break;
        if (LocaleCompare("noop",option+1) == 0)
          break;
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'p':
      {
        if (LocaleCompare("page",option+1) == 0)
          {
            (void) CloneString(&image_info->page,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                image_info->page=GetPageGeometry(argv[i]);
              }
            break;
          }
        if (LocaleCompare("progress",option+1) == 0)
          {
            resource_info.image_info->progress=(*option == '-');
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'q':
      {
        if (LocaleCompare("quality",option+1) == 0)
          {
            image_info->quality=DefaultCompressionQuality;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                image_info->quality=MagickAtoL(argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'r':
      {
        if (LocaleCompare("raise",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              MagickFatalError(OptionFatalError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("remote",option+1) == 0)
          {
            i++;
            if (i == argc)
              MagickFatalError(OptionFatalError,MissingArgument,option);
            status=MagickXRemoteCommand(display,resource_info.window_id,argv[i]);
            Exit(!status);
          }
        if (LocaleCompare("roll",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("rotate",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              MagickFatalError(OptionFatalError,MissingArgument,option);
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 's':
      {
        if (LocaleCompare("sample",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("sampling-factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
                NormalizeSamplingFactor(image_info);
              }
            break;
          }
        if (LocaleCompare("scenes",option+1) == 0)
          {
            first_scene=0;
            last_scene=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                first_scene=MagickAtoL(argv[i]);
                last_scene=first_scene;
                (void) sscanf(argv[i],"%ld-%ld",&first_scene,&last_scene);
              }
            break;
          }
        if (LocaleCompare("segment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("set",option+1) == 0)
          {
            i++;
            if (i == argc)
              MagickFatalError(OptionFatalError,MissingArgument,option);
            if (*option == '-')
              {
                /* -set attribute value */
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("sharpen",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("shared-memory",option+1) == 0)
          {
            resource_info.use_shared_memory=(*option == '-');
            break;
          }
        if (LocaleCompare("size",option+1) == 0)
          {
            (void) CloneString(&image_info->size,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) CloneString(&image_info->size,argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 't':
      {
        if (LocaleCompare("text_font",option+1) == 0)
          {
            resource_info.text_font=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.text_font=argv[i];
              }
            break;
          }
        if (LocaleCompare("texture",option+1) == 0)
          {
            (void) CloneString(&image_info->texture,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) CloneString(&image_info->texture,argv[i]);
              }
            break;
          }
        if (LocaleCompare("title",option+1) == 0)
          {
            resource_info.title=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.title=argv[i];
              }
            break;
          }
        if (LocaleCompare("treedepth",option+1) == 0)
          {
            quantize_info->tree_depth=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                quantize_info->tree_depth=MagickAtoI(argv[i]);
              }
            break;
          }
        if (LocaleCompare("trim",option+1) == 0)
          {
            break;
          }
        if (LocaleCompare("type",option+1) == 0)
          {
            resource_info.image_info->type=UndefinedType;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                resource_info.image_info->type=StringToImageType(option);
                if (resource_info.image_info->type == UndefinedType)
                  MagickFatalError(OptionFatalError,UnrecognizedImageType,
                                   option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'u':
      {
        if (LocaleCompare("update",option+1) == 0)
          {
            resource_info.update=(*option == '-');
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.update=MagickAtoI(argv[i]);
              }
            break;
          }
        if ((LocaleCompare("use_pixmap",option+1) == 0) ||
            (LocaleCompare("usePixmap",option+1) == 0))
          {
            resource_info.use_pixmap=(*option == '-');
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose+=(*option == '-');
            break;
          }
        if (LocaleCompare("version",option+1) == 0)
          break;
        if (LocaleCompare("visual",option+1) == 0)
          {
            resource_info.visual_type=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                resource_info.visual_type=argv[i];
              }
            break;
          }
        if (LocaleCompare("virtual-pixel",option+1) == 0)
          {
            if (*option == '-')
              {
                VirtualPixelMethod
                  virtual_pixel_method;

                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                option=argv[i];
                virtual_pixel_method=StringToVirtualPixelMethod(option);
                if (virtual_pixel_method == UndefinedVirtualPixelMethod)
                  MagickFatalError(OptionFatalError,UnrecognizedVirtualPixelMethod,option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'w':
      {
        if (LocaleCompare("window",option+1) == 0)
          {
            resource_info.window_id=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                resource_info.window_id=argv[i];
              }
            break;
          }
        if (LocaleCompare("window_group",option+1) == 0)
          {
            resource_info.window_group=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                resource_info.window_group=argv[i];
              }
            break;
          }
        if (LocaleCompare("write",option+1) == 0)
          {
            resource_info.write_filename=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.write_filename=argv[i];
                if (IsAccessible(resource_info.write_filename))
                  {
                    char
		      *answer,
                      answer_buffer[2];

		    answer_buffer[0]='\0';
                    (void) fprintf(stderr,"Overwrite %.1024s? ",
				   resource_info.write_filename);
                    answer=fgets(answer_buffer,sizeof(answer_buffer),stdin);
                    if ((NULL == answer) || !((answer[0] == 'y') || (answer[0] == 'Y')))
                      Exit(0);
                  }
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case '?':
      {
        DisplayUsage();
        break;
      }
      default:
      {
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
    }
  }
  if (state & RetainColorsState)
    {
      MagickXRetainWindowColors(display,XRootWindow(display,XDefaultScreen(display)));
      (void) XSync(display,False);
    } 
  if (resource_database != (XrmDatabase) NULL)
    {
      /* It seems that recent X11 libraries (as found in FreeBSD 5.4)
         automatically destroy the resource database associated with
         the display and there are double-frees if we destroy the
         resource database ourselves. */
      /* XrmDestroyDatabase(resource_database); */
      resource_database=(XrmDatabase) NULL;
    }

  MagickFreeMemory(image_marker);
  MagickXDestroyResourceInfo(&resource_info);
  LiberateArgumentList(argc,argv);
  MagickXDestroyX11Resources();
  (void) XCloseDisplay(display);
  return(status);
#else
  ARG_NOT_USED(image_info);
  ARG_NOT_USED(argc);
  ARG_NOT_USED(argv);
  ARG_NOT_USED(metadata);
  ARG_NOT_USED(exception);

  MagickFatalError(MissingDelegateError,XWindowLibraryIsNotAvailable,
    (char *) NULL);
  return(False);
#endif
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t O p t i o n V a l u e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetOptionValue sets the option value to variable pointed by result pointer
%  and return OptionSuccess when the value is not null. Otherwise, print error
%  and returns OPtionMissingValue.
%
%  The format of the GetOptionValue method is:
%
%      OptionStatus GetOptionValue(const char *option, const char *value,
%        char **result)
%
%  A description of each parameter follows:
%
%    o option: The option to get the value.
%
%    o value: The value to be check.
%
%    o result: Points to the variable to be set to value.
%
*/
static OptionStatus GetOptionValue(const char *option, char *value,
  char **result)
{
  OptionStatus status = CheckOptionValue(option, value);
  if (status == OptionSuccess)
    *result = value;
  return status;
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t O p t i o n V a l u e R e s t r i c t e d                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetOptionValueRestricted searchs the value in the a list of predefined
%  values. If a match is found, it sets variable pointed by the result pointer
%  and return OptionSuccess when the value is one. Otherwise, print error
%  and returns OPtionInvalidValue.
%
%  The format of the GetOptionValueRestricted method is:
%
%      OptionStatus GetOptionValueRestricted(const char *option, char **values,
%        const char *value, int *result)
%
%  A description of each parameter follows:
%
%    o option: The option to get the value.
%
%    o value: The option value to be checked.
%
%    o values: The predefined set of acceptable values.
%
%    o result: Points to the variable to be set to value.
%
*/
static OptionStatus GetOptionValueRestricted(const char *option,
  const char **values, const char *value, int *result)
{
  int i;
  OptionStatus status = CheckOptionValue(option, value);
  if (status != OptionSuccess)
    return status;
  for (i = 0; values[i] != (char *) NULL; i++)
    {
      if (LocaleCompare(values[i], value) == 0)
        {
          *result = i;
          return OptionSuccess;
        }
    }
  fprintf(stderr, "Error: Invalid value for %s option: %s\n", option, value);
  return OptionInvalidValue;
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t O n O f f O p t i o n                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetOnOffOptionValue expectes the value to be either "on" or "off". It then
%  sets the corresponding value to the boolean variable pointed by the result
%  pointer and return OptionSuccess. Otherwise, print error and returns
%  OPtionInvalidValue.
%
%  The format of the GetOnOffOptionValue method is:
%
%      OptionStatus GetOnOffOptionValue(const char *option,
%        const char *value, int *result)
%
%  A description of each parameter follows:
%
%    o option: The option to get the value.
%
%    o value: The value to be checked.
%
%    o result: Points to the variable to accept the result.
%
*/
static OptionStatus GetOnOffOptionValue(const char *option, const char *value,
  MagickBool *result)
{
  int i;
  OptionStatus status = GetOptionValueRestricted(option, on_off_option_values, value, &i);
  if (status != OptionSuccess)
    return status;
  *result = i;
  return OptionSuccess;
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G M U s a g e                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GMUsage() displays the gm program command syntax.
%
%  The format of the GMUsage method is:
%
%      void GMUsage()
%
%
*/
static void GMUsage(void)
{
  int
    i;

  PrintUsageHeader();
  (void) printf("Usage: %.1024s command [options ...]\n",GetClientName());
  (void) printf("\nWhere commands include: \n");
      
  for (i=0; commands[i].command != 0; i++)
    {
      if (commands[i].support_mode & run_mode)
        (void) printf("%11s - %s\n",commands[i].command,
                      commands[i].description);
    }

  return;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    H e l p C o m m a n d                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  HelpCommand() prints a usage message for gm, or for a gm subcommand.
%
%  The format of the HelpCommand method is:
%
%      unsigned int HelpCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int HelpCommand(ImageInfo *ARGUNUSED(image_info),
  int argc,char **argv,char **ARGUNUSED(metadata),
  ExceptionInfo *ARGUNUSED(exception))
{
  if (argc > 1)
    {
      int
        i;
      
      for (i=0; commands[i].command != 0; i++)
        {
          if (!(commands[i].support_mode & run_mode))
            continue;
          if (LocaleCompare(commands[i].command,argv[1]) == 0)
            {
              (void) SetClientName(commands[i].command);
              if (commands[i].usage_vector)
                {
                  (commands[i].usage_vector)();
                  return True;
                }
            }
        }
    }
  
  GMUsage();

  return MagickPass;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I d e n t i f y I m a g e C o m m a n d                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IdentifyImageCommand() describes the format and characteristics of one or
%  more image files. It will also report if an image is incomplete or corrupt.
%  The information displayed includes the scene number, the file name, the
%  width and height of the image, whether the image is colormapped or not,
%  the number of colors in the image, the number of bytes in the image, the
%  format of the image (JPEG, PNM, etc.), and finally the number of seconds
%  it took to read and process the image.
%
%  The format of the IdentifyImageCommand method is:
%
%      unsigned int IdentifyImageCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

#define ThrowIdentifyException(code,reason,description) \
{ \
  status = MagickFail; \
  ThrowException(exception,code,reason,description); \
  goto identify_cleanup_and_return; \
}
#define ThrowIdentifyException3(code,reason,description) \
{ \
  status = MagickFail; \
  ThrowException3(exception,code,reason,description); \
  goto identify_cleanup_and_return; \
}

MagickExport unsigned int IdentifyImageCommand(ImageInfo *image_info,
  int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
  char
    *format = NULL,
    *option = NULL,
    *q = NULL;

  Image
    *image;

  long
    count,
    number_images,
    x;

  register Image
    *p;

  register long
    i;

  unsigned int
    ping,
    status = 0;

  /*
    Check for sufficient arguments
  */  
  if (argc < 2 || ((argc < 3) && (LocaleCompare("-help",argv[1]) == 0 ||
      LocaleCompare("-?",argv[1]) == 0)))
    {
      IdentifyUsage();
      if (argc < 2)
	{
	  ThrowException(exception,OptionError,UsageError,NULL);
	  return MagickFail;
	}
      return MagickPass;
    }
  if (LocaleCompare("-version",argv[1]) == 0)
    {
      (void) VersionCommand(image_info,argc,argv,metadata,exception);
      return MagickPass;
    }

  /*
    Set defaults.
  */
  count=0;
  format=(char *) NULL;
  image=NewImageList();
  number_images=0;
  status=True;
  ping=True;

  /*
    Expand argument list
  */
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickFatalError(ResourceLimitFatalError,MemoryAllocationFailed,
      (char *) NULL);

  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if (LocaleCompare("-format",argv[i]) == 0)
      {
        i++;
        if (i == argc)
          ThrowIdentifyException(OptionError,MissingArgument,option);
        (void) CloneString(&format,argv[i]);
        break;
      }
    else if (LocaleCompare("+ping",argv[i]) == 0)
      {
        ping=MagickFalse;
      }
  }
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) < 2) ||
	/* stdin + subexpression */
	((option[0] == '-') && (option[1] == '[')) ||
	((option[0] != '-') && option[0] != '+'))
      {
        /*
          Identify image.
        */
        (void) strlcpy(image_info->filename,argv[i],MaxTextExtent);
        if (format != (char *) NULL)
          for (q=strchr(format,'%'); q != (char *) NULL; q=strchr(q+1,'%'))
            {
              const char c=*(q+1);
              if ((c == 'A') || (c == 'k') || (c == 'q') || (c == 'r') ||
                  (c == '#'))
                {
                  ping=False;
                  break;
                }
            }
        if (image_info->verbose || !ping)
          image=ReadImage(image_info,exception);
        else
          image=PingImage(image_info,exception);
        status&=image != (Image *) NULL;
        if (image == (Image *) NULL)
	  {
	    CatchException(exception);
	    DestroyExceptionInfo(exception);
	    GetExceptionInfo(exception);
	    continue;
	  }
        for (p=image; p != (Image *) NULL; p=p->next)
        {
          if (p->scene == 0)
            p->scene=count++;
          if (format == (char *) NULL)
            {
              (void) DescribeImage(p,stdout,image_info->verbose);
              continue;
            }
          if (metadata != (char **) NULL)
            {
              char
                *text;

              text=TranslateText(image_info,p,format);
              if (text == (char *) NULL)
                ThrowIdentifyException(ResourceLimitError,
                  MemoryAllocationFailed,
                    MagickMsg(OptionError,UnableToFormatImageMetadata));
              (void) ConcatenateString(&(*metadata),text);
/*               (void) ConcatenateString(&(*metadata),"\n"); */
              MagickFreeMemory(text);
            }
        }
        DestroyImageList(image);
        image=NewImageList();
        number_images++;
        continue;
      }
    switch(*(option+1))
    {
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            (void) SetLogEventMask("None");
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowIdentifyException(OptionError,MissingArgument,option);
                (void) SetLogEventMask(argv[i]);
              }
            break;
          }
        if (LocaleCompare("define",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowIdentifyException(OptionError,MissingArgument,option);
            if (*option == '+')
              (void) RemoveDefinitions(image_info,argv[i]);
            else
              (void) AddDefinitions(image_info,argv[i],exception);
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            (void) CloneString(&image_info->density,(char *) NULL);
            if (*option == '-')
              {
              i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowIdentifyException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->density,argv[i]);
              }
            break;
          }
        if (LocaleCompare("depth",option+1) == 0)
          {
            image_info->depth=QuantumDepth;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowIdentifyException(OptionError,MissingArgument,
                    option);
                image_info->depth=MagickAtoL(argv[i]);
              }
            break;
          }
        ThrowIdentifyException(OptionError,UnrecognizedOption,option)
      }
      case 'f':
      {
        if (LocaleCompare("format",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowIdentifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        ThrowIdentifyException(OptionError,UnrecognizedOption,option)
      }
      case 'h':
      {
        if (LocaleCompare("help",option+1) == 0)
          break;
        ThrowIdentifyException(OptionError,UnrecognizedOption,option)
      }
      case 'i':
      {
        if (LocaleCompare("interlace",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowIdentifyException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->interlace=StringToInterlaceType(option);
                if (image_info->interlace == UndefinedInterlace)
                  ThrowIdentifyException(OptionError,UnrecognizedInterlaceType,option);
              }
            break;
          }
        ThrowIdentifyException(OptionError,UnrecognizedOption,option)
      }
      case 'l':
      {
        if (LocaleCompare("limit",option+1) == 0)
          {
            if (*option == '-')
              {
                ResourceType
                  resource_type;

                char
                  *type;

                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                type=argv[i];
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_type=StringToResourceType(type);
                if (resource_type == UndefinedResource)
                  MagickFatalError(OptionFatalError,UnrecognizedResourceType,type);
                (void) SetMagickResourceLimit(resource_type,MagickSizeStrToInt64(argv[i],1024));
              }
            break;
          }
        if (LocaleCompare("log",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowIdentifyException(OptionError,MissingArgument,option);
                (void) SetLogFormat(argv[i]);
              }
            break;
          }
        ThrowIdentifyException(OptionError,UnrecognizedOption,option)
      }
      case 'm':
      {
        if (LocaleCompare("monitor",option+1) == 0)
          {
            if (*option == '+')
	      {
		(void) SetMonitorHandler((MonitorHandler) NULL);
		(void) MagickSetConfirmAccessHandler((ConfirmAccessHandler) NULL);
	      }
            else
	      {
		(void) SetMonitorHandler(CommandProgressMonitor);
		(void) MagickSetConfirmAccessHandler(CommandAccessMonitor);
	      }
            break;
          }
        ThrowIdentifyException(OptionError,UnrecognizedOption,option)
      }
      case 'p':
      {
        if (LocaleCompare("ping",option+1) == 0)
          break;
        ThrowIdentifyException(OptionError,UnrecognizedOption,option)
      }
      case 's':
      {
        if (LocaleCompare("sampling-factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowIdentifyException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
                NormalizeSamplingFactor(image_info);
              }
            break;
          }
        if (LocaleCompare("size",option+1) == 0)
          {
            (void) CloneString(&image_info->size,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowIdentifyException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->size,argv[i]);
              }
            break;
          }
        ThrowIdentifyException(OptionError,UnrecognizedOption,option)
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose+=(*option == '-');
            break;
          }
        if (LocaleCompare("verbose",option+1) == 0)
          break;
        if (LocaleCompare("virtual-pixel",option+1) == 0)
          {
            if (*option == '-')
              {
                VirtualPixelMethod
                  virtual_pixel_method;

                i++;
                if (i == argc)
                  ThrowIdentifyException(OptionError,MissingArgument,option);
                option=argv[i];
                virtual_pixel_method=StringToVirtualPixelMethod(option);
                if (virtual_pixel_method == UndefinedVirtualPixelMethod)
                  ThrowIdentifyException(OptionError,UnrecognizedVirtualPixelMethod,option);
              }
            break;
          }
        ThrowIdentifyException(OptionError,UnrecognizedOption,option)
      }
      case '?':
        break;
      default:
        ThrowIdentifyException(OptionError,UnrecognizedOption,option)
    }
  }
  if (number_images == 0)
    {
      if (exception->severity == UndefinedException)
        ThrowIdentifyException(OptionError,RequestDidNotReturnAnImage,
          (char *) NULL);
      status = MagickFail;
    }
  if (i != argc)
    ThrowIdentifyException(OptionError,MissingAnImageFilename,(char *) NULL);
 identify_cleanup_and_return:
  MagickFreeMemory(format);
  DestroyImageList(image);
  LiberateArgumentList(argc,argv);
  return(status);
}
#undef ThrowIdentifyException
#undef ThrowIdentifyException3

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I d e n t i f y U s a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  IdentifyUsage() displays the program command syntax.
%
%  The format of the IdentifyUsage method is:
%
%      void IdentifyUsage()
%
%
*/
static void IdentifyUsage(void)
{
  const char
    **p;

  static const char
    *options[]=
    {
      "-debug events        display copious debugging information",
      "-define values       Coder/decoder specific options",
      "-density geometry    horizontal and vertical density of the image",
      "-depth value         image depth",
      "-format \"string\"   output formatted image characteristics",
      "-help                print program options",
      "-interlace type      None, Line, Plane, or Partition",
      "-limit type value    Disk, File, Map, Memory, Pixels, Width, Height or",
      "                     Threads resource limit",
      "-log format          format of debugging information",
      "-monitor             show progress indication",
      "-ping                efficiently determine image attributes",
      "-sampling-factor HxV[,...]",
      "                     horizontal and vertical sampling factors",
      "-size geometry       width and height of image",
      "-verbose             print detailed information about the image",
      "-version             print version information",
      "-virtual-pixel method",
      "                     Constant, Edge, Mirror, or Tile",
      (char *) NULL
    };

  PrintUsageHeader();
  (void) printf("Usage: %.1024s [options ...] file [ [options ...] "
    "file ... ]\n",GetClientName());
  (void) printf("\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n i t i a l i z e B a t c h O p t i o n s                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InitializeBatchOptions initializes the default batch options.
%
%  The format of the InitializeBatchOptions method is:
%
%      void InitializeBatchOptions(MagickBool prompt)
%
%  A description of each parameter follows:
%
%    o prompt: whether enable prompt or not
%
*/
static void InitializeBatchOptions(MagickBool prompt)
{
  strcpy(batch_options.pass, "PASS");
  strcpy(batch_options.fail, "FAIL");
#if defined(MSWINDOWS)
  batch_options.command_line_parser = ParseWindowsCommandLine;
#else
  batch_options.command_line_parser = ParseUnixCommandLine;
#endif
  if (prompt)
    strcpy(batch_options.prompt, "GM> ");
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   L i b e r a t e A r g u m e n t L i s t                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  LiberateArgumentList() deallocates the argument list allocated by
%  ExpandFilenames().
%
%  The format of the LiberateArgumentList method is:
%
%      void LiberateArgumentList(const int argc,char **argv)
%
%  A description of each parameter follows:
%
%    o argc: number of arguments in argument vector array
%
%    o argv: argument vector array
%
*/
static void LiberateArgumentList(const int argc,char **argv)
{
  int
    i;

  for (i=0; i< argc; i++)
    MagickFreeMemory(argv[i]);
  MagickFreeMemory(argv);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M a g i c k C o m m a n d                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MagickCommand() invokes a GraphicsMagick utility subcommand based
%  on the first argument supplied in the argument vector.
%
%  The format of the MagickCommand method is:
%
%      unsigned int MagickCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int MagickCommand(ImageInfo *image_info,
  int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
  char
    *option;

  unsigned int
    status=False;

  int
    i;

  option=argv[0];
  if (option[0] == '-')
    option++;

  for (i=0; commands[i].command != 0; i++)
    {
      if (!(commands[i].support_mode & run_mode))
        continue;
      if (LocaleCompare(commands[i].command,option) == 0)
        {
          char
            command_name[MaxTextExtent];

	  const char
	    *pos;

          /*
            Append subcommand name to existing client name if end of
            existing client name is not identical to subcommand name.
          */
          LockSemaphoreInfo(command_semaphore);
          if (run_mode == BatchMode)
            (void) SetClientName(commands[i].command);
          else
            {
              GetPathComponent(GetClientName(),BasePath,command_name);
              pos=strrchr(command_name,' ');
              if ((pos == (const char *) NULL) ||
                  (LocaleCompare(commands[i].command,pos+1) != 0))
                {
                  char
                    client_name[MaxTextExtent];
                    
                  FormatString(client_name,"%.1024s %s",GetClientName(),
                               commands[i].command);
                  
                  (void) SetClientName(client_name);
                }
            }
          UnlockSemaphoreInfo(command_semaphore);

          return(commands[i].command_vector)(image_info,argc,argv,
            commands[i].pass_metadata ? metadata : (char **) NULL,exception);
        }
    }
  ThrowException(exception,OptionError,UnrecognizedCommand,option);
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k D e s t r o y C o m m a n d I n f o                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickDestroyCommandInfo deallocates memory associated with the
%  command parser.
%
%  The format of the MagickDestroyCommandInfo method is:
%
%      void MagickDestroyCommandInfo(void)
%
%
*/
void MagickDestroyCommandInfo(void)
{
  DestroySemaphoreInfo(&command_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   M a g i c k I n i t i a l i z e C o m m a n d I n f o                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MagickInitializeCommandInfo initializes the command parsing
%  facility.
%
%  The format of the MagickInitializeCommandInfo method is:
%
%      MagickPassFail MagickInitializeCommandInfo(void)
%
%
*/
MagickPassFail
MagickInitializeCommandInfo(void)
{
  assert(command_semaphore == (SemaphoreInfo *) NULL);
  command_semaphore=AllocateSemaphoreInfo();
  return MagickPass;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
+     M o g r i f y I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MogrifyImage applies image processing options to an image as
%  prescribed by command line options.
%
%  The format of the MogrifyImage method is:
%
%      unsigned int MogrifyImage(const ImageInfo *image_info,const int argc,
%        char **argv,Image **image)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.
%
%    o image: The image;  returned from
%      ReadImage.
%
%
*/
MagickExport unsigned int MogrifyImage(const ImageInfo *image_info,
  int argc,char **argv,Image **image)
{
  char
    *option;

  DrawInfo
    *draw_info;

  Image
    *region_image;

  ImageInfo
    *clone_info;

  int
    count;

  QuantizeInfo
    quantize_info;

  RectangleInfo
    geometry,
    region_geometry;

  register long
    i;

  unsigned int
    matte;

  /*
    Verify option length.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image **) NULL);
  assert((*image)->signature == MagickSignature);
  if (argc <= 0)
    return(True);

#if defined(DEBUG_COMMAND_PARSER)
  fprintf(stderr,"  MogrifyImage (0x%p->%s):", *image, (*image)->filename);
  for (i=0; i < argc; i++)
    fprintf(stderr," %s",argv[i]);
  fprintf(stderr,"\n");
#endif /* DEBUG_COMMAND_PARSER */

  for (i=0; i < argc; i++)
    if (strlen(argv[i]) > (MaxTextExtent/2-1))
      MagickFatalError(OptionFatalError,OptionLengthExceedsLimit,argv[i]);
  /*
    Initialize method variables.
  */
  clone_info=CloneImageInfo(image_info);
  draw_info=CloneDrawInfo(clone_info,(DrawInfo *) NULL);
  GetQuantizeInfo(&quantize_info);
  quantize_info.number_colors=0;
  quantize_info.tree_depth=0;
  quantize_info.dither=MagickTrue;
  SetGeometry(*image,&region_geometry);
  region_image=(Image *) NULL;
  /*
    Transmogrify the image.
  */
  for (i=0; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) <= 1) || ((option[0] != '-') && (option[0] != '+')))
      continue;
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("affine",option+1) == 0)
          {
            char
              *p;

            /*
              Draw affine matrix.
            */
            if (*option == '+')
              {
                IdentityAffine(&draw_info->affine);
                continue;
              }
            p=argv[++i];
            draw_info->affine.sx=strtod(p,&p);
            if (*p ==',')
              p++;
            draw_info->affine.rx=strtod(p,&p);
            if (*p ==',')
              p++;
            draw_info->affine.ry=strtod(p,&p);
            if (*p ==',')
              p++;
            draw_info->affine.sy=strtod(p,&p);
            if (*p ==',')
              p++;
            draw_info->affine.tx=strtod(p,&p);
            if (*p ==',')
              p++;
            draw_info->affine.ty=strtod(p,&p);
            break;
          }
        if (LocaleCompare("antialias",option+1) == 0)
          {
            clone_info->antialias=(*option == '-');
            draw_info->stroke_antialias=(*option == '-');
            draw_info->text_antialias=(*option == '-');
            break;
          }
	if (LocaleCompare("asc-cdl",option+1) == 0)
          {
	    ++i;
	    (void) CdlImage(*image,argv[i]);
	    continue;
	  }
        if (LocaleCompare("auto-orient",option+1) == 0)
          {
            Image
              *orient_image;

            /*
              Auto orient (upright) image scanlines.
            */
            orient_image=AutoOrientImage(*image,(*image)->orientation,
                                         &(*image)->exception);
            if (orient_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=orient_image;
            continue;
          }        break;
      }
      case 'b':
      {
        if (LocaleCompare("background",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&clone_info->background_color,
              &(*image)->exception);
            (*image)->background_color=clone_info->background_color;
            continue;
          }
        if (LocaleCompare("blue-primary",option+1) == 0)
          {
            /*
              Blue chromaticity primary point.
            */
            if (*option == '+')
              {
                (*image)->chromaticity.blue_primary.x=0.0;
                (*image)->chromaticity.blue_primary.y=0.0;
                continue;
              }
            (void) sscanf(argv[++i],"%lf%*[,/]%lf",
              &(*image)->chromaticity.blue_primary.x,
              &(*image)->chromaticity.blue_primary.y);
            continue;
          }
        if (LocaleCompare("black-threshold",option+1) == 0)
          {
            /*
              Black threshold image.
            */
            ++i;
            (void) BlackThresholdImage(*image,argv[i]);
            continue;
          }
        if (LocaleCompare("blur",option+1) == 0)
          {
            double
              radius,
              sigma;

            Image
              *blur_image;

            /*
              Gaussian blur image.
            */
            radius=0.0;
            sigma=1.0;
            (void) GetMagickDimension(argv[++i],&radius,&sigma,NULL,NULL);
            blur_image=BlurImage(*image,radius,sigma,&(*image)->exception);
            if (blur_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=blur_image;
            continue;
          }
        if (LocaleCompare("border",option+1) == 0)
          {
            Image
              *border_image;

            /*
              Surround image with a border of solid color.
            */
            (void) GetImageGeometry(*image,argv[++i],False,&geometry);
            border_image=BorderImage(*image,&geometry,&(*image)->exception);
            if (border_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=border_image;
            continue;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&clone_info->border_color,
              &(*image)->exception);
            draw_info->border_color=clone_info->border_color;
            (*image)->border_color=clone_info->border_color;
            continue;
          }
        if (LocaleCompare("box",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&draw_info->undercolor,
              &(*image)->exception);
            continue;
          }
        break;
      }
      case 'c':
      {
        if (LocaleCompare("channel",option+1) == 0)
          {
            ChannelType
              channel;

            channel=StringToChannelType(argv[++i]);
            if (clone_info->colorspace != UndefinedColorspace)
              (void) TransformColorspace(*image,clone_info->colorspace);
            (void) ChannelImage(*image,channel);
            continue;
          }
        if (LocaleCompare("charcoal",option+1) == 0)
          {
            double
              radius,
              sigma;

            Image
              *charcoal_image;

            /*
              Charcoal image.
            */
            radius=0.0;
            sigma=1.0;
            (void) GetMagickDimension(argv[++i],&radius,&sigma,NULL,NULL);
            charcoal_image=
              CharcoalImage(*image,radius,sigma,&(*image)->exception);
            if (charcoal_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=charcoal_image;
            continue;
          }
        if (LocaleCompare("chop",option+1) == 0)
          {
            Image
              *chop_image;

            /*
              Chop the image.
            */
            (void) GetImageGeometry(*image,argv[++i],False,&geometry);
            chop_image=ChopImage(*image,&geometry,&(*image)->exception);
            if (chop_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=chop_image;
            continue;
          }
        if (LocaleCompare("clip",option+1) == 0)
          {
            if (*option == '+')
              {
                (void) SetImageClipMask(*image,(Image *) NULL);
                continue;
              }
            (void) ClipImage(*image);
            continue;
          }
        if (LocaleCompare("clippath",option+1) == 0)
          {
            (void) ClipPathImage(*image,argv[++i],*option == '-');
            continue;
          }
        if (LocaleCompare("colorize",option+1) == 0)
          {
            Image
              *colorize_image;

            /*
              Colorize the image.
            */
            colorize_image=ColorizeImage(*image,argv[++i],draw_info->fill,
              &(*image)->exception);
            if (colorize_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=colorize_image;
            continue;
          }
        if (LocaleCompare("colors",option+1) == 0)
          {
            quantize_info.number_colors=MagickAtoL(argv[++i]);

	    if ( IsGrayColorspace(quantize_info.colorspace) )
	      {
		/*
		  If color reduction is requested, then quantize to the requested
		  number of colors in the gray colorspace, otherwise simply
		  transform the image to the gray colorspace.
		*/
		if ( quantize_info.number_colors != 0 )
		  (void) QuantizeImage(&quantize_info,*image);
		else
		  (void) TransformColorspace(*image,quantize_info.colorspace);
	      }
	    else
	      {
		/*
		  If color reduction is requested, and the image is DirectClass,
		  or the image is PseudoClass and the number of colors exceeds
		  the number requested, then quantize the image colors. Otherwise
		  compress an existing colormap.
		*/
		if ( quantize_info.number_colors != 0 )
		  {
		    if (((*image)->storage_class == DirectClass) ||
			((*image)->colors > quantize_info.number_colors))
		      (void) QuantizeImage(&quantize_info,*image);
		    else
		      CompressImageColormap(*image);
		  }
	      }

	    quantize_info.number_colors=0;
	    
            continue;
          }
        if (LocaleCompare("colorspace",option+1) == 0)
          {
            char
              type;

            ColorspaceType
              colorspace;

            type=(*option);
            option=argv[++i];
            colorspace=StringToColorspaceType(option);
            quantize_info.colorspace=colorspace;
            /* Never quantize in CMYK colorspace */
            if (IsCMYKColorspace(colorspace))
              quantize_info.colorspace=RGBColorspace;
            (void) TransformColorspace(*image,colorspace);
            clone_info->colorspace=colorspace;
            if (type == '+')
              (*image)->colorspace=colorspace;
            continue;
          }
        if (LocaleCompare("comment",option+1) == 0)
          {
            (void) SetImageAttribute(*image,"comment",(char *) NULL);
            if (*option == '-')
              (void) SetImageAttribute(*image,"comment",argv[++i]);
            continue;
          }
        if (LocaleCompare("compose",option+1) == 0)
          {
	    (*image)->compose=CopyCompositeOp;
	    if (*option == '-')
	      (*image)->compose=StringToCompositeOperator(argv[++i]);
            continue;
          }
        if (LocaleCompare("compress",option+1) == 0)
          {
            if (*option == '+')
              {
                (*image)->compression=UndefinedCompression;
                continue;
              }
            option=argv[++i];
            (*image)->compression=StringToCompressionType(option);
            continue;
          }
        if (LocaleCompare("contrast",option+1) == 0)
          {
            (void) ContrastImage(*image,*option == '-');
            continue;
          }
        if (LocaleCompare("convolve",option+1) == 0)
          {
            Image
              *convolve_image;

            char
              *p,
              token[MaxTextExtent];

            double
              *kernel;

            register long
              x;

            unsigned int
	      elements,
              order;

            /*
              Convolve image.
            */
            p=argv[++i];
            for (elements=0; *p != '\0'; elements++)
            {
              MagickGetToken(p,&p,token,MaxTextExtent);
	      if (token[0] == '\0')
		break;
              if (*token == ',')
                MagickGetToken(p,&p,token,MaxTextExtent);
	      if (token[0] == '\0')
		break;
            }
            order=(unsigned int) sqrt(elements);
	    if ((0 == elements) || (order*order != elements))
	      {
		char
		  message[MaxTextExtent];

		FormatString(message,"%u",elements);
		ThrowException(&(*image)->exception,OptionError,MatrixIsNotSquare,message);
		continue;
	      }
            kernel=MagickAllocateArray(double *,order*order,sizeof(double));
            if (kernel == (double *) NULL)
              MagickFatalError(ResourceLimitFatalError,MemoryAllocationFailed,
                MagickMsg(ResourceLimitError,UnableToAllocateCoefficients));
            p=argv[i];
            for (x=0; *p != '\0'; x++)
            {
              MagickGetToken(p,&p,token,MaxTextExtent);
	      if (token[0] == '\0')
		break;
              if (token[0] == ',')
                MagickGetToken(p,&p,token,MaxTextExtent);
	      if (token[0] == '\0')
		break;
              kernel[x]=MagickAtoF(token);
            }
            for ( ; x < (long) (order*order); x++)
              kernel[x]=0.0;
            convolve_image=ConvolveImage(*image,order,kernel,
              &(*image)->exception);
            MagickFreeMemory(kernel);
	    if (convolve_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=convolve_image;
            continue;
          }
        if (LocaleCompare("crop",option+1) == 0)
          {
	    /*
	      FIXME: This command can produce multiple images from one
	      image, causing only the first to be fully processed.
	    */
            TransformImage(image,argv[++i],(char *) NULL);
            continue;
          }
        if (LocaleCompare("cycle",option+1) == 0)
          {
            /*
              Cycle an image colormap.
            */
            (void) CycleColormapImage(*image,MagickAtoI(argv[++i]));
            continue;
          }
        break;
      }
      case 'd':
      {
        if (LocaleCompare("define",option+1) == 0)
          {
            i++;
            if (*option == '+')
              (void) RemoveDefinitions(clone_info,argv[i]);
            else
              (void) AddDefinitions(clone_info,argv[i],&(*image)->exception);
            break;
          }
        if (LocaleCompare("delay",option+1) == 0)
          {
            double
              maximum_delay,
              minimum_delay;

            /*
              Set image delay.
            */
            if (*option == '+')
              {
                (*image)->delay=0;
                continue;
              }
            count=sscanf(argv[++i],"%lf-%lf",&minimum_delay,&maximum_delay);
            if (count == 1)
              (*image)->delay=(unsigned long) minimum_delay;
            else
              {
                if ((*image)->delay < minimum_delay)
                  (*image)->delay=(unsigned long) minimum_delay;
                if ((*image)->delay > maximum_delay)
                  (*image)->delay=(unsigned long) maximum_delay;
              }
            continue;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            /*
              Set image density.
            */
            (void) CloneString(&clone_info->density,argv[++i]);
            (void) CloneString(&draw_info->density,clone_info->density);
            count=GetMagickDimension(clone_info->density,
                                     &(*image)->x_resolution,
                                     &(*image)->y_resolution,NULL,NULL);
            if (count != 2)
              (*image)->y_resolution=(*image)->x_resolution;
            continue;
          }
        if (LocaleCompare("depth",option+1) == 0)
          {
            /* (*image)->depth = MagickAtoL(argv[++i]); */
            (void) SetImageDepth(*image,MagickAtoL(argv[++i]));
            continue;
          }
        if (LocaleCompare("despeckle",option+1) == 0)
          {
            Image
              *despeckle_image;

            /*
              Reduce the speckles within an image.
            */
            despeckle_image=DespeckleImage(*image,&(*image)->exception);
            if (despeckle_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=despeckle_image;
            continue;
          }
        if (LocaleCompare("display",option+1) == 0)
          {
            (void) CloneString(&clone_info->server_name,argv[++i]);
            (void) CloneString(&draw_info->server_name,clone_info->server_name);
            continue;
          }
        if (LocaleCompare("dispose",option+1) == 0)
          {
            DisposeType
              dispose;;

            if (*option == '+')
              {
                (*image)->dispose=UndefinedDispose;
                continue;
              }
            option=argv[++i];
            dispose=UndefinedDispose;
            if (LocaleCompare("0",option) == 0)
              dispose=UndefinedDispose;
            if (LocaleCompare("1",option) == 0)
              dispose=NoneDispose;
            if (LocaleCompare("2",option) == 0)
              dispose=BackgroundDispose;
            if (LocaleCompare("3",option) == 0)
              dispose=PreviousDispose;
            if (LocaleCompare("Background",option) == 0)
              dispose=BackgroundDispose;
            if (LocaleCompare("None",option) == 0)
              dispose=NoneDispose;
            if (LocaleCompare("Previous",option) == 0)
              dispose=PreviousDispose;
            if (LocaleCompare("Undefined",option) == 0)
              dispose=UndefinedDispose;
            (*image)->dispose=dispose;
            continue;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            clone_info->dither=(*option == '-');
            quantize_info.dither=clone_info->dither;
	    (*image)->dither=quantize_info.dither;
            continue;
          }
        if (LocaleCompare("draw",option+1) == 0)
          {
            /*
              Draw image.
            */
            (void) CloneString(&draw_info->primitive,argv[++i]);
            (void) DrawImage(*image,draw_info);
            continue;
          }
        break;
      }
      case 'e':
      {
        if (LocaleCompare("edge",option+1) == 0)
          {
            double
              radius;

            Image
              *edge_image;

            /*
              Enhance edges in the image.
            */
            radius=MagickAtoF(argv[++i]);
            edge_image=EdgeImage(*image,radius,&(*image)->exception);
            if (edge_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=edge_image;
            continue;
          }
        if (LocaleCompare("emboss",option+1) == 0)
          {
            double
              radius,
              sigma;

            Image
              *emboss_image;

            /*
              Emboss the image.
            */
            radius=0.0;
            sigma=1.0;
            (void) GetMagickDimension(argv[++i],&radius,&sigma,NULL,NULL);
            emboss_image=EmbossImage(*image,radius,sigma,&(*image)->exception);
            if (emboss_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=emboss_image;
            continue;
          }
        if (LocaleCompare("encoding",option+1) == 0)
          {
            (void) CloneString(&draw_info->encoding,argv[++i]);
            continue;
          }
        if (LocaleCompare("endian",option+1) == 0)
          {
            if (*option == '+')
              {
                clone_info->endian=NativeEndian;
                continue;
              }
            option=argv[++i];
            clone_info->endian=StringToEndianType(option);
            continue;
          }
        if (LocaleCompare("enhance",option+1) == 0)
          {
            Image
              *enhance_image;

            /*
              Enhance image.
            */
            enhance_image=EnhanceImage(*image,&(*image)->exception);
            if (enhance_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=enhance_image;
            continue;
          }
        if (LocaleCompare("equalize",option+1) == 0)
          {
            /*
              Equalize image.
            */
            (void) EqualizeImage(*image);
            continue;
          }
        if (LocaleCompare("extent",option+1) == 0)
          {
	    Image
              *extent_image;

            /*
              Extent image.
            */
            (void) GetImageGeometry(*image,argv[++i],MagickFalse,&geometry);
            if (geometry.width == 0)
              geometry.width=(*image)->columns;
            if (geometry.height == 0)
              geometry.height=(*image)->rows;
            geometry.x=(-geometry.x);
            geometry.y=(-geometry.y);
            extent_image=ExtentImage(*image,&geometry,&(*image)->exception);
            if (extent_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=extent_image;
            continue;
	  }
        break;
      }
      case 'f':
      {
        if (LocaleCompare("fill",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&draw_info->fill,
              &(*image)->exception);
            continue;
          }
        if (LocaleCompare("filter",option+1) == 0)
          {
            FilterTypes
              filter;

            if (*option == '+')
              {
                (*image)->filter=DefaultResizeFilter;
                continue;
              }
            option=argv[++i];
            filter=StringToFilterTypes(option);
            (*image)->filter=filter;
            continue;
          }
        if (LocaleCompare("flip",option+1) == 0)
          {
            Image
              *flip_image;

            /*
              Flip image scanlines.
            */
            flip_image=FlipImage(*image,&(*image)->exception);
            if (flip_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=flip_image;
            continue;
          }
        if (LocaleCompare("flop",option+1) == 0)
          {
            Image
              *flop_image;

            /*
              Flop image scanlines.
            */
            flop_image=FlopImage(*image,&(*image)->exception);
            if (flop_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=flop_image;
            continue;
          }
        if (LocaleCompare("frame",option+1) == 0)
          {
            Image
              *frame_image;

            FrameInfo
              frame_info;

            /*
              Surround image with an ornamental border.
            */
            (void) GetImageGeometry(*image,argv[++i],False,&geometry);
            frame_info.width=geometry.width;
            frame_info.height=geometry.height;
            frame_info.outer_bevel=geometry.x;
            frame_info.inner_bevel=geometry.y;
            frame_info.x=(long) frame_info.width;
            frame_info.y=(long) frame_info.height;
            frame_info.width=(*image)->columns+2*frame_info.width;
            frame_info.height=(*image)->rows+2*frame_info.height;
            frame_image=FrameImage(*image,&frame_info,&(*image)->exception);
            if (frame_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=frame_image;
            continue;
          }
        if (LocaleCompare("fuzz",option+1) == 0)
          {
            (*image)->fuzz=StringToDouble(argv[++i],MaxRGB);
            continue;
          }
        if (LocaleCompare("font",option+1) == 0)
          {
            (void) CloneString(&clone_info->font,argv[++i]);
            (void) CloneString(&draw_info->font,clone_info->font);
            continue;
          }
        break;
      }
      case 'g':
      {
        if (LocaleCompare("gamma",option+1) == 0)
          {
            if (*option == '+')
              (*image)->gamma=MagickAtoF(argv[++i]);
            else
              (void) GammaImage(*image,argv[++i]);
            continue;
          }
        if ((LocaleCompare("gaussian",option+1) == 0) ||
            (LocaleCompare("gaussian-blur",option+1) == 0))
          {
            double
              radius,
              sigma;

            Image
              *blur_image;

            /*
              Gaussian blur image.
            */
            radius=0.0;
            sigma=1.0;
            (void) GetMagickDimension(argv[++i],&radius,&sigma,NULL,NULL);
            blur_image=
              GaussianBlurImage(*image,radius,sigma,&(*image)->exception);
            if (blur_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=blur_image;
            continue;
          }
        if (LocaleCompare("geometry",option+1) == 0)
          {
            Image
              *zoom_image;

            /*
              Resize image.
            */
            (void) GetImageGeometry(*image,argv[++i],True,&geometry);
            if ((geometry.width == (*image)->columns) &&
                (geometry.height == (*image)->rows))
              break;
            zoom_image=ZoomImage(*image,geometry.width,geometry.height,
              &(*image)->exception);
            if (zoom_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=zoom_image;
            continue;
          }
        if (LocaleCompare("gravity",option+1) == 0)
          {
            GravityType
              gravity;

            if (*option == '+')
              {
                draw_info->gravity=(GravityType) ForgetGravity;
                (*image)->gravity=(GravityType) ForgetGravity;
                continue;
              }
            option=argv[++i];
	    gravity=StringToGravityType(option);
            draw_info->gravity=gravity;
            (*image)->gravity=gravity;
            continue;
          }
        if (LocaleCompare("green-primary",option+1) == 0)
          {
            /*
              Green chromaticity primary point.
            */
            if (*option == '+')
              {
                (*image)->chromaticity.green_primary.x=0.0;
                (*image)->chromaticity.green_primary.y=0.0;
                continue;
              }
            (void) sscanf(argv[++i],"%lf%*[,/]%lf",
              &(*image)->chromaticity.green_primary.x,
              &(*image)->chromaticity.green_primary.y);
            continue;
          }
        break;
      }
      case 'h':
      {
	if (LocaleCompare("hald-clut",option+1) == 0)
          {
	    Image
	      *clut_image;
			  
	    (void) strlcpy(clone_info->filename,argv[++i],MaxTextExtent);
	    clut_image=ReadImage(clone_info,&(*image)->exception);
	    if (clut_image == (Image *) NULL)
	      continue;
	    
	    (void) HaldClutImage(*image,clut_image);

	    (void) DestroyImage(clut_image);
	    clut_image=(Image *) NULL;
	    continue;
          }
	break;
      }
      case 'i':
      {
        if (LocaleCompare("implode",option+1) == 0)
          {
            double
              amount;

            Image
              *implode_image;

            /*
              Implode image.
            */
            amount=MagickAtoF(argv[++i]);
            implode_image=ImplodeImage(*image,amount,&(*image)->exception);
            if (implode_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=implode_image;
            continue;
          }
        if (LocaleCompare("interlace",option+1) == 0)
          {
            if (*option == '+')
              {
                clone_info->interlace=UndefinedInterlace;
                continue;
              }
            option=argv[++i];
            clone_info->interlace=StringToInterlaceType(option);
            continue;
          }
        if (LocaleCompare("intent",option+1) == 0)
          {
            RenderingIntent
              rendering_intent;

            if (*option == '+')
              {
                (*image)->rendering_intent=UndefinedIntent;
                continue;
              }
            option=argv[++i];
            rendering_intent=UndefinedIntent;
            if (LocaleCompare("Absolute",option) == 0)
              rendering_intent=AbsoluteIntent;
            if (LocaleCompare("Perceptual",option) == 0)
              rendering_intent=PerceptualIntent;
            if (LocaleCompare("Relative",option) == 0)
              rendering_intent=RelativeIntent;
            if (LocaleCompare("Saturation",option) == 0)
              rendering_intent=SaturationIntent;
            (*image)->rendering_intent=rendering_intent;
            continue;
          }
        break;
      }
      case 'l':
      {
        if (LocaleCompare("label",option+1) == 0)
          {
            (void) SetImageAttribute(*image,"label",(char *) NULL);
            if (*option == '-')
              (void) SetImageAttribute(*image,"label",argv[++i]);
            continue;
          }
        if (LocaleCompare("lat",option+1) == 0)
          {
            Image
              *threshold_image;

            double
              offset;

            unsigned long
              height,
              width;

            /*
              Local adaptive threshold image.
            */
            offset=0;
            height=3;
            width=3;
            (void) sscanf(argv[++i],"%lux%lu%lf",&width,&height,&offset);
            if (strchr(argv[i],'%') != (char *) NULL)
              offset*=((double) MaxRGB/100.0);
            threshold_image=AdaptiveThresholdImage(*image,width,height,offset,
              &(*image)->exception);
            if (threshold_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=threshold_image;
            continue;
          }
        if (LocaleCompare("level",option+1) == 0)
          {
            (void) LevelImage(*image,argv[++i]);
            continue;
          }
        if (LocaleCompare("linewidth",option+1) == 0)
          {
            draw_info->stroke_width=MagickAtoF(argv[++i]);
            continue;
          }
        if (LocaleCompare("loop",option+1) == 0)
          {
            /*
              Set image iterations.
            */
            (*image)->iterations=MagickAtoL(argv[++i]);
            continue;
          }
        break;
      }
      case 'm':
      {
        if (LocaleCompare("magnify",option+1) == 0)
          {
            Image
              *magnify_image;

            /*
              Magnify image.
            */
            magnify_image=MagnifyImage(*image,&(*image)->exception);
            if (magnify_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=magnify_image;
            continue;
          }
        if (LocaleCompare("map",option+1) == 0)
          {
            /*
              Transform image colors to match this set of colors.
            */
            if (*option == '+')
              continue;
            (void) strlcpy(clone_info->filename,argv[++i],MaxTextExtent);
	    {
	      Image
		*map_image;

	      if ((map_image=ReadImage(clone_info,&(*image)->exception)) !=
		  (Image *) NULL)
		{
		  (void) MapImage(*image,map_image,quantize_info.dither);
		  DestroyImage(map_image);
		}
	    }
            continue;
          }
        if (LocaleCompare("mask",option+1) == 0)
          {
            Image
              *mask;

            long
              y;

            register long
              x;

            register PixelPacket
              *q;

            if (*option == '+')
              {
                /*
                  Remove a clip mask.
                */
                (void) SetImageClipMask(*image,(Image *) NULL);
                continue;
              }
            /*
              Set the image clip mask.
            */
            (void) strlcpy(clone_info->filename,argv[++i],MaxTextExtent);
            mask=ReadImage(clone_info,&(*image)->exception);
            if (mask == (Image *) NULL)
              continue;
            for (y=0; y < (long) mask->rows; y++)
            {
              q=GetImagePixels(mask,0,y,mask->columns,1);
              if (q == (PixelPacket *) NULL)
                break;
              for (x=0; x < (long) mask->columns; x++)
              {
                if (!mask->matte)
                  q->opacity=PixelIntensityToQuantum(q);
                q->red=q->opacity;
                q->green=q->opacity;
                q->blue=q->opacity;
                q++;
              }
              if (!SyncImagePixels(mask))
                break;
            }
            (void) SetImageType(mask,TrueColorMatteType);
            (void) SetImageClipMask(*image,mask);
	    /*
	      SetImageClipMask clones the image.
	     */
	    DestroyImage(mask);
          }
        if (LocaleCompare("matte",option+1) == 0)
          {
            if (*option == '-')
              if (!(*image)->matte)
                SetImageOpacity(*image,OpaqueOpacity);
            (*image)->matte=(*option == '-');
            continue;
          }
        if (LocaleCompare("mattecolor",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&clone_info->matte_color,
              &(*image)->exception);
            (*image)->matte_color=clone_info->matte_color;
            continue;
          }
        if (LocaleCompare("median",option+1) == 0)
          {
            double
              radius;

            Image
              *median_image;

            /*
              Median filter image.
            */
            radius=MagickAtoF(argv[++i]);
            median_image=MedianFilterImage(*image,radius,&(*image)->exception);
            if (median_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=median_image;
            continue;
          }
        if (LocaleCompare("minify",option+1) == 0)
          {
            Image
              *minify_image;

            /*
              Minify image.
            */
            minify_image=MinifyImage(*image,&(*image)->exception);
            if (minify_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=minify_image;
            continue;
          }
        if (LocaleCompare("modulate",option+1) == 0)
          {
            (void) ModulateImage(*image,argv[++i]);
            continue;
          }
        if ((LocaleCompare("mono",option+1) == 0) ||
            (LocaleCompare("monochrome",option+1) == 0))
          {
            clone_info->monochrome=MagickTrue;
	    (void) SetImageType(*image,BilevelType);
            continue;
          }
        if (LocaleCompare("motion-blur",option+1) == 0)
          {
            double
              angle,
              radius,
              sigma;

            Image
              *blur_image;

            /*
              Motion blur image.
            */
            radius=0.0;
            sigma=1.0;
            angle=0.0;
            (void) GetMagickDimension(argv[++i],&radius,&sigma,&angle,NULL);
            blur_image=MotionBlurImage(*image,radius,sigma,angle,&(*image)->exception);
            if (blur_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=blur_image;
            continue;
          }        
        break;
      }
      case 'n':
      {
        if (LocaleCompare("negate",option+1) == 0)
          {
            (void) NegateImage(*image,*option == '+');
            continue;
          }
        if (LocaleCompare("noise",option+1) == 0)
          {
            Image
              *noisy_image;

            if (*option == '-')
              noisy_image=
                ReduceNoiseImage(*image,MagickAtoL(argv[++i]),&(*image)->exception);
            else
              {
                NoiseType
                  noise_type;

                /*
                  Add noise to image.
                */
                option=argv[++i];
                noise_type=StringToNoiseType(option);
                noisy_image=
                  AddNoiseImage(*image,noise_type,&(*image)->exception);
              }
            if (noisy_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=noisy_image;
            continue;
          }
        if (LocaleCompare("normalize",option+1) == 0)
          {
            (void) NormalizeImage(*image);
            continue;
          }
        break;
      }
      case 'o':
      {
        if (LocaleCompare("opaque",option+1) == 0)
          {
            PixelPacket
              target;

            (void) AcquireOnePixelByReference(*image,&target,0,0,&(*image)->exception);
            (void) QueryColorDatabase(argv[++i],&target,&(*image)->exception);
            (void) OpaqueImage(*image,target,draw_info->fill);
            continue;
          }
        if (LocaleCompare("operator",option+1) == 0)
          {
            ChannelType
              channel;
            
            QuantumOperator
              quantum_operator;

            double
              rvalue;

            /* channel */
            channel=StringToChannelType(argv[++i]);

            /* operator id */
            quantum_operator=StringToQuantumOperator(argv[++i]);

            /* rvalue */
            option=argv[++i];
            rvalue=StringToDouble(option,MaxRGB);
            (void) QuantumOperatorImage(*image,channel,quantum_operator,
               rvalue,&(*image)->exception);

            continue;
          }
        if (LocaleCompare("ordered-dither",option+1) == 0)
          {
            /*
              Ordered-dither image.
            */
            (void) RandomChannelThresholdImage(*image,argv[i+1],argv[i+2],
                &(*image)->exception);
            i+=2;
            continue;
          }
        if (LocaleCompare("orient",option+1) == 0)
          {
	    (*image)->orientation=UndefinedOrientation;
	    if (*option == '-')
	      (*image)->orientation=StringToOrientationType(argv[++i]);
            continue;
          }
        break;
      }
      case 'p':
      {
        if (LocaleCompare("page",option+1) == 0)
          {
            if (option[0] == '+')
              {
                (*image)->page.width=0U;
                (*image)->page.height=0U;
                (*image)->page.x=0;
                (*image)->page.y=0;
              }
            else
              {
                char
                  *geometry_str;
                
                geometry_str=GetPageGeometry(argv[++i]);
                (void) GetGeometry(geometry_str,&(*image)->page.x,&(*image)->page.y,
                                   &(*image)->page.width,&(*image)->page.height);
                MagickFreeMemory(geometry_str);
              }
          }
        if (LocaleCompare("paint",option+1) == 0)
          {
            double
              radius;

            Image
              *paint_image;

            /*
              Oil paint image.
            */
            radius=MagickAtoF(argv[++i]);
            paint_image=OilPaintImage(*image,radius,&(*image)->exception);
            if (paint_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=paint_image;
            continue;
          }
        if (LocaleCompare("pen",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&draw_info->fill,
              &(*image)->exception);
            continue;
          }
        if (LocaleCompare("pointsize",option+1) == 0)
          {
            clone_info->pointsize=MagickAtoF(argv[++i]);
            draw_info->pointsize=clone_info->pointsize;
            continue;
          }
        if (LocaleCompare("profile",option+1) == 0)
          {
            void
              *client_data;

            if (*option == '+')
              {
                /*
                  Remove a ICM, IPTC, or generic profile from the image.
                */
                (void) ProfileImage(*image,argv[++i],
                  (unsigned char *) NULL,0,True);
                continue;
              }
            else if (*option == '-')
            {
              /*
                Add a ICM, IPTC, or generic profile to the image.
              */

              Image
                *profile_image;
              
              ProfileInfo
                profile_info;

              const char
                *profile_name;

              size_t
                profile_length;
              
              const unsigned char *
                profile_data;

              ImageProfileIterator
                profile_iterator;
              
              client_data=clone_info->client_data;

              /*
                 FIXME: Next three lines replace:
                  clone_info->client_data=(void *) &(*image)->iptc_profile;
              */
              profile_info.name="IPTC";
              profile_info.info=
		(unsigned char *) GetImageProfile(*image,profile_info.name,
						  &profile_info.length);
              clone_info->client_data=&profile_info; /* used to pass profile to meta.c */

              (void) strlcpy(clone_info->filename,argv[++i],MaxTextExtent);
              profile_image=ReadImage(clone_info,&(*image)->exception);
              if (profile_image == (Image *) NULL)
                {
                  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                       "Failed to load profile from file \"%s\"",
                                       clone_info->filename);
                  continue;
                }
              /*
                Transfer profile(s) to image.
              */
              profile_iterator=AllocateImageProfileIterator(profile_image);
              while(NextImageProfile(profile_iterator,&profile_name,&profile_data,
                                     &profile_length) != MagickFail)
                {
		  size_t
		    existing_length;

                  if (((LocaleCompare(profile_name,"ICC") == 0) ||
		       (LocaleCompare(profile_name,"ICM") == 0)) &&
		      (GetImageProfile(*image,"ICM",&existing_length)))
		    {
		      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
					    "Transform using %s profile \"%s\","
					    " %lu bytes",
					    profile_name,clone_info->filename,
					    (unsigned long) profile_length);
		      (void) ProfileImage(*image,profile_name,
					  (unsigned char *) profile_data,
					  profile_length,True);
		    }
                  else
		    {
		      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
					    "Adding %s profile \"%s\","
					    " %lu bytes",
					    profile_name,clone_info->filename,
					    (unsigned long) profile_length);
		      (void) SetImageProfile(*image,profile_name,profile_data,profile_length);
		    }
                }
              DeallocateImageProfileIterator(profile_iterator);
              DestroyImage(profile_image);
	      profile_image=(Image *) NULL;
              clone_info->client_data=client_data;
            }
            continue;
          }
        break;
      }
      case 'q':
      {
        if (LocaleCompare("quality",option+1) == 0)
          {
            /*
              Set image compression quality.
            */
            clone_info->quality=MagickAtoL(argv[++i]);
            continue;
          }
        break;
      }
      case 'r':
      {
        if (LocaleCompare("raise",option+1) == 0)
          {
            /*
              Surround image with a raise of solid color.
            */
            (void) GetImageGeometry(*image,argv[++i],False,&geometry);
            (void) RaiseImage(*image,&geometry,*option == '-');
            continue;
          }
        if (LocaleCompare("random-threshold",option+1) == 0)
          {
            /*
              Threshold image.
            */
            (void) RandomChannelThresholdImage(*image,argv[i+1],argv[i+2],
                &(*image)->exception);
            i+=2;
            continue;
          }
        if (LocaleCompare("recolor",option+1) == 0)
          {
            char
              *p,
              token[MaxTextExtent];

            double
              *matrix;

            register long
              x;

            unsigned int
	      elements,
              order;

            /*
              Color matrix image.
            */
            p=argv[++i];
            for (elements=0; *p != '\0'; elements++)
            {
              MagickGetToken(p,&p,token,MaxTextExtent);
	      if (token[0] == '\0')
		break;
              if (token[0] == ',')
                MagickGetToken(p,&p,token,MaxTextExtent);
	      if (token[0] == '\0')
		break;
            }
            order=(unsigned int) sqrt(elements);
	    if ((0 == elements) || (order*order != elements))
	      {
		char
		  message[MaxTextExtent];

		FormatString(message,"%u",elements);
		ThrowException(&(*image)->exception,OptionError,MatrixIsNotSquare,message);
		continue;
	      }
            matrix=MagickAllocateArray(double *,order*order,sizeof(double));
            if (matrix == (double *) NULL)
              MagickFatalError(ResourceLimitFatalError,MemoryAllocationFailed,
                MagickMsg(ResourceLimitError,UnableToAllocateCoefficients));
            p=argv[i];
            for (x=0; *p != '\0'; x++)
            {
              MagickGetToken(p,&p,token,MaxTextExtent);
              if (token[0] == ',')
                MagickGetToken(p,&p,token,MaxTextExtent);
	      if (token[0] == '\0')
		break;
	      matrix[x]=MagickAtoF(token);
            }
            for ( ; x < (long) (order*order); x++)
              matrix[x]=0.0;
	    (void) ColorMatrixImage(*image,order,matrix);
            MagickFreeMemory(matrix);
            continue;
          }
        if (LocaleCompare("red-primary",option+1) == 0)
          {
            /*
              Red chromaticity primary point.
            */
            if (*option == '+')
              {
                (*image)->chromaticity.red_primary.x=0.0;
                (*image)->chromaticity.red_primary.y=0.0;
                continue;
              }
            (void) sscanf(argv[++i],"%lf%*[,/]%lf",
              &(*image)->chromaticity.red_primary.x,
              &(*image)->chromaticity.red_primary.y);
            continue;
          }
        if (LocaleCompare("region",option+1) == 0)
          {
            Image
              *crop_image;

            if (region_image != (Image *) NULL)
              {
                /*
                  Composite region.
                */
                (void) CompositeImage(region_image,(*image)->matte ?
                  OverCompositeOp : CopyCompositeOp,*image,region_geometry.x,
                  region_geometry.y);
                DestroyImage(*image);
                *image=region_image;
              }
            if (*option == '+')
              continue;
            /*
              Apply transformations to a selected region of the image.
            */
            (void) GetImageGeometry(*image,argv[++i],False,&region_geometry);
            crop_image=CropImage(*image,&region_geometry,&(*image)->exception);
            if (crop_image == (Image *) NULL)
              break;
            region_image=(*image);
            *image=crop_image;
            continue;
          }
        if (LocaleCompare("render",option+1) == 0)
          {
            draw_info->render=(*option == '+');
            continue;
          }
	if (LocaleCompare("repage",option+1) == 0)
          {
	    if (*option == '+')
	      {
		/* Reset page to defaults */
                (*image)->page.width=0U;
                (*image)->page.height=0U;
                (*image)->page.x=0;
                (*image)->page.y=0;
	      }
	    else
	      {
		/* Adjust page offsets */
		(void) ResetImagePage(*image,argv[++i]);
	      }
            continue;
          }
        if (LocaleCompare("resample",option+1) == 0)
          {
            Image
              *resample_image;
            
            char
              resample_density[MaxTextExtent];
            
            double
              x_resolution,
              y_resolution;
            
            unsigned long
              resample_height,
              resample_width;
            
            /*
              Verify that image contains useful resolution information.
            */
            if ( ((*image)->x_resolution == 0) || ((*image)->y_resolution == 0) )
              {
                ThrowException(&(*image)->exception,ImageError,
                               ImageDoesNotContainResolution,image_info->filename);
                continue;
              }
            
            /*
              Obtain target resolution.
            */
            {
              unsigned long
                x_integral_resolution=0,
                y_integral_resolution=0;
              
              long
                x,
                y;
              
              int
                flags;
              
              flags=GetGeometry(argv[++i],&x,&y,&x_integral_resolution,&y_integral_resolution);
              if (!(flags & HeightValue))
                y_integral_resolution=x_integral_resolution;
              FormatString(resample_density,"%lux%lu",x_integral_resolution,y_integral_resolution);
              x_resolution=x_integral_resolution;
              y_resolution=y_integral_resolution;
            }
            
            resample_width=(unsigned long)
              ((*image)->columns*(x_resolution/(*image)->x_resolution)+0.5);
            if (resample_width < 1)
              resample_width = 1;
            resample_height=(unsigned long)
              ((*image)->rows*(y_resolution/(*image)->y_resolution)+0.5);
            if (resample_height < 1)
              resample_height = 1;

            (void) CloneString(&clone_info->density,resample_density);
            (void) CloneString(&draw_info->density,resample_density);
            (*image)->x_resolution=x_resolution;
            (*image)->y_resolution=y_resolution;
            if ((((*image)->columns == resample_width)) && ((*image)->rows == resample_height))
              break;
            
            /*
              Resample image.
            */
            resample_image=ResizeImage(*image,resample_width,resample_height,(*image)->filter,
                                       (*image)->blur,&(*image)->exception);
            if (resample_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=resample_image;
            continue;
          }
        if (LocaleCompare("resize",option+1) == 0)
          {
            Image
              *resize_image;

            /*
              Resize image.
            */
            (void) GetImageGeometry(*image,argv[++i],True,&geometry);
            if ((geometry.width == (*image)->columns) &&
                (geometry.height == (*image)->rows))
              break;
            resize_image=ResizeImage(*image,geometry.width,geometry.height,
              (*image)->filter,(*image)->blur,&(*image)->exception);
            if (resize_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=resize_image;
            continue;
          }
        if (LocaleCompare("roll",option+1) == 0)
          {
            Image
              *roll_image;

            /*
              Roll image.
            */
            (void) GetImageGeometry(*image,argv[++i],False,&geometry);
            roll_image=RollImage(*image,geometry.x,geometry.y,
              &(*image)->exception);
            if (roll_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=roll_image;
            continue;
          }
        if (LocaleCompare("rotate",option+1) == 0)
          {
            double
              degrees;

            Image
              *rotate_image;

            /*
              Check for conditional image rotation.
            */
            i++;
            if (strchr(argv[i],'>') != (char *) NULL)
              if ((*image)->columns <= (*image)->rows)
                break;
            if (strchr(argv[i],'<') != (char *) NULL)
              if ((*image)->columns >= (*image)->rows)
                break;
            /*
              Rotate image.
            */
            degrees=MagickAtoF(argv[i]);
            rotate_image=RotateImage(*image,degrees,&(*image)->exception);
            if (rotate_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=rotate_image;
            continue;
          }
        break;
      }
      case 's':
      {
        if (LocaleCompare("sample",option+1) == 0)
          {
            Image
              *sample_image;

            /*
              Sample image with pixel replication.
            */
            (void) GetImageGeometry(*image,argv[++i],True,&geometry);
            if ((geometry.width == (*image)->columns) &&
                (geometry.height == (*image)->rows))
              break;
            sample_image=SampleImage(*image,geometry.width,geometry.height,
              &(*image)->exception);
            if (sample_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=sample_image;
            continue;
          }
        if (LocaleCompare("sampling_factor",option+1) == 0)
          {
            /*
              Set image sampling factor.
            */
            (void) CloneString(&clone_info->sampling_factor,argv[++i]);
            NormalizeSamplingFactor(clone_info);
            continue;
          }
        if (LocaleCompare("sans",option+1) == 0)
          if (*option == '-')
            i++;
        if (LocaleCompare("scale",option+1) == 0)
          {
            Image
              *scale_image;

            /*
              Resize image.
            */
            (void) GetImageGeometry(*image,argv[++i],True,&geometry);
            if ((geometry.width == (*image)->columns) &&
                (geometry.height == (*image)->rows))
              break;
            scale_image=ScaleImage(*image,geometry.width,geometry.height,
              &(*image)->exception);
            if (scale_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=scale_image;
            continue;
          }
        if (LocaleCompare("scene",option+1) == 0)
          {
            (*image)->scene=MagickAtoL(argv[++i]);
            continue;
          }
        if (LocaleCompare("set",option+1) == 0)
          {
            const char
              *key,
              *value;

            key=argv[++i];
            (void) SetImageAttribute(*image,key,(char *) NULL);
            if (*option == '-')
              {
                value=argv[++i];
                (void) SetImageAttribute(*image,key,value);
              }
            continue;
          }
        if (LocaleCompare("segment",option+1) == 0)
          {
            double
              cluster_threshold,
              smoothing_threshold;

            /*
              Segment image.
            */
            cluster_threshold=1.0;
            smoothing_threshold=1.5;
            (void) GetMagickDimension(argv[++i],&cluster_threshold,
                                      &smoothing_threshold,NULL,NULL);
            (void) SegmentImage(*image,quantize_info.colorspace,
              clone_info->verbose,cluster_threshold,smoothing_threshold);
            continue;
          }
        if (LocaleCompare("shade",option+1) == 0)
          {
            double
              azimuth,
              elevation;

            Image
              *shade_image;

            /*
              Shade image.
            */
            azimuth=30.0;
            elevation=30.0;
            (void) GetMagickDimension(argv[++i],&azimuth,&elevation,NULL,NULL);
            shade_image=ShadeImage(*image,*option == '-',azimuth,elevation,
              &(*image)->exception);
            if (shade_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=shade_image;
            continue;
          }
        if (LocaleCompare("sharpen",option+1) == 0)
          {
            double
              radius,
              sigma;

            Image
              *sharp_image;

            /*
              Gaussian sharpen image.
            */
            radius=0.0;
            sigma=1.0;
            (void) GetMagickDimension(argv[++i],&radius,&sigma,NULL,NULL);
            sharp_image=SharpenImage(*image,radius,sigma,&(*image)->exception);
            if (sharp_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=sharp_image;
            continue;
          }
        if (LocaleCompare("shave",option+1) == 0)
          {
            Image
              *shave_image;

            /*
              Shave the image edges.
            */
            (void) GetImageGeometry(*image,argv[++i],False,&geometry);
            shave_image=ShaveImage(*image,&geometry,&(*image)->exception);
            if (shave_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=shave_image;
            continue;
          }
        if (LocaleCompare("shear",option+1) == 0)
          {
            double
              x_shear,
              y_shear;

            Image
              *shear_image;

            /*
              Shear image.
            */
            x_shear=0.0;
            y_shear=0.0;
            (void) GetMagickDimension(argv[++i],&x_shear,&y_shear,NULL,NULL);
            shear_image=ShearImage(*image,x_shear,y_shear,&(*image)->exception);
            if (shear_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=shear_image;
            continue;
          }
        if (LocaleCompare("solarize",option+1) == 0)
          {
            double
              threshold;

            threshold=StringToDouble(argv[++i],MaxRGB);
            (void) SolarizeImage(*image,threshold);
            continue;
          }
        if (LocaleCompare("spread",option+1) == 0)
          {
            unsigned int
              amount;

            Image
              *spread_image;

            /*
              Spread an image.
            */
            amount=MagickAtoI(argv[++i]);
            spread_image=SpreadImage(*image,amount,&(*image)->exception);
            if (spread_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=spread_image;
            continue;
          }
	if (LocaleCompare("strip",option+1) == 0)
	  {
	    (void) StripImage(*image);
	    continue;
	  }
        if (LocaleCompare("stroke",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&draw_info->stroke,
              &(*image)->exception);
            continue;
          }
        if (LocaleCompare("strokewidth",option+1) == 0)
          {
            draw_info->stroke_width=MagickAtoF(argv[++i]);
            continue;
          }
        if (LocaleCompare("swirl",option+1) == 0)
          {
            double
              degrees;

            Image
              *swirl_image;

            /*
              Swirl image.
            */
            degrees=MagickAtoF(argv[++i]);
            swirl_image=SwirlImage(*image,degrees,&(*image)->exception);
            if (swirl_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=swirl_image;
            continue;
          }
        break;
      }
      case 't':
      {
        if (LocaleCompare("threshold",option+1) == 0)
          {
            /*
              Threshold image.
            */
            double
              threshold;

            ++i;
            count=sscanf(argv[i],"%lf",&threshold);
            if (count > 0)
              {
                if (strchr(argv[i],'%') != (char *) NULL)
                  threshold *=  MaxRGB/100.0;
                (void) ThresholdImage(*image,threshold);
              }
            continue;
          }
        if (LocaleCompare("thumbnail",option+1) == 0)
          {
            Image
              *resize_image;

            /*
              Resize image.
            */
            (void) GetImageGeometry(*image,argv[++i],True,&geometry);
            if ((geometry.width == (*image)->columns) &&
                (geometry.height == (*image)->rows))
              break;
            resize_image=ThumbnailImage(*image,geometry.width,geometry.height,
              &(*image)->exception);
            if (resize_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=resize_image;
            continue;
          }
        if (LocaleCompare("tile",option+1) == 0)
          {
            Image
              *fill_pattern;

            (void) strlcpy(clone_info->filename,argv[++i],MaxTextExtent);
            fill_pattern=ReadImage(clone_info,&(*image)->exception);
            if (fill_pattern == (Image *) NULL)
              continue;
            draw_info->fill_pattern=
              CloneImage(fill_pattern,0,0,True,&(*image)->exception);
            DestroyImage(fill_pattern);
	    fill_pattern=(Image *) NULL;
            continue;
          }
        if (LocaleCompare("transform",option+1) == 0)
          {
            Image
              *transform_image;

            /*
              Affine transform image.
            */
            transform_image=AffineTransformImage(*image,&draw_info->affine,
              &(*image)->exception);
            if (transform_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=transform_image;
            continue;
          }
        if (LocaleCompare("transparent",option+1) == 0)
          {
            PixelPacket
              target;

            (void) AcquireOnePixelByReference(*image,&target,0,0,&(*image)->exception);
            (void) QueryColorDatabase(argv[++i],&target,&(*image)->exception);
            (void) TransparentImage(*image,target,TransparentOpacity);
            continue;
          }
        if (LocaleCompare("treedepth",option+1) == 0)
          {
            quantize_info.tree_depth=MagickAtoI(argv[++i]);
            continue;
          }
        if (LocaleCompare("trim",option+1) == 0)
          {
            TransformImage(image,"0x0",(char *) NULL);
            continue;
          }
        if (LocaleCompare("type",option+1) == 0)
          {
            ImageType
              image_type;

            option=argv[++i];
            image_type=StringToImageType(option);
            (*image)->dither=image_info->dither;
            if (UndefinedType != image_type)
              (void) SetImageType(*image,image_type);
            continue;
          }
        break;
      }
      case 'u':
      {
        if (LocaleCompare("undercolor",option+1) == 0)
          {
            (void) QueryColorDatabase(argv[++i],&draw_info->undercolor,
              &(*image)->exception);
            continue;
          }
        if (LocaleCompare("units",option+1) == 0)
          {
            ResolutionType
              resolution_type = UndefinedResolution;

            if (*option == '+')
              {
              }
            else if (*option == '-')
              {
                option=argv[++i];
                if (LocaleCompare("PixelsPerInch",option) == 0)
                  resolution_type=PixelsPerInchResolution;
                else if (LocaleCompare("PixelsPerCentimeter",option) == 0)
                  resolution_type=PixelsPerCentimeterResolution;
                else
                  {
                    ThrowException(&(*image)->exception,OptionError,
                                   UnrecognizedUnitsType,option);
                    continue;
                  }

                /*
                  Adjust resolution values if necessary.
                */
                if ( (resolution_type == PixelsPerInchResolution) &&
                     ((*image)->units == PixelsPerCentimeterResolution) )
                  {
                    (*image)->x_resolution *= 2.54;
                    (*image)->y_resolution *= 2.54;
                  }
                else if ( (resolution_type == PixelsPerCentimeterResolution) &&
                          ((*image)->units == PixelsPerInchResolution) )
                  {
                    (*image)->x_resolution /= 2.54;
                    (*image)->y_resolution /= 2.54;
                  }
              }
            
            (*image)->units=resolution_type;
            continue;
          }
        if (LocaleCompare("unsharp",option+1) == 0)
          {
            double
              amount,
              radius,
              sigma,
              threshold;

            Image
              *unsharp_image;

            /*
              Gaussian unsharpen image.
            */
            amount=1.0;
            radius=0.0;
            sigma=1.0;
            threshold=0.05;
            (void) GetMagickDimension(argv[++i],&radius,&sigma,&amount,&threshold);
            unsharp_image=UnsharpMaskImage(*image,radius,sigma,amount,threshold,
              &(*image)->exception);
            if (unsharp_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=unsharp_image;
            continue;
          }
        break;
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            clone_info->verbose+=(*option == '-');
            quantize_info.measure_error=(*option == '-');
            continue;
          }
        if (LocaleCompare("virtual-pixel",option+1) == 0)
          {
            VirtualPixelMethod
              virtual_pixel_method;

            if (*option == '+')
              {
                (void) SetImageVirtualPixelMethod(*image,UndefinedVirtualPixelMethod);
                continue;
              }
            option=argv[++i];
            virtual_pixel_method=StringToVirtualPixelMethod(option);
            (void) SetImageVirtualPixelMethod(*image,virtual_pixel_method);
            continue;
          }
        break;
      }
      case 'w':
      {
        if (LocaleCompare("wave",option+1) == 0)
          {
            double
              amplitude,
              wavelength;

            Image
              *wave_image;

            /*
              Wave image.
            */
            amplitude=25.0;
            wavelength=150.0;
            (void) GetMagickDimension(argv[++i],&amplitude,&wavelength,NULL,NULL);
            wave_image=WaveImage(*image,amplitude,wavelength,
              &(*image)->exception);
            if (wave_image == (Image *) NULL)
              break;
            DestroyImage(*image);
            *image=wave_image;
            continue;
          }
        if (LocaleCompare("white-point",option+1) == 0)
          {
            /*
              White chromaticity point.
            */
            if (*option == '+')
              {
                (*image)->chromaticity.white_point.x=0.0;
                (*image)->chromaticity.white_point.y=0.0;
                continue;
              }
            (void) sscanf(argv[++i],"%lf%*[,/]%lf",
              &(*image)->chromaticity.white_point.x,
              &(*image)->chromaticity.white_point.y);
            continue;
          }
        if (LocaleCompare("white-threshold",option+1) == 0)
          {
            /*
              White threshold image.
            */
            ++i;
            (void) WhiteThresholdImage(*image,argv[i]);
            continue;
          }
        if (LocaleCompare("write",option+1) == 0)
          {
            /*
              Write current image to specified file.
            */
            Image
              *clone_image;

            ++i;
            clone_image=CloneImage(*image,0,0,MagickTrue,&(*image)->exception);
            if (clone_image != (Image *) NULL)
              {
                (void) strlcpy(clone_image->filename,argv[i],sizeof(clone_image->filename));
                (void) WriteImage(clone_info,clone_image);
                if (clone_info->verbose)
                  (void) DescribeImage(clone_image,stderr,False);
                DestroyImage(clone_image);
                clone_image=(Image *) NULL;
              }
            continue;
          }
        break;
      }
      default:
        break;
    }
  }
  if (region_image != (Image *) NULL)
    {
      /*
        Composite transformed region onto image.
      */
      matte=region_image->matte;
      (void) CompositeImage(region_image,
        (*image)->matte ? OverCompositeOp : CopyCompositeOp,*image,
        region_geometry.x,region_geometry.y);
      DestroyImage(*image);
      *image=region_image;
      (*image)->matte=matte;
    }
#if 0
  if ( IsGrayColorspace(quantize_info.colorspace) )
    {
      /*
        If color reduction is requested, then quantize to the requested
        number of colors in the gray colorspace, otherwise simply
        transform the image to the gray colorspace.
      */

      if ( quantize_info.number_colors != 0 )
        (void) QuantizeImage(&quantize_info,*image);
      else
        (void) TransformColorspace(*image,quantize_info.colorspace);
    }
  else
    {
      /*
        If color reduction is requested, and the image is DirectClass,
        or the image is PseudoClass and the number of colors exceeds
        the number requested, then quantize the image colors. Otherwise
        compress an existing colormap.
      */

      if ( quantize_info.number_colors != 0 )
        {
          if (((*image)->storage_class == DirectClass) ||
              ((*image)->colors > quantize_info.number_colors))
            (void) QuantizeImage(&quantize_info,*image);
          else
            CompressImageColormap(*image);
        }
    }
#endif

  /*
    Free resources.
  */
  DestroyDrawInfo(draw_info);
  DestroyImageInfo(clone_info);
  return((*image)->exception.severity == UndefinedException);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
+     M o g r i f y I m a g e s                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method MogrifyImages applies next processing options to a sequence of
%  images as prescribed by command line options.
%
%  The format of the MogrifyImage method is:
%
%      unsigned int MogrifyImages(const ImageInfo *image_info,const int argc,
%        char **argv,Image **images)
%
%  A description of each parameter follows:
%
%    o image_info: The image info..
%
%    o argc: Specifies a pointer to an integer describing the number of
%      elements in the argument vector.
%
%    o argv: Specifies a pointer to a text array containing the command line
%      arguments.
%
%    o images: The image;  returned from
%      ReadImage.
%
%
*/
MagickExport unsigned int MogrifyImages(const ImageInfo *image_info,
  int argc,char **argv,Image **images)
{
#define MogrifyImageText  "Transform image...  "

  char
    *option;

  Image
    *image,
    *mogrify_images;

  register long
    i;

  unsigned int
    status;

  unsigned long
    scene;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(images != (Image **) NULL);
  assert((*images)->signature == MagickSignature);
  if ((argc <= 0) || (*argv == (char *) NULL))
    return(True);

#if defined(DEBUG_COMMAND_PARSER)
  fprintf(stderr,"MogrifyImages (");
  for (image=*images; image; image=image->next)
    {
      if (image != *images)
	fprintf(stderr,", ");
      fprintf(stderr,"0x%p->%s", image, image->filename);
    }
  fprintf(stderr,"):");
  for (i=0; i < argc; i++)
    fprintf(stderr," %s",argv[i]);
  fprintf(stderr,"\n");
#endif /* DEBUG_COMMAND_PARSER */

  scene=False;
  for (i=0; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) <= 1) || ((*option != '-') && (*option != '+')))
      continue;
    switch (*(option+1))
    {
      case 's':
      {
        if (LocaleCompare("scene",option+1) == 0)
          scene=True;
        break;
      }
      default:
        break;
    }
  }
  /*
    Apply options to each individual image in the list.
  */
  status=True;
  mogrify_images=NewImageList();
  i=0;
  while ((image=RemoveFirstImageFromList(images)) != (Image *) NULL)
    {
      status&=MogrifyImage(image_info,argc,argv,&image);
      {
	Image
	  *p;

	for (p=image; p != (Image *) NULL; p=p->next)
	  {
	    if (scene)
	      p->scene += i;
	    if (image_info->verbose)
	      (void) DescribeImage(p,stderr,False);
	    i++;
	  }
      }
      AppendImageToList(&mogrify_images,image);
    }

  /*
    Apply options to the entire image list.
  */
  for (i=0; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) == 1) || ((option[0] != '-') && (option[0] != '+')))
      continue;
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("append",option+1) == 0)
          {
            Image
              *append_image;

            append_image=AppendImages(mogrify_images,*option == '-',
              &mogrify_images->exception);
            if (append_image != (Image *) NULL)
              {
                DestroyImageList(mogrify_images);
                mogrify_images=append_image;
              }
            break;
          }
        if (LocaleCompare("average",option+1) == 0)
          {
            Image
              *average_image;

            average_image=AverageImages(mogrify_images,
              &mogrify_images->exception);
            if (average_image != (Image *) NULL)
              {
                DestroyImageList(mogrify_images);
                mogrify_images=average_image;
              }
            break;
          }
        break;
      }
      case 'c':
      {
        if (LocaleCompare("coalesce",option+1) == 0)
          {
            Image
              *coalesce_image;

            coalesce_image=CoalesceImages(mogrify_images,
              &mogrify_images->exception);
            if (coalesce_image != (Image *) NULL)
              {
                DestroyImageList(mogrify_images);
                mogrify_images=coalesce_image;
              }
            break;
          }
        break;
      }
      case 'd':
      {
        if (LocaleCompare("deconstruct",option+1) == 0)
          {
            Image
              *deconstruct_image;

            deconstruct_image=DeconstructImages(mogrify_images,
              &mogrify_images->exception);
            if (deconstruct_image != (Image *) NULL)
              {
                DestroyImageList(mogrify_images);
                mogrify_images=deconstruct_image;
              }
            break;
          }
        break;
      }
      case 'f':
      {
        if (LocaleCompare("flatten",option+1) == 0)
          {
	    Image
	      *flatten_image;
	    
	    flatten_image=FlattenImages(mogrify_images,
					&mogrify_images->exception);
	    if (flatten_image != (Image *) NULL)
	      {
		DestroyImageList(mogrify_images);
		mogrify_images=flatten_image;
	      }
            break;
          }
        break;
      }
      case 'm':
      {
        if (LocaleCompare("map",option+1) == 0)
          {
             if (*option == '+')
               {
                 (void) MapImages(mogrify_images,(Image *) NULL,
                   image_info->dither);
                 break;
               }
             i++;
             break;
          }
        if (LocaleCompare("morph",option+1) == 0)
          {
            Image
              *morph_image;

            morph_image=MorphImages(mogrify_images,MagickAtoL(argv[++i]),
              &mogrify_images->exception);
            if (morph_image != (Image *) NULL)
              {
                DestroyImageList(mogrify_images);
                mogrify_images=morph_image;
              }
            break;
          }
        if (LocaleCompare("mosaic",option+1) == 0)
          {
            Image
              *mosaic_image;

            mosaic_image=MosaicImages(mogrify_images,
              &mogrify_images->exception);
            if (mosaic_image != (Image *) NULL)
              {
                DestroyImageList(mogrify_images);
                mogrify_images=mosaic_image;
              }
            break;
          }
        break;
      }
      case 'p':
      {
        if (LocaleCompare("process",option+1) == 0)
          {
            char
              *arguments,
              breaker,
              quote,
              *token;

            int
              next,
              t_status;
	    
            size_t
              length;
	    
            TokenInfo
              token_info;
	    
            length=strlen(argv[++i]);
            token=MagickAllocateMemory(char *,length+1);
            if (token == (char *) NULL)
              continue;
            next=0;
            /* FIXME: This code truncates the last character for an
               argument like "analyze" but works for "analyze=" */
            arguments=argv[i];
            t_status=Tokenizer(&token_info,0,token,length,arguments,
			       (char *) "",(char *) "=",(char *) "\"",
			       0,&breaker,&next,&quote);
            if (t_status == 0)
              {
                char
                  *t_argv;
		
                t_argv=&(arguments[next]);
                (void) ExecuteModuleProcess(token,&mogrify_images,1,
					    &t_argv);
              }
            MagickFreeMemory(token);
            continue;
          }
        break;
      }
      default:
        break;
    }
  }
  *images=mogrify_images;
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    M o g r i f y I m a g e C o m m a n d                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MogrifyImageCommand() transforms an image or a sequence of images. These
%  transforms include image scaling, image rotation, color reduction, and
%  others. The transmogrified image overwrites the original image.
%
%  The format of the MogrifyImageCommand method is:
%
%      unsigned int MogrifyImageCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

#define ThrowMogrifyException(code,reason,description) \
{ \
  status = MagickFail; \
  ThrowException(exception,code,reason,description); \
  goto mogrify_cleanup_and_return; \
}

typedef struct _TransmogrifyOptions
{
  ImageInfo *image_info;
  const char *input_filename;
  int argc;
  char **argv;
  const char *output_format;
  const char *output_directory;
  MagickBool create_directories;
  MagickBool global_colormap;
  MagickBool preserve_file_attr;
  MagickPassFail status;
  ExceptionInfo exception;
} TransmogrifyOptions;
static MagickPassFail* TransmogrifyImage(TransmogrifyOptions *options)
{
  Image
    *image;

  ImageInfo
    *image_info;

  MagickPassFail
    status = MagickPass;

  int
    fileatt_error = -1;

  MagickStatStruct_t
    statbuf;

  assert(options != (TransmogrifyOptions *) NULL);
  assert(options->input_filename != (char *) NULL);

  image_info=CloneImageInfo(options->image_info);
  (void) strlcpy(image_info->filename,options->input_filename,MaxTextExtent);

  while (MagickTrue)
    {
      char
        output_filename[MaxTextExtent],
        temporary_filename[MaxTextExtent];

      image=ReadImage(image_info,&options->exception);
      status = (image != (Image *) NULL) &&
        (options->exception.severity < ErrorException);

      if (status == MagickFail)
        break;
        
      /*
        Transmogrify image as defined by the preceding image
        processing options.
      */
      status = MogrifyImages(image_info,options->argc,options->argv,&image);
      if (image->exception.severity > options->exception.severity)
        CopyException(&options->exception,&image->exception);
      if (status == MagickFail)
        break;

      if (options->global_colormap)
        {
          /*
            Apply a global colormap to the image sequence.
          */
          status = MapImages(image,(Image *) NULL,image_info->dither);
          if (image->exception.severity > options->exception.severity)
            CopyException(&options->exception,&image->exception);
        }
      if (status == MagickFail)
        break;

      /*
        Write transmogrified image to disk.
      */

      (void) strlcpy(temporary_filename,"",MaxTextExtent);

      /*
        Compute final output file name and format
      */
      (void) strlcpy(output_filename,"",MaxTextExtent);
      if (((const char *) NULL != options->output_directory) &&
	  (options->output_directory[0] != '\0'))
        {
          size_t
            output_directory_length;
                  
          output_directory_length = strlen(options->output_directory);
          if (0 != output_directory_length)
            {
              (void) strlcat(output_filename,options->output_directory,MaxTextExtent);
              if (output_filename[output_directory_length-1] != DirectorySeparator[0])
                (void) strlcat(output_filename,DirectorySeparator,MaxTextExtent);
            }
        }
      (void) strlcat(output_filename,image->filename,MaxTextExtent);
      if (options->output_format != (char *) NULL)
        {
          /*
            Replace file extension with new output format.
          */
          AppendImageFormat(options->output_format,output_filename);
          (void) strlcpy(image->magick,options->output_format,MaxTextExtent);
        }

      if (options->preserve_file_attr)
        fileatt_error = MagickGetFileAttributes(image->filename, &statbuf);

      if (options->create_directories)
        {
          /*
            Create directory (as required)
          */
          char
            directory[MaxTextExtent];
                  
          GetPathComponent(output_filename,HeadPath,directory);
          if (IsAccessibleNoLogging(directory) == MagickFalse)
            {
              if (image_info->verbose)
                fprintf(stdout,"Creating directory \"%s\".\n",directory);
                      
              if (MagickCreateDirectoryPath(directory,&options->exception)
                  == MagickFail)
                {
                  status = MagickFail;
                }
            }
          if (status == MagickFail)
            break;          
        }
      if (LocaleCompare(image_info->filename,"-") != 0)
        {
          /*
            If output file already exists and is writeable by the
            user, then create a rescue file by adding a tilde to the
            existing file name and renaming.  If output file does
            not exist or the rename of the existing file fails, then
            there is no backup!  Note that this approach allows working
            within a read-only directory if the output file already exists
            and is writeable.
          */
          if (IsWriteable(output_filename))
            {
              (void) strlcpy(temporary_filename,output_filename,MaxTextExtent);
              (void) strcat(temporary_filename,"~");
              if (rename(output_filename,temporary_filename) == 0)
                {
                  if (image_info->verbose)
                    (void) fprintf(stdout, "rename to backup %.1024s=>%.1024s\n",
                                   output_filename,temporary_filename);
                }
              else
                {
                  (void) strlcpy(temporary_filename,"",MaxTextExtent);
                }
            }
        }
      /*
        Write the output file.
      */
      (void) strlcpy(image->filename,output_filename,MaxTextExtent);
      status = WriteImages(image_info,image,image->filename,&options->exception);

      if (options->preserve_file_attr)
        {
          if (fileatt_error == 0)
            {
              if (MagickSetFileAttributes(image->filename, &statbuf) != 0)
                {
                  fprintf(stderr, "Error preserving file timestamps\n");
                }
            }
        }

      if ((status != MagickFail) && (temporary_filename[0] != 0))
        {
          /*
            Remove backup file.
          */
          if (remove(temporary_filename) == 0)
            {
              if (image_info->verbose)
                (void) fprintf(stdout, "remove backup %.1024s\n",temporary_filename);
            }
        }
      break;
    }
  if (image)
    DestroyImageList(image);
  DestroyImageInfo(image_info);
  options->status=status;
  return (&options->status);
}

static MagickPassFail
LoadAndCacheImageFile(char **filename,long *id,ExceptionInfo *exception)
{
  MagickPassFail
    status;

  status=MagickFail;
  *id=-1;
  if (LocaleNCompare(*filename,"MPRI:",5) != 0)
    {
      ImageInfo
	*image_info;
      
      image_info=CloneImageInfo((const ImageInfo *) NULL);
      if (image_info != (ImageInfo *) NULL)
	{
	  Image
	    *clut_image;
	  
	  (void) strlcpy(image_info->filename,*filename,MaxTextExtent);
	  clut_image=ReadImage(image_info,exception);
	  if (clut_image != (Image *) NULL)
	    {
	      *id=SetMagickRegistry(ImageRegistryType,clut_image,sizeof(Image),exception);
	      if (*id != -1)
		{
		  char
		    mpri[MaxTextExtent];
		  
		  FormatString(mpri,"MPRI:%ld",*id);
		  MagickFreeMemory(*filename);
		  *filename=AcquireString(mpri);
		  if (*filename != (char *) NULL)
		    status=MagickPass;
		}
	      DestroyImage(clut_image);
	    }
	  DestroyImageInfo(image_info);
	}
    }
  return status;
}

#define MaxStaticArrayEntries(a) (sizeof(a)/sizeof(*a))

#define CacheArgumentImage(argp,cache,index,exception)	\
  {									\
    if (index < MaxStaticArrayEntries(cache))				\
      {									\
	if (LoadAndCacheImageFile(argp,&cache[index],exception)) \
	  index++;							\
      }									\
}


MagickExport unsigned int MogrifyImageCommand(ImageInfo *image_info,
  int argc,char **argv,char **metadata,ExceptionInfo *exception)
{

  char
    *format = NULL,
    *option = NULL,
    output_directory[MaxTextExtent];

  double
    sans;

  long
    image_cache[64],
    j,
    k,
    x;

  size_t
    image_cache_entries=0;    

  register long
    i;

  MagickBool
    create_directories,
    global_colormap,
    preserve_file_attr;

  unsigned int
    status;

  /*
    Check for sufficient arguments
  */  
  if (argc < 2 || ((argc < 3) && (LocaleCompare("-help",argv[1]) == 0 ||
      LocaleCompare("-?",argv[1]) == 0)))
    {
      MogrifyUsage();
      if (argc < 2)
	{
	  ThrowException(exception,OptionError,UsageError,NULL);
	  return MagickFail;
	}
      return MagickPass;
    }
  if (LocaleCompare("-version",argv[1]) == 0)
    {
      (void) VersionCommand(image_info,argc,argv,metadata,exception);
      return MagickPass;
    }

  /*
    Set defaults.
  */
  format=(char *) NULL;
  output_directory[0]='\0';
  create_directories=MagickFalse;
  global_colormap=MagickFalse;
  preserve_file_attr=MagickFalse;
  status=True;

  /*
    Expand argument list
  */
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickFatalError(ResourceLimitFatalError,MemoryAllocationFailed,
      (char *) NULL);

  j=1;
  k=0;
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      {
        TransmogrifyOptions
            transmogrify_options;
        /*
          Option is a file name: begin by reading image from specified file.
        */
        k=i;

        transmogrify_options.image_info=image_info;
        transmogrify_options.input_filename=argv[i];
        transmogrify_options.argc=i-j;
        transmogrify_options.argv=argv+j;
        transmogrify_options.output_format=format;
        transmogrify_options.output_directory=output_directory;
        transmogrify_options.create_directories=create_directories;
        transmogrify_options.global_colormap=global_colormap;
        transmogrify_options.preserve_file_attr=preserve_file_attr;
        transmogrify_options.status=MagickPass;
        GetExceptionInfo(&transmogrify_options.exception);
        status &= *TransmogrifyImage(&transmogrify_options);
        if (transmogrify_options.exception.severity > exception->severity)
          CopyException(exception,&transmogrify_options.exception);
        DestroyExceptionInfo(&transmogrify_options.exception);
        continue;
      }
    j=k+1;
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("affine",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("antialias",option+1) == 0)
          {
            image_info->antialias=(*option == '-');
            break;
          }
	if (LocaleCompare("asc-cdl",option+1) == 0)
          {
	    if (*option == '-')
              {
		i++;
		if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
		  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
	    break;
          }
        if (LocaleCompare("authenticate",option+1) == 0)
          {
            (void) CloneString(&image_info->authenticate,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->authenticate,argv[i]);
              }
            break;
          }
        if (LocaleCompare("auto-orient",option+1) == 0)
          break;
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'b':
      {
        if (LocaleCompare("background",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->background_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("black-threshold",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("blue-primary",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("blur",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowMogrifyException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("border",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->border_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("box",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'c':
      {
        if (LocaleCompare("channel",option+1) == 0)
          {
            if (*option == '-')
              {
                ChannelType
                  channel;

                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                channel=StringToChannelType(argv[i]);
                if (channel == UndefinedChannel)
                  ThrowMogrifyException(OptionError,UnrecognizedChannelType,
                    option);
              }
            break;
          }
        if (LocaleCompare("charcoal",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("chop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("colorize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("colors",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("colorspace",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->colorspace=StringToColorspaceType(option);
                if (image_info->colorspace == UndefinedColorspace)
                  ThrowMogrifyException(OptionError,UnrecognizedColorspace,option);
              }
            break;
          }
        if (LocaleCompare("comment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("compose",option+1) == 0)
          {
	    if (*option == '-')
              {
		i++;
		if (i == argc)
		  ThrowMogrifyException(OptionError,MissingArgument,option);
		option=argv[i];
		if ((StringToCompositeOperator(option)) == UndefinedCompositeOp)
		  ThrowMogrifyException(OptionError,UnrecognizedComposeOperator,
					option);
	      }
            break;
          }
        if (LocaleCompare("compress",option+1) == 0)
          {
            image_info->compression=NoCompression;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->compression=StringToCompressionType(option);
                if (image_info->compression == UndefinedCompression)
                  ThrowMogrifyException(OptionError,UnrecognizedImageCompression,
                    option);
              }
            break;
          }
        if (LocaleCompare("contrast",option+1) == 0)
          break;
        if (LocaleCompare("convolve",option+1) == 0)
          {
           if (*option == '-')
             {
               i++;
               if (i == (argc-1))
                 ThrowMogrifyException(OptionError,MissingArgument,
                   option);
             }
           break;
         }
        if (LocaleCompare("create-directories",option+1) == 0)
          {
            create_directories=(*option == '-');
            break;
          }
        if (LocaleCompare("crop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("cycle",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            (void) SetLogEventMask("None");
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                (void) SetLogEventMask(argv[i]);
              }
            break;
          }
        if (LocaleCompare("define",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowMogrifyException(OptionError,MissingArgument,option);
            if (*option == '+')
              (void) RemoveDefinitions(image_info,argv[i]);
            else
              (void) AddDefinitions(image_info,argv[i],exception);
            break;
          }
        if (LocaleCompare("delay",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            (void) CloneString(&image_info->density,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                (void) CloneString(&image_info->density,argv[i]);
              }
            break;
          }
        if (LocaleCompare("depth",option+1) == 0)
          {
            image_info->depth=QuantumDepth;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                image_info->depth=MagickAtoL(argv[i]);
              }
            break;
          }
        if (LocaleCompare("despeckle",option+1) == 0)
          break;
        if (LocaleCompare("display",option+1) == 0)
          {
            (void) CloneString(&image_info->server_name,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->server_name,argv[i]);
              }
            break;
          }
        if (LocaleCompare("dispose",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                option=argv[i];
                if ((LocaleCompare("0",option) != 0) &&
                    (LocaleCompare("1",option) != 0) &&
                    (LocaleCompare("2",option) != 0) &&
                    (LocaleCompare("3",option) != 0) &&
                    (LocaleCompare("Undefined",option) != 0) &&
                    (LocaleCompare("None",option) != 0) &&
                    (LocaleCompare("Background",option) != 0) &&
                    (LocaleCompare("Previous",option) != 0))
                  ThrowMogrifyException(OptionError,UnrecognizedDisposeMethod,
                    option);
              }
            break;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            image_info->dither=(*option == '-');
            break;
          }
        if (LocaleCompare("draw",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'e':
      {
        if (LocaleCompare("edge",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("emboss",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("encoding",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("endian",option+1) == 0)
          {
            image_info->endian=UndefinedEndian;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->endian=StringToEndianType(option);
                if (image_info->endian == UndefinedEndian)
                  ThrowMogrifyException(OptionError,UnrecognizedEndianType,
                    option);
              }
            break;
          }
        if (LocaleCompare("enhance",option+1) == 0)
          break;
        if (LocaleCompare("equalize",option+1) == 0)
          break;
       if (LocaleCompare("extent",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'f':
      {
        if (LocaleCompare("fill",option+1) == 0)
          {
            (void) QueryColorDatabase("none",&image_info->pen,exception);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                (void) QueryColorDatabase(argv[i],&image_info->pen,exception);
              }
            break;
          }
        if (LocaleCompare("filter",option+1) == 0)
          {
            if (*option == '-')
              {
                FilterTypes
                  filter;

                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                option=argv[i];
                filter=StringToFilterTypes(option);
                if (filter == UndefinedFilter)
                  ThrowMogrifyException(OptionError,UnrecognizedImageFilter,
                    option);
              }
            break;
          }
        if (LocaleCompare("flip",option+1) == 0)
          break;
        if (LocaleCompare("flop",option+1) == 0)
          break;
        if (LocaleCompare("font",option+1) == 0)
          {
            (void) CloneString(&image_info->font,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                (void) CloneString(&image_info->font,argv[i]);
              }
            break;
          }
        if (LocaleCompare("format",option+1) == 0)
          {
            (void) CloneString(&format,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                (void) CloneString(&format,argv[i]);
                (void) strlcpy(image_info->filename,format,MaxTextExtent);
                (void) strcat(image_info->filename,":");
                (void) SetImageInfo(image_info,SETMAGICK_WRITE,exception);
                if (*image_info->magick == '\0')
                  ThrowMogrifyException(OptionError,UnrecognizedImageFormat,format);
              }
            break;
          }
        if (LocaleCompare("frame",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("fuzz",option+1) == 0)
          {
            image_info->fuzz=0.0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                image_info->fuzz=StringToDouble(argv[i],MaxRGB);
              }
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'g':
      {
        if (LocaleCompare("gamma",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowMogrifyException(OptionError,MissingArgument,option);
            break;
          }
        if ((LocaleCompare("gaussian",option+1) == 0) ||
            (LocaleCompare("gaussian-blur",option+1) == 0))
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowMogrifyException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("geometry",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("gravity",option+1) == 0)
          {
            GravityType
              gravity;

            gravity=ForgetGravity;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                option=argv[i];
		gravity=StringToGravityType(option);
                if (gravity == ForgetGravity)
                  ThrowMogrifyException(OptionError,UnrecognizedGravityType,
                    option);
              }
            break;
          }
        if (LocaleCompare("green-primary",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'h':
      {
	if (LocaleCompare("hald-clut",option+1) == 0)
          {
	    if (*option == '-')
              {
		i++;
		if (i == argc)
		  ThrowMogrifyException(OptionError,MissingArgument,option);

		/*
		  Cache argument image.
		*/
		CacheArgumentImage(&argv[i],image_cache,image_cache_entries,
				   exception);
              }
	    break;
          }
	if (LocaleCompare("help",option+1) == 0)
          break;
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'i':
      {
        if (LocaleCompare("implode",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("interlace",option+1) == 0)
          {
            image_info->interlace=UndefinedInterlace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->interlace=StringToInterlaceType(option);
                if (image_info->interlace == UndefinedInterlace)
                  ThrowMogrifyException(OptionError,UnrecognizedInterlaceType,
                    option);
              }
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'l':
      {
        if (LocaleCompare("label",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("lat",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("level",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
              ThrowMogrifyException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("linewidth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("limit",option+1) == 0)
          {
            if (*option == '-')
              {
                ResourceType
                  resource_type;

                char
                  *type;

                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                type=argv[i];
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                resource_type=StringToResourceType(type);
                if (resource_type == UndefinedResource)
                  ThrowMogrifyException(OptionError,UnrecognizedResourceType,type);
                (void) SetMagickResourceLimit(resource_type,MagickSizeStrToInt64(argv[i],1024));
              }
            break;
          }
        if (LocaleCompare("list",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                option=argv[i];
                switch (*option)
                {
                  case 'C':
                  case 'c':
                  {
                    if ((LocaleCompare("Color",option) == 0) ||
                        (LocaleCompare("Colors",option) == 0))
                      {
                        (void) ListColorInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowMogrifyException(OptionError,UnrecognizedListType,
                      option)
                  }
                  case 'D':
                  case 'd':
                  {
                    if ((LocaleCompare("Delegate",option) == 0) ||
                        (LocaleCompare("Delegates",option) == 0))
                      {
                        (void) ListDelegateInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowMogrifyException(OptionError,UnrecognizedListType,
                      option)
                  }
                  case 'F':
                  case 'f':
                  {
                    if ((LocaleCompare("Font",option) == 0) ||
                        (LocaleCompare("Fonts",option) == 0))
                      {
                        (void) ListTypeInfo((FILE *) NULL,exception);
                        break;
                      }
                    if ((LocaleCompare("Format",option) == 0) ||
                        (LocaleCompare("Formats",option) == 0))
                      {
                        (void) ListMagickInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowMogrifyException(OptionError,UnrecognizedListType,
                      option)
                  }
                  case 'M':
                  case 'm':
                  {
                    if (LocaleCompare("Magic",option) == 0)
                      {
                        (void) ListMagicInfo((FILE *) NULL,exception);
                        break;
                      }
#if defined(SupportMagickModules)
                    if ((LocaleCompare("Module",option) == 0) ||
                        (LocaleCompare("Modules",option) == 0))
                      {
                        (void) ListModuleInfo((FILE *) NULL,exception);
                        break;
                      }
#endif /* SupportMagickModules */
                    if (LocaleCompare("ModuleMap",option) == 0)
                      {
                        (void) ListModuleMap((FILE *) NULL,exception);
                        break;
                      }
                    ThrowMogrifyException(OptionError,UnrecognizedListType,
                      option)
                  }
                  case 'R':
                  case 'r':
                  {
                    if ((LocaleCompare("Resource",option) == 0) ||
                        (LocaleCompare("Resources",option) == 0))
                      {
                        (void) ListMagickResourceInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowMogrifyException(OptionError,UnrecognizedListType,
                      option)
                  }
                  case 'T':
                  case 't':
                  {
                    if (LocaleCompare("Type",option) == 0)
                      {
                        (void) ListTypeInfo((FILE *) NULL,exception);
                        break;
                      }
                    ThrowMogrifyException(OptionError,UnrecognizedListType,
                      option)
                  }
                  default:
                    ThrowMogrifyException(OptionError,UnrecognizedListType,
                      option)
                }
                status=MagickPass;
		goto mogrify_cleanup_and_return;
              }
            break;
          }
        if (LocaleCompare("log",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                (void) SetLogFormat(argv[i]);
              }
            break;
          }
        if (LocaleCompare("loop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'm':
      {
        if (LocaleCompare("magnify",option+1) == 0)
          break;
        if (LocaleCompare("map",option+1) == 0)
          {
            global_colormap=(*option == '+');
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);

		/*
		  Cache argument image.
		*/
		CacheArgumentImage(&argv[i],image_cache,image_cache_entries,
				   exception);
              }
            break;
          }
        if (LocaleCompare("mask",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);

		/*
		  Cache argument image.
		*/
		CacheArgumentImage(&argv[i],image_cache,image_cache_entries,
				   exception);
              }
            break;
          }
        if (LocaleCompare("matte",option+1) == 0)
          break;
        if (LocaleCompare("mattecolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->matte_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("modulate",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("median",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("minify",option+1) == 0)
          break;
        if (LocaleCompare("monitor",option+1) == 0)
          {
            if (*option == '+')
	      {
		(void) SetMonitorHandler((MonitorHandler) NULL);
		(void) MagickSetConfirmAccessHandler((ConfirmAccessHandler) NULL);
	      }
            else
	      {
		(void) SetMonitorHandler(CommandProgressMonitor);
		(void) MagickSetConfirmAccessHandler(CommandAccessMonitor);
	      }
            break;
          }
        if (LocaleCompare("monochrome",option+1) == 0)
          {
            image_info->monochrome=(*option == '-');
            break;
          }
        if (LocaleCompare("motion-blur",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowMogrifyException(OptionError,MissingArgument,option);
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'n':
      {
        if (LocaleCompare("negate",option+1) == 0)
          break;
        if (LocaleCompare("noise",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            if (*option == '+')
              {
                NoiseType
                  noise_type;

                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                option=argv[i];
		noise_type=StringToNoiseType(option);
		if (UndefinedNoise == noise_type)
                  ThrowMogrifyException(OptionError,UnrecognizedNoiseType,
                    option);
              }
            break;
          }
        if (LocaleCompare("noop",option+1) == 0)
          break;
        if (LocaleCompare("normalize",option+1) == 0)
          break;
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'o':
      {
        if (LocaleCompare("opaque",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("operator",option+1) == 0)
          {
            if (*option == '-')
              {
                ChannelType
                  channel;

                QuantumOperator
                  quantum_operator;

                double
                  rvalue;

                /* channel */
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                channel=StringToChannelType(argv[i]);
                if (channel == UndefinedChannel)
                  ThrowMogrifyException(OptionError,UnrecognizedChannelType,
                    option);

                /* operator id */
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                quantum_operator=StringToQuantumOperator(argv[i]);
                if (quantum_operator == UndefinedQuantumOp)
                  ThrowMogrifyException(OptionError,UnrecognizedOperator,
                    option);

                /* rvalue */
                i++;
                if ((i == argc) || !sscanf(argv[i],"%lf",&rvalue))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("ordered-dither",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("orient",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowMogrifyException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("output-directory",option+1) == 0)
          {
	    output_directory[0]='\0';
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
		(void) strlcpy(output_directory,argv[i],sizeof(output_directory));
              }
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'p':
      {
        if (LocaleCompare("page",option+1) == 0)
          {
            (void) CloneString(&image_info->page,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                image_info->page=GetPageGeometry(argv[i]);
              }
            break;
          }
        if (LocaleCompare("paint",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("pointsize",option+1) == 0)
          {
            image_info->pointsize=12;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                image_info->pointsize=MagickAtoF(argv[i]);
              }
            break;
          }
        if (LocaleCompare("preserve-timestamp", option+1) == 0)
          {
            preserve_file_attr = MagickTrue;
            break;
          }
        if (LocaleCompare("profile",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowMogrifyException(OptionError,MissingArgument,option);
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'q':
      {
        if (LocaleCompare("quality",option+1) == 0)
          {
            image_info->quality=DefaultCompressionQuality;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                image_info->quality=MagickAtoL(argv[i]);
              }
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'r':
      {
        if (LocaleCompare("raise",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowMogrifyException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("random-threshold",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
          break;
          }
        if (LocaleCompare("recolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("red-primary",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("region",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("render",option+1) == 0)
          break;
        if (LocaleCompare("repage",option+1) == 0)
          {
	    if (*option == '-')
	      {
		i++;
		if ((i == argc) || !IsGeometry(argv[i]))
		  ThrowMogrifyException(OptionError,MissingArgument,option);
	      }
            break;
          }
        if (LocaleCompare("resample",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == (argc-1)) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("resize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("resize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("roll",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("rotate",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowMogrifyException(OptionError,MissingArgument,option);
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 's':
      {
        if (LocaleCompare("sample",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("sampling-factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
                NormalizeSamplingFactor(image_info);
              }
            break;
          }
        if (LocaleCompare("scale",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("scene",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("set",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowMogrifyException(OptionError,MissingArgument,option);
            if (*option == '-')
              {
                /* -set attribute value */
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("segment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("shade",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowMogrifyException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("sharpen",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowMogrifyException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("shave",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("shear",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowMogrifyException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("size",option+1) == 0)
          {
            (void) CloneString(&image_info->size,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                (void) CloneString(&image_info->size,argv[i]);
              }
            break;
          }
        if (LocaleCompare("solarize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("spread",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("strip",option+1) == 0)
	  {
	    break;
	  }
        if (LocaleCompare("stroke",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("strokewidth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("swirl",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 't':
      {
        if (LocaleCompare("texture",option+1) == 0)
          {
            (void) CloneString(&image_info->texture,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                (void) CloneString(&image_info->texture,argv[i]);
              }
            break;
          }
        if (LocaleCompare("threshold",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("thumbnail",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("tile",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowMogrifyException(OptionError,MissingArgument,option);

	    /*
	      Cache argument image.
	    */
	    CacheArgumentImage(&argv[i],image_cache,image_cache_entries,
			       exception);
            break;
          }
        if (LocaleCompare("transform",option+1) == 0)
          break;
        if (LocaleCompare("transparent",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("treedepth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("trim",option+1) == 0)
          break;
        if (LocaleCompare("type",option+1) == 0)
          {
            image_info->type=UndefinedType;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->type=StringToImageType(option);
                if (image_info->type == UndefinedType)
                  ThrowMogrifyException(OptionError,UnrecognizedImageType,
                    option);
              }
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'u':
      {
        if (LocaleCompare("undercolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("units",option+1) == 0)
          {
            image_info->units=UndefinedResolution;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->units=UndefinedResolution;
                if (LocaleCompare("PixelsPerInch",option) == 0)
                  image_info->units=PixelsPerInchResolution;
                if (LocaleCompare("PixelsPerCentimeter",option) == 0)
                  image_info->units=PixelsPerCentimeterResolution;
              }
            break;
          }
        if (LocaleCompare("unsharp",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowMogrifyException(OptionError,MissingArgument,option);
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose+=(*option == '-');
            break;
          }
        if (LocaleCompare("view",option+1) == 0)
          {
            (void) CloneString(&image_info->view,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,
                    option);
                (void) CloneString(&image_info->view,argv[i]);
              }
            break;
          }
        if (LocaleCompare("virtual-pixel",option+1) == 0)
          {
            if (*option == '-')
              {
                VirtualPixelMethod
                  virtual_pixel_method;

                i++;
                if (i == argc)
                  ThrowMogrifyException(OptionError,MissingArgument,option);
                option=argv[i];
                virtual_pixel_method=StringToVirtualPixelMethod(option);
                if (virtual_pixel_method == UndefinedVirtualPixelMethod)
                  ThrowMogrifyException(OptionError,UnrecognizedVirtualPixelMethod,option);
              }
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case 'w':
      {
        if (LocaleCompare("wave",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowMogrifyException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("white-point",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("white-threshold",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMogrifyException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
      }
      case '?':
        break;
      default:
        ThrowMogrifyException(OptionError,UnrecognizedOption,option)
    }
  }
  if (i != argc)
    {
      if (exception->severity == UndefinedException)
        ThrowMogrifyException(OptionError,MissingAnImageFilename,
          (char *) NULL);
    }
 mogrify_cleanup_and_return:

  /*
    Release cached temporary files.
  */
  while (image_cache_entries > 0)
    (void) DeleteMagickRegistry(image_cache[--image_cache_entries]);
  MagickFreeMemory(format);
  LiberateArgumentList(argc,argv);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M o g r i f y U s a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MogrifyUsage() displays the program command syntax.
%
%  The format of the MogrifyUsage method is:
%
%      void MogrifyUsage()
%
%
*/
static void MogrifyUsage(void)
{
  static const char
    *options[]=
    {
      "-affine matrix       affine transform matrix",
      "-antialias           remove pixel-aliasing",
      "-asc-cdl spec        apply ASC CDL transform",
      "-authenticate value  decrypt image with this password",
      "-auto-orient         orient (rotate) image so it is upright",
      "-background color    background color",
      "-black-threshold value",
      "                     pixels below the threshold become black",
      "-blue-primary point  chomaticity blue primary point",
      "-blur radius         blur the image",
      "-border geometry     surround image with a border of color",
      "-bordercolor color   border color",
      "-box color           set the color of the annotation bounding box",
      "-channel type        extract a particular color channel from image",
      "-charcoal radius     simulate a charcoal drawing",
      "-chop geometry       remove pixels from the image interior",
      "-colorize value      colorize the image with the fill color",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     alternate image colorspace",
      "-comment string      annotate image with comment",
      "-compose operator    composite operator",
      "-compress type       image compression type",
      "-contrast            enhance or reduce the image contrast",
      "-convolve kernel     convolve image with the specified convolution kernel",
      "-create-directories  create output directories if required",
      "-crop geometry       preferred size and location of the cropped image",
      "-cycle amount        cycle the image colormap",
      "-debug events        display copious debugging information",
      "-define values       Coder/decoder specific options",
      "-delay value         display the next image after pausing",
      "-density geometry    horizontal and vertical density of the image",
      "-depth value         image depth",
      "-despeckle           reduce the speckles within an image",
      "-display server      get image or font from this X server",
      "-dispose method      Undefined, None, Background, Previous",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-draw string         annotate the image with a graphic primitive",
      "-edge radius         apply a filter to detect edges in the image",
      "-emboss radius       emboss an image",
      "-encoding type       text encoding type",
      "-endian type         multibyte word order (LSB, MSB, or Native)",
      "-enhance             apply a digital filter to enhance a noisy image",
      "-equalize            perform histogram equalization to an image",
      "-extent              composite image on background color canvas image",
      "-fill color          color to use when filling a graphic primitive",
      "-filter type         use this filter when resizing an image",
      "-flip                flip image in the vertical direction",
      "-flop                flop image in the horizontal direction",
      "-font name           render text with this font",
      "-format type         image format type",
      "-frame geometry      surround image with an ornamental border",
      "-fuzz distance       colors within this distance are considered equal",
      "-gamma value         level of gamma correction",
      "-gaussian geometry   gaussian blur an image",
      "-geometry geometry   perferred size or location of the image",
      "-gravity type        horizontal and vertical text/object placement",
      "-green-primary point chomaticity green primary point",
      "-implode amount      implode image pixels about the center",
      "-interlace type      None, Line, Plane, or Partition",
      "-hald-clut clut      apply a Hald CLUT to the image",
      "-help                print program options",
      "-label name          assign a label to an image",
      "-lat geometry        local adaptive thresholding",
      "-level value         adjust the level of image contrast",
      "-limit type value    Disk, File, Map, Memory, Pixels, Width, Height or",
      "                     Threads resource limit",
      "-linewidth width     the line width for subsequent draw operations",
      "-list type           Color, Delegate, Format, Magic, Module, Resource,",
      "                     or Type",
      "-log format          format of debugging information",
      "-loop iterations     add Netscape loop extension to your GIF animation",
      "-magnify             interpolate image to double size",
      "-map filename        transform image colors to match this set of colors",
      "-mask filename       set the image clip mask",
      "-matte               store matte channel if the image has one",
      "-mattecolor color    specify the color to be used with the -frame option",
      "-median radius       apply a median filter to the image",
      "-minify              interpolate the image to half size",
      "-modulate value      vary the brightness, saturation, and hue",
      "-monitor             show progress indication",
      "-monochrome          transform image to black and white",
      "-motion-blur radiusxsigma+angle",
      "                     simulate motion blur",
      "-negate              replace every pixel with its complementary color ",
      "-noop                do not apply options to image",
      "-noise radius        add or reduce noise in an image",
      "-normalize           transform image to span the full range of colors",
      "-opaque color        change this color to the fill color",
      "-operator channel operator rvalue",
      "                     apply a mathematical or bitwise operator to channel",
      "-ordered-dither channeltype NxN",
      "                     ordered dither the image",
      "-orient orientation  set image orientation attribute",
      "-output-directory directory",
      "                     write output files to directory",
      "+page                reset current page offsets to default",
      "-page geometry       size and location of an image canvas",
      "-paint radius        simulate an oil painting",
      "-fill color           color for annotating or changing opaque color",
      "-pointsize value     font point size",
      "-profile filename    add ICM or IPTC information profile to image",
      "-preserve-timestamp  preserve original timestamps of the file",
      "-quality value       JPEG/MIFF/PNG compression level",
      "-raise value         lighten/darken image edges to create a 3-D effect",
      "-random-threshold channeltype LOWxHIGH",
      "                     random threshold the image",
      "-recolor matrix      apply a color translation matrix to image channels",
      "-red-primary point   chomaticity red primary point",
      "-region geometry     apply options to a portion of the image",
      "-render              render vector graphics",
      "+render              disable rendering vector graphics",
      "-resample geometry   resample to horizontal and vertical resolution",
      "+repage              reset current page offsets to default",
      "-repage geometry     adjust current page offsets by geometry",
      "-resize geometry     perferred size or location of the image",
      "-roll geometry       roll an image vertically or horizontally",
      "-rotate degrees      apply Paeth rotation to the image",
      "-sample geometry     scale image with pixel sampling",
      "-sampling-factor HxV[,...]",
      "                     horizontal and vertical sampling factors",
      "-scale geometry      scale the image",
      "-scene number        image scene number",
      "-seed value          pseudo-random number generator seed value",
      "-segment values      segment an image",
      "-set attribute value set image attribute",
      "+set attribute       unset image attribute",
      "-shade degrees       shade the image using a distant light source",
      "-sharpen radius      sharpen the image",
      "-shave geometry      shave pixels from the image edges",
      "-shear geometry      slide one edge of the image along the X or Y axis",
      "-size geometry       width and height of image",
      "-solarize threshold  negate all pixels above the threshold level",
      "-spread amount       displace image pixels by a random amount",
      "-strip               strip all profiles and text attributes from image",
      "-stroke color        graphic primitive stroke color",
      "-strokewidth value   graphic primitive stroke width",
      "-swirl degrees       swirl image pixels about the center",
      "-texture filename    name of texture to tile onto the image background",
      "-threshold value     threshold the image",
      "-thumbnail geometry  resize the image (optimized for thumbnails)",
      "-tile filename       tile image when filling a graphic primitive",
      "-transform           affine transform image",
      "-transparent color   make this color transparent within the image",
      "-treedepth value     color tree depth",
      "-trim                trim image edges",
      "-type type           image type",
      "-undercolor color    annotation bounding box color",
      "-units type          PixelsPerInch, PixelsPerCentimeter, or Undefined",
      "-unsharp geometry    sharpen the image",
      "-verbose             print detailed information about the image",
      "-version             print version information",
      "-view                FlashPix viewing transforms",
      "-virtual-pixel method",
      "                     Constant, Edge, Mirror, or Tile",
      "-wave geometry       alter an image along a sine wave",
      "-white-point point   chomaticity white point",
      "-white-threshold value",
      "                     pixels above the threshold become white",
      (char *) NULL
    };

  const char
    **p;

  PrintUsageHeader();
  (void) printf("Usage: %.1024s [options ...] file [ [options ...] file ...]\n",
    GetClientName());
  (void) printf("\nWhere options include: \n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf(
    "\nBy default, the image format of `file' is determined by its magic\n");
  (void) printf(
    "number.  To specify a particular image format, precede the filename\n");
  (void) printf(
    "with an image format name and a colon (i.e. ps:image) or specify the\n");
  (void) printf(
    "image type as the filename suffix (i.e. image.ps).  Specify 'file' as\n");
  (void) printf("'-' for standard input or output.\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    M o n t a g e I m a g e C o m m a n d                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MontageImageCommand() reads one or more images, applies one or more image
%  processing operations, and writes out the image in the same or
%  differing format.
%
%  The format of the MontageImageCommand method is:
%
%      unsigned int MontageImageCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define ThrowMontageException(code,reason,description) \
{ \
  DestroyImageList(image); \
  DestroyImageList(image_list); \
  DestroyImageList(montage_image); \
  DestroyMontageInfo(montage_info); \
  ThrowException(exception,code,reason,description); \
  LiberateArgumentList(argc,argv); \
  return(False); \
}
#define ThrowMontageException3(code,reason,description) \
{ \
  DestroyImageList(image); \
  DestroyImageList(image_list); \
  DestroyImageList(montage_image); \
  DestroyMontageInfo(montage_info); \
  ThrowException3(exception,code,reason,description); \
  LiberateArgumentList(argc,argv); \
  return(False); \
}
MagickExport unsigned int MontageImageCommand(ImageInfo *image_info,
  int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
  char
    *format,
    *option,
    *transparent_color;

  double
    sans;

  Image
    *image,
    *image_list,
    *montage_image,
    *next_image;

  long
    first_scene,
    j,
    k,
    last_scene,
    scene,
    x;

  MontageInfo
    *montage_info;

  QuantizeInfo
    quantize_info;

  register long
    i;

  unsigned int
    status;

  /*
    Validate command line.
  */
  if (argc < 2 || ((argc < 3) && (LocaleCompare("-help",argv[1]) == 0 ||
      LocaleCompare("-?",argv[1]) == 0)))
    {
      MontageUsage();
      if (argc < 2)
	{
	  ThrowException(exception,OptionError,UsageError,NULL);
	  return MagickFail;
	}
      return MagickPass;
    }

  /*
    Set defaults.
  */
  format=(char *) NULL;
  first_scene=0;
  image=NewImageList();
  image_list=(Image *) NULL;
  montage_image=NewImageList();
  last_scene=0;

  /*
    Expand argument list
  */
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickFatalError(ResourceLimitFatalError,MemoryAllocationFailed,
      (char *) NULL);

  (void) strlcpy(image_info->filename,argv[argc-1],MaxTextExtent);
  (void) SetImageInfo(image_info,SETMAGICK_WRITE,exception);
  montage_info=CloneMontageInfo(image_info,(MontageInfo *) NULL);
  GetQuantizeInfo(&quantize_info);
  quantize_info.number_colors=0;
  scene=0;
  status=True;
  transparent_color=(char *) NULL;

  j=1;
  k=0;
  for (i=1; i < (argc-1); i++)
  {
    option=argv[i];
    if ((strlen(option) < 2) ||
	/* stdin + subexpression */
	((option[0] == '-') && (option[1] == '[')) ||
	((option[0] != '-') && option[0] != '+'))
      {
        k=i;
        for (scene=first_scene; scene <= last_scene ; scene++)
        {
          /*
            Option is a file name: begin by reading image from specified file.
          */
          (void) strlcpy(image_info->filename,argv[i],MaxTextExtent);
          if (first_scene != last_scene)
            {
              char
                filename[MaxTextExtent];

              /*
                Form filename for multi-part images.
              */
              (void) MagickSceneFileName(filename,image_info->filename,".%lu",MagickTrue,scene);
              (void) strlcpy(image_info->filename,filename,MaxTextExtent);
            }
          (void) CloneString(&image_info->font,montage_info->font);
          image_info->colorspace=quantize_info.colorspace;
          image_info->dither=quantize_info.dither;
          if (image_info->size == (char *) NULL)
            (void) CloneString(&image_info->size,montage_info->geometry);
          next_image=ReadImage(image_info,exception);
          status&=(next_image != (Image *) NULL) &&
            (exception->severity < ErrorException);
          if (next_image == (Image *) NULL)
            continue;
          if (image == (Image *) NULL)
            {
              image=next_image;
              continue;
            }
          AppendImageToList(&image,next_image);
        }
        continue;
      }
    if ((image != (Image *) NULL) && (j != (k+1)))
      {
        status&=MogrifyImages(image_info,i-j,argv+j,&image);
        GetImageException(image,exception);
        AppendImageToList(&image_list,image);
        image=NewImageList();
        j=k+1;
      }
    switch (*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("adjoin",option+1) == 0)
          {
            image_info->adjoin=(*option == '-');
            break;
          }
        if (LocaleCompare("affine",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("authenticate",option+1) == 0)
          {
            (void) CloneString(&image_info->authenticate,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->authenticate,argv[i]);
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'b':
      {
        if (LocaleCompare("background",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) QueryColorDatabase(argv[i],
                  &montage_info->background_color,exception);
                (void) QueryColorDatabase(argv[i],&image_info->background_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("blue-primary",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("blur",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) QueryColorDatabase(argv[i],&montage_info->border_color,
                  exception);
                (void) QueryColorDatabase(argv[i],&image_info->border_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("borderwidth",option+1) == 0)
          {
            montage_info->border_width=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMontageException(OptionError,MissingArgument,option);
                montage_info->border_width=MagickAtoL(argv[i]);
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'c':
      {
        if (LocaleCompare("colors",option+1) == 0)
          {
            quantize_info.number_colors=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMontageException(OptionError,MissingArgument,option);
                quantize_info.number_colors=MagickAtoL(argv[i]);
              }
            break;
          }
        if (LocaleCompare("colorspace",option+1) == 0)
          {
            quantize_info.colorspace=RGBColorspace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                option=argv[i];
                quantize_info.colorspace=StringToColorspaceType(option);
                if (IsGrayColorspace(quantize_info.colorspace))
                  {
                    quantize_info.number_colors=256;
                    quantize_info.tree_depth=8;
                  }
                if (quantize_info.colorspace == UndefinedColorspace)
                  ThrowMontageException(OptionError,UnrecognizedColorspace,
                    option);
              }
            break;
          }
        if (LocaleCompare("comment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("compose",option+1) == 0)
          {
            CompositeOperator
              compose;

            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                option=argv[i];
                compose=StringToCompositeOperator(option);
                if (compose == UndefinedCompositeOp)
                  ThrowMontageException(OptionError,UnrecognizedComposeOperator,
                    option);
              }
            break;
          }
        if (LocaleCompare("compress",option+1) == 0)
          {
            image_info->compression=NoCompression;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->compression=StringToCompressionType(option);
                if (image_info->compression == UndefinedCompression)
                  ThrowMontageException(OptionError,UnrecognizedImageCompression,
                    option);
              }
            break;
          }
        if (LocaleCompare("crop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            (void) SetLogEventMask("None");
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) SetLogEventMask(argv[i]);
              }
            break;
          }
        if (LocaleCompare("define",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowMontageException(OptionError,MissingArgument,option);
            if (*option == '+')
              (void) RemoveDefinitions(image_info,argv[i]);
            else
              (void) AddDefinitions(image_info,argv[i],exception);
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            (void) CloneString(&image_info->density,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->density,argv[i]);
              }
            break;
          }
        if (LocaleCompare("depth",option+1) == 0)
          {
            image_info->depth=QuantumDepth;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMontageException(OptionError,MissingArgument,option);
                image_info->depth=MagickAtoL(argv[i]);
              }
            break;
          }
        if (LocaleCompare("display",option+1) == 0)
          {
            (void) CloneString(&image_info->server_name,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,
                    option);
                (void) CloneString(&image_info->server_name,argv[i]);
              }
            break;
          }
        if (LocaleCompare("dispose",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,
                    option);
                option=argv[i];
                if ((LocaleCompare("0",option) != 0) &&
                    (LocaleCompare("1",option) != 0) &&
                    (LocaleCompare("2",option) != 0) &&
                    (LocaleCompare("3",option) != 0) &&
                    (LocaleCompare("Undefined",option) != 0) &&
                    (LocaleCompare("None",option) != 0) &&
                    (LocaleCompare("Background",option) != 0) &&
                    (LocaleCompare("Previous",option) != 0))
                  ThrowMontageException(OptionError,UnrecognizedDisposeMethod,
                    option);
              }
            break;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            quantize_info.dither=(*option == '-');
            break;
          }
        if (LocaleCompare("draw",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'e':
      {
        if (LocaleCompare("encoding",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("endian",option+1) == 0)
          {
            image_info->endian=UndefinedEndian;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->endian=StringToEndianType(option);
                if (image_info->endian == UndefinedEndian)
                  ThrowMontageException(OptionError,UnrecognizedEndianType,
                    option);
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'f':
      {
        if (LocaleCompare("fill",option+1) == 0)
          {
            (void) QueryColorDatabase("none",&image_info->pen,exception);
            (void) QueryColorDatabase("none",&montage_info->fill,exception);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) QueryColorDatabase(argv[i],&image_info->pen,exception);
                (void) QueryColorDatabase(argv[i],&montage_info->fill,
                  exception);
              }
            break;
          }
        if (LocaleCompare("filter",option+1) == 0)
          {
            if (*option == '-')
              {
                FilterTypes
                  filter;

                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                option=argv[i];
                filter=StringToFilterTypes(option);
                if (filter == UndefinedFilter)
                  ThrowMontageException(OptionError,UnrecognizedImageFilter,
                    option);
              }
            break;
          }
        if (LocaleCompare("flip",option+1) == 0)
          break;
        if (LocaleCompare("flop",option+1) == 0)
          break;
        if (LocaleCompare("font",option+1) == 0)
          {
            (void) CloneString(&image_info->font,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->font,argv[i]);
                (void) CloneString(&montage_info->font,argv[i]);
              }
            break;
          }
        if (LocaleCompare("format",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,
                    option);
                format=argv[i];
              }
            break;
          }
        if (LocaleCompare("frame",option+1) == 0)
          {
            (void) CloneString(&montage_info->frame,(char *) NULL);
            (void) strcpy(argv[i]+1,"sans");
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) CloneString(&montage_info->frame,argv[i]);
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'g':
      {
        if (LocaleCompare("gamma",option+1) == 0)
          {
            i++;
            if ((i == argc) || !sscanf(argv[i],"%lf",&sans))
              ThrowMontageException(OptionError,MissingArgument,option);
            break;
          }
        if (LocaleCompare("geometry",option+1) == 0)
          {
            (void) CloneString(&montage_info->geometry,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) CloneString(&montage_info->geometry,argv[i]);
              }
            break;
          }
        if (LocaleCompare("gravity",option+1) == 0)
          {
            GravityType
              gravity;

            gravity=ForgetGravity;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                option=argv[i];
                gravity=StringToGravityType(option);
              }
            montage_info->gravity=gravity;
            break;
          }
        if (LocaleCompare("green-primary",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'h':
      {
        if (LocaleCompare("help",option+1) == 0)
          break;
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'i':
      {
        if (LocaleCompare("interlace",option+1) == 0)
          {
            image_info->interlace=UndefinedInterlace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->interlace=StringToInterlaceType(option);
                if (image_info->interlace == UndefinedInterlace)
                  ThrowMontageException(OptionError,UnrecognizedInterlaceType,option);
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'l':
      {
        if (LocaleCompare("label",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("limit",option+1) == 0)
          {
            if (*option == '-')
              {
                ResourceType
                  resource_type;

                char
                  *type;

                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                type=argv[i];
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMontageException(OptionError,MissingArgument,option);
                resource_type=StringToResourceType(type);
                if (resource_type == UndefinedResource)
                  ThrowMontageException(OptionError,UnrecognizedResourceType,type);
                (void) SetMagickResourceLimit(resource_type,MagickSizeStrToInt64(argv[i],1024));
              }
            break;
          }
        if (LocaleCompare("log",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) SetLogFormat(argv[i]);
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'm':
      {
        if (LocaleCompare("matte",option+1) == 0)
          break;
        if (LocaleCompare("mattecolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) QueryColorDatabase(argv[i],&montage_info->matte_color,
                  exception);
                (void) QueryColorDatabase(argv[i],&image_info->matte_color,
                  exception);
              }
            break;
          }
        if (LocaleCompare("mode",option+1) == 0)
          {
            if (*option == '-')
              {
                MontageMode
                  mode;

                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                option=argv[i];
                mode=UndefinedMode;
                if (LocaleCompare("frame",option) == 0)
                  {
                    mode=FrameMode;
                    (void) CloneString(&montage_info->frame,"15x15+3+3");
                    montage_info->shadow=True;
                    break;
                  }
                if (LocaleCompare("unframe",option) == 0)
                  {
                    mode=UnframeMode;
                    montage_info->frame=(char *) NULL;
                    montage_info->shadow=False;
                    montage_info->border_width=0;
                    break;
                  }
                if (LocaleCompare("concatenate",option) == 0)
                  {
                    mode=ConcatenateMode;
                    montage_info->frame=(char *) NULL;
                    montage_info->shadow=False;
                    (void) CloneString(&montage_info->geometry,"+0+0");
                    montage_info->border_width=0;
                    break;
                  }
                if (mode == UndefinedMode)
                  ThrowMontageException(OptionError,UnrecognizedImageMode,
                    option);
              }
            break;
          }
        if (LocaleCompare("monitor",option+1) == 0)
          {
            if (*option == '+')
	      {
		(void) SetMonitorHandler((MonitorHandler) NULL);
		(void) MagickSetConfirmAccessHandler((ConfirmAccessHandler) NULL);
	      }
            else
	      {
		(void) SetMonitorHandler(CommandProgressMonitor);
		(void) MagickSetConfirmAccessHandler(CommandAccessMonitor);
	      }
            break;
          }
        if (LocaleCompare("monochrome",option+1) == 0)
          {
            image_info->monochrome=(*option == '-');
            if (image_info->monochrome)
              {
                quantize_info.number_colors=2;
                quantize_info.tree_depth=8;
                quantize_info.colorspace=GRAYColorspace;
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'n':
      {
        if (LocaleCompare("noop",option+1) == 0)
          break;
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'p':
      {
        if (LocaleCompare("page",option+1) == 0)
          {
            (void) CloneString(&image_info->page,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,
                    option);
                image_info->page=GetPageGeometry(argv[i]);
              }
            break;
          }
        if (LocaleCompare("pointsize",option+1) == 0)
          {
            image_info->pointsize=12;
            montage_info->pointsize=12;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,option);
                image_info->pointsize=MagickAtoF(argv[i]);
                montage_info->pointsize=MagickAtoF(argv[i]);
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'q':
      {
        if (LocaleCompare("quality",option+1) == 0)
          {
            image_info->quality=DefaultCompressionQuality;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,
                    option);
                image_info->quality=MagickAtoL(argv[i]);
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'r':
      {
        if (LocaleCompare("red-primary",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("render",option+1) == 0)
          break;
        if (LocaleCompare("repage",option+1) == 0)
          {
	    if (*option == '-')
	      {
		i++;
		if ((i == argc) || !IsGeometry(argv[i]))
		  ThrowMontageException(OptionError,MissingArgument,option);
	      }
            break;
          }
        if (LocaleCompare("resize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("rotate",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
              ThrowMontageException(OptionError,MissingArgument,option);
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 's':
      {
        if (LocaleCompare("sampling-factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
                NormalizeSamplingFactor(image_info);
              }
            break;
          }
        if (LocaleCompare("scenes",option+1) == 0)
          {
            first_scene=0;
            last_scene=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMontageException(OptionError,MissingArgument,option);
                first_scene=MagickAtoL(argv[i]);
                last_scene=first_scene;
                (void) sscanf(argv[i],"%ld-%ld",&first_scene,&last_scene);
              }
            break;
          }
        if (LocaleCompare("set",option+1) == 0)
          {
            i++;
            if (i == argc)
              ThrowMontageException(OptionError,MissingArgument,option);
            if (*option == '-')
              {
                /* -set attribute value */
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("shadow",option+1) == 0)
          {
            montage_info->shadow=(*option == '-');
            break;
          }
        if (LocaleCompare("sharpen",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("size",option+1) == 0)
          {
            (void) CloneString(&image_info->size,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) CloneString(&image_info->size,argv[i]);
              }
            break;
          }
        if (LocaleCompare("strip",option+1) == 0)
	  {
	    break;
	  }
        if (LocaleCompare("stroke",option+1) == 0)
          {
            (void) QueryColorDatabase("none",&montage_info->stroke,exception);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) QueryColorDatabase(argv[i],&montage_info->stroke,
                  exception);
              }
            break;
          }
        if (LocaleCompare("strokewidth",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMontageException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 't':
      {
        if (LocaleCompare("texture",option+1) == 0)
          {
            (void) CloneString(&montage_info->texture,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) CloneString(&montage_info->texture,argv[i]);
              }
            break;
          }
        if (LocaleCompare("thumbnail",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("tile",option+1) == 0)
          {
            (void) CloneString(&montage_info->tile,(char *) NULL);
            (void) strcpy(argv[i]+1,"sans");
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) CloneString(&montage_info->tile,argv[i]);
              }
            break;
          }
        if (LocaleCompare("title",option+1) == 0)
          {
            (void) CloneString(&montage_info->title,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) CloneString(&montage_info->title,argv[i]);
              }
            break;
          }
        if (LocaleCompare("transform",option+1) == 0)
          break;
        if (LocaleCompare("transparent",option+1) == 0)
          {
            transparent_color=(char *) NULL;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                (void) CloneString(&transparent_color,argv[i]);
              }
            break;
          }
        if (LocaleCompare("treedepth",option+1) == 0)
          {
            quantize_info.tree_depth=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  ThrowMontageException(OptionError,MissingArgument,option);
                quantize_info.tree_depth=MagickAtoI(argv[i]);
              }
            break;
          }
        if (LocaleCompare("trim",option+1) == 0)
          break;
        if (LocaleCompare("type",option+1) == 0)
          {
            image_info->type=UndefinedType;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,option);
                option=argv[i];
                image_info->type=StringToImageType(option);
                if (image_info->type == UndefinedType)
                  ThrowMontageException(OptionError,UnrecognizedImageType,
                    option);
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose+=(*option == '-');
            break;
          }
        if (LocaleCompare("verbose",option+1) == 0)
          break;
        if (LocaleCompare("virtual-pixel",option+1) == 0)
          {
            if (*option == '-')
              {
                VirtualPixelMethod
                  virtual_pixel_method;

                i++;
                if (i == argc)
                  ThrowMontageException(OptionError,MissingArgument,
                    option);
                option=argv[i];
                virtual_pixel_method=StringToVirtualPixelMethod(option);
                if (virtual_pixel_method == UndefinedVirtualPixelMethod)
                  ThrowMontageException(OptionError,UnrecognizedVirtualPixelMethod,option);
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case 'w':
      {
        if (LocaleCompare("white-point",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  ThrowMontageException(OptionError,MissingArgument,option);
              }
            break;
          }
        ThrowMontageException(OptionError,UnrecognizedOption,option)
      }
      case '?':
        break;
      default:
        ThrowMontageException(OptionError,UnrecognizedOption,option)
    }
  }
  if ((image == (Image *) NULL) && (image_list == (Image *) NULL))
    {
      if (exception->severity == UndefinedException)
        ThrowMontageException(OptionError,RequestDidNotReturnAnImage,
          (char *) NULL);
      return(False);
    }
  if (i != (argc-1))
    ThrowMontageException(OptionError,MissingAnImageFilename,(char *) NULL);
  if (image != (Image *) NULL)
    {
      status&=MogrifyImages(image_info,i-j,argv+j,&image);
      GetImageException(image,exception);
      AppendImageToList(&image_list,image);
      image=NewImageList();
      j=i;
    }
  /*
    FIXME: Overlapping memory detected here where memory should not be overlapping.
   */
  (void) strlcpy(montage_info->filename,argv[argc-1],MaxTextExtent);
  /* (void) memmove(montage_info->filename,argv[argc-1],strlen(argv[argc-1])+1); */
  montage_image=MontageImages(image_list,montage_info,exception);
  if (montage_image == (Image *) NULL)
    ThrowMontageException(OptionError,RequestDidNotReturnAnImage,(char *) NULL);
  DestroyImageList(image_list);
  /*
    Write image.
  */
  status&=MogrifyImages(image_info,i-j,argv+j,&montage_image);
  GetImageException(montage_image,exception);
  (void) strlcpy(image_info->filename,argv[argc-1],MaxTextExtent);
  (void) strlcpy(montage_image->magick_filename,argv[argc-1],MaxTextExtent);
  status&=WriteImages(image_info,montage_image,argv[argc-1],exception);
  if (metadata != (char **) NULL)
    {
      char
        *text;

      /*
	Return metadata to user
      */
      text=TranslateText(image_info,montage_image,(format != (char *) NULL) ? format : "%w,%h,%m");
      if (text == (char *) NULL)
        ThrowMontageException(ResourceLimitError,MemoryAllocationFailed,
          MagickMsg(OptionError,UnableToFormatImageMetadata));
      (void) ConcatenateString(&(*metadata),text);
      (void) ConcatenateString(&(*metadata),"\n");
      MagickFreeMemory(text);
    }
  DestroyImageList(montage_image);
  DestroyMontageInfo(montage_info);
  LiberateArgumentList(argc,argv);
  return(status);
}
#undef ThrowMontageException
#undef ThrowMontageException3

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   M o n t a g e U s a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MontageUsage() displays the program command syntax.
%
%  The format of the MontageUsage method is:
%
%      void MontageUsage()
%
%
*/
static void MontageUsage(void)
{
  const char
    **p;

  static const char
    *options[]=
    {
      "-adjoin              join images into a single multi-image file",
      "-affine matrix       affine transform matrix",
      "-authenticate value  decrypt image with this password",
      "-background color    background color",
      "-blue-primary point  chomaticity blue primary point",
      "-blur factor         apply a filter to blur the image",
      "-bordercolor color   border color",
      "-borderwidth geometry",
      "                     border width",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     alternate image colorsapce",
      "-comment string      annotate image with comment",
      "-compose operator    composite operator",
      "-compress type       image compression type",
      "-crop geometry       preferred size and location of the cropped image",
      "-debug events        display copious debugging information",
      "-define values       Coder/decoder specific options",
      "-density geometry    horizontal and vertical density of the image",
      "-depth value         image depth",
      "-display server      query font from this X server",
      "-dispose method      Undefined, None, Background, Previous",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-draw string         annotate the image with a graphic primitive",
      "-encoding type       text encoding type",
      "-endian type         multibyte word order (LSB, MSB, or Native)",
      "-fill color          color to use when filling a graphic primitive",
      "-filter type         use this filter when resizing an image",
      "-flip                flip image in the vertical direction",
      "-flop                flop image in the horizontal direction",
      "-font name           font to use when annotating with text",
      "-format string       output formatted image characteristics",
      "-frame geometry      surround image with an ornamental border",
      "-gamma value         level of gamma correction",
      "-geometry geometry   preferred tile and border sizes",
      "-gravity direction   which direction to gravitate towards",
      "-green-primary point chomaticity green primary point",
      "-help                print program options",
      "-interlace type      None, Line, Plane, or Partition",
      "-label name          assign a label to an image",
      "-limit type value    Disk, File, Map, Memory, Pixels, Width, Height or",
      "                     Threads resource limit",
      "-log format          format of debugging information",
      "-matte               store matte channel if the image has one",
      "-mattecolor color    color to be used with the -frame option",
      "-mode type           Frame, Unframe, or Concatenate",
      "-monitor             show progress indication",
      "-monochrome          transform image to black and white",
      "-noop                do not apply options to image",
      "+page                reset current page offsets to default",
      "-page geometry       size and location of an image canvas",
      "-pointsize value     font point size",
      "-quality value       JPEG/MIFF/PNG compression level",
      "-red-primary point   chomaticity red primary point",
      "+repage              reset current page offsets to default",
      "-repage geometry     adjust current page offsets by geometry",
      "-resize geometry     resize the image",
      "-rotate degrees      apply Paeth rotation to the image",
      "-sampling-factor HxV[,...]",
      "                     horizontal and vertical sampling factors",
      "-scenes range        image scene range",
      "-set attribute value set image attribute",
      "+set attribute       unset image attribute",
      "-shadow              add a shadow beneath a tile to simulate depth",
      "-sharpen geometry    sharpen the image",
      "-size geometry       width and height of image",
      "-strip               strip all profiles and text attributes from image",
      "-stroke color        color to use when stroking a graphic primitive",
      "-strokewidth value   stroke (line) width",
      "-texture filename    name of texture to tile onto the image background",
      "-thumbnail geometry  resize the image (optimized for thumbnails)",
      "-tile geometry       number of tiles per row and column",
      "-title string        thumbnail title",
      "-transform           affine transform image",
      "-transparent color   make this color transparent within the image",
      "-treedepth value     color tree depth",
      "-trim                trim image edges",
      "-type type           image type",
      "-verbose             print detailed information about the image",
      "-version             print version information",
      "-virtual-pixel method",
      "                     Constant, Edge, Mirror, or Tile",
      "-white-point point   chomaticity white point",
      (char *) NULL
    };

  PrintUsageHeader();
  (void) printf("Usage: %.1024s [options ...] file [ [options ...] file ...]\n",
    GetClientName());
  (void) printf("\nWhere options include: \n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf(
    "\nIn addition to those listed above, you can specify these standard X\n");
  (void) printf(
    "resources as command line options:  -background, -bordercolor,\n");
  (void) printf(
    "-borderwidth, -font, -mattecolor, or -title\n");
  (void) printf(
    "\nBy default, the image format of `file' is determined by its magic\n");
  (void) printf(
    "number.  To specify a particular image format, precede the filename\n");
  (void) printf(
    "with an image format name and a colon (i.e. ps:image) or specify the\n");
  (void) printf(
    "image type as the filename suffix (i.e. image.ps).  Specify 'file' as\n");
  (void) printf("'-' for standard input or output.\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m p o r t I m a g e C o m m a n d                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImportImageCommand() reads an image from any visible window on an X server
%  and outputs it as an image file. You can capture a single window, the
%  entire screen, or any rectangular portion of the screen.  You can use the
%  display utility for redisplay, printing, editing, formatting, archiving,
%  image processing, etc. of the captured image.</dd>
%
%  The target window can be specified by id, name, or may be selected by
%  clicking the mouse in the desired window. If you press a button and then
%  drag, a rectangle will form which expands and contracts as the mouse moves.
%  To save the portion of the screen defined by the rectangle, just release
%  the button. The keyboard bell is rung once at the beginning of the screen
%  capture and twice when it completes.
%
%  The format of the ImportImageCommand method is:
%
%      unsigned int ImportImageCommand(ImageInfo *image_info,int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int ImportImageCommand(ImageInfo *image_info,
  int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
#if defined(HasX11)
  char
    *filename,
    *option,
    *resource_value,
    *server_name,
    *target_window;

  const char
    *client_name;

  Display
    *display;

  Image
    *image,
    *next_image;

  long
    snapshots,
    x;

  QuantizeInfo
    *quantize_info;

  register long
    i;

  unsigned int
    status;

  MagickXImportInfo
    ximage_info;

  MagickXResourceInfo
    resource_info;

  XrmDatabase
    resource_database;

  /*
    Check for server name specified on the command line.
  */
  server_name=(char *) NULL;
  for (i=1; i < argc; i++)
  {
    /*
      Check command line for server name.
    */
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      continue;
    if (LocaleCompare("display",option+1) == 0)
      {
        /*
          User specified server name.
        */
        i++;
        if (i == argc)
          MagickFatalError(OptionFatalError,MissingArgument,option);
        server_name=argv[i];
        break;
      }
    if (LocaleCompare("help",option+1) == 0 ||
        LocaleCompare("?", option+1) == 0)
      {
        ImportUsage();
        return MagickPass;
      }
    if (LocaleCompare("version",option+1) == 0)
      {
        (void) VersionCommand(image_info,argc,argv,metadata,exception);
        return MagickPass;
      }
  }

  /*
    Expand argument list
  */
  status=ExpandFilenames(&argc,&argv);
  if (status == False)
    MagickFatalError(ResourceLimitFatalError,MemoryAllocationFailed,
      (char *) NULL);

  /*
    Get user defaults from X resource database.
  */
  SetNotifyHandlers;
  display=XOpenDisplay(server_name);
  if (display == (Display *) NULL)
    MagickFatalError(OptionFatalError,UnableToOpenXServer,
      XDisplayName(server_name));
  (void) XSetErrorHandler(MagickXError);
  client_name=GetClientName();
  resource_database=MagickXGetResourceDatabase(display,client_name);
  MagickXGetImportInfo(&ximage_info);
  MagickXGetResourceInfo(resource_database,(char *) client_name,&resource_info);
  image_info=resource_info.image_info;
  quantize_info=resource_info.quantize_info;
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"border","False");
  ximage_info.borders=MagickIsTrue(resource_value);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"delay","0");
  resource_info.delay=MagickAtoL(resource_value);
  image_info->density=MagickXGetResourceInstance(resource_database,client_name,
    "density",(char *) NULL);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"descend","True");
  ximage_info.descend=MagickIsTrue(resource_value);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"frame","False");
  ximage_info.frame=MagickIsTrue(resource_value);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"interlace","none");
  image_info->interlace=StringToInterlaceType(resource_value);
  if (image_info->interlace == UndefinedInterlace)
    MagickError(OptionError,UnrecognizedInterlaceType,resource_value);
  image_info->page=MagickXGetResourceInstance(resource_database,client_name,
    "pageGeometry",(char *) NULL);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"pause","0");
  resource_info.pause=MagickAtoL(resource_value);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"quality","85");
  image_info->quality=MagickAtoL(resource_value);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"screen","False");
  ximage_info.screen=MagickIsTrue(resource_value);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"silent","False");
  ximage_info.silent=MagickIsTrue(resource_value);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"verbose","False");
  image_info->verbose=MagickIsTrue(resource_value);
  resource_value=
    MagickXGetResourceInstance(resource_database,client_name,"dither","True");
  quantize_info->dither=MagickIsTrue(resource_value);
  snapshots=1;
  status=True;
  filename=(char *) NULL;
  target_window=(char *) NULL;
  /*
    Check command syntax.
  */
  for (i=1; i < argc; i++)
  {
    option=argv[i];
    if ((strlen(option) == 1) || ((*option != '-') && (*option != '+')))
      {
        filename=argv[i];
        continue;
      }
    switch(*(option+1))
    {
      case 'a':
      {
        if (LocaleCompare("adjoin",option+1) == 0)
          {
            image_info->adjoin=(*option == '-');
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'b':
      {
        if (LocaleCompare("border",option+1) == 0)
          {
            ximage_info.borders=(*option == '-');
            break;
          }
        if (LocaleCompare("bordercolor",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                (void) QueryColorDatabase(argv[i],&image_info->border_color,
                  exception);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'c':
      {
        if (LocaleCompare("colors",option+1) == 0)
          {
            quantize_info->number_colors=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                quantize_info->number_colors=MagickAtoL(argv[i]);
              }
            break;
          }
        if (LocaleCompare("colorspace",option+1) == 0)
          {
            quantize_info->colorspace=RGBColorspace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                quantize_info->colorspace=StringToColorspaceType(option);
                if (IsGrayColorspace(quantize_info->colorspace))
                  {
                    quantize_info->number_colors=256;
                    quantize_info->tree_depth=8;
                  }
                if (quantize_info->colorspace == UndefinedColorspace)
                  MagickFatalError(OptionFatalError,InvalidColorspaceType,
                    option);
              }
            break;
          }
        if (LocaleCompare("comment",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("compress",option+1) == 0)
          {
            image_info->compression=NoCompression;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                image_info->compression=StringToCompressionType(option);
                if (image_info->compression == UndefinedCompression)
                  MagickFatalError(OptionFatalError,
                                   UnrecognizedImageCompressionType,option);
              }
            break;
          }
        if (LocaleCompare("crop",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'd':
      {
        if (LocaleCompare("debug",option+1) == 0)
          {
            (void) SetLogEventMask("None");
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) SetLogEventMask(argv[i]);
              }
            break;
          }
        if (LocaleCompare("define",option+1) == 0)
          {
            i++;
            if (i == argc)
              MagickFatalError(OptionFatalError,MissingArgument,option);
            if (*option == '+')
              (void) RemoveDefinitions(image_info,argv[i]);
            else
              (void) AddDefinitions(image_info,argv[i],exception);
            break;
          }
        if (LocaleCompare("delay",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("density",option+1) == 0)
          {
            (void) CloneString(&image_info->density,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) CloneString(&image_info->density,argv[i]);
              }
            break;
          }
        if (LocaleCompare("depth",option+1) == 0)
          {
            image_info->depth=QuantumDepth;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                image_info->depth=MagickAtoL(argv[i]);
              }
            break;
          }
        if (LocaleCompare("descend",option+1) == 0)
          {
            ximage_info.descend=(*option == '-');
            break;
          }
        if (LocaleCompare("display",option+1) == 0)
          {
            (void) CloneString(&image_info->server_name,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                (void) CloneString(&image_info->server_name,argv[i]);
              }
            break;
          }
        if (LocaleCompare("dispose",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                option=argv[i];
                if ((LocaleCompare("0",option) != 0) &&
                    (LocaleCompare("1",option) != 0) &&
                    (LocaleCompare("2",option) != 0) &&
                    (LocaleCompare("3",option) != 0) &&
                    (LocaleCompare("Undefined",option) != 0) &&
                    (LocaleCompare("None",option) != 0) &&
                    (LocaleCompare("Background",option) != 0) &&
                    (LocaleCompare("Previous",option) != 0))
                  MagickFatalError(OptionFatalError,UnrecognizedDisposeMethod,
                    option);
              }
            break;
          }
        if (LocaleCompare("dither",option+1) == 0)
          {
            quantize_info->dither=(*option == '-');
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'e':
      {
        if (LocaleCompare("encoding",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("endian",option+1) == 0)
          {
            image_info->endian=UndefinedEndian;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                image_info->endian=StringToEndianType(option);
                if (image_info->endian == UndefinedEndian)
                  MagickFatalError(OptionFatalError,InvalidEndianType,option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'f':
      {
        if (LocaleCompare("frame",option+1) == 0)
          {
            MagickFreeMemory(argv[i]);
            argv[i]=AcquireString("-ignore");  /* resolve option confict */
            ximage_info.frame=(*option == '-');
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'g':
      {
        if (LocaleCompare("geometry",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'h':
      {
        if (LocaleCompare("help",option+1) == 0)
          break;
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'i':
      {
        if (LocaleCompare("interlace",option+1) == 0)
          {
            image_info->interlace=UndefinedInterlace;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                image_info->interlace=StringToInterlaceType(option);
                if (image_info->interlace == UndefinedInterlace)
                  MagickFatalError(OptionFatalError,InvalidInterlaceType,
                    option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'l':
      {
        if (LocaleCompare("label",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("limit",option+1) == 0)
          {
            if (*option == '-')
              {
                ResourceType
                  resource_type;

                char
                  *type;

                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                type=argv[i];
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_type=StringToResourceType(type);
                if (resource_type == UndefinedResource)
                  MagickFatalError(OptionFatalError,UnrecognizedResourceType,type);
                (void) SetMagickResourceLimit(resource_type,MagickSizeStrToInt64(argv[i],1024));
              }
            break;
          }
        if (LocaleCompare("log",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) SetLogFormat(argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'm':
      {
        if (LocaleCompare("monitor",option+1) == 0)
          {
            if (*option == '+')
	      {
		(void) SetMonitorHandler((MonitorHandler) NULL);
		(void) MagickSetConfirmAccessHandler((ConfirmAccessHandler) NULL);
	      }
            else
	      {
		(void) SetMonitorHandler(CommandProgressMonitor);
		(void) MagickSetConfirmAccessHandler(CommandAccessMonitor);
	      }
            break;
          }
        if (LocaleCompare("monochrome",option+1) == 0)
          {
            image_info->monochrome=(*option == '-');
            if (image_info->monochrome)
              {
                quantize_info->number_colors=2;
                quantize_info->tree_depth=8;
                quantize_info->colorspace=GRAYColorspace;
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'n':
      {
        if (LocaleCompare("negate",option+1) == 0)
          break;
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'p':
      {
        if (LocaleCompare("page",option+1) == 0)
          {
            (void) CloneString(&image_info->page,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                image_info->page=GetPageGeometry(argv[i]);
              }
            break;
          }
        if (LocaleCompare("pause",option+1) == 0)
          {
            resource_info.pause=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                resource_info.pause=MagickAtoI(argv[i]);
              }
            break;
          }
       if (LocaleCompare("pointsize",option+1) == 0)
          {
            image_info->pointsize=12;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                image_info->pointsize=MagickAtoF(argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'q':
      {
        if (LocaleCompare("quality",option+1) == 0)
          {
            image_info->quality=DefaultCompressionQuality;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
                image_info->quality=MagickAtoL(argv[i]);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'r':
      {
        if (LocaleCompare("resize",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("rotate",option+1) == 0)
          {
            i++;
            if ((i == argc) || !IsGeometry(argv[i]))
               MagickFatalError(OptionFatalError,MissingArgument,option);
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 's':
      {
        if (LocaleCompare("sampling-factor",option+1) == 0)
          {
            (void) CloneString(&image_info->sampling_factor,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) CloneString(&image_info->sampling_factor,argv[i]);
                NormalizeSamplingFactor(image_info);
              }
            break;
          }
        if (LocaleCompare("scene",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,
                    option);
              }
            break;
          }
        if (LocaleCompare("screen",option+1) == 0)
          {
            ximage_info.screen=(*option == '-');
            break;
          }
        if (LocaleCompare("set",option+1) == 0)
          {
            i++;
            if (i == argc)
              MagickFatalError(OptionFatalError,MissingArgument,option);
            if (*option == '-')
              {
                /* -set attribute value */
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("silent",option+1) == 0)
          {
            ximage_info.silent=(*option == '-');
            break;
          }
        if (LocaleCompare("size",option+1) == 0)
          {
            (void) CloneString(&image_info->size,(char *) NULL);
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                (void) CloneString(&image_info->size,argv[i]);
              }
            break;
          }
        if (LocaleCompare("snaps",option+1) == 0)
          {
            (void) strcpy(argv[i]+1,"sans");
            i++;
            if ((i == argc) || !sscanf(argv[i],"%ld",&x))
              MagickFatalError(OptionFatalError,MissingArgument,option);
            snapshots=MagickAtoL(argv[i]);
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 't':
      {
        if (LocaleCompare("thumbnail",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !IsGeometry(argv[i]))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("transparent",option+1) == 0)
          {
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
              }
            break;
          }
        if (LocaleCompare("treedepth",option+1) == 0)
          {
            quantize_info->tree_depth=0;
            if (*option == '-')
              {
                i++;
                if ((i == argc) || !sscanf(argv[i],"%ld",&x))
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                quantize_info->tree_depth=MagickAtoI(argv[i]);
              }
            break;
          }
        if (LocaleCompare("trim",option+1) == 0)
          break;
        if (LocaleCompare("type",option+1) == 0)
          {
            image_info->type=UndefinedType;
            if (*option == '-')
              {
                i++;
                if (i == argc)
                  MagickFatalError(OptionFatalError,MissingArgument,option);
                option=argv[i];
                image_info->type=StringToImageType(option);
                if (image_info->type == UndefinedType)
                  MagickFatalError(OptionFatalError,InvalidImageType,option);
              }
            break;
          }
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'v':
      {
        if (LocaleCompare("verbose",option+1) == 0)
          {
            image_info->verbose+=(*option == '-');
            break;
          }
        if (LocaleCompare("version",option+1) == 0)
          break;
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
      case 'w':
      {
        i++;
        if (i == argc)
          MagickFatalError(OptionFatalError,MissingArgument,option);
        (void) CloneString(&target_window,argv[i]);
        break;
      }
      case '?':
      {
        break;
      }
      default:
      {
        MagickFatalError(OptionFatalError,UnrecognizedOption,option);
        break;
      }
    }
  }
  /*
    Erase existing exception.
  */
  DestroyExceptionInfo(exception);
  GetExceptionInfo(exception);
  if (filename == (char *) NULL)
    filename=(char *) "magick.miff";
  /*
    Read image from X server.
  */
  if (target_window != (char *) NULL)
    (void) strlcpy(image_info->filename,target_window,MaxTextExtent);
  image_info->colorspace=quantize_info->colorspace;
  image_info->dither=quantize_info->dither;
  image=(Image *) NULL;
  for (i=0; i < (long) Max(snapshots,1); i++)
  {
    (void) MagickSleep(resource_info.pause);
    next_image=MagickXImportImage(image_info,&ximage_info);
    status&=next_image != (Image *) NULL;
    if (next_image == (Image *) NULL)
      continue;
    (void) strlcpy(next_image->filename,filename,MaxTextExtent);
    (void) strcpy(next_image->magick,"PS");
    next_image->scene=i;
    next_image->previous=image;
    if (image != (Image *) NULL)
      image->next=next_image;
    image=next_image;
  }
  if (image == (Image *) NULL)
    MagickFatalError(OptionFatalError,RequestDidNotReturnAnImage,(char *) NULL);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  status&=MogrifyImages(image_info,argc-1,argv,&image);
  (void) CatchImageException(image);
  status&=WriteImages(image_info,image,filename,&image->exception);

  DestroyImageList(image);
  LiberateArgumentList(argc,argv);
  MagickXDestroyResourceInfo(&resource_info);
  MagickXDestroyX11Resources();
  (void) XCloseDisplay(display);
  return(status);
#else
  ARG_NOT_USED(image_info);
  ARG_NOT_USED(argc);
  ARG_NOT_USED(argv);
  ARG_NOT_USED(metadata);
  ARG_NOT_USED(exception);

  MagickFatalError(MissingDelegateError,XWindowLibraryIsNotAvailable,
    (char *) NULL);
  return(False);
#endif
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I m p o r t U s a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ImportUsage() displays the program command syntax.
%
%  The format of the ImportUsage method is:
%
%      void ImportUsage()
%
%
*/
#if defined(HasX11)
static void ImportUsage(void)
{
  const char
    **p;

  static const char
    *options[]=
    {
      "-adjoin              join images into a single multi-image file",
      "-border              include image borders in the output image",
      "-colors value        preferred number of colors in the image",
      "-colorspace type     alternate image colorspace",
      "-comment string      annotate image with comment",
      "-compress type       image compression type",
      "-crop geometry       preferred size and location of the cropped image",
      "-debug events        display copious debugging information",
      "-define values       Coder/decoder specific options",
      "-delay value         display the next image after pausing",
      "-density geometry    horizontal and vertical density of the image",
      "-depth value         image depth",
      "-descend             obtain image by descending window hierarchy",
      "-display server      X server to contact",
      "-dispose method      Undefined, None, Background, Previous",
      "-dither              apply Floyd/Steinberg error diffusion to image",
      "-frame               include window manager frame",
      "-encoding type       text encoding type",
      "-endian type         multibyte word order (LSB, MSB, or Native)",
      "-geometry geometry   perferred size or location of the image",
      "-interlace type      None, Line, Plane, or Partition",
      "-help                print program options",
      "-label name          assign a label to an image",
      "-limit type value    Disk, File, Map, Memory, Pixels, Width, Height or",
      "                     Threads resource limit",
      "-log format          format of debugging information",
      "-monitor             show progress indication",
      "-monochrome          transform image to black and white",
      "-negate              replace every pixel with its complementary color ",
      "-page geometry       size and location of an image canvas",
      "-pause value         seconds delay between snapshots",
      "-pointsize value     font point size",
      "-quality value       JPEG/MIFF/PNG compression level",
      "-resize geometry     resize the image",
      "-rotate degrees      apply Paeth rotation to the image",
      "-sampling-factor HxV[,...]",
      "                     horizontal and vertical sampling factors",
      "-scene value         image scene number",
      "-screen              select image from root window",
      "-set attribute value set image attribute",
      "+set attribute       unset image attribute",
      "-silent              operate silently, i.e. don't ring any bells ",
      "-snaps value         number of screen snapshots",
      "-thumbnail geometry  resize the image (optimized for thumbnails)",
      "-transparent color   make this color transparent within the image",
      "-treedepth value     color tree depth",
      "-trim                trim image edges",
      "-type type           image type",
      "-verbose             print detailed information about the image",
      "-version             print version information",
      "-virtual-pixel method",
      "                     Constant, Edge, Mirror, or Tile",
      "-window id           select window with this id or name",
      (char *) NULL
    };

  PrintUsageHeader();
  (void) printf("Usage: %.1024s [options ...] [ file ]\n",GetClientName());
  (void) printf("\nWhere options include:\n");
  for (p=options; *p != (char *) NULL; p++)
    (void) printf("  %.1024s\n",*p);
  (void) printf(
  "\nBy default, 'file' is written in the MIFF image format.  To\n");
  (void) printf(
    "specify a particular image format, precede the filename with an image\n");
  (void) printf(
    "format name and a colon (i.e. ps:image) or specify the image type as\n");
  (void) printf(
    "the filename suffix (i.e. image.ps).  Specify 'file' as '-' for\n");
  (void) printf("standard input or output.\n");
}
#endif /* HasX11 */


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P a r s e U n i x C o m m a n d L i n e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ParseUnixCommandLine reads the command line from input file handler,
%  parses the arguments following the Unix escape rule, and stores it in a
%  given text array.
%
%  The format of the ParseUnixCommandLine method is:
%
%      void ParseUnixCommandLine(FILE *in, int acmax, char **av)
%
%  A description of each parameter follows:
%
%    o in: An input file handler to read the command line from.
%
%    o acmax: The max capacity of the 'av' text array.
%
%    o av: A text array to store the parsed command line arguments.
%
*/
static int ParseUnixCommandLine(FILE *in, int acmax, char **av)
{
    register int c;
    register char *p = commandline;
    register int n = 1;

    char *limit = p + MAX_PARAM_CHAR;
    limit[1] = 0;
    av[1] = p;
    *p = 0;
    do c = fgetc(in); while(MagickIsBlank(c));

    while (c != EOF)
      {
        switch (c)
          {
          case '\'':
            while((c = fgetc(in)) != '\'')
              {
                if (p >= limit )
                  {
                    while ((c = fgetc(in)) != '\n');
                    return 0;
                  }
                *p++ = c;
              }
            break;

          case '"':
            while((c = fgetc(in)) != '"')
              {
                if (c == '\\')
                  {
                    int next = fgetc(in);
                    if (next != '\\' && next != '"')
                      *p++ = c;
                    c = next;
                  }
                if (p >= limit )
                  {
                    while ((c = fgetc(in)) != '\n');
                    return 0;
                  }
                *p++ = c;
              }
            break;

          case ' ':
          case '\t':
            *p++ = '\0';
            if (++n > acmax)
              {
                while ((c = fgetc(in)) != '\n');
                return acmax+1;
              }
            av[n] = p;
            *p = 0;
            do { c = fgetc(in); }
            while(MagickIsBlank(c));
            continue;

          case '\r':
            break;

          case '#':
            while ((c = fgetc(in)) != '\n');
            /* Same handling as '\n' since we are at end of line */
            *p = 0;
            n = av[n][0] ? n+1 : n;
            av[n] = (char *)NULL;
            return n;

          case '\n':
            *p = 0;
            n = av[n][0] ? n+1 : n;
            av[n] = (char *)NULL;
            return n;

          case '\\':
            c = fgetc(in);
            if (p >= limit )
              {
                while ((c = fgetc(in)) != '\n');
                return 0;
              }
            *p++ = c;
            break;

          default:
            if (p >= limit )
              {
                while ((c = fgetc(in)) != '\n');
                return 0;
              }
            *p++ = c;
            break;
          }
        c = fgetc(in);
      }
    return EOF;
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P a r s e W i n d o w s C o m m a n d L i n e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ParseWindowsCommandLine reads the command line from input file handler,
%  parses the arguments following the Windows escape rule, and stores it in a
%  given text array.
%
%  The format of the ParseWindowsCommandLine method is:
%
%      void ParseWindowsCommandLine(FILE *in, int acmax, char **av)
%
%  A description of each parameter follows:
%
%    o in: An input file handler to read the command line from.
%
%    o acmax: The max capacity of the 'av' text array.
%
%    o av: A text array to store the parsed command line arguments.
%
*/
static int ParseWindowsCommandLine(FILE *in, int acmax, char **av)
{
    register int c;
    register char *p = commandline;
    register int n = 1;

    char *limit = p + MAX_PARAM_CHAR;
    limit[1] = 0;
    av[1] = p;
    *p = 0;
    do c = fgetc(in); while(MagickIsBlank(c));

    while (c != EOF)
      {
        switch (c)
          {
          case '"':
            for(;;)
              {
                c = fgetc(in);
                if (c == '"')
                  {
                    int next = fgetc(in);
                    if (next != '"')
                      {
                        ungetc(next, in);
                        break;
                      }
                  }
                if (p >= limit )
                  {
                    while ((c = fgetc(in)) != '\n');
                    return 0;
                  }
                *p++ = c;
              }
            break;

          case ' ':
          case '\t':
            *p++ = '\0';
            if (++n > acmax)
              {
                while ((c = fgetc(in)) != '\n');
                return acmax+1;
              }
            av[n] = p;
            *p = 0;
            do { c = fgetc(in); }
            while(MagickIsBlank(c));
            continue;

          case '\r':
            break;

          case '#':
            while ((c = fgetc(in)) != '\n');
            /* Same handling as '\n' since we are at end of line */
            *p = 0;
            n = av[n][0] ? n+1 : n;
            av[n] = (char *)NULL;
            return n;

          case '\n':
            *p = 0;
            n = av[n][0] ? n+1 : n;
            av[n] = (char *)NULL;
            return n;

          default:
            if (p >= limit )
                {
                  while ((c = fgetc(in)) != '\n');
                  return 0;
                }
            *p++ = c;
            break;
          }
        c = fgetc(in);
      }
    return EOF;
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P r o c e s s B a t c h O p t i o n s                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ProcessBatchOptions processes arguments of batch and set command and stores
%  option values in given pointer. The return value is either OptionStatus if
%  negative or the first none option argument position.
%
%  The format of the ProcessBatchOptions method is:
%
%      int ProcessBatchOptions(int argc, char **argv, BatchOptions *options)
%
%  A description of each parameter follows:
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o options: Points to the variable to accept the the option value.
%
*/
static int ProcessBatchOptions(int argc, char **argv, BatchOptions *options)
{
  int i;
  for (i = 1; i < argc; i++ )
    {
      char *option;
      char *p = argv[i];
      OptionStatus status = OptionUnknown;

      if (p[0] != '-')
        return i;

      switch (p[1])
        {
        case '\0':
          return i;

        case '-':
          if (p[2] == '\0')
            return i+1;
          break;

        case 'e':
        case 'E':
          if (LocaleCompare(option = "-escape", p) == 0)
            {
              int index;
              status = GetOptionValueRestricted(option, escape_option_values, argv[++i], &index);
              if (status == OptionSuccess)
                options->command_line_parser = index ? ParseWindowsCommandLine : ParseUnixCommandLine;
            }
          else if (LocaleCompare(option = "-echo", p) == 0)
            status = GetOnOffOptionValue(option, argv[++i], &options->is_echo_enabled);
          break;

        case 'f':
        case 'F':
          if (LocaleCompare(option = "-feedback", p) == 0)
            status = GetOnOffOptionValue(option, argv[++i], &options->is_feedback_enabled);
          else if (LocaleCompare(option = "-fail", p) == 0)
            {
              char *value = NULL;
              status = GetOptionValue(option, argv[++i], &value);
              if (OptionSuccess == status)
                strlcpy(options->fail, value, sizeof(options->fail));
            }
          break;

        case '?':
          if (p[2] == '\0')
            status = OptionHelp;
          break;

        case 'h':
        case 'H':
          if (LocaleCompare("-help", p) == 0)
            status = OptionHelp;
          break;

        case 'p':
        case 'P':
          if (LocaleCompare(option = "-pass", p) == 0)
            {
              char *value = NULL;
              status = GetOptionValue(option, argv[++i], &value);
              if (OptionSuccess == status)
                strlcpy(options->pass, value, sizeof(options->pass));
            }
          else if (LocaleCompare(option = "-prompt", p) == 0) {
              char *value = NULL;
              status = GetOptionValue(option, argv[++i], &value);
              if (OptionSuccess == status)
                strlcpy(options->prompt,
                        LocaleCompare("off", value) == 0 ? "" : value,
                        sizeof(options->prompt));
            }
          break;

        case 's':
        case 'S':
          if (LocaleCompare(option = "-stop-on-error", p) == 0)
            status = GetOnOffOptionValue(option, argv[++i], &options->stop_on_error);
          break;
        }
      if (status == OptionSuccess)
        continue;
      if (status == OptionUnknown)
        fprintf(stderr, "Error: Unknown option: %s\n", p);
      return status;
    }
  return argc;
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t C o m m a n d                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetCommand reads the arguments and sets the batch options.
%
%  The format of the SetCommand method is:
%
%      unsigned int SetCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
static unsigned int SetCommand(ImageInfo *image_info,
  int argc,char **argv,char **metadata,ExceptionInfo *exception)
{

  ARG_NOT_USED(image_info);
  ARG_NOT_USED(metadata);
  ARG_NOT_USED(exception);

  if (argc > 1)
    {
      /* use a dummy first so we don't change the real one when error. */
      BatchOptions dummy;
      int i = ProcessBatchOptions(argc, argv, &dummy);
      if (i < 0)
        {
          SetUsage();
          return i == OptionHelp;
        }
      if (i != argc)
        {
          fprintf(stderr, "Error: unexpected parameter: %s\n", argv[i]);
          SetUsage();
          return MagickFalse;
        }
      ProcessBatchOptions(argc, argv, &batch_options);
      return MagickTrue;
    }

  printf("escape        : %s\n", escape_option_values[batch_options.command_line_parser == ParseWindowsCommandLine]);
  printf("fail          : %s\n", batch_options.fail);
  printf("feedback      : %s\n", on_off_option_values[batch_options.is_feedback_enabled]);
  printf("stop-on-error : %s\n", on_off_option_values[batch_options.stop_on_error]);
  printf("pass          : %s\n", batch_options.pass);
  printf("prompt        : %s\n", batch_options.prompt);
  return MagickTrue;
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S e t U s a g e                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SetUsage displays the program command syntax.
%
%  The format of the SetUsage method is:
%
%      void SetUsage()
%
*/
static void SetUsage(void)
{
  (void) puts("Usage: set [options ...]");
  BatchOptionUsage();
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   T i m e U s a g e                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TimeUsage() displays the program command syntax.
%
%  The format of the TimeUsage method is:
%
%      void TimeUsage()
%
%  A description of each parameter follows:
%
%
*/
static void TimeUsage(void)
{
  PrintUsageHeader();
  (void) printf("Usage: %.1024s command ... \n",GetClientName());
  (void) printf("where 'command' is some other GraphicsMagick command\n");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  T i m e I m a g e C o m m a n d                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  TimeImageCommand() executes a specified GraphicsMagick sub-command
%  (e.g. 'convert') and prints out a summary of how long it took to the
%  standard error output.  The format is similar to the output of the 'time'
%  command in the zsh shell.
%
%  The format of the TimeImageCommand method is:
%
%      unsigned int TimeImageCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int
TimeImageCommand(ImageInfo *image_info,
		 int argc,char **argv,char **metadata,ExceptionInfo *exception)
{
  double
    elapsed_time,
    /* resolution, */
    user_time;

  TimerInfo
    timer;

  const char
    *pad="    ";

  char
    client_name[MaxTextExtent];

  int
    formatted,
    i,
    screen_width;

  unsigned int
    status=True;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);

  if (argc < 2 || ((argc < 3) && (LocaleCompare("-help",argv[1]) == 0 ||
                                  LocaleCompare("-?",argv[1]) == 0)))
    {
      TimeUsage();
      if (argc < 2)
	{
	  ThrowException(exception,OptionError,UsageError,NULL);
	  return MagickFail;
	}
      return MagickPass;
    }
  if (LocaleCompare("-version",argv[1]) == 0)
    {
      (void) VersionCommand(image_info,argc,argv,metadata,exception);
      return MagickPass;
    }

  /*
    Skip over our command name argv[0].
  */
  argc--;
  argv++;
  i=0;

  (void) strlcpy(client_name,GetClientName(),sizeof(client_name));
  GetTimerInfo(&timer);
  status=ExecuteSubCommand(image_info,argc,argv,metadata,exception);
  (void) SetClientName(client_name);

  /* resolution=GetTimerResolution(); */
  user_time=GetUserTime(&timer);
  elapsed_time=GetElapsedTime(&timer);
  (void) fflush(stdout);

  screen_width=0;
  if (getenv("COLUMNS"))
    screen_width=MagickAtoI(getenv("COLUMNS"))-1;
  if (screen_width < 80)
    screen_width=80;

  formatted=0;
  for (i=0; i < argc; i++)
    {
      if (i != 0)
	formatted += fprintf(stderr," ");
      formatted += fprintf(stderr,"%s",argv[i]);
      if (formatted > (screen_width-55))
	{
	  if ((i+1) < argc)
	    pad="... ";
	  break;
	}
    }
  (void) fprintf(stderr,
		 "%s%.2fs user %.2fs system %.0f%% cpu %.3f total\n",
		 pad,user_time,0.0,100.0*user_time/elapsed_time,elapsed_time);
  (void) fflush(stderr);

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    V e r s i o n C o m m a n d                                              %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  VersionCommand() prints the package copyright and release version.
%
%  The format of the VersionCommand method is:
%
%      unsigned int VersionCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static void PrintFeatureTextual(const char* feature,MagickBool support,const char *text)
{
  const char
    *support_text;

  support_text=(support ? "yes" : "no");
  if ((text != NULL) && strlen(text) != 0)
    (void) fprintf(stdout,"  %-24s %s (%s)\n", feature,  support_text, text);
  else
    (void) fprintf(stdout,"  %-24s %s\n", feature, support_text);
}
static void PrintFeature(const char* feature,MagickBool support)
{
  PrintFeatureTextual(feature,support, NULL);
}
static unsigned int VersionCommand(ImageInfo *ARGUNUSED(image_info),
				   int ARGUNUSED(argc),char **ARGUNUSED(argv),char **ARGUNUSED(metadata),
				   ExceptionInfo *ARGUNUSED(exception))
{
  MagickBool
    supported;

  char
    text[MaxTextExtent];

  PrintVersionAndCopyright();

  (void) fprintf(stdout,"\nFeature Support:\n");

  /* Thread Safe */
  supported=MagickFalse;
#if defined(MSWINDOWS) || defined(HAVE_PTHREAD)
  supported=MagickTrue;
#endif /* defined((MSWINDOWS) || defined(HAVE_PTHREAD) */
  PrintFeature("Native Thread Safe", supported);

  /*
    Large File Support

    For POSIX, the 'stat' structure has 'st_size' member which is of
    type 'off_t'.  However, Windows provides a large family of _stat*
    structures with varying proportions and we might be using 'struct
    stat' or 'struct _stati64' which use different hard-coded types
    for the 'st_size' member.  While large files might be otherwise
    supported, we consider the maximum size reportable by 'stat' to be
    the limiting factor.
  */
  {
    MagickStatStruct_t
      attributes;

    supported=(sizeof(attributes.st_size) > 4);
    (void) attributes;
  }
  PrintFeature("Large Files (> 32 bit)", supported);

  /* Large Memory Support */
  supported=(sizeof(PixelPacket *) > 4);
  PrintFeature("Large Memory (> 32 bit)", supported);

  /* BZIP */
  supported=MagickFalse;
#if defined(HasBZLIB)
  supported=MagickTrue;
#endif /* defined(HasBZLIB) */
  PrintFeature("BZIP", supported);

  /* DPS */
  supported=MagickFalse;
#if defined(HasDPS)
  supported=MagickTrue;
#endif /* defined(HasDPS) */
  PrintFeature("DPS", supported);

  /* FlashPix */
  supported=MagickFalse;
#if defined(HasFPX)
  supported=MagickTrue;
#endif /* defined(HasFPX) */
  PrintFeature("FlashPix", supported);

  /* FreeType */
  supported=MagickFalse;
#if defined(HasTTF)
  supported=MagickTrue;
#endif /* defined(HasTTF) */
  PrintFeature("FreeType", supported);

  /* Ghostscript Library */
  supported=MagickFalse;
#if defined(HasGS)
  supported=MagickTrue;
#endif /* defined(HasGS) */
  PrintFeature("Ghostscript (Library)", supported);

  /* JBIG */
  supported=MagickFalse;
#if defined(HasJBIG)
  supported=MagickTrue;
#endif /* defined(HasJBIG) */
  PrintFeature("JBIG",supported);

  /* JPEG-2000 */
  supported=MagickFalse;
#if defined(HasJP2)
  supported=MagickTrue;
#endif /* defined(HasJP2) */
  PrintFeature("JPEG-2000", supported);

  /* JPEG */
  supported=MagickFalse;
#if defined(HasJPEG)
  supported=MagickTrue;
#endif /* defined(HasJPEG) */
  PrintFeature("JPEG", supported);

  /* Little CMS */
  supported=MagickFalse;
#if defined(HasLCMS)
  supported=MagickTrue;
#endif /* defined(HasLCMS) */
  PrintFeature("Little CMS", supported);

  /* Loadable Modules */
  supported=MagickFalse;
#if defined(SupportMagickModules)
  supported=MagickTrue;
#endif /* defined(SupportMagickModules) */
  PrintFeature("Loadable Modules", supported);

  /* OpenMP */
  supported=MagickFalse;
  text[0]='\0';
#if defined(HAVE_OPENMP)
  supported=MagickTrue;
  FormatString(text,"%u",(unsigned int) _OPENMP);
#endif /* defined(HAVE_OPENMP) */
  PrintFeatureTextual("OpenMP", supported, text);

  /* PNG */
  supported=MagickFalse;
#if defined(HasPNG)
  supported=MagickTrue;
#endif /* defined(HasPNG) */
  PrintFeature("PNG", supported);

  /* TIFF */
  supported=MagickFalse;
#if defined(HasTIFF)
  supported=MagickTrue;
#endif /* defined(HasTIFF) */
  PrintFeature("TIFF", supported);

  /* TRIO */
  supported=MagickFalse;
#if defined(HasTRIO)
  supported=MagickTrue;
#endif /* defined(HasTRIO) */
  PrintFeature("TRIO", supported);

  /* Solaris libumem */
  supported=MagickFalse;
#if defined(HasUMEM)
  supported=MagickTrue;
#endif /* defined(HasUMEM) */
  PrintFeature("UMEM", supported);

  /* WebP */
  supported=MagickFalse;
#if defined(HasWEBP)
  supported=MagickTrue;
#endif /* defined(HasWEBP) */
  PrintFeature("WebP", supported);

  /* WMF */
  supported=MagickFalse;
#if defined(HasWMF) || defined(HasWMFlite)
  supported=MagickTrue;
#endif /* defined(HasWMF) || defined(HasWMFlite) */
  PrintFeature("WMF", supported);

  /* X11 */
  supported=MagickFalse;
#if defined(HasX11)
  supported=MagickTrue;
#endif /* defined(HasX11) */
  PrintFeature("X11", supported);

  /* XML */
  supported=MagickFalse;
#if defined(HasXML)
  supported=MagickTrue;
#endif /* defined(HasXML) */
  PrintFeature("XML", supported);

  /* ZLIB */
  supported=MagickFalse;
#if defined(HasZLIB)
  supported=MagickTrue;
#endif /* defined(HasZLIB) */
  PrintFeature("ZLIB", supported);

#if defined(GM_BUILD_HOST)
  (void) fprintf(stdout,"\nHost type: %.1024s\n", GM_BUILD_HOST);
#endif /* defined(GM_BUILD_HOST) */

#if defined(GM_BUILD_CONFIGURE_ARGS)
  (void) fprintf(stdout,"\nConfigured using the command:\n  %.1024s\n",
                 GM_BUILD_CONFIGURE_ARGS);
#endif /* defined(GM_BUILD_CONFIGURE_ARGS) */

#if defined(GM_BUILD_CC)
  (void) fprintf(stdout,"\nFinal Build Parameters:\n");
  (void) fprintf(stdout,"  CC       = %.1024s\n", GM_BUILD_CC);
#endif /* defined(GM_BUILD_CC) */

#if defined(GM_BUILD_CFLAGS)
  (void) fprintf(stdout,"  CFLAGS   = %.1024s\n", GM_BUILD_CFLAGS);
#endif /* defined(GM_BUILD_CFLAGS) */

#if defined(GM_BUILD_CPPFLAGS)
  (void) fprintf(stdout,"  CPPFLAGS = %.1024s\n", GM_BUILD_CPPFLAGS);
#endif /* defined(GM_BUILD_CPPFLAGS) */

#if defined(GM_BUILD_CXX)
  (void) fprintf(stdout,"  CXX      = %.1024s\n", GM_BUILD_CXX);
#endif /* defined(GM_BUILD_CXX) */

#if defined(GM_BUILD_CXXFLAGS)
  (void) fprintf(stdout,"  CXXFLAGS = %.1024s\n", GM_BUILD_CXXFLAGS);
#endif /* defined(GM_BUILD_CXXFLAGS) */

#if defined(GM_BUILD_LDFLAGS)
  (void) fprintf(stdout,"  LDFLAGS  = %.1024s\n", GM_BUILD_LDFLAGS);
#endif /* defined(GM_BUILD_LDFLAGS) */

#if defined(GM_BUILD_LIBS)
  (void) fprintf(stdout,"  LIBS     = %.1024s\n", GM_BUILD_LIBS);
#endif /* defined(GM_BUILD_LIBS) */

#if defined(_VISUALC_)

  (void) fprintf(stdout,"\nWindows Build Parameters:\n\n");

#  if defined(_MSC_VER)
  (void) fprintf(stdout,"  MSVC Version:            %d\n", _MSC_VER);
#  endif /* defined(_MSC_VER) */

#  if defined(__CLR_VER)
  (void) fprintf(stdout,"  CLR Version:             %d\n", __CLR_VER);
#  endif /* defined(__CLR_VER) */

  /* Defined for compilations that target x86 processors. This is not
     defined for x64 processors. */
#  if defined(_M_IX86)
  {
    const char
      *processor_target = "";

    if (_M_IX86 >= 600)
      processor_target="Pentium Pro, Pentium II, and Pentium III";
    else if (_M_IX86 >= 500)
      processor_target="Pentium";
    else if (_M_IX86 >= 400)
      processor_target="80486";
    else if (_M_IX86 >= 300)
      processor_target="80386";

    if (strlen(processor_target) > 0)
      (void) fprintf(stdout,"  Processor target:        %s\n", processor_target);
  }
#  endif /* defined(_M_IX86) */
#  if defined(_M_IX86_FP)
  {
    const char
      *processor_arch = "";

    if (_M_IX86_FP == 0)
      processor_arch="NONE";
    else if (_M_IX86_FP == 1)
      processor_arch="SSE";
    else if (_M_IX86_FP == 2)
      processor_arch="SSE2";

    if (strlen(processor_arch) > 0)
      (void) fprintf(stdout,"  Processor arch:          %s\n", processor_arch);
  }
#  endif /* defined(_M_IX86_FP) */

#endif /* defined(_VISUALC_) */

  return True;
}

#if defined(MSWINDOWS)
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%    R e g i s t e r C o m m a n d                                            %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RegisterCommand() registers this appplication as the source for messages
%  compatible with the windows event logging system. All this does is to set
%  a registry value to point to either an EXE or DLL that contains a special
%  binary resource containing all the messages that can be used.
%
%  The format of the RegisterCommand method is:
%
%      unsigned int RegisterCommand(ImageInfo *image_info,const int argc,
%        char **argv,char **metadata,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: The image info.
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%    o metadata: any metadata is returned here.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static unsigned int RegisterCommand(ImageInfo *image_info,
                                    int argc,
                                    char **argv,
                                    char **metadata,
                                    ExceptionInfo *exception)
{
  char
    *szRegPath = 
    "SYSTEM\\CurrentControlSet\\Services\\Eventlog\\Application\\";

  char
    szKey[_MAX_PATH*2];

  DWORD
    dwResult = 0;

  HKEY
    hKey = NULL;

  LONG
    lRet;

  ARG_NOT_USED(image_info);
  ARG_NOT_USED(argc);
  ARG_NOT_USED(argv);
  ARG_NOT_USED(metadata);
  ARG_NOT_USED(exception);

  memset(szKey, 0, _MAX_PATH*2*sizeof(char));
  strcpy(szKey, szRegPath);
  strcat(szKey, "GraphicsMagick");

  /* open the registry event source key */
  lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, NULL,
                        REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
                        &hKey, &dwResult);
  if (lRet == ERROR_SUCCESS)
    {
      char
        szPathName[MaxTextExtent];

      DWORD
        dwSupportedTypes;

      /* set a pointer to thsi application as the source for our messages */
      memset(szPathName, 0, MaxTextExtent*sizeof(char));
      FormatString(szPathName,"%.1024s%s%.1024s",
                   GetClientPath(),DirectorySeparator,GetClientName());
      RegSetValueEx(hKey, "EventMessageFile", 0, REG_SZ,
                    (const BYTE *) szPathName,
                    ((DWORD) strlen(szPathName) + 1)*sizeof(char));
      (void) LogMagickEvent(ConfigureEvent,GetMagickModule(),
                            "Registered path to messages as: %s",szPathName);

      /* supports all types of messages */
      dwSupportedTypes = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE |
        EVENTLOG_INFORMATION_TYPE | EVENTLOG_AUDIT_SUCCESS |
        EVENTLOG_AUDIT_FAILURE;
      RegSetValueEx(hKey, "TypesSupported", 0, REG_DWORD,
                    (const BYTE *) &dwSupportedTypes, sizeof(DWORD));

      RegCloseKey(hKey);
      return TRUE;
    }

  return False;
}
#endif


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G M C o m m a n d S i n g l e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GMCommandSingle() is used by GMCommand() and BatchCommand to run one single
%  command of the 'gm' utility.
%
%  The format of the GMCommandSingle method is:
%
%      int GMCommandSingle(int argc,char **argv)
%
%  A description of each parameter follows:
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%
*/
static MagickBool GMCommandSingle(int argc,char **argv)
{
  char
    command[MaxTextExtent],
    *text;

  ExceptionInfo
    exception;

  ImageInfo
    *image_info;

  MagickBool
    status=MagickTrue;

  /*
    Initialize locale from environment variables (LANG, LC_CTYPE,
    LC_NUMERIC, LC_TIME, LC_COLLATE, LC_MONETARY, LC_MESSAGES,
    LC_ALL), but require that LC_NUMERIC use common conventions.  The
    LC_NUMERIC variable affects the decimal point character and
    thousands separator character for the formatted input/output
    functions and string conversion functions.
  */
  (void) setlocale(LC_ALL,"");
  (void) setlocale(LC_NUMERIC,"C");

  if (run_mode == SingleMode)
    {
#if defined(MSWINDOWS)
      InitializeMagick((char *) NULL);
#else
      InitializeMagick(argv[0]);
#endif
    }

  ReadCommandlLine(argc,&argv);

  (void) SetClientName(argv[0]);
  {
    /*
      Support traditional alternate names for GraphicsMagick subcommands.
    */
    static const char *command_names [] =
      {
        "animate",
        "composite",
        "conjure",
        "convert",
        "display",
        "identify",
        "import",
        "mogrify",
        "montage",
        NULL
      };

    unsigned int
      i;

    GetPathComponent(argv[0],BasePath,command);
    for (i=0; command_names[i]; i++)
      if (LocaleCompare(command,command_names[i]) == 0)
        break;

    if (command_names[i])
      {
        /*
          Set command name to alternate name.
        */
        argv[0]=(char *) SetClientName(command);
      }
    else
      {
        if (argc < 2)
          {
            GMUsage();
            return(MagickFalse);
          }

        /*
          Skip to subcommand name.
        */
        argc--;
        argv++;
      }
    if (!strcmp(argv[0], "ping"))
      return MagickTrue;
  }

  GetExceptionInfo(&exception);
  image_info=CloneImageInfo((ImageInfo *) NULL);
  text=(char *) NULL;
  status=MagickCommand(image_info,argc,argv,&text,&exception);
  if (text != (char *) NULL)
    {
      if (strlen(text))
        {
          (void) fputs(text,stdout);
          (void) fputc('\n',stdout);
          (void) fflush(stdout);
        }
      MagickFreeMemory(text);
    }
  if (exception.severity != UndefinedException)
    CatchException(&exception);
  DestroyImageInfo(image_info);
  DestroyExceptionInfo(&exception);
  if (run_mode == SingleMode)
    DestroyMagick();

  return (status);
}


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G M C o m m a n d                                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GMCommand() implements the 'gm' utility.
%
%  The format of the GMCommand method is:
%
%      int GMCommand(int argc,char **argv)
%
%  A description of each parameter follows:
%
%    o argc: The number of elements in the argument vector.
%
%    o argv: A text array containing the command line arguments.
%
%
*/

MagickExport int GMCommand(int argc,char **argv)
{
  int status;
  if ((argc <= 1) || LocaleCompare("batch", argv[1]) != 0)
    {
      status = GMCommandSingle(argc, argv);
    }
  else
    {
      status = BatchCommand(argc, argv);
    }
  return(!status);
}
