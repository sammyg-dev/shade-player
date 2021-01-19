#ifndef _SHADEPLAYER_INCLUDE_LAYER3D_HPP_
#define _SHADEPLAYER_INCLUDE_LAYER3D_HPP_
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
  class Layer3D: public ILayer {
    public:
      Layer3D(){

      }

      Layer3D(Rectangle rec){
        m_dimensions = rec;
      }

      ~Layer3D(){};

      void Init(const char* filePath, bool isReactive) override {
        m_camera = { { 2.0f, 3.0f, 2.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, CAMERA_PERSPECTIVE };
        SetCameraMode(m_camera, CAMERA_ORBITAL);
      };

      void Render(float deltaTime, RenderTexture2D target) override {
        UpdateCamera(&m_camera);  
        BeginTextureMode(target);
        ClearBackground(BLANK);
        BeginMode3D(m_camera);

        DrawGrid(100, 1.0f);
        DrawSphere( {1, 1, 1}, 1, GREEN);
        DrawSphere( {0.5, 0, 1}, 1, RED);
        DrawSphere( {0, 1, 1}, 1, BLUE);
        // for(size_t i = 0; i <= m_pDisplayObjects.size(); ++i){
        //   m_pDisplayObjects[i]->Render(deltaTime);
        // }

        EndMode3D();
        EndTextureMode();
        
        DrawTextureRec(target.texture, { 0, 0, target.texture.width, -target.texture.height }, { 0, 0 }, WHITE);
      };

      // add and resort ??
      void AddObj(){
        //std::sort(m_pDisplayObjects.begin(), m_pDisplayObjects.end(), ZIndexComparator())
      }

    protected:
      //std::vector<IDisplayObject*> m_pDisplayObjects;

      virtual Layer3D* clone_impl() const override { return new Layer3D(*this); }
      Camera m_camera =  { { 2.0f, 3.0f, 2.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, 45.0f, 0 };
  };
}
#endif // _SHADEPLAYER_INCLUDE_LAYER3D_HPP_