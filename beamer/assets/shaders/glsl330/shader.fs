#version 330

#ifdef GL_ES
precision mediump float;
#endif

in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

void main() {
	vec2 st = gl_FragCoord.xy/u_resolution;
	gl_FragColor = vec4(1-st.x,st.y,0.0,1.0)*texture(texture0, fragTexCoord).a;
}

// #version 330

// // Input vertex attributes (from vertex shader)
// in vec2 fragTexCoord;
// in vec4 fragColor;

// // Input uniform values
// uniform sampler2D texture0;
// uniform vec4 colDiffuse;

// // Output fragment color
// out vec4 finalColor;

// // NOTE: Add here your custom variables

// // NOTE: Render size values must be passed from code
// const float renderWidth = 800;
// const float renderHeight = 450;

// float offset[3] = float[](0.0, 1.0, 2.0);
// float weight[3] = float[](0.2270270270, 0.8, 0.0702702703);

// void main()
// {
//     // Texel color fetching from texture sampler
//     vec3 texelColor = texture(texture0, fragTexCoord).rgb*weight[0];

//     for (int i = 1; i < 3; i++)
//     {
//         texelColor += texture(texture0, fragTexCoord + vec2(offset[i])/renderWidth, 0.0).rgb*weight[i];
//         texelColor += texture(texture0, fragTexCoord - vec2(offset[i])/renderWidth, 0.0).rgb*weight[i];
//     }

//     // Calculate the brightness as the average of RGB channels
//     float brightness = (texelColor.r + texelColor.g + texelColor.b) / 3.0;

//     // Set the final color, preserving the RGB but using the calculated alpha
//     finalColor = vec4(texelColor, brightness);
// }