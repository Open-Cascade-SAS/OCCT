// Created on: 1993-03-09
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

#ifndef _Geom_BezierSurface_HeaderFile
#define _Geom_BezierSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <TColgp_HArray2OfPnt.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Geom_BoundedSurface.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <GeomAbs_Shape.hxx>
#include <BSplSLib.hxx>

class gp_Pnt;
class gp_Vec;
class Geom_Curve;
class gp_Trsf;
class Geom_Geometry;


class Geom_BezierSurface;
DEFINE_STANDARD_HANDLE(Geom_BezierSurface, Geom_BoundedSurface)

//! Describes a rational or non-rational Bezier surface.
//! - A non-rational Bezier surface is defined by a table
//! of poles (also known as control points).
//! - A rational Bezier surface is defined by a table of
//! poles with varying associated weights.
//! This data is manipulated using two associative 2D arrays:
//! - the poles table, which is a 2D array of gp_Pnt, and
//! - the weights table, which is a 2D array of reals.
//! The bounds of these arrays are:
//! - 1 and NbUPoles for the row bounds, where
//! NbUPoles is the number of poles of the surface
//! in the u parametric direction, and
//! - 1 and NbVPoles for the column bounds, where
//! NbVPoles is the number of poles of the surface
//! in the v parametric direction.
//! The poles of the surface, the "control points", are the
//! points used to shape and reshape the surface. They
//! comprise a rectangular network of points:
//! - The points (1, 1), (NbUPoles, 1), (1,
//! NbVPoles) and (NbUPoles, NbVPoles)
//! are the four parametric "corners" of the surface.
//! - The first column of poles and the last column of
//! poles define two Bezier curves which delimit the
//! surface in the v parametric direction. These are
//! the v isoparametric curves corresponding to
//! values 0 and 1 of the v parameter.
//! - The first row of poles and the last row of poles
//! define two Bezier curves which delimit the surface
//! in the u parametric direction. These are the u
//! isoparametric curves corresponding to values 0
//! and 1 of the u parameter.
//! It is more difficult to define a geometrical significance
//! for the weights. However they are useful for
//! representing a quadric surface precisely. Moreover, if
//! the weights of all the poles are equal, the surface has
//! a polynomial equation, and hence is a "non-rational surface".
//! The non-rational surface is a special, but frequently
//! used, case, where all poles have identical weights.
//! The weights are defined and used only in the case of
//! a rational surface. This rational characteristic is
//! defined in each parametric direction. Hence, a
//! surface can be rational in the u parametric direction,
//! and non-rational in the v parametric direction.
//! Likewise, the degree of a surface is defined in each
//! parametric direction. The degree of a Bezier surface
//! in a given parametric direction is equal to the number
//! of poles of the surface in that parametric direction,
//! minus 1. This must be greater than or equal to 1.
//! However, the degree for a Geom_BezierSurface is
//! limited to a value of (25) which is defined and
//! controlled by the system. This value is returned by the
//! function MaxDegree.
//! The parameter range for a Bezier surface is [ 0, 1 ]
//! in the two parametric directions.
//! A Bezier surface can also be closed, or open, in each
//! parametric direction. If the first row of poles is
//! identical to the last row of poles, the surface is closed
//! in the u parametric direction. If the first column of
//! poles is identical to the last column of poles, the
//! surface is closed in the v parametric direction.
//! The continuity of a Bezier surface is infinite in the u
//! parametric direction and the in v parametric direction.
//! Note: It is not possible to build a Bezier surface with
//! negative weights. Any weight value that is less than,
//! or equal to, gp::Resolution() is considered
//! to be zero. Two weight values, W1 and W2, are
//! considered equal if: |W2-W1| <= gp::Resolution()
class Geom_BezierSurface : public Geom_BoundedSurface
{

public:

  

