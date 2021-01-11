#ifndef _SHADEPLAYER_INCLUDE_SHADERLAYER_HPP_
#define _SHADEPLAYER_INCLUDE_SHADERLAYER_HPP_
#define DEFAULT_ZINDEX 100

#include <map>
#include <string>
#include <raylib.h>
#include "ILayer.hpp"
#include "../Core/EventEmitter.hpp"

using namespace std;

namespace shade {

  /** 
   * 
   *  Specialized Layer class for rendering a post-process (screen) shader
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

      void Init(const char* filePath, bool isReactive){
        m_shader = LoadShader("", filePath);

        // setup default shader uniform params
        AddShaderParam("iTime");
        AddShaderParam("iResolution");
        float res[2] = { (float)m_dimensions.width, (float)m_dimensions.height };
        SetShaderValue(m_shader, m_uniformLocs["iResolution"], res, UNIFORM_VEC2);

        // setup avx music-reactive shader uniform params and event binding
        if(isReactive){
          AddShaderParam("avxFFTSpectrum");
          AddShaderParam("avxRMS");
          AddShaderParam("avxPeak");
          AddShaderParam("avxBeatImp");
          EventEmitter::On<FFTUpdateEvent>([&](FFTUpdateEvent& e) { OnFFTUpdate(e); });
        }
      }
      // only bound when shader layer was set to reactive on Init
      void OnFFTUpdate(FFTUpdateEvent &e){
        SetShaderValueV(m_shader, m_uniformLocs["avxNoteBin"], &e.NoteBin, UNIFORM_INT, e.NoteBinSize);
        SetShaderValueV(m_shader, m_uniformLocs["avxFreqBin"], &e.FreqBin, UNIFORM_INT, e.FreqBinSize);
        SetShaderValue(m_shader, m_uniformLocs["avxRMS"], &e.RMS, UNIFORM_FLOAT);
        SetShaderValue(m_shader, m_uniformLocs["avxPeak"], &e.Peak, UNIFORM_FLOAT);
        // SetShaderValue(m_shader, m_uniformLocs["avxBeatImp"], &avxBeatImp, UNIFORM_FLOAT);
      }

      void Render(float deltaTime){
        // setup render
        m_iTime += deltaTime;
        SetShaderValue(m_shader, m_uniformLocs["iTime"], &m_iTime, UNIFORM_FLOAT);
        BeginShaderMode(m_shader);

        // maybe provide some kind of hook to customize rendering without needing to subclass?
        // render empty rec for shader visualization
        DrawRectangle(m_dimensions.x, m_dimensions.y, m_dimensions.width, m_dimensions.height, WHITE);

        // end render
        EndShaderMode();
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
  };
}
#endif // _SHADEPLAYER_INCLUDE_SHADERLAYER_HPP_