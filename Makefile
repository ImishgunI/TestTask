CXX = g++
CXXFLAGS = -Wall -fPIC -Werror -Wextra -std=c++17 -Iinclude
LDFLAGS = -shared

LIB_SRCS = $(wildcard src/*.cpp)
LIB_OBJS = $(patsubst src/%.cpp, $(BUILD_DIR)/%.o, $(LIB_SRCS))

APP_SRCS = app/main.cpp
APP_OBJS = $(patsubst app/%.cpp, $(BUILD_DIR)/%.o, $(APP_SRCS))


LIB_NAME = liblogger.so

BUILD_DIR = build

main: dir lib $(APP_OBJS)
	$(CXX) $(CXXFLAGS) $(APP_OBJS) -o $@ -L. -llogger -Wl,-rpath,'$$ORIGIN'

$(BUILD_DIR)/%.o: app/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

lib: $(LIB_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIB_OBJS) -o $(LIB_NAME)

$(BUILD_DIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

dir:
	mkdir -p $(BUILD_DIR)

clean:
	rm -f $(BUILD_DIR)/*.o $(LIB_NAME) main *.log

.PHONY: lib clean
