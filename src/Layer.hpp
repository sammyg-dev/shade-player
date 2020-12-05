#ifndef _SHADEPLAYER_INCLUDE_LAYER_HPP_
#define _SHADEPLAYER_INCLUDE_LAYER_HPP_
#define DEFAULT_ZINDEX 100

namespace shade {

  /** 
   * 
   *  Handles rendering by z-index of defined objects in layer
   * 
   */
  class Layer {
    public:

      Layer(){

      }

      ~Layer(){

      }

      void Render(){

      }

    protected:
      bool m_isActive = true;
      int m_zIndex = DEFAULT_ZINDEX;

    private:
  };
}

#endif // _SHADEPLAYER_INCLUDE_LAYER_HPP_