  //! Creates a non-rational Bezier surface with a set of poles.
  //! Control points representation :
  //! SPoles(Uorigin,Vorigin) ...................SPoles(Uorigin,Vend)
  //! .                                     .
  //! .                                     .
  //! SPoles(Uend, Vorigin) .....................SPoles(Uend, Vend)
  //! For the double array the row indice corresponds to the parametric
  //! U direction and the columns indice corresponds to the parametric
  //! V direction.
  //! The weights are defaulted to all being 1.
  //!
  //! Raised if the number of poles of the surface is lower than 2
  //! or greater than MaxDegree + 1 in one of the two directions
  //! U or V.
  Standard_EXPORT Geom_BezierSurface(const TColgp_Array2OfPnt& SurfacePoles);
  
  //! ---Purpose
  //! Creates a rational Bezier surface with a set of poles and a
  //! set of weights.
  //! For the double array the row indice corresponds to the parametric
  //! U direction and the columns indice corresponds to the parametric
  //! V direction.
  //! If all the weights are identical the surface is considered as
  //! non-rational (the tolerance criterion is Resolution from package
  //! gp).
  //!
  //! Raised if SurfacePoles and PoleWeights have not the same
  //! Rowlength or have not the same ColLength.
  //! Raised if PoleWeights (i, j) <= Resolution from gp;
  //! Raised if the number of poles of the surface is lower than 2
  //! or greater than MaxDegree + 1 in one of the two directions U or V.
  Standard_EXPORT Geom_BezierSurface(const TColgp_Array2OfPnt& SurfacePoles, const TColStd_Array2OfReal& PoleWeights);
  
  //! Exchanges the direction U and V on a Bezier surface
  //! As a consequence:
  //! - the poles and weights tables are transposed,
  //! - degrees, rational characteristics and so on are
  //! exchanged between the two parametric directions, and
  //! - the orientation of the surface is reversed.
  Standard_EXPORT void ExchangeUV();
  
  //! Increases the degree of this Bezier surface in the two parametric directions.
  //!
  //! Raised if UDegree < UDegree <me>  or VDegree < VDegree <me>
  //! Raised if the degree of the surface is greater than MaxDegree
  //! in one of the two directions U or V.
  Standard_EXPORT void Increase (const Standard_Integer UDeg, const Standard_Integer VDeg);
  

  //! Inserts a column of poles. If the surface is rational the weights
  //! values associated with CPoles are equal defaulted to 1.
  //!
  //! Raised if Vindex < 1 or VIndex > NbVPoles.
  //!
  //! raises if VDegree is greater than MaxDegree.
  //! raises if the Length of CPoles is not equal to NbUPoles
  Standard_EXPORT void InsertPoleColAfter (const Standard_Integer VIndex, const TColgp_Array1OfPnt& CPoles);
  

  //! Inserts a column of poles and weights.
  //! If the surface was non-rational it can become rational.
  //!
  //! Raised if Vindex < 1 or VIndex > NbVPoles.
  //! Raised if
  //! . VDegree is greater than MaxDegree.
  //! . the Length of CPoles is not equal to NbUPoles
  //! . a weight value is lower or equal to Resolution from
  //! package gp
  Standard_EXPORT void InsertPoleColAfter (const Standard_Integer VIndex, const TColgp_Array1OfPnt& CPoles, const TColStd_Array1OfReal& CPoleWeights);
  

  //! Inserts a column of poles. If the surface is rational the weights
  //! values associated with CPoles are equal defaulted to 1.
  //!
  //! Raised if Vindex < 1 or VIndex > NbVPoles.
  //!
  //! Raised if VDegree is greater than MaxDegree.
  //! Raised if the Length of CPoles is not equal to NbUPoles
  Standard_EXPORT void InsertPoleColBefore (const Standard_Integer VIndex, const TColgp_Array1OfPnt& CPoles);
  

