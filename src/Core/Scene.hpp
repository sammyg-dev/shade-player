#ifndef _SHADEPLAYER_INCLUDE_SCENE_HPP_
#define _SHADEPLAYER_INCLUDE_SCENE_HPP_

#include <vector>
#include <memory>
#include "ILayer.hpp"
#include "ShaderLayer.hpp"
#include "Layer.hpp"
#include "Layer3D.hpp"
#include "../ViewModels/AudioPlayerLayer.hpp"

using namespace std;

namespace shade {

  /** 
   * 
   *  A scene is a collection of layers to render, including utilities for layer management
   * 
   */
  class Scene {
    public:
      Scene(){}
      ~Scene(){
        m_pLayers.clear();
      }
      Scene(const Scene& other)
        : m_pLayers()  {}; // copy ctor, not sure what to do here yet
      Scene(Scene && other) = default; // move ctor
      Scene& operator=(Scene const & other) { return *this; }
      Scene& operator=(Scene && other) = default;

      void Init(Rectangle rec){
        // load ish from file later?
  
        /* we need some kind of editor to manage
         *
         * Add Layer -> Select Layer Type
         * Input Layer config options (just dimensions atm which can default to screen)
         * 
         * Add DisplayObject to Layer -> Select DO Type
         * Input DO config options (depends on type selected)
         * 
         * Layer and DO render sorting (dragndrop or just input index)
         * 
         * REMOVE Layers, REMOVE DOs
         * 
         * HIDE/SHOW toggle for Layers
         * 
         * SAVE to INI
         */


        /////////////////////////
        // temp layers
        /////////////////////////
        unique_ptr<ILayer> shaderTest2 = make_unique<ShaderLayer>(rec);
        shaderTest2->Init("resources/shaders/linewaves.fs", true);
        InsertLayer(move(shaderTest2));        
      
        // unique_ptr<ILayer> test3d = make_unique<Layer3D>(rec);
        // test3d->Init(nullptr, false);
        // InsertLayer(move(test3d));

        unique_ptr<ILayer> audioPlayerGUI = make_unique<AudioPlayerLayer>(rec);
        audioPlayerGUI->Init(nullptr, false);
        InsertLayer(move(audioPlayerGUI));
        /////////////////////////

        // todo: support multiple render textures?
        m_target = LoadRenderTexture(rec.width, rec.height);
      }

      void Render(float deltaTime){        
        for(const auto &layer : m_pLayers){
          layer->Render(deltaTime, m_target);
        }
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
      bool m_isActive = true;
      int m_zIndex = DEFAULT_ZINDEX;

    private:
      vector<unique_ptr<ILayer>> m_pLayers;
  };
}

#endif // _SHADEPLAYER_INCLUDE_SCENE_HPP_