// Created on: 2013-10-10
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

varying vec3 View;     //!< Direction to the viewer
varying vec3 Normal;   //!< Vertex normal   in view space
varying vec4 Position; //!< Vertex position in view space

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
  Position = occWorldViewMatrix * occModelWorldMatrix * occVertex; // position in the view space
  Normal   = TransformNormal (occNormal);                          // normal   in the view space

  // Note: The specified view vector is absolutely correct only for the orthogonal projection.
  // For perspective projection it will be approximate, but it is in good agreement with the OpenGL calculations.
  View = vec3 (0.0, 0.0, 1.0);

  // Do fixed functionality vertex transform
  gl_Position = occProjectionMatrix * occWorldViewMatrix * occModelWorldMatrix * occVertex;
}
