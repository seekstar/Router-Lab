CXX ?= g++
LAB_ROOT ?= ../..
BACKEND ?= LINUX
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


.PHONY: all clean
all: boilerplate

clean: clean_share
	rm -f *.o boilerplate std *.d *.d.*

hal.o: $(LAB_ROOT)/HAL/src/linux/router_hal.cpp $(LAB_ROOT)/HAL/src/linux/platform/standard.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

boilerplate: main.o hal.o protocol.o forwarding.o $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS) 
