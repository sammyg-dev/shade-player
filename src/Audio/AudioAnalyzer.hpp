#ifndef _SHADEPLAYER_INCLUDE_AUDIOANALYZER_HPP_
#define _SHADEPLAYER_INCLUDE_AUDIOANALYZER_HPP_

#include <raylib.h>
#include <kiss_fft.h>
#include <kiss_fftr.h>

#define DEBUG_DRAW true

namespace shade {
  double cheby_poly(int n, double x){
    double res;
    if (fabs(x) <= 1) res = cos(n*acos(x));
    else              res = cosh(n*acosh(x));
    return res;
  }
  void cheby_win(float *out, int N, float atten){
    int nn, i;
    double M, n, sum = 0, max=0;
    double tg = pow(10,atten/20);  /* 1/r term [2], 10^gamma [2] */
    double x0 = cosh((1.0/(N-1))*acosh(tg));
    M = (N-1)/2;
    if(N%2==0) M = M + 0.5; /* handle even length windows */
    for(nn=0; nn<(N/2+1); nn++){
        n = nn-M;
        sum = 0;
        for(i=1; i<=M; i++){
            sum += cheby_poly(N-1,x0*cos(PI*i/N))*cos(2.0*n*PI*i/N);
        }
        out[nn] = tg + 2*sum;
        out[N-nn-1] = out[nn];
        if(out[nn]>max)max=out[nn];
    }
    for(nn=0; nn<N; nn++) out[nn] /= max; /* normalise everything */
    return;
  }
  
  
  
  // used for debug drawing
  struct PlotData {
    int Value;
    int Timestamp;
  };
  /*
   *  AudioAnalyzer provides real-time audio analysis
   *  features:
   *    - .wav supoprt (todo: mp3 + more)
   *    - included debug drawing methods that can be used as examples
   * refs:
   *  - kiss_fft lib by Marc Borgerding (https://github.com/mborgerding/kissfft)
   *  - approach adapted from Prof. Jski (https://github.com/ProfJski/RayLib-Examples/blob/master/Audio1.cpp)
   */
  class AudioAnalyzer {

    public:
      AudioAnalyzer(){}
      ~AudioAnalyzer(){
        if(m_fBuff != nullptr){
          kiss_fftr_free(m_cfg);
          delete [] m_fBuff;
          delete [] m_window;
          delete [] m_freqs;
        }
      }

      void Init(short* audioBuff, int spu, int rawSize){
        float sampleRate = 44100.0; //music.stream.sampleRate;

        float hzPerBand = sampleRate / m_fftSamples;

        printf("%f bands at Hz Per Band=%f",(m_fftSamples/2)+1, hzPerBand);
        m_fBuff = new float[m_fftSamples];
        m_window = new float[spu];

        cheby_win(m_window, m_fftSamples, 240);
        for(int i = 0; i < m_fftSamples; ++i){
          // Hamming window
         // m_window[i]=0.54-0.46*cos(2*PI*i/(m_fftSamples-1));
          // set fft samples
          m_fBuff[i]=(float)audioBuff[i]*m_window[i];
        }

        m_cfg = kiss_fftr_alloc(m_fftSamples, 0, NULL, NULL);
        m_freqs = new kiss_fft_cpx[m_fftSamples/2 + 1];
        //int smoothbins[m_fftSamples/2 + 1] = {0};
      }

      void Update(float deltaTime, short* pAudioBuff, int spu, int cursorPos){
        // update fft buffer
        for(int i = 0; i < m_fftSamples; ++i){
          m_fBuff[i] = (float)pAudioBuff[i] * m_window[i];
        }
        kiss_fftr(m_cfg, m_fBuff, m_freqs);
        if(DEBUG_DRAW) DebugDraw(pAudioBuff, spu, cursorPos);
      }

      void DebugDraw(short* pAudioBuff, int spu, int cursorPos){
        //Draw full FFT and dots
        ++m_frameCounter;
        int smoothbins[m_fftSamples/2 + 1] = {0};
        int barwidth=1600/(m_fftSamples/2)-1;
        PlotData zero = {0, 120};
        PlotData mbars[256] = { zero };
        int mbins[256]={0}; //resets bins to zero
        PlotData dots[m_fftSamples / 2] = { zero };
        int ttl = 30;
        float hzPerBand = 44100.0 / m_fftSamples;

        if (barwidth<1) barwidth=1;
        for (int i=1;i<m_fftSamples/2;i++) {
            float height=10*log((m_freqs[i].r*m_freqs[i].r)/m_fftSamples);  //10* not 20* because no sqrt of magnitude calc
            height=(height<0)?0:height;  //Clamp negative values to zero -- what is significance of negative besides really quiet?
            DrawRectangle(i*(barwidth+1),850-height,barwidth,height,BLUE);
            smoothbins[i]=height;
            if ( (dots[i].Value<smoothbins[i]) || (m_frameCounter-dots[i].Timestamp>ttl) ) {
                dots[i].Value=smoothbins[i];
                dots[i].Timestamp=m_frameCounter;
            }
            DrawLine(i*(barwidth+1),850-dots[i].Value,i*(barwidth+1)+barwidth,850-dots[i].Value,WHITE);
        }

        int rightbin=1, counter=1, oldbin=1;
        for (int i=1;i<m_fftSamples/2+1;i++) {
            rightbin=trunc(12.0*(log(i*hzPerBand)-log(440.0))/log(2.0)+48);  // map frequency of FFT bin to correct consolidated bin, inverse of f(x)=440*2^(i-48)/12 to get note frequencies on scale starting 4 octaves down from A=440Hz
            mbins[rightbin]+=smoothbins[i];
            if (rightbin==oldbin) {
                    counter++;
                    oldbin=rightbin;
            }
            else {  //If we have moved to a new bin, divide the previous one by counter to average it
                mbins[rightbin-1]/=counter;
                counter=1;
                oldbin=rightbin;
            }
        }
        mbins[oldbin]/=counter;  //divide the last bin too
        //Draw consolidated bins and bars
        for (int i=0;i<128;i++) {
            DrawRectangle(i*11,600-mbins[i],10,mbins[i],BLUE);
            if ( (mbars[i].Value<mbins[i]) || (m_frameCounter-mbars[i].Timestamp>ttl) ) {
                mbars[i].Value=mbins[i];
                mbars[i].Timestamp=m_frameCounter;
            }
            DrawLine(i*11,600-mbars[i].Value,i*11+10,600-mbars[i].Value,WHITE);
        }

        //Plot wave data and read line.
        float xratio=spu/1600;  //Change SPU to rawsize/X to plot X part of whole wave...
        for (int i=0;i<spu;i++) {
                DrawPixel(i/xratio, 250+200*pAudioBuff[i]/32000, RED);
        }
        DrawLine(cursorPos/xratio,0,cursorPos/xratio,800,GREEN);
      }
    protected:
    private:
      // fft samples - result after applying window (buff*window)
      float* m_fBuff = nullptr; 
      // window function to apply before doing fft
      float* m_window = nullptr; 
      // fft config
      kiss_fftr_cfg m_cfg;
      // fft samples
      static const int m_fftSamples = 1600;
      // fft frequency spectrum results
      kiss_fft_cpx* m_freqs;

      int m_frameCounter = 0;

  };


}
#endif // _SHADEPLAYER_INCLUDE_AUDIOANALYZER_HPP_