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
// This file has been automatically generated from resource file resources/Shaders/PBRIllumination.glsl

static const char Shaders_PBRIllumination_glsl[] =
  "//! Calculates direct illumination using Cook-Torrance BRDF.\n"
  "vec3 occPBRIllumination (in vec3  theView,\n"
  "                         in vec3  theLight,\n"
  "                         in vec3  theNormal,\n"
  "                         in vec4  theBaseColor,\n"
  "                         in float theMetallic,\n"
  "                         in float theRoughness,\n"
  "                         in float theIOR,\n"
  "                         in vec3  theLightColor,\n"
  "                         in float theLightIntensity)\n"
  "{\n"
  "  vec3 aHalf = normalize (theView + theLight);\n"
  "  float aCosVH = max(dot(theView, aHalf), 0.0);\n"
  "  vec3 aFresnel = occPBRFresnel (theBaseColor.rgb, theMetallic, theIOR, aCosVH);\n"
  "  vec3 aSpecular = occPBRCookTorrance (theView,\n"
  "                                       theLight,\n"
  "                                       theNormal,\n"
  "                                       theBaseColor.rgb,\n"
  "                                       theMetallic,\n"
  "                                       theRoughness,\n"
  "                                       theIOR);\n"
  "  vec3 aDiffuse = vec3(1.0) - aFresnel;\n"
  "  aDiffuse *= 1.0 - theMetallic;\n"
  "  aDiffuse *= INV_PI;\n"
  "  aDiffuse *= theBaseColor.rgb;\n"
  "  aDiffuse = mix (vec3(0.0), aDiffuse, theBaseColor.a);\n"
  "  return (aDiffuse + aSpecular) * theLightColor * theLightIntensity * max(0.0, dot(theLight, theNormal));\n"
  "}\n";

// clang-format on
