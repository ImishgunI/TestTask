CXX = g++
CXXFLAGS = -Wall -fPIC -Werror -Wextra -std=c++17 -Iinclude
LDFLAGS = -shared

LIB_SRCS = $(wildcard src/*.cpp)
LIB_OBJS = $(patsubst src/%.cpp, $(BUILD_DIR)/%.o, $(LIB_SRCS))

APP_SRCS = app/main.cpp
APP_OBJS = $(patsubst app/%.cpp, $(BUILD_DIR)/%.o, $(APP_SRCS))

LIB_NAME = liblogger.so
BUILD_DIR = build

all: lib main

lib: $(LIB_OBJS)
	$(CXX) $(LDFLAGS) $(LIB_OBJS) -o $(LIB_NAME)

main: lib $(APP_OBJS)
	$(CXX) $(CXXFLAGS) $(APP_OBJS) -o $@ -L. -llogger -Wl,-rpath,'$$ORIGIN'


$(BUILD_DIR)/%.o: src/%.cpp include/*.hpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: app/%.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@


test: all
	chmod +x tests/run_tests.sh
	./tests/run_tests.sh

clean:
	rm -f $(BUILD_DIR)/*.o $(LIB_NAME) main *.log

.PHONY: lib clean main all
