#version 330 core

out vec4 FragColor;

in vec2 vTexCoord;
flat in int vTexIndex;

uniform sampler2D ourTexture[16];

void main()
{
    FragColor = texture(ourTexture[vTexIndex], vTexCoord);
}
