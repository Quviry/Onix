#include "proxy.hpp"
#include "Window.hpp"
#include "datatypes.h"
#include "tcp.hpp"

#include "../imgui.h"
#include "../backends/imgui_impl_glfw.h"
#include "../backends/imgui_impl_opengl3.h"
#include <thread>
#include <memory>

constexpr int MAX_PING_CONNECTIONS = 8;

namespace onix
{

    std::shared_ptr<proxy_state> get_proxy_state()
    {
        auto ps = std::make_shared<proxy_state>();
        ps->connection_types = static_cast<uint8_t>(ConnectionType::ICMP | ConnectionType::TCP | ConnectionType::UDP);
        return ps;
    }

    void ping_access(std::shared_ptr<proxy_state> ps)
    {
        for (;;)
        {
            for (const auto &resolver : ps->resolvers)
            {

                // std::sstring(reesollveerr.adddreeeesss)
                if (strstr(resolver.address, ":") == nullptr)
                {
                    client_tcp_pipeline(std::string(resolver.address), 4000, [](int fd)
                                        {
                    char buffer[30];
                    write(fd, buffer, 30); });
                }
                else
                {
                }
            }
        };
    }

    void run_proxy_network(std::shared_ptr<proxy_state> ps)
    {
        std::jthread{ping_access, ps};
    }

    void run_proxy_gui(std::shared_ptr<proxy_state> ps)
    {
        Window window{};
        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        (void)window.start(
            [&show_demo_window, &show_another_window, &clear_color](GLFWwindow *window, ImGuiIO &io)
            {
                glfwPollEvents();

                // Start the Dear ImGui frame
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                ImGui::Begin("Hello, world!"); // GLOBAL HW
                ImGui::End();

                // 2. Show a simple window that we create ourselves. We use a Begin/End
                // pair to create a named window.
                {
                    static float f = 0.0f;
                    static int counter = 0;

                    ImGui::Begin("Hello, world!"); // Create a window called "Hello,
                                                   // world!" and append into it.

                    ImGui::Text(
                        "This is some useful text."); // Display some text (you can use
                                                      // a format strings too)
                    ImGui::Checkbox("Demo Window",
                                    &show_demo_window); // Edit bools storing our
                                                        // window open/close state
                    ImGui::Checkbox("Another Window", &show_another_window);

                    ImGui::SliderFloat(
                        "float", &f, 0.0f,
                        1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
                    ImGui::ColorEdit3(
                        "clear color",
                        (float *)&clear_color); // Edit 3 floats representing a color

                    if (ImGui::Button(
                            "Button")) // Buttons return true when clicked (most
                                       // widgets return true when edited/activated)
                        counter++;
                    ImGui::SameLine();
                    ImGui::Text("counter = %d", counter);

                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                                1000.0f / io.Framerate, io.Framerate);
                    ImGui::End();
                }

                // 3. Show another simple window.
                if (show_another_window)
                {
                    ImGui::Begin(
                        "Another Window",
                        &show_another_window); // Pass a pointer to our bool variable
                                               // (the window will have a closing
                                               // button that will clear the bool when
                                               // clicked)
                    ImGui::Text("Hello from another window!");
                    if (ImGui::Button("Close Me"))
                        show_another_window = false;
                    ImGui::End();
                }

                // Rendering
                ImGui::Render();
                int display_w, display_h;
                glfwGetFramebufferSize(window, &display_w, &display_h);
                glViewport(0, 0, display_w, display_h);
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