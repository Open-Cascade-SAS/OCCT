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
// This file has been automatically generated from resource file resources/Shaders/PBRDistribution.glsl

static const char Shaders_PBRDistribution_glsl[] =
  "//! Calculates micro facet normals distribution.\n"
  "float occPBRDistribution (in float theCosH,\n"
  "                          in float theRoughness)\n"
  "{\n"
  "  float aDistribution = theRoughness * theRoughness;\n"
  "  aDistribution = aDistribution / (theCosH * theCosH * (aDistribution * aDistribution - 1.0) + 1.0);\n"
  "  aDistribution = INV_PI * aDistribution * aDistribution;\n"
  "  return aDistribution;\n"
  "}\n";

// clang-format on
