/*
% Copyright (C) 2003 GraphicsMagick Group
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
%                                                                             %
%                      IIIII   CCCC   CCCC   CCCC  M   M                      %
%                        I    C      C      C      MM MM                      %
%                        I    C      C      C      M M M                      %
%                        I    C      C      C      M   M                      %
%                      IIIII   CCCC   CCCC   CCCC  M   M                      %
%                                                                             %
%                          X11 Compatibility Methods                          %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                December 1994                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

#include "magick/studio.h"
#if defined(HasX11)
#include "magick/xwindow.h"

#if defined(PRE_R6_ICCCM)
/*
  Compatibility methods for pre X11R6 ICCCM.
*/
Status XInitImage(XImage *ximage)
{
  Display
    display;

  ScreenFormat
    screen_format;

  XImage
    *created_ximage,
    target_ximage;

  /*
    Initialize the X image.
  */
  screen_format.depth=ximage->depth;
  screen_format.bits_per_pixel=(int) ximage->bits_per_pixel;
  display.byte_order=ximage->byte_order;
  display.bitmap_unit=ximage->bitmap_unit;
  display.bitmap_bit_order=ximage->bitmap_bit_order;
  display.pixmap_format=(&screen_format);
  display.nformats=1;
  created_ximage=XCreateImage(&display,(Visual *) NULL,ximage->depth,
    ximage->format,ximage->xoffset,(char *) NULL,ximage->width,ximage->height,
    ximage->bitmap_pad,ximage->bytes_per_line);
  if (created_ximage == (XImage *) NULL)
    return(0);
  target_ximage=(*ximage);
  *ximage=(*created_ximage);
  created_ximage->data=(char *) NULL;
  XDestroyImage(created_ximage);
  ximage->red_mask=target_ximage.red_mask;
  ximage->green_mask=target_ximage.green_mask;
  ximage->blue_mask=target_ximage.blue_mask;
  return(1);
}
#endif

#if defined(PRE_R5_ICCCM)
/*
  Compatibility methods for pre X11R5 ICCCM.
*/
void XrmCombineDatabase(XrmDatabase source,XrmDatabase *target,
  Bool override)
{
  XrmMergeDatabases(source,target);
}

Status XrmCombineFileDatabase(const char *filename,XrmDatabase *target,
  Bool override)
{
  XrmDatabase
    *combined_database,
    source;

  source=XrmGetFileDatabase(filename);
  if (!override)
    XrmMergeDatabases(source,target);
  return(1);
}

XrmDatabase XrmGetDatabase(Display *display)
{
  return(display->db);
}

char *XSetLocaleModifiers(char *modifiers)
{
  return((char *) NULL);
}

Bool XSupportsLocale()
{
  return(0);
}
#endif

#if defined(PRE_R4_ICCCM)
/*
  Compatibility methods for pre X11R4 ICCCM.
*/
XClassHint *XAllocClassHint)
{
  returnMagickAllocateMemory((XClassHint *,sizeof(XClassHint)));
}

XIconSize *XAllocIconSize)
{
  returnMagickAllocateMemory((XIconSize *,sizeof(XIconSize)));
}

XSizeHints *XAllocSizeHints)
{
  returnMagickAllocateMemory((XSizeHints *,sizeof(XSizeHints)));
}

Status XReconfigureWMWindow(Display *display,Window window,int screen_number,
  unsigned int value_mask,XWindowChanges *values)
{
  return(XConfigureWindow(display,window,value_mask,values));
}

XStandardColormap *XAllocStandardColormap)
{
  returnMagickAllocateMemory((XStandardColormap *,sizeof(XStandardColormap)));
}

XWMHints *XAllocWMHints)
{
  returnMagickAllocateMemory((XWMHints *,sizeof(XWMHints)));
}

Status XGetGCValues(Display *display,GC gc,unsigned long mask,
  XGCValues *values)
{
  return(True);
}

Status XGetRGBColormaps(Display *display,Window window,
  XStandardColormap **colormap,int *count,Atom property)
{
  *count=1;
  return(XGetStandardColormap(display,window,*colormap,property));
}

