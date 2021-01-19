#ifndef _SHADEPLAYER_INCLUDE_SCENE3D_HPP_
#define _SHADEPLAYER_INCLUDE_SCENE3D_HPP_

#include "IScene.hpp"
#include "ILayer.hpp"
#include "ShaderLayer.hpp"
#include "Layer.hpp"
#include "../ViewModels/AudioPlayerLayer.hpp"
#include "../Audio/AudioPlayer.hpp"

using namespace std;

namespace shade {

  /** 
   * 
   *  A Scene3D is a collection of layers to render, including utilities for layer management
   * 
   */
  class Scene3D {
    public:
      Scene3D(){}
      ~Scene3D(){
        m_pLayers.clear();
        UnloadRenderTexture(m_target);
      }
      Scene3D(const Scene3D& other)
        : m_pLayers()  {}; // copy ctor, not sure what to do here yet
      Scene3D(Scene3D && other) = default; // move ctor
      Scene3D& operator=(Scene3D const & other) { return *this; }
      Scene3D& operator=(Scene3D && other) = default;

      void Init(Rectangle rec){
        
        // temp layers
        unique_ptr<ILayer> shaderTest = make_unique<ShaderLayer>(rec);
        shaderTest->Init("resources/shaders/clouds.fs", true);
        InsertLayer(move(shaderTest));
        unique_ptr<ILayer> audioPlayerGUI = make_unique<AudioPlayerLayer>(rec);
        audioPlayerGUI->Init(nullptr, false);
        InsertLayer(move(audioPlayerGUI));


        m_target = LoadRenderTexture(rec.width, rec.height);
        m_camera = { { 2.0f, 3.0f, 2.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, CAMERA_PERSPECTIVE };
        SetCameraMode(m_camera, CAMERA_ORBITAL);
      }

      void Render(float deltaTime){   

        for(const auto &layer : m_pLayers){
          layer->Render(deltaTime, m_target);
        }
        UpdateCamera(&m_camera);  
        BeginTextureMode(m_target);
        ClearBackground(BLANK);
        BeginMode3D(m_camera);

        DrawSphere( {1, 1, 1}, 1, GREEN);
        DrawSphere( {0.5, 0, 1}, 1, RED);
        DrawSphere( {0, 1, 1}, 1, BLUE);
        DrawGrid(100, 1.0f);

        EndMode3D();
        EndTextureMode();


        // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
        DrawTextureRec(m_target.texture, { 0, 0, m_target.texture.width, -m_target.texture.height }, { 0, 0 }, WHITE);

      }

      void InsertLayer(unique_ptr<ILayer> layer){
        m_pLayers.push_back(move(layer));
        // resort here?
      }

      void RemoveLayer(ILayer* layer){
        //todo
      }

      // load layers from file
      bool LoadFromINI(){
        return false;
      };

      bool SaveToINI(){
        return false;
      };


    protected:
      RenderTexture2D m_target;
      Camera m_camera =  { { 2.0f, 3.0f, 2.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };
      bool m_isActive = true;
      int m_zIndex = DEFAULT_ZINDEX;
      vector<unique_ptr<ILayer>> m_pLayers;
    private:
  };
}

#endif // _SHADEPLAYER_INCLUDE_SCENE3D_HPP_