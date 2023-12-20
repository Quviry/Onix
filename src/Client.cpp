#include "Client.h"

#include <stdio.h>

#include "../imgui/imgui.h"

void Client::start() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // Build atlas
    unsigned char* tex_pixels = nullptr;
    int tex_w, tex_h;
    io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);
    for (int n = 0; n < 2000; n++) {
        printf("NewFrame() %d\n", n);
        io.DisplaySize = ImVec2(1920, 1080);
        io.DeltaTime = 1.0f / 60.0f;
        ImGui::NewFrame();
        static float f = 0.0f;
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / io.Framerate, io.Framerate);
        ImGui::ShowDemoWindow(nullptr);
        ImGui::Render();
    }
    printf("DestroyContext()\n");
    ImGui::DestroyContext();
}