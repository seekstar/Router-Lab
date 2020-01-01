r1root ?= $(LAB_ROOT)/config/netns/my123/r1

$(r1root)/board.o: $(r1root)/board.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(r1root)/hal.o: $(LAB_ROOT)/HAL/src/linux/router_hal.cpp $(r1root)/standard.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

boilerplate1: main.o protocol.o forwarding.o $(OBJS) $(r1root)/hal.o $(r1root)/board.o
	$(CXX) $^ -o $@ $(LDFLAGS) 

clean_r1:
	$(RM) $(r1root)/*.o

.PHONY: clean_r1
