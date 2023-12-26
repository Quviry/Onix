#include "widgets.hpp"

#include "tcp.hpp"

#include "../imgui.h"
#include <vector>
#include <string>
#include <algorithm>

void ResolversListTable(std::vector<Resolver> &resolvers)
{
    if (ImGui::BeginTable("Resolvers Table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {

        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Address");
        ImGui::TableSetupColumn("Status");
        ImGui::TableSetupColumn("Dlt");
        ImGui::TableHeadersRow();

        for (int i = 0; i < resolvers.size(); ++i)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", resolvers[i].name);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", resolvers[i].address);
            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted("unknown");

            ImGui::TableSetColumnIndex(3);
            if (ImGui::Button("Delete"))
            {
                ImGui::OpenPopup("Delete Resolver");
            }

            if (ImGui::BeginPopupModal("Delete Resolver", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Are you sure?");
                if (ImGui::Button("Yes, delete", ImVec2(120, 0)))
                {
                    resolvers.erase(std::next(resolvers.begin(), i));
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
        }
        ImGui::EndTable();
    }
}

void TicketSaveButton(GTTable gt, char address_buffer[], size_t nb_s, char token_buffer[], size_t ab_s){
     if (ImGui::Button("Save", ImVec2(120, 0)))
    {
        if (check_address_valid(address_buffer, nb_s))
        {
            GoldentTicket ticket;
            strncpy(ticket.proxy.address, address_buffer, ab_s);
            strncpy(ticket.ticket.p, token_buffer, nb_s);
            gt.push_back(ticket);
            ImGui::CloseCurrentPopup();
        }
        else
        {
            ImGui::OpenPopup("Wrong_address_ft");

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(.5f, 0.5f));
            if (ImGui::BeginPopupModal("Wrong_address_ft", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Wrong address type or format");
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
    }
}


void ResolversSaveButton(
    std::vector<Resolver> &resolvers, char name_buffer[],
    size_t nb_s, char address_buffer[], size_t ab_s)
{
    if (ImGui::Button("Save", ImVec2(120, 0)))
    {
        if (check_address_valid(address_buffer, nb_s))
        {
            Resolver *new_resolver = new Resolver{};
            strncpy(new_resolver->address, address_buffer, ab_s);
            strncpy(new_resolver->name, name_buffer, nb_s);
            resolvers.push_back(*new_resolver);
            delete new_resolver;
            ImGui::CloseCurrentPopup();
        }
        else
        {
            ImGui::OpenPopup("Wrong_address");

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(.5f, 0.5f));
            if (ImGui::BeginPopupModal("Wrong_address", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Wrong address type or format");
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }
    }
}

struct ExampleAppLog
{
    ImGuiTextBuffer Buf;
    ImGuiTextFilter Filter;
    ImVector<int> LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool AutoScroll;           // Keep scrolling if already at the bottom.

    ExampleAppLog()
    {
        AutoScroll = true;
        Clear();
    }

    void Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void AddLog(const char *fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
    }

    void Draw(const char *title, bool *p_open = NULL)
    {
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }

        // Main window
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        bool clear = ImGui::Button("Save");
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy to clipboard");
        ImGui::SameLine();
        bool close = ImGui::Button("Close");

        ImGui::Separator();

        if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (clear)
                Clear();
            if (copy)
                ImGui::LogToClipboard();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            const char *buf = Buf.begin();
            const char *buf_end = Buf.end();
            if (Filter.IsActive())
            {
                // In this example we don't use the clipper when Filter is enabled.
                // This is because we don't have random access to the result of our filter.
                // A real application processing logs with ten of thousands of entries may want to store the result of
                // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
                for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
                {
                    const char *line_start = buf + LineOffsets[line_no];
                    const char *line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    if (Filter.PassFilter(line_start, line_end))
                        ImGui::TextUnformatted(line_start, line_end);
                }
            }
            else
            {
                // The simplest and easy way to display the entire buffer:
                //   ImGui::TextUnformatted(buf_begin, buf_end);
                // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
                // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
                // within the visible area.
                // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
                // on your side is recommended. Using ImGuiListClipper requires
                // - A) random access into your data
                // - B) items all being the  same height,
                // both of which we can handle since we have an array pointing to the beginning of each line of text.
                // When using the filter (in the block of code above) we don't have random access into the data to display
                // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
                // it possible (and would be recommended if you want to search through tens of thousands of entries).
                ImGuiListClipper clipper;
                clipper.Begin(LineOffsets.Size);
                while (clipper.Step())
                {
                    for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                    {
                        const char *line_start = buf + LineOffsets[line_no];
                        const char *line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                        ImGui::TextUnformatted(line_start, line_end);
                    }
                }
                clipper.End();
            }
            ImGui::PopStyleVar();

            // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
            // Using a scrollbar or mouse-wheel will take away from the bottom edge.
            if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
        ImGui::End();
    }
};


void TokensListTable(GTTable gt){
    if (ImGui::BeginTable("Tickets Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {

        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("View");
        ImGui::TableSetupColumn("Dlt");
        ImGui::TableHeadersRow();

        for (int i = 0; i < gt.size(); ++i)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", gt[i].proxy.name);
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", gt[i].ticket.p);

            ImGui::TableSetColumnIndex(2);
            if (ImGui::Button("Delete"))
            {
                ImGui::OpenPopup("Delete Ticket");
            }

            if (ImGui::BeginPopupModal("Delete Ticket", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("Are you sure?");
                if (ImGui::Button("Yes, delete", ImVec2(120, 0)))
                {
                    gt.erase(std::next(gt.begin(), i));
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
        }
        ImGui::EndTable();
    }
}

namespace onix
{
    void ShowAppLog()
    {
        static ExampleAppLog log;

        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos({20, 20});
        ImGui::Begin("Log");
        
        ImGui::End();
        log.AddLog("2023-10-31 00:55:40 [info] Application waming up...\n");
        log.AddLog("2023-10-31 00:55:40 [info] Mang: TCP Socket Listening on [AF_INET]127.0.0.1:25343\n");
        log.AddLog("2023-10-31 00:55:40 [info] Mang: Awaition hold release...\n");
        log.AddLog("2023-10-31 00:55:40 [info] Mang: Client connected to [AF_INET]127.0.0.1:25343\n");
        log.AddLog("2023-10-31 00:55:40 [info] Mang: RECV address from [AF_INET]127.0.0.1:25343\n");
        log.AddLog("2023-10-31 00:55:40 [info] Mang: AP fast_connect\n");
        log.AddLog("2023-10-31 00:55:40 [info] Mang: AP TCP connect\n");

        log.Draw("Log", nullptr);
    }

    bool ResolversList(std::vector<Resolver> &resolvers)
    {
        ResolversListTable(resolvers);

        if (ImGui::Button("Add Resolver"))
            ImGui::OpenPopup("Resolvers");

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(.5f, 0.5f));
        if (ImGui::BeginPopupModal("Resolvers", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static char name_buffer[sizeof(Resolver::name)];
            static char address_buffer[sizeof(Resolver::address)];
            ImGui::InputText("Name", name_buffer, sizeof(name_buffer));
            ImGui::InputText("Address", address_buffer, sizeof(address_buffer));

            ResolversSaveButton(resolvers, name_buffer, sizeof(name_buffer), address_buffer, sizeof(address_buffer));

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

    bool TokensList(GTTable &gt)
    {
        TokensListTable(gt);

        if (ImGui::Button("Add Token"))
            ImGui::OpenPopup("Tokens");

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(.5f, 0.5f));
        if (ImGui::BeginPopupModal("Tokens", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static char address_buffer[sizeof(GoldentTicket::proxy)];
            static char token_buffer[sizeof(GoldentTicket::ticket)];
            ImGui::InputText("Address", address_buffer, sizeof(address_buffer));
            ImGui::InputText("Token", token_buffer, sizeof(token_buffer));

            TicketSaveButton(gt, address_buffer, sizeof(address_buffer), token_buffer, sizeof(token_buffer));

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