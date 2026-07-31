//! Returns point light source attenuation factor
float occRangedPointLightAttenuation (in float theDistance, in float theRange)
{
  if (theDistance <= theRange)
  {
    float aResult = theDistance / theRange;
    aResult *= aResult;
    aResult *= aResult;
    aResult = 1.0 - aResult;
    aResult = clamp(aResult, 0.0, 1.0);
    aResult /= max(0.0001, theDistance * theDistance);
    return aResult;
  }
  return -1.0;
}

//! Returns point light source attenuation factor with quadratic attenuation in case of zero range.
float occPointLightAttenuation (in float theDistance, in float theRange)
{
  if (theRange == 0.0)
  {
    return 1.0 / max(0.0001, theDistance * theDistance);
  }
  return occRangedPointLightAttenuation (theDistance, theRange);
}

//! Returns point light source attenuation factor with linear attenuation in case of zero range.
float occPointLightAttenuation (in float theDistance, in float theRange, in float theLinearAttenuation, in float theConstAttenuation)
{
  if (theRange == 0.0)
  {
    return 1.0 / (theConstAttenuation + theLinearAttenuation * theDistance);
  }
  return occRangedPointLightAttenuation (theDistance, theRange);
}
