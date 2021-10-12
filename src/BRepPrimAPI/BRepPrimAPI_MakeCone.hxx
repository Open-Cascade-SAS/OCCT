// Created on: 1993-07-21
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _BRepPrimAPI_MakeCone_HeaderFile
#define _BRepPrimAPI_MakeCone_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepPrim_Cone.hxx>
#include <BRepPrimAPI_MakeOneAxis.hxx>
#include <Standard_Real.hxx>
#include <Standard_Address.hxx>
class gp_Ax2;


//! Describes functions to build cones or portions of cones.
//! A MakeCone object provides a framework for:
//! -   defining the construction of a cone,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class BRepPrimAPI_MakeCone  : public BRepPrimAPI_MakeOneAxis
{
public:

  DEFINE_STANDARD_ALLOC


  //! Make a cone.
  //! @param R1 [in] cone bottom radius, may be null (z = 0)
  //! @param R2 [in] cone top radius, may be null (z = H)
  //! @param H  [in] cone height
  Standard_EXPORT BRepPrimAPI_MakeCone(const Standard_Real R1, const Standard_Real R2, const Standard_Real H);

  //! Make a cone.
  //! @param R1    [in] cone bottom radius, may be null (z = 0)
  //! @param R2    [in] cone top radius, may be null (z = H)
  //! @param H     [in] cone height
  //! @param angle [in] angle to create a part cone
  Standard_EXPORT BRepPrimAPI_MakeCone(const Standard_Real R1, const Standard_Real R2, const Standard_Real H, const Standard_Real angle);

  //! Make a cone.
  //! @param axes [in] coordinate system for the construction of the cone
  //! @param R1   [in] cone bottom radius, may be null (z = 0)
  //! @param R2   [in] cone top radius, may be null (z = H)
  //! @param H    [in] cone height
  Standard_EXPORT BRepPrimAPI_MakeCone(const gp_Ax2& Axes, const Standard_Real R1, const Standard_Real R2, const Standard_Real H);
  
  //! Make a cone of height H radius R1 in the plane z =
  //! 0, R2 in the plane Z = H. R1 and R2 may be null.
  //! Take a section of <angle>
  //! Constructs a cone, or a portion of a cone, of height H,
  //! and radius R1 in the plane z = 0 and R2 in the plane
  //! z = H. The result is a sharp cone if R1 or R2 is equal to 0.
  //! The cone is constructed about the "Z Axis" of either:
  //! -   the global coordinate system, or
  //! -   the local coordinate system Axes.
  //! It is limited in these coordinate systems as follows:
  //! -   in the v parametric direction (the Z coordinate), by
  //! the two parameter values 0 and H,
  //! -   and in the u parametric direction (defined by the
  //! angle of rotation around the Z axis), in the case of a
  //! portion of a cone, by the two parameter values 0 and
  //! angle. Angle is given in radians.
  //! The resulting shape is composed of:
  //! -   a lateral conical face
  //! -   two planar faces in the planes z = 0 and z = H,
  //! or only one planar face in one of these two planes if a
  //! radius value is null (in the case of a complete cone,
  //! these faces are circles), and
  //! -   and in the case of a portion of a cone, two planar
  //! faces to close the shape. (either two parallelograms or
  //! two triangles, in the planes u = 0 and u = angle).
  //! Exceptions
  //! Standard_DomainError if:
  //! -   H is less than or equal to Precision::Confusion(), or
  //! -   the half-angle at the apex of the cone, defined by
  //! R1, R2 and H, is less than Precision::Confusion()/H, or greater than
  //! (Pi/2)-Precision::Confusion()/H.f
  Standard_EXPORT BRepPrimAPI_MakeCone(const gp_Ax2& Axes, const Standard_Real R1, const Standard_Real R2, const Standard_Real H, const Standard_Real angle);
  
  //! Returns the algorithm.
  Standard_EXPORT Standard_Address OneAxis();
  
  //! Returns the algorithm.
  Standard_EXPORT BRepPrim_Cone& Cone();




protected:





private:



  BRepPrim_Cone myCone;


};







#endif // _BRepPrimAPI_MakeCone_HeaderFile
