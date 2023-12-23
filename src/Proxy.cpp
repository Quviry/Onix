#include "Proxy.hpp"
#include "Window.hpp"

namespace onix{
    Proxy::start(){
        Window window{};
        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        (void)window.start(
            [this, &show_demo_window, &show_another_window, &clear_color](GLFWwindow *window, ImGuiIO &io)
            {
                glfwPollEvents();

                // Start the Dear ImGui frame
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                ImGui::Begin("Hello, world!"); // GLOBAL HW
                ImGui::Text(state.c_str());
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
}