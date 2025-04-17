#ifndef _WIDGET_COMBO_BOX__
#define _WIDGET_COMBO_BOX__


#include <string>
#include <vector>

#include "imgui.h"

class WidgetComboBox
{
private:
    int item_selected_index;
    std::string combo_name;
    std::vector<std::string> combo_items;
    ImGuiComboFlags flags;
    ImGuiTextFilter filter;

public:
    WidgetComboBox(const std::string& _combo_name, const std::vector<std::string>& _combo_items, int _item_selected_index = 0, ImGuiComboFlags _flags = 0)
        : combo_name(_combo_name), combo_items(_combo_items), item_selected_index(_item_selected_index), flags(_flags) {}

    void set_combo_name(const std::string& _combo_name)
    {
        combo_name = _combo_name;
    }

    void set_combo_items(const std::vector<std::string>& _combo_items)
    {
        combo_items = _combo_items;
    }

    void set_combo_item_selected_index(int _item_selected_index)
    {
        item_selected_index = _item_selected_index;
    }

    void set_combo_flags(ImGuiComboFlags _flags)
    {
        flags = _flags;
    }

    int get_selected_index() const
    {
        return item_selected_index;
    }

    const std::string& get_selected_item() const
    {
        return combo_items[item_selected_index];
    }

    void DrawComboBox()
    {
        if (combo_items.empty()) return;

        const std::string& preview = combo_items[item_selected_index];

        if (ImGui::BeginCombo(combo_name.c_str(), preview.c_str(), flags))
        {
            if (ImGui::IsWindowAppearing())
            {
                ImGui::SetKeyboardFocusHere();
                filter.Clear();
            }

            ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
            filter.Draw("##Filter", -FLT_MIN);

            for (int n = 0; n < combo_items.size(); n++)
            {
                const bool is_selected = (item_selected_index == n);

                if (filter.PassFilter(combo_items[n].c_str()))
                {
                    if (ImGui::Selectable(combo_items[n].c_str(), is_selected))
                    {
                        item_selected_index = n;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }
    }
};

#endif // _WIDGET_COMBO_BOX__