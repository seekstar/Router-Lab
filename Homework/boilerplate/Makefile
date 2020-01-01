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


-include $(LAB_ROOT)/config/netns/my123/r1/Makefile
-include $(LAB_ROOT)/config/netns/my123/r2/Makefile
-include $(LAB_ROOT)/config/netns/my123/r3/Makefile

all: boilerplate1 boilerplate2 boilerplate3

clean: clean_share clean_r1 clean_r2 clean_r3
	rm -f *.o std *.d *.d.* boilerplate boilerplate1 boilerplate2 boilerplate3

.PHONY: all clean
