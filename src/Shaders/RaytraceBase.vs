in vec4 aPosition;

//! Normalized pixel coordinates.
out vec2 vPixel;

void main (void)
{
  vPixel = vec2 ((aPosition.x + 1.f) * 0.5f,
                 (aPosition.y + 1.f) * 0.5f);
                 
  gl_Position = aPosition;
}