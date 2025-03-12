in vec4 occVertex;

//! Normalized pixel coordinates.
out vec2 vPixel;

void main (void)
{
  vPixel = vec2 ((occVertex.x + 1.f) * 0.5f,
                 (occVertex.y + 1.f) * 0.5f);

  gl_Position = occVertex;
}
