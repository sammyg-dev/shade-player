#ifndef _SHADEPLAYER_INCLUDE_SHADEPLAYERAPP_HPP_
#define _SHADEPLAYER_INCLUDE_SHADEPLAYERAPP_HPP_

#include <raylib.h>
#include "LayerStack.hpp"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB -> Not supported at this moment
    #define GLSL_VERSION            100
#endif

namespace shade {
  /** 
   * 
   *  Shade Player App creates and manages a window, providing customization options 
   *  for an audio-reactive experience
   *  note - perhaps use a factory pattern later to also support other usecases (ie ShadetPlayerEmbedded)
   */
  struct WindowConfig {
    int ScreenWidth;
    int ScreenHeight;
    int TargetFPS;
  };

  class ShadePlayerApp {
    public:

      ShadePlayerApp(int screenWidth, int screenHeight, int fps){
        m_windowConfig = {
          screenWidth,
          screenHeight,
          fps
        };
      }

      ~ShadePlayerApp(){

      }

      // loads resources and initializes objects needed at start
      void Init(){
 
        InitWindow(m_windowConfig.ScreenWidth, m_windowConfig.ScreenHeight, "Shade Player");
        SetTargetFPS(m_windowConfig.TargetFPS);

        // temp
        m_shader = LoadShader("", "resources/shaders/driveHome.fs");
        m_iTime = 0.0f;
        m_iTimeLoc = GetShaderLocation(m_shader, "iTime");
        m_iResolutionLoc = GetShaderLocation(m_shader, "iResolution");
        m_iResolution[0] = (float)m_windowConfig.ScreenWidth;
        m_iResolution[1] = (float)m_windowConfig.ScreenHeight;
        SetShaderValue(m_shader, m_iResolutionLoc, m_iResolution, UNIFORM_VEC2);


        m_layerStack = LayerStack();
      };

      // Main app loop
      void Start(){
        while (!WindowShouldClose())
        {
            // temp update shader params
            m_iTime += GetFrameTime();
            SetShaderValue(m_shader, m_iTimeLoc, &m_iTime, UNIFORM_FLOAT);

            // do pause ish later
            if (IsKeyPressed(KEY_SPACE)) m_pause = !m_pause;
            //-----------------------------------------------------

            ///////////////////////////////
            // main draw loop
            BeginDrawing();
            ClearBackground(BLANK);

            // todo: layerstack rendering
            

            // temp shader
            BeginShaderMode(m_shader);
            DrawRectangle(0, 0, m_windowConfig.ScreenWidth, m_windowConfig.ScreenHeight, WHITE);
            EndShaderMode();

            DrawFPS(10, 10);
            EndDrawing();
            ///////////////////////////////
        }
      };

    protected:
      bool m_isActive = true;
      int m_zIndex = DEFAULT_ZINDEX;

    private:
      LayerStack m_layerStack;
      WindowConfig m_windowConfig;

      // temp props
      Shader m_shader;
      float m_iTime;
      int m_iTimeLoc;
      int m_iResolutionLoc;
      float m_iResolution[2] = {};
      bool m_pause = false;
  };
}

#endif // _SHADEPLAYER_INCLUDE_SHADEPLAYERAPP_HPP_