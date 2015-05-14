/*
% Copyright (C) 2003 - 2014 GraphicsMagick Group
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
%                      SSSSS  H   H  EEEEE   AAA    RRRR                      %
%                      SS     H   H  E      A   A   R   R                     %
%                       SSS   HHHHH  EEE    AAAAA   RRRR                      %
%                         SS  H   H  E      A   A   R R                       %
%                      SSSSS  H   H  EEEEE  A   A   R  R                      %
%                                                                             %
%                                                                             %
%            Methods to Shear or Rotate an Image by an Arbitrary Angle        %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                  July 1992                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RotateImage, XShearImage, and YShearImage is based on the paper
%  "A Fast Algorithm for General Raster Rotatation" by Alan W. Paeth,
%  Graphics Interface '86 (Vancouver).  RotateImage is adapted from a similar
%  method based on the Paeth paper written by Michael Halle of the Spatial
%  Imaging Group, MIT Media Lab.
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/alpha_composite.h"
#include "magick/color.h"
#include "magick/decorate.h"
#include "magick/log.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/render.h"
#include "magick/shear.h"
#include "magick/transform.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A f f i n e T r a n s f o r m I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AffineTransformImage() transforms an image as dictated by the affine matrix.
%  It allocates the memory necessary for the new Image structure and returns
%  a pointer to the new image.
%
%  The format of the AffineTransformImage method is:
%
%      Image *AffineTransformImage(const Image *image,
%        AffineMatrix *affine,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o affine: The affine transform.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *
AffineTransformImage(const Image *image,const AffineMatrix *affine,
		     ExceptionInfo *exception)
{
  AffineMatrix
    transform;

  Image
    *affine_image;

  long
    y;

  PointInfo
    extent[4],
    min,
    max;

  register long
    i,
    x;

  /*
    Determine bounding box.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(affine != (AffineMatrix *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  extent[0].x=0;
  extent[0].y=0;
  extent[1].x=image->columns;
  extent[1].y=0;
  extent[2].x=image->columns;
  extent[2].y=image->rows;
  extent[3].x=0;
  extent[3].y=image->rows;
  for (i=0; i < 4; i++)
  {
    x=(long) (extent[i].x+0.5);
    y=(long) (extent[i].y+0.5);
    extent[i].x=x*affine->sx+y*affine->ry+affine->tx;
    extent[i].y=x*affine->rx+y*affine->sy+affine->ty;
  }
  min=extent[0];
  max=extent[0];
  for (i=1; i < 4; i++)
  {
    if (min.x > extent[i].x)
      min.x=extent[i].x;
    if (min.y > extent[i].y)
      min.y=extent[i].y;
    if (max.x < extent[i].x)
      max.x=extent[i].x;
    if (max.y < extent[i].y)
      max.y=extent[i].y;
  }
  /*
    Affine transform image.
  */
  affine_image=CloneImage(image,(unsigned long) ceil(max.x-min.x-0.5),
    (unsigned long) ceil(max.y-min.y-0.5),True,exception);
  if (affine_image == (Image *) NULL)
    return((Image *) NULL);
  (void) SetImage(affine_image,TransparentOpacity);
  transform.sx=affine->sx;
  transform.rx=affine->rx;
  transform.ry=affine->ry;
  transform.sy=affine->sy;
  transform.tx=(-min.x);
  transform.ty=(-min.y);
  (void) DrawAffineImage(affine_image,image,&transform);
  return(affine_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   A u t o O r i e n t I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AutoOrientImage() returns an image adjusted so that its orientation is
%  suitable for viewing (i.e. top-left orientation).  
%
%  The format of the AutoOrientImage method is:
%
%      Image *AutoOrientImage(const Image *image,
%        const OrientationType current_orientation,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o current_orientation: Current image orientation (normally same as
%                   image->orientation).
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *
AutoOrientImage(const Image *image,
                const OrientationType current_orientation,
                ExceptionInfo *exception)
{
  Image
    *orient_image;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  orient_image=(Image *) NULL;

  /*
       1        2       3      4         5            6           7          8

    888888  888888      88  88      8888888888  88                  88  8888888888
    88          88      88  88      88  88      88  88          88  88      88  88
    8888      8888    8888  8888    88          8888888888  8888888888          88
    88          88      88  88
    88          88  888888  888888
   */

  switch(current_orientation)
    {
    case UndefinedOrientation:
    case TopLeftOrientation: /* 1 */
    default:
      {
        orient_image=CloneImage(image,0,0,MagickTrue,exception);
        break;
      }
    case TopRightOrientation: /* 2 */
      {
        orient_image=FlopImage(image,exception);
        break;
      }
    case BottomRightOrientation: /* 3 */
      {
        orient_image=RotateImage(image,180.0,exception);
        break;
      }
    case BottomLeftOrientation: /* 4 */
      {
        orient_image=FlipImage(image,exception);
        break;
      }
    case LeftTopOrientation: /* 5 */
      {
        Image
          *rotate_image;

        rotate_image=RotateImage(image,90,exception);
        if (rotate_image != (Image *) NULL)
          {
            orient_image=FlopImage(rotate_image,exception);
            DestroyImage(rotate_image);
          }

        break;
      }
    case RightTopOrientation: /* 6 */
      {
        orient_image=RotateImage(image,90.0,exception);
        break;
      }
    case RightBottomOrientation: /* 7 */
      {
        Image
          *rotate_image;

        rotate_image=RotateImage(image,270,exception);
        if (rotate_image != (Image *) NULL)
          {
            orient_image=FlopImage(rotate_image,exception);
            DestroyImage(rotate_image);
          }
        break;
      }
    case LeftBottomOrientation: /* 8 */
      {
        orient_image=RotateImage(image,270.0,exception);
        break;
      }
    };

  if (orient_image != (Image *) NULL)
    orient_image->orientation=TopLeftOrientation;

  return orient_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C r o p T o F i t I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method CropToFitImage crops the sheared image as determined by the bounding
%  box as defined by width and height and shearing angles.
%
%  The format of the CropToFitImage method is:
%
%      MagickPassFail CropToFitImage(Image **image,const double x_shear,
%        const double x_shear,const double width,const double height,
%        const unsigne int rotate,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o image: The address of a structure of type Image.
%
%    o x_shear, y_shear, width, height: Defines a region of the image to crop.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static MagickPassFail
CropToFitImage(Image **image,
	       const double x_shear,const double y_shear,
	       const double width,const double height,
	       const unsigned int rotate,ExceptionInfo *exception)
{
  Image
    *crop_image;

  PointInfo
    extent[4],
    min,
    max;

  RectangleInfo
    geometry;

  register long
    i;

  /*
    Calculate the rotated image size.
  */
  extent[0].x=(-width/2.0);
  extent[0].y=(-height/2.0);
  extent[1].x=width/2.0;
  extent[1].y=(-height/2.0);
  extent[2].x=(-width/2.0);
  extent[2].y=height/2.0;
  extent[3].x=width/2.0;
  extent[3].y=height/2.0;
  for (i=0; i < 4; i++)
  {
    extent[i].x+=x_shear*extent[i].y;
    extent[i].y+=y_shear*extent[i].x;
    if (rotate)
      extent[i].x+=x_shear*extent[i].y;
    extent[i].x+=(double) (*image)->columns/2.0;
    extent[i].y+=(double) (*image)->rows/2.0;
  }
  min=extent[0];
  max=extent[0];
  for (i=1; i < 4; i++)
  {
    if (min.x > extent[i].x)
      min.x=extent[i].x;
    if (min.y > extent[i].y)
      min.y=extent[i].y;
    if (max.x < extent[i].x)
      max.x=extent[i].x;
    if (max.y < extent[i].y)
      max.y=extent[i].y;
  }
  geometry.width=(unsigned long) floor(max.x-min.x+0.5);
  geometry.height=(unsigned long) floor(max.y-min.y+0.5);
  geometry.x=(long) ceil(min.x-0.5);
  geometry.y=(long) ceil(min.y-0.5);
  crop_image=CropImage(*image,&geometry,exception);
  if (crop_image != (Image *) NULL)
    crop_image->page=(*image)->page;
  DestroyImage(*image);
  *image=crop_image;

  return (*image != (Image *) NULL ? MagickPass : MagickFail);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n t e g r a l R o t a t e I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IntegralRotateImage rotates the image an integral of 90 degrees.
%  It allocates the memory necessary for the new Image structure and returns
%  a pointer to the rotated image.
%
%  The format of the IntegralRotateImage method is:
%
%      Image *IntegralRotateImage(const Image *image,unsigned int rotations,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o rotate_image: Method IntegralRotateImage returns a pointer to the
%      rotated image.  A null image is returned if there is a a memory shortage.
%
%    o image: The image.
%
%    o rotations: Specifies the number of 90 degree rotations.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#if 1
#if !defined(DisableSlowOpenMP)
#  define IntegralRotateImageUseOpenMP
#  define RotateThreads (Min(2,omp_get_max_threads()))
#endif
#endif
static Image *
IntegralRotateImage(const Image *image,unsigned int rotations,
		    ExceptionInfo *exception)
{
  char
    message[MaxTextExtent];

  Image
    *rotate_image;

  RectangleInfo
    page;

  long
    tile_width_max,
    tile_height_max;

  MagickPassFail
    status=MagickPass;

  /*
    Initialize rotated image attributes.
  */
  assert(image != (Image *) NULL);
  page=image->page;
  rotations%=4;

  {
    /*
      Clone appropriately to create rotate image.
    */
    unsigned long
      clone_columns=0,
      clone_rows=0;
    
    switch (rotations)
      {
      case 0:
	clone_columns=0;
	clone_rows=0;
	break;
      case 2:
	clone_columns=image->columns;
	clone_rows=image->rows;
	break;
      case 1:
      case 3:
	clone_columns=image->rows;
	clone_rows=image->columns;
	break;
      }
    rotate_image=CloneImage(image,clone_columns,clone_rows,True,exception);
    if (rotate_image == (Image *) NULL)
      return((Image *) NULL);
    if (rotations != 0)
      if (ModifyCache(rotate_image,exception) != MagickPass)
	{
	  DestroyImage(rotate_image);
	  return (Image *) NULL;
	}
  }

  tile_height_max=tile_width_max=2048/sizeof(PixelPacket); /* 2k x 2k = 4MB */
  if ((rotations == 1) || (rotations == 3))
    {
      /*
	Allow override of tile geometry for testing.
      */
      const char *
	value;

      if (!GetPixelCacheInCore(image) || !GetPixelCacheInCore(rotate_image))
	tile_height_max=tile_width_max=8192/sizeof(PixelPacket); /* 8k x 8k = 64MB */

      if ((value=getenv("MAGICK_ROTATE_TILE_GEOMETRY")))
	{
	  double
	    width,
	    height;
	  
	  if (GetMagickDimension(value,&width,&height,NULL,NULL) == 2)
	    {
	      tile_height_max=(unsigned long) height;
	      tile_width_max=(unsigned long) width;
	    }
	}
    }

  /*
    Integral rotate the image.
  */
  switch (rotations)
    {
    case 0:
      {
        /*
          Rotate 0 degrees (nothing more to do).
        */
	(void) strlcpy(message,"[%s] Rotate: 0 degrees...",sizeof(message));
	if (!MagickMonitorFormatted(image->rows-1,image->rows,exception,
				    message,image->filename))
	  status=MagickFail;
        break;
      }
    case 1:
      {
        /*
          Rotate 90 degrees.
        */
        magick_int64_t
          tile;

        magick_uint64_t
          total_tiles;

        long
          tile_y;

#if defined(IntegralRotateImageUseOpenMP)
#  if defined(HAVE_OPENMP)
        int
          rotate_threads = RotateThreads;
#  endif
#endif

        (void) strlcpy(message,"[%s] Rotate: 90 degrees...",sizeof(message));
        total_tiles=(((image->rows/tile_height_max)+1)*
                     ((image->columns/tile_width_max)+1));        
        tile=0;

#if defined(IntegralRotateImageUseOpenMP)
#  if defined(HAVE_OPENMP)
#    if defined(TUNE_OPENMP)
#      pragma omp parallel for schedule(runtime) shared(status, tile)
#    else
#      pragma omp parallel for num_threads(rotate_threads) schedule(static,1) shared(status, tile)
#    endif
#  endif
#endif
        for (tile_y=0; tile_y < (long) image->rows; tile_y+=tile_height_max)
          {
            long
              tile_x;

            MagickPassFail
              thread_status;

#if defined(IntegralRotateImageUseOpenMP)
#  if defined(HAVE_OPENMP)
#    pragma omp critical (GM_IntegralRotateImage)
#  endif
#endif
            thread_status=status;
            if (thread_status == MagickFail)
              continue;

            for (tile_x=0; tile_x < (long) image->columns; tile_x+=tile_width_max)
              {
                long
                  dest_tile_x,
                  dest_tile_y;

                long
                  tile_width,
                  tile_height;

                const PixelPacket
                  *tile_pixels;

                long
                  y;

                /*
                  Compute image region corresponding to tile.
                */
                if ((unsigned long) tile_x+tile_width_max > image->columns)
                  tile_width=(tile_width_max-(tile_x+tile_width_max-image->columns));
                else
                  tile_width=tile_width_max;
                if ((unsigned long) tile_y+tile_height_max > image->rows)
                  tile_height=(tile_height_max-(tile_y+tile_height_max-image->rows));
                else
                  tile_height=tile_height_max;
                /*
                  Acquire tile
                */
                tile_pixels=AcquireImagePixels(image,tile_x,tile_y,
                                               tile_width,tile_height,exception);
                if (tile_pixels == (const PixelPacket *) NULL)
                  {
                    thread_status=MagickFail;
                    break;
                  }
                /*
                  Compute destination tile coordinates.
                */
                dest_tile_x=rotate_image->columns-(tile_y+tile_height);
                dest_tile_y=tile_x;
                /*
                  Rotate tile
                */
                for (y=0; y < tile_width; y++)
                  {
                    register const PixelPacket
                      *p;
                    
                    register PixelPacket
                      *q;

                    register const IndexPacket
                      *indexes;
        
                    IndexPacket
                      *rotate_indexes;

                    register long
                      x;

                    q=SetImagePixelsEx(rotate_image,dest_tile_x,dest_tile_y+y,
                                       tile_height,1,exception);
                    if (q == (PixelPacket *) NULL)
                      {
                        thread_status=MagickFail;
                        break;
                      }
                    /*
                      DirectClass pixels
                    */
                    p=tile_pixels+(tile_height-1)*tile_width + y;
                    for (x=tile_height; x != 0; x--) 
                      {
                        *q = *p;
                        q++;
                        p-=tile_width;
                      }
                    /*
                      Indexes
                    */
                    indexes=AccessImmutableIndexes(image);
                    if (indexes != (IndexPacket *) NULL)
                      {
                        rotate_indexes=AccessMutableIndexes(rotate_image);
                        if (rotate_indexes != (IndexPacket *) NULL)
                          {
                            register IndexPacket
                              *iq;
                            
                            register const IndexPacket
                              *ip;

                            iq=rotate_indexes;
                            ip=indexes+(tile_height-1)*tile_width + y;
                            for (x=tile_height; x != 0; x--) 
                              {
                                *iq = *ip;
                                iq++;
                                ip -= tile_width;
                              }
                          }
                      }
                    if (!SyncImagePixelsEx(rotate_image,exception))
                      {
                        thread_status=MagickFail;
                        break;
                      }
                  }

#if defined(IntegralRotateImageUseOpenMP)
#  if defined(HAVE_OPENMP)
#    pragma omp critical (GM_IntegralRotateImage)
#  endif
#endif
                {
                  tile++;
                  if (QuantumTick(tile,total_tiles))
                    if (!MagickMonitorFormatted(tile,total_tiles,exception,
                                                message,image->filename))
                      thread_status=MagickFail;
                  
                  if (thread_status == MagickFail)
                    status=MagickFail;
                }
              }
          }
        Swap(page.width,page.height);
        Swap(page.x,page.y);
        page.x=(long) (page.width-rotate_image->columns-page.x);
        break;
      }
    case 2:
      {
        /*
          Rotate 180 degrees.
        */
        long
          y;

        unsigned long
          row_count=0;

#if defined(IntegralRotateImageUseOpenMP)
#  if defined(HAVE_OPENMP)
        int
          rotate_threads = RotateThreads;
#  endif
#endif

        (void) strlcpy(message,"[%s] Rotate: 180 degrees...",sizeof(message));
#if defined(IntegralRotateImageUseOpenMP)
#  if defined(HAVE_OPENMP)
#    if defined(TUNE_OPENMP)
#      pragma omp parallel for schedule(runtime) shared(row_count, status)
#    else
#      pragma omp parallel for num_threads(rotate_threads) schedule(static,8) shared(row_count, status)
#    endif
#  endif
#endif
        for (y=0; y < (long) image->rows; y++)
          {
            register const PixelPacket
              *p;

            register PixelPacket
              *q;

            register const IndexPacket
              *indexes;
        
            IndexPacket
              *rotate_indexes;

            register long
              x;

            MagickPassFail
              thread_status;

#if defined(IntegralRotateImageUseOpenMP)
#  if defined(HAVE_OPENMP)
#    pragma omp critical (GM_IntegralRotateImage)
#  endif
#endif
            thread_status=status;
            if (thread_status == MagickFail)
              continue;

            p=AcquireImagePixels(image,0,y,image->columns,1,exception);
            q=SetImagePixelsEx(rotate_image,0,(long) (image->rows-y-1),
                               image->columns,1,exception);
            if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
              thread_status=MagickFail;
            if (thread_status != MagickFail)
              {
                q+=image->columns;
                indexes=AccessImmutableIndexes(image);
                rotate_indexes=AccessMutableIndexes(rotate_image);
                if ((indexes != (IndexPacket *) NULL) &&
                    (rotate_indexes != (IndexPacket *) NULL))
                  for (x=0; x < (long) image->columns; x++)
                    rotate_indexes[image->columns-x-1]=indexes[x];
                for (x=0; x < (long) image->columns; x++)
                  *--q=(*p++);
                if (!SyncImagePixelsEx(rotate_image,exception))
                  thread_status=MagickFail;
              }
#if defined(IntegralRotateImageUseOpenMP)
#  if defined(HAVE_OPENMP)
#    pragma omp critical (GM_IntegralRotateImage)
#  endif
#endif
            {
              row_count++;
              if (QuantumTick(row_count,image->rows))
                if (!MagickMonitorFormatted(row_count,image->rows,exception,
                                            message,image->filename))
                  thread_status=MagickFail;
                  
              if (thread_status == MagickFail)
                status=MagickFail;
            }
          }
        page.x=(long) (page.width-rotate_image->columns-page.x);
        page.y=(long) (page.height-rotate_image->rows-page.y);
        break;
      }
    case 3:
      {
        /*
          Rotate 270 degrees.
        */

        magick_int64_t
          tile;

        magick_uint64_t
          total_tiles;

        long
          tile_y;

#if defined(IntegralRotateImageUseOpenMP)
#  if defined(HAVE_OPENMP)
        int
          rotate_threads = RotateThreads;
#  endif
#endif

        (void) strlcpy(message,"[%s] Rotate: 270 degrees...",sizeof(message));
        total_tiles=(((image->rows/tile_height_max)+1)*
                     ((image->columns/tile_width_max)+1));
        tile=0;
#if defined(IntegralRotateImageUseOpenMP)
#  if defined(HAVE_OPENMP)
#    if defined(TUNE_OPENMP)
#      pragma omp parallel for schedule(runtime) shared(status, tile)
#    else
#      pragma omp parallel for num_threads(rotate_threads) schedule(static,1) shared(status, tile)
#    endif
#  endif
#endif
        for (tile_y=0; tile_y < (long) image->rows; tile_y+=tile_height_max)
          {
            long
              tile_x;

            MagickPassFail
              thread_status;

#if defined(IntegralRotateImageUseOpenMP)
#  if defined(HAVE_OPENMP)
#    pragma omp critical (GM_IntegralRotateImage)
#  endif
#endif
            thread_status=status;
            if (thread_status == MagickFail)
              continue;

            for (tile_x=0; tile_x < (long) image->columns; tile_x+=tile_width_max)
              {
                long
                  tile_width,
                  tile_height;

                long
                  dest_tile_x,
                  dest_tile_y;

                long
                  y;

                const PixelPacket
                  *tile_pixels;
                    
                /*
                  Compute image region corresponding to tile.
                */
                if ((unsigned long) tile_x+tile_width_max > image->columns)
                  tile_width=(tile_width_max-(tile_x+tile_width_max-image->columns));
                else
                  tile_width=tile_width_max;
                if ((unsigned long) tile_y+tile_height_max > image->rows)
                  tile_height=(tile_height_max-(tile_y+tile_height_max-image->rows));
                else
                  tile_height=tile_height_max;
                /*
                  Acquire tile
                */
                tile_pixels=AcquireImagePixels(image,tile_x,tile_y,
                                               tile_width,tile_height,exception);
                if (tile_pixels == (const PixelPacket *) NULL)
                  {
                    thread_status=MagickFail;
                    break;
                  }
                /*
                  Compute destination tile coordinates.
                */
                dest_tile_x=tile_y;
                dest_tile_y=rotate_image->rows-(tile_x+tile_width);
                /*
                  Rotate tile
                */
                for (y=0; y < tile_width; y++)
                  {
                    register const PixelPacket
                      *p;
                    
                    register PixelPacket
                      *q;

                    register const IndexPacket
                      *indexes;

                    register long
                      x;

                    IndexPacket
                      *rotate_indexes;

                    q=SetImagePixelsEx(rotate_image,dest_tile_x,dest_tile_y+y,
                                       tile_height,1,exception);
                    if (q == (PixelPacket *) NULL)
                      {
                        thread_status=MagickFail;
                        break;
                      }
                    /*
                      DirectClass pixels
                    */
                    p=tile_pixels+(tile_width-1-y);
                    for (x=tile_height; x != 0; x--)
                      {
                        *q = *p;
                        q++;
                        p += tile_width;
                      }
                    /*
                      Indexes
                    */
                    indexes=AccessImmutableIndexes(image);
                    if (indexes != (IndexPacket *) NULL)
                      {
                        rotate_indexes=AccessMutableIndexes(rotate_image);
                        if (rotate_indexes != (IndexPacket *) NULL)
                          {
                            register IndexPacket
                              *iq;
                            
                            register const IndexPacket
                              *ip;

                            iq=rotate_indexes;
                            ip=indexes+(tile_width-1-y);
                            for (x=tile_height; x != 0; x--)
                              {
                                *iq = *ip;
                                iq++;
                                ip += tile_width;
                              }
                          }
                      }
                    if (!SyncImagePixelsEx(rotate_image,exception))
                      {
                        thread_status=MagickFail;
                        break;
                      }
                  }

#if defined(IntegralRotateImageUseOpenMP)
#  if defined(HAVE_OPENMP)
#    pragma omp critical (GM_IntegralRotateImage)
#  endif
#endif
                {
                  tile++;
                  if (QuantumTick(tile,total_tiles))
                    if (!MagickMonitorFormatted(tile,total_tiles,exception,
                                                message,image->filename))
                      thread_status=MagickFail;

		  if (thread_status == MagickFail)
		    status=MagickFail;
                }

                if (thread_status == MagickFail)
		  break;
              }
          }
        Swap(page.width,page.height);
        Swap(page.x,page.y);
        page.y=(long) (page.height-rotate_image->rows-page.y);
        break;
      }
    }

  rotate_image->page=page;
  rotate_image->is_grayscale=image->is_grayscale;
  rotate_image->is_monochrome=image->is_monochrome;
  return(rotate_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   X S h e a r I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure XShearImage shears the image in the X direction with a shear angle
%  of 'degrees'.  Positive angles shear counter-clockwise (right-hand rule),
%  and negative angles shear clockwise.  Angles are measured relative to a
%  vertical Y-axis.  X shears will widen an image creating 'empty' triangles
%  on the left and right sides of the source image.
%
%  The format of the XShearImage method is:
%
%      MagickPassFail XShearImage(Image *image,const double degrees,
%        const unsigned long width,const unsigned long height,
%        const long x_offset,long y_offset,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o degrees: A double representing the shearing angle along the X axis.
%
%    o width, height, x_offset, y_offset: Defines a region of the image
%      to shear.
%
%    o exception: Return any errors or warnings in this structure.
%
*/

static MagickPassFail
XShearImage(Image *image,const double degrees,
	    const unsigned long width,const unsigned long height,
	    const long x_offset,long y_offset,ExceptionInfo *exception)
{
#define XShearImageText  "[%s] X Shear: %+g degrees, region %lux%lu%+ld%+ld...  "

  long
    y,
    xr_offset;

  unsigned long
    row_count=0;

  unsigned int
    is_grayscale;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  is_grayscale=image->is_grayscale;

  assert(x_offset >= 0);
  assert(x_offset < (long) image->columns);
  assert(y_offset >= 0);
  assert(y_offset < (long) image->rows);
  assert(width <= (image->columns-(unsigned long) x_offset));
  assert(height <= (image->rows-(unsigned long) y_offset));
  xr_offset=image->columns-width-x_offset;

#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    if defined(USE_STATIC_SCHEDULING_ONLY)
#      pragma omp parallel for schedule(static) shared(row_count, status)
#    else
#      pragma omp parallel for schedule(dynamic) shared(row_count, status)
#    endif
#  endif
#endif
  for (y=0; y < (long) height; y++)
    {
      double
        alpha,
        displacement;

      long
        step,
        skip;

      PixelPacket
        pixel;

      register long
        i;

      register PixelPacket
        *p,
        *q;

      enum
        {
          LEFT,
          RIGHT
        } direction;

      MagickPassFail
        thread_status;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_XShearImage)
#endif
      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      displacement=degrees*(y-height/2.0);
      if (displacement == 0.0)
        continue;
      if (displacement > 0.0)
        direction=RIGHT;
      else
        {
          displacement*=(-1.0);
          direction=LEFT;
        }
      step=(long) floor(displacement);
      alpha=MaxRGBDouble*(displacement-step);
      if (alpha == 0.0)
        {
          /*
            No fractional displacement-- just copy.
          */
          switch (direction)
            {
            case LEFT:
              {
                /*
                  Transfer pixels left-to-right.
                */
                skip = (step > x_offset) ? step - x_offset : 0;
                p=GetImagePixelsEx(image,0,y+y_offset,image->columns,1,exception);
                if (p == (PixelPacket *) NULL)
                  {
                    thread_status=MagickFail;
                    break;
                  }
                p+=x_offset+skip;
                q=p-step;
                (void) memcpy(q,p,(width-(unsigned long)skip)*sizeof(PixelPacket));
                q+=width;
                for (i=0; i < step; i++)
                  *q++=image->background_color;
                break;
              }
            case RIGHT:
              {
                /*
                  Transfer pixels right-to-left.
                */
                skip = (step > xr_offset) ? step - xr_offset : 0;
                p=GetImagePixelsEx(image,0,y+y_offset,image->columns,1,exception);
                if (p == (PixelPacket *) NULL)
                  {
                    thread_status=MagickFail;
                    break;
                  }
                p+=x_offset+width-skip;
                q=p+step;
                for (i=0; i < ((long) width - skip); i++)
                  *--q=(*--p);
                for (i=0; i < step; i++)
                  *--q=image->background_color;
                break;
              }
            }
          if (!SyncImagePixelsEx(image,exception))
            thread_status=MagickFail;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_XShearImage)
#endif
          {
            row_count++;
            if (QuantumTick(row_count,height))
              if (!MagickMonitorFormatted(row_count,height,exception,
                                          XShearImageText,image->filename,
					  degrees,width,height,
					  x_offset,y_offset))
                thread_status=MagickFail;
            
            if (thread_status == MagickFail)
              status=MagickFail;
          }

          continue;
        }
      /*
        Fractional displacement.
      */
      step++;
      pixel=image->background_color;
      switch (direction)
        {
        case LEFT:
          {
            /*
              Transfer pixels left-to-right.
            */
            p=GetImagePixelsEx(image,0,y+y_offset,image->columns,1,exception);
            if (p == (PixelPacket *) NULL)
              {
                thread_status=MagickFail;
                break;
              }
            p+=x_offset;
            q=p-step;
            for (i=0; i < (long) width; i++)
              {
                if ((x_offset+i) < step)
                  {
                    pixel=(*++p);
                    q++;
                    continue;
                  }
                BlendCompositePixel(q,&pixel,p,alpha);
                q++;
                pixel=(*p++);
              }
            BlendCompositePixel(q,&pixel,&image->background_color,alpha);
            q++;
            for (i=0; i < (step-1); i++)
              *q++=image->background_color;
            break;
          }
        case RIGHT:
          {
            /*
              Transfer pixels right-to-left.
            */
            p=GetImagePixelsEx(image,0,y+y_offset,image->columns,1,exception);
            if (p == (PixelPacket *) NULL)
              {
                thread_status=MagickFail;
                break;
              }
            p+=x_offset+width;
            q=p+step;
            for (i=0; i < (long) width; i++)
              {
                p--;
                q--;
                if ((x_offset+width+step-i) > image->columns)
                  continue;
                BlendCompositePixel(q,&pixel,p,alpha);
                pixel=(*p);
              }
            --q;
            BlendCompositePixel(q,&pixel,&image->background_color,alpha);
            for (i=0; i < (step-1); i++)
              *--q=image->background_color;
            break;
          }
        }
      if (!SyncImagePixelsEx(image,exception))
        thread_status=MagickFail;
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_XShearImage)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,height))
          if (!MagickMonitorFormatted(row_count,height,exception,
                                          XShearImageText,image->filename,
					  degrees,width,height,
					  x_offset,y_offset))
            thread_status=MagickFail;
        
        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }
  if (is_grayscale && IsGray(image->background_color))
    image->is_grayscale=True;

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   Y S h e a r I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Procedure YShearImage shears the image in the Y direction with a shear
%  angle of 'degrees'.  Positive angles shear counter-clockwise (right-hand
%  rule), and negative angles shear clockwise.  Angles are measured relative
%  to a horizontal X-axis.  Y shears will increase the height of an image
%  creating 'empty' triangles on the top and bottom of the source image.
%
%  The format of the YShearImage method is:
%
%      MagickPassFail YShearImage(Image *image,const double degrees,
%        const unsigned long width,const unsigned long height,long x_offset,
%        const long y_offset,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o degrees: A double representing the shearing angle along the Y axis.
%
%    o width, height, x_offset, y_offset: Defines a region of the image
%      to shear.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static MagickPassFail
YShearImage(Image *image,const double degrees,
	    const unsigned long width,const unsigned long height,long x_offset,
	    const long y_offset,ExceptionInfo *exception)
{
#define YShearImageText  "[%s] Y Shear: %+g degrees, region %lux%lu%+ld%+ld...  "

  long
    x,
    yr_offset;

  unsigned long
    row_count=0;

  unsigned int
    is_grayscale;

  MagickPassFail
    status=MagickPass;

  assert(image != (Image *) NULL);
  is_grayscale=image->is_grayscale;

  assert(x_offset >= 0);
  assert(x_offset < (long) image->columns);
  assert(y_offset >= 0);
  assert(y_offset < (long) image->rows);
  assert(width <= (image->columns-(unsigned long) x_offset));
  assert(height <= (image->rows-(unsigned long) y_offset));
  yr_offset=image->rows-height-y_offset;

#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    if defined(USE_STATIC_SCHEDULING_ONLY)
#      pragma omp parallel for schedule(static) shared(row_count, status)
#    else
#      pragma omp parallel for schedule(dynamic) shared(row_count, status)
#    endif
#  endif
#endif
  for (x=0; x < (long) width; x++)
    {
      double
        alpha,
        displacement;

      enum
        {
          UP,
          DOWN
        } direction;

      long
        step,
        skip;

      register PixelPacket
        *p,
        *q;

      register long
        i;

      PixelPacket
        pixel;

      MagickPassFail
        thread_status;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_YShearImage)
#endif
      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      displacement=degrees*(x-width/2.0);
      if (displacement == 0.0)
        continue;
      if (displacement > 0.0)
        direction=DOWN;
      else
        {
          displacement*=(-1.0);
          direction=UP;
        }
      step=(long) floor(displacement);
      alpha=(double) MaxRGB*(displacement-step);
      if (alpha == 0.0)
        {
          /*
            No fractional displacement-- just copy the pixels.
          */
          switch (direction)
            {
            case UP:
              {
                /*
                  Transfer pixels top-to-bottom.
                */
                skip = (step > y_offset) ? step - y_offset : 0;
                p=GetImagePixelsEx(image,x+x_offset,0,1,image->rows,exception);
                if (p == (PixelPacket *) NULL)
                  {
                    thread_status=MagickFail;
                    break;
                  }
                p+=y_offset+skip;
                q=p-step;
                (void) memcpy(q,p,(height-(unsigned long)skip)*sizeof(PixelPacket));
                q+=height;
                for (i=0; i < (long) step; i++)
                  *q++=image->background_color;
                break;
              }
            case DOWN:
              {
                /*
                  Transfer pixels bottom-to-top.
                */
                skip = (step > yr_offset) ? step - yr_offset : 0;
                p=GetImagePixelsEx(image,x+x_offset,0,1,image->rows,exception);
                if (p == (PixelPacket *) NULL)
                  {
                    thread_status=MagickFail;
                    break;
                  }
                p+=y_offset+height-skip;
                q=p+step;
                for (i=0; i < ((long) height - skip); i++)
                  *--q=(*--p);
                for (i=0; i < step; i++)
                  *--q=image->background_color;
                break;
              }
            }
          if (!SyncImagePixelsEx(image,exception))
            thread_status=MagickFail;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_YShearImage)
#endif
          {
            row_count++;
            if (QuantumTick(row_count,width))
              if (!MagickMonitorFormatted(row_count,width,exception,
                                          YShearImageText,image->filename,
					  degrees,width,height,
					  x_offset,y_offset))
                thread_status=MagickFail;
            
            if (thread_status == MagickFail)
              status=MagickFail;
          }

          continue;
        }
      /*
        Fractional displacment.
      */
      step++;
      pixel=image->background_color;
      switch (direction)
        {
        case UP:
          {
            /*
              Transfer pixels top-to-bottom.
            */
            p=GetImagePixelsEx(image,x+x_offset,0,1,image->rows,exception);
            if (p == (PixelPacket *) NULL)
              {
                thread_status=MagickFail;
                break;
              }
            p+=y_offset;
            q=p-step;
            for (i=0; i < (long) height; i++)
              {
                if ((y_offset+i) < step)
                  {
                    pixel=(*++p);
                    q++;
                    continue;
                  }
                BlendCompositePixel(q,&pixel,p,alpha);
                q++;
                pixel=(*p++);
              }
            BlendCompositePixel(q,&pixel,&image->background_color,alpha);
            q++;
            for (i=0; i < (step-1); i++)
              *q++=image->background_color;
            break;
          }
        case DOWN:
          {
            /*
              Transfer pixels bottom-to-top.
            */
            p=GetImagePixelsEx(image,x+x_offset,0,1,image->rows,exception);
            if (p == (PixelPacket *) NULL)
              {
                thread_status=MagickFail;
                break;
              }
            p+=y_offset+height;
            q=p+step;
            for (i=0; i < (long) height; i++)
              {
                p--;
                q--;
                if ((y_offset+height+step-i) > image->rows)
                  continue;
                BlendCompositePixel(q,&pixel,p,alpha);
                pixel=(*p);
              }
            --q;
            BlendCompositePixel(q,&pixel,&image->background_color,alpha);
            for (i=0; i < (step-1); i++)
              *--q=image->background_color;
            break;
          }
        }
      if (!SyncImagePixelsEx(image,exception))
        thread_status=MagickFail;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_YShearImage)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,width))
          if (!MagickMonitorFormatted(row_count,width,exception,
                                      YShearImageText,image->filename,
				      degrees,width,height,
				      x_offset,y_offset))
            thread_status=MagickFail;
        
        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }
  if (is_grayscale && IsGray(image->background_color))
    image->is_grayscale=True;

  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R o t a t e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RotateImage creates a new image that is a rotated copy of an
%  existing one.  Positive angles rotate counter-clockwise (right-hand rule),
%  while negative angles rotate clockwise.  Rotated images are usually larger
%  than the originals and have 'empty' triangular corners.  X axis.  Empty
%  triangles left over from shearing the image are filled with the color
%  specified by the image background_color.  RotateImage allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.
%
%  Method RotateImage is based on the paper "A Fast Algorithm for General
%  Raster Rotatation" by Alan W. Paeth.  RotateImage is adapted from a similar
%  method based on the Paeth paper written by Michael Halle of the Spatial
%  Imaging Group, MIT Media Lab.
%
%  The format of the RotateImage method is:
%
%      Image *RotateImage(const Image *image,const double degrees,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o status: Method RotateImage returns a pointer to the image after
%      rotating.  A null image is returned if there is a memory shortage.
%
%    o image: The image;  returned from
%      ReadImage.
%
%    o degrees: Specifies the number of degrees to rotate the image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *
RotateImage(const Image *image,const double degrees,ExceptionInfo *exception)
{
  double
    angle;

  Image
    *integral_image = (Image *) NULL,
    *rotate_image = (Image *) NULL;

  long
    x_offset,
    y_offset;

  PointInfo
    shear;

  RectangleInfo
    border_info;

  unsigned long
    height,
    rotations,
    width,
    shear1_width,
    shear2_height,
    shear3_width,
    max_width,
    max_height;

  /*
    Adjust rotation angle.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  angle=degrees;
  while (angle < -45.0)
    angle+=360.0;
  for (rotations=0; angle > 45.0; rotations++)
    angle-=90.0;
  rotations%=4;
  /*
    Calculate shear equations.
  */
  integral_image=IntegralRotateImage(image,rotations,exception);
  if (integral_image == (Image *) NULL)
    goto rotate_image_exception;

  shear.x=(-tan(DegreesToRadians(angle)/2.0));
  shear.y=sin(DegreesToRadians(angle));
  if ((shear.x == 0.0) || (shear.y == 0.0))
    return(integral_image);
  /*
    Compute image size.
  */
  width=integral_image->columns;
  height=integral_image->rows;
  shear1_width=(unsigned long) floor(fabs(height*shear.x)+width+0.5);
  shear2_height=(unsigned long) floor(fabs(shear1_width*shear.y)+height+0.5);
  shear3_width=(unsigned long) floor(fabs(shear2_height*shear.x)+shear1_width+0.5);
  /*
    Compute maximum bounds to perform 3 shear operations.
    Add extra pixels to account for fractional displacement
  */
  max_width = ((shear3_width > shear1_width) ? shear3_width : shear1_width) + 2;
  max_height = shear2_height + 2;
  x_offset = (long) floor((max_width-width)/2.0+0.5);
  y_offset = (long) floor((max_height-height)/2.0+0.5);
  /*
    Surround image with a border.
  */
  integral_image->border_color=integral_image->background_color;
  border_info.width=x_offset;
  border_info.height=y_offset;
  rotate_image=BorderImage(integral_image,&border_info,exception);
  DestroyImage(integral_image);
  integral_image=(Image *) NULL;
  if (rotate_image == (Image *) NULL)
    goto rotate_image_exception;

  /*
    Rotate the image.
  */
  rotate_image->storage_class=DirectClass;
  rotate_image->matte|=rotate_image->background_color.opacity != OpaqueOpacity;
  
  if (XShearImage(rotate_image,shear.x,width,height,
          x_offset,y_offset,exception) != MagickPass)
    goto rotate_image_exception;

  if (YShearImage(rotate_image,shear.y,shear1_width,height,
		  (long) (rotate_image->columns-shear1_width)/2,y_offset,exception)
      != MagickPass)
    goto rotate_image_exception;

  if (XShearImage(rotate_image,shear.x,shear1_width,shear2_height,
		  (long) (rotate_image->columns-shear1_width)/2,
		  (long) (rotate_image->rows-shear2_height)/2,exception)
     != MagickPass)
    goto rotate_image_exception;

  if (CropToFitImage(&rotate_image,shear.x,shear.y,width,height,True,exception)
      != MagickPass)
    goto rotate_image_exception;

  rotate_image->page.width=0;
  rotate_image->page.height=0;
  return(rotate_image);

 rotate_image_exception:

  DestroyImage(integral_image);
  DestroyImage(rotate_image);
  return (Image *) NULL;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   S h e a r I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ShearImage creates a new image that is a shear_image copy of an
%  existing one.  Shearing slides one edge of an image along the X or Y
%  axis, creating a parallelogram.  An X direction shear slides an edge
%  along the X axis, while a Y direction shear slides an edge along the Y
%  axis.  The amount of the shear is controlled by a shear angle.  For X
%  direction shears, x_shear is measured relative to the Y axis, and
%  similarly, for Y direction shears y_shear is measured relative to the
%  X axis.  Empty triangles left over from shearing the image are filled
%  with the color defined by the pixel at location (0,0).  ShearImage
%  allocates the memory necessary for the new Image structure and returns
%  a pointer to the new image.
%
%  Method ShearImage is based on the paper "A Fast Algorithm for General
%  Raster Rotatation" by Alan W. Paeth.
%
%  The format of the ShearImage method is:
%
%      Image *ShearImage(const Image *image,const double x_shear,
%        const double y_shear,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o status: Method ShearImage returns a pointer to the image after
%      rotating.  A null image is returned if there is a memory shortage.
%
%    o image: The image;  returned from
%      ReadImage.
%
%    o x_shear, y_shear: Specifies the number of degrees to shear the image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *
ShearImage(const Image *image,const double x_shear,
	   const double y_shear,ExceptionInfo *exception)
{
  Image
    *integral_image = (Image *) NULL,
    *shear_image = (Image *) NULL;

  long
    x_offset,
    y_offset;

  PointInfo
    shear;

  RectangleInfo
    border_info;

  unsigned long
    y_width;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((x_shear == 180.0) || (y_shear == 180.0))
    ThrowImageException3(ImageError,UnableToShearImage,AngleIsDiscontinuous);

  /*
    Initialize shear angle.
  */
  integral_image=IntegralRotateImage(image,0,exception);
  if (integral_image == (Image *) NULL)
    goto shear_image_exception;
  shear.x=(-tan(DegreesToRadians(x_shear)/2.0));
  shear.y=sin(DegreesToRadians(y_shear));
  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
			"Shear angles x,y: %g,%g degrees", shear.x, shear.y);
  if ((shear.x == 0.0) && (shear.y == 0.0))
    return(integral_image);

  /*
    Compute image size.
  */
  x_offset=(long) ceil(fabs(2.0*image->rows*shear.x)-0.5);
  y_width=(unsigned long) floor(fabs(image->rows*shear.x)+image->columns+0.5);
  y_offset=(long) ceil(fabs(y_width*shear.y)-0.5);
  /*
    Surround image with border.
  */
  integral_image->border_color=integral_image->background_color;
  border_info.width=x_offset;
  border_info.height=y_offset;
  shear_image=BorderImage(integral_image,&border_info,exception);
  DestroyImage(integral_image);
  integral_image=(Image *) NULL;
  if (shear_image == (Image *) NULL)
    goto shear_image_exception;
  /*
    Shear the image.
  */
  shear_image->storage_class=DirectClass;
  shear_image->matte|=shear_image->background_color.opacity != OpaqueOpacity;
 
  if (XShearImage(shear_image,shear.x,image->columns,image->rows,x_offset,
		  (long) (shear_image->rows-image->rows)/2,exception)
      != MagickPass)
    goto shear_image_exception;
  
  if (YShearImage(shear_image,shear.y,y_width,image->rows,
		  (long) (shear_image->columns-y_width)/2,y_offset,exception)
      != MagickPass)
    goto shear_image_exception;
  
  if (CropToFitImage(&shear_image,shear.x,shear.y,image->columns,image->rows,
		     False,exception) != MagickPass)
    goto shear_image_exception;

  shear_image->page.width=0;
  shear_image->page.height=0;

  return(shear_image);

 shear_image_exception:

  DestroyImage(integral_image);
  DestroyImage(shear_image);
  return (Image *) NULL;
}
