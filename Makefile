BINS := viz

VIZ_OBJS := \
	src/viz_main.o \
	src/tinythread.o \
	src/kiss_fftr.o \
	src/kiss_fft.o \
	src/wb_analyzer.o \
	src/wb_biquad.o \
	src/wb_gldebug.o

CDEPS := src/*.h

PACKAGES := ao libglfw
CCFLAGS := -Wall -Werror -g -O3 -ffast-math
LIBS := -lstdc++ -lAntTweakBar -lm

CCFLAGS += $(shell pkg-config --cflags $(PACKAGES))
LIBS += $(shell pkg-config --libs $(PACKAGES))

all: $(BINS)

viz: $(VIZ_OBJS)
	$(CC) -o $@ $(VIZ_OBJS) $(LIBS)

%.o: %.cpp $(CDEPS)
	$(CC) -c -o $@ $< $(CCFLAGS)

.PHONY: clean

clean:
	rm -f $(BINS) $(VIZ_OBJS)
