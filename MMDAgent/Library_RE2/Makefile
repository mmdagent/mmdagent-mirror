
TARGET   = lib/RE2.a

SOURCES  = src/util/hash.cc \
           src/util/rune.cc \
           src/util/stringprintf.cc \
           src/util/strutil.cc \
           src/util/valgrind.cc \
           src/re2/bitstate.cc \
           src/re2/compile.cc \
           src/re2/dfa.cc \
           src/re2/filtered_re2.cc \
           src/re2/mimics_pcre.cc \
           src/re2/nfa.cc \
           src/re2/onepass.cc \
           src/re2/parse.cc \
           src/re2/perl_groups.cc \
           src/re2/prefilter.cc \
           src/re2/prefilter_tree.cc \
           src/re2/prog.cc \
           src/re2/re2.cc \
           src/re2/regexp.cc \
           src/re2/set.cc \
           src/re2/simplify.cc \
           src/re2/stringpiece.cc \
           src/re2/tostring.cc \
           src/re2/unicode_casefold.cc \
           src/re2/unicode_groups.cc

OBJECTS  = $(SOURCES:.cc=.o)

CXX      = g++
AR       = ar
CXXFLAGS = -Wall -O3 -fomit-frame-pointer -fPIC \
           -pthread -DHAVE_PTHREAD -DHAVE_RWLOCK
INCLUDE  = -I src

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(AR) cru $(TARGET) $(OBJECTS)
	cp src/re2/*.h include/re2
	cp src/util/*.h include/util

.cc.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(<:.cc=.o) -c $<

clean:
	rm -f $(OBJECTS) $(TARGET)
