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

#ifndef _BRepPrimAPI_MakeTorus_HeaderFile
#define _BRepPrimAPI_MakeTorus_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepPrim_Torus.hxx>
#include <BRepPrimAPI_MakeOneAxis.hxx>
#include <Standard_Real.hxx>
#include <Standard_Address.hxx>
class gp_Ax2;


//! Describes functions to build tori or portions of tori.
//! A MakeTorus object provides a framework for:
//! -   defining the construction of a torus,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class BRepPrimAPI_MakeTorus  : public BRepPrimAPI_MakeOneAxis
{
public:

  DEFINE_STANDARD_ALLOC


  //! Make a torus.
  //! @param R1 [in] distance from the center of the pipe to the center of the torus
  //! @param R2 [in] radius of the pipe
  Standard_EXPORT BRepPrimAPI_MakeTorus(const Standard_Real R1, const Standard_Real R2);

  //! Make a section of a torus.
  //! @param R1    [in] distance from the center of the pipe to the center of the torus
  //! @param R2    [in] radius of the pipe
  //! @param angle [in] angle to create a torus pipe segment
  Standard_EXPORT BRepPrimAPI_MakeTorus(const Standard_Real R1, const Standard_Real R2, const Standard_Real angle);
  
  //! Make  a torus with angles on the small circle.
  //! @param R1     [in] distance from the center of the pipe to the center of the torus
  //! @param R2     [in] radius of the pipe
  //! @param angle1 [in] first  angle to create a torus ring segment
  //! @param angle2 [in] second angle to create a torus ring segment
  Standard_EXPORT BRepPrimAPI_MakeTorus(const Standard_Real R1, const Standard_Real R2, const Standard_Real angle1, const Standard_Real angle2);
  
  //! Make  a torus with angles on the small circle.
  //! @param R1     [in] distance from the center of the pipe to the center of the torus
  //! @param R2     [in] radius of the pipe
  //! @param angle1 [in] first  angle to create a torus ring segment
  //! @param angle2 [in] second angle to create a torus ring segment
  //! @param angle  [in] angle to create a torus pipe segment
  Standard_EXPORT BRepPrimAPI_MakeTorus(const Standard_Real R1, const Standard_Real R2, const Standard_Real angle1, const Standard_Real angle2, const Standard_Real angle);
  
  //! Make a torus.
  //! @param Axes [in] coordinate system for the construction of the sphere
  //! @param R1   [in] distance from the center of the pipe to the center of the torus
  //! @param R2   [in] radius of the pipe
  Standard_EXPORT BRepPrimAPI_MakeTorus(const gp_Ax2& Axes, const Standard_Real R1, const Standard_Real R2);
  
  //! Make a section of a torus.
  //! @param Axes  [in] coordinate system for the construction of the sphere
  //! @param R1    [in] distance from the center of the pipe to the center of the torus
  //! @param R2    [in] radius of the pipe
  //! @param angle [in] angle to create a torus pipe segment
  Standard_EXPORT BRepPrimAPI_MakeTorus(const gp_Ax2& Axes, const Standard_Real R1, const Standard_Real R2, const Standard_Real angle);
  
  //! Make a torus.
  //! @param Axes   [in] coordinate system for the construction of the sphere
  //! @param R1     [in] distance from the center of the pipe to the center of the torus
  //! @param R2     [in] radius of the pipe
  //! @param angle1 [in] first  angle to create a torus ring segment
  //! @param angle2 [in] second angle to create a torus ring segment
  Standard_EXPORT BRepPrimAPI_MakeTorus(const gp_Ax2& Axes, const Standard_Real R1, const Standard_Real R2, const Standard_Real angle1, const Standard_Real angle2);
  
  //! Make a section of a torus of radii R1 R2.
  //! For all algorithms The resulting shape is composed of
  //! -      a lateral toroidal face,
  //! -      two conical faces (defined  by the equation v = angle1 and
  //! v = angle2) if the sphere is truncated in the v parametric
  //! direction (they may be cylindrical faces in some
  //! particular conditions), and in case of a portion
  //! of torus, two planar faces to close the shape.(in the planes
  //! u = 0 and u = angle).
  //! Notes:
  //! -      The u parameter corresponds to a rotation angle around the Z axis.
  //! -      The circle whose radius is equal to the minor radius,
  //! located in the plane defined by the X axis and the Z axis,
  //! centered on the X axis, on its positive side, and positioned
  //! at a distance from the origin equal to the major radius, is
  //! the reference circle of the torus. The rotation around an
  //! axis parallel to the Y axis and passing through the center
  //! of the reference circle gives the v parameter on the
  //! reference circle. The X axis gives the origin of the v
  //! parameter. Near 0, as v increases, the Z coordinate decreases.
  Standard_EXPORT BRepPrimAPI_MakeTorus(const gp_Ax2& Axes, const Standard_Real R1, const Standard_Real R2, const Standard_Real angle1, const Standard_Real angle2, const Standard_Real angle);
  
  //! Returns the algorithm.
  Standard_EXPORT Standard_Address OneAxis();
  
  //! Returns the algorithm.
  Standard_EXPORT BRepPrim_Torus& Torus();




protected:





private:



  BRepPrim_Torus myTorus;


};







#endif // _BRepPrimAPI_MakeTorus_HeaderFile
