r3root ?= $(LAB_ROOT)/config/netns/my123/r3

$(r3root)/board.o: $(r3root)/board.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(r3root)/hal.o: $(LAB_ROOT)/HAL/src/linux/router_hal.cpp $(r3root)/standard.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

boilerplate3: main.o protocol.o forwarding.o $(OBJS) $(r3root)/hal.o $(r3root)/board.o
	$(CXX) $^ -o $@ $(LDFLAGS)

clean_r3:
	$(RM) $(r3root)/*.o

.PHONY: clean_r3
