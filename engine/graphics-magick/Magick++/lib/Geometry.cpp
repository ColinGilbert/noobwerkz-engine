// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999 - 2014
//
// Geometry implementation
//

#define MAGICK_IMPLEMENTATION
#define MAGICK_PLUSPLUS_IMPLEMENTATION

#include "Magick++/Include.h"
#include <string>
#include <ctype.h> // for isdigit
#include <string.h> // for strcpy
#include <cassert>

using namespace std;

#include "Magick++/Geometry.h"
#include "Magick++/Exception.h"

#define AbsoluteValue(x)  ((x) < 0 ? -(x) : (x))

// Does object contain valid geometry?
void Magick::Geometry::isValid ( bool isValid_ )
{
  _flags._b._isValid = isValid_;
}

bool Magick::Geometry::isValid ( void ) const
{
  return _flags._b._isValid;
}

// Width
void Magick::Geometry::width ( unsigned int width_ )
{
  _width = width_;
  isValid( true );
}
unsigned int Magick::Geometry::width ( void ) const
{
  return _width;
}

// Height
void Magick::Geometry::height ( unsigned int height_ )
{
  _height = height_;
}
unsigned int Magick::Geometry::height ( void ) const
{
  return _height;
}

// X offset from origin
void Magick::Geometry::xOff ( unsigned int xOff_ )
{
  _xOff = xOff_;
}
unsigned int Magick::Geometry::xOff ( void ) const
{
  return _xOff;
}

// Y offset from origin
void Magick::Geometry::yOff ( unsigned int yOff_ )
{
  _yOff = yOff_;
}
unsigned int Magick::Geometry::yOff ( void ) const
{
  return _yOff;
}

// Sign of X offset negative? (X origin at right)
void Magick::Geometry::xNegative ( bool xNegative_ )
{
  _flags._b._xNegative = xNegative_;
}
bool Magick::Geometry::xNegative ( void ) const
{
  return _flags._b._xNegative;
}

// Sign of Y offset negative? (Y origin at bottom)
void Magick::Geometry::yNegative ( bool yNegative_ )
{
  _flags._b._yNegative = yNegative_;
}
bool Magick::Geometry::yNegative ( void ) const
{
  return _flags._b._yNegative;
}

// Interpret width & height as percentages (%)
void Magick::Geometry::percent ( bool percent_ )
{
  _flags._b._percent = percent_;
}
bool Magick::Geometry::percent ( void ) const
{
  return _flags._b._percent;
}

// Resize without preserving aspect ratio (!)
void Magick::Geometry::aspect ( bool aspect_ )
{
  _flags._b._aspect = aspect_;
}
bool Magick::Geometry::aspect ( void ) const
{
  return _flags._b._aspect;
}

// Resize if image is greater than size (>)
void Magick::Geometry::greater ( bool greater_ )
{
  _flags._b._greater = greater_;
}
bool Magick::Geometry::greater ( void ) const
{
  return _flags._b._greater;
}

// Resize if image is less than size (<)
void Magick::Geometry::less ( bool less_ )
{
  _flags._b._less = less_;
}
bool Magick::Geometry::less ( void ) const
{
  return _flags._b._less;
}

// Resize image to fit total pixel area specified by dimensions (@).
void Magick::Geometry::limitPixels ( bool limitPixels_ )
{
  _flags._b._limitPixels = limitPixels_;
}
bool Magick::Geometry::limitPixels ( void ) const
{
  return _flags._b._limitPixels;
}

// Resize image to fit a set of dimensions
void Magick::Geometry::fillArea ( bool fillArea_ )
{
  _flags._b._fillArea = fillArea_;
}
bool Magick::Geometry::fillArea ( void ) const
{
  return _flags._b._fillArea;
}


