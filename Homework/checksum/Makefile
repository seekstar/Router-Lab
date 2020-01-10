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



grade: checksum
	python3 grade.py

hal.o: $(LAB_ROOT)/HAL/src/stdio/router_hal.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

checksum: main.o hal.o $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

std: std.o main.o hal.o
	$(CXX) $^ -o $@ $(LDFLAGS)


target_platform ?= netns

-include $(LAB_ROOT)/config/$(target_platform)/my123/r1/Makefile
-include $(LAB_ROOT)/config/$(target_platform)/my123/r2/Makefile
-include $(LAB_ROOT)/config/$(target_platform)/my123/r3/Makefile


all: checksum

clean: clean_share
	$(RM) *.o *.d *.d.* checksum std

.PHONY: all clean grade
