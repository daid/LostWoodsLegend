[VERTEX]
attribute vec3 a_vertex;
attribute vec3 a_normal;
attribute vec2 a_uv;

uniform mat4 projection_matrix;
uniform mat4 camera_matrix;
uniform mat4 object_matrix;
uniform vec3 object_scale;

varying vec2 v_uv;

void main()
{
    gl_Position = projection_matrix * camera_matrix * object_matrix * vec4(a_vertex.xyz * object_scale, 1.0);
	gl_Position.z += object_matrix[3].y * 0.01 - 0.5;
    v_uv = a_uv.xy;
}

[FRAGMENT]
uniform sampler2D texture_map;
uniform vec4 color;

varying vec2 v_uv;

void main()
{
    gl_FragColor = texture2D(texture_map, v_uv) * color;
    if (gl_FragColor.a == 0.0)
        discard;
}
