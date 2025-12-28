// Created on: 1993-07-23
// Created by: Remi LEQUETTE
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

#include <BRep_Builder.hxx>
#include <BRepLib.hxx>
#include <BRepLib_FindSurface.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace()
    : myError(BRepLib_NoFace)
{
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const TopoDS_Face& F)
{
  Init(F);
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Pln& P)
{
  occ::handle<Geom_Plane> GP = new Geom_Plane(P);
  Init(GP, true, Precision::Confusion());
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Cylinder& C)
{
  occ::handle<Geom_CylindricalSurface> GC = new Geom_CylindricalSurface(C);
  Init(GC, true, Precision::Confusion());
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Cone& C)
{
  occ::handle<Geom_ConicalSurface> GC = new Geom_ConicalSurface(C);
  Init(GC, true, Precision::Confusion());
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Sphere& S)
{
  occ::handle<Geom_SphericalSurface> GS = new Geom_SphericalSurface(S);
  Init(GS, true, Precision::Confusion());
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Torus& T)
{
  occ::handle<Geom_ToroidalSurface> GT = new Geom_ToroidalSurface(T);
  Init(GT, true, Precision::Confusion());
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const occ::handle<Geom_Surface>& S, const double TolDegen)
{
  Init(S, true, TolDegen);
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Pln& P,
                                   const double  UMin,
                                   const double  UMax,
                                   const double  VMin,
                                   const double  VMax)
{
  occ::handle<Geom_Plane> GP = new Geom_Plane(P);
  Init(GP, UMin, UMax, VMin, VMax, Precision::Confusion());
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Cylinder& C,
                                   const double       UMin,
                                   const double       UMax,
                                   const double       VMin,
                                   const double       VMax)
{
  occ::handle<Geom_CylindricalSurface> GC = new Geom_CylindricalSurface(C);
  Init(GC, UMin, UMax, VMin, VMax, Precision::Confusion());
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Cone& C,
                                   const double   UMin,
                                   const double   UMax,
                                   const double   VMin,
                                   const double   VMax)
{
  occ::handle<Geom_ConicalSurface> GC = new Geom_ConicalSurface(C);
  Init(GC, UMin, UMax, VMin, VMax, Precision::Confusion());
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Sphere& S,
                                   const double     UMin,
                                   const double     UMax,
                                   const double     VMin,
                                   const double     VMax)
{
  occ::handle<Geom_SphericalSurface> GS = new Geom_SphericalSurface(S);
  Init(GS, UMin, UMax, VMin, VMax, Precision::Confusion());
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Torus& T,
                                   const double    UMin,
                                   const double    UMax,
                                   const double    VMin,
                                   const double    VMax)
{
  occ::handle<Geom_ToroidalSurface> GT = new Geom_ToroidalSurface(T);
  Init(GT, UMin, UMax, VMin, VMax, Precision::Confusion());
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const occ::handle<Geom_Surface>& S,
                                   const double                     UMin,
                                   const double                     UMax,
                                   const double                     VMin,
                                   const double                     VMax,
                                   const double                     TolDegen)
{
  Init(S, UMin, UMax, VMin, VMax, TolDegen);
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const TopoDS_Wire& W, const bool OnlyPlane)

