#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform vec2 resolution;  
uniform vec4 buttonColor;

uniform vec4 corners;  
uniform vec2 size;  
uniform vec2 offset;  

void main()
{   
    // vec2 offset  = vec2( 0., 400. );
    // vec4 corners = vec4( 100. );
    // vec2 size    = vec2( 400., 200. );

    vec2 frag = gl_FragCoord.xy;

    // // Rectangle space normalization
    vec2 uv = (frag - offset) / size;
    // vec2 uv = vUV;

    /**
        IF this shaders EVER come to use, it NEEDS
        to be altered.
    */
    // if (frag.x < offset.x || frag.x > offset.x + size.x ||
    //     frag.y < offset.y || frag.y > offset.y + size.y) {
    //     discard;
    // } 


    // Compute pixel aspect ratio (normalized to square space)
    vec2 aspect = vec2(1.0, size.x / size.y);

    float rtl = corners.x;
    float rtr = corners.y;
    float rbl = corners.z;
    float rbr = corners.w;
    
    vec2 tl = vec2(0., 1.);
    vec2 tr = vec2(1., 1.);
    vec2 bl = vec2(0., 0.);
    vec2 br = vec2(1., 0.);

    vec2 ctl = vec2( tl.x + rtl/size.x, tl.y  - rtl/size.y );
    vec2 ctr = vec2( tr.x - rtr/size.x, tr.y  - rtr/size.y );
    vec2 cbl = vec2( bl.x + rbl/size.x, bl.y  + rbl/size.y );
    vec2 cbr = vec2( br.x - rbr/size.x, br.y  + rbr/size.y );

    vec2 uv_iso = vec2(uv.x, uv.y * aspect);
    vec2 ctl_iso = vec2(ctl.x, ctl.y * aspect);
    vec2 ctr_iso = vec2(ctr.x, ctr.y * aspect);
    vec2 cbl_iso = vec2(cbl.x, cbl.y * aspect);
    vec2 cbr_iso = vec2(cbr.x, cbr.y * aspect);

    // usar smoothstep para antialasing
    
    if ( uv.x < ctl.x && uv.y > ctl.y && length( ctl_iso - uv_iso ) > rtl / size.x ) {
        discard;
    } 

    else if ( uv.x > ctr.x && uv.y > ctr.y && length( ctr_iso - uv_iso ) > rtr / size.x ) {
        discard;
    }
    
    else if ( uv.x < cbl.x && uv.y < cbl.y && length( cbl_iso - uv_iso ) > rbl / size.x ) {
        discard;
    } 

    else if ( uv.x > cbr.x && uv.y < cbr.y && length( cbr_iso - uv_iso ) > rbr / size.x ) {
        discard;
    }

    else {
        FragColor = buttonColor;
    }

    return;
}
