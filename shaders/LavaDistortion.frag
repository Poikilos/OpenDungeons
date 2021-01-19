#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
 
// The MIT License
// Copyright © 2017 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


// One way to avoid texture tile repetition one using one small
// texture to cover a huge area. Basically, it creates 8 different
// offsets for the texture and picks two to interpolate between.
//
// Unlike previous methods that tile space (https://www.shadertoy.com/view/lt2GDd
// or https://www.shadertoy.com/view/4tsGzf), this one uses a random
// low frequency texture (cache friendly) to pick the actual
// texture's offset.
//
// Also, this one mipmaps to something (ugly, but that's better than
// not having mipmaps at all like in previous methods)
//
// More info here: http://www.iquilezles.org/www/articles/texturerepetition/texturerepetition.htm

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
in vec2 out_UV0;
in vec3 FragPos;
uniform float time2;
out vec4 color;



float sum( vec3 v ) { return v.x+v.y+v.z; }

vec3 textureNoTile( in vec2 x, float v )
{
    float k = texture( iChannel1, 0.005*x ).x; // cheap (cache friendly) lookup
    
    vec2 duvdx = dFdx( x );
    vec2 duvdy = dFdy( x );
    
    float l = k*8.0;
    float f = fract(l);
    
#if 1
    float ia = floor(l); // my method
    float ib = ia + 1.0;
#else
    float ia = floor(l+0.5); // suslik's method (see comments)
    float ib = floor(l);
    f = min(f, 1.0-f)*2.0;
#endif    
    
    vec2 offa = sin(vec2(3.0,7.0)*ia); // can replace with any other hash
    vec2 offb = sin(vec2(3.0,7.0)*ib); // can replace with any other hash

    vec3 cola = textureGrad( iChannel0, x + v*offa, duvdx, duvdy ).xyz;
    vec3 colb = textureGrad( iChannel0, x + v*offb, duvdx, duvdy ).xyz;
    
    return mix( cola, colb, smoothstep(0.2,0.8,f-0.1*sum(cola-colb)) );
}

void main(  )
{
	vec2 uv = FragPos.xy/4.0 ;
	
	float f = smoothstep( -1.0, 1.0, sin(time2/50.0)    );

        
	vec3 col = textureNoTile( (uv), f );
	
	color = vec4( col, 1.0 );
} 
 
