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

#ifndef _Geom2d_Direction_HeaderFile
#define _Geom2d_Direction_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom2d_Vector.hxx>
#include <Standard_Real.hxx>
class gp_Dir2d;
class gp_Trsf2d;
class Geom2d_Geometry;


class Geom2d_Direction;
DEFINE_STANDARD_HANDLE(Geom2d_Direction, Geom2d_Vector)


//! The class Direction specifies a vector that is never null.
//! It is a unit vector.
class Geom2d_Direction : public Geom2d_Vector
{

public:

  
  //! Creates a unit vector with it 2 cartesian coordinates.
  //!
  //! Raised if Sqrt( X*X + Y*Y) <= Resolution from gp.
  Standard_EXPORT Geom2d_Direction(const Standard_Real X, const Standard_Real Y);
  
  //! Creates a persistent copy of <me>.
  Standard_EXPORT Geom2d_Direction(const gp_Dir2d& V);
  
  //! Assigns the coordinates X and Y to this unit vector,
  //! then normalizes it.
  //! Exceptions
  //! Standard_ConstructionError if Sqrt(X*X +
  //! Y*Y) is less than or equal to gp::Resolution().
  Standard_EXPORT void SetCoord (const Standard_Real X, const Standard_Real Y);
  
  //! Converts the gp_Dir2d unit vector V into this unit vector.
  Standard_EXPORT void SetDir2d (const gp_Dir2d& V);
  

  //! Assigns a value to the X coordinate of this unit vector, then normalizes it.
  //! Exceptions
  //! Standard_ConstructionError if the value assigned
  //! causes the magnitude of the vector to become less
  //! than or equal to gp::Resolution().
  Standard_EXPORT void SetX (const Standard_Real X);
  
  //! Assigns a value to the Y coordinate of this unit vector, then normalizes it.
  //! Exceptions
  //! Standard_ConstructionError if the value assigned
  //! causes the magnitude of the vector to become less
  //! than or equal to gp::Resolution().
  Standard_EXPORT void SetY (const Standard_Real Y);
  
  //! Converts this unit vector into a gp_Dir2d unit vector.
  Standard_EXPORT gp_Dir2d Dir2d() const;
  
  //! returns 1.0
  Standard_EXPORT Standard_Real Magnitude() const Standard_OVERRIDE;
  
  //! returns 1.0
  Standard_EXPORT Standard_Real SquareMagnitude() const Standard_OVERRIDE;
  
  //! Computes the cross product between <me> and <Other>.
  Standard_EXPORT Standard_Real Crossed (const Handle(Geom2d_Vector)& Other) const Standard_OVERRIDE;
Standard_Real operator ^ (const Handle(Geom2d_Vector)& Other) const
{
  return Crossed(Other);
}
  
  //! Applies the transformation T to this unit vector, then normalizes it.
  Standard_EXPORT void Transform (const gp_Trsf2d& T) Standard_OVERRIDE;
  
  //! Creates a new object which is a copy of this unit vector.
  Standard_EXPORT Handle(Geom2d_Geometry) Copy() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Geom2d_Direction,Geom2d_Vector)

protected:




private:




};







#endif // _Geom2d_Direction_HeaderFile
