#version 430

uniform float aspect_ratio;
uniform vec2 displacement;
uniform mat4 model_view_projection;
uniform mat4 model_matrix;
uniform mat4 normal_matrix;

layout(location = 0) in vec3 in_position_os;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal_os;

out vec2 uv;
out vec3 normal;
out vec3 position_ws;

vec3 apply_matrix_to_position(mat4 matrix, vec3 point)
{
    vec4 tmp = matrix * vec4(point, 1.);
    return tmp.xyz / tmp.w;
}

vec3 apply_matrix_to_direction(mat4 matrix, vec3 direction)
{
    vec4 tmp = matrix * vec4(direction, 0.);
    return normalize(tmp.xyz);
}

void main()
{
    //gl_Position = vec4(in_position, 1.);

    //vec3 position = in_position;
    //vec3 position = in_position + vec3(displacement, O.);
    //gl_Position = vec4(position.x / aspect_ratio, position.y / aspect_ratio, position.z, 1.);

    uv = in_uv;
    normal = apply_matrix_to_direction(normal_matrix, in_normal_os);

    position_ws = apply_matrix_to_position(model_matrix, in_position_os);
    gl_Position = model_view_projection * vec4(in_position_os, 1.);
}