int Magick::operator == ( const Magick::Geometry& left_,
			  const Magick::Geometry& right_ )
{
  return (
	  ( left_.isValid()     == right_.isValid() ) &&
	  ( left_.width()       == right_.width() ) &&
	  ( left_.height()      == right_.height() ) &&
	  ( left_.xOff()        == right_.xOff() ) &&
	  ( left_.yOff()        == right_.yOff() ) &&
	  ( left_.xNegative()   == right_.xNegative() ) &&
	  ( left_.yNegative()   == right_.yNegative() ) &&
	  ( left_.percent()     == right_.percent() ) &&
	  ( left_.aspect()      == right_.aspect() ) &&
	  ( left_.greater()     == right_.greater() ) &&
	  ( left_.less()        == right_.less() ) &&
	  ( left_.limitPixels() == right_.limitPixels() ) &&
          ( left_.fillArea()    == right_.fillArea() )
	  );
}
int Magick::operator != ( const Magick::Geometry& left_,
			  const Magick::Geometry& right_ )
{
  return ( ! (left_ == right_) );
}
int Magick::operator >  ( const Magick::Geometry& left_,
			  const Magick::Geometry& right_ )
{
  return ( !( left_ < right_ ) && ( left_ != right_ ) );
}
int Magick::operator <  ( const Magick::Geometry& left_,
			  const Magick::Geometry& right_ )
{
  return (
	  ( left_.width() * left_.height() )
	  <
	  ( right_.width() * right_.height() )
	  );
}
int Magick::operator >= ( const Magick::Geometry& left_,
			  const Magick::Geometry& right_ )
{
  return ( ( left_ > right_ ) || ( left_ == right_ ) );
}
int Magick::operator <= ( const Magick::Geometry& left_,
			  const Magick::Geometry& right_ )
{
  return ( ( left_ < right_ ) || ( left_ == right_ ) );
}

// Construct using parameterized arguments
Magick::Geometry::Geometry ( unsigned int width_,
			     unsigned int height_,
			     unsigned int xOff_,
			     unsigned int yOff_,
			     bool xNegative_,
			     bool yNegative_ )
  : _width( width_ ),
    _height( height_ ),
    _xOff( xOff_ ),
    _yOff( yOff_ )
{
  xNegative( xNegative_ );
  yNegative( yNegative_ );
  isValid( true );
  percent( false );
  aspect( false );
  greater( false );
  less( false );
  limitPixels (false );
  fillArea( false );
}

// Assignment from C++ string
Magick::Geometry::Geometry ( const std::string &geometry_ )
  : _width( 0 ),
    _height( 0 ),
    _xOff( 0 ),
    _yOff( 0 )
{
  assert(sizeof(_flags._padding) >= sizeof(_flags._b));
  xNegative( false );
  yNegative( false );
  isValid( false );
  percent( false );
  aspect( false );
  greater( false );
  less( false );
  limitPixels (false );
  fillArea( false );
  *this = geometry_; // Use assignment operator
}


// Assignment from C character string
Magick::Geometry::Geometry ( const char *geometry_ )
  : _width( 0 ),
    _height( 0 ),
    _xOff( 0 ),
    _yOff( 0 )
{
  xNegative( false );
  yNegative( false );
  isValid( false );
  percent( false );
  aspect( false );
  greater( false );
  less( false );
  limitPixels ( false );
  fillArea( false );
  *this = geometry_; // Use assignment operator
}

// Copy constructor
Magick::Geometry::Geometry ( const Geometry &geometry_ )
  :  _width( geometry_._width ),
     _height( geometry_._height ),
     _xOff( geometry_._xOff ),
     _yOff( geometry_._yOff )
{
  xNegative( geometry_.xNegative() );
  yNegative( geometry_.yNegative() );
  isValid ( geometry_.isValid() );
  percent( geometry_.percent() );
  aspect( geometry_.aspect() );
  greater( geometry_.greater() );
  less( geometry_.less() );
  limitPixels( geometry_.limitPixels() );
  fillArea( geometry_.fillArea() );
}

// Default constructor
Magick::Geometry::Geometry ( void )
  : _width( 0 ),
    _height( 0 ),
    _xOff( 0 ),
    _yOff( 0 )
{
  assert(sizeof(_flags._padding) >= sizeof(_flags._b));
  xNegative( false );
  yNegative( false );
  isValid ( false );
  percent( false );
  aspect( false );
  greater( false );
  less( false );
  limitPixels( false );
  fillArea( false );
}

/* virtual */ Magick::Geometry::~Geometry ( void )
{
  // Nothing to do
}

Magick::Geometry& Magick::Geometry::operator = ( const Geometry& geometry_ )
{
  // If not being set to ourself
  if ( this != &geometry_ )
    {
      _width = geometry_._width;
      _height = geometry_._height;
      _xOff = geometry_._xOff;
      _yOff = geometry_._yOff;

      xNegative( geometry_.xNegative() );
      yNegative( geometry_.yNegative() );
      isValid ( geometry_.isValid() );
      percent( geometry_.percent() );
      aspect( geometry_.aspect() );
      greater( geometry_.greater() );
      less( geometry_.less() );
      limitPixels( geometry_.limitPixels() );
      fillArea( geometry_.fillArea() );
    }
  return *this;
}

