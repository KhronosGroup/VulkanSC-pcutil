
all: ptest pcinfo

INCLUDES = fake/vulkan/vulkan_sc_core.hpp pcreader.hpp pcwriter.hpp
SOURCES =
CPP_FLAGS = -Wall -Wextra -Werror -Wconversion -std=c++11

ptest: pipeline.cpp $(INCLUDES)
	g++ -g $(CPP_FLAGS) -I fake/ pipeline.cpp -o ptest

ptest32: pipeline.cpp $(INCLUDES)
	g++ -g $(CPP_FLAGS) -I fake/ -m32 pipeline.cpp -o ptest32

pcinfo: pcinfo.cpp $(INCLUDES)
	g++ -g $(CPP_FLAGS) -I fake/ pcinfo.cpp -o pcinfo

clean:
	rm -f ptest ptest32 pcinfo
