
all: ptest

INCLUDES = fake/vulkan/vulkan_sc_core.hpp pcreader.hpp pcwriter.hpp
SOURCES =

ptest: pipeline.cpp $(INCLUDES)
	g++ -g -Wall -Wextra -Werror -Wconversion -std=c++11 -I fake pipeline.cpp -o ptest

ptest32: pipeline.cpp $(INCLUDES)
	g++ -g -Wall -Wextra -Werror -Wconversion -std=c++11 -I fake -m32 pipeline.cpp -o ptest32

clean:
	rm -f ptest ptest32
