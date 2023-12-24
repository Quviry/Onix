#include "proxy.hpp"
#include "Window.hpp"
#include "datatypes.h"
#include "tcp.hpp"

#include "../imgui.h"
#include "../backends/imgui_impl_glfw.h"
#include "../backends/imgui_impl_opengl3.h"
#include <thread>
#include <memory>
#include <iostream>

constexpr int MAX_PING_CONNECTIONS = 8;

namespace onix
{

    std::shared_ptr<proxy_state> get_proxy_state()
    {
        auto ps = std::make_shared<proxy_state>();
        ps->connection_types = static_cast<uint8_t>(ConnectionType::ICMP | ConnectionType::TCP | ConnectionType::UDP);
        ps->resolvers = { {.name = "Localhost\0", .address = "127.0.0.1\0"} };
        return ps;
    }

    void ping_access(std::shared_ptr<proxy_state> ps)
    {
        // std::cout << "Try ip " << ps->orphan << " \n";
        for (;;)
        {
            bool orphan = true;
            for (const auto &resolver : ps->resolvers)
            {

                auto address = std::string(resolver.address);
                // std::cout << "Try ip " << address << " \n";
                if (address.find(":") == address.npos)
                {
                    client_tcp_pipeline(address, L2_COMMUNICATION_PORT, [&orphan](int fd)
                                        {
                                            std::cout << "Start sending buff \n";
                    char buffer[] = "ping::init";
                    if (-1 != write(fd, buffer, sizeof(buffer))){
                        orphan = false;
                    }std::cout << "Start sending buff \n"; });
                }
                else
                {
                    client_tcp_pipeline(address.substr(0, address.find(":")), atoi(address.substr(address.find(":") + 1).c_str()), [&orphan](int fd)
                                        {
                    char buffer[] = "ping::init";
                    if(-1 != write(fd, buffer, sizeof(buffer))){
                        orphan = false;
                    } });
                }
            }
            ps->orphan = orphan;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        };
    }

    void run_proxy_network(std::shared_ptr<proxy_state> ps)
    {
        std::jthread{ping_access, ps};
    }

    void run_proxy_gui(std::shared_ptr<proxy_state> ps)
    {
        Window window{};

        (void)window.start(
            [&ps](GLFWwindow *window, ImGuiIO &io)
            {
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
                ImGui::Begin("Proxy control", nullptr,
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
    void run_proxy()
    {
        std::shared_ptr<proxy_state> ps = get_proxy_state();
        std::jthread network_thread{run_proxy_network, ps};
        std::jthread gui_thread{run_proxy_gui, ps};
    }
}