{
  // Find a surface through the wire
  BRepLib_FindSurface FS(W, -1, OnlyPlane, true);
  if (!FS.Found())
  {
    myError = BRepLib_NotPlanar;
    return;
  }

  // build the face and add the wire
  BRep_Builder B;
  myError = BRepLib_FaceDone;

  double tol = std::max(1.2 * FS.ToleranceReached(), FS.Tolerance());

  B.MakeFace(TopoDS::Face(myShape), FS.Surface(), FS.Location(), tol);

  TopoDS_Wire aW;
  if (OnlyPlane)
  {
    // get rid of degenerative edges in the input wire
    BRep_Builder aB;
    aB.MakeWire(aW);

    TopoDS_Wire aWForw         = W;
    bool        hasDegenerated = false;
    aWForw.Orientation(TopAbs_FORWARD);
    TopoDS_Iterator anIter(aWForw);
    for (; anIter.More(); anIter.Next())
    {
      const TopoDS_Edge& aE = TopoDS::Edge(anIter.Value());

      if (BRep_Tool::Degenerated(aE))
      {
        hasDegenerated = true;
      }
      else
      {
        aB.Add(aW, aE);
      }
    }

    if (hasDegenerated)
    {
      aW.Orientation(W.Orientation()); // return to original orient
      aW.Closed(W.Closed());
    }
    else
    {
      aW = W;
    }
  }
  else
  {
    aW = W;
  }

  Add(aW);
  //
  BRepLib::UpdateTolerances(myShape);
  //
  BRepLib::SameParameter(myShape, tol, true);
  //
  if (BRep_Tool::IsClosed(aW))
    CheckInside();
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Pln& P, const TopoDS_Wire& W, const bool Inside)
{
  occ::handle<Geom_Plane> Pl = new Geom_Plane(P);
  Init(Pl, false, Precision::Confusion());
  Add(W);
  if (Inside && BRep_Tool::IsClosed(W))
    CheckInside();
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Cylinder& C, const TopoDS_Wire& W, const bool Inside)
{
  occ::handle<Geom_CylindricalSurface> GC = new Geom_CylindricalSurface(C);
  Init(GC, false, Precision::Confusion());
  Add(W);
  if (Inside && BRep_Tool::IsClosed(W))
    CheckInside();
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Cone& C, const TopoDS_Wire& W, const bool Inside)
{
  occ::handle<Geom_ConicalSurface> GC = new Geom_ConicalSurface(C);
  Init(GC, false, Precision::Confusion());
  Add(W);
  if (Inside && BRep_Tool::IsClosed(W))
    CheckInside();
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Sphere& S, const TopoDS_Wire& W, const bool Inside)
{
  occ::handle<Geom_SphericalSurface> GS = new Geom_SphericalSurface(S);
  Init(GS, false, Precision::Confusion());
  Add(W);
  if (Inside && BRep_Tool::IsClosed(W))
    CheckInside();
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const gp_Torus& T, const TopoDS_Wire& W, const bool Inside)
{
  occ::handle<Geom_ToroidalSurface> GT = new Geom_ToroidalSurface(T);
  Init(GT, false, Precision::Confusion());
  Add(W);
  if (Inside && BRep_Tool::IsClosed(W))
    CheckInside();
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const occ::handle<Geom_Surface>& S,
                                   const TopoDS_Wire&               W,
                                   const bool                       Inside)
{
  Init(S, false, Precision::Confusion());
  Add(W);
  if (Inside && BRep_Tool::IsClosed(W))
    CheckInside();
}

//=================================================================================================

BRepLib_MakeFace::BRepLib_MakeFace(const TopoDS_Face& F, const TopoDS_Wire& W)
{
  Init(F);
  Add(W);
}

//=================================================================================================

void BRepLib_MakeFace::Init(const TopoDS_Face& F)
{
  // copy the face
  myShape = F.EmptyCopied();
  myError = BRepLib_FaceDone;

  BRep_Builder    B;
  TopoDS_Iterator It(F);
  while (It.More())
  {
    B.Add(myShape, It.Value());
    It.Next();
  }
}

//=================================================================================================

void BRepLib_MakeFace::Init(const occ::handle<Geom_Surface>& S,
                            const bool                       Bound,
                            const double                     TolDegen)
{
  myError = BRepLib_FaceDone;
  if (Bound)
  {
    double UMin, UMax, VMin, VMax;
    S->Bounds(UMin, UMax, VMin, VMax);
    Init(S, UMin, UMax, VMin, VMax, TolDegen);
  }
  else
  {
    BRep_Builder B;
    B.MakeFace(TopoDS::Face(myShape), S, Precision::Confusion());
  }
  BRep_Builder B;
  B.NaturalRestriction(TopoDS::Face(myShape), true);
}

//=======================================================================
// function : IsDegenerated
// purpose  : Checks whether the passed curve is degenerated with the
//           passed tolerance value
//=======================================================================

