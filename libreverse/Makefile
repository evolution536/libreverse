CXX                  = g++
STANDARD_FLAGS       = -std=c++11
WARN_AS_ERRORS_FLAGS = -Wall -Wextra -Werror
DEBUG_FLAGS          = -g3
OPT_FLAGS            = -O3
CPPFLAGS             = $(OPT_FLAGS) $(DEBUG_FLAGS) $(STANDARD_FLAGS) $(WARN_AS_ERRORS_FLAGS)

MAIN = libreverse.a
LIBS =
INCLUDES = -I"$(CURDIR)/include"

SRCS = pe_util.cpp string_util.cpp binary.cpp encodings.cpp lznt1.cpp md5.cpp sha1.cpp checksums.cpp math_opt.cpp aplib.cpp rc4.cpp sha256.cpp hmac.cpp
OBJS = pe_util.o string_util.o binary.o encodings.o lznt1.o md5.o sha1.o checksums.o math_opt.o aplib.o rc4.o sha256.o hmac.o
VPATH = src

all: $(MAIN)

$(MAIN): $(OBJS)
	ar -r $(MAIN) $(OBJS) $(LIBS)

%.o : %.cpp
	$(CXX) $(INCLUDES) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f *.o *.a
