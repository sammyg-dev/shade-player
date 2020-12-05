#ifndef _SHADEPLAYER_INCLUDE_SHADEPLAYERAPP_HPP_
#define _SHADEPLAYER_INCLUDE_SHADEPLAYERAPP_HPP_
#include <raylib.h>
//#include "LayerStack.hpp"
#include "ShaderLayer.hpp"

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
      ShadePlayerApp(int screenWidth = 1920, int screenHeight = 1080, int fps = 60){
        m_windowConfig = {
          screenWidth,
          screenHeight,
          fps
        };
      }
      ~ShadePlayerApp(){}
      // loads resources and initializes objects needed at start
      void Init(){
 
        InitWindow(m_windowConfig.ScreenWidth, m_windowConfig.ScreenHeight, "Shade Player");
        SetTargetFPS(m_windowConfig.TargetFPS);

        // temp
        Rectangle rec = {0, 0, (float)m_windowConfig.ScreenWidth, (float)m_windowConfig.ScreenHeight};
        m_layer = ShaderLayer(rec);
        m_layer.Init("resources/shaders/driveHome.fs");


        //m_layerStack = LayerStack();
      };
      // Main app loop
      void Start(){
        while (!WindowShouldClose())
        {
            float deltaTime = GetFrameTime();
            
            // do pause ish later
            if (IsKeyPressed(KEY_SPACE)) m_pause = !m_pause;
            //-----------------------------------------------------

            ///////////////////////////////
            // main draw loop
            BeginDrawing();
            ClearBackground(BLANK);

            // todo: layerstack rendering
            m_layer.Render(deltaTime);

            DrawFPS(10, 10);
            EndDrawing();
            ///////////////////////////////
        }
      };
    protected:
      bool m_isActive = true;
      int m_zIndex = DEFAULT_ZINDEX;
    private:
      //LayerStack m_layerStack;
      ShaderLayer m_layer; // temp testing
      WindowConfig m_windowConfig;
      bool m_pause = false;
  };
}

#endif // _SHADEPLAYER_INCLUDE_SHADEPLAYERAPP_HPP_