#ifndef _SHADEPDisplayObject_INCLUDE_IDISPLAYOBJECT_HPP_
#define _SHADEPDisplayObject_INCLUDE_IDISPLAYOBJECT_HPP_
#define DEFAULT_ZINDEX 100

#include <memory>

namespace shade {

  /** 
   * 
   *  Base class for any renderable object, defining an interface to be used by it's Layer container
   * 
   */
  class IDisplayObject {
    public:
      virtual ~IDisplayObject(){}

      virtual void UpdateScale(float newScale) = 0;// temp

      virtual void Render(float deltaTime) = 0;


      auto clone() const { return std::unique_ptr<IDisplayObject>(clone_impl()); }
    protected:
      bool m_isActive = true;
      virtual IDisplayObject* clone_impl() const = 0;

    private:
  };
}

#endif // _SHADEPDisplayObject_INCLUDE_IDISPLAYOBJECT_HPP_