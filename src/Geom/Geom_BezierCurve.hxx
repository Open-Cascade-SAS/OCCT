// Created on: 1993-03-09
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

#ifndef _Geom_BezierCurve_HeaderFile
#define _Geom_BezierCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Geom_BoundedCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomAbs_Shape.hxx>
#include <BSplCLib.hxx>

class Standard_ConstructionError;
class Standard_DimensionError;
class Standard_RangeError;
class Standard_OutOfRange;
class gp_Pnt;
class gp_Vec;
class gp_Trsf;
class Geom_Geometry;


class Geom_BezierCurve;
DEFINE_STANDARD_HANDLE(Geom_BezierCurve, Geom_BoundedCurve)

//! Describes a rational or non-rational Bezier curve
//! - a non-rational Bezier curve is defined by a table of
//! poles (also called control points),
//! - a rational Bezier curve is defined by a table of
//! poles with varying weights.
//! These data are manipulated by two parallel arrays:
//! - the poles table, which is an array of gp_Pnt points, and
//! - the weights table, which is an array of reals.
//! The bounds of these arrays are 1 and "the number of "poles" of the curve.
//! The poles of the curve are "control points" used to deform the curve.
//! The first pole is the start point of the curve, and the
//! last pole is the end point of the curve. The segment
//! that joins the first pole to the second pole is the
//! tangent to the curve at its start point, and the
//! segment that joins the last pole to the
//! second-from-last pole is the tangent to the curve at its end point.
//! It is more difficult to give a geometric signification to
//! the weights but they are useful for providing the exact
//! representations of arcs of a circle or ellipse.
//! Moreover, if the weights of all poles are equal, the
//! curve is polynomial; it is therefore a non-rational
//! curve. The non-rational curve is a special and
//! frequently used case. The weights are defined and
//! used only in the case of a rational curve.
//! The degree of a Bezier curve is equal to the number
//! of poles, minus 1. It must be greater than or equal to
//! 1. However, the degree of a Geom_BezierCurve
//! curve is limited to a value (25) which is defined and
//! controlled by the system. This value is returned by the function MaxDegree.
//! The parameter range for a Bezier curve is [ 0, 1 ].
//! If the first and last control points of the Bezier curve
//! are the same point then the curve is closed. For
//! example, to create a closed Bezier curve with four
//! control points, you have to give the set of control
//! points P1, P2, P3 and P1.
//! The continuity of a Bezier curve is infinite.
//! It is not possible to build a Bezier curve with negative
//! weights. We consider that a weight value is zero if it
//! is less than or equal to gp::Resolution(). We
//! also consider that two weight values W1 and W2 are equal if:
//! |W2 - W1| <= gp::Resolution().
//! Warning
//! - When considering the continuity of a closed Bezier
//! curve at the junction point, remember that a curve
//! of this type is never periodic. This means that the
//! derivatives for the parameter u = 0 have no
//! reason to be the same as the derivatives for the
//! parameter u = 1 even if the curve is closed.
//! - The length of a Bezier curve can be null.
class Geom_BezierCurve : public Geom_BoundedCurve
{

public:

  
  //! Creates a non rational Bezier curve with a set of poles
  //! CurvePoles.  The weights are defaulted to all being 1.
  //! Raises ConstructionError if the number of poles is greater than MaxDegree + 1
  //! or lower than 2.
  Standard_EXPORT Geom_BezierCurve(const TColgp_Array1OfPnt& CurvePoles);
  
  //! Creates a rational Bezier curve with the set of poles
  //! CurvePoles and the set of weights  PoleWeights .
  //! If all the weights are identical the curve is considered
  //! as non rational. Raises ConstructionError if
  //! the number of poles is greater than  MaxDegree + 1 or lower
  //! than 2 or CurvePoles and CurveWeights have not the same length
  //! or one weight value is lower or equal to Resolution from package gp.
  Standard_EXPORT Geom_BezierCurve(const TColgp_Array1OfPnt& CurvePoles, const TColStd_Array1OfReal& PoleWeights);
  
  //! Increases the degree of a bezier curve. Degree is the new
  //! degree of <me>. Raises ConstructionError
  //! if Degree is greater than MaxDegree or lower than 2
  //! or lower than the initial degree of <me>.
  Standard_EXPORT void Increase (const Standard_Integer Degree);
  
  //! Inserts a pole P after the pole of range Index.
  //! If the curve <me> is rational the weight value for the new
  //! pole of range Index is 1.0.
  //! raised if Index is not in the range [1, NbPoles]
  //!
  //! raised if the resulting number of poles is greater than
  //! MaxDegree + 1.
  Standard_EXPORT void InsertPoleAfter (const Standard_Integer Index, const gp_Pnt& P);
  

