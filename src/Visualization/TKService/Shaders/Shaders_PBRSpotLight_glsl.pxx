// Copyright (c) 2025 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// clang-format off
// This file has been automatically generated from resource file resources/Shaders/PBRSpotLight.glsl

static const char Shaders_PBRSpotLight_glsl[] =
  "//! Function computes contribution of spotlight source\n"
  "//! into global variable DirectLighting (PBR shading).\n"
  "//! @param theId      light source index\n"
  "//! @param theNormal  surface normal\n"
  "//! @param theView    view direction\n"
  "//! @param thePoint   3D position (world space)\n"
  "//! @param theIsFront front/back face flag\n"
  "void occSpotLight (in int  theId,\n"
  "                   in vec3 theNormal,\n"
  "                   in vec3 theView,\n"
  "                   in vec3 thePoint,\n"
  "                   in bool theIsFront,\n"
  "                   in float theShadow)\n"
  "{\n"
  "  vec3 aLight = occLight_Position (theId) - thePoint;\n"
  "\n"
  "  float aDist = length (aLight);\n"
  "  float aRange = occLight_Range (theId);\n"
  "  float anAtten = occPointLightAttenuation (aDist, aRange);\n"
  "  if (anAtten <= 0.0) return;\n"
  "  aLight /= aDist;\n"
  "\n"
  "  vec3 aSpotDir = occLight_SpotDirection (theId);\n"
  "  // light cone\n"
  "  float aCosA = dot (aSpotDir, -aLight);\n"
  "  float aRelativeAngle = 2.0 * acos(aCosA) / occLight_SpotCutOff(theId);\n"
  "  if (aCosA >= 1.0 || aRelativeAngle > 1.0)\n"
  "  {\n"
  "    return;\n"
  "  }\n"
  "  float anExponent = occLight_SpotExponent (theId);\n"
  "  if ((1.0 - aRelativeAngle) <= anExponent)\n"
  "  {\n"
  "    float anAngularAttenuationOffset = cos(0.5 * occLight_SpotCutOff(theId));\n"
  "    float anAngularAttenuationScale = 1.0 / max(0.001, cos(0.5 * occLight_SpotCutOff(theId) * (1.0 - anExponent)) - anAngularAttenuationOffset);\n"
  "    anAngularAttenuationOffset *= -anAngularAttenuationScale;\n"
  "    float anAngularAttenuantion = clamp(aCosA * anAngularAttenuationScale + anAngularAttenuationOffset, 0.0, 1.0);\n"
  "    anAtten *= anAngularAttenuantion * anAngularAttenuantion;\n"
  "  }\n"
  "  theNormal = theIsFront ? theNormal : -theNormal;\n"
  "  DirectLighting += occPBRIllumination (theView, aLight, theNormal,\n"
  "                                        BaseColor, Metallic, Roughness, IOR,\n"
  "                                        occLight_Specular(theId),\n"
  "                                        occLight_Intensity(theId) * anAtten) * theShadow;\n"
  "}\n";

// clang-format on
