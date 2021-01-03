#ifndef _SHADEPLAYER_INCLUDE_AUDIOPLAYER_HPP_
#define _SHADEPLAYER_INCLUDE_AUDIOPLAYER_HPP_

#include <raylib.h>
#include <vector>
#include <string>
#include "AudioAnalyzer.hpp"

using namespace std;

namespace shade {
  enum PlaybackState { STOPPED, LOADING, PLAYING, PAUSED, ERROR };
  struct Artist {
    string Name;    
  };
  struct Song {
    string Name;
    Artist* pArtist;
    string Filepath;
  };

  class AudioPlayer {
    public:
      AudioPlayer(){
        m_audioAnalyzer = AudioAnalyzer();
      };
      ~AudioPlayer(){
        m_playlist.clear();
      };

      void Update(float deltaTime){
        if(m_state == PLAYING){
          // alternatively check GetMusicTimePlayed >= GetMusicTimeLength?
          if(IsMusicPlaying(m_playingMusic)){
            // update cursor ish
            // do fft spectrum analysis?
            //m_audioAnalyzer.Update();    
            UpdateMusicStream(m_playingMusic);
          } else { // sound finished playing
            OnMusicFinished();
          }
        }
      }

      void OnMusicFinished(){
        Stop();
        UnloadMusicStream(m_playingMusic);
        PlayNext();
      }

      //////////////////////////////
      /// Play Controls
      //////////////////////////////
      // begins playing current song in playlist, or continues from pause
      void Play(){
        if(m_playlist.size() <= 0 ) return;

        m_state = PlaybackState::LOADING;
        auto song = m_playlist[m_playlistIndex];
        m_playingMusic = LoadMusicStream(song.Filepath.c_str());
        PlayMusicStream(m_playingMusic);

        if(IsMusicPlaying(m_playingMusic)){
          m_state = PlaybackState::PLAYING;
          //m_audioAnalyzer.Init();
        } else {
          m_state = PlaybackState::ERROR;
        }
      }
      // stops playing, resets play cursor
      void Stop(){

        StopMusicStream(m_playingMusic);
        m_state = PlaybackState::STOPPED;
      }
      // pause playing
      void Pause(){
        PauseMusicStream(m_playingMusic);
        m_state = PlaybackState::PAUSED;
      }
      // plays next song in playlist
      void PlayNext(){
        if(m_playlist.size() <= 0){
          return;
        } else if(m_playlist.size() - 1 == m_playlistIndex) {
          m_playlistIndex = 0;          
        } else {
          ++m_playlistIndex;
        }
        Play();
        m_state = PlaybackState::PLAYING;
      }
      // plays previous song in playlist
      void PlayPrev(){

        m_state = PlaybackState::PLAYING;
      }
      // update playback volume
      void SetVolume(){}
      //////////////////////////////

      //////////////////////////////
      /// Playlist Management
      //////////////////////////////
      // return what state the player is in
      PlaybackState GetState(){
        return m_state;
      }

      // adds songs to playlist
      void AddSongs(char** files, int count){
        for(int i = 0; i < count; ++i){
          // do more things l8r like try parsing artist name and song name
          Song s = {
            "Unknown",
            nullptr,
            files[i]
          };
          m_playlist.push_back(s);
        }        
      }
      // removes songs to playlist
      void RemoveSongs(){
        // probs need more cleanup l8r
        m_playlist.clear();
      }
      //////////////////////////////

      vector<string> GetSongNames(){
        vector<string> v;
        for(auto i = 0; i < m_playlist.size(); ++i){
          v.push_back(m_playlist[i].Filepath);
        }
        return v;
      }

    protected:
      int m_playlistIndex = 0;
      Music m_playingMusic;
      vector<Song> m_playlist;
      PlaybackState m_state = PlaybackState::STOPPED;
      AudioAnalyzer m_audioAnalyzer;

    private:
  };
}
#endif // _SHADEPLAYER_INCLUDE_AUDIOPLAYER_HPP_