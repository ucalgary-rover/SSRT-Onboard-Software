CXX = g++
CXXFLAGS = -std=c++17 -I/usr/local/include
LDFLAGS = -L/usr/local/lib -lpaho-mqttpp3 -lpaho-mqtt3a

all: publisher

publisher: publisher.cpp
	$(CXX) $(CXXFLAGS) -o publisher publisher.cpp $(LDFLAGS)

clean:
	rm -f publisher
