#ifndef _SHADEPLAYER_INCLUDE_SHADERLAYER_HPP_
#define _SHADEPLAYER_INCLUDE_SHADERLAYER_HPP_
#define DEFAULT_ZINDEX 100

#include <raylib.h>
#include "ILayer.hpp"
//temp
#include "../Core/EventEmitter.hpp"

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
      }

      void Init(const char* filePath){
        m_shader = LoadShader("", filePath);

        // how to handle shader params... perhaps just create one standard 
        // "interface" or set of params every shader gets passed, whether or not they use them
        m_iTime = 0.0f;
        m_iTimeLoc = GetShaderLocation(m_shader, "iTime");
        m_iResolutionLoc = GetShaderLocation(m_shader, "iResolution");
        m_iResolution[0] = (float)m_dimensions.width;
        m_iResolution[1] = (float)m_dimensions.height;
        SetShaderValue(m_shader, m_iResolutionLoc, m_iResolution, UNIFORM_VEC2);

        // temp
        EventEmitter::On<FFTUpdateEvent>([&](FFTUpdateEvent& e) { OnFFTUpdate(e); });
        m_fftSamplesLoc = GetShaderLocation(m_shader, "fftSamples");
        m_rmsLoc = GetShaderLocation(m_shader, "rms");
      }
      // temp
      void OnFFTUpdate(FFTUpdateEvent &e){
        float rms = 0;
        for(int i = 0; i < 512; ++i){
          m_fftSamples[i] = e.FreqBins[i];
          rms += e.FreqBins[i];
        }
        rms /= 512;
        SetShaderValueV(m_shader, m_fftSamplesLoc, &m_fftSamples, UNIFORM_INT, 512);
        SetShaderValue(m_shader, m_rmsLoc, &rms, UNIFORM_FLOAT);
      }

      void Render(float deltaTime){
        m_iTime += deltaTime;
        SetShaderValue(m_shader, m_iTimeLoc, &m_iTime, UNIFORM_FLOAT);
        BeginShaderMode(m_shader);
        DrawRectangle(m_dimensions.x, m_dimensions.y, m_dimensions.width, m_dimensions.height, WHITE);
        EndShaderMode();
      }

    protected:
      bool m_isActive = true;
      int m_zIndex = DEFAULT_ZINDEX;
      Rectangle m_dimensions;

      virtual ShaderLayer* clone_impl() const override { return new ShaderLayer(*this); }
    private:
      // temp props
      Shader m_shader;
      float m_iTime;
      int m_iTimeLoc;
      int m_iResolutionLoc;
      float m_iResolution[2] = {};
      int m_fftSamplesLoc;
      int m_fftSamples[512] = {255};
      int m_rmsLoc;
  };
}
#endif // _SHADEPLAYER_INCLUDE_SHADERLAYER_HPP_