// Set value via geometry string
/* virtual */ const Magick::Geometry&
Magick::Geometry::operator = ( const std::string &geometry_ )
{
  char
    geom[MaxTextExtent];

  // If argument does not start with digit, presume that it is a
  // page-size specification that needs to be converted to an
  // equivalent geometry specification using PostscriptGeometry()
  strcpy(geom,geometry_.c_str());
  if ( geom[0] != '-' &&
       geom[0] != '+' &&
       geom[0] != 'x' &&
       !isdigit(static_cast<int>(geom[0])))
    {
      char *pageptr = GetPageGeometry( geom );
      if ( pageptr != 0 )
	{
	  strcpy(geom,pageptr);
          MagickFreeMemory(pageptr);
	}
    }

  long x = 0;
  long y = 0;
  unsigned long width_val = 0;
  unsigned long height_val = 0;
  int flags = GetGeometry (geom, &x, &y, &width_val, &height_val );

  if (flags == NoValue)
    {
      // Total failure!
      *this=Geometry();
      isValid( false );
      return *this;
    }

  if ( ( flags & WidthValue ) != 0 )
    {
      width( width_val );
      isValid( true );
    }

  if ( ( flags & HeightValue ) != 0 )
    height( height_val );

  if ( ( flags & XValue ) != 0 )
    {
      xOff( static_cast<unsigned int>(AbsoluteValue(x)) );
      isValid( true );
    }

  if ( ( flags & YValue ) != 0 )
    {
      yOff( static_cast<unsigned int>(AbsoluteValue(y)) );
      isValid( true );
    }

  if ( ( flags & XNegative ) != 0 )
    xNegative( true );

  if ( ( flags & YNegative ) != 0 )
    yNegative( true );

  if ( ( flags & PercentValue ) != 0 ) // '%'
    percent( true );

  if ( ( flags & AspectValue ) != 0 )  // '!'
    aspect( true );

  if ( ( flags & LessValue ) != 0 )    // '<'
    less( true );

  if ( ( flags & GreaterValue ) != 0 ) // '>'
    greater( true );

  if ( ( flags & AreaValue ) != 0 )    // '@'
    limitPixels( true );

  if ( ( flags & MinimumValue ) != 0 ) // '^'
    fillArea( true );

  return *this;
}


// Set value via geometry C string
/* virtual */ const Magick::Geometry& Magick::Geometry::operator = ( const char * geometry_ )
{
  *this = std::string(geometry_);
  return *this;
}

// Return geometry string
Magick::Geometry::operator std::string() const
{
  if (!isValid())
    {
      throwExceptionExplicit( OptionError, "Invalid geometry argument" );
    }

  string geometry;
  char buffer[32];

  if ( _width )
    {
      FormatString( buffer, "%u", _width );
      geometry += buffer;
    }

  if ( _width && _height )
    {
      FormatString( buffer, "%u",  _height);
      geometry += 'x';
      geometry +=  buffer;
    }

  if ( _xOff || _yOff )
    {
      if ( xNegative() )
        geometry += '-';
      else
        geometry += '+';

      FormatString( buffer, "%u", _xOff);
      geometry += buffer;

      if ( yNegative() )
        geometry += '-';
      else
        geometry += '+';

      FormatString( buffer, "%u", _yOff);
      geometry += buffer;
    }

  if ( percent() )
    geometry += '%';

  if ( aspect() )
    geometry += '!';

  if ( greater() )
    geometry += '>';

  if ( less() )
    geometry += '<';

  if ( limitPixels() )
    geometry += '@';

  if ( fillArea() )
    geometry += '^';

  return geometry;
}

// Construct from RectangleInfo
Magick::Geometry::Geometry ( const MagickLib::RectangleInfo &rectangle_ )
  : _width(static_cast<unsigned int>(rectangle_.width)),
    _height(static_cast<unsigned int>(rectangle_.height)),
    _xOff(static_cast<unsigned int>(AbsoluteValue(rectangle_.x))),
    _yOff(static_cast<unsigned int>(AbsoluteValue(rectangle_.y)))
{
  xNegative(rectangle_.x < 0 ? true : false);
  yNegative(rectangle_.y < 0 ? true : false);
  isValid(true);
  percent(false);
  aspect(false);
  greater(false);
  less(false);
  limitPixels(false);
  fillArea(false);
}

// Return an ImageMagick RectangleInfo struct
Magick::Geometry::operator MagickLib::RectangleInfo() const
{
  RectangleInfo rectangle;
  rectangle.width = _width;
  rectangle.height = _height;
  xNegative() ? rectangle.x = static_cast<long>(0-_xOff) : rectangle.x = static_cast<long>(_xOff);
  yNegative() ? rectangle.y = static_cast<long>(0-_yOff) : rectangle.y = static_cast<long>(_yOff);
  return rectangle;
}
