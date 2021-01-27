#ifndef _SHADEPLAYER_INCLUDE_AUDIOPLAYERLAYER_HPP_
#define _SHADEPLAYER_INCLUDE_AUDIOPLAYERLAYER_HPP_
#define DEFAULT_ZINDEX 100


#include <tuple> 
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
        GuiLoadStyle("resources/styles/shade/shade.rgs");
        int defFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
        printf("Loading custom font w/ default size: %i", defFontSize);
        Font font = LoadFontEx("resources/styles/shade/Maxellight.ttf", defFontSize, 0, 0);
        GuiSetFont(font);
        // we load font at larger size so it scales down instead of up
        GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
      }

      // ~AudioPlayerLayer(){}

      void Init(const char* filePath, bool isReactive){
        EventEmitter::On<PlaySongEvent>([&](PlaySongEvent& e) { OnPlaySong(e); });
      }      

      void Render(float deltaTime, RenderTexture2D target){
        // hardcode the gui for now
        if(m_isActive){
          // bottom bar controls
          float h = 50;
          float w = m_dimensions.width;
          float cw = m_dimensions.width / 2;
          float ch = m_dimensions.height - h;
          // bottom bar
          DrawRectangle(0, m_dimensions.height - h, w, h, Fade(BLACK, 0.9));          
          // play/pause btn
          GetPlaybackStateEvent currState = { PlaybackState::ERROR };
          // not sure what the perf is on doing this every update, but "it works"
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
          newSliderVal = GuiSlider({ cw + 200, ch + 25 - 5, 150, 10}, "", label.c_str(), currVolumeEv.Value, 0, 1.0);
          if(newSliderVal != currVolumeEv.Value){
            VolumeUpdateEvent e = { newSliderVal };
            EventEmitter::Emit<VolumeUpdateEvent>(e);
          }
          // open files btn (mayb do this l8r, for now just drag&drop)
          // if (GuiButton({w - 100 , ch, 50, 50}, GuiIconText(RICON_FOLDER_OPEN, ""))) { 
            

          // show song title
          if(!m_currentSong.Filepath.empty()){
            int defFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
            GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
            GuiDrawText(m_currentSong.Name.c_str(), { 10, 24, 200, 10}, GuiTextAlignment::GUI_TEXT_ALIGN_LEFT, Fade(RAYWHITE, 0.75));
            GuiSetStyle(DEFAULT, TEXT_SIZE, defFontSize);
          }

          // shade radio icon (btn left)
          // todo

          // visualizations "dropdown"
          m_showVizOptions = GuiToggle({w - 400 , ch, 200, 50}, m_selectedViz[1].c_str(), m_showVizOptions);
          // load these from an ini l8r
          string options[5][2] = { 
            {"linewaves.fs", "line waves" },
            {"starfield.fs", "star field"},
            {"clouds.fs", "cloud tunnel"},
            {"kifs.fs", "koch tunnel"},
            {"idkyet.fs", "WIP"}
          };          
          if(m_showVizOptions){
            for(int i = 0; i < sizeof options / sizeof options[0]; ++i){
              if(GuiButton({w - 400, ch - 52*(i+1), 200, 50}, options[i][1].c_str())){
                m_selectedViz[0] = options[i][0]; // bleh but easiest way to do this?
                m_selectedViz[1] = options[i][1];
                // emit option to shader layer... hmm this will be interesting
                SetShaderFileEvent e = { options[i][0] };
                EventEmitter::Emit<SetShaderFileEvent>(e);
                // close options
                m_showVizOptions = false;
              }
            }
          }
          // autoplay viz toggle
          m_isVizAutoPlaying = GuiToggle({w - 450 , ch, 50, 50}, GuiIconText(RICON_CAMERA, ""), m_isVizAutoPlaying);

          // temp placement of viz swapping over time
          if(m_isVizAutoPlaying){
            int randViz = GetRandomValue(0, 30000);
            if(randViz < sizeof options / sizeof options[0]){
              if(m_selectedViz[0] != options[randViz][0]){
                m_selectedViz[0] = options[randViz][0];
                m_selectedViz[1] = options[randViz][1];
                SetShaderFileEvent e = { options[randViz][0] };
                EventEmitter::Emit<SetShaderFileEvent>(e);
              }
            }
          }          

          // }
          // playlist show/hide toggle
          m_showPlaylist = GuiToggle({w - 100 , ch, 100, 50}, "playlist", m_showPlaylist);
          if(m_showPlaylist){
            float pw = w / 3;
            float ph = m_dimensions.height * 2 / 3;
            Rectangle panelRec = { w - pw, ch - ph, pw, ph };
            DrawRectangle(panelRec.x,panelRec.y, panelRec.width, panelRec.height, Fade(GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL)), 0.8));
            GetPlaylistEvent playlistEvent = { vector<Song>() };
            EventEmitter::Emit<GetPlaylistEvent>(playlistEvent);

            // playlist header
            // shuffle btn -> shuffles current playlist
            int hy =26;
            DrawRectangle(panelRec.x, panelRec.y - hy, panelRec.width, hy, Fade(GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_NORMAL)), 0.85));
            if(GuiButton({panelRec.x, panelRec.y - hy, panelRec.width, hy}, GuiIconText(RICON_SHUFFLE, ""))){
              // emit to audio player which will do shuffling
              ShufflePlaylistEvent e = {};
              EventEmitter::Emit<ShufflePlaylistEvent>(e); // maybe this should return new playlist
              // todo - we should reset current song to top of shuffled playlist but eh nbd
            }

            m_panelContentRec = { w - pw-1, ch - ph-1, pw, playlistEvent.Playlist.size() * 50 }; // loop and calc
            //printf("scroll: %f %f\n", m_panelScroll.x, m_panelScroll.y);
            // panel content            
            BeginScissorMode(panelRec.x, panelRec.y, panelRec.width, panelRec.height);
            DrawPlaylist(panelRec, playlistEvent.Playlist);
            EndScissorMode();    
            UpdatePanelScroll(panelRec);
          }        
        }
      }

      // there was a GetMouseWheelMove bug in 3.0 so beware if reverting raylib back
      void UpdatePanelScroll(Rectangle &bounds){
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds)){
          float wheel = GetMouseWheelMove();
          float speed = 20.0;
          //printf("m_panelScroll: %f wheel: %f\n", m_panelScroll.y, wheel);
          m_panelScroll.y += wheel * speed;
          float minV = m_panelContentRec.height > bounds.height ? m_panelContentRec.height - bounds.height : 0;
          m_panelScroll.y = max(-minV, min((float)0, m_panelScroll.y));
        }
      }

      float DrawPlaylist(Rectangle &view, vector<Song> &songs){        
        int i = 0;
        int pad = 10;
        float total = 0;
        // temp set font smaller for playlist labels
        int defFontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
        GuiSetStyle(DEFAULT, TEXT_SIZE, 15);
        for(auto &s : songs) {
          // if(currentSong == s){
          //   DrawRectangleLinesEx({view.x, view.y + i*50, view.width, 50}, 1, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_FOCUSED)));  
          // }
          int y = view.y + m_panelScroll.y + i*50;
          int x = view.x + m_panelScroll.x;
          Color c = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL));
          int thick = 1;
          if(s.Filepath == m_currentSong.Filepath || (i == 0 && m_currentSong.Filepath.empty())){
            c = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
            thick = 2;
          }
          DrawRectangleLinesEx({x, y, view.width, 50}, thick, c);
          // song name label
          // todo: make a btn that can be clicked to replace current song
          GuiLabel({ x + pad, y, view.width - pad*2, 50}, s.Name.c_str());
          //DrawText(s.Name.c_str(), view.x + pad, y + 25, 16,  GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
          // remove song btn
          if(GuiButton({ x + view.width - 30 - pad, y + 25 - 15, 30, 30 }, GuiIconText(RICON_CROSS_SMALL, ""))){
            RemoveSongByIndexEvent e = { i };
            EventEmitter::Emit<RemoveSongByIndexEvent>(e);
          }
          total += 50;
          ++i;
        }
        GuiSetStyle(DEFAULT, TEXT_SIZE, defFontSize); // reset font to default size
      }

    protected:
      bool m_isActive = true;
      bool m_showPlaylist = false;
      int m_zIndex = DEFAULT_ZINDEX;
      Rectangle m_dimensions;

      // viz options state
      bool m_isVizAutoPlaying = true;
      bool m_showVizOptions = false;
      string m_selectedViz[2] = { "linewaves.fs", "line waves"}; // temp need to set on init or somethin

      // playlist panel state
      Vector2 m_panelScroll = { 0, 0 };
      Rectangle m_panelContentRec;

      Song m_currentSong;

      void OnPlaySong(PlaySongEvent e){
        m_currentSong = e.Song;
      }

      virtual AudioPlayerLayer* clone_impl() const override { return new AudioPlayerLayer(*this); }
    private:

  };
}
#endif // _SHADEPLAYER_INCLUDE_AUDIOPLAYERLAYER_HPP_