#ifndef _SHADEPLAYER_INCLUDE_SCENE_HPP_
#define _SHADEPLAYER_INCLUDE_SCENE_HPP_

#include <vector>
#include <memory>
#include "ILayer.hpp"
#include "ShaderLayer.hpp"
#include "Layer.hpp"

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
        // for(size_t i = 0; i <= m_pLayers.size(); ++i){
        //   m_pLayers[i]->Init();
        // }
        // load ish from file later
        
        unique_ptr<ILayer> layer = make_unique<ShaderLayer>(rec);
        layer->Init("resources/shaders/driveHome.fs");
        InsertLayer(move(layer));
        unique_ptr<ILayer> layer2 = make_unique<Layer>(rec);
        InsertLayer(move(layer2));
      }

      void Render(float deltaTime){        
        for(const auto &layer : m_pLayers){
          layer->Render(deltaTime);
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
      bool m_isActive = true;
      int m_zIndex = DEFAULT_ZINDEX;


    private:

      vector<unique_ptr<ILayer>> m_pLayers;
  };
}

#endif // _SHADEPLAYER_INCLUDE_SCENE_HPP_