#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform vec2 resolution;  
uniform vec4 buttonColor;

uniform vec4 u_corners;  
uniform vec4 u_border_widths;  

uniform vec4 u_color_top;  
uniform vec4 u_color_right;  
uniform vec4 u_color_bottom;  
uniform vec4 u_color_left;  

uniform vec2 u_size;  
uniform vec2 u_offset;  

// returns signed distance to a rounded rectangle of size `size`,
// corner radius `r`, 
// centered at 0,0, 
// with a pixel‐space position `at`
float sdRoundRect(vec2 at, vec2 size, float r) {
    // move from center to positive quadrant:
    vec2 d = abs(at) - (size * 0.5 - vec2(r));

    // outside the box part:
    float outsideDist = length(max(d, 0.0)) - r;

    // inside the box part (negative inside):
    float insideDist  = min(max(d.x, d.y), 0.0);

    return outsideDist + insideDist;
}

void main()
{   
    vec2 size = u_size;
    vec2 offset = u_offset;

    vec4 corners = u_corners; 
    vec4 borderW = u_border_widths;

    float bwt = borderW.x;
    float bwr = borderW.y;
    float bwb = borderW.z;
    float bwl = borderW.w;

    vec2 frag = gl_FragCoord.xy;
    vec2 uv = (frag - offset) / size;

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

    vec2 aspect = vec2(size.x/size.y, 1.0);

    // warped coords
    vec2 uv_iso  = uv   * aspect;
    vec2 ctl_iso = ctl  * aspect;
    vec2 ctr_iso = ctr  * aspect;
    vec2 cbl_iso = cbl  * aspect;
    vec2 cbr_iso = cbr  * aspect;

    // normalize radii into warped-UV space
    float rtl_uv  = corners.x / size.x;
    float rtr_uv  = corners.y / size.x;
    float rbl_uv  = corners.z / size.x;
    float rbr_uv  = corners.w / size.x;

    float rtl_iso = rtl_uv * aspect.x;
    float rtr_iso = rtr_uv * aspect.x;
    float rbl_iso = rbl_uv * aspect.x;
    float rbr_iso = rbr_uv * aspect.x;

    if ( (bwt + bwr + bwb + bwl) == 0.0 ) {
        if      ( uv.x < ctl.x && uv.y > ctl.y && length( uv_iso - ctl_iso ) > rtl_iso ) {
            discard;
        }
        else if ( uv.x > ctr.x && uv.y > ctr.y && length( uv_iso - ctr_iso ) > rtr_iso ) {
            discard;
        }
    
        else if ( uv.x < cbl.x && uv.y < cbl.y && length( uv_iso - cbl_iso ) > rbl_iso ) {
            discard;
        } 

        else if ( uv.x > cbr.x && uv.y < cbr.y && length( uv_iso - cbr_iso ) > rbr_iso ) {
            discard;
        }

        FragColor = buttonColor;
        return;
    }
    else {
        vec2 extent = size * .5;
        vec2 pix = gl_FragCoord.xy - (offset + extent);

        float ex_y = extent.y;
        float ex_x = extent.x;

        float rx = corners.x;
        float ry = corners.y;
        float rz = corners.z;
        float rw = corners.w;

        // dist outer shape
        float dOuterX = sdRoundRect(pix, size, rx);
        float dOuterY = sdRoundRect(pix, size, ry);
        float dOuterZ = sdRoundRect(pix, size, rz);
        float dOuterW = sdRoundRect(pix, size, rw);

        // dist inner shape (shrunk by borderW, radius reduced too):
        vec2 innerSzTL = size - vec2( bwl * 2.0, bwt * 2.0 ); 
        vec2 innerSzTR = size - vec2( bwr * 2.0, bwt * 2.0 ); 
        vec2 innerSzBR = size - vec2( bwr * 2.0, bwb * 2.0 ); 
        vec2 innerSzBL = size - vec2( bwl * 2.0, bwb * 2.0 ); 

        float innerRX = max(rx - max(bwl, bwt), 0.0);
        float innerRY = max(ry - max(bwr, bwt), 0.0);
        float innerRZ = max(rz - max(bwr, bwb), 0.0);
        float innerRW = max(rw - max(bwl, bwb), 0.0);

        float dInnerX = sdRoundRect(pix, innerSzTL, innerRX);
        float dInnerY = sdRoundRect(pix, innerSzTR, innerRY);
        float dInnerZ = sdRoundRect(pix, innerSzBR, innerRZ);
        float dInnerW = sdRoundRect(pix, innerSzBL, innerRW);

        bool top   = pix.y > (  ex_y - ( borderW.x ) );
        bool right = pix.x > (  ex_x - ( borderW.y ) );
        bool down  = pix.y < ( -ex_y + ( borderW.z ) );
        bool left  = pix.x < ( -ex_x + ( borderW.w ) );

        bool in_topl = pix.y > (  ex_y - rx ) && pix.x < ( -ex_x + rx );
        bool in_topr = pix.y > (  ex_y - ry ) && pix.x > (  ex_x - ry );
        bool in_botr = pix.y < ( -ex_y + rz ) && pix.x > (  ex_x - rz ); 
        bool in_botl = pix.y < ( -ex_y + rw ) && pix.x < ( -ex_x + rw );

        if ( in_topl ) {
            if ( dOuterX > 0.0 || dInnerX < 0.0  ) {
                discard;
            }
            else FragColor = pix.y > (ex_y - rx * .5) ? u_color_top : u_color_left;
        }
        else if ( in_topr ) {
            if ( dOuterY > 0.0 || dInnerY < 0.0  ) {
                discard;
            }
            else FragColor = pix.y > (ex_y - ry * .5) ? u_color_top : u_color_right;
        }
        else if ( in_botr ) {
            if ( dOuterZ > 0.0 || dInnerZ < 0.0  ) {
                discard;
            }
            else FragColor = pix.y < ( -ex_y + rz * .5 ) ? u_color_bottom: u_color_right;
        }
        else if ( in_botl ) {
            if ( dOuterW > 0.0 || dInnerW < 0.0  ) {
                discard;
            }
            else FragColor = pix.y < ( -ex_y + rz * .5 ) ? u_color_bottom: u_color_left;
        }
        // 
        else if ( top ) {
            if ( pix.y < (  ex_y - bwt ) ) discard;
            else FragColor = u_color_top;
        }
        else if ( right ) {
            if ( pix.x < (  ex_x - bwr ) ) discard;
            else FragColor = u_color_right;
        }
        else if ( down ) {
            if ( pix.y > ( -ex_y + bwb ) ) discard;
            else FragColor = u_color_bottom;
        }
        else if ( left ) {
            if ( pix.x > ( -ex_x + bwl ) ) discard;
            else FragColor = u_color_left;
        }
        else {
            // FragColor = vec4(1., 1., 1., .1);
            discard;
        }

        // keep only the band between the two:
        // if ( (dOuterX > 0.0 || dInnerX < 0.0 ) && ( dOuterY > 0.0 || dInnerY < 0.0  ) ) {
        //     discard;  
        // }

        // FragColor = buttonColor;
        // FragColor = vec4(1., 0., 0., 1.);
    }

    return;
}
