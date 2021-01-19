#ifndef _SHADEPLAYER_INCLUDE_LAYER_HPP_
#define _SHADEPLAYER_INCLUDE_LAYER_HPP_
#define DEFAULT_ZINDEX 100

#include <vector>
#include <algorithm>
#include "ILayer.hpp"
#include "IDisplayObject.hpp"

namespace shade {

  /** 
   * 
   *  Layer interface so different Layer types can be used within the same container and have
   *  a shared interface (LayerStack needs this)
   * 
   */
  class Layer: public ILayer {
    public:
      Layer(){

      }

      Layer(Rectangle rec){
        m_dimensions = rec;
      }

      ~Layer(){};

      void Init(const char* filePath, bool isReactive) override {

      };

      void Render(float deltaTime, RenderTexture2D target) override {
        // for(size_t i = 0; i <= m_pDisplayObjects.size(); ++i){
        //   m_pDisplayObjects[i]->Render(deltaTime);
        // }
      };

      // add and resort ??
      void AddObj(){
        //std::sort(m_pDisplayObjects.begin(), m_pDisplayObjects.end(), ZIndexComparator())
      }

    protected:
      //std::vector<IDisplayObject*> m_pDisplayObjects;

      virtual Layer* clone_impl() const override { return new Layer(*this); }
  };
}
#endif // _SHADEPLAYER_INCLUDE_LAYER_HPP_