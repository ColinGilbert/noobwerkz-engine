include_directories(lib/freetype/include)

list (APPEND main_src
	lib/freetype/src/psaux/psconv.c
	lib/freetype/src/psaux/afmparse.c
	lib/freetype/src/psaux/t1decode.c
	lib/freetype/src/psaux/psauxmod.c
	lib/freetype/src/psaux/t1cmap.c
	lib/freetype/src/psaux/psobjs.c
	lib/freetype/src/autofit/afblue.c
	lib/freetype/src/autofit/afshaper.c
	lib/freetype/src/autofit/afmodule.c
	lib/freetype/src/autofit/afpic.c
	lib/freetype/src/autofit/afcjk.c
	lib/freetype/src/autofit/afindic.c
	lib/freetype/src/autofit/aflatin.c
	lib/freetype/src/autofit/afglobal.c
	lib/freetype/src/autofit/afdummy.c
	lib/freetype/src/autofit/afloader.c
	lib/freetype/src/autofit/afranges.c
	lib/freetype/src/autofit/afangles.c
	lib/freetype/src/autofit/afwarp.c
	lib/freetype/src/autofit/afhints.c
	lib/freetype/src/sfnt/ttkern.c
	lib/freetype/src/sfnt/ttpost.c
	lib/freetype/src/sfnt/sfobjs.c
	lib/freetype/src/sfnt/pngshim.c
	lib/freetype/src/sfnt/ttload.c
	lib/freetype/src/sfnt/ttmtx.c
	lib/freetype/src/sfnt/ttsbit.c
	lib/freetype/src/sfnt/ttcmap.c
	lib/freetype/src/sfnt/ttbdf.c
	lib/freetype/src/sfnt/sfdriver.c
	lib/freetype/src/sfnt/sfntpic.c
	lib/freetype/src/pfr/pfrload.c
	lib/freetype/src/pfr/pfrcmap.c
	lib/freetype/src/pfr/pfrsbit.c
	lib/freetype/src/pfr/pfrdrivr.c
	lib/freetype/src/pfr/pfrobjs.c
	lib/freetype/src/pfr/pfrgload.c
	lib/freetype/src/bdf/bdfdrivr.c
	lib/freetype/src/bdf/bdflib.c
	lib/freetype/src/psnames/psmodule.c
	lib/freetype/src/psnames/pspic.c
	lib/freetype/src/raster/ftrend1.c
	lib/freetype/src/raster/rastpic.c
	lib/freetype/src/raster/ftraster.c
	lib/freetype/src/pcf/pcfutil.c
	lib/freetype/src/pcf/pcfdrivr.c
	lib/freetype/src/pcf/pcfread.c
	lib/freetype/src/otvalid/otvcommn.c
	lib/freetype/src/otvalid/otvjstf.c
	lib/freetype/src/otvalid/otvgsub.c
	lib/freetype/src/otvalid/otvmod.c
	lib/freetype/src/otvalid/otvbase.c
	lib/freetype/src/otvalid/otvgdef.c
	lib/freetype/src/otvalid/otvgpos.c
	lib/freetype/src/otvalid/otvmath.c
	lib/freetype/src/type42/t42objs.c
	lib/freetype/src/type42/t42parse.c
	lib/freetype/src/type42/t42drivr.c
	lib/freetype/src/smooth/ftspic.c
	lib/freetype/src/smooth/ftsmooth.c
	lib/freetype/src/smooth/ftgrays.c
	lib/freetype/src/cache/ftcmru.c
	lib/freetype/src/cache/ftcglyph.c
	lib/freetype/src/cache/ftccache.c
	lib/freetype/src/cache/ftcimage.c
	lib/freetype/src/cache/ftcbasic.c
	lib/freetype/src/cache/ftcmanag.c
	lib/freetype/src/cache/ftcsbits.c
	lib/freetype/src/cache/ftccmap.c
	lib/freetype/src/pshinter/pshalgo.c
	lib/freetype/src/pshinter/pshrec.c
	lib/freetype/src/pshinter/pshglob.c
	lib/freetype/src/pshinter/pshpic.c
	lib/freetype/src/pshinter/pshmod.c
	lib/freetype/src/type1/t1driver.c
	lib/freetype/src/type1/t1objs.c
	lib/freetype/src/type1/t1afm.c
	lib/freetype/src/type1/t1gload.c
	lib/freetype/src/type1/t1load.c
	lib/freetype/src/type1/t1parse.c
	lib/freetype/src/bzip2/ftbzip2.c
	lib/freetype/src/winfonts/winfnt.c
	lib/freetype/src/base/ftsnames.c
	lib/freetype/src/base/ftotval.c
	lib/freetype/src/base/fttype1.c
	lib/freetype/src/base/ftapi.c
	lib/freetype/src/base/ftoutln.c
	lib/freetype/src/base/ftinit.c
	lib/freetype/src/base/ftgasp.c
	lib/freetype/src/base/ftpic.c
	lib/freetype/src/base/ftfntfmt.c
	lib/freetype/src/base/ftfstype.c
	lib/freetype/src/base/ftobjs.c
	lib/freetype/src/base/md5.c
	lib/freetype/src/base/ftstream.c
	lib/freetype/src/base/ftpfr.c
	lib/freetype/src/base/ftwinfnt.c
	lib/freetype/src/base/ftcid.c
	lib/freetype/src/base/ftadvanc.c
	lib/freetype/src/base/ftsynth.c
	lib/freetype/src/base/ftdebug.c
	lib/freetype/src/base/ftgxval.c
	lib/freetype/src/base/ftbitmap.c
	lib/freetype/src/base/ftcalc.c
	lib/freetype/src/base/ftpatent.c
	lib/freetype/src/base/ftglyph.c
	lib/freetype/src/base/ftgloadr.c
	lib/freetype/src/base/ftmm.c
	lib/freetype/src/base/ftbdf.c
	lib/freetype/src/base/ftstroke.c
	lib/freetype/src/base/ftrfork.c
	lib/freetype/src/base/ftutil.c
	lib/freetype/src/base/ftsystem.c
	lib/freetype/src/base/fthash.c
	lib/freetype/src/base/basepic.c
	lib/freetype/src/base/ftlcdfil.c
	lib/freetype/src/base/ftbbox.c
	lib/freetype/src/base/ftdbgmem.c
	lib/freetype/src/base/fttrigon.c
	lib/freetype/src/lzw/ftlzw.c
	lib/freetype/src/lzw/ftzopen.c	
	lib/freetype/src/truetype/ttdriver.c
	lib/freetype/src/truetype/ttgload.c
	lib/freetype/src/truetype/ttobjs.c
	lib/freetype/src/truetype/ttpload.c
	lib/freetype/src/truetype/ttinterp.c
	lib/freetype/src/truetype/ttpic.c
	lib/freetype/src/truetype/ttgxvar.c
	lib/freetype/src/truetype/ttsubpix.c
	lib/freetype/src/cff/cf2font.c
	lib/freetype/src/cff/cffload.c
	lib/freetype/src/cff/cffgload.c
	lib/freetype/src/cff/cf2hints.c
	lib/freetype/src/cff/cf2error.c
	lib/freetype/src/cff/cf2read.c
	lib/freetype/src/cff/cf2arrst.c
	lib/freetype/src/cff/cf2ft.c
	lib/freetype/src/cff/cffdrivr.c
	lib/freetype/src/cff/cf2intrp.c
	lib/freetype/src/cff/cf2blues.c
	lib/freetype/src/cff/cffparse.c
	lib/freetype/src/cff/cffcmap.c
	lib/freetype/src/cff/cf2stack.c
	lib/freetype/src/cff/cffobjs.c
	lib/freetype/src/cff/cffpic.c
	lib/freetype/src/cid/cidgload.c
	lib/freetype/src/cid/cidload.c
	lib/freetype/src/cid/cidobjs.c
	lib/freetype/src/cid/cidriver.c
	lib/freetype/src/cid/cidparse.c
	)
