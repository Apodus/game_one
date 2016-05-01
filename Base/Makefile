COMMON_SRC_DIRS_ = src/ $(wildcard src/*/) $(wildcard src/*/*/) $(wildcard src/*/*/*/)
COMMON_SRC_DIRS = $(patsubst %.o,,$(patsubst %.d,,$(patsubst %.icc,,$(patsubst %.h,,$(patsubst %.hpp,,$(patsubst %.cpp,,$(COMMON_SRC_DIRS_)))))))
OBJECT_DIRS = $(patsubst src/%, out/%, $(COMMON_SRC_DIRS))
INCLUDE_DIRS = -I src/ -I lib/linux/glfw3/include
CXXFLAGS = -pedantic -Wall -lpthread --std=c++11 -O3 -DGLEW_STATIC $(INCLUDE_DIRS)
LDLIBS = -L lib/linux/glfw3/lib -lglfw3 -lassimp -lXi -lXrandr -lXxf86vm -lpng -lGLEW -lGL -lX11
CXX  = g++

GAME_TARGET = build/bin/game

#obj_game = $(patsubst %.cpp,%.o,$(foreach dir, $(COMMON_SRC_DIRS), $(wildcard $(dir)*.cpp)))
obj_game = $(patsubst src/%.cpp,out/%.o, $(foreach dir, $(COMMON_SRC_DIRS), $(wildcard $(dir)*.cpp)))
dep_game = $(obj_game:.o=.d)
#dep_game := $(sort $(patsubst src/%,out/%, $(dep_game)))
dep_game := $(sort $(dep_game))

all: $(GAME_TARGET)

debug: CXXFLAGS += -O0 -g
debug: LDLIBS += -g
debug: GAME_TARGET = build/bin/game_d
debug: all

prof: CXXFLAGS += -pg
prof: LDLIBS += -pg
prof: GAME_TARGET = build/bin/game_p
prof: all

.PHONY: all clean

$(GAME_TARGET): $(obj_game)
	$(CXX) -o $@ $^ $(LDLIBS)

$(dep_game): out/%.d: src/%.cpp | createDirs
	$(CXX) -MT "$(@:.d=.o) $@" -MM $(CXXFLAGS) $< > $@

createDirs:
	mkdir -p $(foreach dir, $(OBJECT_DIRS), $(dir))

clean:
	$(RM) $(GAME_TARGET)
	$(RM) -rf out/

out/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

ifneq ($(MAKECMDGOALS),clean)
  include $(dep_game)
endif