  //! Inserts a column of poles and weights.
  //! If the surface was non-rational it can become rational.
  //!
  //! Raised if Vindex < 1 or VIndex > NbVPoles.
  //! Raised if :
  //! . VDegree is greater than MaxDegree.
  //! . the Length of CPoles is not equal to NbUPoles
  //! . a weight value is lower or equal to Resolution from
  //! package gp
  Standard_EXPORT void InsertPoleColBefore (const Standard_Integer VIndex, const TColgp_Array1OfPnt& CPoles, const TColStd_Array1OfReal& CPoleWeights);
  

  //! Inserts a row of poles. If the surface is rational the weights
  //! values associated with CPoles are equal defaulted to 1.
  //!
  //! Raised if Uindex < 1 or UIndex > NbUPoles.
  //!
  //! Raised if UDegree is greater than MaxDegree.
  //! Raised if the Length of CPoles is not equal to NbVPoles
  Standard_EXPORT void InsertPoleRowAfter (const Standard_Integer UIndex, const TColgp_Array1OfPnt& CPoles);
  

  //! Inserts a row of poles and weights.
  //! If the surface was non-rational it can become rational.
  //!
  //! Raised if Uindex < 1 or UIndex > NbUPoles.
  //! Raised if :
  //! . UDegree is greater than MaxDegree.
  //! . the Length of CPoles is not equal to NbVPoles
  //! . a weight value is lower or equal to Resolution from
  //! package gp
  Standard_EXPORT void InsertPoleRowAfter (const Standard_Integer UIndex, const TColgp_Array1OfPnt& CPoles, const TColStd_Array1OfReal& CPoleWeights);
  

  //! Inserts a row of poles. If the surface is rational the weights
  //! values associated with CPoles are equal defaulted to 1.
  //!
  //! Raised if Uindex < 1 or UIndex > NbUPoles.
  //!
  //! Raised if UDegree is greater than MaxDegree.
  //! Raised if the Length of CPoles is not equal to NbVPoles
  Standard_EXPORT void InsertPoleRowBefore (const Standard_Integer UIndex, const TColgp_Array1OfPnt& CPoles);
  

  //! Inserts a row of poles and weights.
  //! If the surface was non-rational it can become rational.
  //!
  //! Raised if Uindex < 1 or UIndex > NbUPoles.
  //! Raised if :
  //! . UDegree is greater than MaxDegree.
  //! . the Length of CPoles is not equal to NbVPoles
  //! . a weight value is lower or equal to Resolution from
  //! pacakage gp
  Standard_EXPORT void InsertPoleRowBefore (const Standard_Integer UIndex, const TColgp_Array1OfPnt& CPoles, const TColStd_Array1OfReal& CPoleWeights);
  
  //! Removes a column of poles.
  //! If the surface was rational it can become non-rational.
  //!
  //! Raised if NbVPoles <= 2 after removing, a Bezier surface
  //! must have at least two columns of poles.
  //! Raised if Vindex < 1 or VIndex > NbVPoles
  Standard_EXPORT void RemovePoleCol (const Standard_Integer VIndex);
  
  //! Removes a row of poles.
  //! If the surface was rational it can become non-rational.
  //!
  //! Raised if NbUPoles <= 2 after removing, a Bezier surface
  //! must have at least two rows of poles.
  //! Raised if Uindex < 1 or UIndex > NbUPoles
  Standard_EXPORT void RemovePoleRow (const Standard_Integer UIndex);
  
  //! Modifies this Bezier surface by segmenting it
  //! between U1 and U2 in the u parametric direction,
  //! and between V1 and V2 in the v parametric
  //! direction. U1, U2, V1, and V2 can be outside the
  //! bounds of this surface.
  //! - U1 and U2 isoparametric Bezier curves,
  //! segmented between V1 and V2, become the two
  //! bounds of the surface in the v parametric
  //! direction (0. and 1. u isoparametric curves).
  //! - V1 and V2 isoparametric Bezier curves,
  //! segmented between U1 and U2, become the two
  //! bounds of the surface in the u parametric
  //! direction (0. and 1. v isoparametric curves).
  //! The poles and weights tables are modified, but the
  //! degree of this surface in the u and v parametric
  //! directions does not change.
  //! U1 can be greater than U2, and V1 can be greater
  //! than V2. In these cases, the corresponding
  //! parametric direction is inverted. The orientation of
  //! the surface is inverted if one (and only one)
  //! parametric direction is inverted.
  Standard_EXPORT void Segment (const Standard_Real U1, const Standard_Real U2, const Standard_Real V1, const Standard_Real V2);
  
