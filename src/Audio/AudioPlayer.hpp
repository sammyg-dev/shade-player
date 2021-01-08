#ifndef _SHADEPLAYER_INCLUDE_AUDIOPLAYER_HPP_
#define _SHADEPLAYER_INCLUDE_AUDIOPLAYER_HPP_

#include <raylib.h>
#include <vector>
#include <string>
#include "AudioTypes.h"
#include "AudioAnalyzer.hpp"
#include "../Core/EventEmitter.hpp"

using namespace std;

namespace shade {
  /*
   *  AudioPlayer based on raylib's AudioStream
   *  features:
   *    - songs playlist with standard controls
   *    - .wav file support (todo: add more, limited by AudioAnalyzer atm)
   *    - AudioAnalyzer for real-time audio analysis
   */
  class AudioPlayer {
    public:
      AudioPlayer(){
        m_samplingRate = 44100.0;
        m_spu = 4096;
        m_audioAnalyzer = AudioAnalyzer();

        BindEvents();
      };
      ~AudioPlayer(){
        if(m_audioData != nullptr){
          delete [] m_audioData;
        }
        if(m_audioBuff != nullptr){
          delete [] m_audioBuff;
        }

        if(m_pAudioStream != nullptr){
          CloseAudioStream(*m_pAudioStream);        
          delete m_pAudioStream;
          m_pAudioStream = nullptr;
        }
        
        m_playlist.clear();
      };      

      void Update(float deltaTime){
        if(m_state == PLAYING){
          m_playTime += deltaTime;
          if(IsAudioStreamProcessed(*m_pAudioStream)){
            for(int i = 0; i < m_spu; ++i){
              // check for end of song data
              if(m_audioDataCursor > m_audioDataSize){
                // loop buffer else fill with zeroes and playnext
                if(m_loopState == LoopState::SONG || (m_playlist.size() == 1 && m_loopState == LoopState::PLAYLIST)){
                  // this should only trigger once, we reset ish
                  m_audioDataCursor = 0;
                  ResetPlayTime();
                } else {
                  m_audioBuff[i] = -100;
                  m_state = PlaybackState::SONG_FINISHED;
                }
              } else { // not end, just set data
                m_audioBuff[i] = m_audioData[m_audioDataCursor];
              }
              ++m_audioDataCursor;
            }
          
            // } else { // setup next audio buffer
            //   int j = 0;
            //   for(int i = m_audioDataCursor; i < m_audioDataCursor + m_spu; ++i){
            //     m_audioBuff[j] = m_audioData[i];
            //     ++j;
            //   }
            //   m_audioDataCursor += m_spu;
            // }
            UpdateAudioStream(*m_pAudioStream, m_audioBuff, m_spu);
          }
          // update analyzer
          m_audioAnalyzer.Update(deltaTime, m_audioBuff, m_spu, m_audioDataCursor);
        } else if (m_state == SONG_FINISHED){
          if(IsAudioStreamProcessed(*m_pAudioStream)){
            OnSongFinished();
          }
        }
        // do nothing
      }

