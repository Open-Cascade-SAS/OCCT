out vec4 OutColor;

// =======================================================================
// function : main
// purpose  :
// =======================================================================
void main (void)
{
  SRay aRay = GenerateRay (vPixel);
  
  vec3 aInvDirect = 1.f / max (abs (aRay.Direct), SMALL);
  
  aInvDirect = vec3 (aRay.Direct.x < 0.f ? -aInvDirect.x : aInvDirect.x,
                     aRay.Direct.y < 0.f ? -aInvDirect.y : aInvDirect.y,
                     aRay.Direct.z < 0.f ? -aInvDirect.z : aInvDirect.z);

  OutColor = clamp (Radiance (aRay, aInvDirect), 0.f, 1.f);
}