  //! Modifies a pole value.
  //! If the surface is rational the weight of range (UIndex, VIndex)
  //! is not modified.
  //!
  //! Raised if  UIndex < 1 or UIndex > NbUPoles  or  VIndex < 1
  //! or VIndex > NbVPoles.
  Standard_EXPORT void SetPole (const Standard_Integer UIndex, const Standard_Integer VIndex, const gp_Pnt& P);
  

  //! Substitutes the pole and the weight of range UIndex, VIndex.
  //! If the surface <me> is not rational it can become rational.
  //! if the surface was rational it can become non-rational.
  //!
  //! raises if  UIndex < 1 or UIndex > NbUPoles  or  VIndex < 1
  //! or VIndex > NbVPoles.
  //! Raised if Weight <= Resolution from package gp.
  Standard_EXPORT void SetPole (const Standard_Integer UIndex, const Standard_Integer VIndex, const gp_Pnt& P, const Standard_Real Weight);
  
  //! Modifies a column of poles.
  //! The length of CPoles can be lower but not greater than NbUPoles
  //! so you can modify just a part of the column.
  //! Raised if VIndex < 1 or  VIndex > NbVPoles
  //!
  //! Raised if CPoles.Lower() < 1 or CPoles.Upper() > NbUPoles
  Standard_EXPORT void SetPoleCol (const Standard_Integer VIndex, const TColgp_Array1OfPnt& CPoles);
  
  //! Modifies a column of poles.
  //! If the surface was rational it can become non-rational
  //! If the surface was non-rational it can become rational.
  //! The length of CPoles can be lower but not greater than NbUPoles
  //! so you can modify just a part of the column.
  //! Raised if VIndex < 1 or  VIndex > NbVPoles
  //!
  //! Raised if CPoles.Lower() < 1 or CPoles.Upper() > NbUPoles
  //! Raised if CPoleWeights and CPoles have not the same bounds.
  //! Raised if one of the weight value CPoleWeights (i) is lower
  //! or equal to Resolution from package gp.
  Standard_EXPORT void SetPoleCol (const Standard_Integer VIndex, const TColgp_Array1OfPnt& CPoles, const TColStd_Array1OfReal& CPoleWeights);
  
  //! Modifies a row of poles.
  //! The length of CPoles can be lower but not greater than NbVPoles
  //! so you can modify just a part of the row.
  //! Raised if UIndex < 1 or  UIndex > NbUPoles
  //!
  //! Raised if CPoles.Lower() < 1 or CPoles.Upper() > NbVPoles
  Standard_EXPORT void SetPoleRow (const Standard_Integer UIndex, const TColgp_Array1OfPnt& CPoles);
  
  //! Modifies a row of poles and weights.
  //! If the surface was rational it can become non-rational.
  //! If the surface was non-rational it can become rational.
  //! The length of CPoles can be lower but not greater than NbVPoles
  //! so you can modify just a part of the row.
  //! Raised if UIndex < 1 or  UIndex > NbUPoles
  //!
  //! Raised if CPoles.Lower() < 1 or CPoles.Upper() > NbVPoles
  //! Raised if CPoleWeights and CPoles have not the same bounds.
  //! Raised if one of the weight value CPoleWeights (i) is lower
  //! or equal to Resolution from gp.
  Standard_EXPORT void SetPoleRow (const Standard_Integer UIndex, const TColgp_Array1OfPnt& CPoles, const TColStd_Array1OfReal& CPoleWeights);
  

