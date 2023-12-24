#include "widgets.hpp"

#include "../imgui.h"
#include <vector>
#include <string>
#include <algorithm>

namespace onix
{
    bool ResolversList(std::vector<Resolver> &resolvers)
    {
        if (ImGui::BeginTable("Resolvers Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {

            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Address");
            ImGui::TableSetupColumn("Status");
            ImGui::TableHeadersRow();

            for (const auto& item : resolvers)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(item.name);
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(item.address);
                ImGui::TableSetColumnIndex(2);
                ImGui::TextUnformatted("unknown");
            }
            ImGui::EndTable();
        }

        if (ImGui::Button("Add Resolver"))
            ImGui::OpenPopup("Resolvers");

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(.5f, 1.5f));
        if (ImGui::BeginPopupModal("Resolvers", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static char buffer[50];
            ImGui::InputText("Address", buffer, 50);
            if (ImGui::Button("Save", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        return true;
    }
}