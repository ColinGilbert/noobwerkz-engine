cd ../engine/lib/bgfx &&
make linux-release64 -j8 &&
cp .build/linux64_gcc/bin/shadercRelease ../../../sandbox/shaderc &&
cp examples/common/font/*.bin.h ../../common/drawing/font # &&
# cp examples/common/font/*.sc ../../common/drawing/font
