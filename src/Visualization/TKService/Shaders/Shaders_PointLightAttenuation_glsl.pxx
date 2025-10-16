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
// This file has been automatically generated from resource file resources/Shaders/PointLightAttenuation.glsl

static const char Shaders_PointLightAttenuation_glsl[] =
  "//! Returns point light source attenuation factor\n"
  "float occRangedPointLightAttenuation (in float theDistance, in float theRange)\n"
  "{\n"
  "  if (theDistance <= theRange)\n"
  "  {\n"
  "    float aResult = theDistance / theRange;\n"
  "    aResult *= aResult;\n"
  "    aResult *= aResult;\n"
  "    aResult = 1.0 - aResult;\n"
  "    aResult = clamp(aResult, 0.0, 1.0);\n"
  "    aResult /= max(0.0001, theDistance * theDistance);\n"
  "    return aResult;\n"
  "  }\n"
  "  return -1.0;\n"
  "}\n"
  "\n"
  "//! Returns point light source attenuation factor with quadratic attenuation in case of zero range.\n"
  "float occPointLightAttenuation (in float theDistance, in float theRange)\n"
  "{\n"
  "  if (theRange == 0.0)\n"
  "  {\n"
  "    return 1.0 / max(0.0001, theDistance * theDistance);\n"
  "  }\n"
  "  return occRangedPointLightAttenuation (theDistance, theRange);\n"
  "}\n"
  "\n"
  "//! Returns point light source attenuation factor with linear attenuation in case of zero range.\n"
  "float occPointLightAttenuation (in float theDistance, in float theRange, in float theLinearAttenuation, in float theConstAttenuation)\n"
  "{\n"
  "  if (theRange == 0.0)\n"
  "  {\n"
  "    return 1.0 / (theConstAttenuation + theLinearAttenuation * theDistance);\n"
  "  }\n"
  "  return occRangedPointLightAttenuation (theDistance, theRange);\n"
  "}\n";

// clang-format on
