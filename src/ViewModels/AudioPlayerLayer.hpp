#ifndef _SHADEPLAYER_INCLUDE_AUDIOPLAYERLAYER_HPP_
#define _SHADEPLAYER_INCLUDE_AUDIOPLAYERLAYER_HPP_
#define DEFAULT_ZINDEX 100



#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_RICONS
#include <ricons.h>
#include <raygui.h>

#include <string>
#include "../Audio/AudioTypes.h"
#include "../Core/EventEmitter.hpp"
#include "../Core/ILayer.hpp"

using namespace std;

namespace shade {

  /** 
   * 
   *  GUI Layer for AudioPlayer (which acts as the Model)
   *  We implement all our app's GUIs as Layers following the MVVM pattern
   *  todo:
   *    - some kind of event emitter to communicate between this and AudioPlayer
   *    - decouple the View from ViewModel (maybe some kind of json or xml parser?)
   * 
   */
  class AudioPlayerLayer : public ILayer {
    public:
      AudioPlayerLayer(){
        
      }
      AudioPlayerLayer(Rectangle dimensions){
        m_dimensions = dimensions;
        // move this l8r
        GuiLoadStyle("resources/styles/cherry/cherry.rgs");
      }

      // ~AudioPlayerLayer(){}

      void Init(const char* filePath){

      }

      void Render(float deltaTime){
        // hardcode the gui for now
        if(m_isActive){
          // bottom bar controls
          float h = 50;
          float w = m_dimensions.width;
          float cw = m_dimensions.width / 2;
          float ch = m_dimensions.height - h;
          // bottom bar
          DrawRectangle(0, m_dimensions.height - h, w, h, BLACK);          
          // play/pause btn
          GetPlaybackStateEvent currState = { PlaybackState::ERROR };
          EventEmitter::Emit<GetPlaybackStateEvent>(currState);
          if(currState.State == PlaybackState::PLAYING){
            if (GuiButton({cw - 25, ch, 50, 50}, GuiIconText(RICON_PLAYER_PAUSE, ""))) { 
              PauseClickEvent e = {};
              EventEmitter::Emit<PauseClickEvent>(e);
            }
          } else if(currState.State == PlaybackState::PAUSED || currState.State == PlaybackState::STOPPED) {
            if (GuiButton({cw - 25, ch, 50, 50}, GuiIconText(RICON_PLAYER_PLAY, ""))) { 
              PlayClickEvent e = {};
              EventEmitter::Emit<PlayClickEvent>(e);
            }
          }
          // next btn
          if (GuiButton({cw + 100 - 25, ch, 50, 50}, GuiIconText(RICON_PLAYER_NEXT, ""))) { 
            PlayNextClickEvent e = {};
            EventEmitter::Emit<PlayNextClickEvent>(e);
          }
          // prev btn
          if (GuiButton({cw - 100 - 25, ch, 50, 50}, GuiIconText(RICON_PLAYER_PREVIOUS, ""))) { 
            PlayPrevClickEvent e = {};
            EventEmitter::Emit<PlayPrevClickEvent>(e);
          }
          // volume slider
          GetVolumeEvent currVolumeEv { -1 };
          // not sure what the perf is on doing this every update, but "it works"
          EventEmitter::Emit<GetVolumeEvent>(currVolumeEv);
          float newSliderVal = currVolumeEv.Value;
          string label = to_string((int)(currVolumeEv.Value * 100));
          newSliderVal = GuiSlider({ cw + 150, ch + 25 - 5, 150, 10}, "", label.c_str(), currVolumeEv.Value, 0, 1.0);
          if(newSliderVal != currVolumeEv.Value){
            VolumeUpdateEvent e = { newSliderVal };
            EventEmitter::Emit<VolumeUpdateEvent>(e);
          }
          

        }

      }

    protected:
      bool m_isActive = true;
      int m_zIndex = DEFAULT_ZINDEX;
      Rectangle m_dimensions;

      virtual AudioPlayerLayer* clone_impl() const override { return new AudioPlayerLayer(*this); }
    private:

  };
}
#endif // _SHADEPLAYER_INCLUDE_AUDIOPLAYERLAYER_HPP_