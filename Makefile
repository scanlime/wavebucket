BINS := viz

VIZ_OBJS := \
	src/viz_main.o \
	src/tinythread.o \
	src/wb_analyzer.o \
	src/wb_gldebug.o

CDEPS := src/*.h

PACKAGES := ao libglfw
CCFLAGS := -Wall -g -O2
LIBS := -lstdc++ -lAntTweakBar

CCFLAGS += $(shell pkg-config --cflags $(PACKAGES))
LIBS += $(shell pkg-config --libs $(PACKAGES))

all: $(BINS)

viz: $(VIZ_OBJS) $(CDEPS)
	$(CC) -o $@ $(VIZ_OBJS) $(LIBS)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CCFLAGS)

.PHONY: clean

clean:
	rm -f $(BINS) $(VIZ_OBJS)
