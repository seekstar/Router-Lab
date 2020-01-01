r2root ?= $(LAB_ROOT)/config/netns/my123/r2

$(r2root)/board.o: $(r2root)/board.cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(r2root)/hal.o: $(LAB_ROOT)/HAL/src/linux/router_hal.cpp $(r2root)/standard.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

boilerplate2: main.o protocol.o forwarding.o $(OBJS) $(r2root)/hal.o $(r2root)/board.o
	$(CXX) $^ -o $@ $(LDFLAGS) 

clean_r2:
	$(RM) $(r2root)/*.o

.PHONY: clean_r2