  //! Modifies the weight of the pole of range UIndex, VIndex.
  //! If the surface was non-rational it can become rational.
  //! If the surface was rational it can become non-rational.
  //!
  //! Raised if UIndex < 1  or  UIndex > NbUPoles or VIndex < 1 or
  //! VIndex > NbVPoles.
  //! Raised if Weight <= Resolution from package gp.
  Standard_EXPORT void SetWeight (const Standard_Integer UIndex, const Standard_Integer VIndex, const Standard_Real Weight);
  
  //! Modifies a column of weights.
  //! If the surface was rational it can become non-rational.
  //! If the surface was non-rational it can become rational.
  //! The length of CPoleWeights can be lower but not greater than
  //! NbUPoles.
  //! Raised if VIndex < 1 or  VIndex > NbVPoles
  //!
  //! Raised if CPoleWeights.Lower() < 1 or CPoleWeights.Upper() >
  //! NbUPoles
  //! Raised if one of the weight value CPoleWeights (i) is lower
  //! or equal to Resolution from package gp.
  Standard_EXPORT void SetWeightCol (const Standard_Integer VIndex, const TColStd_Array1OfReal& CPoleWeights);
  
  //! Modifies a row of weights.
  //! If the surface was rational it can become non-rational.
  //! If the surface was non-rational it can become rational.
  //! The length of CPoleWeights can be lower but not greater than
  //! NbVPoles.
  //! Raised if UIndex < 1 or  UIndex > NbUPoles
  //!
  //! Raised if CPoleWeights.Lower() < 1 or CPoleWeights.Upper() >
  //! NbVPoles
  //! Raised if one of the weight value CPoleWeights (i) is lower
  //! or equal to Resolution from package gp.
  Standard_EXPORT void SetWeightRow (const Standard_Integer UIndex, const TColStd_Array1OfReal& CPoleWeights);
  
  //! Changes the orientation of this Bezier surface in the
  //! u  parametric direction. The bounds of the
  //! surface are not changed, but the given parametric
  //! direction is reversed. Hence, the orientation of the surface is reversed.
  Standard_EXPORT void UReverse() Standard_OVERRIDE;
  
  //! Computes the u (or v) parameter on the modified
  //! surface, produced by reversing its u (or v) parametric
  //! direction, for any point of u parameter U (or of v
  //! parameter V) on this Bezier surface.
  //! In the case of a Bezier surface, these functions return respectively:
  //! - 1.-U, or 1.-V.
  Standard_EXPORT Standard_Real UReversedParameter (const Standard_Real U) const Standard_OVERRIDE;
  
  //! Changes the orientation of this Bezier surface in the
  //! v parametric direction. The bounds of the
  //! surface are not changed, but the given parametric
  //! direction is reversed. Hence, the orientation of the
  //! surface is reversed.
  Standard_EXPORT void VReverse() Standard_OVERRIDE;
  
  //! Computes the u (or v) parameter on the modified
  //! surface, produced by reversing its u (or v) parametric
  //! direction, for any point of u parameter U (or of v
  //! parameter V) on this Bezier surface.
  //! In the case of a Bezier surface, these functions return respectively:
  //! - 1.-U, or 1.-V.
  Standard_EXPORT Standard_Real VReversedParameter (const Standard_Real V) const Standard_OVERRIDE;
  
  //! Returns the parametric bounds U1, U2, V1 and V2 of
  //! this Bezier surface.
  //! In the case of a Bezier surface, this function returns
  //! U1 = 0, V1 = 0, U2 = 1, V2 = 1.
  Standard_EXPORT void Bounds (Standard_Real& U1, Standard_Real& U2, Standard_Real& V1, Standard_Real& V2) const Standard_OVERRIDE;
  

  //! Returns the continuity of the surface CN : the order of
  //! continuity is infinite.
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;
  
  Standard_EXPORT void D0 (const Standard_Real U, const Standard_Real V, gp_Pnt& P) const Standard_OVERRIDE;
  
  Standard_EXPORT void D1 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const Standard_OVERRIDE;
  
