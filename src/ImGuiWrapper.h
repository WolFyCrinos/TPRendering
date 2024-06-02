#pragma once

#include <imgui/imgui.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vector>
#include <string_view>
#include "Camera.hpp"
#include "EventsCallbacks.hpp"
#include "Mesh.hpp"
#include "RenderTarget.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "make_absolute_path.hpp"
#include <glad/glad.h>
#include "glm/glm.hpp"
#include "tiny_obj_loader.h"

namespace ImGuiWrapper {

void        create_window(int width, int height, const char* title);
void        begin_frame();
void        end_frame(/*ImVec4 background_color = {0.45f, 0.55f, 0.60f, 1.00f}*/);
void        shutdown();
void        maximize_window();
void        set_events_callbacks(std::vector<EventsCallbacks>);
bool        window_is_open();
void        getTitle();
auto        framebuffer_width_in_pixels() -> int;
auto        framebuffer_height_in_pixels() -> int;
auto        framebuffer_aspect_ratio() -> float;
auto        window_width_in_screen_coordinates() -> int;
auto        window_height_in_screen_coordinates() -> int;
auto        window_aspect_ratio() -> float;

auto        time_in_seconds() -> float;
auto        delta_time_in_seconds() -> float;

void        bind_default_shader();
auto        sphere_vertices();

} // namespace ImGuiWrapper