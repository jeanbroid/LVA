# LVA global makefile

# todo: rls/ dbg mode

include config.mk

LVA_SRC := $(shell find $(LVA_DIR) -name '*.c')
LVA_OBJC := $(LVA_SRC:%.c=%.o)
LVA_OBJC_DBG := $(LVA_SRC:%.c=%.dbg.o)

LVA_SRCXX := $(shell find $(LVA_DIR) -name '*.cpp')
LVA_OBJXX := $(LVA_SRCXX:%.cpp=%.o)
LVA_OBJXX_DBG := $(LVA_SRCXX:%.cpp=%.dbg.o)

LVA_OBJ = $(LVA_OBJC) $(LVA_OBJXX)

all: $(LVA_LIB)

lib: $(LVA_LIB)

$(LVA_LIB): $(LVA_OBJ)
	$(LD) $(LDFLAGS) -shared $^ -o $(BUILD_DIR)/$@ 

demos: 
	@mkdir -p $(BUILD_DIR)
	@make demo_cycle demo_one_row demo_matrix demo_pattern_7x5

options:
	@echo build options:
	@echo "CC = ${CC}"
	@echo "CCX = ${CCX}"
	@echo "CFLAGS = ${CFLAGS}"
	@echo "CFLAGS_GTK = ${CFLAGS_GTK}"
	@echo "LDFLAGS = ${LDFLAGS}"
	@echo "LDFLAGS_GTK = ${LDFLAGS_GTK}"

lva: $(DEMO_DIR)/lva.o 
	$(CC) $^ $(LDFLAGS_LVA) $(LDFLAGS)  -o $(BUILD_DIR)/$@ 

bench: $(TESTS_DIR)/bench.o 
	$(CC) $^ $(LDFLAGS_LVA) $(LDFLAGS)  -o $(BUILD_DIR)/$@ 

# GPIOs demos
demo_pattern_7x5: $(DEMO_DIR)/demo_pattern_7x5.o 
	$(CC) $^ $(LDFLAGS) $(LDFLAGS_LVA) -o $(BUILD_DIR)/$@ 

demo_pattern_7x5_ng: $(DEMO_DIR)/demo_pattern_7x5_ng.o 
	$(CC) $^ $(LDFLAGS) $(LDFLAGS_LVA) -o $(BUILD_DIR)/$@ 

demo_matrix: $(DEMO_DIR)/demo_matrix.o
	$(CC) $^ $(LDFLAGS) $(LDFLAGS_LVA) -o $(BUILD_DIR)/$@ 

demo_one_row: $(DEMO_DIR)/demo_one_row.o 
	$(CC) $^ $(LDFLAGS) $(LDFLAGS_LVA) -o $(BUILD_DIR)/$@ 

demo_cycle: $(DEMO_DIR)/demo_cycle.o 
	$(CC) $^ $(LDFLAGS) $(LDFLAGS_LVA) -o $(BUILD_DIR)/$@ 

# Simulator
gleds: GLeds/gleds.o
	@mkdir -p $(BUILD_DIR)
	@$(CC) $^  $(LDFLAGS_GTK) $(LDFLAGS_LVA) $(LDFLAGS) -o $(BUILD_DIR)/$@ 

GLeds/gleds.o: GLeds/gleds.c
	@$(CC) $< $(CFLAGS) $(CFLAGS_GTK) -c -o $@

# Generic
%.o: %.c
	@$(CC) $< $(CFLAGS) -c -o $@ 

%.o: %.cpp
	@$(CXX) $< $(CFLAGS) -c -o $@ 

doc:
	@doxygen

clean:
	@$(RM) -rf $(shell find . -name '*.o') 
	@$(RM) -rf $(LVA_LIB)
	@$(RM) -rf $(BUILD_DIR)/*
	@$(RM) -rf doxygen

.PHONY: all options clean doc
