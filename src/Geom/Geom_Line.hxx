// Created on: 1993-03-10
// Created by: Philippe DAUTRY
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

#ifndef _Geom_Line_HeaderFile
#define _Geom_Line_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Ax1.hxx>
#include <Geom_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
class gp_Lin;
class gp_Pnt;
class gp_Dir;
class gp_Vec;
class gp_Trsf;
class Geom_Geometry;

class Geom_Line;
DEFINE_STANDARD_HANDLE(Geom_Line, Geom_Curve)

//! Describes an infinite line.
//! A line is defined and positioned in space with an axis
//! (gp_Ax1 object) which gives it an origin and a unit vector.
//! The Geom_Line line is parameterized:
//! P (U) = O + U*Dir, where:
//! - P is the point of parameter U,
//! - O is the origin and Dir the unit vector of its positioning axis.
//! The parameter range is ] -infinite, +infinite [.
//! The orientation of the line is given by the unit vector
//! of its positioning axis.
class Geom_Line : public Geom_Curve
{

public:
  //! Creates a line located in 3D space with the axis placement A1.
  //! The Location of A1 is the origin of the line.
  Standard_EXPORT Geom_Line(const gp_Ax1& A1);

  //! Creates a line from a non transient line from package gp.
  Standard_EXPORT Geom_Line(const gp_Lin& L);

  //! Constructs a line passing through point P and parallel to vector V
  //! (P and V are, respectively, the origin and the unit
  //! vector of the positioning axis of the line).
  Standard_EXPORT Geom_Line(const gp_Pnt& P, const gp_Dir& V);

  //! Set <me> so that <me> has the same geometric properties as L.
  Standard_EXPORT void SetLin(const gp_Lin& L);

  //! changes the direction of the line.
  Standard_EXPORT void SetDirection(const gp_Dir& V);

  //! changes the "Location" point (origin) of the line.
  Standard_EXPORT void SetLocation(const gp_Pnt& P);

  //! changes the "Location" and a the "Direction" of <me>.
  Standard_EXPORT void SetPosition(const gp_Ax1& A1);

  //! Returns non transient line from gp with the same geometric
  //! properties as <me>
  Standard_EXPORT gp_Lin Lin() const;

  //! Returns the positioning axis of this line; this is also its local coordinate system.
  Standard_EXPORT const gp_Ax1& Position() const;

  //! Changes the orientation of this line. As a result, the
  //! unit vector of the positioning axis of this line is reversed.
  Standard_EXPORT void Reverse() Standard_OVERRIDE;

  //! Computes the parameter on the reversed line for the
  //! point of parameter U on this line.
  //! For a line, the returned value is -U.
  Standard_EXPORT Standard_Real ReversedParameter(const Standard_Real U) const Standard_OVERRIDE;

  //! Returns the value of the first parameter of this
  //! line. This is Standard_Real::RealFirst().
  Standard_EXPORT Standard_Real FirstParameter() const Standard_OVERRIDE;

  //! Returns the value of the last parameter of this
  //! line. This is  Standard_Real::RealLast().
  Standard_EXPORT Standard_Real LastParameter() const Standard_OVERRIDE;

  //! returns False
  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;

  //! returns False
  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;

  //! Returns GeomAbs_CN, which is the global continuity of any line.
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;

  //! returns True.
  //! Raised if N < 0.
  Standard_EXPORT Standard_Boolean IsCN(const Standard_Integer N) const Standard_OVERRIDE;

  //! Returns in P the point of parameter U.
  //! P (U) = O + U * Dir where O is the "Location" point of the
  //! line and Dir the direction of the line.
  Standard_EXPORT void D0(const Standard_Real U, gp_Pnt& P) const Standard_OVERRIDE;

  //! Returns the point P of parameter u and the first derivative V1.
  Standard_EXPORT void D1(const Standard_Real U, gp_Pnt& P, gp_Vec& V1) const Standard_OVERRIDE;

  //! Returns the point P of parameter U, the first and second
  //! derivatives V1 and V2. V2 is a vector with null magnitude
  //! for a line.
  Standard_EXPORT void D2(const Standard_Real U,
                          gp_Pnt&             P,
                          gp_Vec&             V1,
                          gp_Vec&             V2) const Standard_OVERRIDE;

  //! V2 and V3 are vectors with null magnitude for a line.
  Standard_EXPORT void D3(const Standard_Real U,
                          gp_Pnt&             P,
                          gp_Vec&             V1,
                          gp_Vec&             V2,
                          gp_Vec&             V3) const Standard_OVERRIDE;

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec DN(const Standard_Real    U,
                            const Standard_Integer N) const Standard_OVERRIDE;

  //! Applies the transformation T to this line.
  Standard_EXPORT void Transform(const gp_Trsf& T) Standard_OVERRIDE;

  //! Returns the  parameter on the  transformed  curve for
  //! the transform of the point of parameter U on <me>.
  //!
  //! me->Transformed(T)->Value(me->TransformedParameter(U,T))
  //!
  //! is the same point as
  //!
  //! me->Value(U).Transformed(T)
  //!
  //! This methods returns <U> * T.ScaleFactor()
  Standard_EXPORT virtual Standard_Real TransformedParameter(const Standard_Real U,
                                                             const gp_Trsf&      T) const
    Standard_OVERRIDE;

  //! Returns a  coefficient to compute the parameter on
  //! the transformed  curve  for  the transform  of the
  //! point on <me>.
  //!
  //! Transformed(T)->Value(U * ParametricTransformation(T))
  //!
  //! is the same point as
  //!
  //! Value(U).Transformed(T)
  //!
  //! This methods returns T.ScaleFactor()
  Standard_EXPORT virtual Standard_Real ParametricTransformation(const gp_Trsf& T) const
    Standard_OVERRIDE;

  //! Creates a new object which is a copy of this line.
  Standard_EXPORT Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Geom_Line, Geom_Curve)

protected:
private:
  gp_Ax1 pos;
};

#endif // _Geom_Line_HeaderFile
