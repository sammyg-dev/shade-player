#ifndef _SHADEPLAYER_INCLUDE_SHADEPLAYERAPP_HPP_
#define _SHADEPLAYER_INCLUDE_SHADEPLAYERAPP_HPP_

// #include <memory>
#include <raylib.h>
#include "Scene.hpp"
#include "ShaderLayer.hpp"
#include "Layer.hpp"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_RPI, PLATFORM_ANDROID, PLATFORM_WEB -> Not supported at this moment
    #define GLSL_VERSION            100
#endif

/** 
 *  Shade Player App creates and manages a window, providing customization options 
 *  for an audio-reactive experience
 *  note - perhaps use a factory pattern later to also support other usecases (ie ShadetPlayerEmbedded)
 */

namespace shade {

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
        LoadScene();




        //m_layerStack = LayerStack();
      };

      void LoadScene(){
        m_scene = Scene();
        Rectangle rec = {0, 0, (float)m_windowConfig.ScreenWidth, (float)m_windowConfig.ScreenHeight};
        m_scene.Init(rec);
      }

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
            m_scene.Render(deltaTime);

            DrawFPS(10, 10);
            EndDrawing();
            ///////////////////////////////
        }
      };
    protected:
      bool m_isActive = true;
      Scene m_scene;
    private:
      //LayerStack m_layerStack;
      WindowConfig m_windowConfig;
      bool m_pause = false;
  };
}

#endif // _SHADEPLAYER_INCLUDE_SHADEPLAYERAPP_HPP_