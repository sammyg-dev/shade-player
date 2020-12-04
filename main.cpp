/*******************************************************************************************
*
*   sammy g sandbox
*
*   testing raylib and raylib-cpp
*
*
********************************************************************************************/
#include "include/raylib-cpp.hpp"
#include <cmath>
using namespace std;

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB -> Not supported at this moment
    #define GLSL_VERSION            100
#endif

int main(void)
{
    // Initialization
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    raylib::Window w(screenWidth, screenHeight, "raylib-cpp sandbox");
    bool pause = false;             // Movement pause
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //----------------------------------------------------------
    //raylib::Shader shader = LoadShader(0, TextFormat("resources/driveHome.fs", GLSL_VERSION));
    raylib::Shader shader = raylib::Shader::Load("", "resources/driveHome.fs");
    float iTime = 0.0f;
    int iTimeLoc = shader.GetLocation("iTime");
    int iResolutionLoc = shader.GetLocation("iResolution");
    const float iResolution[2] = { (float)screenWidth, (float)screenHeight };
    shader.SetValue(iResolutionLoc, iResolution, UNIFORM_VEC2);
    //SetShaderValue(shader, iResolutionLoc, iResolution, UNIFORM_VEC2);
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //-----------------------------------------------------
        iTime += GetFrameTime();
        SetShaderValue(shader, iTimeLoc, &iTime, UNIFORM_FLOAT);

        // Pause Box A movement
        if (IsKeyPressed(KEY_SPACE)) pause = !pause;
        //-----------------------------------------------------

        // Draw
        //-----------------------------------------------------
        BeginDrawing();
        ClearBackground(BLANK);

        shader.BeginShaderMode();
        DrawRectangle(0, 0, screenWidth, screenHeight, WHITE);
        shader.EndShaderMode();

        DrawFPS(10, 10);
        EndDrawing();
        //-----------------------------------------------------
    }

    shader.Unload();
    CloseWindow();
    return 0;
}
