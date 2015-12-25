
all:
	@echo "----------------------------------------------------------------------------------"
	@echo "  make x11              to compile MMDAgent for X11 on Unix-like systems"
	@echo "  make x11-clean        to remove any files generated for X11 on Unix-like systems"
	@echo "----------------------------------------------------------------------------------"
	@echo "  make cocoa            to compile MMDAgent for Cocoa on Mac OS X"
	@echo "  make cocoa-clean      to remove any files generated for Cocoa on Mac OS X"
	@echo "----------------------------------------------------------------------------------"
	@echo "  make android          to compile MMDAgent for Android"
	@echo "  make android-clean    to remove any files generated for Android"
	@echo "----------------------------------------------------------------------------------"

x11:
	cd Library_Bullet_Physics && make
	cd Library_GLFW           && make
	cd Library_GLee           && make
	cd Library_JPEG           && make
	cd Library_libpng         && make
	cd Library_FreeType       && make
	cd Library_MMDAgent       && make
	cd Library_MMDFiles       && make
	cd Library_zlib           && make
	cd Library_PortAudio      && make
	cd Library_RE2            && make
	cd Library_hts_engine_API && make
	cd Library_Open_JTalk     && make
	cd Library_Julius         && make
	cd main                   && make
	cd Plugin_VIManager       && make
	cd Plugin_Variables       && make
	cd Plugin_LookAt          && make
	cd Plugin_Open_JTalk      && make
	cd Plugin_Julius          && make

x11-clean:
	cd Library_Bullet_Physics && make clean
	cd Library_GLFW           && make clean
	cd Library_GLee           && make clean
	cd Library_JPEG           && make clean
	cd Library_libpng         && make clean
	cd Library_FreeType       && make clean
	cd Library_MMDAgent       && make clean
	cd Library_MMDFiles       && make clean
	cd Library_zlib           && make clean
	cd Library_PortAudio      && make clean
	cd Library_RE2            && make clean
	cd Library_hts_engine_API && make clean
	cd Library_Open_JTalk     && make clean
	cd Library_Julius         && make clean
	cd main                   && make clean
	cd Plugin_VIManager       && make clean
	cd Plugin_Variables       && make clean
	cd Plugin_LookAt          && make clean
	cd Plugin_Open_JTalk      && make clean
	cd Plugin_Julius          && make clean

cocoa:
	cd Library_Bullet_Physics && make
	cd Library_GLFW           && make -f Makefile.cocoa
	cd Library_GLee           && make
	cd Library_JPEG           && make
	cd Library_libpng         && make
	cd Library_FreeType       && make
	cd Library_MMDAgent       && make
	cd Library_MMDFiles       && make
	cd Library_zlib           && make
	cd Library_PortAudio      && make -f Makefile.cocoa
	cd Library_RE2            && make
	cd Library_hts_engine_API && make
	cd Library_Open_JTalk     && make
	cd Library_Julius         && make
	cd main                   && make -f Makefile.cocoa
	cd Plugin_VIManager       && make -f Makefile.cocoa
	cd Plugin_Variables       && make -f Makefile.cocoa
	cd Plugin_Audio           && make -f Makefile.cocoa
	cd Plugin_LookAt          && make -f Makefile.cocoa
	cd Plugin_Open_JTalk      && make -f Makefile.cocoa
	cd Plugin_Julius          && make -f Makefile.cocoa

cocoa-clean:
	cd Library_Bullet_Physics && make                   clean
	cd Library_GLFW           && make -f Makefile.cocoa clean
	cd Library_GLee           && make                   clean
	cd Library_JPEG           && make                   clean
	cd Library_libpng         && make                   clean
	cd Library_FreeType       && make                   clean
	cd Library_MMDAgent       && make                   clean
	cd Library_MMDFiles       && make                   clean
	cd Library_zlib           && make                   clean
	cd Library_PortAudio      && make -f Makefile.cocoa clean
	cd Library_RE2            && make                   clean
	cd Library_hts_engine_API && make                   clean
	cd Library_Open_JTalk     && make                   clean
	cd Library_Julius         && make                   clean
	cd main                   && make -f Makefile.cocoa clean
	cd Plugin_VIManager       && make -f Makefile.cocoa clean
	cd Plugin_Variables       && make -f Makefile.cocoa clean
	cd Plugin_Audio           && make -f Makefile.cocoa clean
	cd Plugin_LookAt          && make -f Makefile.cocoa clean
	cd Plugin_Open_JTalk      && make -f Makefile.cocoa clean
	cd Plugin_Julius          && make -f Makefile.cocoa clean

