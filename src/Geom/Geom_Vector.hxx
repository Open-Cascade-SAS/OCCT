// Created on: 1993-03-10
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

#ifndef _Geom_Vector_HeaderFile
#define _Geom_Vector_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Vec.hxx>
#include <Geom_Geometry.hxx>
#include <Standard_Real.hxx>


class Geom_Vector;
DEFINE_STANDARD_HANDLE(Geom_Vector, Geom_Geometry)

//! The abstract class Vector describes the common
//! behavior of vectors in 3D space.
//! The Geom package provides two concrete classes of
//! vectors: Geom_Direction (unit vector) and Geom_VectorWithMagnitude.
class Geom_Vector : public Geom_Geometry
{

public:

  
  //! Reverses the vector <me>.
  Standard_EXPORT void Reverse();
  

  //! Returns a copy of <me> reversed.
  Standard_NODISCARD Standard_EXPORT Handle(Geom_Vector) Reversed() const;
  
  //! Computes the angular value, in radians, between this
  //! vector and vector Other. The result is a value between 0 and Pi.
  //! Exceptions
  //! gp_VectorWithNullMagnitude if:
  //! - the magnitude of this vector is less than or equal to
  //! gp::Resolution(), or
  //! - the magnitude of vector Other is less than or equal
  //! to gp::Resolution().
  Standard_EXPORT Standard_Real Angle (const Handle(Geom_Vector)& Other) const;
  
  //! Computes the angular value, in radians, between this
  //! vector and vector Other. The result is a value
  //! between -Pi and Pi. The vector VRef defines the
  //! positive sense of rotation: the angular value is positive
  //! if the cross product this ^ Other has the same
  //! orientation as VRef (in relation to the plane defined
  //! by this vector and vector Other). Otherwise, it is negative.
  //! Exceptions
  //! Standard_DomainError if this vector, vector Other
  //! and vector VRef are coplanar, except if this vector
  //! and vector Other are parallel.
  //! gp_VectorWithNullMagnitude if the magnitude of
  //! this vector, vector Other or vector VRef is less than
  //! or equal to gp::Resolution().
  Standard_EXPORT Standard_Real AngleWithRef (const Handle(Geom_Vector)& Other, const Handle(Geom_Vector)& VRef) const;
  
  //! Returns the coordinates X, Y and Z of this vector.
  Standard_EXPORT void Coord (Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const;
  
  //! Returns the  Magnitude of <me>.
  Standard_EXPORT virtual Standard_Real Magnitude() const = 0;
  
  //! Returns the square magnitude of <me>.
  Standard_EXPORT virtual Standard_Real SquareMagnitude() const = 0;
  
  //! Returns the X coordinate of <me>.
  Standard_EXPORT Standard_Real X() const;
  
  //! Returns the Y coordinate of <me>.
  Standard_EXPORT Standard_Real Y() const;
  
  //! Returns the Z coordinate of <me>.
  Standard_EXPORT Standard_Real Z() const;
  

  //! Computes the cross product between <me> and <Other>.
  //!
  //! Raised if <me> is a "Direction" and if <me> and <Other>
  //! are parallel because it is not possible to build a
  //! "Direction" with null length.
  Standard_EXPORT virtual void Cross (const Handle(Geom_Vector)& Other) = 0;
  

  //! Computes the cross product between <me> and <Other>.
  //! A new direction is returned.
  //!
  //! Raised if <me> is a "Direction" and if the two vectors
  //! are parallel because it is not possible to create a
  //! "Direction" with null length.
  Standard_EXPORT virtual Handle(Geom_Vector) Crossed (const Handle(Geom_Vector)& Other) const = 0;
  

  //! Computes the triple vector product  <me> ^(V1 ^ V2).
  //!
  //! Raised if <me> is a "Direction" and if V1 and V2 are parallel
  //! or <me> and (V1 ^ V2) are  parallel
  Standard_EXPORT virtual void CrossCross (const Handle(Geom_Vector)& V1, const Handle(Geom_Vector)& V2) = 0;
  

  //! Computes the triple vector product <me> ^(V1 ^ V2).
  //!
  //! Raised if <me> is a direction and if V1 and V2 are
  //! parallel or <me> and (V1 ^ V2) are parallel
  Standard_EXPORT virtual Handle(Geom_Vector) CrossCrossed (const Handle(Geom_Vector)& V1, const Handle(Geom_Vector)& V2) const = 0;
  
  //! Computes the scalar product of this vector and vector Other.
  Standard_EXPORT Standard_Real Dot (const Handle(Geom_Vector)& Other) const;
  

  //! Computes the triple scalar product. Returns me . (V1 ^ V2)
  Standard_EXPORT Standard_Real DotCross (const Handle(Geom_Vector)& V1, const Handle(Geom_Vector)& V2) const;
  
  //! Converts this vector into a gp_Vec vector.
  Standard_EXPORT const gp_Vec& Vec() const;




  DEFINE_STANDARD_RTTIEXT(Geom_Vector,Geom_Geometry)

protected:


  gp_Vec gpVec;


private:




};







#endif // _Geom_Vector_HeaderFile
