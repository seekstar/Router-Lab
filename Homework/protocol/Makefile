CXX ?= g++
LAB_ROOT ?= ../..
BACKEND ?= STDIO
CXXFLAGS ?= --std=c++11 -I $(LAB_ROOT)/HAL/include -DROUTER_BACKEND_$(BACKEND)
LDFLAGS ?= -lpcap

-include $(LAB_ROOT)/lib/Makefile

%.d: %.cpp
	@set -e; \
	$(RM) $@; \
	$(CC) -MM $(CXXFLAGS) -I$(INCLUDE) $< > $@.$$$$; \
	sed 's,^.*:,$@ &,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

-include $(patsubst %.cpp,%.d,$(wildcard *.cpp))

%.o: %.cpp %.d
	$(CXX) $(CXXFLAGS) -c -I$(INCLUDE) $< -o $@


.PHONY: all clean grade
all: protocol

clean:
	rm -f *.o protocol std *.d *.d.*

grade: protocol
	python3 grade.py

hal.o: $(LAB_ROOT)/HAL/src/stdio/router_hal.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

protocol: protocol.o main.o hal.o $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS) 

std: std.o main.o hal.o
	$(CXX) $^ -o $@ $(LDFLAGS) 
