#version 330
layout( location = 0 ) in vec3 vtxCoord;
layout( location = 1 ) in vec2 vtxUv;
layout( location = 2 ) in vec3 normal;

out vec3 fragColor;
out vec2 uv;
out float gl_ClipDistance[2];

uniform mat4 M, V, P;
uniform vec3 lightColor, lightPos;
uniform vec3 diffuse, ambient, specular;
uniform float lightPower;
uniform vec4 clipPlane0, clipPlane1;

void main(){
    gl_Position = P * V * M * vec4( vtxCoord, 1.0 );

    // use clipping to get reflection and refraction texture
    gl_ClipDistance[0] = -dot(M * vec4(vtxCoord, 1.0), clipPlane0);
    gl_ClipDistance[1] = dot(M * vec4(vtxCoord, 1.0), clipPlane1);

    // *View means view space
    vec3 vtxCoordView = ( V * M * vec4( vtxCoord, 1.0 ) ).xyz;

    //transforming normal is different with transforming vertex
    vec3 normalView = (
        transpose( inverse( V ) ) * M * vec4( normal, 0.0 ) ).xyz;
    normalView = normalize( normalView );

    //point light
    vec3 lightPosView = ( V * M * vec4( lightPos, 1.0 ) ).xyz;
    vec3 lightDirView = lightPosView - vtxCoordView;
    lightDirView = normalize( lightDirView );

    //eye direction
    //in V space, eye position is (0 0 0)
    vec3 eyeDirView = vec3( 0.0 ) - vtxCoordView;
    eyeDirView = normalize( eyeDirView );

    //reflect vector
    vec3 reflectDirView = reflect(
            -lightDirView, normalView
    );

    //for diffuse
    float cosTheta = clamp(
        dot( lightDirView, normalView ), 0.0, 1.0
    );
    float distance_viewspace = length(
        lightPosView - vtxCoordView
    );

    //for specular
    float cosAlpha = clamp(
        dot( eyeDirView, reflectDirView ), 0.0, 1.0
    );

    //diffuse
    //with distance damping
    fragColor = diffuse * lightColor * lightPower * cosTheta
        / ( distance_viewspace * distance_viewspace );

    //ambient
    fragColor += ambient;

    //specular
    //with no distance damping
    fragColor += specular * lightColor * lightPower * pow( cosAlpha, 5 );

    uv = vtxUv;
}
