#include "ImGuiWrapper.h"
#include "framebuffer.h"
#include <glad/glad.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <iostream>

namespace ImGuiWrapper {

struct Context { // NOLINT(*special-member-functions)
    GLFWwindow*                      window{nullptr};
    std::vector<EventsCallbacks> events_callbacks{};
    float                            last_time{0.f};
    float                            delta_time{0.f};
    bool                             is_first_frame{true};

    ~Context()
    {
        glfwDestroyWindow(window);
    }
};

auto context() -> Context&
{
    static auto instance = Context{};
    return instance;
}

void assert_init_has_been_called()
{
    assert(context().window != nullptr && "You must call create_window() as the first line of your program.");
}

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "[Glfw Error] " << error << ": " << description << "\n";
}

void mouse_move_callback(GLFWwindow*, double x_pos, double y_pos)
{
    for (auto const& callbacks : context().events_callbacks)
        callbacks.on_mouse_moved({.position = glm::vec2{static_cast<float>(x_pos), static_cast<float>(y_pos)}});
}
void mouse_button_callback(GLFWwindow*, int button, int action, int mods)
{
    double x, y;
    glfwGetCursorPos(context().window, &x, &y);
    if (action == GLFW_PRESS)
    {
        for (auto const& callbacks : context().events_callbacks)
            callbacks.on_mouse_pressed({.position = glm::vec2{static_cast<float>(x), static_cast<float>(y)}, .button = button, .mods = mods});
    }
    else
    {
        assert(action == GLFW_RELEASE);
        for (auto const& callbacks : context().events_callbacks)
            callbacks.on_mouse_released({.position = glm::vec2{static_cast<float>(x), static_cast<float>(y)}, .button = button, .mods = mods});
    }
}
void scroll_callback(GLFWwindow*, double x_offset, double y_offset)
{
    for (auto const& callbacks : context().events_callbacks)
        callbacks.on_scroll({.scroll = static_cast<float>(y_offset), .horizontal_scroll = static_cast<float>(x_offset)});
}
void framebuffer_resized_callback(GLFWwindow*, int width_in_pixels, int height_in_pixels)
{
    glViewport(0, 0, width_in_pixels, height_in_pixels);
    for (auto const& callbacks : context().events_callbacks)
        callbacks.on_framebuffer_resized({.width_in_pixels = width_in_pixels, .height_in_pixels = height_in_pixels});
}

void window_resized_callback(GLFWwindow*, int width_in_screen_coordinates, int height_in_screen_coordinates)
{
    for (auto const& callbacks : context().events_callbacks)
        callbacks.on_window_resized({.width_in_screen_coordinates = width_in_screen_coordinates, .height_in_screen_coordinates = height_in_screen_coordinates});
}

void set_events_callbacks(std::vector<EventsCallbacks> callbacks)
{
    context().events_callbacks = std::move(callbacks);
}

void getTitle()
{
    // Title
    const char* glVersion = (const char*)glGetString(GL_VERSION);
    std::string tempTitle = "MY SUPER RENDERING TP " + std::to_string(1000.0f / ImGui::GetIO().Framerate)
                            + " ms/frame ("
                            + std::to_string((int32_t)(ImGui::GetIO().Framerate))
                            + " FPS) => GLFW v "
                            + glfwGetVersionString()
                            + " "
                            + glVersion;
    const char* newTitle = tempTitle.c_str();

    glfwSetWindowTitle(context().window, newTitle);
}

void create_window(int width, int height, const char* title)
{ // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize Glfw\n";
        std::terminate();
    }

#if defined(__APPLE__)
    const char* glsl_version = "#version 430";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    const char* glsl_version = "#version 430";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window with graphics context
    context().window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!context().window) {
        std::cerr << "Failed to create a window\n";
        std::terminate();
    }
    glfwMakeContextCurrent(context().window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) { // NOLINT
        std::cerr << "Failed to initialize glad\n";
        std::terminate();
    }
    gladLoadGL();
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
    io.ConfigViewportsNoAutoMerge = true;
    io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(context().window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    glfwSetCursorPosCallback(context().window, &mouse_move_callback);
    glfwSetMouseButtonCallback(context().window, &mouse_button_callback);
    glfwSetScrollCallback(context().window, &scroll_callback);
    glfwSetWindowSizeCallback(context().window, &window_resized_callback);
    glfwSetFramebufferSizeCallback(context().window, &framebuffer_resized_callback);
}

void maximize_window()
{
    assert_init_has_been_called();
    glfwMaximizeWindow(context().window);
}

auto window_is_open() -> bool
{
    assert_init_has_been_called();

    float const time = time_in_seconds();
    if (!context().is_first_frame)
        context().delta_time = time - context().last_time;
    context().last_time = time;

    glfwSwapBuffers(context().window);
    glfwPollEvents();
    context().is_first_frame = false;
    return !glfwWindowShouldClose(context().window);
}

auto framebuffer_width_in_pixels() -> int
{
    int w; // NOLINT(*init-variables)
    glfwGetFramebufferSize(context().window, &w, nullptr);
    return w;
}

auto framebuffer_height_in_pixels() -> int
{
    int h; // NOLINT(*init-variables)
    glfwGetFramebufferSize(context().window, nullptr, &h);
    return h;
}

auto framebuffer_aspect_ratio() -> float
{
    int w, h; // NOLINT(*init-variables)
    glfwGetFramebufferSize(context().window, &w, &h);
    return static_cast<float>(w) / static_cast<float>(h);
}

auto window_width_in_screen_coordinates() -> int
{
    int w; // NOLINT(*init-variables)
    glfwGetWindowSize(context().window, &w, nullptr);
    return w;
}

auto window_height_in_screen_coordinates() -> int
{
    int h; // NOLINT(*init-variables)
    glfwGetWindowSize(context().window, nullptr, &h);
    return h;
}

auto window_aspect_ratio() -> float
{
    int w, h; // NOLINT(*init-variables)
    glfwGetWindowSize(context().window, &w, &h);
    return static_cast<float>(w) / static_cast<float>(h);
}

auto time_in_seconds() -> float
{
    return static_cast<float>(glfwGetTime());
}

auto delta_time_in_seconds() -> float
{
    return context().delta_time;
}

static auto default_shader() -> Shader&
{
    static auto instance = Shader{{
        .vertex   = ShaderSource::Code{R"GLSL(
#version 410
layout(location = 0) in vec3 in_position;

void main()
{
    gl_Position = vec4(in_position, 1.);
}
)GLSL"},
        .fragment = ShaderSource::Code{R"GLSL(
#version 410
out vec4 out_color;

void main()
{
    out_color = vec4(1.);
}
)GLSL"},
    }};
    return instance;
}

void bind_default_shader()
{
    default_shader().bind();
}

void begin_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    /*if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        static constexpr ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        static constexpr ImGuiWindowFlags   window_flags    = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

        ImGui::Begin("MyMainDockSpace", nullptr, window_flags);
        ImGui::PopStyleVar(3);
        ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.f, 0.f), dockspace_flags);
        ImGui::End();
    }*/
}

void end_frame(/*ImVec4 background_color*/)
{
    // Rendering
    ImGui::Render();
    //int display_w, display_h;
    //glfwGetFramebufferSize(context().window, &display_w, &display_h);
    //glViewport(0, 0, display_w, display_h);
    //glClearColor(background_color.x, background_color.y, background_color.z, background_color.w);
    //glClear(GL_COLOR_BUFFER_BIT);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glfwSwapBuffers(context().window);
}

void shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(context().window);
    glfwTerminate();
}

} // namespace ImGuiWrapper