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

#ifndef _Geom2d_Line_HeaderFile
#define _Geom2d_Line_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Ax2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
class Standard_RangeError;
class gp_Ax2d;
class gp_Lin2d;
class gp_Pnt2d;
class gp_Dir2d;
class gp_Vec2d;
class gp_Trsf2d;
class Geom2d_Geometry;


class Geom2d_Line;
DEFINE_STANDARD_HANDLE(Geom2d_Line, Geom2d_Curve)

//! Describes an infinite line in the plane (2D space).
//! A line is defined and positioned in the plane with an
//! axis (gp_Ax2d object) which gives it an origin and a unit vector.
//! The Geom2d_Line line is parameterized as follows:
//! P (U) = O + U*Dir
//! where:
//! - P is the point of parameter U,
//! - O is the origin and Dir the unit vector of its positioning axis.
//! The parameter range is ] -infinite, +infinite [.
//! The orientation of the line is given by the unit vector
//! of its positioning axis.
//! See Also
//! GCE2d_MakeLine which provides functions for more
//! complex line constructions
//! gp_Ax2d
//! gp_Lin2d for an equivalent, non-parameterized data structure.
class Geom2d_Line : public Geom2d_Curve
{

public:

  

  //! Creates a line located in 2D space with the axis placement A.
  //! The Location of A is the origin of the line.
  Standard_EXPORT Geom2d_Line(const gp_Ax2d& A);
  

  //! Creates a line by conversion of the gp_Lin2d line L.
  Standard_EXPORT Geom2d_Line(const gp_Lin2d& L);
  
  //! Constructs a line passing through point P and parallel to
  //! vector V (P and V are, respectively, the origin
  //! and the unit vector of the positioning axis of the line).
  Standard_EXPORT Geom2d_Line(const gp_Pnt2d& P, const gp_Dir2d& V);
  

  //! Set <me> so that <me> has the same geometric properties as L.
  Standard_EXPORT void SetLin2d (const gp_Lin2d& L);
  
  //! changes the direction of the line.
  Standard_EXPORT void SetDirection (const gp_Dir2d& V);
  
  //! changes the direction of the line.
  Standard_EXPORT const gp_Dir2d& Direction() const;
  

  //! Changes the "Location" point (origin) of the line.
  Standard_EXPORT void SetLocation (const gp_Pnt2d& P);
  

  //! Changes the "Location" point (origin) of the line.
  Standard_EXPORT const gp_Pnt2d& Location() const;
  

  //! Changes the "Location" and a the "Direction" of <me>.
  Standard_EXPORT void SetPosition (const gp_Ax2d& A);
  
  Standard_EXPORT const gp_Ax2d& Position() const;
  

  //! Returns non persistent line from gp with the same geometric
  //! properties as <me>
  Standard_EXPORT gp_Lin2d Lin2d() const;
  
  //! Changes the orientation of this line. As a result, the
  //! unit vector of the positioning axis of this line is reversed.
  Standard_EXPORT void Reverse() Standard_OVERRIDE;
  
  //! Computes the parameter on the reversed line for the
  //! point of parameter U on this line.
  //! For a line, the returned value is -U.
  Standard_EXPORT Standard_Real ReversedParameter (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Returns RealFirst  from  Standard.
  Standard_EXPORT Standard_Real FirstParameter() const Standard_OVERRIDE;
  
  //! Returns RealLast  from Standard
  Standard_EXPORT Standard_Real LastParameter() const Standard_OVERRIDE;
  
  //! Returns False
  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;
  
  //! Returns False
  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;
  
  //! Returns GeomAbs_CN, which is the global continuity of any line.
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  
  //! Computes the distance between <me> and the point P.
  Standard_EXPORT Standard_Real Distance (const gp_Pnt2d& P) const;
  
  //! Returns True.
  Standard_EXPORT Standard_Boolean IsCN (const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Returns in P the point of parameter U.
  //! P (U) = O + U * Dir where O is the "Location" point of the
  //! line and Dir the direction of the line.
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt2d& P) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter u and the first derivative V1.
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1) const Standard_OVERRIDE;
  

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2. V2 is a vector with null magnitude
  //! for a line.
  Standard_EXPORT void D2 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const Standard_OVERRIDE;
  

  //! V2 and V3 are vectors with null magnitude for a line.
  Standard_EXPORT void D3 (const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3) const Standard_OVERRIDE;
  
  //! For the point of parameter U of this line, computes
  //! the vector corresponding to the Nth derivative.
  //! Note: if N is greater than or equal to 2, the result is a
  //! vector with null magnitude.
  //! Exceptions Standard_RangeError if N is less than 1.
  Standard_EXPORT gp_Vec2d DN (const Standard_Real U, const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Applies the transformation T to this line.
  Standard_EXPORT void Transform (const gp_Trsf2d& T) Standard_OVERRIDE;
  
  //! Computes the parameter on the line transformed by
  //! T for the point of parameter U on this line.
  //! For a line, the returned value is equal to U multiplied
  //! by the scale factor of transformation T.
  Standard_EXPORT virtual Standard_Real TransformedParameter (const Standard_Real U, const gp_Trsf2d& T) const Standard_OVERRIDE;
  
  //! Returns the coefficient required to compute the
  //! parametric transformation of this line when
  //! transformation T is applied. This coefficient is the
  //! ratio between the parameter of a point on this line
  //! and the parameter of the transformed point on the
  //! new line transformed by T.
  //! For a line, the returned value is the scale factor of the transformation T.
  Standard_EXPORT virtual Standard_Real ParametricTransformation (const gp_Trsf2d& T) const Standard_OVERRIDE;
  
  //! Creates a new object, which is a copy of this line.
  Standard_EXPORT Handle(Geom2d_Geometry) Copy() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Geom2d_Line,Geom2d_Curve)

protected:




private:


  gp_Ax2d pos;


};







#endif // _Geom2d_Line_HeaderFile
