#ifndef _SHADEPDISPLAYOBJECT_INCLUDE_MODELDISPLAYOBJECT_HPP_
#define _SHADEPDISPLAYOBJECT_INCLUDE_MODELDISPLAYOBJECT_HPP_

#define DEFAULT_ZINDEX 100

#include <memory>
#include <vector>
#include <string>
#include "./IDisplayObject.hpp"
#include "raylib.h"

using namespace std;

namespace shade {

  /** 
   * 
   *  Base class for any renderable object, defining an interface to be used by it's Layer container
   * 
   */
  class ModelDisplayObject : public IDisplayObject {
    public:
      ModelDisplayObject(){}
      ~ModelDisplayObject(){
        for(auto m : m_pModels){
          UnloadModel(*m); // not sure if this unloads textures as well...
        }
        m_pModels.clear();
      }

      void Render(float deltaTime) override{
        for( auto m : m_pModels ){
          // we may need some kind of asset struct soon
          DrawModel(*m, {0, 0, 0}, m_scale, WHITE);
        }
      }

      void UpdateScale(float newScale) override {
        m_scale = newScale;
      }

      // only supports TRIANGLE MESHES with no GROUPS and shit like that,
      // keep it real basic for now
      void AddModel(string fileName, string diffuse){
        // this doesn't work for more complex models (and gltf is wonky) so lets either
        // find a new lib or write our own...this will take time not worth rn
        auto pm = make_shared<Model>(LoadModel(fileName.c_str()));
        

        // hmhmhm...i wonder how game engines do this...
        // may need some kind of asset packer/loader
        Texture2D tex = LoadTexture(diffuse.c_str());
        SetMaterialTexture(&pm->materials[0], MAP_DIFFUSE, tex);
        // more l8r

        m_pModels.push_back(pm);
      }

      void AddModel(Model& model){}
      void RemoveModel(){};

    protected:
      float m_scale = 1.0;
      vector<shared_ptr<Model>> m_pModels;

      virtual ModelDisplayObject* clone_impl() const override { return new ModelDisplayObject(*this); }
    private:
  };
}

#endif // _SHADEPDISPLAYOBJECT_INCLUDE_MODELDISPLAYOBJECT_HPP_