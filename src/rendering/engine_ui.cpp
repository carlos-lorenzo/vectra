#include "vectra/rendering/engine_ui.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


void EngineUI::apply_dracula_theme()
{
    ImGuiStyle& style = ImGui::GetStyle();

    // Dracula color palette
    ImVec4 background = ImVec4(0.157f, 0.165f, 0.212f, 1.0f);      // #282a36
    ImVec4 current_line = ImVec4(0.267f, 0.278f, 0.353f, 1.0f);    // #44475a
    ImVec4 foreground = ImVec4(0.973f, 0.973f, 0.949f, 1.0f);      // #f8f8f2
    ImVec4 comment = ImVec4(0.384f, 0.447f, 0.643f, 1.0f);         // #6272a4
    ImVec4 purple = ImVec4(0.741f, 0.576f, 0.976f, 1.0f);          // #bd93f9
    ImVec4 green = ImVec4(0.314f, 0.980f, 0.482f, 1.0f);           // #50fa7b
    ImVec4 pink = ImVec4(1.0f, 0.475f, 0.776f, 1.0f);              // #ff79c6
    ImVec4 cyan = ImVec4(0.545f, 0.914f, 0.992f, 1.0f);            // #8be9fd
    ImVec4 red = ImVec4(1.0f, 0.333f, 0.333f, 1.0f);               // #ff5555
    ImVec4 orange = ImVec4(1.0f, 0.725f, 0.424f, 1.0f);            // #ffb86c

    // Godot-inspired clean styling
    style.WindowRounding = 4.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.TabRounding = 4.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
    style.IndentSpacing = 20.0f;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;

    ImVec4* colors = style.Colors;

    // Window
    colors[ImGuiCol_WindowBg] = background;
    colors[ImGuiCol_ChildBg] = background;
    colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.19f, 0.24f, 0.95f);

    // Borders
    colors[ImGuiCol_Border] = current_line;
    colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    // Frame (input fields, checkboxes, etc.)
    colors[ImGuiCol_FrameBg] = current_line;
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.31f, 0.32f, 0.40f, 1.0f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.36f, 0.45f, 1.0f);

    // Title bar
    colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.13f, 0.17f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.15f, 0.19f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.12f, 0.13f, 0.17f, 0.75f);

    // Menu bar
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.15f, 0.19f, 1.0f);

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.13f, 0.17f, 1.0f);
    colors[ImGuiCol_ScrollbarGrab] = current_line;
    colors[ImGuiCol_ScrollbarGrabHovered] = comment;
    colors[ImGuiCol_ScrollbarGrabActive] = purple;

    // Buttons
    colors[ImGuiCol_Button] = current_line;
    colors[ImGuiCol_ButtonHovered] = ImVec4(purple.x, purple.y, purple.z, 0.7f);
    colors[ImGuiCol_ButtonActive] = purple;

    // Headers (collapsing headers, tree nodes, selectables, menu items)
    colors[ImGuiCol_Header] = current_line;
    colors[ImGuiCol_HeaderHovered] = ImVec4(purple.x, purple.y, purple.z, 0.5f);
    colors[ImGuiCol_HeaderActive] = ImVec4(purple.x, purple.y, purple.z, 0.7f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.19f, 0.24f, 1.0f);
    colors[ImGuiCol_TabHovered] = ImVec4(purple.x, purple.y, purple.z, 0.7f);
    colors[ImGuiCol_TabActive] = ImVec4(purple.x, purple.y, purple.z, 0.5f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.15f, 0.19f, 1.0f);
    colors[ImGuiCol_TabUnfocusedActive] = current_line;

    // Separator
    colors[ImGuiCol_Separator] = current_line;
    colors[ImGuiCol_SeparatorHovered] = purple;
    colors[ImGuiCol_SeparatorActive] = purple;

    // Resize grip
    colors[ImGuiCol_ResizeGrip] = current_line;
    colors[ImGuiCol_ResizeGripHovered] = purple;
    colors[ImGuiCol_ResizeGripActive] = purple;

    // Docking
    colors[ImGuiCol_DockingPreview] = ImVec4(purple.x, purple.y, purple.z, 0.5f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.12f, 0.13f, 0.17f, 1.0f);

    // Text
    colors[ImGuiCol_Text] = foreground;
    colors[ImGuiCol_TextDisabled] = comment;

    // Checkbox, slider
    colors[ImGuiCol_CheckMark] = green;
    colors[ImGuiCol_SliderGrab] = purple;
    colors[ImGuiCol_SliderGrabActive] = ImVec4(purple.x, purple.y, purple.z, 0.8f);

    // Plot
    colors[ImGuiCol_PlotLines] = cyan;
    colors[ImGuiCol_PlotLinesHovered] = pink;
    colors[ImGuiCol_PlotHistogram] = green;
    colors[ImGuiCol_PlotHistogramHovered] = pink;

    // Table
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.18f, 0.19f, 0.24f, 1.0f);
    colors[ImGuiCol_TableBorderStrong] = current_line;
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.22f, 0.23f, 0.29f, 1.0f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.02f);

    // Nav highlight
    colors[ImGuiCol_NavHighlight] = purple;
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);

    // Modal dim
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.6f);
}

