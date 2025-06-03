#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform vec2 texelSize;
uniform int subIter;    // 1 = first sub-iteration, 
                        // 2 = second sub-iteration

// threshold, but normally we'll 
// receive either 0.0 or 1.0.
bool v( vec4 px ) {
    return px.r > .5;
}

int connectivity(vec4 px, in vec4[8] nghs) {
    int a = 0;
    for (int i = 0; i < 8; i++) {

        bool curr = v(nghs[i]);
        bool next = v(nghs[(i + 1) % 8]);

        if ( curr && !next ) {
            a++;
        }
    }
    return a;
}
bool edge( vec4 px, in vec4[8] ngh) {
    if (!v(px)) return false;

    if ( connectivity ( px, ngh ) != 1 ) {
        return false;
    }

    int cont = 0;
    for ( int i = 0 ; i < 8 ; i++) {
        if ( ! v(ngh[i]) ) {
            cont++;
        }
    }
    return cont >= 2 && cont <= 6;
}

// Zhang–Suen triple‐pixel conditions for sub‐iteration 1
bool z_mask1_2(vec4 C, vec4 N, vec4 S, vec4 L, vec4 O)
{
    // ensure you only ever call this when v(C) is true
    return 
        !( v(N) && v(L) && v(S) )  &&   // not P2*P4*P6
        !( v(L) && v(S) && v(O) );     // not P4*P6*P8
}

// Zhang–Suen triple‐pixel conditions for sub‐iteration 2
bool z_mask3_4(vec4 C, vec4 N, vec4 S, vec4 L, vec4 O)
{
    return
        !( v(N) && v(L) && v(O) ) &&   // not P2*P4*P8
        !( v(N) && v(S) && v(O) );     // not P2*P6*P8
}



bool expr1 ( vec4 C, vec4 N, vec4 S, vec4 L, vec4 O, in vec4[8] nghs ) {
    return v(C) && ( !edge(C, nghs) || ( v(L) && v(S) && ( v(N) || v(O) ) ) );
}

bool expr2 ( vec4 C, vec4 N, vec4 S, vec4 L, vec4 O, in vec4[8] nghs  ) {
    return v(C) && ( !edge(C, nghs) || ( v(O) && v(N) && ( v(S) || v(L) ) ) );
}

bool holt_1( vec4 C, vec4 N, vec4 S, vec4 L, vec4 O, vec4 NE, vec4 NO, vec4 SE, vec4 SO, in vec4[8] nghs ) {
    return v(C) && (!edge(C, nghs) 
        || (edge(L, nghs) && v(N) && v(S)) 
        || (edge(S, nghs) && v(O) && v(L)) 
        || (edge(L, nghs) && edge(SE, nghs) && edge(S, nghs)));
}

bool mask1_2( vec4 C, vec4 N, vec4 S, vec4 L, vec4 O, vec4 NE, vec4 NO, vec4 SE, vec4 SO  ) {
    return v(C) && !( 
            v(N) && ( 
                (
                    v(L) 
                    && !v(NE)
                    && !v(SO) 
                    && ( !v(O) || !v(S)) 
                    || (
                        v(O) 
                        && !v(NO) 
                        && !v(SE) 
                        && (!v(L) && !v(S))
                    )
            )
        )
    );
}

bool mask3_4( vec4 C, vec4 N, vec4 S, vec4 L, vec4 O, vec4 NE, vec4 NO, vec4 SE, vec4 SO ) {
    return v(C) && !(
        v(S) && (
            ( 
                v(L)
                && !v(NE)
                && !v(SO)
                && (!v(O) || !v(N)) 
                || (
                    v(O)
                    && !v(NO)
                    && !v(SE) 
                    && (!v(L) && !v(N))
                )
            )
        )
    );
}



void main () {

    vec2 uv = gl_FragCoord.xy * texelSize;

    // manter essa ordem por causada função edge
    vec2[8] nghs = vec2[8](
        vec2(-1., 1.0),  
        vec2( 0., 1.0),  
        vec2( 1., 1.0),  

        vec2(-1.0, 0.0),   
        vec2( 1.0, 0.0),   

        vec2(-1., -1.),  
        vec2( 0., -1.),  
        vec2( 1., -1.)
    );
        
    vec4[8] neighbours;

    for (int i = 0; i < 8; i++) {
        vec2 offset = nghs[i] * texelSize;
        neighbours[i] = texture(ourTexture, uv + offset);
    }

    vec4 C  = texture( ourTexture, uv );
    vec4 NO = neighbours[0];
    vec4 N  = neighbours[1];
    vec4 NE = neighbours[2];
    vec4 O  = neighbours[3];
    vec4 L  = neighbours[4];
    vec4 SO = neighbours[5];
    vec4 S  = neighbours[6];
    vec4 SE = neighbours[7];

    // if (subIter == 1) {
    //     if ( v(C)
    //     && edge(C, neighbours)
    //     && z_mask1_2(C, N, S, L, O) )
    //     {
    //         discard;  // delete
    //         return;
    //     }
    // }

    // if (subIter == 2) {
    //     if ( v(C)
    //     && edge(C, neighbours)
    //     && z_mask3_4(C, N, S, L, O) )
    //     {
    //         discard;  // delete
    //         return;
    //     }
    // }

    // FragColor = C;  // keep
    // return;
    if (subIter== 2) { 
        FragColor = C; 
        return; 
    }

    // if ( !expr1( C, N, S, L, O, neighbours ) && ! expr2( C, N, S, L, O, neighbours ))
    if ( ! holt_1(  C, N, S, L, O, NE, NO, SE, SO, neighbours ) )
    {
        discard;
        return;
    }

    // if ( subIter == 1 && ! expr1( C, N, S, L, O, neighbours ) ) {
    //     // FragColor = vec4(0., 0., 1., 1.);
    //     discard;
    // }

    // if ( subIter == 2 && ! expr2( C, N, S, L, O, neighbours) ) {
    //     // FragColor = vec4( 0., 1., 0. , 1.);
    //     discard;
    // }

    // if ( subIter == 1 && ! mask1_2( C, N, S, L, O, NE, NO, SE, SO ) ) {
    //     discard;
    //     return;
    // }
    // if( subIter == 2 && ! mask3_4( C, N, S, L, O, NE, NO, SE, SO ) ) {
    //     discard;
    //     return;
    // }
    FragColor = texture( ourTexture, uv );
    // FragColor = vec4(1., 0., 0., 1.);
    // FragColor = vec4(vec3(0.), 1.);
    return;
}
