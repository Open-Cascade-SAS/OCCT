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

#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Poly_Triangulation.hxx>

//! Base class to build 3D surfaces presentation of quadric surfaces.
class Prs3d_ToolQuadric
{
public:
  DEFINE_STANDARD_ALLOC

  //! Return number of triangles for presentation with the given params.
  static Standard_Integer TrianglesNb (const Standard_Integer theSlicesNb,
                                       const Standard_Integer theStacksNb)
  {
    return theSlicesNb * theStacksNb * 2;
  }

  //! Return number of vertices for presentation with the given params.
  static Standard_Integer VerticesNb (const Standard_Integer theSlicesNb,
                                      const Standard_Integer theStacksNb,
                                      const Standard_Boolean theIsIndexed = Standard_True)
  {
    return theIsIndexed
      ? (theSlicesNb + 1) * (theStacksNb + 1)
      : TrianglesNb (theSlicesNb, theStacksNb) * 3;
  }

public:

  //! Generate primitives for 3D quadric surface presentation.
  //! @param theTrsf [in] optional transformation to apply
  //! @return generated triangulation
  Standard_EXPORT Handle(Graphic3d_ArrayOfTriangles) CreateTriangulation (const gp_Trsf& theTrsf) const;

  //! Generate primitives for 3D quadric surface presentation.
  //! @param theTrsf [in] optional transformation to apply
  //! @return generated triangulation
  Standard_EXPORT Handle(Poly_Triangulation) CreatePolyTriangulation (const gp_Trsf& theTrsf) const;

  //! Generate primitives for 3D quadric surface and fill the given array.
  //! @param theArray [in][out] the array of vertices;
  //!                           when NULL, function will create an indexed array;
  //!                           when not NULL, triangles will be appended to the end of array
  //!                           (will raise an exception if reserved array size is not large enough)
  //! @param theTrsf [in] optional transformation to apply
  Standard_EXPORT void FillArray (Handle(Graphic3d_ArrayOfTriangles)& theArray,
                                  const gp_Trsf& theTrsf) const;

  //! Return number of triangles in generated presentation.
  Standard_Integer TrianglesNb() const { return mySlicesNb * myStacksNb * 2; }

  //! Return number of vertices in generated presentation.
  Standard_Integer VerticesNb (bool theIsIndexed = true) const
  {
    return theIsIndexed
         ? (mySlicesNb + 1) * (myStacksNb + 1)
         : TrianglesNb() * 3;
  }

public:

  //! Generate primitives for 3D quadric surface presentation.
  //! @param theArray [out] generated array of triangles
  //! @param theTriangulation [out] generated triangulation
  //! @param theTrsf [in] optional transformation to apply
  Standard_DEPRECATED("Deprecated method, CreateTriangulation() and CreatePolyTriangulation() should be used instead")
  Standard_EXPORT void FillArray (Handle(Graphic3d_ArrayOfTriangles)& theArray,
                                  Handle(Poly_Triangulation)& theTriangulation,
                                  const gp_Trsf& theTrsf) const;

protected:

  //! Redefine this method to generate vertex at given parameters.
  virtual gp_Pnt Vertex (const Standard_Real theU, const Standard_Real theV) const = 0;

  //! Redefine this method to generate normal at given parameters.
  virtual gp_Dir Normal (const Standard_Real theU, const Standard_Real theV) const = 0;

protected:

  Standard_Integer mySlicesNb; //!< number of slices within U parameter
  Standard_Integer myStacksNb; //!< number of stacks within V parameter
};

#endif // _Prs3d_ToolQuadric_HeaderFile
