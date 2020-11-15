#include "xinject.hpp"

XInject::XInject()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    this->window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    this->window = SDL_CreateWindow("x-inject", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, window_flags);
    this->gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, this->gl_context);
    SDL_GL_SetSwapInterval(1);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;

    this->Style();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();

    //clear_color = ImVec4(0.1, 0.1, 0.1, 1.f);

    this->run = true;
}

XInject::~XInject()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(this->gl_context);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void XInject::Run()
{
    ImGuiIO& io = ImGui::GetIO();
    while(this->run)
    {
        SDL_GetWindowSize(this->window, &this->width, &this->height);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                run = false;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                run = false;
        }

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        this->Draw();

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
    
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
}

void XInject::Draw()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(this->width, this->height));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;
    ImVec4 old_colors[sizeof(style.Colors) / sizeof(ImVec4)];
    memcpy(old_colors, style.Colors, sizeof(old_colors));

    ImGui::Begin("main-window", NULL, flags);
    {

        ImGui::Columns(2);

        ImGui::PushFont(this->Ubuntu_Regular0);
        ImGui::BeginChild("injector-settings");
        {
            ImGui::Indent(5.f);
            ImGui::Text("Injector settings");

            ImGui::PopFont();
            ImGui::PushFont(this->Ubuntu_Regular1);

            ImGui::Separator();

            ImGui::Text("Library Path: ");
            ImGui::InputText("##lib-path", this->lib_path, sizeof(this->lib_path)); ImGui::SameLine();
            if(ImGui::Button("..."))
            {
#				if defined(MEM_WIN)
				OPENFILENAME ofn;
				ZeroMemory(&ofn, sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = NULL;
				ofn.lpstrFile = this->lib_path;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(this->lib_path);
				ofn.lpstrFilter = "DLL Files\0*.dll\0";
				ofn.nFilterIndex = 1;
				GetOpenFileName(&ofn);
#				elif defined(MEM_LINUX)
                FILE *f = popen("zenity --title=\"Select a file...\" --file-selection", "r");
                fgets(this->lib_path, sizeof(this->lib_path), f);

                char holder[sizeof(this->lib_path)];
                memset(holder, 0x0, sizeof(holder));
                for(size_t i = 0; i < sizeof(this->lib_path); i++)
                {
                    if(this->lib_path[i] != '\n')
                        holder[i] = this->lib_path[i];
                }

                holder[sizeof(holder) - 1] = '\0';
                strcpy(this->lib_path, holder);
#				endif
            }

            ImGui::Separator();
            ImGui::Text("Target Process");

            if(!mem_process_is_valid(&this->target))
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "No Process Selected!");
            } 
            
            else
            {
                ImGui::Text("Name: %s", mem_string_c_str(&this->target.name));
                ImGui::Text("PID:  %i", this->target.pid);
            }

            ImGui::Separator();

            if(ImGui::Button("Inject"))
            {
                this->Inject();
            }

            ImGui::PopFont();

            ImGui::EndChild();
        }

        ImGui::NextColumn();

        ImGui::BeginChild("process-picker");
        {
            ImGui::Indent(5.f);
            ImGui::PushFont(this->Ubuntu_Regular0);

            ImGui::Text("Target Process Picker");

            ImGui::PopFont();
            ImGui::PushFont(this->Ubuntu_Regular1);

            ImGui::Separator();

            if(ImGui::Button("Update Process List"))
            {
                this->Update();
            }

            ImGui::BeginChild("process-list");
            {
                if(!mem_process_list_is_valid(&this->proc_list))
                {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "No Processes Found!");
                }

                else
                {
                    style.Colors[ImGuiCol_Button] = ImVec4(0, 0, 0, 0);
                    for(mem_size_t i = 0; i < mem_process_list_length(&this->proc_list); i++)
                    {
                        mem_process_t cur_process = mem_process_list_at(&this->proc_list, i);
                        if(mem_process_is_valid(&cur_process))
                        {
                            char proc_info[256];
                            snprintf(proc_info, sizeof(proc_info), "%s - %i", mem_string_c_str(&cur_process.name), cur_process.pid);
                            
                            if(ImGui::Button(proc_info))
                            {
                                this->target = cur_process;
                            }
                        }
                    }
                }

                ImGui::EndChild();
            }

            ImGui::PopFont();
            ImGui::EndChild();
        }
        ImGui::End();
    }

    memcpy(style.Colors, old_colors, sizeof(old_colors));
}

void XInject::Inject()
{
    if(!mem_process_is_valid(&this->target))
    {
        this->MsgBox("Invalid process!");
        return;
    }
    mem_lib_t lib = mem_lib_new2(this->lib_path, LIB_MODE);
    if(!mem_lib_is_valid(&lib))
    {
        this->MsgBox("Invalid library!");
        return;
    }

    mem_module_t mod = mem_ex_load_library(this->target, lib);
    if(!mem_module_is_valid(&mod))
    {
        this->MsgBox("Injection failed!");
    }

    mem_module_free(&mod);
}

void XInject::Update()
{
    mem_process_list_free(&this->proc_list);
    this->target = {};
    this->proc_list = mem_ex_get_process_list();
}

void XInject::Style()
{
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig cfg {  };
    cfg.FontDataOwnedByAtlas = false;
    this->Ubuntu_Regular0 = io.Fonts->AddFontFromMemoryTTF((void*)Ubuntu_Regular_ttf, sizeof(Ubuntu_Regular_ttf), 24, &cfg);
    this->Ubuntu_Regular1 = io.Fonts->AddFontFromMemoryTTF((void*)Ubuntu_Regular_ttf, sizeof(Ubuntu_Regular_ttf), 18, &cfg);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.f;
    style.WindowMinSize = ImVec2(640, 480);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1, 0.1, 0.1, 1);
    style.Colors[ImGuiCol_ChildBg]  = ImVec4(0.05, 0.05, 0.05, 1);
    style.Colors[ImGuiCol_FrameBg]  = ImVec4(0.15, 0.15, 0.15, 1);
    style.Colors[ImGuiCol_FrameBgHovered]  = ImVec4(0.20, 0.20, 0.20, 1);
    style.Colors[ImGuiCol_FrameBgActive]  = ImVec4(0.25, 0.25, 0.25, 1);
    style.Colors[ImGuiCol_Button] = ImVec4(0.15, 0.15, 0.35, 1);
    style.Colors[ImGuiCol_ButtonHovered]  = ImVec4(0.20, 0.20, 0.40, 1);
    style.Colors[ImGuiCol_ButtonActive]  = ImVec4(0.25, 0.25, 0.45, 1);
}

void XInject::MsgBox(const char msg[])
{
#	if defined(MEM_WIN)
	MessageBoxA(NULL, msg, "x-inject log", MB_OK);
#	elif defined(MEM_LINUX)
    const char cmd[] = "zenity --info --title=\"x-inject log\" --text=\"%s\"";
    char buf[sizeof(cmd) + strlen(msg) * sizeof(char) + 10];
    snprintf(buf, sizeof(buf), cmd, msg);
    system(buf);
#	endif
}
