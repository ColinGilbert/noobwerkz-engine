include_directories(lib/libvorbis/include)
include_directories(lib/libvorbis/lib)
include_directories(lib/libvorbis/lib/modes)
include_directories(lib/libvorbis/lib/books/coupled)
include_directories(lib/libvorbis/lib/books/uncoupled)
include_directories(lib/libvorbis/lib/books/floor)

list (APPEND main_src
	lib/libvorbis/lib/codebook.c
	lib/libvorbis/lib/psytune.c
	lib/libvorbis/lib/block.c
	lib/libvorbis/lib/bitrate.c
	lib/libvorbis/lib/lookup.c
	lib/libvorbis/lib/envelope.c
	lib/libvorbis/lib/lsp.c
	lib/libvorbis/lib/vorbisenc.c
	lib/libvorbis/lib/floor0.c
	lib/libvorbis/lib/psy.c
	lib/libvorbis/lib/mapping0.c
	lib/libvorbis/lib/vorbisfile.c
	lib/libvorbis/lib/smallft.c
	lib/libvorbis/lib/window.c
	lib/libvorbis/lib/floor1.c
	lib/libvorbis/lib/sharedbook.c
	lib/libvorbis/lib/mdct.c
	lib/libvorbis/lib/lpc.c
	lib/libvorbis/lib/analysis.c
	lib/libvorbis/lib/info.c
	lib/libvorbis/lib/res0.c
	lib/libvorbis/lib/synthesis.c
	lib/libvorbis/lib/registry.c
	)
#########################
#
# Consciously excluded:
#
#########################
#
# lib/libvorbis/lib/tone.c	
# lib/libvorbis/lib/barkmel.c
# lib/libvorbis/vq/latticetune.c
# lib/libvorbis/vq/huffbuild.c
# lib/libvorbis/vq/distribution.c
# lib/libvorbis/vq/metrics.c
# lib/libvorbis/vq/latticebuild.c
# lib/libvorbis/vq/vqgen.c
# lib/libvorbis/vq/bookutil.c



