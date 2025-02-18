// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include <iostream>
#include <string>
#include <ctime>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>

#include "ImGuiWrapper.h"

void updateRandomSeeds(double[3], double[3]);

/// This is an example of how you would create windows and widgets with ImGui.
/// Replace it with your own code!
void example_imgui_windows()
{
    static bool show_demo_window    = false;
    static bool show_another_window = false;
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f       = 0.0f;
        static int   counter = 0;

        ImGui::Begin("Information"); // Create a window called "Hello, world!" and append into it.

        //ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
        //ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
        //ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f

        if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window) {
        ImGui::Begin("Another Window", &show_another_window); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
}

int main(int, char**)
{
    ImGuiWrapper::create_window(1280, 720, "Look at my BEAUTIFUL RENDER ENGINE");

    ImGuiWrapper::maximize_window();

    auto camera = Camera{};

    ImGuiWrapper::set_events_callbacks({
        camera.events_callbacks(),
        {
            .on_mouse_pressed = [&](MousePressedEvent const& e) {
                std::cout << "Mouse pressed at " << e.position.x << " " << e.position.y << '\n';
            },
        },
    });

    glEnable(GL_DEPTH_TEST);

    auto const cube_mesh = Mesh{{
    .vertex_buffers = {{
        .layout = {VertexAttribute::Position3D{0}, VertexAttribute::UV{1}, VertexAttribute::Normal3D{2}},
        .data   = {
            // Positions            // UVs         // Normales
            // Face gauche
            -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,
            -1.0f, -1.0f,  1.0f,    1.0f, 0.0f,   -1.0f,  0.0f,  0.0f,
            -1.0f,  1.0f,  1.0f,    1.0f, 1.0f,   -1.0f,  0.0f,  0.0f,
            -1.0f,  1.0f, -1.0f,    0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,

            // Face droite
             1.0f, -1.0f, -1.0f,    0.0f, 0.0f,    1.0f,  0.0f,  0.0f,
             1.0f, -1.0f,  1.0f,    1.0f, 0.0f,    1.0f,  0.0f,  0.0f,
             1.0f,  1.0f,  1.0f,    1.0f, 1.0f,    1.0f,  0.0f,  0.0f,
             1.0f,  1.0f, -1.0f,    0.0f, 1.0f,    1.0f,  0.0f,  0.0f,

            // Face arrière
            -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,    0.0f,  0.0f, -1.0f,
             1.0f, -1.0f, -1.0f,    1.0f, 0.0f,    0.0f,  0.0f, -1.0f,
             1.0f,  1.0f, -1.0f,    1.0f, 1.0f,    0.0f,  0.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,    0.0f, 1.0f,    0.0f,  0.0f, -1.0f,

            // Face avant
            -1.0f, -1.0f,  1.0f,    0.0f, 0.0f,    0.0f,  0.0f,  1.0f,
             1.0f, -1.0f,  1.0f,    1.0f, 0.0f,    0.0f,  0.0f,  1.0f,
             1.0f,  1.0f,  1.0f,    1.0f, 1.0f,    0.0f,  0.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,    0.0f, 1.0f,    0.0f,  0.0f,  1.0f,

            // Face du bas
            -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,    0.0f, -1.0f,  0.0f,
             1.0f, -1.0f, -1.0f,    1.0f, 0.0f,    0.0f, -1.0f,  0.0f,
             1.0f, -1.0f,  1.0f,    1.0f, 1.0f,    0.0f, -1.0f,  0.0f,
            -1.0f, -1.0f,  1.0f,    0.0f, 1.0f,    0.0f, -1.0f,  0.0f,

            // Face du haut
            -1.0f,  1.0f, -1.0f,    0.0f, 0.0f,    0.0f,  1.0f,  0.0f,
             1.0f,  1.0f, -1.0f,    1.0f, 0.0f,    0.0f,  1.0f,  0.0f,
             1.0f,  1.0f,  1.0f,    1.0f, 1.0f,    0.0f,  1.0f,  0.0f,
            -1.0f,  1.0f,  1.0f,    0.0f, 1.0f,    0.0f,  1.0f,  0.0f
        },
    }},
    .index_buffer   = {
        // Indices pour chaque face
        0, 1, 2,  0, 2, 3,    // Face gauche
        4, 5, 6,  4, 6, 7,    // Face droite
        8, 9, 10, 8, 10,11,   // Face arrière
        12,13,14, 12,14,15,   // Face avant
        16,17,18, 16,18,19,   // Face du bas
        20,21,22, 20,22,23    // Face du haut
    }
}};

    auto const texture = Texture{
        TextureSource::File{
            .path           = "res/texture.png",
            .flip_y         = true,
            .texture_format = InternalFormat::RGBA8,
        },
        TextureOptions{
            .minification_filter  = Filter::Linear, // Comment on va moyenner les pixels quand on voit l'image de loin ?
            .magnification_filter = Filter::Linear, // Comment on va interpoler entre les pixels quand on zoom dans l'image ?
            .wrap_x               = Wrap::Repeat,   // Quelle couleur va-t-on lire si jamais on essaye de lire en dehors de la texture ?
            .wrap_y               = Wrap::Repeat,   // Idem, mais sur l'axe Y. En général on met le même wrap mode sur les deux axes.
        }
    };

    auto const sky = Texture{
        TextureSource::File{
            .path           = "res/sky.hdr",
            .flip_y         = true,
            .texture_format = InternalFormat::RGBA32F,
        },
        TextureOptions{
            .minification_filter  = Filter::Linear, // Comment on va moyenner les pixels quand on voit l'image de loin ?
            .magnification_filter = Filter::Linear, // Comment on va interpoler entre les pixels quand on zoom dans l'image ?
            .wrap_x               = Wrap::Repeat,   // Quelle couleur va-t-on lire si jamais on essaye de lire en dehors de la texture ?
            .wrap_y               = Wrap::Repeat,   // Idem, mais sur l'axe Y. En général on met le même wrap mode sur les deux axes.
        }
    };

    auto const shader = Shader{{
        .vertex   = ShaderSource::File{"res/vertex.glsl"},
        .fragment = ShaderSource::File{"res/fragment.glsl"},
        //.vertex = ShaderSource::File{"res/default.vert"},
        //.fragment = ShaderSource::File{"res/default.frag"},
    }};

    /*auto const shader2 = Shader{{
        .vertex = ShaderSource::File{"res/default.vert"},
        .fragment = ShaderSource::File{"res/display.frag"},
    }};

    auto const shader3 = Shader{{
        .vertex = ShaderSource::File{"res/default.vert"},
        .fragment = ShaderSource::File{"res/postProcess.frag"},
    }};

    auto const shader4 = Shader{{
        .vertex = ShaderSource::File{"res/default.vert"},
        .fragment = ShaderSource::File{"res/bloom.frag"},
    }};*/

    int frameStill = 0;

    // random seeds
    srand(static_cast<unsigned int>(time(nullptr))); // init random with time
    double rSeed1[2], rSeed2[2];

    // Main loop
    while (ImGuiWrapper::window_is_open()) {
        ++frameStill;
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        //glfwPollEvents();

        // Title
        ImGuiWrapper::getTitle();

        updateRandomSeeds(rSeed1, rSeed2);

        // ImGui Wrapper
        glClearColor(1.0f, .0f, .0f, .0f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGuiWrapper::begin_frame();
        example_imgui_windows();
        ImGuiWrapper::end_frame(/*ImVec4(1.0f, .0f, .0f, 1.00f)*/);

        glm::mat4 const view_matrix = camera.view_matrix();
        glm::mat4 const projection_matrix = glm::infinitePerspective(glm::radians(45.f) /*field of view in radians*/, ImGuiWrapper::framebuffer_aspect_ratio() /*aspect ratio*/, 0.001f /*near plane*/);

        glm::mat4 const rotation = glm::rotate(glm::mat4{1.f}, ImGuiWrapper::time_in_seconds() /*angle de la rotation*/, glm::vec3{0.f, 0.f, 1.f} /* axe autour duquel on tourne */);
        glm::mat4 const translation = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, 0.f, 0.f} /* déplacement */);

        glm::mat4 const model_matrix = rotation * translation;

        //glClearColor(0.f, 0.f, 1.f, 1.f); // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
        //glClear(GL_COLOR_BUFFER_BIT); // Exécute concrètement l'action d'appliquer sur tout l'écran la couleur choisie au-dessus
        //glEnable(GL_BLEND);
        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE); // On peut configurer l'équation qui mélange deux couleurs, comme pour faire différents blend mode dans Photoshop. Cette équation-ci donne le blending "normal" entre pixels transparents.
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Vient remplacer glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();
        //shader.set_uniform("in_color", glm::vec4{1.f, 1.f, 0.f, 1.f});
        //shader.set_uniform("aspect_ratio", gl::framebuffer_aspect_ratio());
        //shader.set_uniform("displacement", glm::vec2{cos(gl::time_in_seconds()), sin(gl::time_in_seconds())});
        shader.set_uniform("model_view_projection",  projection_matrix * view_matrix * model_matrix );
        shader.set_uniform("model_matrix", model_matrix);
        shader.set_uniform("normal_matrix",  glm::inverse(glm::transpose(model_matrix)));
        shader.set_uniform("my_texture", texture);
        shader.set_uniform("skybox", texture);
        shader.set_uniform("light_direction", glm::normalize(glm::vec3(1, 0.0, 0.0)));
        //shader.set_uniform("resolution", glm::vec2{ImGuiWrapper::framebuffer_width_in_pixels(), ImGuiWrapper::framebuffer_height_in_pixels()});
        //shader.set_uniform("framesStill", frameStill);
        //shader.set_uniform("rSeed1", glm::vec2{rSeed1[0], rSeed1[1]});
        //shader.set_uniform("rSeed2", glm::vec2{rSeed2[0], rSeed2[1]});
        //shader.set_uniform("treshHoldIntensity", );

        cube_mesh.draw();

    }

    ImGuiWrapper::shutdown();
}

void updateRandomSeeds(double rSeed1[3], double rSeed2[3]) {
    rSeed1[0] = rand() % 10000 / 100.0;
    rSeed1[1] = rand() % 10000 / 100.0;

    rSeed2[0] = rand() % 10000 / 100.0;
    rSeed2[1] = rand() % 10000 / 100.0;
}
