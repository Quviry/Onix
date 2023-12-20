
# Options
WITH_EXTRA_WARNINGS ?= 0
WITH_FREETYPE ?= 0

EXE = example_null
IMGUI_DIR = ./imgui
SOURCES = main.cpp ./src/Client.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)

CXXFLAGS += -std=c++11 -I$(IMGUI_DIR)
CXXFLAGS += -g -Wall -Wformat
LIBS =

# We use the WITH_EXTRA_WARNINGS flag on our CI setup to eagerly catch zealous warnings
ifeq ($(WITH_EXTRA_WARNINGS), 1)
	CXXFLAGS += -Wno-zero-as-null-pointer-constant -Wno-double-promotion -Wno-variadic-macros
endif

# We use the WITH_FREETYPE flag on our CI setup to test compiling misc/freetype/imgui_freetype.cpp
# (only supported on Linux, and note that the imgui_freetype code currently won't be executed)
ifeq ($(WITH_FREETYPE), 1)
	SOURCES += $(IMGUI_DIR)/misc/freetype/imgui_freetype.cpp
	CXXFLAGS += $(shell pkg-config --cflags freetype2)
	LIBS += $(shell pkg-config --libs freetype2)
endif

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	ifeq ($(WITH_EXTRA_WARNINGS), 1)
		CXXFLAGS += -Wextra -Wpedantic
		ifeq ($(shell $(CXX) -v 2>&1 | grep -c "clang version"), 1)
			CXXFLAGS += -Wshadow -Wsign-conversion
		endif
	endif
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	ifeq ($(WITH_EXTRA_WARNINGS), 1)
		CXXFLAGS += -Weverything -Wno-reserved-id-macro -Wno-c++98-compat-pedantic -Wno-padded -Wno-poison-system-directories
	endif
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(OS), Windows_NT)
	ECHO_MESSAGE = "MinGW"
	ifeq ($(WITH_EXTRA_WARNINGS), 1)
		CXXFLAGS += -Wextra -Wpedantic
	endif
	LIBS += -limm32
	CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------



%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/misc/freetype/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)