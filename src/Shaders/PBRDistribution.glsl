//! Calculates micro facet normals distribution.
float occPBRDistribution (in float theCosH,
                          in float theRoughness)
{
  float aDistribution = theRoughness * theRoughness;
  aDistribution = aDistribution / (theCosH * theCosH * (aDistribution * aDistribution - 1.0) + 1.0);
  aDistribution = INV_PI * aDistribution * aDistribution;
  return aDistribution;
}
