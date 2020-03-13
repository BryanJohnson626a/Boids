// All content � 2018 DigiPen (USA) Corporation, all rights reserved.
/*!
@filename EngineSystenGraphics.cpp
@author   Bryan Johnson
*/

#include <iostream>
#include <assert.h>
#include <algorithm>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>

#include "Types.h"
#include "Graphics.h"
#include "Mesh.h"
#include "Model.h"
#include "Color.h"
#include "FBO.h"

std::vector<std::string> errors; // GL error holder for debug.

bool show_vert_normals = false;
bool show_face_normals = false;
const uint INPUT_BUFFER_SIZE = 250;
char file_input_buffer[INPUT_BUFFER_SIZE] = "";

namespace PE
{
    const bool DEBUG_MODE = true;
    const Color FILL_COLOR = BLACK;
    float cam_fov = glm::radians(90.f);
    const float CAM_NEAR = 0.1f;
    const float CAM_FAR = 1000.f;

    Graphics * Graphics::instance;

    // OpenGL error logging.
    int Graphics::LogError(const char * file, int line)
    {
        if (DEBUG_MODE)
        {
            int error = glGetError();
            std::stringstream name;
            while (error)
            {
                name.clear();
                name << "OpenGL error 0x" << std::hex << error << " in file " << file << " on line " << std::dec
                     << line;
                errors.push_back(name.str());
                error = glGetError();
            }
            return error;
        }
    }

    void Graphics::PrintErrors()
    {
        for (auto er : errors)
            std::cout << er << std::endl;
    }

    Graphics::Graphics()
    {
        // SDL: initialize and create a window
        SDL_Init(SDL_INIT_VIDEO);
        title = "ProtoEngine";

        SDL_DisplayMode display_mode;
        SDL_GetCurrentDisplayMode(0, &display_mode);
        window_size_x = display_mode.w;
        window_size_y = display_mode.h;

        window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, window_size_x, window_size_y,
                                  SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
        context = SDL_GL_CreateContext(window);

        // GLEW: get function bindings
        glewInit();

        gBuffer = new PrerenderFBO();

        SetScreenSize(false, 1080, 720);

        std::cout << "OpenGL version is " << glGetString(GL_VERSION) << std::endl;

        // Compile Shaders.
        CompileShader(phong_shading, "phong_shading");
        CompileShader(prerender, "prerender");
        CompileShader(globallight, "globallight");
        CompileShader(gbufferdebug, "gbufferdebug");

        // Initialize settings.
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(2);

        // Ensure no errors.
        LogError(__FILE__, __LINE__);
        PrintErrors();
        assert(errors.size() == 0);

        cam_position = Vec3( 0, 1, 24);
        cam_facing = glm::normalize(Vec3(0, -0.1, -1));
    }

    void Graphics::Initialize()
    {
        // Ensure no errors.
        LogError(__FILE__, __LINE__);
        PrintErrors();
        assert(errors.size() == 0);

        instance = this;

        // Create FSQ mesh.
        std::vector<Vertex> vertices;
        std::vector<uint> indices;
        TextureList textures;

        vertices.push_back(Vertex{Vec3{-1, -1, 0},
                                  Vec3(0, 0, 1),
                                  Vec2(0, 0)});

        vertices.push_back(Vertex{Vec3{1, -1, 0},
                                  Vec3(0, 0, 1),
                                  Vec2(1, 0)});

        vertices.push_back(Vertex{Vec3{1, 1, 0},
                                  Vec3(0, 0, 1),
                                  Vec2(1, 1)});

        vertices.push_back(Vertex{Vec3{-1, 1, 0},
                                  Vec3(0, 0, 1),
                                  Vec2(0, 1)});

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(0);

        FSQ = new Mesh(vertices, indices, textures);

        FSQ->GenerateBuffers();
    }

    // Update the window and check for events.
    void Graphics::Update(float dt)
    {
        wtndc_needs_recalc = true;

        PreDraw();

        // Draw to default FB.
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, window_size_x, window_size_y);

        SetView(cam_position, cam_facing);
        cam_fov = glm::radians(45.f);
        RecalcWTNDC();

        LogError(__FILE__, __LINE__);
        RenderObjects(0);

        PostDraw();

