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
// This file has been automatically generated from resource file resources/Shaders/PBRCookTorrance.glsl

static const char Shaders_PBRCookTorrance_glsl[] =
  "//! Calculates Cook-Torrance BRDF.\n"
  "vec3 occPBRCookTorrance (in vec3  theView,\n"
  "                         in vec3  theLight,\n"
  "                         in vec3  theNormal,\n"
  "                         in vec3  theBaseColor,\n"
  "                         in float theMetallic,\n"
  "                         in float theRoughness,\n"
  "                         in float theIOR)\n"
  "{\n"
  "  vec3 aHalf = normalize (theView + theLight);\n"
  "  float aCosV = max(dot(theView, theNormal), 0.0);\n"
  "  float aCosL = max(dot(theLight, theNormal), 0.0);\n"
  "  float aCosH = max(dot(aHalf, theNormal), 0.0);\n"
  "  float aCosVH = max(dot(aHalf, theView), 0.0);\n"
  "  vec3 aCookTorrance = occPBRDistribution (aCosH, theRoughness)\n"
  "                     * occPBRGeometry     (aCosV, aCosL, theRoughness)\n"
  "                     * occPBRFresnel      (theBaseColor, theMetallic, theIOR, aCosVH);\n"
  "  aCookTorrance /= 4.0;\n"
  "  return aCookTorrance;\n"
  "}\n";

// clang-format on
