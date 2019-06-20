//! Calculates geometry factor for Cook-Torrance BRDF.
float occPBRGeometry (in float theCosV,
                      in float theCosL,
                      in float theRoughness)
{
  float k = theRoughness + 1.0;
  k *= 0.125 * k;
  float g1 = 1.0;
  g1 /= theCosV * (1.0 - k) + k;
  float g2 = 1.0;
  g2 /= theCosL * (1.0 - k) + k;
  return g1 * g2;
}
