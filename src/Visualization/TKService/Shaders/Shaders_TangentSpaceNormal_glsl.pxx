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
// This file has been automatically generated from resource file resources/Shaders/TangentSpaceNormal.glsl

static const char Shaders_TangentSpaceNormal_glsl[] =
  "//! Calculates transformation from tangent space and apply it to value from normal map to get normal in object space\n"
  "vec3 TangentSpaceNormal (in mat2 theDeltaUVMatrix,\n"
  "                         in mat2x3 theDeltaVectorMatrix,\n"
  "                         in vec3 theNormalMapValue,\n"
  "                         in vec3 theNormal,\n"
  "                         in bool theIsInverse)\n"
  "{\n"
  "  theNormalMapValue = normalize(theNormalMapValue * 2.0 - vec3(1.0));\n"
  "  // Inverse matrix\n"
  "  theDeltaUVMatrix = mat2 (theDeltaUVMatrix[1][1], -theDeltaUVMatrix[0][1], -theDeltaUVMatrix[1][0], theDeltaUVMatrix[0][0]);\n"
  "  theDeltaVectorMatrix = theDeltaVectorMatrix * theDeltaUVMatrix;\n"
  "  // Gram-Schmidt orthogonalization\n"
  "  theDeltaVectorMatrix[1] = normalize(theDeltaVectorMatrix[1] - dot(theNormal, theDeltaVectorMatrix[1]) * theNormal);\n"
  "  theDeltaVectorMatrix[0] = cross(theDeltaVectorMatrix[1], theNormal);\n"
  "  float aDirection = theIsInverse ? -1.0 : 1.0;\n"
  "  return mat3 (aDirection * theDeltaVectorMatrix[0], aDirection * theDeltaVectorMatrix[1], theNormal) * theNormalMapValue;\n"
  "}\n";

// clang-format on
