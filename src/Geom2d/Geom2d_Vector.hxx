// Created on: 1993-03-24
// Created by: JCV
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

#ifndef _Geom2d_Vector_HeaderFile
#define _Geom2d_Vector_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Vec2d.hxx>
#include <Geom2d_Geometry.hxx>
#include <Standard_Real.hxx>


class Geom2d_Vector;
DEFINE_STANDARD_HANDLE(Geom2d_Vector, Geom2d_Geometry)

//! The abstract class Vector describes the common
//! behavior of vectors in 2D space.
//! The Geom2d package provides two concrete
//! classes of vectors: Geom2d_Direction (unit vector)
//! and Geom2d_VectorWithMagnitude.
class Geom2d_Vector : public Geom2d_Geometry
{

public:

  
  //! Reverses the vector <me>.
  Standard_EXPORT void Reverse();
  
  //! Returns a copy of <me> reversed.
  Standard_NODISCARD Standard_EXPORT Handle(Geom2d_Vector) Reversed() const;
  
  //! Computes the angular value, in radians, between this
  //! vector and vector Other. The result is a value
  //! between -Pi and Pi. The orientation is from this
  //! vector to vector Other.
  //! Raises VectorWithNullMagnitude if one of the two vectors is a vector with
  //! null magnitude because the angular value is indefinite.
  Standard_EXPORT Standard_Real Angle (const Handle(Geom2d_Vector)& Other) const;
  
  //! Returns the coordinates of <me>.
  Standard_EXPORT void Coord (Standard_Real& X, Standard_Real& Y) const;
  
  //! Returns the  Magnitude of <me>.
  Standard_EXPORT virtual Standard_Real Magnitude() const = 0;
  
  //! Returns the square magnitude of <me>.
  Standard_EXPORT virtual Standard_Real SquareMagnitude() const = 0;
  
  //! Returns the X coordinate of <me>.
  Standard_EXPORT Standard_Real X() const;
  
  //! Returns the Y coordinate of <me>.
  Standard_EXPORT Standard_Real Y() const;
  
  //! Cross product of <me> with the vector <Other>.
  Standard_EXPORT virtual Standard_Real Crossed (const Handle(Geom2d_Vector)& Other) const = 0;
  
  //! Returns the scalar product of 2 Vectors.
  Standard_EXPORT Standard_Real Dot (const Handle(Geom2d_Vector)& Other) const;
  
  //! Returns a non persistent copy of <me>.
  Standard_EXPORT gp_Vec2d Vec2d() const;




  DEFINE_STANDARD_RTTIEXT(Geom2d_Vector,Geom2d_Geometry)

protected:


  gp_Vec2d gpVec2d;


private:




};







#endif // _Geom2d_Vector_HeaderFile
