CXX ?= g++
LAB_ROOT ?= ../..
BACKEND ?= STDIO
CXXFLAGS ?= --std=c++11 -I $(LAB_ROOT)/HAL/include -DROUTER_BACKEND_$(BACKEND)
LDFLAGS ?= -lpcap

-include $(LAB_ROOT)/lib/Makefile

grade: checksum
	python3 grade.py

%.d: %.cpp
	@set -e; \
	$(RM) $@; \
	$(CC) -MM $(CXXFLAGS) -I$(INCLUDE) $< > $@.$$$$; \
	sed 's,^.*:,$@ &,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

-include $(patsubst %.cpp,%.d,$(wildcard *.cpp))

%.o: %.cpp %.d
	$(CXX) $(CXXFLAGS) -c -I$(INCLUDE) $< -o $@

hal.o: $(LAB_ROOT)/HAL/src/stdio/router_hal.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

checksum: main.o hal.o $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

std: std.o main.o hal.o
	$(CXX) $^ -o $@ $(LDFLAGS)

all: checksum

clean: clean_share
	$(RM) *.o *.d *.d.* checksum std

.PHONY: all clean grade
