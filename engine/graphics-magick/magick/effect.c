/*
% Copyright (C) 2003-2012 GraphicsMagick Group
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
%                   EEEEE  FFFFF  FFFFF  EEEEE  CCCC  TTTTT                   %
%                   E      F      F      E     C        T                     %
%                   EEE    FFF    FFF    EEE   C        T                     %
%                   E      F      F      E     C        T                     %
%                   EEEEE  F      F      EEEEE  CCCC    T                     %
%                                                                             %
%                                                                             %
%                    GraphicsMagick Image Effects Methods                     %
%                                                                             %
%                                                                             %
%                               Software Design                               %
%                                 John Cristy                                 %
%                                 October 1996                                %
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
#include "magick/channel.h"
#include "magick/color.h"
#include "magick/colormap.h"
#include "magick/effect.h"
#include "magick/enhance.h"
#include "magick/enum_strings.h"
#include "magick/gem.h"
#include "magick/log.h"
#include "magick/monitor.h"
#include "magick/omp_data_view.h"
#include "magick/operator.h"
#include "magick/pixel_cache.h"
#include "magick/pixel_iterator.h"
#include "magick/random-private.h"
#include "magick/random.h"
#include "magick/render.h"
#include "magick/shear.h"
#include "magick/utility.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A d a p t i v e T h r e s h o l d I m a g e                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AdaptiveThresholdImage() selects an individual threshold for each pixel
%  based on the range of intensity values in its local neighborhood.  This
%  allows for thresholding of an image whose global intensity histogram
%  doesn't contain distinctive peaks.
%
%  The format of the AdaptiveThresholdImage method is:
%
%      Image *AdaptiveThresholdImage(Image *image,const unsigned long width,
%        const unsigned long height,const unsigned long unsigned long,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o width: The width of the local neighborhood.
%
%    o height: The height of the local neighborhood.
%
%    o offset: The mean offset.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define AdaptiveThresholdImageText "[%s] Adaptive threshold..."
/* Macros for accessing dynamic preprocess buffer */
#define PRE(X,Y) ((Y + height + 2) % (height + 2)) * (image->columns + (width << 1)) + X

