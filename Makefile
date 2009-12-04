SRC = nxtdriver.cc nxtdriver.h nxtcomms.cc nxtcomms.h
OBJLIBS = libnxtdriver.so
OBJS = nxtdriver.o nxtcomms.o

all: $(OBJLIBS)

$(OBJS): $(SRC)
	echo "Building the NXTDriver plugin..."
	$(CXX) -lusb -Wall -fpic -g3 `pkg-config --cflags playercore` -c $(SRC)

$(OBJLIBS): $(OBJS)
	$(CXX) -shared -nostartfiles -o $@ $^

clean:
	echo "Cleaning up the NXTDriver plugin..."
	rm -f $(OBJS) $(OBJLIBS) *.gch
