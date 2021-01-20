#ifndef _SHADEPLAYER_INCLUDE_SHADEPLAYERAPP_HPP_
#define _SHADEPLAYER_INCLUDE_SHADEPLAYERAPP_HPP_

// #include <memory>
#include <raylib.h>
#include "Core/Scene.hpp"
//#include "Core/Scene3D.hpp"
#include "Core/ShaderLayer.hpp"
#include "Core/Layer.hpp"
#include "Audio/AudioPlayer.hpp"

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
      ShadePlayerApp(int screenWidth = 2560, int screenHeight = 1440, int fps = 60){
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
 
        SetConfigFlags(FLAG_MSAA_4X_HINT);      // Enable Multi Sampling Anti Aliasing 4x (if available)

        InitWindow(m_windowConfig.ScreenWidth, m_windowConfig.ScreenHeight, "Shade Player");
        InitAudioDevice();        
        SetTargetFPS(m_windowConfig.TargetFPS);
        //ToggleFullscreen();

        m_pAudioPlayer = make_unique<AudioPlayer>();

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
        int count = 0;
        char** droppedFiles = { 0 };
        while (!WindowShouldClose())
        {
            float deltaTime = GetFrameTime();
            
            // check for file drop
            if(IsFileDropped()){
              droppedFiles = GetDroppedFiles(&count);
              m_pAudioPlayer->AddSongs(droppedFiles, count);
              ClearDroppedFiles();
            }

            // do pause ish later
            if (IsKeyPressed(KEY_SPACE)){
              auto s = m_pAudioPlayer->GetState();
              if(s == PlaybackState::PLAYING){
                m_pAudioPlayer->Stop();
              } else {
                m_pAudioPlayer->Play();
              }
            }
            //-----------------------------------------------------

            ///////////////////////////////
            // main draw loop
            BeginDrawing();
            ClearBackground(RAYWHITE);


            // todo: layerstack rendering
            m_scene.Render(deltaTime);

            // update loop for audio player (detect sound finish among other ish)
            m_pAudioPlayer->Update(deltaTime);

            DrawFPS(10, 10);            
            EndDrawing();
            ///////////////////////////////
        }
        ClearDroppedFiles();
        CloseAudioDevice();
        CloseWindow();
      };
    protected:
      bool m_isActive = true;
      Scene m_scene;
      unique_ptr<AudioPlayer> m_pAudioPlayer = nullptr;
    private:
      //LayerStack m_layerStack;
      WindowConfig m_windowConfig;
      bool m_pause = false;
  };
}

#endif // _SHADEPLAYER_INCLUDE_SHADEPLAYERAPP_HPP_