bool BRepLib_MakeFace::IsDegenerated(const occ::handle<Geom_Curve>& theCurve,
                                     const double                   theMaxTol,
                                     double&                        theActTol)
{
  GeomAdaptor_Curve AC(theCurve);
  double            aConfusion = Precision::Confusion();
  theActTol                    = aConfusion;
  GeomAbs_CurveType Type       = AC.GetType();

  if (Type == GeomAbs_Circle)
  {
    gp_Circ Circ = AC.Circle();
    if (Circ.Radius() > theMaxTol)
      return false;
    theActTol = std::max(Circ.Radius(), aConfusion);
    return true;
  }
  else if (Type == GeomAbs_BSplineCurve)
  {
    occ::handle<Geom_BSplineCurve> BS            = AC.BSpline();
    int                            NbPoles       = BS->NbPoles();
    double                         aMaxPoleDist2 = 0.0, aMaxTol2 = theMaxTol * theMaxTol;
    gp_Pnt                         P1, P2;
    P1 = BS->Pole(1);
    for (int i = 2; i <= NbPoles; i++)
    {
      P2                = BS->Pole(i);
      double aPoleDist2 = P1.SquareDistance(P2);
      if (aPoleDist2 > aMaxTol2)
        return false;
      if (aPoleDist2 > aMaxPoleDist2)
        aMaxPoleDist2 = aPoleDist2;
    }
    theActTol = std::max(1.000001 * std::sqrt(aMaxPoleDist2), aConfusion);
    return true;
  }
  else if (Type == GeomAbs_BezierCurve)
  {
    occ::handle<Geom_BezierCurve> BZ            = AC.Bezier();
    int                           NbPoles       = BZ->NbPoles();
    double                        aMaxPoleDist2 = 0.0, aMaxTol2 = theMaxTol * theMaxTol;
    gp_Pnt                        P1, P2;
    P1 = BZ->Pole(1);
    for (int i = 2; i <= NbPoles; i++)
    {
      P2                = BZ->Pole(i);
      double aPoleDist2 = P1.SquareDistance(P2);
      if (aPoleDist2 > aMaxTol2)
        return false;
      if (aPoleDist2 > aMaxPoleDist2)
        aMaxPoleDist2 = aPoleDist2;
    }
    theActTol = std::max(1.000001 * std::sqrt(aMaxPoleDist2), aConfusion);
    return true;
  }

  return false;
}

//=================================================================================================

