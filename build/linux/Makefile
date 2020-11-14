CC=gcc
CXX=g++
ARCH=64
INCLUDE_DIR=../../include
ASSETS_DIR=../../assets
SRC_DIR=../../src
INCLUDES=-I$(INCLUDE_DIR) -I$(ASSETS_DIR) $(shell pkg-config --cflags sdl2 gl)
LIBS=-ldl $(shell pkg-config --libs sdl2 gl)
CFLAGS:=-g $(INCLUDES) -Wall -Wextra -Wpedantic
OBJS=main.o xinject.o libmem.o imgui_demo.o imgui_draw.o imgui_impl_opengl2.o imgui_impl_sdl.o imgui_widgets.o imgui.o
OUTPUT=x-inject

all:
	make x86
	make x64

x64: ARCH=64
x64: build_clean

x86: ARCH=32
x86: build_clean

build_clean: x-inject clean_obj

x-inject: $(OBJS)
	$(CXX) -m$(ARCH) -m$(ARCH) $(CFLAGS) $(OBJS) -o $(OUTPUT)_$(ARCH) $(LIBS)

main.o:
	$(CXX) -c -m$(ARCH) $(CFLAGS) $(SRC_DIR)/main.cpp -o main.o $(LIBS)

xinject.o:
	$(CXX) -c -m$(ARCH) $(CFLAGS) $(SRC_DIR)/xinject/xinject.cpp -o xinject.o $(LIBS)

libmem.o:
	$(CXX) -c -m$(ARCH) $(CFLAGS) $(INCLUDE_DIR)/libmem/libmem.c -o libmem.o $(LIBS)

imgui imgui_demo.o imgui_draw.o imgui_impl_opengl2.o imgui_impl_sdl.o imgui_widgets.o imgui.o:
	$(CXX) -c -m$(ARCH) $(CFLAGS) $(INCLUDE_DIR)/imgui/imgui.cpp -o imgui.o $(LIBS)
	$(CXX) -c -m$(ARCH) $(CFLAGS) $(INCLUDE_DIR)/imgui/imgui_demo.cpp -o imgui_demo.o $(LIBS)
	$(CXX) -c -m$(ARCH) $(CFLAGS) $(INCLUDE_DIR)/imgui/imgui_draw.cpp -o imgui_draw.o $(LIBS)
	$(CXX) -c -m$(ARCH) $(CFLAGS) $(INCLUDE_DIR)/imgui/imgui_impl_opengl2.cpp -o imgui_impl_opengl2.o $(LIBS)
	$(CXX) -c -m$(ARCH) $(CFLAGS) $(INCLUDE_DIR)/imgui/imgui_impl_sdl.cpp -o imgui_impl_sdl.o $(LIBS)
	$(CXX) -c -m$(ARCH) $(CFLAGS) $(INCLUDE_DIR)/imgui/imgui_widgets.cpp -o imgui_widgets.o $(LIBS)
	$(CXX) -c -m$(ARCH) $(CFLAGS) $(INCLUDE_DIR)/imgui/imgui.cpp -o imgui.o $(LIBS)

clean: clean_obj
	find . -type "f" -name "$(OUTPUT)*" -delete

clean_obj:
	find . -type "f" -name "*.o" -delete