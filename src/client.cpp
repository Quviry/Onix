#include "client.h"
#include "tun.hpp"

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <math.h>

#include "../imgui/imgui.h"
#include "Window.hpp"
#include <GLFW/glfw3.h>
#include "../backends/imgui_impl_opengl3.h"
#include "../backends/imgui_impl_glfw.h"

#include "imfilebrowser.h"
#include "widgets.hpp"
#include <thread>
#include "tcp.hpp"

namespace onix
{
    void run_client_gui(std::shared_ptr<client_state> cs)
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
                ImGui::Begin("Client control", nullptr,
                             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse); // GLOBAL HW

                // Running connection user-state
                static bool run_connection = false;
                if (ImGui::Button(run_connection ? "Connect" : "Disconect"))
                {
                    run_connection = !run_connection;
                }

                // Journal call
                ImGui::Button("Open Journal");

                // Traffic speed line
                // float samples[100];
                // for (int n = 0; n < 100; ++n)
                // {
                //     samples[n] = std::sin(n * 0.2f + ImGui::GetTime() * 1.5f);
                // }
                // ImGui::PlotLines("Moving sin", samples, 100);
                // ImGui::PlotLines("Connection state", sinp, NULL, 70, 0, NULL, -1.0f, 1.0f, ImVec2(160, 60));

                // Connection state
                ImGui::TextColored(ImVec4(0.6f, 0.1f, 0.1f, 1.0f), (const char *)u8"[off]");

                // Settings Popup
                if (ImGui::Button("Settings"))
                    ImGui::OpenPopup("Settings");

                // Always center this window when appearing
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                if (ImGui::BeginPopupModal("Settings", NULL, ImGuiWindowFlags_Modal))
                {
                    ImGui::Text("Set up parameters");
                    ImGui::Separator();

                    ImGui::Checkbox("Autoconnect me on start", &cs->auto_connection_enabled);
                    ImGui::Checkbox("Fast connection", &cs->fast_connection_enabled);

                    // Choose prototype
                    ResolversList(cs->resolvers);
                    // Choose resolver

                    if (ImGui::Button("Close", ImVec2(120, 0)))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                ImGui::End();

                // static std::string file = "";
                // if (ImGui::Begin("dummy window"))
                // {
                //     ImGui::Text("%s", file.c_str());
                //     // open file dialog when user clicks this button
                //     if (ImGui::Button("open file dialog"))
                //         fileDialog.Open();
                // }
                // ImGui::End();

                // fileDialog.Display();

                // if (fileDialog.HasSelected())
                // {
                //     file = fileDialog.GetSelected().string();
                //     fileDialog.ClearSelected();
                // }

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

    std::shared_ptr<client_state> get_client_state()
    {
        auto cs = std::make_shared<client_state>();
        cs->auto_connection_enabled = true;
        cs->fast_connection_enabled = true;
        cs->con_type = ConnectionType::UDP;
        cs->gt_table = {};
        cs->resolvers = {
            {.name = "Localhost\0", .address = "127.0.0.1\0"}};
        cs->gt_priority = true;
        std::ifstream fd("/etc/onix/client.conf", std::ios::in);
        std::string parse_string;
        if (fd.is_open())
        {
            fd >> parse_string;
        }
        return cs;
    }

    bool check_address_valid(char buf[], size_t n)
    {
        int d = 3;
        uint32_t acc = 0;
        for (int i = 0; i < n; ++i)
        {
            if (buf[i] == '.')
            {
                d--;
                acc = 0;
            }
            else if (buf[i] >= '0' && buf[i] <= '9')
            {
                acc = acc * 10 + (buf[i] - '0');
            }
            if (d < 0 || acc > 255 || acc < 0)
            {
                return false;
            }
        }
        return true;
    }

    void run_client_network(std::shared_ptr<client_state> cs)
    {
        if (cs->gt_priority && !cs->gt_table.empty())
        {
            for (const GoldentTicket &gt : cs->gt_table)
            {
                client_tcp_pipeline(
                    (gt.proxy.address),
                    L1_COMMUNICATION_PORT,
                    [&gt, &cs](file_descriptor fd)
                    {
                        char buffer[1500] = "ping::gt::avil?\0";
                        if (write(fd, buffer, sizeof(buffer)) == -1)
                            return;
                        if (read(fd, buffer, sizeof(buffer)) == -1)
                            return;
                        if (strncmp(buffer, "ping::gt::avil!\0", 17) != 0)
                            return;
                        strcpy(buffer, gt.ticket.p);
                        if (write(fd, buffer, sizeof(buffer)) == -1)
                            return;
                        if (read(fd, buffer, sizeof(buffer)) == -1)
                            return;
                        if (strncmp(buffer, "ping::gt::appr!\0", 17) != 0)
                        {
                            cs->active_proxy = gt.proxy;
                        }
                    });
                if (cs->active_proxy.has_value())
                {
                    break;
                }
            }
        }
        while (true)
        {
            if (cs->active_proxy.has_value())
            {
                break;
            }
            if (cs->auto_connection_enabled)
            {
                for (const auto &res : cs->resolvers)
                {
                    client_tcp_pipeline(
                        res.address,
                        L3_COMMUNICATION_PORT,
                        [&cs](file_descriptor fd)
                        {
                            char buffer[1500] = "ping::ga\0";
                            if (write(fd, buffer, sizeof(buffer)) == -1)
                            {
                                return;
                            }
                            int nread = read(fd, buffer, sizeof(buffer));
                            if (nread < 2)
                            {
                                // -1 -- error
                                // 0 -- empty file
                                // zeroed file
                                return;
                            }
                            if (check_address_valid(buffer, nread))
                            {
                                Proxy pr;
                                strncpy(pr.address, buffer, nread);
                                cs->active_proxy = pr;
                            };
                        });
                    if (cs->active_proxy.has_value())
                        break;
                }
            }
        }

        // He has a proxxy
        run_tun();
    }

    void run_client()
    {
        std::shared_ptr<client_state> cs = get_client_state();
        std::jthread network_thread{run_client_network, cs};
        std::jthread gui_thread{run_client_gui, cs};
    }
} // namespace onixs