void BRepLib_MakeFace::Init(const occ::handle<Geom_Surface>& SS,
                            const double                     Um,
                            const double                     UM,
                            const double                     Vm,
                            const double                     VM,
                            const double                     TolDegen)
{
  myError = BRepLib_FaceDone;

  double UMin = Um;
  double UMax = UM;
  double VMin = Vm;
  double VMax = VM;

  double umin, umax, vmin, vmax, T;

  occ::handle<Geom_Surface>                   S = SS, BS = SS;
  occ::handle<Geom_RectangularTrimmedSurface> RS =
    occ::down_cast<Geom_RectangularTrimmedSurface>(S);
  if (!RS.IsNull())
    BS = RS->BasisSurface();

  bool OffsetSurface = (BS->DynamicType() == STANDARD_TYPE(Geom_OffsetSurface));

  // adjust periodical surface or reordonate
  // check if the values are in the natural range
  constexpr double epsilon = Precision::PConfusion();

  BS->Bounds(umin, umax, vmin, vmax);

  if (OffsetSurface)
  {
    occ::handle<Geom_OffsetSurface> OS   = occ::down_cast<Geom_OffsetSurface>(BS);
    occ::handle<Geom_Surface>       Base = OS->BasisSurface();

    if (Base->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
    {
      if (Precision::IsInfinite(umin) || Precision::IsInfinite(umax))
        S = new Geom_RectangularTrimmedSurface(OS, UMin, UMax, VMin, VMax);
      else
        S = new Geom_RectangularTrimmedSurface(OS, VMin, VMax, false);
    }
    else if (Base->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfRevolution))
    {
      if (Precision::IsInfinite(vmin) || Precision::IsInfinite(vmax))
        S = new Geom_RectangularTrimmedSurface(OS, VMin, VMax, false);
    }
  }

  if (S->IsUPeriodic())
  {
    ElCLib::AdjustPeriodic(umin, umax, epsilon, UMin, UMax);
  }
  else if (UMin > UMax)
  {
    T    = UMin;
    UMin = UMax;
    UMax = T;
    if ((umin - UMin > epsilon) || (UMax - umax > epsilon))
    {
      myError = BRepLib_ParametersOutOfRange;
      return;
    }
  }

  if (S->IsVPeriodic())
  {
    ElCLib::AdjustPeriodic(vmin, vmax, epsilon, VMin, VMax);
  }
  else if (VMin > VMax)
  {
    T    = VMin;
    VMin = VMax;
    VMax = T;
    if ((vmin - VMin > epsilon) || (VMax - vmax > epsilon))
    {
      myError = BRepLib_ParametersOutOfRange;
      return;
    }
  }

  // compute infinite flags
  bool umininf = Precision::IsNegativeInfinite(UMin);
  bool umaxinf = Precision::IsPositiveInfinite(UMax);
  bool vmininf = Precision::IsNegativeInfinite(VMin);
  bool vmaxinf = Precision::IsPositiveInfinite(VMax);

  // closed flag
  bool uclosed =
    S->IsUClosed() && std::abs(UMin - umin) < epsilon && std::abs(UMax - umax) < epsilon;

  bool vclosed =
    S->IsVClosed() && std::abs(VMin - vmin) < epsilon && std::abs(VMax - vmax) < epsilon;

  // compute 3d curves and degenerate flag
  double                  maxTol = TolDegen;
  occ::handle<Geom_Curve> Cumin, Cumax, Cvmin, Cvmax;
  bool                    Dumin, Dumax, Dvmin, Dvmax;
  Dumin = Dumax = Dvmin = Dvmax = false;
  double uminTol = Precision::Confusion(), umaxTol = Precision::Confusion(),
         vminTol = Precision::Confusion(), vmaxTol = Precision::Confusion();

  if (!umininf)
  {
    Cumin = S->UIso(UMin);
    Dumin = IsDegenerated(Cumin, maxTol, uminTol);
  }
  if (!umaxinf)
  {
    Cumax = S->UIso(UMax);
    Dumax = IsDegenerated(Cumax, maxTol, umaxTol);
  }
  if (!vmininf)
  {
    Cvmin = S->VIso(VMin);
    Dvmin = IsDegenerated(Cvmin, maxTol, vminTol);
  }
  if (!vmaxinf)
  {
    Cvmax = S->VIso(VMax);
    Dvmax = IsDegenerated(Cvmax, maxTol, vmaxTol);
  }

  // compute vertices
  BRep_Builder B;

  TopoDS_Vertex V00, V10, V11, V01;

  if (!umininf)
  {
    if (!vmininf)
      B.MakeVertex(V00, S->Value(UMin, VMin), std::max(uminTol, vminTol));
    if (!vmaxinf)
      B.MakeVertex(V01, S->Value(UMin, VMax), std::max(uminTol, vmaxTol));
  }
  if (!umaxinf)
  {
    if (!vmininf)
      B.MakeVertex(V10, S->Value(UMax, VMin), std::max(umaxTol, vminTol));
    if (!vmaxinf)
      B.MakeVertex(V11, S->Value(UMax, VMax), std::max(umaxTol, vmaxTol));
  }

  if (uclosed)
  {
    V10 = V00;
    V11 = V01;
  }

  if (vclosed)
  {
    V01 = V00;
    V11 = V10;
  }

  if (Dumin)
    V00 = V01;
  if (Dumax)
    V10 = V11;
  if (Dvmin)
    V00 = V10;
  if (Dvmax)
    V01 = V11;

  // make the lines
  occ::handle<Geom2d_Line> Lumin, Lumax, Lvmin, Lvmax;
  if (!umininf)
    Lumin = new Geom2d_Line(gp_Pnt2d(UMin, 0), gp_Dir2d(gp_Dir2d::D::Y));
  if (!umaxinf)
    Lumax = new Geom2d_Line(gp_Pnt2d(UMax, 0), gp_Dir2d(gp_Dir2d::D::Y));
  if (!vmininf)
    Lvmin = new Geom2d_Line(gp_Pnt2d(0, VMin), gp_Dir2d(gp_Dir2d::D::X));
  if (!vmaxinf)
    Lvmax = new Geom2d_Line(gp_Pnt2d(0, VMax), gp_Dir2d(gp_Dir2d::D::X));

  // make the face
  TopoDS_Face& F = TopoDS::Face(myShape);
  B.MakeFace(F, S, Precision::Confusion());

  // make the edges
  TopoDS_Edge eumin, eumax, evmin, evmax;

  if (!umininf)
  {
    if (!Dumin)
      B.MakeEdge(eumin, Cumin, uminTol);
    else
      B.MakeEdge(eumin);
    if (uclosed)
      B.UpdateEdge(eumin, Lumax, Lumin, F, std::max(uminTol, umaxTol));
    else
      B.UpdateEdge(eumin, Lumin, F, uminTol);
    B.Degenerated(eumin, Dumin);
    if (!vmininf)
    {
      V00.Orientation(TopAbs_FORWARD);
      B.Add(eumin, V00);
    }
    if (!vmaxinf)
    {
      V01.Orientation(TopAbs_REVERSED);
      B.Add(eumin, V01);
    }
    B.Range(eumin, VMin, VMax);
  }

  if (!umaxinf)
  {
    if (uclosed)
      eumax = eumin;
    else
    {
      if (!Dumax)
        B.MakeEdge(eumax, Cumax, umaxTol);
      else
        B.MakeEdge(eumax);
      B.UpdateEdge(eumax, Lumax, F, umaxTol);
      B.Degenerated(eumax, Dumax);
      if (!vmininf)
      {
        V10.Orientation(TopAbs_FORWARD);
        B.Add(eumax, V10);
      }
      if (!vmaxinf)
      {
        V11.Orientation(TopAbs_REVERSED);
        B.Add(eumax, V11);
      }
      B.Range(eumax, VMin, VMax);
    }
  }

  if (!vmininf)
  {
    if (!Dvmin)
      B.MakeEdge(evmin, Cvmin, vminTol);
    else
      B.MakeEdge(evmin);
    if (vclosed)
      B.UpdateEdge(evmin, Lvmin, Lvmax, F, std::max(vminTol, vmaxTol));
    else
      B.UpdateEdge(evmin, Lvmin, F, vminTol);
    B.Degenerated(evmin, Dvmin);
    if (!umininf)
    {
      V00.Orientation(TopAbs_FORWARD);
      B.Add(evmin, V00);
    }
    if (!umaxinf)
    {
      V10.Orientation(TopAbs_REVERSED);
      B.Add(evmin, V10);
    }
    B.Range(evmin, UMin, UMax);
  }

  if (!vmaxinf)
  {
    if (vclosed)
      evmax = evmin;
    else
    {
      if (!Dvmax)
        B.MakeEdge(evmax, Cvmax, vmaxTol);
      else
        B.MakeEdge(evmax);
      B.UpdateEdge(evmax, Lvmax, F, vmaxTol);
      B.Degenerated(evmax, Dvmax);
      if (!umininf)
      {
        V01.Orientation(TopAbs_FORWARD);
        B.Add(evmax, V01);
      }
      if (!umaxinf)
      {
        V11.Orientation(TopAbs_REVERSED);
        B.Add(evmax, V11);
      }
      B.Range(evmax, UMin, UMax);
    }
  }

  // make the wires and add them to the face
  eumin.Orientation(TopAbs_REVERSED);
  evmax.Orientation(TopAbs_REVERSED);

  TopoDS_Wire W;

  if (!umininf && !umaxinf && vmininf && vmaxinf)
  {
    // two wires in u
    B.MakeWire(W);
    B.Add(W, eumin);
    B.Add(F, W);
    B.MakeWire(W);
    B.Add(W, eumax);
    B.Add(F, W);
    F.Closed(uclosed);
  }

  else if (umininf && umaxinf && !vmininf && !vmaxinf)
  {
    // two wires in v
    B.MakeWire(W);
    B.Add(W, evmin);
    B.Add(F, W);
    B.MakeWire(W);
    B.Add(W, evmax);
    B.Add(F, W);
    F.Closed(vclosed);
  }

  else if (!umininf || !umaxinf || !vmininf || !vmaxinf)
  {
    // one wire
    B.MakeWire(W);
    if (!umininf)
      B.Add(W, eumin);
    if (!vmininf)
      B.Add(W, evmin);
    if (!umaxinf)
      B.Add(W, eumax);
    if (!vmaxinf)
      B.Add(W, evmax);
    B.Add(F, W);
    W.Closed(!umininf && !umaxinf && !vmininf && !vmaxinf);
    F.Closed(uclosed && vclosed);
  }

  if (OffsetSurface)
  {
    // Les Isos sont Approximees a Precision::Approximation()
    // et on code Precision::Confusion() dans l'arete.
    // ==> Un petit passage dans SamePrameter pour regler les tolerances.
    BRepLib::SameParameter(F, Precision::Confusion(), true);
  }

  Done();
}

