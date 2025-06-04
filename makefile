# ---------------------------------------- 
CXX 		:= g++
CXXFLAGS 	:= -march=native -mavx2 -O3 -Iinclude -I/usr/include/freetype2
LDFLAGS 	:= -lfreetype -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl

# ---------------------------------------- 
SRC_DIR := src
OBJ_DIR := build
BIN := main
# ---------------------------------------- 

# ---------------------------------------- 
SOURCES := $(shell find $(SRC_DIR) -type f \( -name '*.cpp' -o -name '*.c' \))
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
OBJECTS := $(OBJECTS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# ---------------------------------------- 



.PHONY: all clean 

all: $(BIN)

$(BIN): $(OBJECTS)
	@echo Linking $(BIN)...
	$(CXX) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN)
