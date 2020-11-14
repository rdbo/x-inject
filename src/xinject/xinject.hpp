#ifndef XINJECT_HPP
#define XINJECT_HPP

#include <iostream>
#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl2.h>
#include <libmem/libmem.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <fonts/Ubuntu_Regular.h>

#if defined(MEM_WIN)
#define LIB_MODE 0
#elif defined(MEM_LINUX)
#define LIB_MODE RTLD_LAZY
#endif

class XInject
{
    private:
    bool               run = false;
    mem_process_t      target {  };
    mem_process_list_t proc_list {  };
    char               lib_path[512]{ };
    SDL_Window*        window = NULL;
    SDL_WindowFlags    window_flags;
    int                width;
    int                height;
    SDL_GLContext      gl_context = NULL;
    ImVec4             clear_color = ImVec4(0, 0, 0, 0);
    ImFont*            Ubuntu_Regular0;
    ImFont*            Ubuntu_Regular1;

    public:
    XInject();
    ~XInject();
    void Style();
    void Run();
    void Draw();
    void Inject();
    void Update();
    void MsgBox(const char msg[]);
};

#endif