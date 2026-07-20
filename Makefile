CXX = g++
CXXFLAGS = -Wall -fPIC -Werror -Wextra -std=c++17 -Iinclude
LDFLAGS = -shared

SRCS = $(wildcard src/*.cpp)
OBJS = $(patsubst src/%.cpp, build/%.o, $(SRCS))


LIB_NAME = liblogger.so

BUILD_DIR = build

lib: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $(LIB_NAME)

$(BUILD_DIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(BUILD_DIR)/*.o $(LIB_NAME)

.PHONY: lib clean
