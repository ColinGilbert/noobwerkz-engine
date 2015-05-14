// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2001, 2002
//
// Implementation of ImageRef
//
// This is an internal implementation class.
//

#define MAGICK_IMPLEMENTATION
#define MAGICK_PLUSPLUS_IMPLEMENTATION

#include "Magick++/ImageRef.h"
#include "Magick++/Exception.h"
#include "Magick++/Options.h"

// Construct with an image and default options
Magick::ImageRef::ImageRef ( MagickLib::Image * image_ )
  : _image(image_),
    _options(new Options),
    _id(-1),
    _refCount(1),
    _mutexLock()
{
}

// Construct with an image and options
// Inserts Image* in image, but copies Options into image.
Magick::ImageRef::ImageRef ( MagickLib::Image * image_,
			     const Options * options_ )
  : _image(image_),
    _options(0),
    _id(-1),
    _refCount(1),
    _mutexLock()
{
  _options = new Options( *options_ );
}

// Default constructor
Magick::ImageRef::ImageRef ( void )
  : _image(0),
    _options(new Options),
    _id(-1),
    _refCount(1),
    _mutexLock()
{
  // Allocate default image
  _image = AllocateImage( _options->imageInfo() );

  // Test for error and throw exception (like throwImageException())
  throwException(_image->exception);
}

// Destructor
Magick::ImageRef::~ImageRef( void )
{
  // Unregister image (if still registered)
  if( _id > -1 )
    {
      DeleteMagickRegistry( _id );
      _id=-1;
    }

  // Deallocate image
  if ( _image )
    {
      DestroyImageList( _image );
      _image = 0;
    }

  // Deallocate image options
  delete _options;
  _options = 0;
}

// Assign image to reference
void Magick::ImageRef::image ( MagickLib::Image * image_ )
{
  if(_image)
    DestroyImageList( _image );
  _image = image_;
}

// Assign options to reference
void  Magick::ImageRef::options ( Magick::Options * options_ )
{
  delete _options;
  _options = options_;
}

// Assign registration id to reference
void Magick::ImageRef::id ( const long id_ )
{
  if( _id > -1 )
    DeleteMagickRegistry( _id );
  _id = id_;
}