void EngineUI::initialize(GLFWwindow* window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Get DPI scale from the primary monitor
    float dpi_scale = 1.0f;
    GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
    if (primary_monitor)
    {
        float x_scale, y_scale;
        glfwGetMonitorContentScale(primary_monitor, &x_scale, &y_scale);
        dpi_scale = x_scale;  // Use x_scale (typically same as y_scale)
    }

    // Load JetBrains Mono font with DPI-scaled size
    float base_font_size = 15.0f;
    float scaled_font_size = base_font_size * dpi_scale;

    io.Fonts->AddFontFromFileTTF(
        "resources/fonts/jetbrains_mono/JetBrainsMono-Regular.ttf",
        scaled_font_size
    );

    // Scale ImGui style to match DPI
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(dpi_scale);

    // Apply Dracula theme (after scaling so theme values are correct)
    apply_dracula_theme();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void EngineUI::setup_initial_dock_layout(ImGuiID dockspace_id)
{
    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->WorkSize);

    // Split: main area and right panel (Inspector)
    ImGuiID dock_main_id = dockspace_id;
    ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.25f, nullptr, &dock_main_id);

    // Split main: left panel (Hierarchy) and center (Scene View)
    ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);

    // Split top from center for Toolbar
    ImGuiID dock_top_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.06f, nullptr, &dock_main_id);

    // Dock windows
    ImGui::DockBuilderDockWindow("Toolbar", dock_top_id);
    ImGui::DockBuilderDockWindow("Hierarchy", dock_left_id);
    ImGui::DockBuilderDockWindow("Inspector", dock_right_id);
    ImGui::DockBuilderDockWindow("Scene View", dock_main_id);

    ImGui::DockBuilderFinish(dockspace_id);
}

