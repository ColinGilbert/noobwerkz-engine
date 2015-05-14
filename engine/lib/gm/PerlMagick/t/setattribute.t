#!/usr/local/bin/perl
# Copyright (C) 2003 GraphicsMagick Group
# Copyright (C) 2002 ImageMagick Studio
# Copyright (C) 1991-1999 E. I. du Pont de Nemours and Company
#
# This program is covered by multiple licenses, which are described in
# Copyright.txt. You should have received a copy of Copyright.txt with this
# package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
#
#
# Test setting & getting attributes.
#
# Contributed by Bob Friesenhahn <bfriesen@simple.dallas.tx.us>
#
BEGIN { $| = 1; $test=1, print "1..71\n"; }
END {print "not ok 1\n" unless $loaded;}
use Graphics::Magick;
$loaded=1;

require 't/subroutines.pl';

chdir 't' || die 'Cd failed';

# Determine if QuantumMagick is defined
$image=Graphics::Magick->new;
my $depth = $image->Get('depth');

testSetAttribute('input.miff','adjoin','True');

++$test;
testSetAttribute('input.miff','adjoin','False');

++$test;
testSetAttribute('input.miff','antialias','True');

++$test;
testSetAttribute('input.miff','antialias','False');

++$test;
testSetAttribute('input.miff','compression','None');

++$test;
testSetAttribute('input.miff','compression','JPEG');

++$test;
testSetAttribute('input.miff','compression','LZW');

++$test;
testSetAttribute('input.miff','compression','RLE');

++$test;
testSetAttribute('input.miff','compression','Zip');

++$test;
testSetAttribute('input.miff','density','72x72');

++$test;
testSetAttribute('input.miff','dispose','Undefined');

++$test;
testSetAttribute('input.miff','dispose','None');

++$test;
testSetAttribute('input.miff','dispose','Background');

++$test;
testSetAttribute('input.miff','dispose','Previous');

++$test;
testSetAttribute('input.miff','delay',100);

++$test;
testSetAttribute('input.miff','dither','True');

++$test;
testSetAttribute('input.miff','dither','False');

++$test;
testSetAttribute('input.miff','display','bogus:0.0');

++$test;
testSetAttribute('input.miff','filename','bogus.jpg');

++$test;
testSetAttribute('input.miff','font',q/-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-*/);

++$test;
testSetAttribute('input.miff','iterations',10);

++$test;
testSetAttribute('input.miff','interlace','None');

++$test;
testSetAttribute('input.miff','interlace','Line');

++$test;
testSetAttribute('input.miff','interlace','Plane');

++$test;
testSetAttribute('input.miff','interlace','Partition');

++$test;
testSetAttribute('input.miff','loop',100);

++$test;
testSetAttribute('input.miff','magick','TIFF');

++$test;
testSetAttribute('input.miff','monochrome','True');

++$test;
testSetAttribute('input.miff','monochrome','False');

++$test;
testSetAttribute('input.miff','page','595x842>+0+0');

++$test;
testSetAttribute('input.miff','pointsize',12);

++$test;
testSetAttribute('input.miff','preview','Rotate');

++$test;
testSetAttribute('input.miff','preview','Shear');

++$test;
testSetAttribute('input.miff','preview','Roll');

++$test;
testSetAttribute('input.miff','preview','Hue');

++$test;
testSetAttribute('input.miff','preview','Saturation');

++$test;
testSetAttribute('input.miff','preview','Brightness');

++$test;
testSetAttribute('input.miff','preview','JPEG');

++$test;
testSetAttribute('input.miff','preview','Spiff');

++$test;
testSetAttribute('input.miff','preview','Dull');

++$test;
testSetAttribute('input.miff','preview','Grayscale');

++$test;
testSetAttribute('input.miff','preview','Quantize');

++$test;
testSetAttribute('input.miff','preview','Despeckle');

++$test;
testSetAttribute('input.miff','preview','ReduceNoise');

++$test;
testSetAttribute('input.miff','preview','AddNoise');

++$test;
testSetAttribute('input.miff','preview','Sharpen');

++$test;
testSetAttribute('input.miff','preview','Blur');

++$test;
testSetAttribute('input.miff','preview','Threshold');

++$test;
testSetAttribute('input.miff','preview','EdgeDetect');

++$test;
testSetAttribute('input.miff','preview','Spread');

++$test;
testSetAttribute('input.miff','preview','Solarize');

++$test;
testSetAttribute('input.miff','preview','Shade');

++$test;
testSetAttribute('input.miff','preview','Raise');

++$test;
testSetAttribute('input.miff','preview','Segment');

++$test;
testSetAttribute('input.miff','preview','Solarize');

++$test;
testSetAttribute('input.miff','preview','Swirl');

++$test;
testSetAttribute('input.miff','preview','Implode');

++$test;
testSetAttribute('input.miff','preview','Wave');

++$test;
testSetAttribute('input.miff','preview','OilPaint');

++$test;
testSetAttribute('input.miff','preview','Charcoal');

++$test;
testSetAttribute('input.miff','quality',25);

++$test;
testSetAttribute('input.miff','scene',5);

++$test;
testSetAttribute('input.miff','subimage',9);

++$test;
testSetAttribute('input.miff','subrange',16);

++$test;
testSetAttribute('input.miff','server','mymachine:0.0');

++$test;
testSetAttribute('input.miff','size','25x25');

++$test;
testSetAttribute('input.miff','size','25x25');

# I have no idea what this does
++$test;
testSetAttribute('input.miff','tile','some value');

++$test;
testSetAttribute('input.miff','texture','granite:');

++$test;
testSetAttribute('input.miff','verbose','True');

++$test;
testSetAttribute('input.miff','verbose','False');