Status XGetWMColormapWindows(Display *display,Window window,
  Window **colormap_windows,int *number_windows)
{
  Atom
    actual_type,
    *data,
    property;

  int
    actual_format,
    status;

  unsigned long
    leftover,
    number_items;

  property=XInternAtom(display,"WM_COLORMAP_WINDOWS",False);
  if (property == None)
    return(False);
  /*
    Get the window property.
  */
  *data=(Atom) NULL;
  status=XGetWindowProperty(display,window,property,0L,1000000L,False,
    XA_WINDOW,&actual_type,&actual_format,&number_items,&leftover,
    (unsigned char **) &data);
  if (status != Success)
    return(False);
  if ((actual_type != XA_WINDOW) || (actual_format != 32))
    {
      if (data != (Atom *) NULL)
        XFree((char *) data);
      return(False);
    }
  *colormap_windows=(Window *) data;
  *number_windows=(int) number_items;
  return(True);
}

Status XGetWMName(Display *display,Window window,XTextProperty *text_property)
{
  char
    *window_name;

  if (XFetchName(display,window,&window_name) == 0)
    return(False);
  text_property->value=(unsigned char *) window_name;
  text_property->encoding=XA_STRING;
  text_property->format=8;
  text_property->nitems=strlen(window_name);
  return(True);
}

char *XResourceManagerString(Display *display)
{
  return(display->xdefaults);
}

void XrmDestroyDatabase(XrmDatabase database)
{
}

void XSetWMIconName(Display *display,Window window,XTextProperty *property)
{
  XSetIconName(display,window,property->value);
}

void XSetWMName(Display *display,Window window,XTextProperty *property)
{
  XStoreName(display,window,property->value);
}

void XSetWMProperties(Display *display,Window window,
  XTextProperty *window_name,XTextProperty *icon_name,char **argv,
  int argc,XSizeHints *size_hints,XWMHints *manager_hints,
  XClassHint *class_hint)
{
  XSetStandardProperties(display,window,window_name->value,icon_name->value,
    None,argv,argc,size_hints);
  XSetWMHints(display,window,manager_hints);
  XSetClassHint(display,window,class_hint);
}

Status XSetWMProtocols(Display *display,Window window,Atom *protocols,
  int count)
{
  Atom
    wm_protocols;

  wm_protocols=XInternAtom(display,"WM_PROTOCOLS",False);
  XChangeProperty(display,window,wm_protocols,XA_ATOM,32,PropModeReplace,
    (unsigned char *) protocols,count);
  return(True);
}

int XStringListToTextProperty(char **argv,int argc,XTextProperty *property)
{
  register int
    i;

  register unsigned int
    number_bytes;

  XTextProperty
     protocol;

  number_bytes=0;
  for (i=0; i < argc; i++)
    number_bytes+=(unsigned int) ((argv[i] ? strlen(argv[i]) : 0)+1);
  protocol.encoding=XA_STRING;
  protocol.format=8;
  protocol.nitems=0;
  if (number_bytes)
    protocol.nitems=number_bytes-1;
  protocol.value=NULL;
  if (number_bytes <= 0)
    {
      protocol.value=MagickAllocateMemory(unsigned char *,1);
      if (!protocol.value)
        return(False);
      *protocol.value='\0';
    }
  else
    {
      register char
        *buffer;

      buffer=MagickAllocateMemory(char *,number_bytes);
      if (buffer == (char *) NULL)
        return(False);
      protocol.value=(unsigned char *) buffer;
      for (i=0; i < argc; i++)
      {
        char
          *argument;

        argument=argv[i];
        if (!argument)
          *buffer++='\0';
        else
          {
            (void) strlcpy(buffer,argument,MaxTextExtent);
            buffer+=(strlen(argument)+1);
          }
      }
    }
  *property=protocol;
  return(True);
}

VisualID XVisualIDFromVisual(Visual *visual)
{
  return(visual->visualid);
}

Status XWithdrawWindow(Display *display,Window window,int screen)
{
  return(XUnmapWindow(display,window));
}

int XWMGeometry(Display *display,int screen,char *user_geometry,
  char *default_geometry,unsigned int border_width,XSizeHints *size_hints,
  int *x,int *y,int *width,int *height,int *gravity)
{
  int
    status;

  status=XGeometry(display,screen,user_geometry,default_geometry,border_width,
    0,0,0,0,x,y,width,height);
  *gravity=NorthWestGravity;
  return(status);
}
#endif

#endif
