// Created on: 2016-02-04
// Created by: Anastasia BORISOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _Prs3d_ToolQuadric_HeaderFile
#define _Prs3d_ToolQuadric_HeaderFile

#include <gp_Ax1.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Poly_Triangulation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_Drawer.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard.hxx>

//! Base class to build 3D surfaces presentation of quadric surfaces.
class Prs3d_ToolQuadric
{
public:

  DEFINE_STANDARD_ALLOC

  //! Generate primitives for 3D quadric surface and fill the given array. Optional transformation is applied.
  Standard_EXPORT void FillArray (Handle(Graphic3d_ArrayOfTriangles)& theArray, const gp_Trsf& theTrsf);

  //! Generate primitives for 3D quadric surface presentation and fill the given array and poly triangulation structure. Optional transformation is applied.
  Standard_EXPORT void FillArray (Handle(Graphic3d_ArrayOfTriangles)& theArray, Handle(Poly_Triangulation)& theTriangulation, const gp_Trsf& theTrsf);

  //! Number of triangles for presentation with the given params.
  static Standard_Integer TrianglesNb (const Standard_Integer theSlicesNb,
                                       const Standard_Integer theStacksNb)
  {
    return theSlicesNb * theStacksNb * 2;
  }

protected:

  //! Method implements an algorithm to generate arrays of vertices and normals for 3D surface.
  Standard_EXPORT void fillArrays (const gp_Trsf& theTrsf, TColgp_Array1OfPnt& theArray, NCollection_Array1<gp_Dir>& theNormals);

  //! Number of triangles in generated presentation.
  Standard_Integer TrianglesNb() const
  {
    return mySlicesNb * myStacksNb * 2;
  }

  //! Redefine this method to generate vertex at given parameters.
  virtual gp_Pnt Vertex (const Standard_Real theU, const Standard_Real theV) = 0;

  //! Redefine this method to generate normal at given parameters.
  virtual gp_Dir Normal (const Standard_Real theU, const Standard_Real theV) = 0;

protected:

  Standard_Integer mySlicesNb;
  Standard_Integer myStacksNb;
};

#endif // _Prs3d_ToolQuadric_HeaderFile
