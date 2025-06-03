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
SOURCES := $(shell find $(SRC_DIR) -name '*.cpp' -o -name '*.c')
OBJECTS := $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(SOURCES:.cpp=.o))
OBJECTS := $(OBJECTS:.c=.o)
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