      void OnSongFinished(){
        Stop();
        CloseAudioStream(*m_pAudioStream);
        delete m_pAudioStream;
        m_pAudioStream = nullptr;
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
        if(!IsAudioDeviceReady()){
          printf("Audio device not ready! Cannot play song");
          m_state = PlaybackState::ERROR;
          return;
        }

        // things todo:
        // - support 2 channels? only seems to work with setting mono this way
        // - instead of loading wave, just us a Music stream and grab the audio buffer per update somehow?        
        Wave wave = LoadWave(song.Filepath.c_str()); // not sure if i need to unload?
        WaveFormat(&wave, 44100, 16, 1);
        
        m_audioDataSize = wave.sampleCount;
        m_audioData = new short[m_audioDataSize];
        m_audioBuff = new short[m_spu];
        memcpy(m_audioData, wave.data, m_audioDataSize*2);

        // set the audio buff from raw data
        for(int i = 0; i < m_spu; ++i){
          m_audioBuff[i] = m_audioData[i];
        }
        // update audio buff cursor position
        m_audioDataCursor += m_spu;

        // begin playing stream from audio buffer
        m_pAudioStream = new AudioStream(InitAudioStream(m_samplingRate, wave.sampleSize, 1));
        SetVolume(m_volume); // set audio stream volume
        UpdateAudioStream(*m_pAudioStream, m_audioBuff, m_spu);
        PlayAudioStream(*m_pAudioStream);

        // init audio analyzer
        m_audioAnalyzer.Init(m_audioBuff, m_spu, m_audioDataSize);

        // update plaeyr state
        if(IsAudioStreamPlaying(*m_pAudioStream)){
          m_state = PlaybackState::PLAYING;
          //m_audioAnalyzer.Init();
        } else {
          m_state = PlaybackState::ERROR;
        }
      }
      // stops playing, resets play cursor
      void Stop(){
        StopAudioStream(*m_pAudioStream);
        m_state = PlaybackState::STOPPED;
        ResetPlayTime();
        ResetBuffers();
      }
      // pause playing
      void Pause(){
        PauseAudioStream(*m_pAudioStream);
        m_state = PlaybackState::PAUSED;
      }
      // plays next song in playlist
      void PlayNext(){
        if(m_playlist.size() <= 1){
          return;
        } else if(m_playlist.size() - 1 == (size_t)m_playlistIndex) {
          m_playlistIndex = 0;          
        } else {
          ++m_playlistIndex;
        }
        Play();
      }
      // plays previous song in playlist
      void PlayPrev(){
        if(m_playlist.size() <= 1){
            return;
          } else if(m_playlistIndex == 0) {
            m_playlistIndex = m_playlist.size() - 1;          
          } else {
            --m_playlistIndex;
          }
        Play();
      }
      // update playback volume
      void SetVolume(float value){
        float minV = 0.0;
        float maxV = 1.0;
        m_volume = max(minV, min(value, maxV));
        

        // only set if audio stream is valid
        // this means we must call SetVolume after we init the audio stream
        if(m_pAudioStream != nullptr){
          SetAudioStreamVolume(*m_pAudioStream, value);
        }
      }
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
          // basic file name parse for now
          string path = files[i];
          string fileName(path.substr(path.rfind("\\") + 1));
          Song s = {
            fileName, // do more to find song name l8r
            nullptr, // and artist info so we can set this
            path
          };
          m_playlist.push_back(s);
        }        
      }
      // removes songs to playlist
      void RemoveSongByIndex(int index){
        m_playlist.erase(m_playlist.begin() + index);
      }

      // void RemoveSong(Song& song){
      //   m_playlist.erase(remove(m_playlist.begin(), m_playlist.end(), [song](Song& s){ song.Filepath == s.Filepath;}), m_playlist.end());
      // }
      //////////////////////////////

      vector<string> GetSongNames(){
        vector<string> v;
        for(auto i = 0; (size_t)i < m_playlist.size(); ++i){
          v.push_back(m_playlist[i].Filepath);
        }
        return v;
      }

    protected:      
      // audio stream obj
      AudioStream* m_pAudioStream = nullptr;
      // samping rate of audio
      float m_samplingRate;
      // samples per update
      int m_spu;
      // raw audio file data
      short* m_audioData = nullptr;
      // raw audio data buff size
      int m_audioDataSize;
      // audio data buffer for each update
      short* m_audioBuff = nullptr;
      // position of audio data reader
      int m_audioDataCursor = 0;

      // index of current song in playlist vec
      int m_playlistIndex = 0;
      // Song vector
      vector<Song> m_playlist;
      // loop control
      LoopState m_loopState = LoopState::PLAYLIST;
      // current state of player
      PlaybackState m_state = PlaybackState::STOPPED;
      // play time in seconds
      float m_playTime = 0;

      // audio analyzer used to calc DFT in real-time
      AudioAnalyzer m_audioAnalyzer;

      void ResetPlayTime(){
        m_playTime = 0;
      }

      void ResetBuffers(){
        delete [] m_audioData;
        delete [] m_audioBuff;
        m_audioData = nullptr;
        m_audioBuff = nullptr;
        m_audioDataSize = 0;
        m_audioDataCursor = 0;
      }

      //////////////////////////////
      /// Event bindings
      //////////////////////////////
      void BindEvents(){
        EventEmitter::On<PlayClickEvent>([&](PlayClickEvent& e) { OnPlayClick(e); });
        EventEmitter::On<PauseClickEvent>([&](PauseClickEvent& e) { OnPauseClick(e); });
        EventEmitter::On<StopClickEvent>([&](StopClickEvent& e) { OnStopClick(e); });
        EventEmitter::On<PlayNextClickEvent>([&](PlayNextClickEvent& e) { OnPlayNextClick(e); });
        EventEmitter::On<PlayPrevClickEvent>([&](PlayPrevClickEvent& e) { OnPlayPrevClick(e); });
        EventEmitter::On<VolumeUpdateEvent>([&](VolumeUpdateEvent& e) { OnVolumeUpdate(e); });
        EventEmitter::On<GetVolumeEvent>([&](GetVolumeEvent& e) { OnGetVolume(e); });
        EventEmitter::On<GetPlaybackStateEvent>([&](GetPlaybackStateEvent& e) { OnGetPlaybackState(e); });
        EventEmitter::On<GetPlaylistEvent>([&](GetPlaylistEvent& e) { OnGetPlaylist(e); });
        EventEmitter::On<RemoveSongByIndexEvent>([&](RemoveSongByIndexEvent& e) { OnRemoveSongByIndex(e); });
      }
      // handle play btn click from UI
      void OnPlayClick(PlayClickEvent e){
        Play();
      }
      // handle pause btn click from UI
      void OnPauseClick(PauseClickEvent e){
        Pause();
      }
      // handle play btn click from UI
      void OnStopClick(StopClickEvent e){
        Stop();
      }
      // handle play next btn click from UI
      void OnPlayNextClick(PlayNextClickEvent e){
        PlayNext();
      }
      // handle play prev btn click from UI
      void OnPlayPrevClick(PlayPrevClickEvent e){
        PlayPrev();
      }
      // handle volume slider update from UI
      void OnVolumeUpdate(VolumeUpdateEvent e){
        printf("Volume in: %f\n", e.Value);
        SetVolume(e.Value);
      }
      void OnGetVolume(GetVolumeEvent& e){
        e.Value = m_volume;
      }
      void OnGetPlaybackState(GetPlaybackStateEvent& e){
        e.State = m_state;
      }
      void OnGetPlaylist(GetPlaylistEvent& e){
        e.Playlist = m_playlist;// might need to copy ish manually?
      }
      void OnRemoveSongByIndex(RemoveSongByIndexEvent& e){
        RemoveSongByIndex(e.Index);
      }
      //////////////////////////////
    private:
      float m_volume = 1.0;
      bool __you_cant_touch_this = true;
  };
}
#endif // _SHADEPLAYER_INCLUDE_AUDIOPLAYER_HPP_