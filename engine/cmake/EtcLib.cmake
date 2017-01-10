if (DEFINED NOOB_USE_ETCLIB)
	
	include_directories(lib/etc2comp/EtcLib/Etc)
	include_directories(lib/etc2comp/EtcLib/Etcodec)

	list (APPEND main_src
		lib/etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_ETC1.cpp
		lib/etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_RGBA8.cpp
		lib/etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_RGB8A1.cpp
		lib/etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_RG11.cpp
		lib/etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_RGB8.cpp
		lib/etc2comp/EtcLib/EtcCodec/EtcSortedBlockList.cpp
		lib/etc2comp/EtcLib/EtcCodec/EtcBlock4x4.cpp
		lib/etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding.cpp
		lib/etc2comp/EtcLib/EtcCodec/EtcDifferentialTrys.cpp
		lib/etc2comp/EtcLib/EtcCodec/EtcIndividualTrys.cpp
		lib/etc2comp/EtcLib/EtcCodec/EtcBlock4x4Encoding_R11.cpp
		lib/etc2comp/EtcLib/Etc/EtcImage.cpp
		lib/etc2comp/EtcLib/Etc/Etc.cpp
		lib/etc2comp/EtcLib/Etc/EtcMath.cpp
		)
endif()
