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
// This file has been automatically generated from resource file resources/Shaders/PBREnvBaking.vs

static const char Shaders_PBREnvBaking_vs[] =
  "THE_SHADER_OUT vec3 ViewDirection; //!< direction of fetching from environment cubemap\n"
  "\n"
  "uniform int uCurrentSide; //!< current side of cubemap\n"
  "uniform int uYCoeff;      //!< coefficient of Y controlling horizontal flip of cubemap\n"
  "uniform int uZCoeff;      //!< coefficient of Z controlling vertical flip of cubemap\n"
  "\n"
  "void main()\n"
  "{\n"
  "  vec3 aDir;\n"
  "  vec2 aCoord;\n"
  "  if (uCurrentSide == 0)\n"
  "  {\n"
  "    aCoord = mat2( 0,-1,-1, 0) * occVertex.xy;\n"
  "    aDir.x = 1.0;\n"
  "    aDir.y = aCoord.x;\n"
  "    aDir.z = aCoord.y;\n"
  "  }\n"
  "  else if (uCurrentSide == 1)\n"
  "  {\n"
  "    aCoord = mat2( 0, 1,-1, 0) * occVertex.xy;\n"
  "    aDir.x = -1.0;\n"
  "    aDir.y = aCoord.x;\n"
  "    aDir.z = aCoord.y;\n"
  "  }\n"
  "  else if (uCurrentSide == 2)\n"
  "  {\n"
  "    aCoord = mat2( 0, 1, 1, 0) * occVertex.xy;\n"
  "    aDir.x = aCoord.y;\n"
  "    aDir.y = 1.0;\n"
  "    aDir.z = aCoord.x;\n"
  "  }\n"
  "  else if (uCurrentSide == 3)\n"
  "  {\n"
  "    aCoord = mat2( 0, 1,-1, 0) * occVertex.xy;\n"
  "    aDir.x = aCoord.y;\n"
  "    aDir.y = -1.0;\n"
  "    aDir.z = aCoord.x;\n"
  "  }\n"
  "  else if (uCurrentSide == 4)\n"
  "  {\n"
  "    aCoord = mat2( 1, 0, 0,-1) * occVertex.xy;\n"
  "    aDir.x = aCoord.x;\n"
  "    aDir.y = aCoord.y;\n"
  "    aDir.z = 1.0;\n"
  "  }\n"
  "  else //if (uCurrentSide == 5)\n"
  "  {\n"
  "    aCoord = mat2(-1, 0, 0,-1) * occVertex.xy;\n"
  "    aDir.x = aCoord.x;\n"
  "    aDir.y = aCoord.y;\n"
  "    aDir.z = -1.0;\n"
  "  }\n"
  "  ViewDirection = cubemapVectorTransform (aDir, uYCoeff, uZCoeff);\n"
  "  gl_Position = vec4 (occVertex.xy, 0.0, 1.0);\n"
  "}\n";

// clang-format on
