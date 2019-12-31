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
	$(CC) -MM $(CXXFLAGS) -I$(INCLUDE) $< > $@.$$$$; \
	sed 's,^.*:,$@ $(OBJ)/&,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

-include $(DEPS)

$(OBJ)/%.o: $(SRC)/%.cpp $(DEP)/%.d
	$(CC) $(CXXFLAGS) -c -I$(INCLUDE) $< -o $@

.PHONY: clean_share
