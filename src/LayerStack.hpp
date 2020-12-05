#ifndef _SHADEPLAYER_INCLUDE_LAYERSTACK_HPP_
#define _SHADEPLAYER_INCLUDE_LAYERSTACK_HPP_

#include "Layer.hpp"

namespace shade {

  /** 
   * 
   *  Handles all layer management and rendering, sorted them by intended draw order
   * 
   */
  class LayerStack {
    public:

      LayerStack(){

      }

      ~LayerStack(){

      }

      // load layerstack from file
      bool LoadFromINI(){
        return false;
      };

      bool SaveToINI(){
        return false;
      };

      void Render(){

      };

    protected:
      bool m_isActive = true;
      int m_zIndex = DEFAULT_ZINDEX;

    private:
  };
}

#endif // _SHADEPLAYER_INCLUDE_LAYERSTACK_HPP_