        // Ensure no errors.
        LogError(__FILE__, __LINE__);
        PrintErrors();
        assert(errors.size() == 0);
    }

    void Graphics::Deinit()
    {
        // Ensure no errors.
        LogError(__FILE__, __LINE__);
        assert(errors.size() == 0);
    }

    Graphics::~Graphics()
    {
        delete FSQ;
        delete gBuffer;
        // delete shader programs
        glDeleteProgram(phong_shading.program);
        glDeleteProgram(globallight.program);
        glDeleteProgram(gbufferdebug.program);
        glDeleteProgram(prerender.program);
        SDL_GL_DeleteContext(context);
        SDL_Quit();
    }

    void Graphics::CompileShader(Shader & handle, std::string source_name)
    {
        // Load shader source files
        std::ifstream frag_source("../Resources/Shaders/" + source_name + ".frag");
        assert(frag_source.good());
        std::ifstream vert_source("../Resources/Shaders/" + source_name + ".vert");
        assert(vert_source.good());

        // Shove file data into a string for processing.
        const std::string fragment_shader(
                dynamic_cast<std::stringstream const &>(std::stringstream() << frag_source.rdbuf()).str());
        const char * frag_ptr = fragment_shader.c_str();

        const std::string vertex_shader(
                dynamic_cast<std::stringstream const &>(std::stringstream() << vert_source.rdbuf()).str());
        const char * vert_ptr = vertex_shader.c_str();

        // Compile fragment shader.
        GLint value;
        GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fshader, 1, &frag_ptr, 0);
        glCompileShader(fshader);
        glGetShaderiv(fshader, GL_COMPILE_STATUS, &value);
        if (value)
            std::cout << source_name << ": Fragment shader compiled." << std::endl;
        else
        {
            char log[512];
            glGetShaderInfoLog(fshader, 512, nullptr, log);
            std::cout << source_name << ": ERROR: Fragment shader compile failed." << std::endl;
            std::cout << log << std::endl;
        }

        // Compile vertex shader.
        GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vshader, 1, &vert_ptr, 0);
        glCompileShader(vshader);
        glGetShaderiv(vshader, GL_COMPILE_STATUS, &value);
        if (value)
            std::cout << source_name << ": Vertex shader compiled." << std::endl;
        else
        {
            char log[512];
            glGetShaderInfoLog(vshader, 512, nullptr, log);
            std::cout << source_name << ": ERROR: Vertex shader compile failed." << std::endl;
            std::cout << log << std::endl;
        }

        // Link shader program.
        Shader shader;
        shader.program = glCreateProgram();
        glAttachShader(shader.program, fshader);
        glAttachShader(shader.program, vshader);
        glLinkProgram(shader.program);
        glGetProgramiv(shader.program, GL_LINK_STATUS, &value);
        if (value)
            std::cout << source_name << ": Shader program compiled." << std::endl;
        else
        {
            char log[512];
            glGetProgramInfoLog(shader.program, 512, nullptr, log);
            std::cout << source_name << ": ERROR: Shader program compile failed." << std::endl;
            std::cout << log << std::endl;
        }
        glUseProgram(shader.program);

        // Clean up Shaders.
        glDeleteShader(fshader);
        glDeleteShader(vshader);

        glBindAttribLocation(shader.program, 0, "in_position");
        glBindAttribLocation(shader.program, 1, "in_normal");
        glBindAttribLocation(shader.program, 2, "in_texcoord");

        // Set up uniforms.
        shader.uTransform = glGetUniformLocation(shader.program, "transform");
        shader.uModelTransform = glGetUniformLocation(shader.program, "model_transform");
        shader.uViewPosition = glGetUniformLocation(shader.program, "view_position");
        shader.uObjectColor = glGetUniformLocation(shader.program, "object_color");
        shader.uDiffuse = glGetUniformLocation(shader.program, "diffuse");
        shader.uSpecular = glGetUniformLocation(shader.program, "specular");
        shader.uShininess = glGetUniformLocation(shader.program, "shininess");
        shader.uMode = glGetUniformLocation(shader.program, "mode");

        // Ensure no errors.
        LogError(__FILE__, __LINE__);
        if (errors.size())
        {
            std::cout << "Encountered error when compiling " << source_name << ". Shader compilation aborted."
                      << std::endl;
            PrintErrors();
            errors.clear();
        }
        else
        {
            // Delete old compiled shader.
            glDeleteProgram(handle.program);

            // Replace with new shader.
            handle = shader;
        }

    }

    Graphics * Graphics::GetInstance()
    {
        return instance;
    }

    void Graphics::RecalcWTNDC()
    {
        if (wtndc_needs_recalc)
        {
            Radian y_rot = 0;
            Radian x_rot = 0;

            aspect = float(window_size_x) / float(window_size_y);

            projection = glm::perspective(cam_fov, aspect, CAM_NEAR, CAM_FAR);

            view = glm::lookAt(view_position, view_position + view_facing, Up);

            wtndc_needs_recalc = false;
        }
    }

    // Clear the screen prior to drawing.
    void Graphics::PreDraw()
    {
        glClearColor(FILL_COLOR.R, FILL_COLOR.G, FILL_COLOR.B, FILL_COLOR.A);
    }

    // Draw scene to given framebuffer.
    void Graphics::RenderObjects(GLuint target_framebuffer)
    {
        gBuffer->Bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_BLEND);

        current_shader = &prerender;
        glUseProgram(prerender.program);
        LogError(__FILE__, __LINE__);

        Mat4 projection_view = projection * view;

        // Draw all objects to FBO
        for (auto r : level_objects)
            r->DrawDeferred(current_shader, projection_view, cam_position);
        LogError(__FILE__, __LINE__);

        // Unset FBO render target
        gBuffer->Unbind();
        LogError(__FILE__, __LINE__);

        // Set the viewport, and clear the screen
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        LogError(__FILE__, __LINE__);

        if (debug_mode > 0)
        {
            current_shader = &gbufferdebug;
            glUseProgram(gbufferdebug.program);
            glUniform1i(gbufferdebug.uMode, debug_mode);
        }
        else
        {
            current_shader = &globallight;
            glUseProgram(globallight.program);
        }
        LogError(__FILE__, __LINE__);

        // Set FBO render source
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBuffer->gPosition);
        int loc = glGetUniformLocation(current_shader->program, "gPosition");
        glUniform1i(loc, 0);
        LogError(__FILE__, __LINE__);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBuffer->gNormal);
        loc = glGetUniformLocation(current_shader->program, "gNormal");
        glUniform1i(loc, 1);
        LogError(__FILE__, __LINE__);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gBuffer->gDiffuse);
        loc = glGetUniformLocation(current_shader->program, "gDiffuse");
        glUniform1i(loc, 2);
        LogError(__FILE__, __LINE__);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, gBuffer->gSpecular);
        loc = glGetUniformLocation(current_shader->program, "gSpecular");
        glUniform1i(loc, 3);
        LogError(__FILE__, __LINE__);

        // Load in uniforms.
        glUniform3fv(current_shader->uViewPosition, 1, glm::value_ptr(cam_position));
        LogError(__FILE__, __LINE__);

        // Draw faces.
        FSQ->Draw(current_shader);

        // Copy over the depth buffer.
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->fboID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, gBuffer->width, gBuffer->height, 0, 0, gBuffer->width, gBuffer->height,
                          GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        current_shader = &phong_shading;
        glUseProgram(phong_shading.program);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (auto r : debug_objects)
            r->DrawDebug(current_shader, projection_view, &WHITE);
    }

    // Transfer the finished screen frame to the game window.
    void Graphics::PostDraw()
    {
        SDL_GL_MakeCurrent(window, context);
        SDL_GL_SwapWindow(window);
    }

    void Graphics::SetScreenSize(bool fullscreen, int W, int H)
    {
        if (fullscreen)
        {
            SDL_DisplayMode display_mode;
            SDL_GetCurrentDisplayMode(0, &display_mode);
            SDL_SetWindowFullscreen(window, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
            SetWindowSize(display_mode.w, display_mode.h);
            SDL_SetWindowPosition(window, 0, 0);
        }
        else
        {
            SDL_SetWindowFullscreen(window, SDL_WINDOW_OPENGL);
            SDL_SetWindowBordered(window, SDL_bool(true));
            SetWindowSize(W, H);
            CenterWindow();
        }
    }

    void Graphics::SetWindowSize(int W, int H)
    {
        // Recreate gbuffer with new size.
        gBuffer->Create(W, H);
        window_size_x = W;
        window_size_y = H;
        glViewport(0, 0, W, H);
        SDL_SetWindowSize(window, W, H);
        wtndc_needs_recalc = true;
    }

    void Graphics::CenterWindow()
    {
        SDL_DisplayMode display_mode;
        SDL_GetCurrentDisplayMode(0, &display_mode);

        WindowSetPosition((display_mode.w - window_size_x) / 2, (display_mode.h - window_size_y) / 2);
    }

    void Graphics::WindowSetBordered(bool bordered)
    {
        SDL_SetWindowBordered(window, SDL_bool(bordered));
        SDL_SetWindowResizable(window, SDL_bool(bordered));
    }

    void Graphics::WindowSetPosition(int x, int y)
    {
        SDL_SetWindowPosition(window, x, y);
    }

    void Graphics::SetView(Vec3 position, Vec3 facing)
    {
        view_position = position;
        view_facing = facing;
        wtndc_needs_recalc = true;
    }

    void Graphics::AddModel(Model * model)
    {
        level_objects.emplace_front(model);
        LogError(__FILE__, __LINE__);
    }

    void Graphics::RemoveModel(Model * model)
    {
        level_objects.remove(model);
    }

    void Graphics::AddDebugModel(Model * model)
    {
        debug_objects.emplace_front(model);
        LogError(__FILE__, __LINE__);
    }

    void Graphics::RemoveDebugModel(Model * model)
    {
        debug_objects.remove(model);
    }
}
