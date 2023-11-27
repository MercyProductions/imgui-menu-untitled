#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include <d3d11.h>
#include <tchar.h>

#include "imgui_settings.h"
#include <map>
#include <vector>
#include <string>

using namespace ImGui;
using namespace std;

namespace font {
    ImFont* poppins;
    ImFont* icon;
    ImFont* icon_combo;
}


namespace Custom {

    struct container {

        ImVec4 background, text;
        ImVec4 checkmark;
        float slider_slow;
        float tab_slow;

        bool opened_combo = false, hovered = false;
        float combo_size = 0.f, expand_rotation = 1.0f;

        float clip_mark;
        float circle_checkbox;
    };
    map<ImGuiID, container> checkboxanim;
    ////////////////////////////////////////////////////////////////////////////////////////////
    // CHECKBOX
    ////////////////////////////////////////////////////////////////////////////////////////////

    bool Checkbox(const char* label, bool* v)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);

        const float square_sz = 19, checkmark = 10.f;
        const float w = (GetContentRegionMax().x - style.WindowPadding.x) - square_sz;
        const ImVec2 pos = window->DC.CursorPos;
        const ImRect total_bb(pos, pos + ImVec2(square_sz + label_size.x + w, label_size.y));

        ItemSize(ImRect(total_bb.Min, total_bb.Max - ImVec2(0, 1)));

        if (!ItemAdd(total_bb, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(total_bb, id, &hovered, &held);

        if (IsItemClicked())
        {
            *v = !(*v);
            MarkItemEdited(id);
        }

        auto it_anim = checkboxanim.find(id);

        if (it_anim == checkboxanim.end())
        {
            checkboxanim.insert({ id, container() });
            it_anim = checkboxanim.find(id);
        }

        it_anim->second.background = ImLerp(it_anim->second.background, *v ? accent_color : hovered ? checkbox::bg_hov : checkbox::bg, g.IO.DeltaTime * 8.f);
        it_anim->second.checkmark = ImLerp(it_anim->second.checkmark, *v ? checkbox::checkmark : hovered ? checkbox::bg_hov : checkbox::bg, g.IO.DeltaTime * 8.f);
        it_anim->second.clip_mark = ImLerp(it_anim->second.clip_mark, *v ? checkmark : 0.f, g.IO.DeltaTime * 8.f);
        it_anim->second.text = ImLerp(it_anim->second.text, *v ? text::text_active : hovered ? text::text_hov : text::text, g.IO.DeltaTime * 8.f);

        const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));

        GetWindowDrawList()->AddRectFilled(check_bb.Min + ImVec2(w, 0), check_bb.Max + ImVec2(w, 0), GetColorU32(it_anim->second.background), checkbox::rounding);

        if (*v && IsRectVisible(check_bb.Min + ImVec2(w, 0), check_bb.Max + ImVec2(w, 0)))
            GetForegroundDrawList()->AddShadowRect(check_bb.Min + ImVec2(w + 5, 5), check_bb.Max + ImVec2(w - 5, -5), GetColorU32(accent_color), 30.f, ImVec2(0, 0), checkbox::rounding);

        ImGui::RenderCheckMark(window->DrawList, check_bb.Min + ImVec2((square_sz / 2) - (checkmark / 2) + w, (square_sz / 2) - (checkmark / 2)), GetColorU32(it_anim->second.checkmark), checkmark);

        ImGui::GetWindowDrawList()->AddText(ImVec2(check_bb.Max.x - square_sz, check_bb.Min.y - 1), GetColorU32(it_anim->second.text), label);

        return pressed;
    }

    map<ImGuiID, container> Switchboxanim;
    bool Switchbox(const char* label, bool* v)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);

        const float square_sz = 19, checkmark = 10.f;
        const float w = (GetContentRegionMax().x - style.WindowPadding.x) - square_sz;
        const ImVec2 pos = window->DC.CursorPos;
        const ImRect total_bb(pos, pos + ImVec2(square_sz + label_size.x + w, label_size.y));

        ItemSize(ImRect(total_bb.Min, total_bb.Max - ImVec2(0, 1)));

        if (!ItemAdd(total_bb, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(total_bb, id, &hovered, &held);

        if (IsItemClicked())
        {
            *v = !(*v);
            MarkItemEdited(id);
        }

        auto it_anim = Switchboxanim.find(id);

        if (it_anim == Switchboxanim.end())
        {
            Switchboxanim.insert({ id, container() });
            it_anim = Switchboxanim.find(id);
        }


        it_anim->second.circle_checkbox = ImLerp(it_anim->second.circle_checkbox, *v ? 19.f : 0.f, g.IO.DeltaTime * 8.f);
        it_anim->second.background = ImLerp(it_anim->second.background, *v ? accent_color : hovered ? checkbox::bg_hov : checkbox::bg, g.IO.DeltaTime * 8.f);
        it_anim->second.clip_mark = ImLerp(it_anim->second.clip_mark, *v ? checkmark : 0.f, g.IO.DeltaTime * 8.f);
        it_anim->second.text = ImLerp(it_anim->second.text, *v ? text::text_active : hovered ? text::text_hov : text::text, g.IO.DeltaTime * 8.f);

        const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));

        GetWindowDrawList()->AddRect(check_bb.Min + ImVec2(w - 20, 1), check_bb.Max + ImVec2(w, 1), GetColorU32(checkbox::bg), 100.f, ImDrawFlags_None, 2.5f);

        GetForegroundDrawList()->AddCircleFilled(check_bb.Min + ImVec2(w - 10 + it_anim->second.circle_checkbox, 10.2f), 5.5f, GetColorU32(it_anim->second.background), 100.f);

        if (*v && IsRectVisible(check_bb.Min + ImVec2(w, 0), check_bb.Max + ImVec2(w, 0)))
            GetForegroundDrawList()->AddShadowCircle(check_bb.Min + ImVec2(w - 10 + it_anim->second.circle_checkbox, 10), 3.f, GetColorU32(it_anim->second.background), 25.f, ImVec2(0, 0), ImDrawFlags_None, 100.f);

        ImGui::GetWindowDrawList()->AddText(ImVec2(check_bb.Max.x - square_sz, check_bb.Min.y - 1), GetColorU32(it_anim->second.text), label);

        return pressed;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    // SLIDER
    ////////////////////////////////////////////////////////////////////////////////////////////

    static const ImGuiDataTypeInfo GDataTypeInfo[] =
    {
        { sizeof(char),             "S8",   "%d",   "%d"    },  // ImGuiDataType_S8
        { sizeof(unsigned char),    "U8",   "%u",   "%u"    },
        { sizeof(short),            "S16",  "%d",   "%d"    },  // ImGuiDataType_S16
        { sizeof(unsigned short),   "U16",  "%u",   "%u"    },
        { sizeof(int),              "S32",  "%d",   "%d"    },  // ImGuiDataType_S32
        { sizeof(unsigned int),     "U32",  "%u",   "%u"    },
    #ifdef _MSC_VER
        { sizeof(ImS64),            "S64",  "%I64d","%I64d" },  // ImGuiDataType_S64
        { sizeof(ImU64),            "U64",  "%I64u","%I64u" },
    #else
        { sizeof(ImS64),            "S64",  "%lld", "%lld"  },  // ImGuiDataType_S64
        { sizeof(ImU64),            "U64",  "%llu", "%llu"  },
    #endif
        { sizeof(float),            "float", "%.3f","%f"    },  // ImGuiDataType_Float (float are promoted to double in va_arg)
        { sizeof(double),           "double","%f",  "%lf"   },  // ImGuiDataType_Double
    };
    std::map<ImGuiID, container> Slideranim;
    bool SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        const float w = 140;
        SetCursorPos(GetCursorPos() + ImVec2((GetContentRegionMax().x - style.WindowPadding.x) - w, 7));

        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, 15));

        const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? label_size.x : 0.0f, 0.0f));

        const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
        ItemSize(ImRect(total_bb.Min, total_bb.Max + ImVec2(0, 3)));
        if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0)) return false;

        if (format == NULL)
            format = DataTypeGetInfo(data_type)->PrintFmt;

        const bool hovered = ItemHoverable(frame_bb, id);
        bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
        if (!temp_input_is_active)
        {

            const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
            const bool clicked = hovered && IsMouseClicked(0, id);
            const bool make_active = (input_requested_by_tabbing || clicked || g.NavActivateId == id);
            if (make_active && clicked)
                SetKeyOwner(ImGuiKey_MouseLeft, id);
            if (make_active && temp_input_allowed)
                if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || (g.NavActivateId == id && (g.NavActivateFlags & ImGuiActivateFlags_PreferInput)))
                    temp_input_is_active = true;

            if (make_active && !temp_input_is_active)
            {
                SetActiveID(id, window);
                SetFocusID(id, window);
                FocusWindow(window);
                g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            }
        }

        auto it_anim = Slideranim.find(id);

        if (it_anim == Slideranim.end())
        {
            Slideranim.insert({ id, container() });
            it_anim = Slideranim.find(id);
        }

        it_anim->second.background = ImLerp(it_anim->second.background, hovered ? slider::bg_hov : slider::bg, g.IO.DeltaTime * 6.f);
        it_anim->second.text = ImLerp(it_anim->second.text, IsItemActive() ? text::text_active : hovered ? text::text_hov : text::text, g.IO.DeltaTime * 6.f);

        GetWindowDrawList()->AddRectFilled(frame_bb.Min + ImVec2(0, 5), frame_bb.Max - ImVec2(0, 5), GetColorU32(it_anim->second.background), slider::rounding);

        ImRect grab_bb;
        const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);

        if (value_changed) MarkItemEdited(id);

        if (grab_bb.Max.x > grab_bb.Min.x) {

            it_anim->second.slider_slow = ImLerp(it_anim->second.slider_slow, grab_bb.Min.x + 5 - frame_bb.Min.x, g.IO.DeltaTime * 20.f);

            GetWindowDrawList()->AddRectFilled(frame_bb.Min + ImVec2(0, 5), frame_bb.Min + ImVec2(it_anim->second.slider_slow - 3, 10), GetColorU32(accent_color_low), slider::rounding);
            GetWindowDrawList()->AddCircleFilled(ImVec2(frame_bb.Min.x + it_anim->second.slider_slow, grab_bb.Max.y - 6), 7.f, GetColorU32(accent_color), 20.f);

            if (IsRectVisible(total_bb.Min, total_bb.Max)) GetForegroundDrawList()->AddShadowCircle(ImVec2(frame_bb.Min.x + it_anim->second.slider_slow, grab_bb.Max.y - 6), 7.f, GetColorU32(accent_color), 20.f, ImVec2(0, 0), 20.f);

        }

        char value_buf[64];
        const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

        PushStyleColor(ImGuiCol_Text, GetColorU32(it_anim->second.text));
        RenderTextClipped(frame_bb.Min - ImVec2(w - 10, 4), frame_bb.Max + ImVec2(-w - 10, 4), value_buf, value_buf_end, NULL, ImVec2(1.0f, 0.5f));
        PopStyleColor();

        ImGui::GetWindowDrawList()->AddText(ImVec2(frame_bb.Max.x - GetContentRegionMax().x + style.WindowPadding.x, frame_bb.Min.y - 4), GetColorU32(it_anim->second.text), label);

        return value_changed;
    }

    bool SliderScalarN(const char* label, ImGuiDataType data_type, void* v, int components, const void* v_min, const void* v_max, const char* format, ImGuiSliderFlags flags)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        bool value_changed = false;
        BeginGroup();
        PushID(label);
        PushMultiItemsWidths(components, CalcItemWidth());
        size_t type_size = GDataTypeInfo[data_type].Size;
        for (int i = 0; i < components; i++)
        {
            PushID(i);
            if (i > 0)
                SameLine(0, g.Style.ItemInnerSpacing.x);
            value_changed |= SliderScalar("", data_type, v, v_min, v_max, format, flags);
            PopID();
            PopItemWidth();
            v = (void*)((char*)v + type_size);
        }
        PopID();

        const char* label_end = FindRenderedTextEnd(label);
        if (label != label_end)
        {
            SameLine(0, g.Style.ItemInnerSpacing.x);
            TextEx(label, label_end);
        }

        EndGroup();
        return value_changed;
    }

    bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
    {
        return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
    }

    bool SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
    {
        return SliderScalarN(label, ImGuiDataType_Float, v, 2, &v_min, &v_max, format, flags);
    }

    bool SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
    {
        return SliderScalarN(label, ImGuiDataType_Float, v, 3, &v_min, &v_max, format, flags);
    }

    bool SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
    {
        return SliderScalarN(label, ImGuiDataType_Float, v, 4, &v_min, &v_max, format, flags);
    }

    bool SliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max, const char* format, ImGuiSliderFlags flags)
    {
        if (format == NULL)
            format = "%.0f deg";
        float v_deg = (*v_rad) * 360.0f / (2 * IM_PI);
        bool value_changed = SliderFloat(label, &v_deg, v_degrees_min, v_degrees_max, format, flags);
        *v_rad = v_deg * (2 * IM_PI) / 360.0f;
        return value_changed;
    }

    bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
    {
        return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
    }

    bool SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
    {
        return SliderScalarN(label, ImGuiDataType_S32, v, 2, &v_min, &v_max, format, flags);
    }

    bool SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
    {
        return SliderScalarN(label, ImGuiDataType_S32, v, 3, &v_min, &v_max, format, flags);
    }

    bool SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
    {
        return SliderScalarN(label, ImGuiDataType_S32, v, 4, &v_min, &v_max, format, flags);
    }

    bool VSliderScalar(const char* label, const ImVec2& size, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        const ImVec2 label_size = CalcTextSize(label, NULL, true);
        const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size);
        const ImRect bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(frame_bb, id))
            return false;

        // Default format string when passing NULL
        if (format == NULL)
            format = DataTypeGetInfo(data_type)->PrintFmt;

        const bool hovered = ItemHoverable(frame_bb, id);
        const bool clicked = hovered && IsMouseClicked(0, id);
        if (clicked || g.NavActivateId == id)
        {
            if (clicked)
                SetKeyOwner(ImGuiKey_MouseLeft, id);
            SetActiveID(id, window);
            SetFocusID(id, window);
            FocusWindow(window);
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Up) | (1 << ImGuiDir_Down);
        }

        // Draw frame
        const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
        RenderNavHighlight(frame_bb, id);
        RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding);

        // Slider behavior
        ImRect grab_bb;
        const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags | ImGuiSliderFlags_Vertical, &grab_bb);
        if (value_changed)
            MarkItemEdited(id);

        // Render grab
        if (grab_bb.Max.y > grab_bb.Min.y)
            window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), style.GrabRounding);

        // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
        // For the vertical slider we allow centered text to overlap the frame padding
        char value_buf[64];
        const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
        RenderTextClipped(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.0f));
        if (label_size.x > 0.0f)
            RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

        return value_changed;
    }

    bool VSliderFloat(const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
    {
        return VSliderScalar(label, size, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
    }

    bool VSliderInt(const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
    {
        return VSliderScalar(label, size, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    // COMBO
    ////////////////////////////////////////////////////////////////////////////////////////////

    static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
    {
        const char* const* items = (const char* const*)data;
        if (out_text)
            *out_text = items[idx];
        return true;
    }

    // Getter for the old Combo() API: "item1\0item2\0item3\0"
    static bool Items_SingleStringGetter(void* data, int idx, const char** out_text)
    {
        // FIXME-OPT: we could pre-compute the indices to fasten this. But only 1 active combo means the waste is limited.
        const char* items_separated_by_zeros = (const char*)data;
        int items_count = 0;
        const char* p = items_separated_by_zeros;
        while (*p)
        {
            if (idx == items_count)
                break;
            p += strlen(p) + 1;
            items_count++;
        }
        if (!*p)
            return false;
        if (out_text)
            *out_text = p;
        return true;
    }

    static float CalcMaxPopupHeightFromItemCount(int items_count)
    {
        ImGuiContext& g = *GImGui;
        if (items_count <= 0)
            return FLT_MAX;
        return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
    }

    struct comboA_state
    {
        bool active = false;
        float alpha = 0.f;
    };

    struct multi_state
    {
        bool active = false;
        float alpha = 0.f;
    };
    std::map<ImGuiID, comboA_state> Comboanim;
    void MultiCombo(const char* label, bool variable[], const char* labels[], int count)
    {
        ImGuiContext& g = *GImGui;

        std::string preview = "None";

        std::vector<std::string> vec;

        for (auto i = 0, j = 0; i < count; i++)
        {
            if (variable[i])
            {
                vec.push_back(labels[i]);

                if (j >= 3)
                    preview = vec.at(0) + ", " + vec.at(1) + ", " + vec.at(2) + "...";
                else if (j)
                    preview += ", " + (std::string)labels[i];
                else
                    preview = labels[i];

                j++;
            }
        }

        if (ImGui::BeginCombo(label, preview.c_str(), count)) // draw start
        {

            auto it_anim = Comboanim.find(g.CurrentWindow->ID);

            if (it_anim == Comboanim.end())
            {
                Comboanim.insert({ g.CurrentWindow->ID, comboA_state() });
                it_anim = Comboanim.find(g.CurrentWindow->ID);
            }

            if (ImGuiWindow* popup_window = ImGui::FindWindowByName(g.CurrentWindow->Name))
                if (popup_window->WasActive)
                {
                    it_anim->second.active = true;
                }
                else
                {
                    it_anim->second.active = false;
                    it_anim->second.alpha = 0.f;
                }

            it_anim->second.alpha = ImClamp(it_anim->second.alpha + (8.f * ImGui::GetIO().DeltaTime * (it_anim->second.active * it_anim->second.active ? 1.f : -1.f)), 0.f, 1.f);
            PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(g.Style.FramePadding.x, 4));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 4));
            for (auto i = 0; i < count; i++)
                ImGui::Selectable(labels[i], &variable[i], ImGuiSelectableFlags_DontClosePopups);
            ImGui::PopStyleVar(2);
            ImGui::EndCombo();
        }

        preview = ("None");

    }

    // Call directly after the BeginCombo/EndCombo block. The preview is designed to only host non-interactive elements
    // (Experimental, see GitHub issues: #1658, #4168)
    bool BeginComboPreview()
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImGuiComboPreviewData* preview_data = &g.ComboPreviewData;

        if (window->SkipItems || !(g.LastItemData.StatusFlags & ImGuiItemStatusFlags_Visible))
            return false;
        IM_ASSERT(g.LastItemData.Rect.Min.x == preview_data->PreviewRect.Min.x && g.LastItemData.Rect.Min.y == preview_data->PreviewRect.Min.y); // Didn't call after BeginCombo/EndCombo block or forgot to pass ImGuiComboFlags_CustomPreview flag?
        if (!window->ClipRect.Contains(preview_data->PreviewRect)) // Narrower test (optional)
            return false;

        // FIXME: This could be contained in a PushWorkRect() api
        preview_data->BackupCursorPos = window->DC.CursorPos;
        preview_data->BackupCursorMaxPos = window->DC.CursorMaxPos;
        preview_data->BackupCursorPosPrevLine = window->DC.CursorPosPrevLine;
        preview_data->BackupPrevLineTextBaseOffset = window->DC.PrevLineTextBaseOffset;
        preview_data->BackupLayout = window->DC.LayoutType;
        window->DC.CursorPos = preview_data->PreviewRect.Min + g.Style.FramePadding;
        window->DC.CursorMaxPos = window->DC.CursorPos;
        window->DC.LayoutType = ImGuiLayoutType_Horizontal;
        window->DC.IsSameLine = false;
        PushClipRect(preview_data->PreviewRect.Min, preview_data->PreviewRect.Max, true);

        return true;
    }

    void EndComboPreview()
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        ImGuiComboPreviewData* preview_data = &g.ComboPreviewData;

        // FIXME: Using CursorMaxPos approximation instead of correct AABB which we will store in ImDrawCmd in the future
        ImDrawList* draw_list = window->DrawList;
        if (window->DC.CursorMaxPos.x < preview_data->PreviewRect.Max.x && window->DC.CursorMaxPos.y < preview_data->PreviewRect.Max.y)
            if (draw_list->CmdBuffer.Size > 1) // Unlikely case that the PushClipRect() didn't create a command
            {
                draw_list->_CmdHeader.ClipRect = draw_list->CmdBuffer[draw_list->CmdBuffer.Size - 1].ClipRect = draw_list->CmdBuffer[draw_list->CmdBuffer.Size - 2].ClipRect;
                draw_list->_TryMergeDrawCmds();
            }
        PopClipRect();
        window->DC.CursorPos = preview_data->BackupCursorPos;
        window->DC.CursorMaxPos = ImMax(window->DC.CursorMaxPos, preview_data->BackupCursorMaxPos);
        window->DC.CursorPosPrevLine = preview_data->BackupCursorPosPrevLine;
        window->DC.PrevLineTextBaseOffset = preview_data->BackupPrevLineTextBaseOffset;
        window->DC.LayoutType = preview_data->BackupLayout;
        window->DC.IsSameLine = false;
        preview_data->PreviewRect = ImRect();
    }


    bool Combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
    {
        ImGuiContext& g = *GImGui;

        // Call the getter to obtain the preview string which is a parameter to BeginCombo()
        const char* preview_value = NULL;
        if (*current_item >= 0 && *current_item < items_count)
            items_getter(data, *current_item, &preview_value);

        // The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
        if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
            SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

        if (!BeginCombo(label, preview_value, items_count, ImGuiComboFlags_None))
            return false;

        // Display items
        // FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
        bool value_changed = false;

        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 4));
        for (int i = 0; i < items_count; i++)
        {
            PushID(i);
            const bool item_selected = (i == *current_item);
            const char* item_text;
            if (!items_getter(data, i, &item_text))
                item_text = "*Unknown item*";
            if (Selectable(item_text, item_selected))
            {
                value_changed = true;
                *current_item = i;
            }
            if (item_selected)
                SetItemDefaultFocus();
            PopID();
        }
        PopStyleVar(1);
        EndCombo();

        if (value_changed)
            MarkItemEdited(g.LastItemData.ID);

        return value_changed;
    }

    bool Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
    {
        const bool value_changed = Combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
        return value_changed;
    }

    // Combo box helper allowing to pass all items in a single string literal holding multiple zero-terminated items "item1\0item2\0"
    bool Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items)
    {
        int items_count = 0;
        const char* p = items_separated_by_zeros;       // FIXME-OPT: Avoid computing this, or at least only when combo is open
        while (*p)
        {
            p += strlen(p) + 1;
            items_count++;
        }
        bool value_changed = Combo(label, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, items_count, height_in_items);
        return value_changed;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    // COLORPICKER
    ////////////////////////////////////////////////////////////////////////////////////////////

    static void ColorEditRestoreHS(const float* col, float* H, float* S, float* V)
    {
        ImGuiContext& g = *GImGui;
        IM_ASSERT(g.ColorEditCurrentID != 0);
        if (g.ColorEditSavedID != g.ColorEditCurrentID || g.ColorEditSavedColor != ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 0)))
            return;

        // When S == 0, H is undefined.
        // When H == 1 it wraps around to 0.
        if (*S == 0.0f || (*H == 0.0f && g.ColorEditSavedHue == 1))
            *H = g.ColorEditSavedHue;

        // When V == 0, S is undefined.
        if (*V == 0.0f)
            *S = g.ColorEditSavedSat;
    }

    bool ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const float square_sz = GetFrameHeight();
        const float w_full = CalcItemWidth();
        const float w_button = (flags & ImGuiColorEditFlags_NoSmallPreview) ? 0.0f : (square_sz + style.ItemInnerSpacing.x);
        const float w_inputs = w_full - w_button;
        const char* label_display_end = FindRenderedTextEnd(label);
        g.NextItemData.ClearFlags();

        BeginGroup();
        PushID(label);
        const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
        if (set_current_color_edit_id)
            g.ColorEditCurrentID = window->IDStack.back();

        // If we're not showing any slider there's no point in doing any HSV conversions
        const ImGuiColorEditFlags flags_untouched = flags;
        if (flags & ImGuiColorEditFlags_NoInputs)
            flags = (flags & (~ImGuiColorEditFlags_DisplayMask_)) | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions;

        // Context menu: display and modify options (before defaults are applied)
        if (!(flags & ImGuiColorEditFlags_NoOptions))
            ColorEditOptionsPopup(col, flags);

        // Read stored options
        if (!(flags & ImGuiColorEditFlags_DisplayMask_))
            flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DisplayMask_);
        if (!(flags & ImGuiColorEditFlags_DataTypeMask_))
            flags |= (g.ColorEditOptions & ImGuiColorEditFlags_DataTypeMask_);
        if (!(flags & ImGuiColorEditFlags_PickerMask_))
            flags |= (g.ColorEditOptions & ImGuiColorEditFlags_PickerMask_);
        if (!(flags & ImGuiColorEditFlags_InputMask_))
            flags |= (g.ColorEditOptions & ImGuiColorEditFlags_InputMask_);
        flags |= (g.ColorEditOptions & ~(ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_));
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_DisplayMask_)); // Check that only 1 is selected
        IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiColorEditFlags_InputMask_));   // Check that only 1 is selected

        const bool alpha = (flags & ImGuiColorEditFlags_NoAlpha) == 0;
        const bool hdr = (flags & ImGuiColorEditFlags_HDR) != 0;
        const int components = alpha ? 4 : 3;

        // Convert to the formats we need
        float f[4] = { col[0], col[1], col[2], alpha ? col[3] : 1.0f };
        if ((flags & ImGuiColorEditFlags_InputHSV) && (flags & ImGuiColorEditFlags_DisplayRGB))
            ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
        else if ((flags & ImGuiColorEditFlags_InputRGB) && (flags & ImGuiColorEditFlags_DisplayHSV))
        {
            // Hue is lost when converting from grayscale rgb (saturation=0). Restore it.
            ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);
            ColorEditRestoreHS(col, &f[0], &f[1], &f[2]);
        }
        int i[4] = { IM_F32_TO_INT8_UNBOUND(f[0]), IM_F32_TO_INT8_UNBOUND(f[1]), IM_F32_TO_INT8_UNBOUND(f[2]), IM_F32_TO_INT8_UNBOUND(f[3]) };

        bool value_changed = false;
        bool value_changed_as_float = false;

        const ImVec2 pos = window->DC.CursorPos;
        const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;
        window->DC.CursorPos.x = pos.x + inputs_offset_x;

        if ((flags & (ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV)) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
        {
            // RGB/HSV 0..255 Sliders
            const float w_item_one = ImMax(1.0f, IM_FLOOR((w_inputs - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
            const float w_item_last = ImMax(1.0f, IM_FLOOR(w_inputs - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));

            const bool hide_prefix = (w_item_one <= CalcTextSize((flags & ImGuiColorEditFlags_Float) ? "M:0.000" : "M:000").x);
            static const char* ids[4] = { "##X", "##Y", "##Z", "##W" };
            static const char* fmt_table_int[3][4] =
            {
                {   "%3d",   "%3d",   "%3d",   "%3d" }, // Short display
                { "R:%3d", "G:%3d", "B:%3d", "A:%3d" }, // Long display for RGBA
                { "H:%3d", "S:%3d", "V:%3d", "A:%3d" }  // Long display for HSVA
            };
            static const char* fmt_table_float[3][4] =
            {
                {   "%0.3f",   "%0.3f",   "%0.3f",   "%0.3f" }, // Short display
                { "R:%0.3f", "G:%0.3f", "B:%0.3f", "A:%0.3f" }, // Long display for RGBA
                { "H:%0.3f", "S:%0.3f", "V:%0.3f", "A:%0.3f" }  // Long display for HSVA
            };
            const int fmt_idx = hide_prefix ? 0 : (flags & ImGuiColorEditFlags_DisplayHSV) ? 2 : 1;

            for (int n = 0; n < components; n++)
            {
                if (n > 0)
                    SameLine(0, style.ItemInnerSpacing.x);
                SetNextItemWidth((n + 1 < components) ? w_item_one : w_item_last);

                // FIXME: When ImGuiColorEditFlags_HDR flag is passed HS values snap in weird ways when SV values go below 0.
                if (flags & ImGuiColorEditFlags_Float)
                {
                    value_changed |= DragFloat(ids[n], &f[n], 1.0f / 255.0f, 0.0f, hdr ? 0.0f : 1.0f, fmt_table_float[fmt_idx][n]);
                    value_changed_as_float |= value_changed;
                }
                else
                {
                    value_changed |= DragInt(ids[n], &i[n], 1.0f, 0, hdr ? 0 : 255, fmt_table_int[fmt_idx][n]);
                }
                if (!(flags & ImGuiColorEditFlags_NoOptions))
                    OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
            }
        }
        else if ((flags & ImGuiColorEditFlags_DisplayHex) != 0 && (flags & ImGuiColorEditFlags_NoInputs) == 0)
        {
            // RGB Hexadecimal Input
            char buf[64];
            if (alpha)
                ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255), ImClamp(i[3], 0, 255));
            else
                ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));
            SetNextItemWidth(w_inputs);

            if (!(flags & ImGuiColorEditFlags_NoOptions))
                OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
        }

        ImGuiWindow* picker_active_window = NULL;
        if (!(flags & ImGuiColorEditFlags_NoSmallPreview))
        {
            const float button_offset_x = ((flags & ImGuiColorEditFlags_NoInputs) || (style.ColorButtonPosition == ImGuiDir_Left)) ? 0.0f : w_inputs + style.ItemInnerSpacing.x;
            window->DC.CursorPos = ImVec2(pos.x + button_offset_x, pos.y);

            const ImVec4 col_v4(col[0], col[1], col[2], alpha ? col[3] : 1.0f);
            if (ColorButton("##ColorButton", col_v4, flags))
            {
                if (!(flags & ImGuiColorEditFlags_NoPicker))
                {
                    // Store current color and open a picker
                    g.ColorPickerRef = col_v4;
                    OpenPopup("picker");
                    SetNextWindowPos(g.LastItemData.Rect.GetBL() + ImVec2(0.0f, style.ItemSpacing.y));
                }
            }
            if (!(flags & ImGuiColorEditFlags_NoOptions))
                OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);

            PushStyleColor(ImGuiCol_PopupBg, GetColorU32(picker::bg));
            PushStyleVar(ImGuiStyleVar_PopupRounding, (picker::rounding));

            if (BeginPopup("picker"))
            {
                if (g.CurrentWindow->BeginCount == 1)
                {
                    picker_active_window = g.CurrentWindow;

                    ImGuiColorEditFlags picker_flags_to_forward = ImGuiColorEditFlags_DataTypeMask_ | ImGuiColorEditFlags_PickerMask_ | ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaBar;
                    ImGuiColorEditFlags picker_flags = (flags_untouched & picker_flags_to_forward) | ImGuiColorEditFlags_DisplayMask_ | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreviewHalf;
                    SetNextItemWidth(square_sz * 12.0f); // Use 256 + bar sizes?
                    value_changed |= ColorPicker4("##picker", col, picker_flags, &g.ColorPickerRef.x);
                }
                EndPopup();
            }
        }

        PopStyleVar(1);
        PopStyleColor(1);

        char buf[64];

        SameLine(0.0f, style.ItemInnerSpacing.x);
        window->DC.CursorPos.x = pos.x + ((flags & ImGuiColorEditFlags_NoInputs) ? w_button : w_full + style.ItemInnerSpacing.x);

        if (label != label_display_end && !(flags & ImGuiColorEditFlags_NoLabel)) GetWindowDrawList()->AddText(window->DC.CursorPos - ImVec2(w_button, -1), GetColorU32(text::text), label);

        ImFormatString(buf, IM_ARRAYSIZE(buf), "#%02X%02X%02X", ImClamp(i[0], 0, 255), ImClamp(i[1], 0, 255), ImClamp(i[2], 0, 255));
        // GetWindowDrawList()->AddText(window->DC.CursorPos + ImVec2(GetWindowContentRegionMax().x - 140, 1), GetColorU32(text::text), buf);

        PushStyleColor(ImGuiCol_Text, GetColorU32(text::text));
        ImGui::RenderTextClipped(window->DC.CursorPos + ImVec2(GetWindowContentRegionMax().x - 350, 0), window->DC.CursorPos + ImVec2(GetWindowContentRegionMax().x - 80, 19), buf, NULL, NULL, ImVec2(1.0f, 0.5f));
        PopStyleColor();

        // Convert back
        if (value_changed && picker_active_window == NULL)
        {
            if (!value_changed_as_float)
                for (int n = 0; n < 4; n++)
                    f[n] = i[n] / 255.0f;
            if ((flags & ImGuiColorEditFlags_DisplayHSV) && (flags & ImGuiColorEditFlags_InputRGB))
            {
                g.ColorEditSavedHue = f[0];
                g.ColorEditSavedSat = f[1];
                ColorConvertHSVtoRGB(f[0], f[1], f[2], f[0], f[1], f[2]);
                g.ColorEditSavedID = g.ColorEditCurrentID;
                g.ColorEditSavedColor = ColorConvertFloat4ToU32(ImVec4(f[0], f[1], f[2], 0));
            }
            if ((flags & ImGuiColorEditFlags_DisplayRGB) && (flags & ImGuiColorEditFlags_InputHSV))
                ColorConvertRGBtoHSV(f[0], f[1], f[2], f[0], f[1], f[2]);

            col[0] = f[0];
            col[1] = f[1];
            col[2] = f[2];
            if (alpha)
                col[3] = f[3];
        }

        if (set_current_color_edit_id)
            g.ColorEditCurrentID = 0;
        PopID();
        EndGroup();

        // Drag and Drop Target
        // NB: The flag test is merely an optional micro-optimization, BeginDragDropTarget() does the same test.
        if ((g.LastItemData.StatusFlags & ImGuiItemStatusFlags_HoveredRect) && !(flags & ImGuiColorEditFlags_NoDragDrop) && BeginDragDropTarget())
        {
            bool accepted_drag_drop = false;
            if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
            {
                memcpy((float*)col, payload->Data, sizeof(float) * 3); // Preserve alpha if any //-V512 //-V1086
                value_changed = accepted_drag_drop = true;
            }
            if (const ImGuiPayload* payload = AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
            {
                memcpy((float*)col, payload->Data, sizeof(float) * components);
                value_changed = accepted_drag_drop = true;
            }

            // Drag-drop payloads are always RGB
            if (accepted_drag_drop && (flags & ImGuiColorEditFlags_InputHSV))
                ColorConvertRGBtoHSV(col[0], col[1], col[2], col[0], col[1], col[2]);
            EndDragDropTarget();
        }

        // When picker is being actively used, use its active id so IsItemActive() will function on ColorEdit4().
        if (picker_active_window && g.ActiveId != 0 && g.ActiveIdWindow == picker_active_window)
            g.LastItemData.ID = g.ActiveId;

        if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
            MarkItemEdited(g.LastItemData.ID);

        return value_changed;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    // BEGINCHILD
    ////////////////////////////////////////////////////////////////////////////////////////////

    bool BeginChildEx(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags)
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* parent_window = g.CurrentWindow;

        flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;
        flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);  // Inherit the NoMove flag

        // Size
        const ImVec2 content_avail = GetContentRegionAvail();
        ImVec2 size = ImFloor(size_arg);
        const int auto_fit_axises = ((size.x == 0.0f) ? (1 << ImGuiAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << ImGuiAxis_Y) : 0x00);
        if (size.x <= 0.0f)
            size.x = ImMax(content_avail.x + size.x, 4.0f);
        if (size.y <= 0.0f)
            size.y = ImMax(content_avail.y + size.y, 4.0f);
        SetNextWindowPos(ImVec2(parent_window->DC.CursorPos + ImVec2(0, 35)));
        SetNextWindowSize(ImVec2(size.x, (size.y - 35)));

        GetWindowDrawList()->AddRectFilled(parent_window->DC.CursorPos + ImVec2(0, 0), parent_window->DC.CursorPos + ImVec2(size_arg.x, size_arg.y), GetColorU32(child::bg), child::rounding);
        GetWindowDrawList()->AddRect(parent_window->DC.CursorPos + ImVec2(0, 0), parent_window->DC.CursorPos + ImVec2(size_arg), GetColorU32(child::border), child::rounding, ImDrawFlags_None, 1.f);

        GetWindowDrawList()->AddLine(parent_window->DC.CursorPos + ImVec2(0, 35), parent_window->DC.CursorPos + ImVec2(size_arg.x, 35), GetColorU32(child::line), 1.f);

        ImGui::GetWindowDrawList()->AddText(parent_window->DC.CursorPos + ImVec2(25, 35 - ImGui::CalcTextSize(name).y) / 2.f, ImGui::GetColorU32(text::text_active), name);

        const char* temp_window_name;
        if (name)
            ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);
        else
            ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

        const float backup_border_size = g.Style.ChildBorderSize;
        if (!border) g.Style.ChildBorderSize = 0.0f;
        bool ret = Begin(temp_window_name, NULL, flags);
        g.Style.ChildBorderSize = backup_border_size;

        ImGuiWindow* child_window = g.CurrentWindow;
        child_window->ChildId = id;
        child_window->AutoFitChildAxises = (ImS8)auto_fit_axises;

        if (child_window->BeginCount == 1)
            parent_window->DC.CursorPos = child_window->Pos;

        if (g.NavActivateId == id && !(flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavHasScroll))
        {
            FocusWindow(child_window);
            NavInitWindow(child_window, false);
            SetActiveID(id + 1, child_window);
            g.ActiveIdSource = ImGuiInputSource_Nav;
        }
        return ret;
    }

    bool BeginChild(const char* str_id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags)
    {
        ImGuiWindow* window = GetCurrentWindow();

        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 9));
        return BeginChildEx(str_id, window->GetID(str_id), size_arg, border, extra_flags | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoBackground);
    }

    bool BeginChild(ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags)
    {
        IM_ASSERT(id != 0);
        return BeginChildEx(NULL, id, size_arg, border, extra_flags);
    }

    void EndChild()
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;

        PopStyleVar(2);

        IM_ASSERT(g.WithinEndChild == false);
        IM_ASSERT(window->Flags & ImGuiWindowFlags_ChildWindow);

        g.WithinEndChild = true;
        if (window->BeginCount > 1)
        {
            End();
        }
        else
        {
            ImVec2 sz = window->Size;
            if (window->AutoFitChildAxises & (1 << ImGuiAxis_X))
                sz.x = ImMax(4.0f, sz.x);
            if (window->AutoFitChildAxises & (1 << ImGuiAxis_Y))
                sz.y = ImMax(4.0f, sz.y);
            End();

            ImGuiWindow* parent_window = g.CurrentWindow;
            ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + sz);
            ItemSize(sz);
            if ((window->DC.NavLayersActiveMask != 0 || window->DC.NavHasScroll) && !(window->Flags & ImGuiWindowFlags_NavFlattened))
            {
                ItemAdd(bb, window->ChildId);
                RenderNavHighlight(bb, window->ChildId);

                if (window->DC.NavLayersActiveMask == 0 && window == g.NavWindow)
                    RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavHighlightFlags_TypeThin);
            }
            else
            {
                ItemAdd(bb, 0);
            }
            if (g.HoveredWindow == window)
                g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
        }
        g.WithinEndChild = false;
        g.LogLinePosY = -FLT_MAX;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    // TABS
    ////////////////////////////////////////////////////////////////////////////////////////////

    std::map<ImGuiID, container> tabanim;
    bool Tabs(bool selected, const char* ico, const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
    {
        ImGuiWindow* window = GetCurrentWindow();

        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;

        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);

        PushFont(font::icon);
        const ImVec2 ico_size = CalcTextSize(ico, NULL, true);
        PopFont();

        ImVec2 pos = window->DC.CursorPos, size = CalcItemSize(size_arg, label_size.x, label_size.y);

        const ImRect bb(pos, pos + size);
        ItemSize(size, 0.f);

        if (!ItemAdd(bb, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

        auto it_anim = tabanim.find(id);

        if (it_anim == tabanim.end())
        {
            tabanim.insert({ id, container() });
            it_anim = tabanim.find(id);
        }

        it_anim->second.background = ImLerp(it_anim->second.background, selected ? accent_color : ImVec4(0, 0, 0, 0), g.IO.DeltaTime * 6.f);
        it_anim->second.text = ImLerp(it_anim->second.text, selected ? text::text_active : text::text_hov, g.IO.DeltaTime * 6.f);

        GetWindowDrawList()->AddRectFilled(bb.Min + ImVec2(0, size_arg.y - 3), bb.Max, GetColorU32(it_anim->second.background), 10.f, ImDrawCornerFlags_Top);

        PushStyleColor(ImGuiCol_Text, GetColorU32(it_anim->second.text));

        PushFont(font::icon);
        RenderTextClipped(bb.Min, bb.Max, ico, NULL, &ico_size, ImVec2(0.5f, 0.26f), &bb);
        PopFont();

        RenderTextClipped(bb.Min, bb.Max, label, NULL, &label_size, ImVec2(0.5f, 0.74f), &bb);

        PopStyleColor();

        PushStyleVar(ImGuiStyleVar_Alpha, 0.1f);
        GetWindowDrawList()->AddRectFilledMultiColor(bb.Min + ImVec2(0, (size_arg.y / 2.3f)), bb.Max, GetColorU32(accent_alpha), GetColorU32(accent_alpha), GetColorU32(it_anim->second.background), GetColorU32(it_anim->second.background));
        PopStyleVar();

        ImGui::SameLine(0, 0);

        return pressed;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    // INPUT-TEXT
    ////////////////////////////////////////////////////////////////////////////////////////////

    bool InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
    {
        IM_ASSERT(!(flags & ImGuiInputTextFlags_Multiline)); // call InputTextMultiline()
        return InputTextEx(label, NULL, buf, (int)buf_size, ImVec2(0, 0), flags, callback, user_data);
    }

    bool InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
    {
        return InputTextEx(label, NULL, buf, (int)buf_size, size, flags | ImGuiInputTextFlags_Multiline, callback, user_data);
    }

    bool InputTextWithHint(const char* label, const char* hint, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
    {
        IM_ASSERT(!(flags & ImGuiInputTextFlags_Multiline)); // call InputTextMultiline() or  InputTextEx() manually if you need multi-line + hint.
        return InputTextEx(label, hint, buf, (int)buf_size, ImVec2(0, 0), flags, callback, user_data);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////
    // SEPARATOR
    ////////////////////////////////////////////////////////////////////////////////////////////

    void SeparatorEx(ImGuiSeparatorFlags flags)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return;

        ImGuiContext& g = *GImGui;
        IM_ASSERT(ImIsPowerOfTwo(flags & (ImGuiSeparatorFlags_Horizontal | ImGuiSeparatorFlags_Vertical)));   // Check that only 1 option is selected

        const float thickness = 1.0f; // Cannot use g.Style.SeparatorTextSize yet for various reasons.
        if (flags & ImGuiSeparatorFlags_Vertical)
        {
            // Vertical separator, for menu bars (use current line height).
            float y1 = window->DC.CursorPos.y;
            float y2 = window->DC.CursorPos.y + window->DC.CurrLineSize.y;
            const ImRect bb(ImVec2(window->DC.CursorPos.x, y1), ImVec2(window->DC.CursorPos.x + thickness, y2));
            ItemSize(ImVec2(thickness, 0.0f));
            if (!ItemAdd(bb, 0))
                return;

            window->DrawList->AddRectFilled(bb.Min, bb.Max, GetColorU32(separator::bg));

        }
        else if (flags & ImGuiSeparatorFlags_Horizontal)
        {

            float x1 = window->Pos.x;
            float x2 = window->Pos.x + window->Size.x;

            if (g.GroupStack.Size > 0 && g.GroupStack.back().WindowID == window->ID)
                x1 += window->DC.Indent.x;

            if (ImGuiTable* table = g.CurrentTable)
            {
                x1 = table->Columns[table->CurrentColumn].MinX;
                x2 = table->Columns[table->CurrentColumn].MaxX;
            }

            ImGuiOldColumns* columns = (flags & ImGuiSeparatorFlags_SpanAllColumns) ? window->DC.CurrentColumns : NULL;

            if (columns) PushColumnsBackground();

            const float thickness_for_layout = (thickness == 1.0f) ? 0.0f : thickness;
            const ImRect bb(ImVec2(x1, window->DC.CursorPos.y), ImVec2(x2, window->DC.CursorPos.y + thickness));
            ItemSize(ImVec2(0.0f, thickness_for_layout));
            const bool item_visible = ItemAdd(bb, 0);
            if (item_visible)
            {
                // Draw
                window->DrawList->AddRectFilled(bb.Min + ImVec2(GetStyle().WindowPadding.x, 0), bb.Max - ImVec2(GetStyle().WindowPadding.x, 0), GetColorU32(separator::bg));
                if (g.LogEnabled)
                    LogRenderedText(&bb.Min, "--------------------------------\n");

            }
            if (columns)
            {
                PopColumnsBackground();
                columns->LineMinY = window->DC.CursorPos.y;
            }
        }
    }

    void Separator()
    {
        ImGuiContext& g = *GImGui;
        ImGuiWindow* window = g.CurrentWindow;
        if (window->SkipItems)
            return;

        ImGuiSeparatorFlags flags = (window->DC.LayoutType == ImGuiLayoutType_Horizontal) ? ImGuiSeparatorFlags_Vertical : ImGuiSeparatorFlags_Horizontal;
        flags |= ImGuiSeparatorFlags_SpanAllColumns;
        SeparatorEx(flags);
    }


    std::map<ImGuiID, container> buttonanim;
    bool Button(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
    {
        ImGuiWindow* window = GetCurrentWindow();

        if (window->SkipItems) return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);
        const ImVec2 label_size = CalcTextSize(label, NULL, true);

        ImVec2 pos = window->DC.CursorPos;

        ImVec2 size = CalcItemSize(size_arg, label_size.x, label_size.y);

        const ImRect bb(pos, pos + size);

        ItemSize(size, 0.f);
        if (!ItemAdd(bb, id)) return false;

        bool hovered, held, pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

        auto it_anim = buttonanim.find(id);

        if (it_anim == buttonanim.end())
        {
            buttonanim.insert({ id, container() });
            it_anim = buttonanim.find(id);
        }

        it_anim->second.background = ImLerp(it_anim->second.background, IsItemActive() ? button::bg_active : hovered ? button::bg_hov : button::bg, g.IO.DeltaTime * 6.f);

        GetWindowDrawList()->AddRectFilled(bb.Min, bb.Max, GetColorU32(it_anim->second.background), button::rounding);

        RenderTextClipped(bb.Min, bb.Max, label, NULL, &label_size, ImVec2(0.5, 0.5), &bb);

        return pressed;
    }

    const char* keys[] =
    {
        "None",
        "Mouse 1",
        "Mouse 2",
        "CN",
        "Mouse 3",
        "Mouse 4",
        "Mouse 5",
        "-",
        "Back",
        "Tab",
        "-",
        "-",
        "CLR",
        "Enter",
        "-",
        "-",
        "Shift",
        "CTL",
        "Menu",
        "Pause",
        "Caps Lock",
        "KAN",
        "-",
        "JUN",
        "FIN",
        "KAN",
        "-",
        "Escape",
        "CON",
        "NCO",
        "ACC",
        "MAD",
        "Space",
        "PGU",
        "PGD",
        "End",
        "Home",
        "Left",
        "Up",
        "Right",
        "Down",
        "SEL",
        "PRI",
        "EXE",
        "PRI",
        "INS",
        "Delete",
        "HEL",
        "0",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "A",
        "B",
        "C",
        "D",
        "E",
        "F",
        "G",
        "H",
        "I",
        "J",
        "K",
        "L",
        "M",
        "N",
        "O",
        "P",
        "Q",
        "R",
        "S",
        "T",
        "U",
        "V",
        "W",
        "X",
        "Y",
        "Z",
        "WIN",
        "WIN",
        "APP",
        "-",
        "SLE",
        "Numpad 0",
        "Numpad 1",
        "Numpad 2",
        "Numpad 3",
        "Numpad 4",
        "Numpad 5",
        "Numpad 6",
        "Numpad 7",
        "Numpad 8",
        "Numpad 9",
        "MUL",
        "ADD",
        "SEP",
        "MIN",
        "Delete",
        "DIV",
        "F1",
        "F2",
        "F3",
        "F4",
        "F5",
        "F6",
        "F7",
        "F8",
        "F9",
        "F10",
        "F11",
        "F12",
        "F13",
        "F14",
        "F15",
        "F16",
        "F17",
        "F18",
        "F19",
        "F20",
        "F21",
        "F22",
        "F23",
        "F24",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "NUM",
        "SCR",
        "EQU",
        "MAS",
        "TOY",
        "OYA",
        "OYA",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "-",
        "Shift",
        "Shift",
        "Ctrl",
        "Ctrl",
        "Alt",
        "Alt"
    };

    struct key_state
    {
        bool active = false;
        bool hovered = false;
        float alpha = 0.f;
    };
    std::map<ImGuiID, key_state> keyanim;
    bool Keybind(const char* label, int* key, int* mode)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        ImGuiIO& io = g.IO;
        const ImGuiStyle& style = g.Style;

        const ImGuiID id = window->GetID(label);
        const float width = GetContentRegionMax().x - ImGui::GetStyle().WindowPadding.x;

        const ImRect rect(window->DC.CursorPos, window->DC.CursorPos + ImVec2(width, 22.f));

        ImGui::ItemSize(rect, style.FramePadding.y);
        if (!ImGui::ItemAdd(rect, id))
            return false;

        char buf_display[64] = "None";


        bool value_changed = false;
        int k = *key;

        std::string active_key = "";
        active_key += keys[*key];

        if (*key != 0 && g.ActiveId != id) {
            strcpy_s(buf_display, active_key.c_str());
        }
        else if (g.ActiveId == id) {
            strcpy_s(buf_display, "...");
        }

        const ImVec2 label_size = CalcTextSize(buf_display, NULL, true);

        ImRect clickable(ImVec2(rect.Max.x - 10 - label_size.x, rect.Min.y), rect.Max);
        bool hovered = ItemHoverable(clickable, id);


        window->DrawList->AddRectFilled(clickable.Min + ImVec2(0, 1), clickable.Max + ImVec2(0, 0), GetColorU32(keybind::bg), 2.f);
        window->DrawList->AddText(rect.Min - ImVec2(0, -1), GetColorU32(text::text), label);

        PushStyleColor(ImGuiCol_Text, text::text_active);
        RenderTextClipped(clickable.Min + ImVec2(0, 1), clickable.Max + ImVec2(0, 1), buf_display, NULL, &label_size, ImVec2(0.5f, 0.5f));
        PopStyleColor();

        if (hovered && io.MouseClicked[0])
        {
            if (g.ActiveId != id) {
                // Start edition
                memset(io.MouseDown, 0, sizeof(io.MouseDown));
                memset(io.KeysDown, 0, sizeof(io.KeysDown));
                *key = 0;
            }
            ImGui::SetActiveID(id, window);
            ImGui::FocusWindow(window);
        }
        else if (io.MouseClicked[0]) {
            // Release focus when we click outside
            if (g.ActiveId == id)
                ImGui::ClearActiveID();
        }

        if (g.ActiveId == id) {
            for (auto i = 0; i < 5; i++) {
                if (io.MouseDown[i]) {
                    switch (i) {
                    case 0:
                        k = 0x01;
                        break;
                    case 1:
                        k = 0x02;
                        break;
                    case 2:
                        k = 0x04;
                        break;
                    case 3:
                        k = 0x05;
                        break;
                    case 4:
                        k = 0x06;
                        break;
                    }
                    value_changed = true;
                    ImGui::ClearActiveID();
                }
            }
            if (!value_changed) {
                for (auto i = 0x08; i <= 0xA5; i++) {
                    if (io.KeysDown[i]) {
                        k = i;
                        value_changed = true;
                        ImGui::ClearActiveID();
                    }
                }
            }

            if (IsKeyPressedMap(ImGuiKey_Escape)) {
                *key = 0;
                ImGui::ClearActiveID();
            }
            else {
                *key = k;
            }
        }
        auto it_anim = keyanim.find(id);

        if (it_anim == keyanim.end())
        {
            keyanim.insert({ id, key_state() });
            it_anim = keyanim.find(id);
        }

        if (hovered && g.IO.MouseClicked[1] || it_anim->second.active && (g.IO.MouseClicked[0] || g.IO.MouseClicked[1]) && !it_anim->second.hovered)
            it_anim->second.active = !it_anim->second.active;

        it_anim->second.alpha = ImClamp(it_anim->second.alpha + (8.f * g.IO.DeltaTime * (it_anim->second.active ? 1.f : -1.f)), 0.f, 1.f);

        SetNextWindowPos(ImVec2(clickable.GetCenter().x - 34, clickable.Max.y + 5));
        SetNextWindowSize(ImVec2(69, 74));
        PushStyleVar(ImGuiStyleVar_Alpha, it_anim->second.alpha);
        PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 8));
        PushStyleVar(ImGuiStyleVar_WindowRounding, 2.f);
        PushStyleColor(ImGuiCol_WindowBg, keybind::bg);
        PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
        if (it_anim->second.alpha >= 0.01f)
        {
            Begin(label, NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
            {
                it_anim->second.hovered = IsWindowHovered();

                if (Selectable("Hold", *mode == 0))
                {
                    *mode = 0;
                    it_anim->second.active = false;
                }
                if (Selectable("Toggle", *mode == 1))
                {
                    *mode = 1;
                    it_anim->second.active = false;
                }
                if (Selectable("Always", *mode == 2))
                {
                    *mode = 2;
                    it_anim->second.active = false;
                }
            }
            End();
        }
        PopStyleColor(1);
        PopStyleVar(4);

        return value_changed;
    }

}