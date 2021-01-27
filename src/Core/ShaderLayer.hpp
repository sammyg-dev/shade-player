#ifndef _SHADEPLAYER_INCLUDE_SHADERLAYER_HPP_
#define _SHADEPLAYER_INCLUDE_SHADERLAYER_HPP_
#define DEFAULT_ZINDEX 100

#include <map>
#include <string>
#include <raylib.h>
#include "ILayer.hpp"
#include "../Core/EventEmitter.hpp"

using namespace std;

#define GLSL_VERSION 330
#define HOT_RELOAD true

namespace shade {

  /** 
   * 
   *  Specialized Layer class for rendering a post-process (screen) shader
   *  todo: shader hot reloader
   * 
   */
  class ShaderLayer : public ILayer {
    public:
      ShaderLayer(){}
      ShaderLayer(Rectangle dimensions){
        m_dimensions = dimensions;
      }

      ~ShaderLayer(){
        UnloadShader(m_shader);
      }

      void ReloadShader(const char* filePath, bool isReactive){
        Shader updated = LoadShader(0, filePath);
        if(updated.id != GetShaderDefault().id){
          printf("Shader reloading...\n");
          UnloadShader(m_shader);
          m_shaderFile = filePath;
          m_isReactive = isReactive;
          m_shader = updated;
          m_iTime = 0.0;
          m_modTime = GetFileModTime(m_shaderFile);
          m_uniformLocs.clear();
          InitShaderParams(isReactive);
          printf("Shader reloaded\n");
        }
      }

      void InitShaderParams(bool isReactive){
        // setup default shader uniform params
        AddShaderParam("iTime");
        AddShaderParam("iResolution");
        AddShaderParam("iMouse");
        float res[2] = { (float)m_dimensions.width, (float)m_dimensions.height };
        SetShaderValue(m_shader, m_uniformLocs["iResolution"], res, UNIFORM_VEC2);

        // setup avx music-reactive shader uniform params and event binding
        if(isReactive){
          // need a couple smooth bins... do i even want them not smoothed?
          // and do i smooth them in the analyzer or here...
          //AddShaderParam("avxFreqSmoothBin")
          AddShaderParam("avxFreqBin");
          AddShaderParam("avxNoteBin");
          AddShaderParam("avxRMS");
          AddShaderParam("avxPeak");
          AddShaderParam("avxBeatImp");
        }
      }

      void Init(const char* filePath, bool isReactive){
        m_shaderFile = filePath;
        m_isReactive = isReactive;
        m_shader = LoadShader(0, filePath);
        m_modTime = GetFileModTime(m_shaderFile);
        InitShaderParams(isReactive);

        EventEmitter::On<SetShaderFileEvent>([&](SetShaderFileEvent e) { 
          // todo: crossfade between old and new shader
          string fs = "resources/shaders/" + e.Filename;
          ReloadShader(fs.c_str(), true);//ohfuckthat
        });

        // setup event binding
        if(isReactive){
          EventEmitter::On<FFTUpdateEvent>([&](FFTUpdateEvent& e) { OnFFTUpdate(e); });
        }
      }
      // only bound when shader layer was set to reactive on Init
      void OnFFTUpdate(FFTUpdateEvent &e){
        SetShaderValueV(m_shader, m_uniformLocs["avxNoteBin"], e.NoteBin, UNIFORM_INT, e.NoteBinSize);
        SetShaderValueV(m_shader, m_uniformLocs["avxFreqBin"], e.FreqBin, UNIFORM_INT, e.FreqBinSize);
        SetShaderValue(m_shader, m_uniformLocs["avxRMS"], &e.RMS, UNIFORM_FLOAT);
        SetShaderValue(m_shader, m_uniformLocs["avxPeak"], &e.Peak, UNIFORM_FLOAT);
        // SetShaderValue(m_shader, m_uniformLocs["avxBeatImp"], &avxBeatImp, UNIFORM_FLOAT);
        // printf("rms: %f, peak: %f\n", e.RMS, e.Peak);
        // printf("fb0: %i, nb0: %i\n",e.FreqBin[0], e.NoteBin[0]);
      }

      void Render(float deltaTime, RenderTexture2D target){
        // setup render
        m_iTime += deltaTime;
         // test resetting iTime, i think this is causing the weird banding in the shaders
        if(m_iTime > 3600.0) m_iTime = 1.0;

        Vector2 mouse = GetMousePosition();
        SetShaderValue(m_shader, m_uniformLocs["iTime"], &m_iTime, UNIFORM_FLOAT);
        SetShaderValue(m_shader, m_uniformLocs["iMouse"], &mouse, UNIFORM_VEC2);
        BeginShaderMode(m_shader);

        // maybe provide some kind of hook to customize rendering without needing to subclass?
        // render empty rec for shader visualization
        //DrawRectangle(m_dimensions.x, m_dimensions.y, m_dimensions.width, m_dimensions.height, WHITE);
        // todo: support multiple shader layers... multipass? 
        DrawTextureRec(target.texture, { 0, 0, target.texture.width, -target.texture.height }, { 0, 0 }, BLANK);

        // end render
        EndShaderMode();

        // hot reload
        #ifdef HOT_RELOAD
        m_modTimer += deltaTime;
        if(m_modTimer >= 1.0){
          long modTime = GetFileModTime(m_shaderFile);
          if(modTime != m_modTime){
            printf("Shader hot reloading...\n");
            ReloadShader(m_shaderFile, m_isReactive);
            m_modTime = modTime;
            printf("Shader hot reload finished\n");
          }
          
          m_modTimer = 0;
        }
        #endif
      }

      // public interface for adding a custom shader param
      void AddShaderParam(string paramName){
        m_uniformLocs.insert(pair<string, int>(paramName, GetShaderLocation(m_shader, paramName.c_str())));
      }

      // public interface for setting a custom shader param value
      void SetShaderParamValue(string paramName, const void* paramValue, ShaderUniformDataType paramType) {
        SetShaderValue(m_shader, m_uniformLocs[paramName], paramValue, paramType);
      }

      // public interface for setting a custom shader param vector value
      void SetShaderParamValueV(string paramName, const void* paramValue, ShaderUniformDataType paramType, int paramCount){
        SetShaderValueV(m_shader, m_uniformLocs[paramName], paramValue, paramType, paramCount);
      }

    protected:
      bool m_isActive = true;
      int m_zIndex = DEFAULT_ZINDEX;
      Rectangle m_dimensions;

      virtual ShaderLayer* clone_impl() const override { return new ShaderLayer(*this); }
    private:
      // running total time ran, passed to shader by default
      float m_iTime = 0;
      // shader ref for this layer
      Shader m_shader;
      // map of all uniform shader locations
      map<string, int> m_uniformLocs;
      // turns on/off music reactivity
      bool m_isReactive = false;

      // hot reload ish
      const char* m_shaderFile = nullptr;
      long m_modTime = 0;
      float m_modTimer = 0;
  };
}
#endif // _SHADEPLAYER_INCLUDE_SHADERLAYER_HPP_