#########################
#
# Consciously excluded:
#
#########################
#
# lib/freetype/builds/wince/ftdebug.c
# lib/freetype/builds/amiga/src/base/ftdebug.c
# lib/freetype/builds/amiga/src/base/ftsystem.c
# lib/freetype/builds/mac/ftmac.c
# lib/freetype/builds/windows/ftdebug.c
# lib/freetype/builds/vms/ftsystem.c
# lib/freetype/builds/unix/ftsystem.c
# lib/freetype/src/psaux/psaux.c
# lib/freetype/src/autofit/autofit.c
# lib/freetype/src/autofit/aflatin2.c
# lib/freetype/src/pfr/pfr.c
# lib/freetype/src/sfnt/sfnt.c
# lib/freetype/src/bdf/bdf.c
# lib/freetype/src/psnames/psnames.c
# lib/freetype/src/raster/raster.c
# lib/freetype/src/pcf/pcf.c
# lib/freetype/src/otvalid/otvalid.c
# lib/freetype/src/type42/type42.c
# lib/freetype/src/smooth/smooth.c
# lib/freetype/src/cache/ftcache.c
# lib/freetype/src/pshinter/pshinter.c
# lib/freetype/src/type1/type1.c
# lib/freetype/src/tools/test_bbox.c
# lib/freetype/src/tools/ftfuzzer/rasterfuzzer.cc
# lib/freetype/src/tools/ftfuzzer/runinput.cc
# lib/freetype/src/tools/ftfuzzer/ftmutator.cc
# lib/freetype/src/tools/ftfuzzer/ftfuzzer.cc
# lib/freetype/src/tools/test_trig.c
# lib/freetype/src/tools/apinames.c
# lib/freetype/src/tools/test_afm.c
# lib/freetype/src/tools/ftrandom/ftrandom.c
# lib/freetype/src/truetype/truetype.c
# lib/freetype/src/cff/cff.c
# lib/freetype/src/cid/type1cid.c
#
# These gave trouble too:
# lib/freetype/src/gzip/inflate.c
# lib/freetype/src/gzip/adler32.c
# lib/freetype/src/gzip/infcodes.c
# lib/freetype/src/gzip/infblock.c
# lib/freetype/src/gzip/ftgzip.c
# lib/freetype/src/gzip/inftrees.c
# lib/freetype/src/gzip/infutil.c
# lib/freetype/src/gzip/zutil.c
