 #version 330
/*
 *  Koch KIFS - Kaleidoscopic Iterated Function System
 *  by: me@sammyg.dev
 */


#define PI 3.1415926538

uniform vec2 iResolution;
uniform vec2 iMouse;
uniform float iTime;
uniform int avxFreqBin[64];
uniform int avxNoteBin[256];
uniform float avxRMS;
uniform float avxPeak;

out vec4 fragColor;

vec2 N(float a) { return vec2(sin(a), cos(a)); }
void main( void )
{
  // create pixel coordinates
	vec2 uv = (gl_FragCoord.xy-.5*iResolution.xy) / iResolution.y;
  vec2 mouse = iMouse.xy / iResolution.xy;
  vec3 col = vec3(0);

  //uv *= 1.25;
  uv *= 3.*(6. - mod(iTime, 6.));

  uv.x = abs(uv.x);
  uv.y += tan(5./6.*PI)*.5;

  vec2 n = N(5./6.*PI);
  float dist = dot(uv-vec2(.5, 0.), n);
  // reflect to create snowflake
  uv -= n * max(0., dist) * 2.;

  uv.x += .5;
  float scale = 1.;
  //n = N(avxRMS/100. * PI * 2.);
  n = N(avxRMS*.025);
  for(int i = 0; i < 7; ++i){
    uv *= 3.;
    scale *= 3.;
    uv.x -= 1.5;

    uv.x = abs(uv.x);
    uv.x -= .5;
    // reflect to create koch curve
    uv -= n * min(0., dot(uv, n)) * 2.;
  }

  float d = length(uv - vec2(clamp(uv.x, -1., 1.), 0));
  float thickness = 1./iResolution.y;
  //col += vec3(0.,.5,1.) * smoothstep(thickness, 0., abs(d/scale));
  col += vec3(0.96,.5,1.) * smoothstep(thickness, 0., abs(d/scale));
  uv /= scale;
  //col.rgb += texture(texture0, uv*2-iTime*.01).rgb;

	fragColor = vec4(col,1.0);
}