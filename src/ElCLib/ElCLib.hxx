// Created on: 1991-09-10
// Created by: Michel Chauvat
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _ElCLib_HeaderFile
#define _ElCLib_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
class gp_Pnt;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Hypr;
class gp_Parab;
class gp_Vec;
class gp_Lin2d;
class gp_Circ2d;
class gp_Elips2d;
class gp_Hypr2d;
class gp_Parab2d;
class gp_Pnt2d;
class gp_Vec2d;
class gp_Ax1;
class gp_Ax2;
class gp_Ax2d;
class gp_Ax22d;
class gp_Dir;
class gp_Dir2d;


//! Provides functions for basic geometric computations on
//! elementary curves such as conics and lines in 2D and 3D space.
//! This includes:
//! -   calculation of a point or derived vector on a 2D or
//! 3D curve where:
//! -   the curve is provided by the gp package, or
//! defined in reference form (as in the gp package),
//! and
//! -   the point is defined by a parameter,
//! -   evaluation of the parameter corresponding to a point
//! on a 2D or 3D curve from gp,
//! -   various elementary computations which allow you to
//! position parameterized values within the period of a curve.
//! Notes:
//! -   ElCLib stands for Elementary Curves Library.
//! -   If the curves provided by the gp package are not
//! explicitly parameterized, they still have an implicit
//! parameterization, analogous to that which they infer
//! for the equivalent Geom or Geom2d curves.
class ElCLib 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Return a value in   the  range <UFirst, ULast>  by
  //! adding or removing the period <ULast -  UFirst> to
  //! <U>.
  //! ATTENTION!!!
  //!   It is expected but not checked that (ULast > UFirst)
  Standard_EXPORT static Standard_Real InPeriod (const Standard_Real U, const Standard_Real UFirst, const Standard_Real ULast);
  
  //! Adjust U1 and  U2 in the  parametric range  UFirst
  //! Ulast of a periodic curve, where ULast -
  //! UFirst is its period. To do this, this function:
  //! -   sets U1 in the range [ UFirst, ULast ] by
  //! adding/removing the period to/from the value U1, then
  //! -   sets U2 in the range [ U1, U1 + period ] by
  //! adding/removing the period to/from the value U2.
  //! Precision is used to test the equalities.
  Standard_EXPORT static void AdjustPeriodic (const Standard_Real UFirst, const Standard_Real ULast, const Standard_Real Precision, Standard_Real& U1, Standard_Real& U2);
  
  //! For elementary curves (lines, circles and conics) from
  //! the gp package, computes the point of parameter U.
  //! The result is either:
  //! -   a gp_Pnt point for a curve in 3D space, or
  //! -   a gp_Pnt2d point for a curve in 2D space.
  static gp_Pnt Value (const Standard_Real U, const gp_Lin& L);
  
  static gp_Pnt Value (const Standard_Real U, const gp_Circ& C);
  
  static gp_Pnt Value (const Standard_Real U, const gp_Elips& E);
  
  static gp_Pnt Value (const Standard_Real U, const gp_Hypr& H);
  
  static gp_Pnt Value (const Standard_Real U, const gp_Parab& Prb);

  //! For elementary curves (lines, circles and conics) from the
  //! gp package, computes:
  //! -   the point P of parameter U, and
  //! -   the first derivative vector V1 at this point.
  //! The results P and V1 are either:
  //! -   a gp_Pnt point and a gp_Vec vector, for a curve in 3D  space, or
  //! -   a gp_Pnt2d point and a gp_Vec2d vector, for a curve in 2D space.
  static void D1 (const Standard_Real U, const gp_Lin& L, gp_Pnt& P, gp_Vec& V1);
  
  static void D1 (const Standard_Real U, const gp_Circ& C, gp_Pnt& P, gp_Vec& V1);
  
  static void D1 (const Standard_Real U, const gp_Elips& E, gp_Pnt& P, gp_Vec& V1);
  
  static void D1 (const Standard_Real U, const gp_Hypr& H, gp_Pnt& P, gp_Vec& V1);
  
  static void D1 (const Standard_Real U, const gp_Parab& Prb, gp_Pnt& P, gp_Vec& V1);
  
  //! For elementary curves (circles and conics) from the gp
  //! package, computes:
  //! - the point P of parameter U, and
  //! - the first and second derivative vectors V1 and V2 at this point.
  //! The results, P, V1 and V2, are either:
  //! -   a gp_Pnt point and two gp_Vec vectors, for a curve in 3D space, or
  //! -   a gp_Pnt2d point and two gp_Vec2d vectors, for a curve in 2D space.
  static void D2 (const Standard_Real U, const gp_Circ& C, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);
  
  static void D2 (const Standard_Real U, const gp_Elips& E, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);
  
  static void D2 (const Standard_Real U, const gp_Hypr& H, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);
  
  static void D2 (const Standard_Real U, const gp_Parab& Prb, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);
  
  //! For elementary curves (circles, ellipses and hyperbolae)
  //! from the gp package, computes:
  //! -   the point P of parameter U, and
  //! -   the first, second and third derivative vectors V1, V2
  //! and V3 at this point.
  //! The results, P, V1, V2 and V3, are either:
  //! -   a gp_Pnt point and three gp_Vec vectors, for a curve in 3D space, or
  //! -   a gp_Pnt2d point and three gp_Vec2d vectors, for a curve in 2D space.
  static void D3 (const Standard_Real U, const gp_Circ& C, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3);
  
  static void D3 (const Standard_Real U, const gp_Elips& E, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3);
  
  static void D3 (const Standard_Real U, const gp_Hypr& H, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3);

  //! For elementary curves (lines, circles and conics) from
  //! the gp package, computes the vector corresponding to
  //! the Nth derivative at the point of parameter U. The result is either:
  //! -   a gp_Vec vector for a curve in 3D space, or
  //! -   a gp_Vec2d vector for a curve in 2D space.
  //! In the following functions N is the order of derivation
  //! and should be greater than 0
  static gp_Vec DN (const Standard_Real U, const gp_Lin& L, const Standard_Integer N);
  
  static gp_Vec DN (const Standard_Real U, const gp_Circ& C, const Standard_Integer N);
  
  static gp_Vec DN (const Standard_Real U, const gp_Elips& E, const Standard_Integer N);
  
  static gp_Vec DN (const Standard_Real U, const gp_Hypr& H, const Standard_Integer N);
  
  static gp_Vec DN (const Standard_Real U, const gp_Parab& Prb, const Standard_Integer N);
  
  static gp_Pnt2d Value (const Standard_Real U, const gp_Lin2d& L);
  
  static gp_Pnt2d Value (const Standard_Real U, const gp_Circ2d& C);
  
  static gp_Pnt2d Value (const Standard_Real U, const gp_Elips2d& E);
  
  static gp_Pnt2d Value (const Standard_Real U, const gp_Hypr2d& H);
  
  static gp_Pnt2d Value (const Standard_Real U, const gp_Parab2d& Prb);
  
  static void D1 (const Standard_Real U, const gp_Lin2d& L, gp_Pnt2d& P, gp_Vec2d& V1);

  static void D1 (const Standard_Real U, const gp_Circ2d& C, gp_Pnt2d& P, gp_Vec2d& V1);

  static void D1 (const Standard_Real U, const gp_Elips2d& E, gp_Pnt2d& P, gp_Vec2d& V1);

  static void D1 (const Standard_Real U, const gp_Hypr2d& H, gp_Pnt2d& P, gp_Vec2d& V1);

  static void D1 (const Standard_Real U, const gp_Parab2d& Prb, gp_Pnt2d& P, gp_Vec2d& V1);

  static void D2 (const Standard_Real U, const gp_Circ2d& C, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2);

  static void D2 (const Standard_Real U, const gp_Elips2d& E, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2);

  static void D2 (const Standard_Real U, const gp_Hypr2d& H, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2);

  static void D2 (const Standard_Real U, const gp_Parab2d& Prb, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2);

  static void D3 (const Standard_Real U, const gp_Circ2d& C, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3);

  static void D3 (const Standard_Real U, const gp_Elips2d& E, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3);

  //! In the following functions N is the order of derivation
  //! and should be greater than 0
    static void D3 (const Standard_Real U, const gp_Hypr2d& H, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3);
  
    static gp_Vec2d DN (const Standard_Real U, const gp_Lin2d& L, const Standard_Integer N);
  
    static gp_Vec2d DN (const Standard_Real U, const gp_Circ2d& C, const Standard_Integer N);
  
    static gp_Vec2d DN (const Standard_Real U, const gp_Elips2d& E, const Standard_Integer N);
  
    static gp_Vec2d DN (const Standard_Real U, const gp_Hypr2d& H, const Standard_Integer N);
  
    static gp_Vec2d DN (const Standard_Real U, const gp_Parab2d& Prb, const Standard_Integer N);
  
  //! Curve evaluation
  //! The following basis functions compute the derivatives on
  //! elementary curves defined by their geometric characteristics.
  //! These functions can be called without constructing a conic
  //! from package gp. They are called by the previous functions.
  //! Example :
  //! A circle is defined by its position and its radius.
  Standard_EXPORT static gp_Pnt LineValue (const Standard_Real U, const gp_Ax1& Pos);
  
  Standard_EXPORT static gp_Pnt CircleValue (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real Radius);
  
  Standard_EXPORT static gp_Pnt EllipseValue (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  Standard_EXPORT static gp_Pnt HyperbolaValue (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  Standard_EXPORT static gp_Pnt ParabolaValue (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real Focal);
  
  Standard_EXPORT static void LineD1 (const Standard_Real U, const gp_Ax1& Pos, gp_Pnt& P, gp_Vec& V1);
  
  Standard_EXPORT static void CircleD1 (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real Radius, gp_Pnt& P, gp_Vec& V1);
  
  Standard_EXPORT static void EllipseD1 (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt& P, gp_Vec& V1);
  
  Standard_EXPORT static void HyperbolaD1 (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt& P, gp_Vec& V1);
  
  Standard_EXPORT static void ParabolaD1 (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real Focal, gp_Pnt& P, gp_Vec& V1);
  
  Standard_EXPORT static void CircleD2 (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real Radius, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);
  
  Standard_EXPORT static void EllipseD2 (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);
  
  Standard_EXPORT static void HyperbolaD2 (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);
  
  Standard_EXPORT static void ParabolaD2 (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real Focal, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2);
  
  Standard_EXPORT static void CircleD3 (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real Radius, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3);
  
  Standard_EXPORT static void EllipseD3 (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3);
  
  Standard_EXPORT static void HyperbolaD3 (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3);
  

  //! In the following functions N is the order of derivation
  //! and should be greater than 0
  Standard_EXPORT static gp_Vec LineDN (const Standard_Real U, const gp_Ax1& Pos, const Standard_Integer N);
  
  Standard_EXPORT static gp_Vec CircleDN (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real Radius, const Standard_Integer N);
  
  Standard_EXPORT static gp_Vec EllipseDN (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const Standard_Integer N);
  
  Standard_EXPORT static gp_Vec HyperbolaDN (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const Standard_Integer N);
  
  Standard_EXPORT static gp_Vec ParabolaDN (const Standard_Real U, const gp_Ax2& Pos, const Standard_Real Focal, const Standard_Integer N);
  
  Standard_EXPORT static gp_Pnt2d LineValue (const Standard_Real U, const gp_Ax2d& Pos);
  
  Standard_EXPORT static gp_Pnt2d CircleValue (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real Radius);
  
  Standard_EXPORT static gp_Pnt2d EllipseValue (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  Standard_EXPORT static gp_Pnt2d HyperbolaValue (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius);
  
  Standard_EXPORT static gp_Pnt2d ParabolaValue (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real Focal);
  
  Standard_EXPORT static void LineD1 (const Standard_Real U, const gp_Ax2d& Pos, gp_Pnt2d& P, gp_Vec2d& V1);
  
  Standard_EXPORT static void CircleD1 (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real Radius, gp_Pnt2d& P, gp_Vec2d& V1);
  
  Standard_EXPORT static void EllipseD1 (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt2d& P, gp_Vec2d& V1);
  
  Standard_EXPORT static void HyperbolaD1 (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt2d& P, gp_Vec2d& V1);
  
  Standard_EXPORT static void ParabolaD1 (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real Focal, gp_Pnt2d& P, gp_Vec2d& V1);
  
  Standard_EXPORT static void CircleD2 (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real Radius, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2);
  
  Standard_EXPORT static void EllipseD2 (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2);
  
  Standard_EXPORT static void HyperbolaD2 (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2);
  
  Standard_EXPORT static void ParabolaD2 (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real Focal, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2);
  
  Standard_EXPORT static void CircleD3 (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real Radius, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3);
  
  Standard_EXPORT static void EllipseD3 (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3);
  

  //! In the following functions N is the order of derivation
  //! and should be greater than 0
  Standard_EXPORT static void HyperbolaD3 (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3);
  
  Standard_EXPORT static gp_Vec2d LineDN (const Standard_Real U, const gp_Ax2d& Pos, const Standard_Integer N);
  
  Standard_EXPORT static gp_Vec2d CircleDN (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real Radius, const Standard_Integer N);
  
  Standard_EXPORT static gp_Vec2d EllipseDN (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const Standard_Integer N);
  
  Standard_EXPORT static gp_Vec2d HyperbolaDN (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const Standard_Integer N);
  

  //! The following functions compute the parametric value corresponding
  //! to a given point on a elementary curve. The point should be on the
  //! curve.
  Standard_EXPORT static gp_Vec2d ParabolaDN (const Standard_Real U, const gp_Ax22d& Pos, const Standard_Real Focal, const Standard_Integer N);
  

  //! Computes the parameter value of the point P on the given curve.
  //! Note: In its local coordinate system, the parametric
  //! equation of the curve is given by the following:
  //! -   for the line L: P(U) = Po + U*Vo
  //! where Po is the origin and Vo the unit vector of its positioning axis.
  //! -   for the circle C: X(U) = Radius*Cos(U), Y(U) = Radius*Sin(U)
  //! -   for the ellipse E: X(U) = MajorRadius*Cos(U). Y(U) = MinorRadius*Sin(U)
  //! -   for the hyperbola H: X(U) = MajorRadius*Ch(U), Y(U) = MinorRadius*Sh(U)
  //! -   for the parabola Prb:
  //! X(U) = U**2 / (2*p)
  //! Y(U) = U
  //! where p is the distance between the focus and the directrix.
  //! Warning
  //! The point P must be on the curve. These functions are
  //! not protected, however, and if point P is not on the
  //! curve, an exception may be raised.
  static Standard_Real Parameter (const gp_Lin& L, const gp_Pnt& P);
  
  //! parametrization
  //! P (U) = L.Location() + U * L.Direction()
    static Standard_Real Parameter (const gp_Lin2d& L, const gp_Pnt2d& P);
  
    static Standard_Real Parameter (const gp_Circ& C, const gp_Pnt& P);
  
  //! parametrization
  //! In the local coordinate system of the circle
  //! X (U) = Radius * Cos (U)
  //! Y (U) = Radius * Sin (U)
    static Standard_Real Parameter (const gp_Circ2d& C, const gp_Pnt2d& P);
  
    static Standard_Real Parameter (const gp_Elips& E, const gp_Pnt& P);
  
  //! parametrization
  //! In the local coordinate system of the Ellipse
  //! X (U) = MajorRadius * Cos (U)
  //! Y (U) = MinorRadius * Sin (U)
    static Standard_Real Parameter (const gp_Elips2d& E, const gp_Pnt2d& P);
  
    static Standard_Real Parameter (const gp_Hypr& H, const gp_Pnt& P);
  
  //! parametrization
  //! In the local coordinate system of the Hyperbola
  //! X (U) = MajorRadius * Ch (U)
  //! Y (U) = MinorRadius * Sh (U)
    static Standard_Real Parameter (const gp_Hypr2d& H, const gp_Pnt2d& P);
  
    static Standard_Real Parameter (const gp_Parab& Prb, const gp_Pnt& P);
  
  //! parametrization
  //! In the local coordinate system of the parabola
  //! Y**2 = (2*P) * X where P is the distance between the focus
  //! and the directrix.
    static Standard_Real Parameter (const gp_Parab2d& Prb, const gp_Pnt2d& P);
  
  Standard_EXPORT static Standard_Real LineParameter (const gp_Ax1& Pos, const gp_Pnt& P);
  
  //! parametrization
  //! P (U) = L.Location() + U * L.Direction()
  Standard_EXPORT static Standard_Real LineParameter (const gp_Ax2d& Pos, const gp_Pnt2d& P);
  
  Standard_EXPORT static Standard_Real CircleParameter (const gp_Ax2& Pos, const gp_Pnt& P);
  
  //! Pos is the Axis of the Circle
  //! parametrization
  //! In the local coordinate system of the circle
  //! X (U) = Radius * Cos (U)
  //! Y (U) = Radius * Sin (U)
  Standard_EXPORT static Standard_Real CircleParameter (const gp_Ax22d& Pos, const gp_Pnt2d& P);
  
  Standard_EXPORT static Standard_Real EllipseParameter (const gp_Ax2& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const gp_Pnt& P);
  
  //! Pos is the Axis of the Ellipse
  //! parametrization
  //! In the local coordinate system of the Ellipse
  //! X (U) = MajorRadius * Cos (U)
  //! Y (U) = MinorRadius * Sin (U)
  Standard_EXPORT static Standard_Real EllipseParameter (const gp_Ax22d& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const gp_Pnt2d& P);
  
  Standard_EXPORT static Standard_Real HyperbolaParameter (const gp_Ax2& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const gp_Pnt& P);
  
  //! Pos is the Axis of the Hyperbola
  //! parametrization
  //! In the local coordinate system of the Hyperbola
  //! X (U) = MajorRadius * Ch (U)
  //! Y (U) = MinorRadius * Sh (U)
  Standard_EXPORT static Standard_Real HyperbolaParameter (const gp_Ax22d& Pos, const Standard_Real MajorRadius, const Standard_Real MinorRadius, const gp_Pnt2d& P);
  
  Standard_EXPORT static Standard_Real ParabolaParameter (const gp_Ax2& Pos, const gp_Pnt& P);
  
  //! Pos is the mirror axis of the parabola
  //! parametrization
  //! In the local coordinate system of the parabola
  //! Y**2 = (2*P) * X where P is the distance between the focus
  //! and the directrix.
  //! The following functions build  a 3d curve from a
  //! 2d curve at a given position defined with an Ax2.
  Standard_EXPORT static Standard_Real ParabolaParameter (const gp_Ax22d& Pos, const gp_Pnt2d& P);
  
  Standard_EXPORT static gp_Pnt To3d (const gp_Ax2& Pos, const gp_Pnt2d& P);
  
  Standard_EXPORT static gp_Vec To3d (const gp_Ax2& Pos, const gp_Vec2d& V);
  
  Standard_EXPORT static gp_Dir To3d (const gp_Ax2& Pos, const gp_Dir2d& V);
  
  Standard_EXPORT static gp_Ax1 To3d (const gp_Ax2& Pos, const gp_Ax2d& A);
  
  Standard_EXPORT static gp_Ax2 To3d (const gp_Ax2& Pos, const gp_Ax22d& A);
  
  Standard_EXPORT static gp_Lin To3d (const gp_Ax2& Pos, const gp_Lin2d& L);
  
  Standard_EXPORT static gp_Circ To3d (const gp_Ax2& Pos, const gp_Circ2d& C);
  
  Standard_EXPORT static gp_Elips To3d (const gp_Ax2& Pos, const gp_Elips2d& E);
  
  Standard_EXPORT static gp_Hypr To3d (const gp_Ax2& Pos, const gp_Hypr2d& H);
  

  //! These functions build a 3D geometric entity from a 2D geometric entity.
  //! The "X Axis" and the "Y Axis" of the global coordinate
  //! system (i.e. 2D space) are lined up respectively with the
  //! "X Axis" and "Y Axis" of the 3D coordinate system, Pos.
  Standard_EXPORT static gp_Parab To3d (const gp_Ax2& Pos, const gp_Parab2d& Prb);




protected:





private:





};


#include <ElCLib.lxx>





#endif // _ElCLib_HeaderFile
