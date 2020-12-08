#ifndef _SHADEPDisplayObject_INCLUDE_IDISPLAYOBJECT_HPP_
#define _SHADEPDisplayObject_INCLUDE_IDISPLAYOBJECT_HPP_
#define DEFAULT_ZINDEX 100

namespace shade {

  /** 
   * 
   *  Base class for any renderable object, defining an interface to be used by it's Layer container
   * 
   */
  class IDisplayObject {
    public:
      virtual ~IDisplayObject();

      virtual void Render(float deltaTime) = 0;

    protected:
      bool m_isActive = true;

    private:
  };
}

#endif // _SHADEPDisplayObject_INCLUDE_IDISPLAYOBJECT_HPP_