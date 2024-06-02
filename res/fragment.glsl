#version 430

uniform vec4 in_color; // Vous pouvez mettre le type que vous voulez, et le nom que vous voulez
uniform sampler2D my_texture;
uniform sampler2D skybox;
uniform vec3 light_direction;

out vec4 out_color;
in vec3 position_ws;
in vec2 uv;
in vec3 normal;

#define PI 3.141592

void main()
{
    //out_color = in_color;
    vec4 texture_color = texture(my_texture, uv);
    out_color = (texture_color * (max(dot(normal, light_direction), 0) + 0.3));
    //out_color.rgb = normal;
}