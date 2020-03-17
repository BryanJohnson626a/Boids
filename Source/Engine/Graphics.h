/*!
@filename EngineSystemGraphics.h
@author   Bryan Johnson
*/

#pragma once


#include <SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <forward_list>
#include <vector>

#include "Types.h"
#include "Color.h"

namespace PE
{
    class PrerenderFBO;

    // Holds the data for a compiled shader.
    struct Shader
    {
        // Compiled shader program.
        GLuint program = 0;

        // Uniforms.
        GLuint uTransform = 0;
        GLuint uModelTransform = 0;
        GLuint uViewPosition = 0;
        GLuint uObjectColor = 0;
        GLuint uDiffuse = 0;
        GLuint uSpecular = 0;
        GLuint uShininess = 0;
        GLuint uMode = 0;
    };

    class Graphics
    {
    public:

        // The container type for the render object list.
        typedef std::forward_list<Model*> ROList;

        Graphics();
        void Initialize();
        static void Deinit();
        ~Graphics();

        void Update(float dt);

        static int LogError(const char* file, int line);
        static void PrintErrors();

        // Gets the current graphics engine system.
        static Graphics* GetInstance();

        void AddModel(Model * model);
        void RemoveModel(Model * model);
        void AddDebugModel(Model * model);
        void RemoveDebugModel(Model * model);

        // Sets the screen size (or fullscreen) of the game.
        void SetScreenSize(bool fullscreen, int W = 0, int H = 0);

        void SetWindowSize(int W, int H);
        void CenterWindow();

        void WindowSetBordered(bool bordered);
        void WindowSetPosition(int x, int y);

        // Camera
        Vec3 view_position{};
        Vec3 view_facing{};
        Vec3 cam_position{};
        Vec3 cam_facing{};

        void SetView(Vec3 position, Vec3 facing);
        bool wtndc_needs_recalc{ true };

        Mat4 projection{};
        Mat4 view{};

        Shader * current_shader{};
        Shader globallight;
        Shader gbufferdebug;
        Shader phong_shading;
        Shader prerender;

        // Mode for displaying debug info. 0 = no debug info.
        int debug_mode = 0;
        void CompileShaders();
    private:


        static Graphics* instance;

        static void PreDraw();
        void RenderObjects(GLuint target_framebuffer);
        void PostDraw();

        static void CompileShader(Shader & handle, const std::string& source_name);

        void RecalcWTNDC();

        SDL_Window* window;
        std::string title;
        int window_size_x;
        int window_size_y;
        float aspect{};
        SDL_GLContext context;

        GLuint FBOHandle{};

        // List of objects that need to be rendered.
        ROList level_objects;
        ROList debug_objects;

        PrerenderFBO * gBuffer;

        Mesh * FSQ{};
    };
}
