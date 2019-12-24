CXX ?= g++
LAB_ROOT ?= ../..
BACKEND ?= STDIO
CXXFLAGS ?= --std=c++11 -I $(LAB_ROOT)/HAL/include -DROUTER_BACKEND_$(BACKEND)
LDFLAGS ?= -lpcap



CC ?= g++
RM	?= rm

INCLUDE ?= $(LAB_ROOT)/include

SRC ?= $(LAB_ROOT)/src
SRCS		:= $(wildcard $(SRC)/*.cpp)

DEP ?= $(LAB_ROOT)/dep
DEPS		:= $(patsubst $(SRC)%.cpp,$(DEP)/%.d,$(SRCS))

OBJ ?= $(LAB_ROOT)/obj
OBJS	:= $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(SRCS))


clean_share:
	$(RM) $(DEP)/* $(OBJ)/*

$(DEP)/%.d: $(SRC)%.cpp
	@set -e; \
	$(RM) $@; \
	$(CC) -MM -I$(INCLUDE) $< > $@.$$$$; \
	sed 's,^.*:,$@ $(OBJ)/&,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

-include $(DEPS)

$(OBJ)/%.o: $(SRC)/%.cpp $(DEP)/%.d
	$(CC) $(CXXFLAGS) -c -I$(INCLUDE) $< -o $@

.PHONY: clean_share



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
