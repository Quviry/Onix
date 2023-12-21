#pragma once

#include <functional>

#include "../imgui/imgui.h"
#include <GLFW/glfw3.h> 

namespace onix {
class Window {
   public:
    [[maybe_unused]] int start(std::function<void(GLFWwindow*, ImGuiIO&)> redraw_callback);
};
};  // namespace onix