  //! Inserts a pole with its weight in the set of poles after the
  //! pole of range Index. If the curve was non rational it can
  //! become rational if all the weights are not identical.
  //! Raised if Index is not in the range [1, NbPoles]
  //!
  //! Raised if the resulting number of poles is greater than
  //! MaxDegree + 1.
  //! Raised if Weight is lower or equal to Resolution from package gp.
  Standard_EXPORT void InsertPoleAfter (const Standard_Integer Index, const gp_Pnt& P, const Standard_Real Weight);
  
  //! Inserts a pole P before the pole of range Index.
  //! If the curve <me> is rational the weight value for the new
  //! pole of range Index is 1.0.
  //! Raised if Index is not in the range [1, NbPoles]
  //!
  //! Raised if the resulting number of poles is greater than
  //! MaxDegree + 1.
  Standard_EXPORT void InsertPoleBefore (const Standard_Integer Index, const gp_Pnt& P);
  

  //! Inserts a pole with its weight in the set of poles after
  //! the pole of range Index. If the curve was non rational it
  //! can become rational if all the weights are not identical.
  //! Raised if Index is not in the range [1, NbPoles]
  //!
  //! Raised if the resulting number of poles is greater than
  //! MaxDegree + 1.
  //! Raised if Weight is lower or equal to Resolution from
  //! package gp.
  Standard_EXPORT void InsertPoleBefore (const Standard_Integer Index, const gp_Pnt& P, const Standard_Real Weight);
  
  //! Removes the pole of range Index.
  //! If the curve was rational it can become non rational.
  //! Raised if Index is not in the range [1, NbPoles]
  //! Raised if Degree is lower than 2.
  Standard_EXPORT void RemovePole (const Standard_Integer Index);
  

  //! Reverses the direction of parametrization of <me>
  //! Value (NewU) =  Value (1 - OldU)
  Standard_EXPORT void Reverse() Standard_OVERRIDE;
  
  //! Returns the  parameter on the  reversed  curve for
  //! the point of parameter U on <me>.
  //!
  //! returns 1-U
  Standard_EXPORT Standard_Real ReversedParameter (const Standard_Real U) const Standard_OVERRIDE;
  

  //! Segments the curve between U1 and U2 which can be out
  //! of the bounds of the curve. The curve is oriented from U1
  //! to U2.
  //! The control points are modified, the first and the last point
  //! are not the same but the parametrization range is [0, 1]
  //! else it could not be a Bezier curve.
  //! Warnings :
  //! Even if <me> is not closed it can become closed after the
  //! segmentation for example if U1 or U2 are out of the bounds
  //! of the curve <me> or if the curve makes loop.
  //! After the segmentation the length of a curve can be null.
  Standard_EXPORT void Segment (const Standard_Real U1, const Standard_Real U2);
  

  //! Substitutes the pole of range index with P.
  //! If the curve <me> is rational the weight of range Index
  //! is not modified.
  //! raiseD if Index is not in the range [1, NbPoles]
  Standard_EXPORT void SetPole (const Standard_Integer Index, const gp_Pnt& P);
  

  //! Substitutes the pole and the weights of range Index.
  //! If the curve <me> is not rational it can become rational
  //! if all the weights are not identical.
  //! If the curve was rational it can become non rational if
  //! all the weights are identical.
  //! Raised if Index is not in the range [1, NbPoles]
  //! Raised if Weight <= Resolution from package gp
  Standard_EXPORT void SetPole (const Standard_Integer Index, const gp_Pnt& P, const Standard_Real Weight);
  

  //! Changes the weight of the pole of range Index.
  //! If the curve <me> is not rational it can become rational
  //! if all the weights are not identical.
  //! If the curve was rational it can become non rational if
  //! all the weights are identical.
  //! Raised if Index is not in the range [1, NbPoles]
  //! Raised if Weight <= Resolution from package gp
  Standard_EXPORT void SetWeight (const Standard_Integer Index, const Standard_Real Weight);
  

  //! Returns True if the distance between the first point
  //! and the last point of the curve is lower or equal to
  //! the Resolution from package gp.
  Standard_EXPORT Standard_Boolean IsClosed() const Standard_OVERRIDE;
  
  //! Continuity of the curve, returns True.
  Standard_EXPORT Standard_Boolean IsCN (const Standard_Integer N) const Standard_OVERRIDE;
  

  //! Returns True if the parametrization of a curve is periodic.
  //! (P(u) = P(u + T) T = constante)
  Standard_EXPORT Standard_Boolean IsPeriodic() const Standard_OVERRIDE;
  

  //! Returns false if all the weights are identical. The tolerance
  //! criterion is Resolution from package gp.
  Standard_EXPORT Standard_Boolean IsRational() const;
  
  //! a Bezier curve is CN
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  
  //! Returns the polynomial degree of the curve.
  //! it is the number of poles - 1
  //! point P and derivatives (V1, V2, V3) computation
  //! The Bezier Curve has a Polynomial representation so the
  //! parameter U can be out of the bounds of the curve.
  Standard_EXPORT Standard_Integer Degree() const;
  
