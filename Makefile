SRC = nxtdriver.cc nxtdriver.h Connection.cc USBConnection.cc
OBJLIBS = libnxtdriver.so
OBJS = nxtdriver.o Connection.o USBConnection.o

all: $(OBJLIBS)

$(OBJS): $(SRC)
	echo "Building the NXTDriver plugin..."
	$(CXX) -Wall -fpic -g3 `pkg-config --cflags playercore` -c $(SRC)

$(OBJLIBS): $(OBJS)
	$(CXX) -shared -nostartfiles -o $@ $^

clean:
	echo "Cleaning up the NXTDriver plugin..."
	rm -f $(OBJS) $(OBJLIBS) *.gch
