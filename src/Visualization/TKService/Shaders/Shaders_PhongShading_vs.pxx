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
// This file has been automatically generated from resource file resources/Shaders/PhongShading.vs

static const char Shaders_PhongShading_vs[] =
  "// Created on: 2013-10-10\n"
  "// Created by: Denis BOGOLEPOV\n"
  "// Copyright (c) 2013-2014 OPEN CASCADE SAS\n"
  "//\n"
  "// This file is part of Open CASCADE Technology software library.\n"
  "//\n"
  "// This library is free software; you can redistribute it and/or modify it under\n"
  "// the terms of the GNU Lesser General Public License version 2.1 as published\n"
  "// by the Free Software Foundation, with special exception defined in the file\n"
  "// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT\n"
  "// distribution for complete text of the license and disclaimer of any warranty.\n"
  "//\n"
  "// Alternatively, this file may be used under the terms of Open CASCADE\n"
  "// commercial license or contractual agreement.\n"
  "\n"
  "varying vec3 View;          //!< Direction to the viewer\n"
  "varying vec3 Normal;        //!< Vertex normal in view space\n"
  "varying vec4 Position;      //!< Vertex position in view space\n"
  "varying vec4 PositionWorld; //!< Vertex position in world space\n"
  "\n"
  "//! Computes the normal in view space\n"
  "vec3 TransformNormal (in vec3 theNormal)\n"
  "{\n"
  "  vec4 aResult = occWorldViewMatrixInverseTranspose\n"
  "               * occModelWorldMatrixInverseTranspose\n"
  "               * vec4 (theNormal, 0.0);\n"
  "  return normalize (aResult.xyz);\n"
  "}\n"
  "\n"
  "//! Entry point to the Vertex Shader\n"
  "void main()\n"
  "{\n"
  "  PositionWorld = occModelWorldMatrix * occVertex;\n"
  "  Position      = occWorldViewMatrix * PositionWorld;\n"
  "  Normal        = TransformNormal (occNormal);\n"
  "\n"
  "  // Note: The specified view vector is absolutely correct only for the orthogonal projection.\n"
  "  // For perspective projection it will be approximate, but it is in good agreement with the OpenGL calculations.\n"
  "  View = vec3 (0.0, 0.0, 1.0);\n"
  "\n"
  "  // Do fixed functionality vertex transform\n"
  "  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;\n"
  "}\n";

// clang-format on
