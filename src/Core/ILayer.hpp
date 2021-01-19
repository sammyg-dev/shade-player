#ifndef _SHADEPLAYER_INCLUDE_ILAYER_HPP_
#define _SHADEPLAYER_INCLUDE_ILAYER_HPP_
#define DEFAULT_ZINDEX 100

#include <raylib.h>
#include <memory>

namespace shade {

  /** 
   * 
   *  Layer interface so different Layer types can be used within the same container and have
   *  a shared interface (LayerStack needs this)
   * 
   */
  class ILayer {
    public:
      virtual ~ILayer(){}

      virtual void Init(const char* filePath, bool isReactive) = 0;
      virtual void Render(float deltaTime, RenderTexture2D target) = 0;

      // to avoid slicing, use a clone_impl func that our child classes can override
      // thanks to https://stackoverflow.com/a/43263477
      auto clone() const { return std::unique_ptr<ILayer>(clone_impl()); }
    protected:
      bool m_isActive = true;
      int m_zIndex;// = DEFAULT_ZINDEX;
      Rectangle m_dimensions;

      virtual ILayer* clone_impl() const = 0;
  };
}
#endif // _SHADEPLAYER_INCLUDE_ILAYER_HPP_