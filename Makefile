BIN := viz
OBJS := src/main.o

PACKAGES := ao
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
