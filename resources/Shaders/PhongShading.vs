// Created on: 2013-10-10
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

varying vec3 View;          //!< Direction to the viewer
varying vec3 Normal;        //!< Vertex normal in view space
varying vec4 Position;      //!< Vertex position in view space
varying vec4 PositionWorld; //!< Vertex position in world space

//! Computes the normal in view space
vec3 TransformNormal (in vec3 theNormal)
{
  vec4 aResult = occWorldViewMatrixInverseTranspose
               * occModelWorldMatrixInverseTranspose
               * vec4 (theNormal, 0.0);
  return normalize (aResult.xyz);
}

//! Entry point to the Vertex Shader
void main()
{
  PositionWorld = occModelWorldMatrix * occVertex;
  Position      = occWorldViewMatrix * PositionWorld;
  Normal        = TransformNormal (occNormal);

  // Note: The specified view vector is absolutely correct only for the orthogonal projection.
  // For perspective projection it will be approximate, but it is in good agreement with the OpenGL calculations.
  View = vec3 (0.0, 0.0, 1.0);

  // Do fixed functionality vertex transform
  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;
}
