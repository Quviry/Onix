#include <resolver.hpp>

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <tcp.hpp>
#include "widgets.hpp"

#include "../imgui/imgui.h"
#include "Window.hpp"
#include <GLFW/glfw3.h>
#include "../backends/imgui_impl_opengl3.h"
#include "../backends/imgui_impl_glfw.h"

#include "imfilebrowser.h"
#include <thread>
#include "sys/socket.h"
#include <arpa/inet.h>

namespace onix
{
    void run_resolver_gui(std::shared_ptr<resolver_state> cs)
    {
        Window window{};

        ImGui::FileBrowser fileDialog;

        // (optional) set browser properties
        fileDialog.SetTitle("title");
        fileDialog.SetTypeFilters({".h", ".cpp"});

        (void)window.start(
            [&fileDialog, &cs](GLFWwindow *window, ImGuiIO &io)
            {
                // Start polling events
                glfwPollEvents();

                // Start the Dear ImGui frame
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                // Window-sized widget
                ImGuiViewport *viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowSize(viewport->Size);
                ImGui::SetNextWindowPos(viewport->Pos);

                // Main widget
                ImGui::Begin("Resolver Control", nullptr,
                             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse); // GLOBAL HW

                // Running connection user-state
                static bool run_connection = false;
                if (ImGui::Button(run_connection ? "Connect" : "Disconect"))
                {
                    run_connection = !run_connection;
                }

                ImGui::End();

                // Rendering
                ImGui::Render();
                int display_w, display_h;
                glfwGetFramebufferSize(window, &display_w, &display_h);
                glViewport(0, 0, display_w, display_h);

                constexpr ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
                glClearColor(clear_color.x * clear_color.w,
                             clear_color.y * clear_color.w,
                             clear_color.z * clear_color.w, clear_color.w);
                glClear(GL_COLOR_BUFFER_BIT);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                glfwSwapBuffers(window);
            });
    }

    std::shared_ptr<resolver_state> get_resolver_state()
    {
        auto cs = std::make_shared<resolver_state>();
        std::ifstream fd("/etc/onix/resolver.conf", std::ios::in);
        std::string parse_string;
        if (fd.is_open())
        {
            fd >> parse_string;
        }
        return cs;
    }

    void run_resolver_network(std::shared_ptr<resolver_state> cs)
    {
        for (;;)
        {

            std::cout << "servered called with result" << server_tcp_pipeline("127.0.0.1", L2_COMMUNICATION_PORT, [cs](file_descriptor fd)
                                                                              {
                while(1){
                    struct sockaddr* client;
                    accept(fd, client, NULL);
                    char buf [1500];
                    int nread = read(fd, buf, sizeof(buf));
                    if(std::string(buf).substr(0, nread) == "ping::init"){ // ping by proxy
                    Proxy p;
                    strcpy(p.address, client->sa_data);
                    cs->ping_table.push_back(std::make_pair(p, std::chrono::system_clock::now()));
                    };
                    close(fd);
                } })
                      << "\n";
        }
    }

    void run_resolver()
    {
        std::shared_ptr<resolver_state> cs = get_resolver_state();
        std::jthread gui_thread{run_resolver_gui, cs};
        std::jthread network_thread{run_resolver_network, cs};
    }
} // namespace onixs