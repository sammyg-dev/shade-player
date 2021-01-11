#ifndef _SHADEPLAYER_INCLUDE_AUDIOTYPES_H
#define _SHADEPLAYER_INCLUDE_AUDIOTYPES_H

#define FFT_SAMPLES_COUNT 512;

#include <string>
using namespace std;

// utils header for shared enums and structs
// todo - maybe sub-namespace l8r?
namespace shade {
  enum PlaybackState { STOPPED, LOADING, PLAYING, PAUSED, SONG_FINISHED, ERROR };
  enum LoopState { NONE, SONG, PLAYLIST };
  struct Artist {
    string Name;    
  };
  struct Song {
    string Name;
    Artist* pArtist;
    string Filepath;
    float Length;
  };
}
#endif // _SHADEPLAYER_INCLUDE_AUDIOTYPES_H