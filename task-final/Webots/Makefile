CXX = g++
CXXFLAGS += -I./serial/include -I./rapidjson/include
LIBS += ./serial/build/Release/serial.lib

TARGET = controller-fp

all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(TARGET).cpp $(LDFLAGS)
