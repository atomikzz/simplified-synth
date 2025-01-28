
ASSEMBLY :=.
BUILD_DIR := bin
LIBRARY := library
INCLUDE := include


COMPILER_FLAGS := -std=c++17 -O0 -g #-Wall -Werror -Wvla -Wgnu-folding-constant -Wno-missing-braces -fdeclspec -fPIC -ObjC
INCLUDE_FLAGS := -I$(INCLUDE)
LINKER_FLAGS := -L$(LIBRARY) -lglfw #-ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi 
SRC_FILES := $(shell find $(ASSEMBLY) -type f \( -name "*.cpp" -o -name "*.m" \))

.PHONY: build clean

# Main build target
build:
	mkdir -p $(BUILD_DIR)
	g++ $(COMPILER_FLAGS) $(SRC_FILES) -o $(BUILD_DIR)/app $(INCLUDE_FLAGS) $(LINKER_FLAGS)

clean:
	rm -rf $(BUILD_DIR)/*.app

