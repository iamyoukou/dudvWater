#version 330
layout( location = 0 ) in vec3 vtxCoord;
layout( location = 1 ) in vec2 vtxUv;
layout( location = 2 ) in vec3 normal;

out vec3 fragColor;
out vec2 uv;
// out float gl_ClipDistance[2];

uniform mat4 M, V, P;
uniform vec3 lightColor, lightPosition;
uniform vec3 diffuseColor, ambientColor, specularColor;
uniform float lightPower;
// uniform vec4 clipPlane0, clipPlane1;

void main(){
    //P plane
    gl_Position = P * V * M * vec4( vtxCoord, 1.0 );
    // gl_ClipDistance[0] = -dot(M * vec4(vtxCoord, 1.0), clipPlane0);
    // gl_ClipDistance[1] = dot(M * vec4(vtxCoord, 1.0), clipPlane1);

    //V space
    vec3 vPosition_viewspace = ( V * M * vec4( vtxCoord, 1.0 ) ).xyz;

    //transforming normal is different with transforming vertex
    vec3 vNormal_viewspace = (
        transpose( inverse( V ) ) * M * vec4( normal, 0.0 ) ).xyz;
    vNormal_viewspace = normalize( vNormal_viewspace );

    //point light
    vec3 lightPosition_viewspace = ( V * M * vec4( lightPosition, 1.0 ) ).xyz;
    vec3 lightDirection_viewspace = lightPosition_viewspace - vPosition_viewspace;
    lightDirection_viewspace = normalize( lightDirection_viewspace );

    //eye direction
    //in V space, eye position is (0 0 0)
    vec3 eyeDirection_viewspace = vec3( 0.0 ) - vPosition_viewspace;
    eyeDirection_viewspace = normalize( eyeDirection_viewspace );

    //reflect vector
    vec3 reflectDirection_viewspace = reflect(
            -lightDirection_viewspace, vNormal_viewspace
    );

    //for diffuseColor
    float cosTheta = clamp(
        dot( lightDirection_viewspace, vNormal_viewspace ), 0.0, 1.0
    );
    float distance_viewspace = length(
        lightPosition_viewspace - vPosition_viewspace
    );

    //for specularColor
    float cosAlpha = clamp(
        dot( eyeDirection_viewspace, reflectDirection_viewspace ), 0.0, 1.0
    );

    //diffuseColor
    //with distance damping
    fragColor = diffuseColor * lightColor * lightPower * cosTheta
        / ( distance_viewspace * distance_viewspace );

    //ambientColor
    fragColor += ambientColor;

    //specularColor
    //with no distance damping
    fragColor += specularColor * lightColor * lightPower * pow( cosAlpha, 5 );

    uv = vtxUv;
}
