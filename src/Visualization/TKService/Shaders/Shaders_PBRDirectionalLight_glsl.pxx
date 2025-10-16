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
// This file has been automatically generated from resource file resources/Shaders/PBRDirectionalLight.glsl

static const char Shaders_PBRDirectionalLight_glsl[] =
  "//! Function computes contribution of directional light source\n"
  "//! into global variable DirectLighting (PBR shading).\n"
  "//! @param theId      light source index\n"
  "//! @param theNormal  surface normal\n"
  "//! @param theView    view direction\n"
  "//! @param theIsFront front/back face flag\n"
  "//! @param theShadow  shadow attenuation\n"
  "void occDirectionalLight (in int  theId,\n"
  "                          in vec3 theNormal,\n"
  "                          in vec3 theView,\n"
  "                          in bool theIsFront,\n"
  "                          in float theShadow)\n"
  "{\n"
  "  vec3 aLight = occLight_Position (theId);\n"
  "  theNormal = theIsFront ? theNormal : -theNormal;\n"
  "  DirectLighting += occPBRIllumination (theView, aLight, theNormal,\n"
  "                                        BaseColor, Metallic, Roughness, IOR,\n"
  "                                        occLight_Specular (theId),\n"
  "                                        occLight_Intensity(theId)) * theShadow;\n"
  "}\n";

// clang-format on
