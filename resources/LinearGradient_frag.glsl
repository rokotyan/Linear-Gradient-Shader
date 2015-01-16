uniform float windowHeight;

uniform vec2  gradientStartPos;
uniform vec2  gradientEndPos;
uniform int   numStops;
uniform vec4  colors[16];
uniform float stops[16];

void main() {
    
    float  alpha = atan( -gradientEndPos.y + gradientStartPos.y, gradientEndPos.x - gradientStartPos.x );
    float  gradientStartPosRotatedX = gradientStartPos.x*cos(alpha) - gradientStartPos.y*sin(alpha);
    float  gradientEndPosRotatedX   = gradientEndPos.x*cos(alpha) - gradientEndPos.y*sin(alpha);
    float  d = gradientEndPosRotatedX - gradientStartPosRotatedX;
    

    float y = windowHeight - gl_FragCoord.y;
    float x = gl_FragCoord.x;
    float xLocRotated = x*cos( alpha ) - y*sin( alpha );
 
    gl_FragColor = mix(colors[0], colors[1], smoothstep( gradientStartPosRotatedX + stops[0]*d, gradientStartPosRotatedX + stops[1]*d, xLocRotated ) );
    for ( int i=1; i<numStops-1; ++i ) {
        gl_FragColor = mix(gl_FragColor, colors[i+1], smoothstep( gradientStartPosRotatedX + stops[i]*d, gradientStartPosRotatedX + stops[i+1]*d, xLocRotated ) );
    }
 
    
}
