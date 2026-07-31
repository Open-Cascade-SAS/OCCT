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
// This file has been automatically generated from resource file resources/Shaders/PBRGeometry.glsl

static const char Shaders_PBRGeometry_glsl[] =
  "//! Calculates geometry factor for Cook-Torrance BRDF.\n"
  "float occPBRGeometry (in float theCosV,\n"
  "                      in float theCosL,\n"
  "                      in float theRoughness)\n"
  "{\n"
  "  float k = theRoughness + 1.0;\n"
  "  k *= 0.125 * k;\n"
  "  float g1 = 1.0;\n"
  "  g1 /= theCosV * (1.0 - k) + k;\n"
  "  float g2 = 1.0;\n"
  "  g2 /= theCosL * (1.0 - k) + k;\n"
  "  return g1 * g2;\n"
  "}\n";

// clang-format on
