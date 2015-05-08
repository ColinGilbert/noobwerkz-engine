#
# Package name and versioning information for GraphicsMagick
#
# This file is sourced by a Bourne shell (/bin/sh) script so it must
# observe Bourne shell syntax.
#

#
# Package base name
#
PACKAGE_NAME='GraphicsMagick'

#
# Package bugs/support mailing list
#
PACKAGE_BUGREPORT='graphicsmagick-bugs@lists.sourceforge.net'

# Package base version.  This is is the numeric version suffix applied to
# PACKAGE_NAME (e.g. "1.2").
PACKAGE_VERSION='1.3.21'

#
# Package name plus version string.
#
PACKAGE_STRING="$PACKAGE_NAME $PACKAGE_VERSION"

#
# Formal Package release date
# Set to string "unreleased" if package is not a formal release.
PACKAGE_RELEASE_DATE="2015-02-28"
#PACKAGE_RELEASE_DATE="unreleased"

#
# Date of last ChangeLog update
#
PACKAGE_CHANGE_DATE=`awk '/^[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]/ { print substr($1,1,4) substr($1,6,2) substr($1,9,2); exit; }' ${srcdir}/ChangeLog`

#
# Package version addendum.  This is a suffix (if any) appended to the
# package base version.  Formal releases do not have a suffix applied.
#
if test "$PACKAGE_RELEASE_DATE" = "unreleased"
then
  PACKAGE_VERSION_ADDENDUM=".0${PACKAGE_CHANGE_DATE}"
  PACKAGE_RELEASE_DATE="snapshot-${PACKAGE_CHANGE_DATE}"
else
  PACKAGE_VERSION_ADDENDUM=''
fi

#
# Mercurial branch that this release is on.
#
HG_BRANCH_TAG=GraphicsMagick-1_3

#
# Libtool library revision control info
# See the libtool documentation under the heading "Libtool's versioning
# system" in order to understand the meaning of these fields
#
# current
#      The most recent interface number that this library implements.
# revision
#      The implementation number of the current interface.
# age
#      The difference between the newest and oldest interfaces that
#      this library implements. In other words, the library implements
#      all the interface numbers in the range from number current -
#      age to current.
#
# Here are a set of rules to help you update your library version
# information:
#
#  1. Start with version information of `0:0:0' for each libtool library.
#  2. Update the version information only immediately before a public
#     release of your software. More frequent updates are unnecessary, and
#     only guarantee that the current interface number gets larger faster.
#  3. If the library source code has changed at all since the last update,
#     then increment revision (`c:r:a' becomes `c:r+1:a').
#  4. If any interfaces have been added, removed, or changed since the last
#     update, increment current, and set revision to 0.
#  5. If any interfaces have been added since the last public release, then
#     increment age.
#  6. If any interfaces have been removed since the last public release,
#     then set age to 0.

#
# Magick library versioning
#
MAGICK_LIBRARY_CURRENT=16

MAGICK_LIBRARY_REVISION=0
MAGICK_LIBRARY_AGE=13

#
# Magick++ library versioning
#
MAGICK_PLUS_PLUS_LIBRARY_CURRENT=11
MAGICK_PLUS_PLUS_LIBRARY_REVISION=0
MAGICK_PLUS_PLUS_LIBRARY_AGE=0

#
# Magick Wand library versioning
#
MAGICK_WAND_LIBRARY_CURRENT=9
MAGICK_WAND_LIBRARY_REVISION=1
MAGICK_WAND_LIBRARY_AGE=7
