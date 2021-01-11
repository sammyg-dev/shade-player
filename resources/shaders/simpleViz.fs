#version 330

uniform vec2 iResolution;
uniform float iTime;
uniform int avxFreqBin[64];
uniform int avxNoteBin[256];
uniform float avxRMS;
uniform float avxPeak;

out vec4 fragColor;

// use this to test shader params are updating correctly from app
// todo: better test visualizations
void main( void )
{
  // create pixel coordinates
	vec2 uv = gl_FragCoord.xy / iResolution.xy;

  int index = int(uv.x*64);
  int index2 = int(uv.x*256);

  int freqMag = avxFreqBin[index];
  int noteMag = avxNoteBin[index2];

	// draw spectrum bars
  vec3 col = vec3(0.,0.,0.);

  // get in range [0, 1]
  float h1 = freqMag / avxPeak;
  float h2 = noteMag / avxPeak;

  float ch1 = h1 - uv.y;
  float ch2 = h2 - uv.y;

  col = vec3(ch1, 0., ch2);
	
	// output final color
	fragColor = vec4(col,1.0);
}