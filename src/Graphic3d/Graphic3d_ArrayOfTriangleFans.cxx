// Created on: 2001-01-04
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <Graphic3d_ArrayOfTriangleFans.ixx>

Graphic3d_ArrayOfTriangleFans :: Graphic3d_ArrayOfTriangleFans (
                        const Standard_Integer maxVertexs,
                        const Standard_Integer maxFans,
                        const Standard_Boolean hasVNormals,
                        const Standard_Boolean hasVColors,
                        const Standard_Boolean hasFColors,
                        const Standard_Boolean hasVTexels)
	: Graphic3d_ArrayOfPrimitives(Graphic3d_TOPA_TRIANGLEFANS,maxVertexs,maxFans,0,hasVNormals,hasVColors,hasFColors,hasVTexels,Standard_False) {}