  Standard_EXPORT void D0 (const Standard_Real U, gp_Pnt& P) const Standard_OVERRIDE;
  
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1) const Standard_OVERRIDE;
  
  Standard_EXPORT void D2 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const Standard_OVERRIDE;
  
  //! For this Bezier curve, computes
  //! - the point P of parameter U, or
  //! - the point P and one or more of the following values:
  //! - V1, the first derivative vector,
  //! - V2, the second derivative vector,
  //! - V3, the third derivative vector.
  //! Note: the parameter U can be outside the bounds of the curve.
  Standard_EXPORT void D3 (const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const Standard_OVERRIDE;
  
  //! For the point of parameter U of this Bezier curve,
  //! computes the vector corresponding to the Nth derivative.
  //! Note: the parameter U can be outside the bounds of the curve.
  //! Exceptions Standard_RangeError if N is less than 1.
  Standard_EXPORT gp_Vec DN (const Standard_Real U, const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Returns Value (U=0.), it is the first control point of the curve.
  Standard_EXPORT gp_Pnt StartPoint() const Standard_OVERRIDE;
  
  //! Returns Value (U=1.), it is the last control point of the Bezier curve.
  Standard_EXPORT gp_Pnt EndPoint() const Standard_OVERRIDE;
  
  //! Returns the value of the first  parameter of this
  //! Bezier curve. This is 0.0, which gives the start point of this Bezier curve
  Standard_EXPORT Standard_Real FirstParameter() const Standard_OVERRIDE;
  
  //! Returns the value of the last parameter of this
  //! Bezier curve. This is  1.0, which gives the end point of this Bezier curve.
  Standard_EXPORT Standard_Real LastParameter() const Standard_OVERRIDE;
  
  //! Returns the number of poles of this Bezier curve.
  Standard_EXPORT Standard_Integer NbPoles() const;
  
  //! Returns the pole of range Index.
  //! Raised if Index is not in the range [1, NbPoles]
  Standard_EXPORT const gp_Pnt& Pole (const Standard_Integer Index) const;
  
  //! Returns all the poles of the curve.
  //!
  //! Raised if the length of P is not equal to the number of poles.
  Standard_EXPORT void Poles (TColgp_Array1OfPnt& P) const;

    //! Returns all the poles of the curve.
  Standard_EXPORT const TColgp_Array1OfPnt& Poles () const;
  
  //! Returns the weight of range Index.
  //! Raised if Index is not in the range [1, NbPoles]
  Standard_EXPORT Standard_Real Weight (const Standard_Integer Index) const;
  
  //! Returns all the weights of the curve.
  //!
  //! Raised if the length of W is not equal to the number of poles.
  Standard_EXPORT void Weights (TColStd_Array1OfReal& W) const;

  //! Returns all the weights of the curve.
  const TColStd_Array1OfReal* Weights() const
  {
    if (!weights.IsNull())
      return &weights->Array1();
    return BSplCLib::NoWeights();
  }

  //! Applies the transformation T to this Bezier curve.
  Standard_EXPORT void Transform (const gp_Trsf& T) Standard_OVERRIDE;
  

  //! Returns the value of the maximum polynomial degree
  //! of any Geom_BezierCurve curve. This value is 25.
  Standard_EXPORT static Standard_Integer MaxDegree();
  
  //! Computes for this Bezier curve the parametric
  //! tolerance UTolerance for a given 3D tolerance Tolerance3D.
  //! If f(t) is the equation of this Bezier curve,
  //! UTolerance ensures that:
  //! |t1-t0| < UTolerance ===> |f(t1)-f(t0)| < Tolerance3D
  Standard_EXPORT void Resolution (const Standard_Real Tolerance3D, Standard_Real& UTolerance);
  
  //! Creates a new object which is a copy of this Bezier curve.
  Standard_EXPORT Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Geom_BezierCurve,Geom_BoundedCurve)

protected:




private:

  
  //! Set  poles  to  Poles,  weights to  Weights  (not
  //! copied). If Weights is   null  the  curve is    non
  //! rational. Create the arrays of coefficients.  Poles
  //! and    Weights  are   assumed   to  have the  first
  //! coefficient 1.
  //! Update rational and closed.
  //!
  //! if nbpoles < 2 or nbboles > MaDegree + 1
  void Init (const Handle(TColgp_HArray1OfPnt)& Poles, const Handle(TColStd_HArray1OfReal)& Weights);

  Standard_Boolean rational;
  Standard_Boolean closed;
  Handle(TColgp_HArray1OfPnt) poles;
  Handle(TColStd_HArray1OfReal) weights;
  Standard_Real maxderivinv;
  Standard_Boolean maxderivinvok;


};







#endif // _Geom_BezierCurve_HeaderFile
