// Created on: 1993-04-14
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

#ifndef _HLRBRep_Surface_HeaderFile
#define _HLRBRep_Surface_HeaderFile

#include <HLRBRep_Curve.hxx>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <BRepAdaptor_Surface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array2.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Vec.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Ax1.hxx>
class TopoDS_Face;
class gp_Pnt;
class gp_Vec;
class gp_Pln;

class HLRBRep_Surface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an undefined surface with no face loaded.
  Standard_EXPORT HLRBRep_Surface();

  void Projector(const HLRAlgo_Projector* Proj) { myProj = Proj; }

  //! Returns the 3D Surface.
  BRepAdaptor_Surface& Surface();

  //! Returns the 3D Surface (const version).
  const BRepAdaptor_Surface& Surface() const;

  //! Sets the 3D Surface to be projected.
  Standard_EXPORT void Surface(const TopoDS_Face& F);

  //! returns true if it is a side face
  Standard_EXPORT bool IsSide(const double tolf, const double toler) const;

  Standard_EXPORT bool IsAbove(const bool back, const HLRBRep_Curve* A, const double tolC) const;

  double FirstUParameter() const;

  double LastUParameter() const;

  double FirstVParameter() const;

  double LastVParameter() const;

  GeomAbs_Shape UContinuity() const;

  GeomAbs_Shape VContinuity() const;

  //! If necessary, breaks the surface in U intervals of
  //! continuity <S>. And returns the number of
  //! intervals.
  int NbUIntervals(const GeomAbs_Shape S) const;

  //! If necessary, breaks the surface in V intervals of
  //! continuity <S>. And returns the number of
  //! intervals.
  int NbVIntervals(const GeomAbs_Shape S) const;

  GeomAbs_Shape UIntervalContinuity() const;

  GeomAbs_Shape VIntervalContinuity() const;

  bool IsUClosed() const;

  bool IsVClosed() const;

  bool IsUPeriodic() const;

  double UPeriod() const;

  bool IsVPeriodic() const;

  double VPeriod() const;

  //! Computes the point of parameters U,V on the surface.
  Standard_EXPORT gp_Pnt Value(const double U, const double V) const;

  //! Computes the point of parameters U,V on the surface.
  void D0(const double U, const double V, gp_Pnt& P) const;

  //! Computes the point and the first derivatives on
  //! the surface.
  //! Raised if the continuity of the current
  //! intervals is not C1.
  void D1(const double U, const double V, gp_Pnt& P, gp_Vec& D1U, gp_Vec& D1V) const;

  //! Computes the point, the first and second
  //! derivatives on the surface.
  //! Raised if the continuity of the current
  //! intervals is not C2.
  void D2(const double U,
          const double V,
          gp_Pnt&      P,
          gp_Vec&      D1U,
          gp_Vec&      D1V,
          gp_Vec&      D2U,
          gp_Vec&      D2V,
          gp_Vec&      D2UV) const;

  //! Computes the point, the first, second and third
  //! derivatives on the surface.
  //! Raised if the continuity of the current
  //! intervals is not C3.
  void D3(const double U,
          const double V,
          gp_Pnt&      P,
          gp_Vec&      D1U,
          gp_Vec&      D1V,
          gp_Vec&      D2U,
          gp_Vec&      D2V,
          gp_Vec&      D2UV,
          gp_Vec&      D3U,
          gp_Vec&      D3V,
          gp_Vec&      D3UUV,
          gp_Vec&      D3UVV) const;

  //! Computes the derivative of order Nu in the
  //! direction U and Nv in the direction V at the point P(U,
  //! V).
  //! Raised if the current U interval is not not CNu
  //! and the current V interval is not CNv.
  //! Raised if Nu + Nv < 1 or Nu < 0 or Nv < 0.
  gp_Vec DN(const double U, const double V, const int Nu, const int Nv) const;

  //! Returns the type of the surface : Plane, Cylinder,
  //! Cone, Sphere, Torus, BezierSurface,
  //! BSplineSurface, SurfaceOfRevolution,
  //! SurfaceOfExtrusion, OtherSurface
  GeomAbs_SurfaceType GetType() const;

  Standard_EXPORT gp_Pln Plane() const;

  gp_Cylinder Cylinder() const;

  gp_Cone Cone() const;

  gp_Sphere Sphere() const;

  gp_Torus Torus() const;

  int UDegree() const;

  int NbUPoles() const;

  int VDegree() const;

  int NbVPoles() const;

  int NbUKnots() const;

  int NbVKnots() const;

  gp_Ax1 Axis() const;

private:
  //! returns true if it is a side face
  Standard_EXPORT bool SideRowsOfPoles(const double                tol,
                                       const int                   nbuPoles,
                                       const int                   nbvPoles,
                                       NCollection_Array2<gp_Pnt>& Pnt) const;

  BRepAdaptor_Surface      mySurf;
  GeomAbs_SurfaceType      myType;
  const HLRAlgo_Projector* myProj;
};

#include <HLRBRep_Surface.lxx>

#endif // _HLRBRep_Surface_HeaderFile
