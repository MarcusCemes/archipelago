# C++ Out-of-source Makefile
# Author: Marcus Cemes <marcus.cemes@epfl.ch>
# Inspired by https://gist.github.com/vsee/227ff0817856420da8636e1a2b8deb97
# Supports subdirectories, automatic dependency generation and out-of-source builds.
# obj/ and dist/ should be gitignored

BUILD_TARGET  ?= projet

# Folder structure
SRC_DIR       := src
OBJ_DIR       := obj
DIST_DIR      := dist

# Compilation and linker flags
CXX           := g++
CXXFLAGS      := -std=c++11 -Wall -Wextra -pedantic -O3 -c
LD            := g++
LDFLAGS       :=
DFLAGS        := -g3 -O0 -DDEBUG

# Setup external libraries, treating as system headers supresses warnings
INC           ?= gtkmm-3.0
CXXINC        := $(shell pkg-config --cflags $(INC) | sed -e 's/ -I/ -isystem /g')
LDINC         := $(shell pkg-config --libs $(INC))

# Colour output
ESCAPE        := \e
CYAN          := $(ESCAPE)[36m
MAGENTA       := $(ESCAPE)[35m
GREEN_BG      := $(ESCAPE)[30;42m
RESET         := $(ESCAPE)[0m

# System commands, avoids conflicts
ECHO          ?= /bin/echo -e
CHMOD         ?= /bin/chmod

# Runtime variables
SOURCES       := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)
OBJECTS       := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
DEPENDS       := $(patsubst %.o, %.d, $(OBJECTS))


# Use build target as main entry point
# Run the linker on all generated object files to create an executable
# Linker flags MUST come AFTER object files
$(DIST_DIR)/$(BUILD_TARGET): $(OBJECTS) | $(DIST_DIR)
	@$(ECHO) " $(CYAN)→$(RESET) Linking object files into binary"

	@$(LD) -o $@ $^ $(LDFLAGS) $(LDINC)
	@chmod +x $(DIST_DIR)/$(BUILD_TARGET)

	@$(ECHO) "\n$(GREEN_BG) DONE $(RESET) $(BUILD_TARGET) has been successfuly built"



# Compile sources files into object files
$(OBJ_DIR)/%.o: | $(OBJ_DIR)
	@$(ECHO) " $(CYAN)→$(RESET) Compiling $(CYAN)$<$(RESET)"

	@mkdir -p $(shell dirname $@)
	@$(CXX) $(CXXFLAGS) $(CXXINC) -o $@ $<


# Automatically generate each objects dependencies by extracting header
# dependencies into `.d` files next to the corresponding object file
# file must be prepended with subdirectory path
$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@$(ECHO) " $(MAGENTA)→$(RESET) Preparing $(CYAN)$<	$(RESET)"

	@mkdir -p $(shell dirname $@)
	@$(ECHO) -n "$(shell dirname $@)/" > $@
	@$(CXX) -MM $< >> $@



# Don't run the dependency file if cleaning
ifneq ($(MAKECMDGOALS),clean)
-include $(DEPENDS)
endif


# Compile a developmenet version for debugging
.PHONY: debug
debug: CXXFLAGS+=$(DFLAGS)
debug: $(DIST_DIR)/$(BUILD_TARGET)


# Create folder structures
$(DIST_DIR):
	@mkdir -p $@
$(OBJ_DIR):
	@mkdir -p $@


# Run the build target, convienence function
.PHONY: run
run: $(DIST_DIR)/$(BUILD_TARGET)
	@$(DIST_DIR)/$(BUILD_TARGET) "test/tests/s05.txt"


# Run the tests on built target
.PHONY: test
test: $(DIST_DIR)/$(BUILD_TARGET)
	@test/run_tests.sh $(DIST_DIR)/$(BUILD_TARGET)


# Delete build directories
.PHONY: clean
clean:
	@$(ECHO) " $(CYAN)→$(RESET) Cleaning build files"
	@rm -rf $(DIST_DIR) $(OBJ_DIR)
	@$(ECHO) "\n$(GREEN_BG) DONE $(RESET) Build files cleaned"


# Phew. That's all folks.