  Standard_EXPORT void D2 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV) const Standard_OVERRIDE;
  
  //! Computes P, the point of parameters (U, V) of this Bezier surface, and
  //! - one or more of the following sets of vectors:
  //! - D1U and D1V, the first derivative vectors at this point,
  //! - D2U, D2V and D2UV, the second derivative
  //! vectors at this point,
  //! - D3U, D3V, D3UUV and D3UVV, the third
  //! derivative vectors at this point.
  //! Note: The parameters U and V can be outside the bounds of the surface.
  Standard_EXPORT void D3 (const Standard_Real U, const Standard_Real V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V, gp_Vec& D2U, gp_Vec& D2V, gp_Vec& D2UV, gp_Vec& D3U, gp_Vec& D3V, gp_Vec& D3UUV, gp_Vec& D3UVV) const Standard_OVERRIDE;
  
  //! Computes the derivative of order Nu in the u
  //! parametric direction, and Nv in the v parametric
  //! direction, at the point of parameters (U, V) of this Bezier surface.
  //! Note: The parameters U and V can be outside the bounds of the surface.
  //! Exceptions
  //! Standard_RangeError if:
  //! - Nu + Nv is less than 1, or Nu or Nv is negative.
  Standard_EXPORT gp_Vec DN (const Standard_Real U, const Standard_Real V, const Standard_Integer Nu, const Standard_Integer Nv) const Standard_OVERRIDE;
  
  //! Returns the number of poles in the U direction.
  Standard_EXPORT Standard_Integer NbUPoles() const;
  
  //! Returns the number of poles in the V direction.
  Standard_EXPORT Standard_Integer NbVPoles() const;
  
  //! Returns the pole of range UIndex, VIndex
  //! Raised if UIndex < 1 or UIndex > NbUPoles, or
  //! VIndex < 1 or VIndex > NbVPoles.
  Standard_EXPORT const gp_Pnt& Pole(const Standard_Integer UIndex, const Standard_Integer VIndex) const;
  
  //! Returns the poles of the Bezier surface.
  //!
  //! Raised if the length of P in the U an V direction is not equal to
  //! NbUPoles and NbVPoles.
  Standard_EXPORT void Poles (TColgp_Array2OfPnt& P) const;

  //! Returns the poles of the Bezier surface.
  const TColgp_Array2OfPnt& Poles() const
  {
    return poles->Array2();
  }

  //! Returns the degree of the surface in the U direction it is
  //! NbUPoles - 1
  Standard_EXPORT Standard_Integer UDegree() const;
  

  //! Computes the U isoparametric curve. For a Bezier surface the
  //! UIso curve is a Bezier curve.
  Standard_EXPORT Handle(Geom_Curve) UIso (const Standard_Real U) const Standard_OVERRIDE;
  

  //! Returns the degree of the surface in the V direction it is
  //! NbVPoles - 1
  Standard_EXPORT Standard_Integer VDegree() const;
  

  //! Computes the V isoparametric curve. For a Bezier surface the
  //! VIso  curve is a Bezier curve.
  Standard_EXPORT Handle(Geom_Curve) VIso (const Standard_Real V) const Standard_OVERRIDE;
  
  //! Returns the weight of range UIndex, VIndex
  //!
  //! Raised if UIndex < 1 or UIndex > NbUPoles, or
  //! VIndex < 1 or VIndex > NbVPoles.
  Standard_EXPORT Standard_Real Weight (const Standard_Integer UIndex, const Standard_Integer VIndex) const;
  
  //! Returns the weights of the Bezier surface.
  //!
  //! Raised if the length of W in the U an V direction is not
  //! equal to NbUPoles and NbVPoles.
  Standard_EXPORT void Weights (TColStd_Array2OfReal& W) const;
  
  //! Returns the weights of the Bezier surface.
  const TColStd_Array2OfReal* Weights() const
  {
    if (!weights.IsNull())
      return &weights->Array2();
    return BSplSLib::NoWeights();
  }

  //! Returns True if the first control points row and the
  //! last control points row are identical. The tolerance
  //! criterion is Resolution from package gp.
  Standard_EXPORT Standard_Boolean IsUClosed() const Standard_OVERRIDE;
  

  //! Returns True if the first control points column
  //! and the last control points column are identical.
  //! The tolerance criterion is Resolution from package gp.
  Standard_EXPORT Standard_Boolean IsVClosed() const Standard_OVERRIDE;
  
  //! Returns True, a Bezier surface is always  CN
  Standard_EXPORT Standard_Boolean IsCNu (const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Returns True, a BezierSurface is always  CN
  Standard_EXPORT Standard_Boolean IsCNv (const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Returns False.
  Standard_EXPORT Standard_Boolean IsUPeriodic() const Standard_OVERRIDE;
  
  //! Returns False.
  Standard_EXPORT Standard_Boolean IsVPeriodic() const Standard_OVERRIDE;
  

  //! Returns False if the weights are identical in the U direction,
  //! The tolerance criterion is Resolution from package gp.
  //! Example :
  //! |1.0, 1.0, 1.0|
  //! if Weights =  |0.5, 0.5, 0.5|   returns False
  //! |2.0, 2.0, 2.0|
  Standard_EXPORT Standard_Boolean IsURational() const;
  

  //! Returns False if the weights are identical in the V direction,
  //! The tolerance criterion is Resolution from package gp.
  //! Example :
  //! |1.0, 2.0, 0.5|
  //! if Weights =  |1.0, 2.0, 0.5|   returns False
  //! |1.0, 2.0, 0.5|
  Standard_EXPORT Standard_Boolean IsVRational() const;
  
  //! Applies the transformation T to this Bezier surface.
  Standard_EXPORT void Transform (const gp_Trsf& T) Standard_OVERRIDE;
  

  //! Returns the value of the maximum polynomial degree of a
  //! Bezier surface. This value is 25.
  Standard_EXPORT static Standard_Integer MaxDegree();
  
  //! Computes two tolerance values for this Bezier
  //! surface, based on the given tolerance in 3D space
  //! Tolerance3D. The tolerances computed are:
  //! - UTolerance in the u parametric direction, and
  //! - VTolerance in the v parametric direction.
  //! If f(u,v) is the equation of this Bezier surface,
  //! UTolerance and VTolerance guarantee that:
  //! | u1 - u0 | < UTolerance and
  //! | v1 - v0 | < VTolerance
  //! ====> |f (u1,v1) - f (u0,v0)| < Tolerance3D
  Standard_EXPORT void Resolution (const Standard_Real Tolerance3D, Standard_Real& UTolerance, Standard_Real& VTolerance);
  
  //! Creates a new object which is a copy of this Bezier surface.
  Standard_EXPORT Handle(Geom_Geometry) Copy() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Geom_BezierSurface,Geom_BoundedSurface)

protected:




private:

  
  Geom_BezierSurface(const Handle(TColgp_HArray2OfPnt)& SurfacePoles, const Handle(TColStd_HArray2OfReal)& PoleWeights, const Standard_Boolean IsURational, const Standard_Boolean IsVRational);
  
  //! Set  poles  to  Poles,  weights to  Weights  (not
  //! copied).
  //! Create the arrays of coefficients.  Poles
  //! and    Weights  are   assumed   to  have the  first
  //! coefficient 1.
  //!
  //! if nbpoles < 2 or nbpoles > MaDegree
  void Init (const Handle(TColgp_HArray2OfPnt)& Poles, const Handle(TColStd_HArray2OfReal)& Weights);
  

  Standard_Boolean urational;
  Standard_Boolean vrational;
  Handle(TColgp_HArray2OfPnt) poles;
  Handle(TColStd_HArray2OfReal) weights;
  Standard_Real umaxderivinv;
  Standard_Real vmaxderivinv;
  Standard_Boolean maxderivinvok;


};







#endif // _Geom_BezierSurface_HeaderFile
