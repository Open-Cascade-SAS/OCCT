// Created on: 1993-04-01
// Created by: Modelistation
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

#ifndef _HLRBRep_Curve_HeaderFile
#define _HLRBRep_Curve_HeaderFile

#include <BRepAdaptor_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

class TopoDS_Edge;
class gp_Pnt;
class gp_Vec;
class gp_Pnt2d;
class gp_Dir2d;
class gp_Vec2d;
class gp_Lin2d;
class gp_Circ2d;
class gp_Elips2d;
class gp_Hypr2d;
class gp_Parab2d;
class Geom_BSplineCurve;
class HLRAlgo_Projector;

//! Defines a 2d curve by projection of  a 3D curve on
//! a    plane     with  an     optional   perspective
//! transformation.
class HLRBRep_Curve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an undefined Curve.
  Standard_EXPORT HLRBRep_Curve();

  void Projector(const HLRAlgo_Projector* Proj) { myProj = Proj; }

  //! Returns the 3D curve.
  BRepAdaptor_Curve& Curve();

  //! Sets the 3D curve to be projected.
  Standard_EXPORT void Curve(const TopoDS_Edge& E);

  //! Returns the 3D curve.
  const BRepAdaptor_Curve& GetCurve() const;

  //! Returns the parameter on the 2d curve from the
  //! parameter on the 3d curve.
  Standard_EXPORT double Parameter2d(const double P3d) const;

  //! Returns the parameter on the 3d curve from the
  //! parameter on the 2d curve.
  Standard_EXPORT double Parameter3d(const double P2d) const;

  //! Update the minmax and the internal data
  Standard_EXPORT double Update(double TotMin[16], double TotMax[16]);

  //! Update the minmax returns tol for enlarge;
  Standard_EXPORT double UpdateMinMax(double TotMin[16], double TotMax[16]);

  //! Computes the Z coordinate of the point of
  //! parameter U on the curve in the viewing coordinate system
  Standard_EXPORT double Z(const double U) const;

  //! Computes the 3D point of parameter U on the
  //! curve.
  gp_Pnt Value3D(const double U) const;

  //! Computes the 3D point of parameter U on the
  //! curve.
  void D0(const double U, gp_Pnt& P) const;

  //! Computes the point of parameter U on the curve
  //! with its first derivative.
  void D1(const double U, gp_Pnt& P, gp_Vec& V) const;

  //! Depending on <AtStart> computes the 2D point and
  //! tangent on the curve at sart (or at end). If the first
  //! derivative is null look after at start (or before at end)
  //! with the second derivative.
  Standard_EXPORT void Tangent(const bool AtStart, gp_Pnt2d& P, gp_Dir2d& D) const;

  double FirstParameter() const;

  double LastParameter() const;

  GeomAbs_Shape Continuity() const;

  //! If necessary, breaks the curve in intervals of
  //! continuity <S>. And returns the number of
  //! intervals.
  int NbIntervals(const GeomAbs_Shape S) const;

  //! Stores in <T> the parameters bounding the intervals
  //! of continuity <S>.
  //!
  //! The array must provide enough room to accommodate
  //! for the parameters. i.e. T.Length() > NbIntervals()
  void Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const;

  bool IsClosed() const;

  bool IsPeriodic() const;

  double Period() const;

  //! Computes the point of parameter U on the curve.
  gp_Pnt2d Value(const double U) const;

  //! Computes the point of parameter U on the curve.
  Standard_EXPORT void D0(const double U, gp_Pnt2d& P) const;

  //! Computes the point of parameter U on the curve
  //! with its first derivative.
  //! Raised if the continuity of the current interval
  //! is not C1.
  Standard_EXPORT void D1(const double U, gp_Pnt2d& P, gp_Vec2d& V) const;

  //! Raised if the continuity of the current interval
  //! is not C2.
  Standard_EXPORT void D2(const double U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const;

  //! Returns the point P of parameter U, the first, the second
  //! and the third derivative.
  //! Raised if the continuity of the current interval
  //! is not C3.
  Standard_EXPORT void D3(const double U,
                          gp_Pnt2d&    P,
                          gp_Vec2d&    V1,
                          gp_Vec2d&    V2,
                          gp_Vec2d&    V3) const;

  //! The returned vector gives the value of the derivative for the
  //! order of derivation N.
  //! Raised if the continuity of the current interval
  //! is not CN.
  //! Raised if N < 1.
  Standard_EXPORT gp_Vec2d DN(const double U, const int N) const;

  //! Returns the parametric resolution corresponding
  //! to the real space resolution <R3d>.
  double Resolution(const double R3d) const;

  //! Returns the type of the curve in the current
  //! interval: Line, Circle, Ellipse, Hyperbola,
  //! Parabola, BezierCurve, BSplineCurve, OtherCurve.
  GeomAbs_CurveType GetType() const;

  Standard_EXPORT gp_Lin2d Line() const;

  Standard_EXPORT gp_Circ2d Circle() const;

  Standard_EXPORT gp_Elips2d Ellipse() const;

  Standard_EXPORT gp_Hypr2d Hyperbola() const;

  Standard_EXPORT gp_Parab2d Parabola() const;

  bool IsRational() const;

  int Degree() const;

  int NbPoles() const;

  Standard_EXPORT void Poles(NCollection_Array1<gp_Pnt2d>& TP) const;

  Standard_EXPORT void Poles(const occ::handle<Geom_BSplineCurve>& aCurve,
                             NCollection_Array1<gp_Pnt2d>&         TP) const;

  Standard_EXPORT void PolesAndWeights(NCollection_Array1<gp_Pnt2d>& TP,
                                       NCollection_Array1<double>&   TW) const;

  Standard_EXPORT void PolesAndWeights(const occ::handle<Geom_BSplineCurve>& aCurve,
                                       NCollection_Array1<gp_Pnt2d>&         TP,
                                       NCollection_Array1<double>&           TW) const;

  int NbKnots() const;

  Standard_EXPORT void Knots(NCollection_Array1<double>& kn) const;

  Standard_EXPORT void Multiplicities(NCollection_Array1<int>& mu) const;

private:
  BRepAdaptor_Curve        myCurve;
  GeomAbs_CurveType        myType;
  const HLRAlgo_Projector* myProj;
  double                   myOX;
  double                   myOZ;
  double                   myVX;
  double                   myVZ;
  double                   myOF;
};

#include <HLRBRep_Curve.lxx>

#endif // _HLRBRep_Curve_HeaderFile