Library_Julius/include/julius/config.h: Library_Julius/src/libjulius/include/julius/config.h.in
	echo '#define JULIUS_PRODUCTNAME ""'   >  Library_Julius/include/julius/config.h
	echo '#define JULIUS_VERSION "4.3"'    >> Library_Julius/include/julius/config.h
	echo '#define JULIUS_SETUP "fast"'     >> Library_Julius/include/julius/config.h
	echo '#define JULIUS_HOSTINFO ""'      >> Library_Julius/include/julius/config.h
	echo '#define RETSIGTYPE void'         >> Library_Julius/include/julius/config.h
	echo '#define STDC_HEADERS 1'          >> Library_Julius/include/julius/config.h
	echo '#define UNIGRAM_FACTORING 1'     >> Library_Julius/include/julius/config.h
	echo '#define LOWMEM2 1'               >> Library_Julius/include/julius/config.h
	echo '#define PASS1_IWCD 1'            >> Library_Julius/include/julius/config.h
	echo '#define SCAN_BEAM 1'             >> Library_Julius/include/julius/config.h
	echo '#define GPRUNE_DEFAULT_BEAM 1'   >> Library_Julius/include/julius/config.h
	echo '#define CONFIDENCE_MEASURE 1'    >> Library_Julius/include/julius/config.h
	echo '#define LM_FIX_DOUBLE_SCORING 1' >> Library_Julius/include/julius/config.h
	echo '#define GRAPHOUT_DYNAMIC 1'      >> Library_Julius/include/julius/config.h
	echo '#define GRAPHOUT_SEARCH 1'       >> Library_Julius/include/julius/config.h
	echo '#define HAVE_STRCASECMP 1'       >> Library_Julius/include/julius/config.h

Library_Julius/include/sent/config.h: Library_Julius/src/libsent/include/sent/config.h.in
	echo '#define LIBSENT_VERSION "4.3"'   >  Library_Julius/include/sent/config.h
	echo '#define AUDIO_API_NAME ""'       >> Library_Julius/include/sent/config.h
	echo '#define AUDIO_API_DESC ""'       >> Library_Julius/include/sent/config.h
	echo '#define AUDIO_FORMAT_DESC ""'    >> Library_Julius/include/sent/config.h
	echo '#define GZIP_READING_DESC ""'    >> Library_Julius/include/sent/config.h
	echo '#define STDC_HEADERS 1'          >> Library_Julius/include/sent/config.h
	echo '#define USE_MIC 1'               >> Library_Julius/include/sent/config.h
	echo '#define USE_ADDLOG_ARRAY 1'      >> Library_Julius/include/sent/config.h
	echo '#define HAVE_SOCKLEN_T 1'        >> Library_Julius/include/sent/config.h
	echo '#define HAVE_UNISTD_H 1'         >> Library_Julius/include/sent/config.h
	echo '#define HAVE_ZLIB 1'             >> Library_Julius/include/sent/config.h
	echo '#define HAVE_STRCASECMP 1'       >> Library_Julius/include/sent/config.h
	echo '#define HAVE_SLEEP 1'            >> Library_Julius/include/sent/config.h
	echo '#define CLASS_NGRAM 1'           >> Library_Julius/include/sent/config.h
	echo '#define MFCC_SINCOS_TABLE 1'     >> Library_Julius/include/sent/config.h

android: Library_Julius/include/julius/config.h Library_Julius/include/sent/config.h
	ndk-build NDK_DEBUG=0

android-clean:
	rm -f Library_Julius/include/julius/config.h Library_Julius/include/sent/config.h
	ndk-build clean
