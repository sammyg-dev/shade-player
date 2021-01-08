#version 330

uniform vec2 iResolution;
uniform float iTime;
uniform int fftSamples[512];
uniform float rms;

out vec4 fragColor;

void main( void )
{
  // create pixel coordinates
	vec2 uv = gl_FragCoord.xy / iResolution.xy;

  // the sound texture is 512x2
  int tx = int(uv.x*512.0);
    
	// first row is frequency data (48Khz/4 in 512 texels, meaning 23 Hz per texel)
	//float fft  = texelFetch( texture0, ivec2(tx,0), 0 ).x; 
  float fft = fftSamples[tx] / 255.;
  // second row is the sound wave, one texel is one mono sample
  //float wave = texelFetch( texture0, ivec2(tx,1), 0 ).x;
	
	// convert frequency to colors
	//vec3 col = vec3( fft, 4.0*fft*(1.0-fft), 1.0-fft ) * fft;
  vec3 col = vec3(fft, 0.1, 0.1) * rms;
  // add wave form on top	
	//col += 1.0 -  smoothstep( 0.0, 0.15, abs(wave - uv.y) );
	
	// output final color
	fragColor = vec4(col,1.0);
}