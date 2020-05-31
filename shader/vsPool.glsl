#version 330
layout( location = 0 ) in vec3 vertex_coord;
layout( location = 1 ) in vec2 texture_coord;
layout( location = 2 ) in vec3 normal;

out vec3 fragmentColor;
out vec2 tex_coord;
out float gl_ClipDistance[2];

uniform mat4 model_model, view_model, projection_model;
uniform vec3 lightColor, lightPosition;
uniform vec3 diffuseColor, ambientColor, specularColor;
uniform float lightPower;
uniform vec4 clipPlane0, clipPlane1;

void main(){
    //projection_model plane
    gl_Position = projection_model * view_model * model_model * vec4( vertex_coord, 1.0 );
    gl_ClipDistance[0] = -dot(model_model * vec4(vertex_coord, 1.0), clipPlane0);
    gl_ClipDistance[1] = dot(model_model * vec4(vertex_coord, 1.0), clipPlane1);

    //view_model space
    vec3 vPosition_viewspace = ( view_model * model_model * vec4( vertex_coord, 1.0 ) ).xyz;

    //transforming normal is different with transforming vertex
    vec3 vNormal_viewspace = (
        transpose( inverse( view_model ) ) * model_model * vec4( normal, 0.0 ) ).xyz;
    vNormal_viewspace = normalize( vNormal_viewspace );

    //point light
    vec3 lightPosition_viewspace = ( view_model * model_model * vec4( lightPosition, 1.0 ) ).xyz;
    vec3 lightDirection_viewspace = lightPosition_viewspace - vPosition_viewspace;
    lightDirection_viewspace = normalize( lightDirection_viewspace );

    //eye direction
    //in view_model space, eye position is (0 0 0)
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
    fragmentColor = diffuseColor * lightColor * lightPower * cosTheta
        / ( distance_viewspace * distance_viewspace );

    //ambientColor
    fragmentColor += ambientColor;

    //specularColor
    //with no distance damping
    fragmentColor += specularColor * lightColor * lightPower * pow( cosAlpha, 5 );

    tex_coord = texture_coord;
}
