#ifndef _SHADEPLAYER_INCLUDE_ISCENE_HPP_
#define _SHADEPLAYER_INCLUDE_ISCENE_HPP_

#include <vector>
#include <memory>
#include "ILayer.hpp"
#include "ShaderLayer.hpp"
#include "Layer.hpp"

using namespace std;

namespace shade {

  /** 
   * 
   *  A IScene is a collection of layers to render, including utilities for layer management
   * 
   */
  class IScene {
    public:
      virtual ~IScene(){ }

      virtual void Init(Rectangle rec) = 0;

      virtual void Render(float deltaTime) = 0;

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
      vector<unique_ptr<ILayer>> m_pLayers;
    private:
  };
}

#endif // _SHADEPLAYER_INCLUDE_ISCENE_HPP_