BIN := viz
OBJS := \
	src/main.o \
	src/wb_analyzer.o \
	src/wb_gldebug.o

PACKAGES := ao libglfw
CCFLAGS := -O2 $(shell pkg-config --cflags $(PACKAGES))
LIBS := $(shell pkg-config --libs $(PACKAGES))

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CCFLAGS)

.PHONY: clean

clean:
	rm -f $(BIN) $(OBJS)
