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
// This file has been automatically generated from resource file resources/Shaders/PhongPointLight.glsl

static const char Shaders_PhongPointLight_glsl[] =
  "//! Function computes contribution of isotropic point light source\n"
  "//! into global variables Diffuse and Specular (Phong shading).\n"
  "//! @param theId      light source index\n"
  "//! @param theNormal  surface normal\n"
  "//! @param theView    view direction\n"
  "//! @param thePoint   3D position (world space)\n"
  "//! @param theIsFront front/back face flag\n"
  "void occPointLight (in int  theId,\n"
  "                    in vec3 theNormal,\n"
  "                    in vec3 theView,\n"
  "                    in vec3 thePoint,\n"
  "                    in bool theIsFront)\n"
  "{\n"
  "  vec3 aLight = occLight_Position (theId) - thePoint;\n"
  "\n"
  "  float aDist = length (aLight);\n"
  "  float aRange = occLight_Range (theId);\n"
  "  float anAtten = occPointLightAttenuation (aDist, aRange, occLight_LinearAttenuation (theId), occLight_ConstAttenuation (theId));\n"
  "  if (anAtten <= 0.0) return;\n"
  "  aLight /= aDist;\n"
  "\n"
  "  vec3 aHalf = normalize (aLight + theView);\n"
  "\n"
  "  vec3  aFaceSideNormal = theIsFront ? theNormal : -theNormal;\n"
  "  float aNdotL = max (0.0, dot (aFaceSideNormal, aLight));\n"
  "  float aNdotH = max (0.0, dot (aFaceSideNormal, aHalf ));\n"
  "\n"
  "  float aSpecl = 0.0;\n"
  "  if (aNdotL > 0.0)\n"
  "  {\n"
  "    aSpecl = pow (aNdotH, occMaterial_Shininess (theIsFront));\n"
  "  }\n"
  "\n"
  "  Diffuse  += occLight_Diffuse (theId) * aNdotL * anAtten;\n"
  "  Specular += occLight_Specular(theId) * aSpecl * anAtten;\n"
  "}\n";

// clang-format on