//=================================================================================================

void BRepLib_MakeFace::Add(const TopoDS_Wire& W)
{
  BRep_Builder B;
  B.Add(myShape, W);
  B.NaturalRestriction(TopoDS::Face(myShape), false);
  Done();
}

//=================================================================================================

const TopoDS_Face& BRepLib_MakeFace::Face() const
{
  return TopoDS::Face(myShape);
}

//=================================================================================================

BRepLib_MakeFace::operator TopoDS_Face() const
{
  return Face();
}

//=================================================================================================

BRepLib_FaceError BRepLib_MakeFace::Error() const
{
  return myError;
}

//=======================================================================
// function : CheckInside
// purpose  : Reverses the current face if not a bounded area
//=======================================================================

void BRepLib_MakeFace::CheckInside()
{
  // compute the area and return the face if the area is negative
  TopoDS_Face             F = TopoDS::Face(myShape);
  BRepTopAdaptor_FClass2d FClass(F, 0.);
  if (FClass.PerformInfinitePoint() == TopAbs_IN)
  {
    BRep_Builder    B;
    TopoDS_Shape    S = myShape.EmptyCopied();
    TopoDS_Iterator it(myShape);
    while (it.More())
    {
      B.Add(S, it.Value().Reversed());
      it.Next();
    }
    myShape = S;
  }
}