MagickExport Image *AdaptiveThresholdImage(const Image * image,
					   const unsigned long width,
					   const unsigned long height,
					   const double offset,
					   ExceptionInfo * exception)
{
  const PixelPacket * restrict
    p = (const PixelPacket *) NULL;

  Image
    * restrict threshold_image;

  LongPixelPacket
    * restrict dyn_process;

  const unsigned long
    local_area = width * height;

  unsigned long
    int i;

  /*
   *  allocates pre processing buffer,
   *
   *   (window height + 2) * (image width + 2 * width), filled with zero
   */
  const unsigned long
    dyn_process_size = (height + 2) * (image->columns + (width << 1));

  unsigned long
    row_count = 0UL;

  unsigned long
    x,
    y;

  const LongPixelPacket
    long_zero = { 0UL, 0UL, 0UL, 0UL };

  const long
    long_offset = (long) (offset*MaxMap/MaxRGB + 0.5);

  const MagickBool
    is_monochrome = image->is_monochrome,
    is_grayscale = image->is_grayscale;

  MagickPassFail
    status;

  const MagickBool
    matte = ((image->matte)
             || (image->colorspace == CMYKColorspace));

  unsigned long
    overflow_mask = 0x1UL << (sizeof(dyn_process[0].red)*8-1);

  MagickBool
    overflow_eminent;

  /*
    Initialize thresholded image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  if ((image->columns < width) || (image->rows < height))
    ThrowImageException3(OptionError, UnableToThresholdImage,
			 ImageSmallerThanRadius);

  threshold_image = CloneImage(image, 0, 0, MagickTrue, exception);
  if (threshold_image == (Image *) NULL)
    return ((Image *) NULL);
  if (is_monochrome)
    return threshold_image;
  (void) SetImageType(threshold_image, TrueColorType);
  status = MagickPass;

#if 0
  fprintf(stderr,"overflow_mask=%lx\n",overflow_mask);
  fprintf(stderr,"overflow pixels=%lu\n",(((~0UL) >> 1)/MaxMap));
#endif
  /*
    Adaptive threshold image.
  */
  dyn_process = MagickAllocateArray(LongPixelPacket *,dyn_process_size,
                                    sizeof(LongPixelPacket));
  if (dyn_process == (LongPixelPacket *) NULL)
    {
      DestroyImage(threshold_image);
      ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
                           UnableToThresholdImage);
    }
  (void) memset(dyn_process,0,dyn_process_size*sizeof(LongPixelPacket));

  overflow_eminent = MagickFalse;

  for (y = 0; y < (image->rows + height/2 + height + 1); y++)
    {
      PixelPacket
        *restrict q = ((PixelPacket *) NULL);

      /*
       * for each window height + 2 rows, redefine reading area for
       * preprocess and avoid sum overflow
       */
      if (PRE(0, y) == 0)
        {
          p = AcquireImagePixels(image, -(long) width, (long) y - (long) height,
                                 image->columns + (width << 1), height + 2,
                                 exception);

          if (p == (const PixelPacket *) NULL)
            {
              status = MagickFail;
              break;
            }

          /*
           * this is the code for sum overflow avoidance in
           * preprocessing it's only used for really large images.
           * and it can be highly optimized.  I couldn't properly
           * test it this code...
           */
          if (overflow_eminent)
            {
              LongPixelPacket
                min_sum;

              if (image->logging)
                (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                      "LAT: overflow handling activated "
                                      "(y=%lu)!",y);
              min_sum.red = dyn_process[0].red;
              min_sum.green = dyn_process[0].green;
              min_sum.blue = dyn_process[0].blue;
              min_sum.opacity = dyn_process[0].opacity;

              for (i = 0; i < dyn_process_size; i++)
                {
                  dyn_process[i].red -= min_sum.red;
                  dyn_process[i].green -= min_sum.green;
                  dyn_process[i].blue -= min_sum.blue;
                  dyn_process[i].opacity -= min_sum.opacity;
                }
              overflow_eminent = MagickFalse;
            }
        } /* if (PRE(0, y) == 0) */

      /* load line for writing */
      if (y > (height/2 + height))
        {
          q = GetImagePixelsEx(threshold_image, 0, y - height/2 - height - 1,
                               threshold_image->columns, 1, exception);

          if (q == (PixelPacket *) NULL)
            {
              status = MagickFail;
              break;
            }
        }

      for (x = 2; x < (image->columns + (width << 1)); x++)
        {
          LongPixelPacket * restrict current_pre;

          /* preprocess (x,y) */
          current_pre = &dyn_process[PRE(x, y)];

          /* red / gray */
          current_pre->red =
            ScaleQuantumToMap(p[PRE(x, y)].red) +
            dyn_process[PRE(x, y - 1)].red +
            dyn_process[PRE(x - 1, y)].red -
            dyn_process[PRE(x - 1, y - 1)].red;
          overflow_eminent |= (current_pre->red & overflow_mask);

          if (!is_grayscale)
            {
              /* green */
              current_pre->green =
                ScaleQuantumToMap(p[PRE(x, y)].green) +
                dyn_process[PRE(x, y - 1)].green +
                dyn_process[PRE(x - 1, y)].green -
                dyn_process[PRE(x - 1, y - 1)].green;
              overflow_eminent |= (current_pre->green & overflow_mask);

              /* blue */
              current_pre->blue =
                ScaleQuantumToMap(p[PRE(x, y)].blue) +
                dyn_process[PRE(x, y - 1)].blue +
                dyn_process[PRE(x - 1, y)].blue -
                dyn_process[PRE(x - 1, y - 1)].blue;
              overflow_eminent |= (current_pre->blue & overflow_mask);
            }
          if (matte)
            {
              /* opacity */
              current_pre->opacity =
                ScaleQuantumToMap(p[PRE(x, y)].opacity) +
                dyn_process[PRE(x, y - 1)].opacity +
                dyn_process[PRE(x - 1, y)].opacity -
                dyn_process[PRE(x - 1, y - 1)].opacity;
              overflow_eminent |= (current_pre->opacity & overflow_mask);
            }
          /* END preprocess for (x,y) */

          /*
           * start computing threshold mean, with the
           * pre-computed data and only for pixels inside valid
           * domain
           */
          if ((y > (height/2 + height)) && (x >= width) &&
              (x < (image->columns + width)))
            {
              /* Left, Right, Upper, Bottom coord. to calculate the
                 window's sum */
              long
                L,
                R,
                U,
                B;

              LongPixelPacket
                long_sum;

              L = x - width/2 - (width & 1);	/* if is odd, subtract 1... */
              R = x + width/2;
              U = y - height - 1;
              B = y - 1;

              long_sum = long_zero;

              if (L >= 0)
                {
                  long_sum.red += dyn_process[PRE(L, U)].red;
                  long_sum.red -= dyn_process[PRE(L, B)].red;
                }

              long_sum.red += dyn_process[PRE(R, B)].red;
              long_sum.red -= dyn_process[PRE(R, U)].red;
              if (!is_grayscale)
                {
                  if (L >= 0)
                    {
                      long_sum.green += dyn_process[PRE(L, U)].green;
                      long_sum.green -= dyn_process[PRE(L, B)].green;
                      long_sum.blue += dyn_process[PRE(L, U)].blue;
                      long_sum.blue -= dyn_process[PRE(L, B)].blue;
                    }

                  long_sum.green += dyn_process[PRE(R, B)].green;
                  long_sum.green -= dyn_process[PRE(R, U)].green;
                  long_sum.blue += dyn_process[PRE(R, B)].blue;
                  long_sum.blue -= dyn_process[PRE(R, U)].blue;
                }
              if (matte)
                {
                  if (L >= 0)
                    {
                      long_sum.opacity += dyn_process[PRE(L, U)].opacity;
                      long_sum.opacity -= dyn_process[PRE(L, B)].opacity;
                    }

                  long_sum.opacity += dyn_process[PRE(R, B)].opacity;
                  long_sum.opacity -= dyn_process[PRE(R, U)].opacity;
                }

              /*
               * Avoid overflow at mean.  We were able to do
               * this by using some bitwise operations but there
               * was no speedup and the code get pretty hard to
               * read...
               */
              if ((long) (long_sum.red / local_area) + long_offset > (long) MaxMap)
                long_sum.red = MaxMap;
              else if ((long) (long_sum.red / local_area) + long_offset < (long) 0L)
                long_sum.red = 0UL;
              else
                long_sum.red = ((long) (long_sum.red / local_area)) + long_offset;

              /* grayscale and red */
              q[x - width].red = (ScaleQuantumToMap(q[x - width].red) <= long_sum.red ? 0U : MaxRGB);

              if (!is_grayscale)
                {
                  if ((long) (long_sum.green / local_area) + long_offset > (long) MaxMap)
                    long_sum.green = MaxMap;
                  else if ((long) (long_sum.green / local_area) + long_offset < (long) 0L)
                    long_sum.green = 0UL;
                  else
                    long_sum.green = ((long) (long_sum.green / local_area)) + long_offset;

                  if ((long) (long_sum.blue / local_area) + long_offset > (long) MaxMap)
                    long_sum.blue = MaxMap;
                  else if ((long) (long_sum.blue / local_area) + long_offset < (long) 0L)
                    long_sum.blue = 0UL;
                  else
                    long_sum.blue = ((long) (long_sum.blue / local_area)) + long_offset;

                  q[x - width].green = (ScaleQuantumToMap(q[x - width].green) <= long_sum.green ? 0U : MaxRGB);
                  q[x - width].blue = (ScaleQuantumToMap(q[x - width].blue) <= long_sum.blue ? 0U : MaxRGB);

                }
              if (matte)
                {
                  if ((long) (long_sum.opacity / local_area) + long_offset > (long) MaxMap)
                    long_sum.opacity = MaxMap;
                  else if ((long) (long_sum.opacity / local_area) + long_offset < (long) 0)
                    long_sum.opacity = 0UL;
                  else
                    long_sum.opacity = (long_sum.opacity / local_area) + long_offset;

                  q[x - width].opacity =
                    (ScaleQuantumToMap(q[x - width].opacity) <= long_sum.opacity ? 0U : MaxRGB);
                }

              if (is_grayscale)
                q[x - width].green = q[x - width].blue = q[x - width].red;
            } /* if (y ... */
        } /* for (x ... */
      if (q != (const PixelPacket *) NULL)
        {
          if (!SyncImagePixelsEx(threshold_image, exception))
            {
              status = MagickFail;
              break;
            }
        }
      row_count++;
      if (QuantumTick(row_count, image->rows))
        if (!MagickMonitorFormatted(row_count, image->rows, exception,
                                    AdaptiveThresholdImageText, image->filename))
          {
            status = MagickFail;
            break;
          }
    } /* for (y ... */

  MagickFreeMemory(dyn_process);

  if (MagickFail == status)
    {
      DestroyImage(threshold_image);
      threshold_image = (Image *) NULL;
    }
  else if (is_grayscale)
    {
      threshold_image->is_monochrome = MagickTrue;
      threshold_image->is_grayscale = MagickTrue;
    }
  return (threshold_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A d d N o i s e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AddNoiseImage() adds random noise to the image.
%
%  The format of the AddNoiseImage method is:
%
%      Image *AddNoiseImage(const Image *image,const NoiseType noise_type,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o noise_type:  The type of noise: Uniform, Gaussian, Multiplicative,
%      Impulse, Laplacian, Poisson, or Random.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *
AddNoiseImage(const Image *image,const NoiseType noise_type,
              ExceptionInfo *exception)
{
  return AddNoiseImageChannel(image,AllChannels,noise_type,exception);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     A d d N o i s e I m a g e C h a n n e l                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  AddNoiseImageChannel() adds random noise to one image channel.
%
%  The format of the AddNoiseImageChannel method is:
%
%      Image *AddNoiseImageChannel(const Image *image,
%         const ChannelType channel, const NoiseType noise_type,
%         ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: The image channel to apply noise to.
%
%    o noise_type:  The type of noise: Uniform, Gaussian, Multiplicative,
%      Impulse, Laplacian, Poisson, or Random.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *
AddNoiseImageChannel(const Image *image,const ChannelType channel,
                     const NoiseType noise_type,
                     ExceptionInfo *exception)
{
  QuantumOperator
    quantum_operator;
  
  Image
    *noise_image;

  noise_image=CloneImage(image,0,0,True,exception);
  if (noise_image == (Image *) NULL)
    return((Image *) NULL);

  switch (noise_type)
    {
    case GaussianNoise:
      quantum_operator=NoiseGaussianQuantumOp;
      break;
    case ImpulseNoise:
      quantum_operator=NoiseImpulseQuantumOp;
      break;
    case LaplacianNoise:
      quantum_operator=NoiseLaplacianQuantumOp;
      break;
    case MultiplicativeGaussianNoise:
      quantum_operator=NoiseMultiplicativeQuantumOp;
      break;
    case PoissonNoise:
      quantum_operator=NoisePoissonQuantumOp;
      break;
    case RandomNoise:
      quantum_operator=NoiseRandomQuantumOp;
      break;
    case UniformNoise:
      quantum_operator=NoiseUniformQuantumOp;
      break;
    default:
      quantum_operator=UndefinedQuantumOp;
      break;
    }

  (void) QuantumOperatorImage(noise_image,channel,quantum_operator,
                              MaxRGBDouble,exception);
  return noise_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     B l a c k T h r e s h o l d I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BlackThresholdImage() adjusts the levels of image channels such that
%  values below a specified threshold are set to the minimum value (black)
%  while the remaining pixels are unchanged.
%
%  The format of the BlackThresholdImage method is:
%
%      MagickPassFail BlackThresholdImage(Image *image,const char *thresholds)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o thresholds: Channel thresholds which are specified as a comma delimited
%      list containing the thresholds for red, green, blue, and opacity.  If
%      the list contains a percent symbol (%) then all values are treated as
%      a percentage of MaxRGB.
%
*/
MagickExport MagickPassFail BlackThresholdImage(Image *image,const char *thresholds)
{
  return QuantumOperatorImageMultivalue(image,ThresholdBlackQuantumOp,thresholds);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     B l u r I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BlurImage() blurs an image.  We convolve the image with a Gaussian
%  operator of the given radius and standard deviation (sigma).
%  For reasonable results, the radius should be larger than sigma.  Use a
%  radius of 0 and BlurImage() selects a suitable radius for you.
%
%  The format of the BlurImage method is:
%
%      Image *BlurImage(const Image *image,const double radius,
%        const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define BlurImageColumnsText "[%s] Blur columns: order %lu..."
#define BlurImageRowsText "[%s] Blur rows: order %lu...  "
static void
BlurScanline(const double *kernel,const unsigned long width,
             const PixelPacket *source,PixelPacket *destination,
             const unsigned long columns, const MagickBool matte)
{
  double
    scale;

  const DoublePixelPacket
    zero = { 0.0, 0.0, 0.0, 0.0 };

  DoublePixelPacket
    aggregate;

  register const double
    *p;

  register const PixelPacket
    *q;

  register long
    i,
    x;

  if (width > columns)
    {
      for (x=0; x < (long) columns; x++)
      {
        aggregate=zero;
        scale=0.0;
        p=kernel;
        q=source;
        for (i=0; i < (long) columns; i++)
        {
          if ((i >= (x-(long) width/2)) && (i <= (x+(long) width/2)))
            {
              /* This region is the big CPU burner for the whole function */
              aggregate.red+=(*p)*q->red;
              aggregate.green+=(*p)*q->green;
              aggregate.blue+=(*p)*q->blue;
              if (matte)
                aggregate.opacity+=(*p)*q->opacity;
            }
          if (((i+(long)(width/2)-x) >= 0) && ((i+width/2-x) < width))
            scale+=kernel[i+width/2-x];
          p++;
          q++;
        }
        scale=1.0/scale;
        destination[x].red=(Quantum) (scale*(aggregate.red+0.5));
        destination[x].green=(Quantum) (scale*(aggregate.green+0.5));
        destination[x].blue=(Quantum) (scale*(aggregate.blue+0.5));
        if (matte)
          destination[x].opacity=(Quantum) (scale*(aggregate.opacity+0.5));
      }
      return;
    }
  /*
    Blur scanline.
  */
  for (x=0; x < (long) (width/2); x++)
  {
    aggregate=zero;
    scale=0.0;
    p=kernel+width/2-x;
    q=source;
    for (i=width/2-x; i < (long) width; i++)
    {
      aggregate.red+=(*p)*q->red;
      aggregate.green+=(*p)*q->green;
      aggregate.blue+=(*p)*q->blue;
      if (matte)
        aggregate.opacity+=(*p)*q->opacity;
      scale+=(*p);
      p++;
      q++;
    }
    scale=1.0/scale;
    destination[x].red=(Quantum) (scale*(aggregate.red+0.5));
    destination[x].green=(Quantum) (scale*(aggregate.green+0.5));
    destination[x].blue=(Quantum) (scale*(aggregate.blue+0.5));
    if (matte)
      destination[x].opacity=(Quantum) (scale*(aggregate.opacity+0.5));
  }
  for ( ; x < (long) (columns-width/2); x++)
  {
    aggregate=zero;
    p=kernel;
    q=source+(x-width/2);
    for (i=0; i < (long) width; i++)
    {
      aggregate.red+=(*p)*q->red;
      aggregate.green+=(*p)*q->green;
      aggregate.blue+=(*p)*q->blue;
      if (matte)
        aggregate.opacity+=(*p)*q->opacity;
      p++;
      q++;
    }
    destination[x].red=(Quantum) (aggregate.red+0.5);
    destination[x].green=(Quantum) (aggregate.green+0.5);
    destination[x].blue=(Quantum) (aggregate.blue+0.5);
    if (matte)
      destination[x].opacity=(Quantum) (aggregate.opacity+0.5);
  }
  for ( ; x < (long) columns; x++)
  {
    aggregate=zero;
    scale=0;
    p=kernel;
    q=source+(x-width/2);
    for (i=0; i < (long) (columns-x+width/2); i++)
    {
      aggregate.red+=(*p)*q->red;
      aggregate.green+=(*p)*q->green;
      aggregate.blue+=(*p)*q->blue;
      if (matte)
        aggregate.opacity+=(*p)*q->opacity;
      scale+=(*p);
      p++;
      q++;
    }
    scale=1.0/scale;
    destination[x].red=(Quantum) (scale*(aggregate.red+0.5));
    destination[x].green=(Quantum) (scale*(aggregate.green+0.5));
    destination[x].blue=(Quantum) (scale*(aggregate.blue+0.5));
    if (matte)
      destination[x].opacity=(Quantum) (scale*(aggregate.opacity+0.5));
  }
}

static int GetBlurKernel(unsigned long width,const double sigma,double **kernel)
{
#define KernelRank 3

  double
    alpha,
    normalize;

  int
    bias;

  register long
    i;

  /*
    Generate a 1-D convolution matrix.  Calculate the kernel at higher
    resolution than needed and average the results as a form of numerical
    integration to get the best accuracy.
  */
  if (width == 0)
    width=3;
  *kernel=MagickAllocateMemory(double *,width*sizeof(double));
  if (*kernel == (double *) NULL)
    return(0);
  for (i=0; i < (long) width; i++)
    (*kernel)[i]=0.0;
  bias=KernelRank*width/2;
  for (i=(-bias); i <= bias; i++)
  {
    alpha=exp(-((double) i*i)/(2.0*KernelRank*KernelRank*sigma*sigma));
    (*kernel)[(i+bias)/KernelRank]+=alpha/(MagickSQ2PI*sigma);
  }
  normalize=0;
  for (i=0; i < (long) width; i++)
    normalize+=(*kernel)[i];
  for (i=0; i < (long) width; i++)
    (*kernel)[i]/=normalize;
  return(width);
}

static MagickPassFail BlurImageScanlines(Image *image,const double *kernel,
                                         const unsigned long width,
                                         const char *format,
                                         ExceptionInfo *exception)
{
  ThreadViewDataSet
    *data_set;

  MagickBool
    is_grayscale;

  MagickPassFail
    status=MagickPass;

  const MagickBool
    matte=((image->matte) || (image->colorspace == CMYKColorspace));

  is_grayscale=image->is_grayscale;

  data_set=AllocateThreadViewDataArray(image,exception,image->columns,sizeof(PixelPacket));
  if (data_set == (ThreadViewDataSet *) NULL)
    status=MagickFail;

  if (status != MagickFail)
    {
      unsigned long
        row_count=0;

      long
        y;

#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for schedule(static,4) shared(row_count, status)
#  endif
#endif
      for (y=0; y < (long) image->rows; y++)
        {
          register PixelPacket
            *q;

          PixelPacket
            *scanline;

          MagickBool
            thread_status;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_BlurImageScanlines)
#endif
          thread_status=status;
          if (thread_status == MagickFail)
            continue;
  
          scanline=AccessThreadViewData(data_set);
          q=GetImagePixelsEx(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            thread_status=MagickFail;

          if (thread_status != MagickFail)
            {
              unsigned long
                i=0;

              scanline[i]=q[i];
              i++;
              for ( ; i < image->columns; i++)
                {
		  if (NotPixelMatch(&scanline[i-1],&q[i],matte))
                    break;
                  scanline[i]=q[i];
                }
              if (i != image->columns)
                {
                  (void) memcpy(&scanline[i],&q[i],
				(image->columns-i)*sizeof(PixelPacket));
                  BlurScanline(kernel,width,scanline,q,image->columns,matte);
                  if (!SyncImagePixelsEx(image,exception))
                    thread_status=MagickFail;
                }
            }
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_BlurImageScanlines)
#endif
          {
            row_count++;
            if (QuantumTick(row_count,image->rows))
              if (!MagickMonitorFormatted(row_count,image->rows,exception,
                                          format,image->filename,width))
                thread_status=MagickFail;
          
            if (thread_status == MagickFail)
              status=MagickFail;
          }
        }
    }

  DestroyThreadViewDataSet(data_set);
  image->is_grayscale=is_grayscale;

  return status;
}

MagickExport Image *
BlurImage(const Image *original_image,const double radius,
          const double sigma,ExceptionInfo *exception)
{

  double
    *kernel;

  Image
    *blur_image;

  int
    width;

  MagickPassFail
    status=MagickPass;

  /*
    Get convolution matrix for the specified standard-deviation.
  */
  assert(original_image != (Image *) NULL);
  assert(original_image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  kernel=(double *) NULL;
  if (radius > 0)
    width=GetBlurKernel((int) (2*ceil(radius)+1),sigma,&kernel);
  else
    {
      double
        *last_kernel;

      last_kernel=(double *) NULL;
      width=GetBlurKernel(3,sigma,&kernel);
      while ((long) (MaxRGB*kernel[0]) > 0)
        {
          if (last_kernel != (double *)NULL)
            MagickFreeMemory(last_kernel);
          last_kernel=kernel;
          kernel=(double *) NULL;
          width=GetBlurKernel(width+2,sigma,&kernel);
        }
      if (last_kernel != (double *) NULL)
        {
          MagickFreeMemory(kernel);
          width-=2;
          kernel=last_kernel;
        }
    }
  if (width < 3)
    ThrowImageException3(OptionError,UnableToBlurImage,
                         KernelRadiusIsTooSmall);

  blur_image=RotateImage(original_image,90,exception);
  if (blur_image == (Image *) NULL)
    status=MagickFail;
  blur_image->storage_class=DirectClass;

  if (status != MagickFail)
    status&=BlurImageScanlines(blur_image,kernel,width,BlurImageColumnsText,
			       exception);
  
  if (status != MagickFail)
  {
    Image
      *rotate_image;

    rotate_image=RotateImage(blur_image,-90,exception);
    if (rotate_image == (Image *) NULL)
      status=MagickFail;

    if (status != MagickFail)
      {
        DestroyImage(blur_image);
        blur_image=rotate_image;
      }
  }

  if (status != MagickFail)
    status&=BlurImageScanlines(blur_image,kernel,width,BlurImageRowsText,
			       exception);

  MagickFreeMemory(kernel);
  blur_image->is_grayscale=original_image->is_grayscale;
  return(blur_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     B l u r I m a g e C h a n n e l                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  BlurImageChannel() blurs the specified image channel.  We convolve the
%  image channel with a Gaussian operator of the given radius and standard
%  deviation (sigma).  For reasonable results, the radius should be larger
%  than sigma.  Use a radius of 0 and BlurImageChannel() selects a suitable
%  radius for you.
%
%  The format of the BlurImageChannel method is:
%
%      Image *BlurImageChannel(const Image *image,const ChannelType channel,
%        const double radius,const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o channel: The channel to blur.
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *
BlurImageChannel(const Image *image,const ChannelType channel,
                 const double radius,const double sigma,
                 ExceptionInfo *exception)
{
  Image
    *blur_image;

  blur_image=BlurImage(image,radius,sigma,exception);
  if (blur_image != (Image *) NULL)
    (void) ImportImageChannelsMasked(image,blur_image,channel);

  return blur_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C h a n n e l T h r e s h o l d I m a g e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ChannelThresholdImage() changes the value of individual pixels based on
%  the level of each pixel channel.  The result sets the affected channels
%  to the minimum or maximum channel value. A negative threshold value
%  disables thresholding for that channel.  Append a percent symbol to
%  have threshold values automatically scaled from a percentage to MaxRGB.
%
%  Invoked by the '-threshold' option.
%
%  The format of the ChannelThresholdImage method is:
%
%      MagickPassFail ChannelThresholdImage(Image *image,const char *threshold)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o threshold: define the threshold values, <red>{<green>,<blue>,<opacity>}{%}.
%
%
*/
typedef struct _ChannelThresholdOptions_t
{
  PixelPacket
    thresholds;

  MagickBool
    red_enabled,
    green_enabled,
    blue_enabled,
    opacity_enabled;
} ChannelThresholdOptions_t;

static MagickPassFail
ChannelThresholdPixels(void *mutable_data,         /* User provided mutable data */
                       const void *immutable_data, /* User provided immutable data */
                       Image *image,               /* Modify image */
                       PixelPacket *pixels,        /* Pixel row */
                       IndexPacket *indexes,       /* Pixel row indexes */
                       const long npixels,         /* Number of pixels in row */
                       ExceptionInfo *exception)   /* Exception report */
{
  /*
    Threshold channels
  */
  const ChannelThresholdOptions_t
    *options = (const ChannelThresholdOptions_t *) immutable_data;

  register long
    i;

  const PixelPacket
    *thresholds=&options->thresholds;

  const MagickBool
    red_enabled=options->red_enabled,
    green_enabled=options->green_enabled,
    blue_enabled=options->blue_enabled,
    opacity_enabled=options->opacity_enabled;
  
  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(image);
  ARG_NOT_USED(indexes);
  ARG_NOT_USED(exception);

  for (i=0; i < npixels; i++)
    {
      if (red_enabled)
        pixels[i].red=(pixels[i].red <= thresholds->red ? 0U : MaxRGB);
      if (green_enabled)
        pixels[i].green=(pixels[i].green <= thresholds->green ? 0U : MaxRGB);
      if (blue_enabled)
        pixels[i].blue=(pixels[i].blue <= thresholds->blue ? 0U : MaxRGB);
      if (opacity_enabled)
        pixels[i].opacity=(pixels[i].opacity <= thresholds->opacity ? 0U : MaxRGB);
    }
  return MagickPass;
}

#define ChannelThresholdImageText "[%s] Channel threshold..."
MagickExport MagickPassFail ChannelThresholdImage(Image *image,
  const char *threshold)
{
  DoublePixelPacket
    double_threshold;

  ChannelThresholdOptions_t
    options;

  int
    count;

  unsigned int
    is_grayscale=image->is_grayscale;

  MagickPassFail
    status = MagickPass;

  /*
    Threshold image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  
  if (threshold == (const char *) NULL)
    return(MagickFail);

  options.thresholds.red     = 0U;
  options.thresholds.green   = 0U;
  options.thresholds.blue    = 0U;
  options.thresholds.opacity = 0U;

  options.red_enabled        = MagickFalse;
  options.green_enabled      = MagickFalse;
  options.blue_enabled       = MagickFalse;
  options.opacity_enabled    = MagickFalse;

  double_threshold.red       = -1.0;
  double_threshold.green     = -1.0;
  double_threshold.blue      = -1.0;
  double_threshold.opacity   = -1.0;
  count=sscanf(threshold,"%lf%*[/,%%]%lf%*[/,%%]%lf%*[/,%%]%lf",
               &double_threshold.red,
               &double_threshold.green,
               &double_threshold.blue,
               &double_threshold.opacity);
  
  if ((count > 3) && (double_threshold.opacity >= 0.0))
    options.opacity_enabled = MagickTrue;
  if ((count > 2) && (double_threshold.blue >= 0.0))
    options.blue_enabled = MagickTrue;
  if ((count > 1) && (double_threshold.green >= 0.0))
    options.green_enabled = MagickTrue;
  if ((count > 0) && (double_threshold.red >= 0.0))
    options.red_enabled = MagickTrue;
  
  if (strchr(threshold,'%') != (char *) NULL)
    {
      if (options.red_enabled)
        double_threshold.red     *= MaxRGB/100.0;
      if (options.green_enabled)
        double_threshold.green   *= MaxRGB/100.0;
      if (options.blue_enabled)
        double_threshold.blue    *= MaxRGB/100.0;
      if (options.opacity_enabled)
        double_threshold.opacity *= MaxRGB/100.0;
    }

  if (options.red_enabled)
    options.thresholds.red     = RoundDoubleToQuantum(double_threshold.red);
  if (options.green_enabled)
    options.thresholds.green   = RoundDoubleToQuantum(double_threshold.green);
  if (options.blue_enabled)
    options.thresholds.blue    = RoundDoubleToQuantum(double_threshold.blue);
  if (options.opacity_enabled)
    options.thresholds.opacity = RoundDoubleToQuantum(double_threshold.opacity);

  (void) SetImageType(image,TrueColorType);

  status=PixelIterateMonoModify(ChannelThresholdPixels,
                                NULL,
                                ChannelThresholdImageText,
                                NULL,&options,
                                0,0,image->columns,image->rows,
                                image,
                                &image->exception);

  if (is_grayscale && options.red_enabled && options.green_enabled && options.blue_enabled)
    {
      image->is_monochrome=True;
      image->is_grayscale=True;
    }
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     C o n v o l v e I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ConvolveImage() applies a custom convolution kernel to the image.
%
%  The format of the ConvolveImage method is:
%
%      Image *ConvolveImage(const Image *image,const unsigned int order,
%        const double *kernel,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o order: The number of columns and rows in the filter kernel.
%
%    o kernel: An array of double representing the convolution kernel.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define ConvolveImageText "[%s] Convolve: order %u..."
MagickExport Image *ConvolveImage(const Image *image,const unsigned int order,
                                  const double *kernel,ExceptionInfo *exception)
{
#if QuantumDepth < 32
  typedef float float_quantum_t;
  typedef FloatPixelPacket float_packet_t;
#  define RoundFloatQuantumToIntQuantum(value) RoundFloatToQuantum(value)
#else
  typedef double float_quantum_t;
  typedef DoublePixelPacket float_packet_t;
#  define RoundFloatQuantumToIntQuantum(value) RoundDoubleToQuantum(value)
#endif

  float_quantum_t
    * restrict normal_kernel;

  Image
    *convolve_image;

  long
    width,
    y;

  MagickPassFail
    status;

  const MagickBool
    is_grayscale = image->is_grayscale;

  const MagickBool
    matte=((image->matte) || (image->colorspace == CMYKColorspace));

  /*
    Initialize convolve image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=(long) order;
  if ((width % 2) == 0)
    ThrowImageException3(OptionError,UnableToConvolveImage,
                         KernelWidthMustBeAnOddNumber);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException3(OptionError,UnableToConvolveImage,
                         ImageSmallerThanKernelWidth);
  convolve_image=CloneImage(image,image->columns,image->rows,MagickTrue,exception);
  if (convolve_image == (Image *) NULL)
    return((Image *) NULL);
  convolve_image->storage_class=DirectClass;
  {
    /*
      Build normalized kernel.

      0x0.25 --> 3x3
      0x0.5  --> 5x5
      0x1    --> 9x9
      0x2    --> 17x17
      0x3    --> 25x25
      0x4    --> 33x33
      0x5    --> 41x41
      0x6    --> 49x49
    */
    double
      normalize;
    
    register long
      i;

    normal_kernel=MagickAllocateAlignedMemory(float_quantum_t *,
                                              MAGICK_CACHE_LINE_SIZE,
                                              width*width*sizeof(float_quantum_t));
    if (normal_kernel == (float_quantum_t *) NULL)
      {
        DestroyImage(convolve_image);
        ThrowImageException(ResourceLimitError,MemoryAllocationFailed,
                            MagickMsg(OptionError,UnableToConvolveImage));
      }
    normalize=0.0;
    for (i=0; i < (width*width); i++)
      normalize+=kernel[i];
    if (AbsoluteValue(normalize) <= MagickEpsilon)
      normalize=1.0;
    normalize=1.0/normalize;
    for (i=0; i < (width*width); i++)
      {
        normal_kernel[i]=normalize*kernel[i];
      }
  }
  
  if (LogMagickEvent(TransformEvent,GetMagickModule(),
                     "  ConvolveImage with %ldx%ld kernel:",width,width))
    {
      /*
        Log convolution matrix.
      */
      char
        cell_text[MaxTextExtent],
        row_text[MaxTextExtent];

      const double
        *k;

      long
        u,
        v;

      k=kernel;
      for (v=0; v < width; v++)
        {
          *row_text='\0';
          for (u=0; u < width; u++)
            {
              FormatString(cell_text,"%#12.4g",*k++);
              (void) strlcat(row_text,cell_text,sizeof(row_text));
              if (u%5 == 4)
                {
                  (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                        "   %.64s", row_text);
                  *row_text='\0';
                }
            }
          if (u > 5)
            (void) strlcat(row_text,"\n",sizeof(row_text));
          if (row_text[0] != '\0')
            (void) LogMagickEvent(TransformEvent,GetMagickModule(),
                                  "   %s", row_text);
        }
    }

  status=MagickPass;
  /*
    Convolve image.
  */
  {
    unsigned long
      row_count=0;

    float_packet_t
      zero;

    (void) memset(&zero,0,sizeof(float_packet_t));
#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    if defined(USE_STATIC_SCHEDULING_ONLY)
#      pragma omp parallel for schedule(static,4) shared(row_count, status)
#    else
#      pragma omp parallel for schedule(guided) shared(row_count, status)
#    endif
#  endif
#endif
    for (y=0; y < (long) convolve_image->rows; y++)
      {
        const PixelPacket
          * restrict p;
    
        PixelPacket
          * restrict q;

        long
          x;

        MagickBool
          thread_status;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_ConvolveImage)
#endif
        thread_status=status;
        if (thread_status == MagickFail)
          continue;

	/*
	  Acquire rectangle of columns+width high, and width tall.
	*/
        p=AcquireImagePixels(image,-width/2,y-width/2,image->columns+width,width,
                             exception);
	/*
	  Set one row.
	*/
        q=SetImagePixelsEx(convolve_image,0,y,convolve_image->columns,1,exception);
        if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
          thread_status=MagickFail;

        if (thread_status != MagickFail)
          {
            for (x=0; x < (long) convolve_image->columns; x++)
              {
                float_packet_t
                  pixel;


                const PixelPacket
                  * restrict r;

                long
                  u,
                  v;

                const float_quantum_t
                  * restrict k;

                r=p;
                pixel=zero;
                k=normal_kernel;
		if (is_grayscale && !matte)
		  {
		    /* G */
		    for (v=0; v < width; v++)
		      {
			for (u=0; u < width; u++)
			  pixel.red+=k[u]*r[u].red;
			k+= width;
			r+=image->columns+width;
		      }
		    q->red=q->green=q->blue=RoundFloatQuantumToIntQuantum(pixel.red);
		    q->opacity=OpaqueOpacity;
		  }
		else if (!matte)
		  {
		    /* RGB */
		    for (v=0; v < width; v++)
		      {
			for (u=0; u < width; u++)
			  {
			    pixel.red+=k[u]*r[u].red;
			    pixel.green+=k[u]*r[u].green;
			    pixel.blue+=k[u]*r[u].blue;
			  }
			k+=width;
			r+=image->columns+width;
		      }
		    q->red=RoundFloatQuantumToIntQuantum(pixel.red);
		    q->green=RoundFloatQuantumToIntQuantum(pixel.green);
		    q->blue=RoundFloatQuantumToIntQuantum(pixel.blue);
		    q->opacity=OpaqueOpacity;
		  }
		else
		  {
		    /* RGBA */
		    for (v=0; v < width; v++)
		      {
			for (u=0; u < width; u++)
			  {
			    pixel.red+=k[u]*r[u].red;
			    pixel.green+=k[u]*r[u].green;
			    pixel.blue+=k[u]*r[u].blue;
			    pixel.opacity+=k[u]*r[u].opacity;
			  }
			k+=width;
			r+=image->columns+width;
		      }
		    q->red=RoundFloatQuantumToIntQuantum(pixel.red);
		    q->green=RoundFloatQuantumToIntQuantum(pixel.green);
		    q->blue=RoundFloatQuantumToIntQuantum(pixel.blue);
		    q->opacity=RoundFloatQuantumToIntQuantum(pixel.opacity);
		  }
                p++;
                q++;
              }
            if (!SyncImagePixelsEx(convolve_image,exception))
              thread_status=MagickFail;
          }
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_ConvolveImage)
#endif
        {
          row_count++;
          if (QuantumTick(row_count,image->rows))
            if (!MagickMonitorFormatted(row_count,image->rows,exception,
                                        ConvolveImageText,
                                        convolve_image->filename,
					order))
              thread_status=MagickFail;
          
          if (thread_status == MagickFail)
            status=MagickFail;
        }
      }
  }
  MagickFreeAlignedMemory(normal_kernel);
  if (MagickFail == status)
    {
      DestroyImage(convolve_image);
      convolve_image=(Image *) NULL;
    }
  else
    {
      convolve_image->is_grayscale=is_grayscale;
    }
  return(convolve_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     D e s p e c k l e I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Despeckle() reduces the speckle noise in an image while perserving the
%  edges of the original image.
%
%  The format of the DespeckleImage method is:
%
%      Image *DespeckleImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *DespeckleImage(const Image *image,ExceptionInfo *exception)
{
#define DespeckleImageText "[%s] Despeckle..."

  Image
    *despeckle_image;

  Quantum
    *buffer = (Quantum *) NULL,
    *pixels  = (Quantum *) NULL;

  unsigned long
    progress=0UL,
    progress_span;

  int
    layer,
    min_layer,
    max_layer;
  
  size_t
    length;

  ImageCharacteristics
    characteristics;

  static const int
    X[4]=
    {
      0, 1, 1,-1
    };

  static const int
    Y[4]=
    {
      1, 0, 1, 1
    };

  MagickPassFail
    status = MagickPass;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  /*
    Analyze image.
  */
  if (!GetImageCharacteristics(image,&characteristics,MagickFalse,exception))
    return ((Image *) NULL);

  if (characteristics.opaque)
    min_layer=1;
  else
    min_layer=0;
  if (characteristics.grayscale)
    max_layer=2;
  else
    max_layer=4;

  progress_span=4*(max_layer-min_layer);

  /*
    Compute buffer size
  */
  length=(image->columns+2)*(image->rows+2);

  /*
    Allocate planar working buffers
  */
  pixels=MagickAllocateArray(Quantum *,length,sizeof(*pixels));
  if (pixels == (Quantum *) NULL)
    {
      ThrowException3(exception,ResourceLimitError,MemoryAllocationFailed,
		      UnableToDespeckleImage);
      return ((Image *) NULL);
    }
  buffer=MagickAllocateArray(Quantum *,length,sizeof(*buffer));
  if (buffer == (Quantum *) NULL)
    {
      MagickFreeMemory(pixels);
      ThrowException3(exception,ResourceLimitError,MemoryAllocationFailed,
		      UnableToDespeckleImage);
      return ((Image *) NULL);
    }

  /*
    Allocate despeckled image.
  */
  despeckle_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (despeckle_image == (Image *) NULL)
    {
      MagickFreeMemory(pixels);
      MagickFreeMemory(buffer);
      return ((Image *) NULL);
    }
  despeckle_image->storage_class=DirectClass;

  /*
    Reduce speckle in the image.
  */
  for (layer=min_layer; layer < max_layer; layer++)
    {
      long
        j,
        y;

      register const PixelPacket
        *p;
  
      register long
        i,
        x;
  
      register PixelPacket
        *q;

      MagickPassFail
	status=MagickPass;

      /*
	Export image channel
      */
      (void) memset(pixels,0,length*sizeof(Quantum));
      j=(long) image->columns+2;
      for (y=0; y < (long) image->rows; y++)
	{
	  p=AcquireImagePixels(image,0,y,image->columns,1,exception);
	  if (p == (const PixelPacket *) NULL)
	    {
	      status=MagickFail;
	      break;
	    }
	  j++;

	  switch (layer)
	    {
	    case 0:
	      for (x=(long) image->columns; x > 0; x--)
		pixels[j++]=p++->opacity;
	      break;
	    case 1:
	      for (x=(long) image->columns; x > 0; x--)
		pixels[j++]=p++->red;
	      break;
	    case 2:
	      for (x=(long) image->columns; x > 0; x--)
		pixels[j++]=p++->green;
	      break;
	    case 3:
	      for (x=(long) image->columns; x > 0; x--)
		pixels[j++]=p++->blue;
	      break;
	    default:
	      break;
	    }

	  j++;
	}
      if (status == MagickFail)
	break;

      (void) memset(buffer,0,length*sizeof(Quantum));
      for (i=0; i < 4; i++)
	{
	  progress++;
	  if (!MagickMonitorFormatted(progress,progress_span,exception,
				      DespeckleImageText,
				      despeckle_image->filename))
	    {
	      status=MagickFail;
	      break;
	    }
	  Hull(X[i],Y[i],image->columns,image->rows,pixels,buffer,1);
	  Hull(-X[i],-Y[i],image->columns,image->rows,pixels,buffer,1);
	  Hull(-X[i],-Y[i],image->columns,image->rows,pixels,buffer,-1);
	  Hull(X[i],Y[i],image->columns,image->rows,pixels,buffer,-1);
	}
      if (status == MagickFail)
	break;

      /*
	Import image channel
      */
      j=(long) image->columns+2;
      for (y=0; y < (long) image->rows; y++)
	{
	  q=SetImagePixelsEx(despeckle_image,0,y,despeckle_image->columns,1,exception);
	  if (q == (PixelPacket *) NULL)
	    {
	      status=MagickFail;
	      break;
	    }
	  j++;

	  switch (layer)
	    {
	    case 0:
	      for (x=(long) image->columns; x > 0; x--)
		q++->opacity=pixels[j++];
	      break;
	    case 1:
	      if (characteristics.grayscale)
		{
		  for (x=(long) image->columns; x > 0; x--)
		    {
		      q->red=q->green=q->blue=pixels[j++];
		      q++;
		    }
		}
	      else
		{
		  for (x=(long) image->columns; x > 0; x--)
		    q++->red=pixels[j++];
		}
	      break;
	    case 2:
	      for (x=(long) image->columns; x > 0; x--)
		q++->green=pixels[j++];
	      break;
	    case 3:
	      for (x=(long) image->columns; x > 0; x--)
		q++->blue=pixels[j++];
	      break;
	    default:
	      break;
	    }

	  if (!SyncImagePixelsEx(despeckle_image,exception))
	    {
	      status=MagickFail;
	      break;
	    }
	  j++;
	}
    }

  MagickFreeMemory(pixels);
  MagickFreeMemory(buffer);

  if (status == MagickFail)
    {
      DestroyImage(despeckle_image);
      return (Image *) NULL;
    }

  despeckle_image->is_grayscale=image->is_grayscale;
  return(despeckle_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     E d g e I m a g e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EdgeImage() finds edges in an image.  Radius defines the radius of the
%  convolution filter.  Use a radius of 0 and Edge() selects a suitable
%  radius for you.
%
%  The format of the EdgeImage method is:
%
%      Image *EdgeImage(const Image *image,const double radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: the radius of the pixel neighborhood.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *EdgeImage(const Image *image,const double radius,
  ExceptionInfo *exception)
{
  double
    *kernel;

  Image
    *edge_image;

  int
    width;

  register long
    i;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,0.5);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException3(OptionError,UnableToEdgeImage,
      ImageSmallerThanRadius);
  kernel=MagickAllocateMemory(double *,width*width*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToEdgeImage);
  for (i=0; i < (width*width); i++)
    kernel[i]=(-1.0);
  kernel[i/2]=width*width-1.0;
  edge_image=ConvolveImage(image,width,kernel,exception);
  MagickFreeMemory(kernel);
  edge_image->is_grayscale=image->is_grayscale;
  return(edge_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     E m b o s s I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EmbossImage() returns a grayscale image with a three-dimensional effect.
%  We convolve the image with a Gaussian operator of the given radius and
%  standard deviation (sigma).  For reasonable results, radius should be
%  larger than sigma.  Use a radius of 0 and Emboss() selects a suitable
%  radius for you.
%
%  The format of the EmbossImage method is:
%
%      Image *EmbossImage(const Image *image,const double radius,
%        const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: the radius of the pixel neighborhood.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *EmbossImage(const Image *image,const double radius,
  const double sigma,ExceptionInfo *exception)
{
  double
    alpha,
    *kernel;

  Image
    *emboss_image;

  int
    j,
    width;

  register long
    i,
    u,
    v;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,0.5);
  kernel=MagickAllocateMemory(double *,width*width*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToEmbossImage);
  i=0;
  j=width/2;
  for (v=(-width/2); v <= (width/2); v++)
  {
    for (u=(-width/2); u <= (width/2); u++)
    {
      alpha=exp(-((double) u*u+v*v)/(2.0*sigma*sigma));
      kernel[i]=((u < 0) || (v < 0) ? -8.0 : 8.0)*
        alpha/(2.0*MagickPI*sigma*sigma);
      if (u == j)
        kernel[i]=v == j ? 1.0 : 0.0;
      i++;
    }
    j--;
  }
  emboss_image=ConvolveImage(image,width,kernel,exception);
  if (emboss_image != (Image *) NULL)
    (void) EqualizeImage(emboss_image);
  MagickFreeMemory(kernel);
  emboss_image->is_grayscale=image->is_grayscale;
  return(emboss_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     E n h a n c e I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  EnhanceImage() applies a digital filter to the image color channels that
%  improves the quality of a noisy image.  The opacity channel is preserved
%  but is otherwise ignored.
%
%  The format of the EnhanceImage method is:
%
%      Image *EnhanceImage(const Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
#define EnhanceImageText "[%s] Enhance...  "

MagickExport Image *EnhanceImage(const Image *image,ExceptionInfo *exception)
{
  static const double
    Weights[5][5] =
    {
      {  5.0,  8.0, 10.0,  8.0,  5.0 },
      {  8.0, 20.0, 40.0, 20.0,  8.0 },
      { 10.0, 40.0, 80.0, 40.0, 10.0 },
      {  8.0, 20.0, 40.0, 20.0,  8.0 },
      {  5.0,  8.0, 10.0,  8.0,  5.0 }
    };

  Image
    *enhance_image;

  long
    y;

  /*
    Initialize enhanced image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((image->columns < 5) || (image->rows < 5))
    return((Image *) NULL);
  enhance_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (enhance_image == (Image *) NULL)
    return((Image *) NULL);
  enhance_image->storage_class=DirectClass;
  /*
    Enhance image.
  */
  {
    unsigned long
      row_count=0;

    DoublePixelPacket
      zero;

    MagickPassFail
      status=MagickPass;

    (void) memset(&zero,0,sizeof(DoublePixelPacket));
#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    if defined(USE_STATIC_SCHEDULING_ONLY)
#      pragma omp parallel for schedule(static,4) shared(row_count, status)
#    else
#      pragma omp parallel for schedule(guided) shared(row_count, status)
#    endif
#  endif
#endif
    for (y=0; y < (long) image->rows; y++)
      {
        register const PixelPacket
          *p;

        register PixelPacket
          *q;

        register long
          x;

        MagickBool
          thread_status;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_EnhanceImage)
#endif
        thread_status=status;
        if (thread_status == MagickFail)
          continue;

        /*
          Read another scan line.
        */
        p=AcquireImagePixels(image,0,y-2,image->columns,5,exception);
        q=SetImagePixelsEx(enhance_image,0,y,enhance_image->columns,1,exception);
        if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
          thread_status=MagickFail;
        if (thread_status != MagickFail)
          {
	    DoublePixelPacket
	      aggregate;

	    double
	      pixel_red,
	      pixel_green,
	      pixel_blue,
	      total_weight;

	    register const PixelPacket
	      *r;

	    double
	      distance,
	      distance_squared,
	      mean;

	    unsigned long
	      i,
	      j;

            /*
              Transfer first 2 pixels of the scanline.
            */
            *q++=(*(p+2*image->columns));
            *q++=(*(p+2*image->columns+1));
            for (x=2; x < (long) (image->columns-2); x++)
              {
                /*
                  Compute weighted average of target pixel color components.
                */
                aggregate=zero;
                total_weight=0.0;
                r=p+2*image->columns+2;
		pixel_red=r->red;
		pixel_green=r->green;
		pixel_blue=r->blue;

		for (i=0 ; i < 5; i++)
		  {
		    r=p+i*image->columns;
		    for (j = 0; j < 5; j++)
		      {
			const double red = (double) r->red;
			const double green = (double) r->green;
			const double blue = (double) r->blue;
			mean=(red+pixel_red)/2.0;
			distance=red-pixel_red;
			distance_squared=(2.0*(MaxRGBDouble+1.0)+mean)*distance*
			  distance/MaxRGBDouble;
			mean=(green+pixel_green)/2.0;
			distance=green-pixel_green;
			distance_squared+=4.0*distance*distance;
			mean=(blue+pixel_blue)/2.0;
			distance=blue-pixel_blue;
			distance_squared+=
			  (3.0*(MaxRGBDouble+1.0)-1.0-mean)*distance*distance/MaxRGBDouble;
			if (distance_squared < ((MaxRGBDouble*MaxRGBDouble/25.0)))
			  {
			    const double weight = Weights[i][j];
			    aggregate.red+=weight*red;
			    aggregate.green+=weight*green;
			    aggregate.blue+=weight*blue;
			    total_weight+=weight;
			  }
			r++;
		      }
		  }
                q->red=(Quantum) ((aggregate.red+(total_weight/2.0)-1.0)/total_weight);
                q->green=(Quantum) ((aggregate.green+(total_weight/2.0)-1.0)/total_weight);
                q->blue=(Quantum) ((aggregate.blue+(total_weight/2.0)-1.0)/total_weight);
		q->opacity=p->opacity;
                p++;
                q++;
              }
            p++;
            *q++=(*p++);
            *q++=(*p++);
            if (!SyncImagePixelsEx(enhance_image,exception))
              thread_status=MagickFail;
          }
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_EnhanceImage)
#endif
        {
          row_count++;
          if (QuantumTick(row_count,image->rows))
            if (!MagickMonitorFormatted(row_count,image->rows,exception,
                                        EnhanceImageText,image->filename))
              thread_status=MagickFail;
          
          if (thread_status == MagickFail)
            status=MagickFail;
        }
      }
  }
  enhance_image->is_grayscale=image->is_grayscale;
  return(enhance_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     G a u s s i a n B l u r I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GaussianBlurImage() blurs an image.  We convolve the image with a
%  Gaussian operator of the given radius and standard deviation (sigma).
%  For reasonable results, the radius should be larger than sigma.  Use a
%  radius of 0 and GaussianBlurImage() selects a suitable radius for you
%
%  The format of the GaussianBlurImage method is:
%
%      Image *GaussianBlurImage(const Image *image,const double radius,
%        const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o blur_image: Method GaussianBlurImage returns a pointer to the image
%      after it is blur.  A null image is returned if there is a memory
%      shortage.
%
%    o image: Image to blur.
%
%    o radius: the radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: the standard deviation of the Gaussian, in pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *GaussianBlurImage(const Image *image,const double radius,
  const double sigma,ExceptionInfo *exception)
{
  double
    alpha,
    *kernel;

  Image
    *blur_image;

  int
    width;

  register long
    i,
    u,
    v;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth2D(radius,sigma);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException3(OptionError,UnableToBlurImage,
      ImageSmallerThanRadius);
  kernel=MagickAllocateMemory(double *,width*width*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException(ResourceLimitError,MemoryAllocationFailed,
      MagickMsg(OptionError,UnableToBlurImage));
  i=0;
  for (v=(-width/2); v <= (width/2); v++)
  {
    for (u=(-width/2); u <= (width/2); u++)
    {
      alpha=exp(-((double) u*u+v*v)/(2.0*sigma*sigma));
      kernel[i]=alpha/(2.0*MagickPI*sigma*sigma);
      i++;
    }
  }
  blur_image=ConvolveImage(image,width,kernel,exception);
  MagickFreeMemory(kernel);
  blur_image->is_grayscale=image->is_grayscale;
  return(blur_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     G a u s s i a n B l u r I m a g e C h a n n e l                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GaussianBlurImageChannel() blurs an image channel.  We convolve the image
%  with a Gaussian operator of the given radius and standard deviation
%  (sigma).  For reasonable results, the radius should be larger than sigma.
%  Use a radius of 0 and GaussianBlurImage() selects a suitable radius for
%  you.
%
%  The format of the GaussianBlurImageChannel method is:
%
%      Image *GaussianBlurImageChannel(const Image *image,
%        const ChannelType channel,const double radius,
%        const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o blur_image: Method GaussianBlurImage returns a pointer to the image
%      after it is blur.  A null image is returned if there is a memory
%      shortage.
%
%    o image: Image to blur.
%
%    o channel: Channel to blur in image.
%
%    o radius: the radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: the standard deviation of the Gaussian, in pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *GaussianBlurImageChannel(const Image *image,
  const ChannelType channel,const double radius,const double sigma,
  ExceptionInfo *exception)
{
  Image
    *blur_image;

  blur_image=GaussianBlurImage(image,radius,sigma,exception);
  if (blur_image != (Image *) NULL)
    (void) ImportImageChannelsMasked(image,blur_image,channel);

  return blur_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M e d i a n F i l t e r I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MedianFilterImage() applies a digital filter that improves the quality
%  of a noisy image.  Each pixel is replaced by the median in a set of
%  neighboring pixels as defined by radius.
%
%  The algorithm was contributed by Mike Edmonds and implements an insertion
%  sort for selecting median color-channel values.  For more on this algorithm
%  see "Skip Lists: A probabilistic Alternative to Balanced Trees" by William
%  Pugh in the June 1990 of Communications of the ACM.
%
%  The format of the MedianFilterImage method is:
%
%      Image *MedianFilterImage(const Image *image,const double radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: The radius of the pixel neighborhood.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

typedef struct _MedianListNode
{
  unsigned int
    next[9],
    count,
    signature;
} MedianListNode;

typedef struct _MedianSkipList
{
  MedianListNode
    *nodes;

  int
    level;
} MedianSkipList;

typedef struct _MedianPixelList
{
  MedianSkipList
    lists[4];

  unsigned int
    center,
    seed,
    signature;
} MedianPixelList;

static void AddNodeMedianList(MedianPixelList *pixel_list,
                              const unsigned int channel,
                              const unsigned int color)
{
  register MedianSkipList
    *list;

  register int
    level;

  unsigned int
    search,
    update[9];

  /*
    Initialize the node.
  */
  list=pixel_list->lists+channel;
  list->nodes[color].signature=pixel_list->signature;
  list->nodes[color].count=1;
  /*
    Determine where it belongs in the list.
    This loop consumes most of the time.
  */
  search=65536UL;
  for (level=list->level; level >= 0; level--)
    {
      while (list->nodes[search].next[level] < color)
        search=list->nodes[search].next[level];
      update[level]=search;
    }
  /*
    Generate a pseudo-random level for this node.
  */
  for (level=0; ; level++)
    {
      pixel_list->seed=(pixel_list->seed*42893621U)+1U;
      if ((pixel_list->seed & 0x300) != 0x300)
        break;
    }
  if (level > 8)
    level=8;
  if (level > (list->level+2))
    level=list->level+2;
  /*
    If we're raising the list's level, link back to the root node.
  */
  while (level > list->level)
    {
      list->level++;
      update[list->level]=65536U;
    }
  /*
    Link the node into the skip-list.
  */
  do
    {
      list->nodes[color].next[level]=list->nodes[update[level]].next[level];
      list->nodes[update[level]].next[level]=color;
    }
  while (level-- > 0);
}

static void GetMedianList(MedianPixelList * restrict pixel_list,PixelPacket *median_pixel)
{
  register MedianSkipList
    *list;

  register unsigned int
    channel;

  unsigned int
    center,
    color,
    count;

  unsigned short
    channels[4];

  /*
    Find the median value for each of the colors.
  */
  center=pixel_list->center;
  for (channel=0; channel < 4U; channel++)
    {
      list=pixel_list->lists+channel;
      color=65536L;
      count=0;
      do
        {
          color=list->nodes[color].next[0];
          count+=list->nodes[color].count;
        }
      while (count <= center); /* IM now uses count <= (center >> 1) */
      channels[channel]=color;
    }
  median_pixel->red=ScaleShortToQuantum(channels[0]);
  median_pixel->green=ScaleShortToQuantum(channels[1]);
  median_pixel->blue=ScaleShortToQuantum(channels[2]);
  median_pixel->opacity=ScaleShortToQuantum(channels[3]);
}

static inline void InsertMedianListChannel(MedianPixelList *pixel_list,
                                           const unsigned int channel,
                                           const Quantum quantum)
{
  register unsigned int
    index;

  index=ScaleQuantumToShort(quantum);
  if (pixel_list->lists[channel].nodes[index].signature == pixel_list->signature)
    pixel_list->lists[channel].nodes[index].count++;
  else
    AddNodeMedianList(pixel_list,channel,index);
}

static inline void InsertMedianList(MedianPixelList *pixel_list,
                                    const PixelPacket *pixel)
{
  InsertMedianListChannel(pixel_list,0,pixel->red);
  InsertMedianListChannel(pixel_list,1,pixel->green);
  InsertMedianListChannel(pixel_list,2,pixel->blue);
  InsertMedianListChannel(pixel_list,3,pixel->opacity);
}

static void ResetMedianList(MedianPixelList *pixel_list)
{
  register MedianListNode
    *root;

  register MedianSkipList
    *list;

  register long
    channel;

  int
    level;

  /*
    Reset the skip-list.
  */
  for (channel=0; channel < 4; channel++)
    {
      list=pixel_list->lists+channel;
      root=list->nodes+65536UL;
      list->level=0;
      for (level=0; level < 9; level++)
        root->next[level]=65536UL;
    }
  pixel_list->seed=pixel_list->signature++;
}

static void DestroyMedianList(void *pixel_list)
{
  MedianPixelList
    *skiplist;

  skiplist=(MedianPixelList *) pixel_list;

  if (skiplist != (void *) NULL)
    {
      unsigned int
        i;
      
      for (i=0; i < 4U; i++)
        MagickFreeAlignedMemory(skiplist->lists[i].nodes);
    }
  MagickFreeAlignedMemory(skiplist);
}

static MedianPixelList *AllocateMedianList(const long width)
{
  MedianPixelList
    *skiplist;

  skiplist=MagickAllocateAlignedMemory(MedianPixelList *,
				       MAGICK_CACHE_LINE_SIZE,
				       sizeof(MedianPixelList));
  if (skiplist != (MedianPixelList *) NULL)
    {
      unsigned int
        i;

      const size_t
	node_list_size = 65537U*sizeof(MedianListNode);

      (void) memset(skiplist,0,sizeof(MedianPixelList));
      skiplist->center=width*width/2;
      skiplist->signature=MagickSignature;
      for (i=0; i < 4U; i++)
        {
          skiplist->lists[i].nodes =
	    MagickAllocateAlignedMemory(MedianListNode *,MAGICK_CACHE_LINE_SIZE,
					node_list_size);
          if (skiplist->lists[i].nodes == (MedianListNode *) NULL)
            {
              DestroyMedianList(skiplist);
              skiplist=(MedianPixelList *) NULL;
              break;
            }
          (void) memset(skiplist->lists[i].nodes,0,node_list_size);
        }
    }
  return skiplist;
}

MagickExport Image *MedianFilterImage(const Image *image,const double radius,
                                      ExceptionInfo *exception)
{
#define MedianFilterImageText "[%s] Filter with neighborhood ranking..."

  Image
    *median_image;

  long
    width,
    y;

  ThreadViewDataSet
    *data_set;

  unsigned long
    row_count=0;

  MagickPassFail
    status=MagickPass;

  /*
    Initialize median image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth2D(radius,0.5);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException3(OptionError,UnableToFilterImage,
                         ImageSmallerThanRadius);
  median_image=CloneImage(image,image->columns,image->rows,MagickTrue,exception);
  if (median_image == (Image *) NULL)
    return ((Image *) NULL);

  median_image->storage_class=DirectClass;
  /*
    Allocate skip-lists.
  */
  data_set=AllocateThreadViewDataSet(DestroyMedianList,image,exception);
  if (data_set != (ThreadViewDataSet *) NULL)
    {
      unsigned int
        i,
        views;

      views=GetThreadViewDataSetAllocatedViews(data_set);
      for (i=0; i < views; i++)
        {
          MedianPixelList
            *skiplist;

          skiplist=AllocateMedianList(width);
          if (skiplist != (MedianPixelList *) NULL)
            {
              AssignThreadViewData(data_set,i,skiplist);
              continue;
            }

          DestroyThreadViewDataSet(data_set);
          data_set=(ThreadViewDataSet *) NULL;
          break;
        }
    }
  if (data_set == (ThreadViewDataSet *) NULL)
    {
      DestroyImage(median_image);
      return ((Image *) NULL);
    }
  {
#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for schedule(static,4) shared(row_count, status)
#  endif
#endif
    for (y=0; y < (long) median_image->rows; y++)
      {
        register MedianPixelList
          *skiplist;

        const PixelPacket
          *p;

        PixelPacket
          *q;

        long
          x;

        MagickBool
          thread_status;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_MedianFilterImage)
#endif
        thread_status=status;
        if (thread_status == MagickFail)
          continue;

        skiplist=AccessThreadViewData(data_set);
        p=AcquireImagePixels(image,-width/2,y-width/2,
                             image->columns+width,width,exception);
        q=SetImagePixelsEx(median_image,0,y,median_image->columns,1,exception);
        if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
          thread_status=MagickFail;
        if (thread_status != MagickFail)
          {
            for (x=0; x < (long) median_image->columns; x++)
              {
                register const PixelPacket
                  *r;

                register long
                  u,
                  v;

                ResetMedianList(skiplist);
                r=&p[x];
                for (v=0; v < width; v++)
                  {
                    for (u=0; u < width; u++)
                      InsertMedianList(skiplist,&r[u]);
                    r+=image->columns+width;
                  }
                GetMedianList(skiplist,&q[x]);
              }
            if (!SyncImagePixelsEx(median_image,exception))
              thread_status=MagickFail;
          }
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_MedianFilterImage)
#endif
        {
          row_count++;
          if (QuantumTick(row_count,median_image->rows))
            if (!MagickMonitorFormatted(row_count,median_image->rows,exception,
                                        MedianFilterImageText,
                                        median_image->filename))
              thread_status=MagickFail;

          if (thread_status == MagickFail)
            status=MagickFail;
        }
      }
  }
  DestroyThreadViewDataSet(data_set);
  median_image->is_grayscale=image->is_grayscale;
  return(median_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     M o t i o n B l u r I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  MotionBlurImage() simulates motion blur.  We convolve the image with a
%  Gaussian operator of the given radius and standard deviation (sigma).
%  For reasonable results, radius should be larger than sigma.  Use a
%  radius of 0 and MotionBlurImage() selects a suitable radius for you.
%  Angle gives the angle of the blurring motion (direction object appears
%  to be coming from).
%
%  Andrew Protano contributed this effect.
%
%  The format of the MotionBlurImage method is:
%
%    Image *MotionBlurImage(const Image *image,const double radius,
%      const double sigma,const double angle,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: The radius of the Gaussian, in pixels, not counting
%     the center pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%    o angle: Apply the effect along this angle.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static double *AllocateMotionBlurKernel(const int width,const double sigma)
{
#define KernelRank 3

  double
    *kernel;

  /*
    Generate a 1-D convolution matrix.  Calculate the kernel at higher
    resolution than needed and average the results as a form of numerical
    integration to get the best accuracy.
  */
  kernel=MagickAllocateMemory(double *,width*sizeof(double));
  if (kernel != (double *) NULL)
    {
      double
        alpha,
        normalize;
      
      int
        bias;

      register long
        i;

      for (i=0; i < width; i++)
        kernel[i]=0.0;
      bias=KernelRank*width;
      for (i=0; i < bias; i++)
        {
          alpha=exp(-((double) i*i)/(2.0*KernelRank*KernelRank*sigma*sigma));
          kernel[i/KernelRank]+=alpha/(MagickSQ2PI*sigma);
        }
      normalize=0;
      for (i=0; i < width; i++)
        normalize+=kernel[i];
      for (i=0; i < width; i++)
        kernel[i]/=normalize;
    }

  return kernel;
}

typedef struct _BlurOffsetInfo
{
  int
    x,
    y;
} BlurOffsetInfo;
#define MotionBlurImageText "[%s] Motion blur..."
MagickExport Image *MotionBlurImage(const Image *image,const double radius,
                                    const double sigma,const double angle,
                                    ExceptionInfo *exception)
{
  double
    *kernel;

  Image
    *blur_image;

  int
    width;

  BlurOffsetInfo
    *offsets;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  width=GetOptimalKernelWidth1D(radius,sigma);
  if (width < 3)
    ThrowImageException3(OptionError,UnableToBlurImage,
                         KernelRadiusIsTooSmall);
  kernel=AllocateMotionBlurKernel(width,sigma);
  if (kernel == (double *) NULL)
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
                         UnableToMotionBlurImage);
  {
    long
      x;

    long
      y;

    register long
      i;

    /*
      Allocate and initialize offsets.
    */
    offsets=MagickAllocateMemory(BlurOffsetInfo *,width*sizeof(BlurOffsetInfo));
    if (offsets == (BlurOffsetInfo *) NULL)
      {
        MagickFreeMemory(kernel);
        ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
                             UnableToMotionBlurImage);
      }
    x=(long) (width*sin(DegreesToRadians(angle+90)));
    y=(long) (width*cos(DegreesToRadians(angle+90)));
    for (i=0; i < width; i++)
      {
        offsets[i].x=(int) (i*x/sqrt(x*x+y*y)+0.5);
        offsets[i].y=(int) (i*y/sqrt(x*x+y*y)+0.5);
      }
  }
  /*
    Allocate blur image.
  */
  blur_image=CloneImage(image,image->columns,image->rows,MagickTrue,exception);
  if (blur_image == (Image *) NULL)
    {
      MagickFreeMemory(kernel);
      MagickFreeMemory(offsets);
      return((Image *) NULL);
    }
  blur_image->storage_class=DirectClass;
  {
    /*
      Motion blur image.
    */
    unsigned long
      row_count=0;
    
    long
      y;

    DoublePixelPacket
      zero;

    MagickPassFail
      status;

    status=MagickPass;
    (void) memset(&zero,0,sizeof(DoublePixelPacket));
#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for schedule(static,4) shared(row_count, status)
#  endif
#endif
    for (y=0; y < (long) image->rows; y++)
      {
        register PixelPacket
          *q;

        register long
          x;

        MagickBool
          matte=blur_image->matte;

        MagickBool
          thread_status;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_MotionBlurImage)
#endif
        thread_status=status;
        if (thread_status == MagickFail)
          continue;

        q=SetImagePixelsEx(blur_image,0,y,blur_image->columns,1,exception);
        if (q == (PixelPacket *) NULL)
          thread_status=MagickFail;
        if (thread_status != MagickFail)
          {
            for (x=0; x < (long) image->columns; x++)
              {
                DoublePixelPacket
                  aggregate;

                PixelPacket
                  pixel;

                register long
                  i;

                aggregate=zero;
                for (i=0; i < width; i++)
                  {
                    if (AcquireOnePixelByReference(image,&pixel,
                                                   (long) x+offsets[i].x,
                                                   (long) y+offsets[i].y,
                                                   exception)
                        == MagickFail)
                      {
                        thread_status=MagickFail;
                      }

                    aggregate.red+=kernel[i]*pixel.red;
                    aggregate.green+=kernel[i]*pixel.green;
                    aggregate.blue+=kernel[i]*pixel.blue;
                    if (matte)
                      aggregate.opacity+=kernel[i]*pixel.opacity;
                  }
                if (thread_status == MagickFail)
                  break;
                q->red=(Quantum) aggregate.red;
                q->green=(Quantum) aggregate.green;
                q->blue=(Quantum) aggregate.blue;
                if (matte)
                  q->opacity=(Quantum) aggregate.opacity;
                q++;
              }
            if (!SyncImagePixelsEx(blur_image,exception))
              thread_status=MagickFail;
          }
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_MotionBlurImage)
#endif
        {
          row_count++;
          if (QuantumTick(row_count,image->rows))
            if (!MagickMonitorFormatted(row_count,image->rows,exception,
                                        MotionBlurImageText,image->filename))
              thread_status=MagickFail;
          
          if (thread_status == MagickFail)
            status=MagickFail;
        }
      }
  }
  MagickFreeMemory(kernel);
  MagickFreeMemory(offsets);
  blur_image->is_grayscale=image->is_grayscale;
  return(blur_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     R a n d o m C h a n n e l T h r e s h o l d I m a g e                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RandomChannelThresholdImage() changes the value of individual pixels based
%  on the intensity of each pixel compared to a random threshold.  The result
%  is a low-contrast, two color image.
%
%  The format of the RandomChannelThresholdImage method is:
%
%      unsigned int RandomChannelThresholdImage(Image *image,
%         const char *channel, const char *thresholds,
%         ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: The channel or channels to be thresholded.
%
%    o thresholds: a geometry string containing LOWxHIGH thresholds.
%      If the string contains 2x2, 3x3, 4x4, 5x5, 6x6, or 7x7, then
%      an ordered dither of order 2, 3, 4, 5, 6, or 7 will be performed
%      instead.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport MagickPassFail
RandomChannelThresholdImage(Image *image,const char *channel,
                            const char *thresholds,ExceptionInfo *exception)
{
#define RandomChannelThresholdImageText "[%s] Random-channel threshold...  "

  const double
    o2[4]={1.0, 3.0,
           4.0, 2.0};

  const double
    o3[9]={1.0, 6.0, 3.0,
           7.0, 5.0, 8.0,
           4.0, 9.0, 2.0};

  const double
    o4[16]={ 1.0,  9.0,  3.0, 11.0,
            13.0,  5.0, 15.0,  7.0,
             4.0, 12.0,  2.0, 10.0,
            16.0,  8.0, 14.0,  6.0};

  const double
    o5[25]={ 1.0,  5.0, 16.0, 15.0,  4.0,
             6.0, 17.0, 20.0, 19.0, 14.0,
             7.0, 21.0, 25.0, 24.0, 13.0,
             8.0, 18.0, 22.0, 23.0, 12.0,
             2.0,  9.0, 10.0, 11.0,  3.0};

  const double
    o6[36]={ 1.0,   5.0,  14.0,  13.0,  12.0,   4.0,
             6.0,  22.0,  28.0,  27.0,  21.0,  11.0,
            15.0,  29.0,  35.0,  34.0,  26.0,  20.0,
            16.0,  30.0,  36.0,  33.0,  25.0,  19.0,
             7.0,  23.0,  31.0,  32.0,  24.0,  10.0,
             2.0,   8.0,  17.0,  18.0,   9.0,   3.0};

  const double
    o7[49]={ 2.0,   7.0,  17.0,  26.0,  15.0,   6.0,   1.0, 
             8.0,  16.0,  33.0,  39.0,  32.0,  13.0,   5.0, 
            18.0,  34.0,  44.0,  48.0,  43.0,  31.0,  14.0, 
            27.0,  40.0,  45.0,  49.0,  47.0,  38.0,  25.0, 
            20.0,  35.0,  41.0,  46.0,  42.0,  30.0,  24.0, 
             9.0,  19.0,  36.0,  37.0,  29.0,  22.0,  12.0, 
             3.0,  10.0,  21.0,  28.0,  23.0,  11.0,   4.0};


  Quantum
    matrix[49];

  Quantum
    lower_threshold=0U,
    upper_threshold=MaxRGB;

  long
    count;

  unsigned int
    i,
    order;

  const MagickBool
    is_grayscale=image->is_grayscale,
    is_monochrome=image->is_monochrome;

  MagickBool
    logging;

  ChannelType
    channel_type;

  MagickPassFail
    status=MagickPass;

  /*
    Threshold image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);

  if (image->is_monochrome && !image->matte)
    return(True);
  if (thresholds == (const char *) NULL)
    return(True);

  if (LocaleCompare(thresholds,"2x2") == 0)
    order=2;
  else if (LocaleCompare(thresholds,"3x3") == 0)
    order=3;
  else if (LocaleCompare(thresholds,"4x4") == 0)
    order=4;
  else if (LocaleCompare(thresholds,"5x5") == 0)
    order=5;
  else if (LocaleCompare(thresholds,"6x6") == 0)
    order=6;
  else if (LocaleCompare(thresholds,"7x7") == 0)
    order=7;
  else
    {
      double
        lower,
        upper;

      order=1;
      lower=0.0;
      upper=0.0;
      count=sscanf(thresholds,"%lf[/x%%]%lf",
                   &lower,&upper);

      if (strchr(thresholds,'%') != (char *) NULL)
        {
          upper*=(0.01*MaxRGB);
          lower*=(0.01*MaxRGB);
        }
      if (count == 1)
        upper=MaxRGBDouble-lower;
      lower_threshold=RoundDoubleToQuantum(lower);
      upper_threshold=RoundDoubleToQuantum(upper);
    }

  logging=LogMagickEvent(TransformEvent,GetMagickModule(),
                         "  RandomChannelThresholdImage: channel type=%s",channel);
  if (logging)
    (void)LogMagickEvent(TransformEvent,GetMagickModule(),
                         "    Thresholds: %s (%lux%lu)",
                         thresholds,(unsigned long) lower_threshold,
                         (unsigned long) upper_threshold);

  channel_type=StringToChannelType(channel);
  if (UndefinedChannel == channel_type)
    ThrowBinaryException3(OptionError, UnableToThresholdImage,
                          UnrecognizedChannelType);

  if ((AllChannels == channel_type) ||
      (GrayChannel == channel_type))
    if (!AllocateImageColormap(image,2))
      ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
                            UnableToThresholdImage);

  {
    double
      value;

    /*
      Pre-scale the ordered dither matrix.
    */
    if (2 == order)
      for (i=0;i < (sizeof(o2)/sizeof(double)); i++)
        {
          value=o2[i]*(MaxRGBDouble/5.0);
          matrix[i]=RoundDoubleToQuantum(value);
        }
    else if (3 == order)
      for (i=0;i < (sizeof(o3)/sizeof(double)); i++)
        {
          value=o3[i]*(MaxRGBDouble/10.0);
          matrix[i]=RoundDoubleToQuantum(value);
        }
    else if (4 == order)
      for (i=0;i < (sizeof(o4)/sizeof(double)); i++)
        {
          value=o4[i]*(MaxRGBDouble/17.0);
          matrix[i]=RoundDoubleToQuantum(value);
        }
    else if (5 == order)
      for (i=0;i < (sizeof(o5)/sizeof(double)); i++)
        {
          value=o5[i]*(MaxRGBDouble/26.0);
          matrix[i]=RoundDoubleToQuantum(value);
        }
    else if (6 == order)
      for (i=0;i < (sizeof(o6)/sizeof(double)); i++)
        {
          value=o6[i]*(MaxRGBDouble/37.0);
          matrix[i]=RoundDoubleToQuantum(value);
        }
    else if (7 == order)
      for (i=0;i < (sizeof(o7)/sizeof(double)); i++)
        {
          value=o7[i]*(MaxRGBDouble/50.0);
          matrix[i]=RoundDoubleToQuantum(value);
        }
    else
      {
        for (i=0;i < (sizeof(matrix)/sizeof(Quantum)); i++)
          matrix[i]=0U;
      }
  }

  {
    unsigned long
      row_count=0;

    long
      y;

#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for schedule(static,4) shared(row_count, status)
#  endif
#endif
    for (y=0; y < (long) image->rows; y++)
      {
        Quantum
          intensity,
          threshold=0U;

        register IndexPacket
          *indexes;

        register PixelPacket
          *q;

        register IndexPacket
          index;

        register unsigned long
          x;

	MagickRandomKernel
	  *random_kernel;

        MagickBool
          thread_status;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_RandomChannelThresholdImage)
#endif
        thread_status=status;
        if (thread_status == MagickFail)
          continue;

	random_kernel=AcquireMagickRandomKernel();
        q=GetImagePixelsEx(image,0,y,image->columns,1,exception);
        if (q == (PixelPacket *) NULL)
          thread_status=MagickFail;
        if (thread_status != MagickFail)
          {
            indexes=AccessMutableIndexes(image);
            if (((AllChannels == channel_type) ||
                 (GrayChannel == channel_type)) &&
                (!is_monochrome))
              {
                switch (order)
                  {
                  case 1:
                    for (x=(long) image->columns; x > 0; x--)
                      {
                        intensity=(is_grayscale ?
                            q->red : PixelIntensityToQuantum(q));
                        if (intensity < lower_threshold)
                          threshold=lower_threshold;
                        else if (intensity > upper_threshold)
                          threshold=upper_threshold;
                        else
                          threshold=(Quantum) (MaxRGBDouble*MagickRandomRealInlined(random_kernel));
                        index=intensity <= threshold ? 0U : 1U;
                        *indexes++=index;
                        q->red=q->green=q->blue=image->colormap[index].red;
                        q++;
                      }
                    break;
                  case 2:
                  case 3:
                  case 4:
                  case 5:
                  case 6:
                  case 7:
                    for (x=(long) image->columns; x > 0; x--)
                      {
                        intensity=(is_grayscale ?
                            q->red : PixelIntensityToQuantum(q));
                        threshold=matrix[(x%order)+order*(y%order)];
                        index=intensity <= threshold ? 0U : 1U;
                        *indexes++=index;
                        q->red=q->green=q->blue=image->colormap[index].red;
                        q++;
                      }
                    break;
                  }
              }
            if ((OpacityChannel == channel_type) ||
                (AllChannels == channel_type) ||
                (MatteChannel == channel_type) ||
                (BlackChannel == channel_type))
              {
                if (image->matte)
                  switch (order)
                    {
                    case 1:
                      for (x=(long) image->columns; x > 0; x--)
                        {
                          if (q->opacity < lower_threshold)
                            threshold=lower_threshold;
                          else if (q->opacity > upper_threshold)
                            threshold=upper_threshold;
                          else
                            threshold=(Quantum) (MaxRGBDouble*MagickRandomRealInlined(random_kernel));
                          q->opacity=(q->opacity <= threshold ? 0U : MaxRGB);
                          q++;
                        }
                      break;
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                      for (x=(long) image->columns; x > 0; x--)
                        {
                          threshold=matrix[(x%order)+order*(y%order)];
                          q->opacity=(q->opacity <= threshold ? 0U : MaxRGB);
                          q++;
                        }
                      break;
                    }
              }

            if ((RedChannel == channel_type) ||
                (CyanChannel == channel_type))
              {
                switch (order)
                  {
                  case 1:
                    for (x=(long) image->columns; x > 0; x--)
                      {
                        if (q->red < lower_threshold)
                          threshold=lower_threshold;
                        else if (q->red > upper_threshold)
                          threshold=upper_threshold;
                        else
                          threshold=(Quantum) (MaxRGBDouble*MagickRandomRealInlined(random_kernel));
                        q->red=(q->red <= threshold ? 0U : MaxRGB);
                        q++;
                      }
                    break;
                  case 2:
                  case 3:
                  case 4:
                  case 5:
                  case 6:
                  case 7:
                    for (x=(long) image->columns; x > 0; x--)
                      {
                        threshold=matrix[(x%order)+order*(y%order)];
                        q->red=(q->red <= threshold ? 0U : MaxRGB);
                        q++;
                      }
                    break;
                  }
              }

            if ((GreenChannel == channel_type) ||
                (MagentaChannel == channel_type))
              {
                switch (order)
                  {
                  case 1:
                    for (x=(long) image->columns; x > 0; x--)
                      {
                        if (q->green < lower_threshold)
                          threshold=lower_threshold;
                        else if (q->green > upper_threshold)
                          threshold=upper_threshold;
                        else
                          threshold=(Quantum) (MaxRGBDouble*MagickRandomRealInlined(random_kernel));
                        q->green=(q->green <= threshold ? 0U : MaxRGB);
                        q++;
                      }
                    break;
                  case 2:
                  case 3:
                  case 4:
                  case 5:
                  case 6:
                  case 7:
                    for (x=(long) image->columns; x > 0; x--)
                      {
                        threshold=matrix[(x%order)+order*(y%order)];
                        q->green=(q->green <= threshold ? 0U : MaxRGB);
                        q++;
                      }
                    break;
                  }
              }
            
            if ((BlueChannel == channel_type) ||
                (YellowChannel == channel_type))
              {
                switch (order)
                  {
                  case 1:
                    for (x=(long) image->columns; x > 0; x--)
                      {
                        if (q->blue < lower_threshold)
                          threshold=lower_threshold;
                        else if (q->blue > upper_threshold)
                          threshold=upper_threshold;
                        else
                          threshold=(Quantum) (MaxRGBDouble*MagickRandomRealInlined(random_kernel));
                        q->blue=(q->blue <= threshold ? 0U : MaxRGB);
                        q++;
                      }
                    break;
                  case 2:
                  case 3:
                  case 4:
                  case 5:
                  case 6:
                  case 7:
                    for (x=(long) image->columns; x > 0; x--)
                      {
                        threshold=matrix[(x%order)+order*(y%order)];
                        q->blue=(q->blue <= threshold ? 0U : MaxRGB);
                        q++;
                      }
                    break;
                  }
              }

            if (!SyncImagePixelsEx(image,exception))
              thread_status=MagickFail;
          }
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_RandomChannelThresholdImage)
#endif
        {
          row_count++;
          if (QuantumTick(row_count,image->rows))
            if (!MagickMonitorFormatted(row_count,image->rows,exception,
                                        RandomChannelThresholdImageText,
                                        image->filename))
              thread_status=MagickFail;
          
          if (thread_status == MagickFail)
            status=MagickFail;
        }
      }
  }
  if ((AllChannels == channel_type) ||
      (GrayChannel == channel_type))
    {
      image->is_monochrome=True;
      image->is_grayscale=True;
    }
  return (status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     R e d u c e N o i s e I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReduceNoiseImage() smooths the contours of an image while still preserving
%  edge information.  The algorithm works by replacing each pixel with its
%  neighbor closest in value.  A neighbor is defined by radius.  Use a radius
%  of 0 and ReduceNoise() selects a suitable radius for you.
%
%  The format of the ReduceNoiseImage method is:
%
%      Image *ReduceNoiseImage(const Image *image,const double radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: The radius of the pixel neighborhood.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static PixelPacket GetNonpeakMedianList(MedianPixelList *pixel_list)
{
  PixelPacket
    pixel;

  register MedianSkipList
    *list;

  register long
    channel;

  unsigned long
    channels[4],
    center,
    color,
    count,
    previous,
    next;

  /*
    Finds the median value for each of the color.
  */
  center=pixel_list->center;
  for (channel=0; channel < 4; channel++)
  {
    list=pixel_list->lists+channel;
    color=65536L;
    next=list->nodes[color].next[0];
    count=0;
    do
    {
      previous=color;
      color=next;
      next=list->nodes[color].next[0];
      count+=list->nodes[color].count;
    }
    while (count <= center);
    if ((previous == 65536L) && (next != 65536L))
      color=next;
    else
      if ((previous != 65536L) && (next == 65536L))
        color=previous;
    channels[channel]=color;
  }
  pixel.red=ScaleShortToQuantum(channels[0]);
  pixel.green=ScaleShortToQuantum(channels[1]);
  pixel.blue=ScaleShortToQuantum(channels[2]);
  pixel.opacity=ScaleShortToQuantum(channels[3]);
  return(pixel);
}

MagickExport Image *ReduceNoiseImage(const Image *image,const double radius,
                                     ExceptionInfo *exception)
{
#define ReduceNoiseImageText "[%s] Reduce noise...  "

  Image
    *noise_image;

  long
    width,
    y;

  ThreadViewDataSet
    *data_set;

  unsigned long
    row_count=0;

  MagickPassFail
    status=MagickPass;

  /*
    Initialize noise image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth2D(radius,0.5);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException3(OptionError,UnableToFilterImage,
                         ImageSmallerThanRadius);
  noise_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (noise_image == (Image *) NULL)
    return ((Image *) NULL);

  noise_image->storage_class=DirectClass;
  /*
    Allocate and initialize skip-lists.
  */
  data_set=AllocateThreadViewDataSet(DestroyMedianList,image,exception);
  if (data_set != (ThreadViewDataSet *) NULL)
    {
      unsigned int
        i,
        views;

      views=GetThreadViewDataSetAllocatedViews(data_set);
      for (i=0; i < views; i++)
        {
          MedianPixelList
            *skiplist;

          skiplist=AllocateMedianList(width);
          if (skiplist != (MedianPixelList *) NULL)
            {
              AssignThreadViewData(data_set,i,skiplist);
              continue;
            }

          DestroyThreadViewDataSet(data_set);
          data_set=(ThreadViewDataSet *) NULL;
          break;
        }
    }
  if (data_set == (ThreadViewDataSet *) NULL)
    {
      DestroyImage(noise_image);
      return ((Image *) NULL);
    }

#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for schedule(static,4) shared(row_count, status)
#  endif
#endif
  for (y=0; y < (long) noise_image->rows; y++)
    {
      const PixelPacket
        *p;

      PixelPacket
        *q;

      long
        x;

      MedianPixelList
        *skiplist;

      MagickBool
        thread_status;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_ReduceNoiseImage)
#endif
      thread_status=status;
      if (thread_status == MagickFail)
        continue;
      
      skiplist=AccessThreadViewData(data_set);
      p=AcquireImagePixels(image,-width/2,y-width/2,
                           image->columns+width,width,exception);
      q=SetImagePixelsEx(noise_image,0,y,noise_image->columns,1,exception);
      if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        thread_status=MagickFail;
      if (thread_status != MagickFail)
        {
          for (x=0; x < (long) noise_image->columns; x++)
            {
              register const PixelPacket
                *r;

              register long
                u,
                v;

              r=p;
              ResetMedianList(skiplist);
              for (v=width; v > 0; v--)
                {
                  for (u=0; u < width; u++)
                    InsertMedianList(skiplist,&r[u]);
                  r+=image->columns+width;
                }
              q[x]=GetNonpeakMedianList(skiplist);
              p++;
            }
          if (!SyncImagePixelsEx(noise_image,exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_ReduceNoiseImage)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,noise_image->rows))
          if (!MagickMonitorFormatted(row_count,noise_image->rows,exception,
                                      ReduceNoiseImageText,
                                      noise_image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }
  DestroyThreadViewDataSet(data_set);
  noise_image->is_grayscale=image->is_grayscale;
  return(noise_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     S h a d e I m a g e                                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ShadeImage() shines a distant light on an image to create a
%  three-dimensional effect. You control the positioning of the light with
%  azimuth and elevation; azimuth is measured in degrees off the x axis
%  and elevation is measured in pixels above the Z axis.
%
%  The format of the ShadeImage method is:
%
%      Image *ShadeImage(const Image *image,const unsigned int gray,
%        double azimuth,double elevation,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o gray: A value other than zero shades the intensity of each pixel.
%
%    o azimuth, elevation:  Define the light source direction.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

MagickExport Image *ShadeImage(const Image *image,const unsigned int gray,
                               double azimuth,double elevation,ExceptionInfo *exception)
{
#define ShadeImageText "[%s] Shade..."

  Image
    *shade_image;

  long
    y;

  PrimaryInfo
    light;

  MagickPassFail
    status=MagickPass;

  /*
    Initialize shaded image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  shade_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (shade_image == (Image *) NULL)
    return((Image *) NULL);
  shade_image->storage_class=DirectClass;
  /*
    Compute the light vector.
  */
  azimuth=DegreesToRadians(azimuth);
  elevation=DegreesToRadians(elevation);
  light.x=(double) MaxRGBDouble*cos(azimuth)*cos(elevation);
  light.y=(double) MaxRGBDouble*sin(azimuth)*cos(elevation);
  light.z=(double) MaxRGBDouble*sin(elevation);
  /*
    Shade image.
  */
  {
    unsigned long
      row_count=0;
    
#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for schedule(static,4) shared(row_count, status)
#  endif
#endif
    for (y=0; y < (long) image->rows; y++)
      {
        PrimaryInfo
          normal;

        register const PixelPacket
          *p,
          *s0,
          *s1,
          *s2;

        register PixelPacket
          *q;

        register long
          x;

        MagickBool
          thread_status;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_ShadeImage)
#endif
        thread_status=status;
        if (thread_status == MagickFail)
          continue;

        normal.z=2.0*MaxRGBDouble;  /* constant Z of surface normal */

        p=AcquireImagePixels(image,-1,y-1,image->columns+2,3,exception);
        q=SetImagePixelsEx(shade_image,0,y,shade_image->columns,1,exception);
        if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
          thread_status=MagickFail;
        /*
          Shade this row of pixels.
        */
        if (thread_status != MagickFail)
          {
            s0=p+1;
            s1=s0+image->columns+2;
            s2=s1+image->columns+2;
            for (x=0; x < (long) image->columns; x++)
              {
                double
                  distance,
                  normal_distance,
                  shade;

                /*
                  Determine the surface normal and compute shading.
                */
                normal.x=PixelIntensityToDouble(s0-1)+ PixelIntensityToDouble(s1-1)+
                  PixelIntensityToDouble(s2-1)-PixelIntensityToDouble(s0+1)-
                  PixelIntensityToDouble(s1+1)-PixelIntensityToDouble(s2+1);
                normal.y=PixelIntensityToDouble(s2-1)+PixelIntensityToDouble(s2)+
                  PixelIntensityToDouble(s2+1)-PixelIntensityToDouble(s0-1)-
                  PixelIntensityToDouble(s0)-PixelIntensityToDouble(s0+1);
                if ((normal.x == 0.0) && (normal.y == 0.0))
                  shade=light.z;
                else
                  {
                    shade=0.0;
                    distance=normal.x*light.x+normal.y*light.y+normal.z*light.z;
                    if (distance > MagickEpsilon)
                      {
                        normal_distance=
                          normal.x*normal.x+normal.y*normal.y+normal.z*normal.z;
                        if (normal_distance > (MagickEpsilon*MagickEpsilon))
                          shade=distance/sqrt(normal_distance);
                      }
                  }
                if (gray)
                  {
                    q->red=(Quantum) shade;
                    q->green=(Quantum) shade;
                    q->blue=(Quantum) shade;
                  }
                else
                  {
                    q->red=(Quantum) ((shade*s1->red)/MaxRGBDouble+0.5);
                    q->green=(Quantum) ((shade*s1->green)/MaxRGBDouble+0.5);
                    q->blue=(Quantum) ((shade*s1->blue)/MaxRGBDouble+0.5);
                  }
                q->opacity=s1->opacity;
                s0++;
                s1++;
                s2++;
                q++;
              }
            if (!SyncImagePixelsEx(shade_image,exception))
              thread_status=MagickFail;
          }
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_ShadeImage)
#endif
        {
          row_count++;
          if (QuantumTick(row_count,image->rows))
            if (!MagickMonitorFormatted(row_count,image->rows,exception,
                                        ShadeImageText,image->filename))
              thread_status=MagickFail;
          
          if (thread_status == MagickFail)
            status=MagickFail;
        }
      }
  }
  shade_image->is_grayscale=image->is_grayscale;
  if (gray)
    shade_image->is_grayscale=True;
  return(shade_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     S h a r p e n I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SharpenImage() sharpens an image.  We convolve the image with a
%  Gaussian operator of the given radius and standard deviation (sigma).
%  For reasonable results, radius should be larger than sigma.  Use a
%  radius of 0 and SharpenImage() selects a suitable radius for you.
%
%  The format of the SharpenImage method is:
%
%    Image *SharpenImage(const Image *image,const double radius,
%      const double sigma,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Laplacian, in pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *SharpenImage(const Image *image,const double radius,
  const double sigma,ExceptionInfo *exception)
{
  double
    alpha,
    *kernel,
    normalize;

  Image
    *sharp_image;

  long
    width;

  register long
    i,
    u,
    v;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  width=GetOptimalKernelWidth(radius,sigma);
  if (((long) image->columns < width) || ((long) image->rows < width))
    ThrowImageException3(OptionError,UnableToSharpenImage,
      ImageSmallerThanRadius);
  kernel=MagickAllocateMemory(double *,width*width*sizeof(double));
  if (kernel == (double *) NULL)
    ThrowImageException3(ResourceLimitError,MemoryAllocationFailed,
      UnableToSharpenImage);
  i=0;
  normalize=0.0;
  for (v=(-width/2); v <= (width/2); v++)
  {
    for (u=(-width/2); u <= (width/2); u++)
    {
      alpha=exp(-((double) u*u+v*v)/(2.0*sigma*sigma));
      kernel[i]=alpha/(2.0*MagickPI*sigma*sigma);
      normalize+=kernel[i];
      i++;
    }
  }
  kernel[i/2]=(-2.0)*normalize;
  sharp_image=ConvolveImage(image,width,kernel,exception);
  MagickFreeMemory(kernel);
  sharp_image->is_grayscale=image->is_grayscale;
  return(sharp_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     S h a r p e n I m a g e C h a n n e l                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SharpenImageChannel() sharpens an image channel.  We convolve the image
%  channel with a Gaussian operator of the given radius and standard
%  deviation (sigma). For reasonable results, radius should be larger than
%  sigma.  Use a radius of 0 and SharpenImageChannel() selects a suitable
%  radius for you.
%
%  The format of the SharpenImageChannel method is:
%
%    Image *SharpenImageChannel(const Image *image, const ChannelType channel,
%       const double radius,const double sigma, ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: The channel to sharpen.
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Laplacian, in pixels.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *SharpenImageChannel(const Image *image,
  const ChannelType channel,const double radius,const double sigma,
  ExceptionInfo *exception)
{
  Image
    *sharp_image;

  sharp_image=SharpenImage(image,radius,sigma,exception);
  if (sharp_image != (Image *) NULL)
    (void) ImportImageChannelsMasked(image,sharp_image,channel);

  return sharp_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     S p r e a d I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  SpreadImage() is a special effects method that randomly displaces each
%  pixel in a block defined by the radius parameter.
%
%  The format of the SpreadImage method is:
%
%      Image *SpreadImage(const Image *image,const unsigned int radius,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius:  Choose a random pixel in a neighborhood of this extent.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *SpreadImage(const Image *image,const unsigned int radius,
                                ExceptionInfo *exception)
{
#define SpreadImageText "[%s] Spread..."
#define OFFSETS_ENTRIES 5009U /* prime number is best */

  Image
    *spread_image;

  int
    *offsets;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  if ((image->columns < 3) || (image->rows < 3))
    return((Image *) NULL);
  /*
    Initialize spread image attributes.
  */
  spread_image=CloneImage(image,image->columns,image->rows,True,exception);
  if (spread_image == (Image *) NULL)
    return((Image *) NULL);
  spread_image->storage_class=DirectClass;

  /*
    Initialize random offsets cache
  */
  {
    MagickRandomKernel
      *random_kernel;

    unsigned int
      x;

    random_kernel=AcquireMagickRandomKernel();
    offsets=MagickAllocateMemory(int *,OFFSETS_ENTRIES*sizeof(int));
    if (offsets == (int *) NULL)
      {
        ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,NULL);
        return (Image *) NULL;
      }
    for (x=0; x < OFFSETS_ENTRIES; x++)
      {
        offsets[x]=(((2*(double) radius+1)*MagickRandomRealInlined(random_kernel))
		    -((int) radius));
      }
  }

  /*
    Spread each row.
  */
  {
    unsigned long
      row_count=0;

    long
      y;

    MagickPassFail
      status=MagickPass;

#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for schedule(static,8) shared(row_count, status)
#  endif
#endif
    for (y=0; y < (long) image->rows; y++)
      {
        register PixelPacket
          *q;

        const PixelPacket
          *neighbors;

        register long
          x;

        long
          y_min,
          y_max;

        long
          x_distance,
          y_distance;

        unsigned int
          offsets_index;

        MagickBool
          thread_status;

#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical (GM_SpreadImage)
#endif
        thread_status=status;
        if (thread_status == MagickFail)
          continue;

        offsets_index=(y*image->columns) % OFFSETS_ENTRIES;
        q=SetImagePixelsEx(spread_image,0,y,spread_image->columns,1,exception);
        if (q == (PixelPacket *) NULL)
          thread_status=MagickFail;
        if (radius > (unsigned int) y)
          y_min=0;
        else
          y_min=y-radius;
    
        if (((unsigned long) y+radius) >= image->rows)
          y_max=image->rows-1;
        else
          y_max=y+radius;

        neighbors=AcquireImagePixels(image,0,y_min,image->columns,y_max-y_min,exception);
        if (neighbors == (PixelPacket *) NULL)
          thread_status=MagickFail;
        if (thread_status != MagickFail)
          {
            for (x=0; x < (long) image->columns; x++)
              {
                unsigned int
                  tries;

                tries=0;
                do
                  {
                    x_distance=offsets[offsets_index++];
                    if (offsets_index == OFFSETS_ENTRIES)
                      {
                        if (tries++)
                          {
                            x_distance=0;
                            break;
                          }
                        offsets_index=0;
                      }
                  } while (((x+x_distance) < 0) ||
                           ((x+x_distance) >= (long) image->columns));
                tries=0;
                do
                  {
                    y_distance=offsets[offsets_index++];
                    if (offsets_index == OFFSETS_ENTRIES)
                      {
                        if (tries++)
                          {
                            y_distance=0;
                            break;
                          }
                        offsets_index=0;
                      }
                  } while (((y+y_distance) < 0) ||
                           ((y+y_distance) >= (long) image->rows));
                *q=*(neighbors+(x+x_distance)+((y+y_distance-y_min)*image->columns));
                q++;
              }
            if (!SyncImagePixelsEx(spread_image,exception))
              thread_status=MagickFail;
          }
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical (GM_SpreadImage)
#endif
        {
          row_count++;
          if (QuantumTick(row_count,image->rows))
            if (!MagickMonitorFormatted(row_count,image->rows,exception,
                                        EnhanceImageText,image->filename))
              thread_status=MagickFail;
          
          if (thread_status == MagickFail)
            status=MagickFail;
        }
      }
  }
  MagickFreeMemory(offsets);
  spread_image->is_grayscale=image->is_grayscale;
  spread_image->is_monochrome=image->is_monochrome;
  return(spread_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     T h r e s h o l d I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ThresholdImage() changes the value of individual pixels based on
%  the intensity of each pixel compared to a specified threshold.  Values
%  greater than the threshold are set to the maximum quantum value, and
%  values equal to or below the threshold are set to the minimum quantum
%  value.  The result is a high-contrast, two color image.
%
%  The format of the ThresholdImage method is:
%
%      MagickPassFail ThresholdImage(Image *image,const double threshold)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o threshold: Define the threshold value
%
%
*/
MagickExport MagickPassFail ThresholdImage(Image *image,const double threshold)
{
#define ThresholdImageText "[%s] Threshold..."

  long
    y;

  Quantum
    quantum_threshold;

  MagickBool
    grayscale,
    initialize_indexes;

  /*
    Threshold image.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  initialize_indexes=(image->storage_class == PseudoClass ? MagickFalse : MagickTrue);
  grayscale=image->is_grayscale;
  quantum_threshold=RoundDoubleToQuantum(threshold);

  /*
    Check if image is already in desired state.
  */
  if ((quantum_threshold != MaxRGB) && (image->storage_class == PseudoClass)
      && (image->colors == 2))
    {
      if (IsBlackPixel(image->colormap[0]) && (IsWhitePixel(image->colormap[1])))
        {
          image->is_monochrome=MagickTrue;
          image->is_grayscale=MagickTrue;
          return MagickPass;
        }
    }

  if (!AllocateImageColormap(image,2))
    ThrowBinaryException3(ResourceLimitError,MemoryAllocationFailed,
                          UnableToThresholdImage);

  {
    MagickPassFail
      status=MagickPass;

    unsigned long
      row_count=0;

#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for schedule(static,8) shared(row_count, status)
#  endif
#endif
    for (y=0; y < (long) image->rows; y++)
      {
        register unsigned long
          x;

        register PixelPacket
          *q;

        register IndexPacket
          index;

        register IndexPacket
          *indexes;

        MagickBool
          modified;

        Quantum
          intensity;

        MagickBool
          thread_status;

#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical (GM_ThresholdImage)
#endif
        thread_status=status;
        if (thread_status == MagickFail)
          continue;

        q=GetImagePixelsEx(image,0,y,image->columns,1,&image->exception);
        if (q == (PixelPacket *) NULL)
          thread_status=MagickFail;

        if (thread_status != MagickFail)
          {
            indexes=AccessMutableIndexes(image);
            modified=MagickFalse;

            for (x=0; x < image->columns; x++)
              {
                if (grayscale)
                  intensity=q[x].red;
                else
                  intensity=PixelIntensityToQuantum(&q[x]);
                index=(intensity <= quantum_threshold ? 0U : 1U);
                if ((initialize_indexes) || (index != indexes[x]))
                  {
                    modified=MagickTrue;
                    indexes[x]=index;
                  }
                if (NotColorMatch(&image->colormap[index],&q[x]))
                  {
                    modified=MagickTrue;
                    q[x].red=q[x].green=q[x].blue=image->colormap[index].red;
                  }
              }
            if (modified)
              if (!SyncImagePixelsEx(image,&image->exception))
                thread_status=MagickFail;
          }
#if defined(HAVE_OPENMP) && !defined(DisableSlowOpenMP)
#  pragma omp critical (GM_ThresholdImage)
#endif
        {
          row_count++;
          if (QuantumTick(row_count,image->rows))
            if (!MagickMonitorFormatted(row_count,image->rows,&image->exception,
                                        ThresholdImageText,image->filename))
              thread_status=MagickFail;
          
          if (thread_status == MagickFail)
            status=MagickFail;
        }
      }
  }
  image->is_monochrome=MagickTrue;
  image->is_grayscale=MagickTrue;
  return(MagickPass);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     U n s h a r p M a s k I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  UnsharpMaskImage() sharpens an image.  We convolve the image with a
%  Gaussian operator of the given radius and standard deviation (sigma).
%  For reasonable results, radius should be larger than sigma.  Use a radius
%  of 0 and UnsharpMaskImage() selects a suitable radius for you.
%
%  The format of the UnsharpMaskImage method is:
%
%    Image *UnsharpMaskImage(const Image *image,const double radius,
%      const double sigma,const double amount,const double threshold,
%      ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%    o amount: The percentage of the difference between the original and the
%      blur image that is added back into the original.
%
%    o threshold: The threshold in pixels needed to apply the diffence amount.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

typedef struct _UnsharpMaskOptions_t
{
  double amount;    /* Difference multiplier */
  double threshold; /* Scaled to MaxRGB/2 */
} UnsharpMaskOptions_t;

static inline Quantum UnsharpQuantum(const Quantum original, const Quantum sharpened,
                                     const UnsharpMaskOptions_t* options)
{
  double
    value;

  Quantum
    quantum;

  quantum=original;
  value=original-(double) sharpened;
  if (AbsoluteValue(value) >= options->threshold)
    {
      value=original+(value*options->amount);
      quantum=RoundDoubleToQuantum(value);
    }

  return quantum;
}

static MagickPassFail
UnsharpMaskPixels(void *mutable_data,                /* User provided mutable data */
                  const void *immutable_data,        /* User provided immutable data */
                  const Image *source_image,         /* Source image */
                  const PixelPacket *source_pixels,  /* Pixel row in source image */
                  const IndexPacket *source_indexes, /* Pixel row indexes in source image */
                  Image *update_image,               /* Update image */
                  PixelPacket *update_pixels,        /* Pixel row in update image */
                  IndexPacket *update_indexes,       /* Pixel row indexes in update image */
                  const long npixels,                /* Number of pixels in row */
                  ExceptionInfo *exception           /* Exception report */
                  )
{
  const UnsharpMaskOptions_t
    *options = (const UnsharpMaskOptions_t *) immutable_data;

  register long
    i;

  ARG_NOT_USED(mutable_data);
  ARG_NOT_USED(source_image);
  ARG_NOT_USED(source_indexes);
  ARG_NOT_USED(update_image);
  ARG_NOT_USED(update_indexes);
  ARG_NOT_USED(exception);
  for (i=0; i < npixels; i++)
    {
      update_pixels[i].red=UnsharpQuantum(source_pixels[i].red,
					  update_pixels[i].red,
					  options);
      update_pixels[i].green=UnsharpQuantum(source_pixels[i].green,
					    update_pixels[i].green,
					    options);
      update_pixels[i].blue=UnsharpQuantum(source_pixels[i].blue,
					   update_pixels[i].blue,
					   options);
      update_pixels[i].opacity=UnsharpQuantum(source_pixels[i].opacity,
					      update_pixels[i].opacity,
					      options);
    }
  return MagickPass;
}

MagickExport Image *UnsharpMaskImage(const Image *image,const double radius,
  const double sigma,const double amount,const double threshold,
  ExceptionInfo *exception)
{
  UnsharpMaskOptions_t
    options;

  Image
    *sharp_image;

  char
    message[MaxTextExtent];

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  sharp_image=BlurImage(image,radius,sigma,exception);
  if (sharp_image == (Image *) NULL)
    return((Image *) NULL);
  options.amount=amount;
  options.threshold=(MaxRGBFloat*threshold)/2.0;
  FormatString(message,"[%%s] Unsharp mask: amount %g, threshold %g...",
	       amount,threshold);
  (void) PixelIterateDualModify(UnsharpMaskPixels,NULL,
                                message,NULL,&options,
                                image->columns,image->rows,image,0,0,
				sharp_image,
                                0,0,exception);                                
  sharp_image->is_grayscale=image->is_grayscale;
  return(sharp_image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     U n s h a r p M a s k I m a g e C h a n n e l                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  UnsharpMaskImageChannel() sharpens an image channel.  We convolve the
%  image channel with a Gaussian operator of the given radius and standard
%  deviation (sigma). For reasonable results, radius should be larger than
%  sigma.  Use a radius of 0 and UnsharpMaskImage() selects a suitable
%  radius for you.
%
%  The format of the UnsharpMaskImageChannel method is:
%
%    Image *UnsharpMaskImageChannel(const Image *image,
%      const ChannelType channel,const double radius,const double sigma,
%      const double amount,const double threshold,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o channel: The channel to sharpen.
%
%    o radius: The radius of the Gaussian, in pixels, not counting the center
%      pixel.
%
%    o sigma: The standard deviation of the Gaussian, in pixels.
%
%    o amount: The percentage of the difference between the original and the
%      blur image that is added back into the original.
%
%    o threshold: The threshold in pixels needed to apply the diffence amount.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport Image *UnsharpMaskImageChannel(const Image *image,
  const ChannelType channel,const double radius,const double sigma,
  const double amount,const double threshold,
  ExceptionInfo *exception)
{
  Image
    *sharp_image;

  sharp_image=UnsharpMaskImage(image,radius,sigma,amount,threshold,exception);
  if (sharp_image != (Image *) NULL)
    (void) ImportImageChannelsMasked(image,sharp_image,channel);

  return sharp_image;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%     W h i t e T h r e s h o l d I m a g e                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WhiteThresholdImage() adjusts the levels of image channels such that
%  values above a specified threshold are set to the maximum value (white)
%  while the remaining pixels are unchanged.
%
%  The format of the WhiteThresholdImage method is:
%
%      MagickPassFail WhiteThresholdImage(Image *image,const char *thresholds)
%
%  A description of each parameter follows:
%
%    o image: The image.
%
%    o thresholds: Channel thresholds which are specified as a comma delimited
%      list containing the thresholds for red, green, blue, and opacity.  If
%      the list contains a percent symbol (%) then all values are treated as
%      a percentage of MaxRGB.
%
*/
MagickExport MagickPassFail WhiteThresholdImage(Image *image,const char *thresholds)
{
  return QuantumOperatorImageMultivalue(image,ThresholdWhiteQuantumOp,thresholds);
}
