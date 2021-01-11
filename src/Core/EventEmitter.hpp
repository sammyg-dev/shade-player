#ifndef _SHADEPLAYER_INCLUDE_EVENTMANAGER_HPP_
#define _SHADEPLAYER_INCLUDE_EVENTMANAGER_HPP_

#include <functional>
#include <vector>
#include "../Audio/AudioTypes.h"

namespace shade {
  // events defined here for now, figure out better loc l8r
  //////////////////////////////
  /// Audio Player Events
  //////////////////////////////
  struct PlayClickEvent {};
  struct PauseClickEvent {};
  struct StopClickEvent {};
  struct PlayNextClickEvent {};
  struct PlayPrevClickEvent {};
  struct VolumeUpdateEvent {float Value;};
  struct RemoveSongByIndexEvent { int Index; };
  // this is kind of a hack
  struct GetVolumeEvent { float Value; };
  struct GetPlaybackStateEvent { PlaybackState State; };
  struct GetPlaylistEvent { vector<Song> Playlist; };

  // analyzer events
  struct FFTUpdateEvent {
    int* FreqBin;
    int FreqBinSize;
    int* NoteBin;
    int NoteBinSize;
    float RMS;
    float Peak;
  };
  //////////////////////////////
  //////////////////////////////
  /// Shader Param Events
  //////////////////////////////
  //struct UpdateShaderParamEvent
  //////////////////////////////

  /*
   *  A basic event emitter class using std::function and template functions
   *  Code adopted from: https://stackoverflow.com/a/59707355 by super
   *  example use (using lambda): 
   *    EventManager::On<MouseClickEvent>([&](MouseClickEvent& event) { OnClick(event); });
   *    EventManager::Emit<MouseClickEvent>(event);
   */
  class EventEmitter {
    public:
      // Adds a listener
      template <typename T>
      static void On(std::function<void(T&)> callback) {
        get_listeners<T>().push_back(std::move(callback));
      }

      // emits event to all listeners
      template <typename T>
      static void Emit(T& event){
        for(auto& listener : get_listeners<T>()){
          listener(event);
        }
      }

    private:
      template <typename T>
      static std::vector<std::function<void(T&)>>& get_listeners() {
        static std::vector<std::function<void(T&)>> listeners;
        return listeners;
      }
  };
}

#endif //_SHADEPLAYER_INCLUDE_EVENTMANAGER_HPP_
