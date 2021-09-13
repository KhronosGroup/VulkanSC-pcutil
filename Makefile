
all: ptest

INCLUDES = fake/vulkan_sc_core.hpp pcreader.hpp pcwriter.hpp
SOURCES =

ptest: pipeline.cpp $(INCLUDES)
	g++ -g -Wall -std=c++11 pipeline.cpp -o ptest

clean:
	rm ptest