void EngineUI::draw_toolbar(EngineState& state)
{
    ImGuiWindowFlags toolbar_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));

    if (ImGui::Begin("Toolbar", nullptr, toolbar_flags))
    {
        // Play/Pause button
        ImVec4 play_color = state.is_paused ?
            ImVec4(0.314f, 0.980f, 0.482f, 1.0f) :  // Green when paused (click to play)
            ImVec4(1.0f, 0.725f, 0.424f, 1.0f);      // Orange when playing (click to pause)

        ImGui::PushStyleColor(ImGuiCol_Button, play_color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(play_color.x * 0.8f, play_color.y * 0.8f, play_color.z * 0.8f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(play_color.x * 0.6f, play_color.y * 0.6f, play_color.z * 0.6f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

        const char* play_label = state.is_paused ? "  Play  " : "  Pause  ";
        if (ImGui::Button(play_label))
        {
            state.is_paused = !state.is_paused;
        }
        ImGui::PopStyleColor(4);

        ImGui::SameLine();

        // Restart button
        ImVec4 restart_color = ImVec4(0.741f, 0.576f, 0.976f, 1.0f);  // Purple color
        ImGui::PushStyleColor(ImGuiCol_Button, restart_color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(restart_color.x * 0.8f, restart_color.y * 0.8f, restart_color.z * 0.8f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(restart_color.x * 0.6f, restart_color.y * 0.6f, restart_color.z * 0.6f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

        if (ImGui::Button("  Restart  "))
        {
            state.scene_should_restart = true;
            state.is_paused = true;
        }
        ImGui::PopStyleColor(4);

        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();

        // Simulation speed slider
        ImGui::SetNextItemWidth(150.0f);
        auto sim_speed = static_cast<float>(state.simulation_speed);
        ImGui::SliderFloat("##Speed", &sim_speed, 0.0f, 5.0f, "%.2fx");
        state.simulation_speed = static_cast<linkit::real>(sim_speed);

        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
        ImGui::SameLine();

        // FPS display
        ImGui::TextColored(ImVec4(0.545f, 0.914f, 0.992f, 1.0f), "FPS: %.1f", ImGui::GetIO().Framerate);

        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.384f, 0.447f, 0.643f, 1.0f), "(%.2f ms)", 1000.0f / ImGui::GetIO().Framerate);
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void EngineUI::draw_hierarchy(const SceneSnapshot& scene_snapshot)
{
    if (ImGui::Begin("Hierarchy"))
    {
        ImGui::TextColored(ImVec4(0.545f, 0.914f, 0.992f, 1.0f), "Scene Objects");
        ImGui::Separator();

        for (int i = 0; i < static_cast<int>(scene_snapshot.object_snapshots.size()); ++i)
        {
            const auto& obj = scene_snapshot.object_snapshots[i];

            bool is_selected = (selected_object_index_ == i);

            // Use object name, fall back to model_name_index if empty
            std::string display_name = obj.name.empty() ?
                (obj.model_name + "_" + std::to_string(i)) : obj.name;

            if (ImGui::Selectable(display_name.c_str(), is_selected))
            {
                selected_object_index_ = i;
            }

            // Highlight selected item
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
    }
    ImGui::End();
}

void EngineUI::draw_inspector(const SceneSnapshot& scene_snapshot)
{
    if (ImGui::Begin("Inspector"))
    {
        if (selected_object_index_ >= 0 &&
            selected_object_index_ < static_cast<int>(scene_snapshot.object_snapshots.size()))
        {
            const auto& obj = scene_snapshot.object_snapshots[selected_object_index_];

            // Object name header
            std::string display_name = obj.name.empty() ?
                (obj.model_name + "_" + std::to_string(selected_object_index_)) : obj.name;

            ImGui::TextColored(ImVec4(0.741f, 0.576f, 0.976f, 1.0f), "%s", display_name.c_str());
            ImGui::Separator();

            // Transform section
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent(10.0f);

                // Position (read-only)
                ImGui::TextColored(ImVec4(0.545f, 0.914f, 0.992f, 1.0f), "Position");
                float pos[3] = {
                    static_cast<float>(obj.transform.position.x),
                    static_cast<float>(obj.transform.position.y),
                    static_cast<float>(obj.transform.position.z)
                };
                ImGui::BeginDisabled();
                ImGui::InputFloat3("##pos", pos, "%.3f");
                ImGui::EndDisabled();

                ImGui::Spacing();

                // Rotation (read-only) - display as quaternion
                ImGui::TextColored(ImVec4(0.545f, 0.914f, 0.992f, 1.0f), "Rotation (Quaternion)");
                float rot[4] = {
                    static_cast<float>(obj.transform.rotation.x),
                    static_cast<float>(obj.transform.rotation.y),
                    static_cast<float>(obj.transform.rotation.z),
                    static_cast<float>(obj.transform.rotation.w)
                };
                ImGui::BeginDisabled();
                ImGui::InputFloat4("##rot", rot, "%.3f");
                ImGui::EndDisabled();

                ImGui::Spacing();

                // Scale (read-only)
                ImGui::TextColored(ImVec4(0.545f, 0.914f, 0.992f, 1.0f), "Scale");
                float scale[3] = {
                    static_cast<float>(obj.transform.scale.x),
                    static_cast<float>(obj.transform.scale.y),
                    static_cast<float>(obj.transform.scale.z)
                };
                ImGui::BeginDisabled();
                ImGui::InputFloat3("##scale", scale, "%.3f");
                ImGui::EndDisabled();

                ImGui::Unindent(10.0f);
            }

            ImGui::Spacing();

            // Model info section
            if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent(10.0f);
                ImGui::Text("Model: %s", obj.model_name.c_str());
                ImGui::Unindent(10.0f);
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(0.384f, 0.447f, 0.643f, 1.0f), "Select an object from the Hierarchy");
        }
    }
    ImGui::End();
}

void EngineUI::draw_scene_view(EngineState& state, GLuint scene_texture_id)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    if (ImGui::Begin("Scene View"))
    {
        // Update scene_view_focused based on window state
        state.scene_view_focused = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();

        // Get available size for the scene view
        ImVec2 available_size = ImGui::GetContentRegionAvail();

        // Update engine state with new size (ensure minimum size)
        int new_width = static_cast<int>(available_size.x);
        int new_height = static_cast<int>(available_size.y);

        if (new_width > 0 && new_height > 0)
        {
            state.scene_view_width = new_width;
            state.scene_view_height = new_height;
        }

        // Display the framebuffer texture
        // Flip UV vertically because OpenGL textures are bottom-up
        if (scene_texture_id != 0 && available_size.x > 0 && available_size.y > 0)
        {
            ImGui::Image(
                (void*)(intptr_t)scene_texture_id,
                available_size,
                ImVec2(0, 1),  // UV top-left (flipped)
                ImVec2(1, 0)   // UV bottom-right (flipped)
            );
        }

        // Show hint when not focused
        if (!state.scene_view_focused)
        {
            ImVec2 window_pos = ImGui::GetWindowPos();
            ImVec2 text_pos = ImVec2(window_pos.x + 10, window_pos.y + available_size.y - 25);
            ImGui::GetWindowDrawList()->AddText(
                text_pos,
                IM_COL32(100, 100, 100, 200),
                "Click to control camera (ESC to release)"
            );
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void EngineUI::draw(EngineState& state, SceneSnapshot& scene_snapshot, GLuint scene_texture_id)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Setup the main DockSpace window to cover the entire viewport
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    // Configure flags for dockspace container
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // Stylize to remove padding/borders for the root dock window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace Viewport", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // Creates the actual dock node
    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");

    // Setup initial dock layout on first frame
    if (first_frame_)
    {
        setup_initial_dock_layout(dockspace_id);
        first_frame_ = false;
    }

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    ImGui::End();

    // Draw all panels
    draw_toolbar(state);
    draw_hierarchy(scene_snapshot);
    draw_inspector(scene_snapshot);
    draw_scene_view(state, scene_texture_id);

    // Draw restart overlay if scene is restarting
    if (state.scene_should_restart)
    {
        draw_restart_overlay();
    }
}

void EngineUI::draw_restart_overlay()
{
    // Get viewport dimensions
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Semi-transparent dark overlay covering the entire screen
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGuiWindowFlags overlay_flags = ImGuiWindowFlags_NoDecoration |
                                     ImGuiWindowFlags_NoMove |
                                     ImGuiWindowFlags_NoSavedSettings |
                                     ImGuiWindowFlags_NoFocusOnAppearing |
                                     ImGuiWindowFlags_NoNav |
                                     ImGuiWindowFlags_NoInputs;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.6f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("##RestartOverlay", nullptr, overlay_flags);

    // Calculate center position for the notification box
    ImVec2 center = ImVec2(viewport->WorkPos.x + viewport->WorkSize.x * 0.5f,
                           viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);

    // Notification box dimensions
    float box_width = 280.0f;
    float box_height = 100.0f;
    ImVec2 box_min = ImVec2(center.x - box_width * 0.5f, center.y - box_height * 0.5f);
    ImVec2 box_max = ImVec2(center.x + box_width * 0.5f, center.y + box_height * 0.5f);

    // Draw rounded rectangle background (Dracula current_line color)
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(box_min, box_max,
                             IM_COL32(68, 71, 90, 240),  // #44475a with high alpha
                             8.0f);

    // Draw border (Dracula purple)
    draw_list->AddRect(box_min, box_max,
                       IM_COL32(189, 147, 249, 255),  // #bd93f9
                       8.0f, 0, 2.0f);

    // Draw "Restarting Scene..." text
    const char* title_text = "Restarting Scene...";
    ImVec2 title_size = ImGui::CalcTextSize(title_text);
    ImVec2 title_pos = ImVec2(center.x - title_size.x * 0.5f, center.y - 20.0f);
    draw_list->AddText(title_pos,
                       IM_COL32(248, 248, 242, 255),  // #f8f8f2 foreground
                       title_text);

    // Animated loading dots based on time
    float time = static_cast<float>(ImGui::GetTime());
    int dot_count = static_cast<int>(fmodf(time * 3.0f, 4.0f));
    const char* dots[] = {"", ".", "..", "..."};
    const char* loading_text = "Loading";
    char full_text[32];
    snprintf(full_text, sizeof(full_text), "%s%s", loading_text, dots[dot_count]);

    ImVec2 loading_size = ImGui::CalcTextSize(full_text);
    ImVec2 loading_pos = ImVec2(center.x - loading_size.x * 0.5f, center.y + 10.0f);
    draw_list->AddText(loading_pos,
                       IM_COL32(139, 233, 253, 255),  // #8be9fd cyan
                       full_text);

    ImGui::End();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);
}

void EngineUI::end_frame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void EngineUI::cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
