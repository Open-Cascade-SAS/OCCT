// Created on: 2000-01-20
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

//: pdn 11.12.98: FixDegenerated improved
//: pdn 05.01.99: renaming method CheckLittle to CheckSmall
//: l0 abv 10.01.99: CATIA01 #1727: fix intersecting edges always if edge is lacking
//: n2 abv 22.01.99: ma-test5.igs: IGES read (pref3d): remove degen edge with no pcurve
//: o4 abv 17.02.99: r0301_db.stp #53082: adding parameter isClosed to CheckOrder
//    rln 03.03.99  S4135: using updated ShapeAnalysis_Surface for checking of singularities
//: p9 abv 11.03.99: PRO7226 #489490: fix :i9 moved to allow fixing a set of degenerated edges
// #77 rln 11.03.99: S4135: using singularity which has minimum gap between singular point and input
// 3D point #84 rln 18.03.99: inserting degenerated edge between ends of pcurves pdn 12.03.99 S4135
// check degenerated applies minimal tolerance first. pdn 16.03.99 S4135 adding check of non
// adjacent edjes. #83 rln 19.03.99: processing segments in intersection as in BRepCheck %15
// pdn 15.03.99  checking of small area wire added #2 smh 26.03.99  S4163 Zero divide #4 szv S4163
// optimizing
//: r6 abv 08.04.99: protect FixIE against working out of curve range
//: s1 abv 22.04.99: PRO7226 #489490: ensure fixing of degenerated edge
// #9 smh 14.12.99 BUC60615 Using tolerance of verteces during checking degenerated edge.

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Bnd_Box2d.hxx>
#include <NCollection_Array1.hxx>
#include <BndLib_Add2dCurve.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepGProp.hxx>
#include <BRepTools.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt2d.hxx>
#include <GProp_GProps.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_IntersectionSegment.hxx>
#include <IntRes2d_Transition.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeAnalysis_TransferParametersProj.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <ShapeAnalysis_WireOrder.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeExtend.hxx>
#include <ShapeExtend_WireData.hxx>
#include <Standard_Type.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeAnalysis_Wire, Standard_Transient)

// szvsh addition
//=================================================================================================

ShapeAnalysis_Wire::ShapeAnalysis_Wire()
{
  ClearStatuses();
  myPrecision = ::Precision::Confusion();
}

//=================================================================================================

ShapeAnalysis_Wire::ShapeAnalysis_Wire(const TopoDS_Wire& wire,
                                       const TopoDS_Face& face,
                                       const double       precision)
{
  Init(wire, face, precision);
}

//=================================================================================================

ShapeAnalysis_Wire::ShapeAnalysis_Wire(const occ::handle<ShapeExtend_WireData>& sbwd,
                                       const TopoDS_Face&                       face,
                                       const double                             precision)
{
  Init(sbwd, face, precision);
}

//=================================================================================================

void ShapeAnalysis_Wire::Init(const TopoDS_Wire& wire,
                              const TopoDS_Face& face,
                              const double       precision)
{
  Init(new ShapeExtend_WireData(wire), face, precision);
}

//=================================================================================================

void ShapeAnalysis_Wire::Init(const occ::handle<ShapeExtend_WireData>& sbwd,
                              const TopoDS_Face&                       face,
                              const double                             precision)
{
  Load(sbwd);
  SetFace(face);
  SetPrecision(precision);
}

//=================================================================================================

void ShapeAnalysis_Wire::Load(const TopoDS_Wire& wire)
{
  ClearStatuses();
  myWire = new ShapeExtend_WireData(wire);
}

//=================================================================================================

void ShapeAnalysis_Wire::Load(const occ::handle<ShapeExtend_WireData>& sbwd)
{
  ClearStatuses();
  myWire = sbwd;
}

//=================================================================================================

void ShapeAnalysis_Wire::SetFace(const TopoDS_Face& face)
{
  myFace = face;
  if (myFace.IsNull())
  {
    return;
  }

  const occ::handle<Geom_Surface> aSurface = BRep_Tool::Surface(myFace);
  if (!face.IsNull() && !aSurface.IsNull())
  {
    mySurf = new ShapeAnalysis_Surface(aSurface);
  }
}

//=================================================================================================

void ShapeAnalysis_Wire::SetFace(const TopoDS_Face&                        theFace,
                                 const occ::handle<ShapeAnalysis_Surface>& theSurfaceAnalysis)
{
  myFace = theFace;
  mySurf = theSurfaceAnalysis;
}

//=================================================================================================

void ShapeAnalysis_Wire::SetSurface(const occ::handle<ShapeAnalysis_Surface>& theSurfaceAnalysis)
{
  mySurf = theSurfaceAnalysis;
}

//=================================================================================================

void ShapeAnalysis_Wire::SetSurface(const occ::handle<Geom_Surface>& surface)
{
  SetSurface(surface, TopLoc_Location());
}

//=================================================================================================

void ShapeAnalysis_Wire::SetSurface(const occ::handle<Geom_Surface>& surface,
                                    const TopLoc_Location&           location)
{
  BRep_Builder B;
  TopoDS_Face  face;
  B.MakeFace(face, surface, location, ::Precision::Confusion());
  SetFace(face);
}

//=================================================================================================

void ShapeAnalysis_Wire::SetPrecision(const double precision)
{
  myPrecision = precision;
}

//=================================================================================================

void ShapeAnalysis_Wire::ClearStatuses()
{
  myStatusOrder = myStatusConnected = myStatusEdgeCurves = myStatusDegenerated = myStatusClosed =
    myStatusLacking = myStatusSelfIntersection = myStatusSmall = myStatusGaps3d = myStatusGaps2d =
      myStatusCurveGaps = myStatusLoop = myStatus = 0;

  myMin3d = myMin2d = myMax3d = myMax2d = 0.;
}

//=================================================================================================

bool ShapeAnalysis_Wire::Perform()
{
  bool result = false;
  result |= CheckOrder();
  result |= CheckSmall();
  result |= CheckConnected();
  result |= CheckEdgeCurves();
  result |= CheckDegenerated();
  result |= CheckSelfIntersection();
  result |= CheckLacking();
  result |= CheckClosed();
  return result;
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckOrder(const bool isClosed, const bool mode3d)
{
  ShapeAnalysis_WireOrder sawo;
  CheckOrder(sawo, isClosed, mode3d, false);
  myStatusOrder = myStatus;
  return StatusOrder(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckSmall(const double precsmall)
{
  for (int i = 1; i <= myWire->NbEdges(); i++)
  {
    CheckSmall(i, precsmall);
    myStatusSmall |= myStatus;
  }
  return StatusSmall(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckConnected(const double prec)
{
  for (int i = 1; i <= myWire->NbEdges(); i++)
  {
    CheckConnected(i, prec);
    myStatusConnected |= myStatus;
  }
  return StatusConnected(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckEdgeCurves()
{
  myStatusEdgeCurves = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;

  int                i, nb = myWire->NbEdges();
  ShapeAnalysis_Edge SAE;

  for (i = 1; i <= nb; i++)
  {
    TopoDS_Edge E = myWire->Edge(i);

    SAE.CheckCurve3dWithPCurve(E, myFace);
    if (SAE.Status(ShapeExtend_DONE))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    if (SAE.Status(ShapeExtend_FAIL))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);

    SAE.CheckVerticesWithPCurve(E, myFace);
    if (SAE.Status(ShapeExtend_DONE))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
    if (SAE.Status(ShapeExtend_FAIL))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);

    SAE.CheckVerticesWithCurve3d(E);
    if (SAE.Status(ShapeExtend_DONE))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
    if (SAE.Status(ShapeExtend_FAIL))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);

    CheckSeam(i);
    if (LastCheckStatus(ShapeExtend_DONE))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE4);
    if (LastCheckStatus(ShapeExtend_FAIL))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL4);

    CheckGap3d(i);
    if (LastCheckStatus(ShapeExtend_DONE))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE5);
    if (LastCheckStatus(ShapeExtend_FAIL))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL5);

    CheckGap2d(i);
    if (LastCheckStatus(ShapeExtend_DONE))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE6);
    if (LastCheckStatus(ShapeExtend_FAIL))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL6);

    double maxdev = 0.0;
    SAE.CheckSameParameter(myWire->Edge(i), maxdev);
    if (SAE.Status(ShapeExtend_DONE))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_DONE7);
    if (SAE.Status(ShapeExtend_FAIL))
      myStatusEdgeCurves |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL7);
  }
  return StatusEdgeCurves(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckDegenerated()
{
  for (int i = 1; i <= myWire->NbEdges(); i++)
  {
    CheckDegenerated(i);
    myStatusDegenerated |= myStatus;
  }
  return StatusDegenerated(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckSelfIntersection()
{
  myStatusSelfIntersection = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;
  int i, nb = myWire->NbEdges();
  for (i = 1; i <= nb; i++)
  {
    CheckSelfIntersectingEdge(i);
    if (LastCheckStatus(ShapeExtend_DONE))
      myStatusSelfIntersection |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    if (LastCheckStatus(ShapeExtend_FAIL))
      myStatusSelfIntersection |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);

    CheckIntersectingEdges(i);
    if (LastCheckStatus(ShapeExtend_DONE))
      myStatusSelfIntersection |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
    if (LastCheckStatus(ShapeExtend_FAIL))
      myStatusSelfIntersection |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
  }

  NCollection_Array1<Bnd_Box2d>     boxes(1, nb);
  TopLoc_Location                   L;
  const occ::handle<Geom_Surface>&  S = BRep_Tool::Surface(Face(), L);
  occ::handle<Geom2d_Curve>         c2d;
  double                            cf, cl;
  ShapeAnalysis_Edge                sae;
  occ::handle<ShapeExtend_WireData> sbwd = WireData();
  for (i = 1; i <= nb; i++)
  {
    TopoDS_Edge E = sbwd->Edge(i);
    if (sae.PCurve(E, S, L, c2d, cf, cl, false))
    {
      Bnd_Box2d           box;
      Geom2dAdaptor_Curve gac(c2d, cf, cl);
      BndLib_Add2dCurve::Add(gac, ::Precision::Confusion(), box);
      boxes(i) = box;
    }
  }

  bool isFail = false, isDone = false;
  for (int num1 = 1; num1 < nb - 1; num1++)
  {
    int fin = nb;
    if (CheckClosed(Precision::Confusion()) && 1 == num1)
      fin = nb - 1;
    for (int num2 = num1 + 2; num2 <= fin; num2++)
      if (!boxes(num1).IsOut(boxes(num2)))
      {
        CheckIntersectingEdges(num1, num2);
        isFail |= LastCheckStatus(ShapeExtend_FAIL1);
        isDone |= LastCheckStatus(ShapeExtend_DONE1);
      }
  }
  if (isFail)
    myStatusSelfIntersection |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
  if (isDone)
    myStatusSelfIntersection |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);

  return StatusSelfIntersection(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckLacking()
{
  if (!IsReady() || NbEdges() < 2)
    return false;
  for (int i = 1; i <= myWire->NbEdges(); i++)
  {
    CheckLacking(i);
    myStatusLacking |= myStatus;
  }
  return StatusLacking(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckClosed(const double prec)
{
  myStatusClosed = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady() || NbEdges() < 1)
    return false;

  CheckConnected(1, prec);
  if (LastCheckStatus(ShapeExtend_DONE))
    myStatusClosed |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  if (LastCheckStatus(ShapeExtend_FAIL))
    myStatusClosed |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);

  CheckDegenerated(1);
  if (LastCheckStatus(ShapeExtend_DONE))
    myStatusClosed |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
  if (LastCheckStatus(ShapeExtend_FAIL))
    myStatusClosed |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);

  return StatusClosed(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckGaps3d()
{
  myStatusGaps3d = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsLoaded() || NbEdges() < 1)
    return false; // gka IsLoaded

  double dist, maxdist = 0.;

  for (int i = 1; i <= NbEdges(); i++)
  {
    CheckGap3d(i);
    myStatusGaps3d |= myStatus;
    if (!LastCheckStatus(ShapeExtend_FAIL1))
    {
      dist = MinDistance3d();
      if (maxdist < dist)
        maxdist = dist;
    }
  }
  myMin3d = myMax3d = maxdist;

  return StatusGaps3d(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckGaps2d()
{
  myStatusGaps2d = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady() || NbEdges() < 1)
    return false;

  double dist, maxdist = 0.;

  for (int i = 1; i <= NbEdges(); i++)
  {
    CheckGap2d(i);
    myStatusGaps2d |= myStatus;
    if (!LastCheckStatus(ShapeExtend_FAIL1))
    {
      dist = MinDistance2d();
      if (maxdist < dist)
        maxdist = dist;
    }
  }
  myMin2d = myMax2d = maxdist;

  return StatusGaps2d(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckCurveGaps()
{
  myStatusCurveGaps = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady() || NbEdges() < 1)
    return false;

  double dist, maxdist = 0.;

  for (int i = 1; i <= NbEdges(); i++)
  {
    CheckCurveGap(i);
    myStatusCurveGaps |= myStatus;
    if (!LastCheckStatus(ShapeExtend_FAIL1))
    {
      dist = MinDistance3d();
      if (maxdist < dist)
        maxdist = dist;
    }
  }
  myMin3d = myMax3d = maxdist;

  return StatusCurveGaps(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckOrder(ShapeAnalysis_WireOrder& sawo,
                                    const bool               isClosed,
                                    const bool               theMode3D,
                                    const bool               theModeBoth)
{
  if ((!theMode3D || theModeBoth) && myFace.IsNull())
  {
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
    return false;
  }
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  sawo.SetMode(theMode3D, 0.0, theModeBoth);
  int                nb = myWire->NbEdges();
  ShapeAnalysis_Edge EA;
  bool               isAll2dEdgesOk = true;
  for (int i = 1; i <= nb; i++)
  {
    TopoDS_Edge E = myWire->Edge(i);
    gp_XYZ      aP1XYZ, aP2XYZ;
    gp_XY       aP1XY, aP2XY;
    if (theMode3D || theModeBoth)
    {
      TopoDS_Vertex V1 = EA.FirstVertex(E);
      TopoDS_Vertex V2 = EA.LastVertex(E);
      if (V1.IsNull() || V2.IsNull())
      {
        myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
        return false;
      }
      else
      {
        aP1XYZ = BRep_Tool::Pnt(V1).XYZ();
        aP2XYZ = BRep_Tool::Pnt(V2).XYZ();
      }
    }
    if (!theMode3D || theModeBoth)
    {
      double                    f, l;
      occ::handle<Geom2d_Curve> c2d;
      TopoDS_Shape              tmpF = myFace.Oriented(TopAbs_FORWARD);
      if (!EA.PCurve(E, TopoDS::Face(tmpF), c2d, f, l))
      {
        // if mode is 2d, then we can nothing to do, else we can switch to 3d mode
        if (!theMode3D && !theModeBoth)
        {
          myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
          return false;
        }
        else
        {
          isAll2dEdgesOk = false;
        }
      }
      else
      {
        aP1XY = c2d->Value(f).XY();
        aP2XY = c2d->Value(l).XY();
      }
    }
    if (theMode3D && !theModeBoth)
    {
      sawo.Add(aP1XYZ, aP2XYZ);
    }
    else if (!theMode3D && !theModeBoth)
    {
      sawo.Add(aP1XY, aP2XY);
    }
    else
    {
      sawo.Add(aP1XYZ, aP2XYZ, aP1XY, aP2XY);
    }
  }
  // need to switch to 3d mode
  if (theModeBoth && !isAll2dEdgesOk)
  {
    sawo.SetMode(true, 0.0, false);
  }
  sawo.Perform(isClosed);
  int stat = sawo.Status();
  switch (stat)
  {
    case 0:
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
      break;
    case 1:
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      break;
      // clang-format off
  case   2: myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE2); break; // this value is not returned
  case  -1: myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE3); break;
  case  -2: myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE4); break; // this value is not returned
  case   3: myStatus = ShapeExtend::EncodeStatus (ShapeExtend_DONE5); break; // only shifted
  case -10: myStatus = ShapeExtend::EncodeStatus (ShapeExtend_FAIL1); break; // this value is not returned
      // clang-format on
  }
  return LastCheckStatus(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckConnected(const int num, const double prec)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsLoaded() || NbEdges() < 1)
    return false;

  int n2 = (num > 0 ? num : NbEdges());
  int n1 = (n2 > 1 ? n2 - 1 : NbEdges());
  //  if (n1 == n2) return 0;

  TopoDS_Edge E1 = WireData()->Edge(n1);
  TopoDS_Edge E2 = WireData()->Edge(n2);

  ShapeAnalysis_Edge sae;
  TopoDS_Vertex      V1 = sae.LastVertex(E1);
  TopoDS_Vertex      V2 = sae.FirstVertex(E2);
  if (V1.IsNull() || V2.IsNull())
  {
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
    return false;
  }
  if (V1.IsSame(V2))
    return false;

  gp_Pnt p1 = BRep_Tool::Pnt(V1);
  gp_Pnt p2 = BRep_Tool::Pnt(V2);
  myMin3d   = p1.Distance(p2);
  if (myMin3d <= gp::Resolution())
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  else if (myMin3d <= myPrecision)
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
  else if (myMin3d <= prec)
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
  else
  {
    // et en inversant la derniere edge ?
    if (n1 == n2)
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    else
    {
      V2          = sae.LastVertex(E2);
      p2          = BRep_Tool::Pnt(V2);
      double dist = p1.Distance(p2);
      if (dist > myPrecision)
        myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
      else
      {
        myMin3d  = dist;
        myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
      }
    }
    return false;
  }
  return true;
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckSmall(const int num, const double precsmall)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsLoaded() || NbEdges() <= 1)
    return false;

  TopoDS_Edge        E = myWire->Edge(num ? num : NbEdges());
  ShapeAnalysis_Edge sae;

  if (BRep_Tool::Degenerated(E))
  {
    //: n2 abv 22 Jan 99: ma-test5.igs -> IGES (brep) -> read (pref3d):
    // degen edge with no pcurve should be removed
    if (!myFace.IsNull() && sae.HasPCurve(E, Face()))
      return false;
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
  }

  TopoDS_Vertex V1 = sae.FirstVertex(E);
  TopoDS_Vertex V2 = sae.LastVertex(E);
  if (V1.IsNull() || V2.IsNull())
  {
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
    return false;
  }
  gp_Pnt p1   = BRep_Tool::Pnt(V1);
  gp_Pnt p2   = BRep_Tool::Pnt(V2);
  double dist = p1.Distance(p2);
  double prec = precsmall; // Min ( myPrecision, precsmall );
  if (dist > prec)
    return false; // not small enough

  // The 3D curve now: is it CLOSED or ZERO LENGTH...???
  // To do this, we take the midpoint (is there anything better?)
  // If there's no 3D curve, we try 2D...

  gp_Pnt                  aMidpoint;
  double                  cf, cl;
  occ::handle<Geom_Curve> c3d;
  if (sae.Curve3d(E, c3d, cf, cl, false))
  {
    aMidpoint = c3d->Value((cf + cl) / 2.);
  }
  else
  {
    occ::handle<Geom2d_Curve> c2d;
    if (!myFace.IsNull() && !mySurf.IsNull() && sae.PCurve(E, myFace, c2d, cf, cl, false))
    {
      gp_Pnt2d p2m = c2d->Value((cf + cl) / 2.);
      aMidpoint    = mySurf->Value(p2m);
    }
    else
    {
      myStatus  = ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
      aMidpoint = p1;
    }
  }
  if (aMidpoint.Distance(p1) > prec || aMidpoint.Distance(p2) > prec)
    return false;

  myStatus |= ShapeExtend::EncodeStatus(V1.IsSame(V2) ? ShapeExtend_DONE1 : ShapeExtend_DONE2);
  return true;
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckSeam(const int                  num,
                                   occ::handle<Geom2d_Curve>& C1,
                                   occ::handle<Geom2d_Curve>& C2,
                                   double&                    cf,
                                   double&                    cl)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;
  int n = num;
  if (n == 0)
    n = NbEdges();
  TopoDS_Edge E = myWire->Edge(n);
  if (!ShapeAnalysis_Edge().IsSeam(E, myFace))
    return false;
  // Extract the Two PCurves of the Seam
  TopoDS_Face ForwardFace = myFace;
  ForwardFace.Orientation(TopAbs_FORWARD);
  // szv#4:S4163:12Mar99 SGI warns
  TopoDS_Shape EF = E.Oriented(TopAbs_FORWARD);
  TopoDS_Shape ER = E.Oriented(TopAbs_REVERSED);
  C1              = BRep_Tool::CurveOnSurface(TopoDS::Edge(EF), ForwardFace, cf, cl);
  C2              = BRep_Tool::CurveOnSurface(TopoDS::Edge(ER), ForwardFace, cf, cl);
  if (C1.IsNull() || C2.IsNull())
    return false;

  //  SelectForward est destine a devenir un outil distinct

  int theCurveIndice = ShapeAnalysis_Curve().SelectForwardSeam(C1, C2);
  if (theCurveIndice != 2)
    return false;

  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  return true;
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckSeam(const int num)
{
  occ::handle<Geom2d_Curve> C1, C2;
  double                    cf, cl;
  return CheckSeam(num, C1, C2, cf, cl);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckDegenerated(const int num, gp_Pnt2d& p2d1, gp_Pnt2d& p2d2)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady() || NbEdges() < 1)
    return false;

  int         n2 = (num > 0) ? num : NbEdges();
  int         n1 = (n2 > 1) ? n2 - 1 : NbEdges();
  int         n3 = (n2 < NbEdges()) ? n2 + 1 : 1;
  TopoDS_Edge E1 = myWire->Edge(n1);
  TopoDS_Edge E2 = myWire->Edge(n2);
  TopoDS_Edge E3 = myWire->Edge(n3);

  ShapeAnalysis_Edge sae;

  // skip if edge is already marked as degenerated and has pcurve
  if (BRep_Tool::Degenerated(E2) && sae.HasPCurve(E2, Face()))
  {
    // skl 30.12.2004 for OCC7630 - we have to check pcurve
    if (sae.HasPCurve(E1, Face()) && sae.HasPCurve(E3, Face()))
    {
      occ::handle<Geom2d_Curve> c2d;
      double                    fp, lp;
      sae.PCurve(E2, myFace, c2d, fp, lp, true);
      gp_Pnt2d p21 = c2d->Value(fp);
      gp_Pnt2d p22 = c2d->Value(lp);
      sae.PCurve(E1, myFace, c2d, fp, lp, true);
      gp_Pnt2d p12 = c2d->Value(lp);
      sae.PCurve(E3, myFace, c2d, fp, lp, true);
      gp_Pnt2d p31 = c2d->Value(fp);
      if (fabs(p12.Distance(p31) - p21.Distance(p22)) > 2 * Precision::PConfusion())
      {
        // pcurve is bad => we can remove this edge in ShapeFix
        // if set needed status
        myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
      }
    }
    return false;
  }

  // pdn allows to insert two sequences of degenerated edges (on separate bounds of surfaces)
  if (n1 != n2 && BRep_Tool::Degenerated(E1) && !sae.HasPCurve(E1, Face()))
  {
    //: abv 13.05.02: OCC320 - fail (to remove edge) if two consecutive degenerated edges w/o
    //: pcurves
    if (BRep_Tool::Degenerated(E2))
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
    return false;
  }

  TopoDS_Vertex Vp = sae.FirstVertex(E1); //: i9
  TopoDS_Vertex V0 = sae.LastVertex(E1);
  TopoDS_Vertex V1 = sae.FirstVertex(E2);
  TopoDS_Vertex V2 = sae.LastVertex(E2);

  if (Vp.IsNull() || V0.IsNull() || V1.IsNull() || V2.IsNull())
    return false;

  gp_Pnt pp = BRep_Tool::Pnt(Vp); //: i9
  gp_Pnt p0 = BRep_Tool::Pnt(V0);
  gp_Pnt p1 = BRep_Tool::Pnt(V1);
  gp_Pnt p2 = BRep_Tool::Pnt(V2);
  double par1, par2;
  bool   lack = false;
  bool   dgnr = false;
  // pdn 12.03.99 minimal value processing first
  double precFirst = std::min(myPrecision, BRep_Tool::Tolerance(V1));
  double precFin   = std::max(myPrecision, BRep_Tool::Tolerance(V1));
  double precVtx   = (myPrecision < BRep_Tool::Tolerance(V1) ? 2 * precFin : precFin);
  //  forward : si Edge <num> FWD/REV. Si LACK, toujours True
  bool forward = (E2.Orientation() == TopAbs_FORWARD);
  //  FIX FEV 1998 : recompute singularity according precision

  if (p1.Distance(p2) <= precFirst)
  {                                                                                 // edge DGNR
    dgnr = mySurf->DegeneratedValues(p1, precVtx, p2d1, p2d2, par1, par2, forward); // smh#9
    if (dgnr)
    { // abv 24 Feb 00: trj3_as1-ac-214.stp #6065: avoid making closed edge degenerated
      double                  a, b;
      occ::handle<Geom_Curve> C3d = BRep_Tool::Curve(E2, a, b);
      if (!C3d.IsNull())
      {
        gp_Pnt p = C3d->Value(0.5 * (a + b));
        if (p.SquareDistance(p1) > precVtx * precVtx)
          dgnr = false;
      }
    }
  }
  if (!dgnr)
  {
    //: i9 abv 23 Sep 98: CTS20315-2 #63231: check that previous edge is not degenerated
    if (n1 != n2 && p1.Distance(pp) <= precFirst && mySurf->IsDegenerated(pp, precFirst)
        && !BRep_Tool::Degenerated(E1))
      return false;
    // rln S4135 ShapeAnalysis_Surface new algorithms for singularities
    //: 45 by abv 16 Dec 97: BUC60035 2659: precision increased to vertex tolerance
    // double prec = Max ( myPrecision, BRep_Tool::Tolerance(V1) );
    //: 51 abv 22 Dec 97: recompute singularities if necessary
    // rln S4135 if ( prec > myPrecision ) mySurf->ComputeSingularities ( 2 * prec ); //:51 //:74
    // abv 15 Jan 97: *2

    if (p0.Distance(p1) <= precFin)
    { // ou DGNR manquante ?
      // rln S4135 singularity with precision = 2 * prec, but distance <= prec
      // lack = mySurf->DegeneratedValues ( p1, prec, p2d1, p2d2, par1, par2, forward);
      double tmpPreci;
      gp_Pnt tmpP3d;
      bool   tmpUIsoDeg;
      // #77 rln S4135: using singularity which has minimum gap between singular point and input 3D
      // point
      int    indMin  = -1;
      double gapMin2 = RealLast();
      for (int i = 1; i <= mySurf->NbSingularities(precVtx); i++)
      {
        mySurf->Singularity(i, tmpPreci, tmpP3d, p2d1, p2d2, par1, par2, tmpUIsoDeg);
        double gap2 = p1.SquareDistance(tmpP3d);
        if (gap2 <= precVtx * precVtx)
          if (gapMin2 > gap2)
          {
            gapMin2 = gap2;
            indMin  = i;
          }
      }
      if (indMin >= 1)
      {
        mySurf->Singularity(indMin, tmpPreci, tmpP3d, p2d1, p2d2, par1, par2, tmpUIsoDeg);
        lack = true;
      }
    }

    // rln S4135 if ( prec > myPrecision ) mySurf->ComputeSingularities ( myPrecision ); //:51
  }

  // there you go, we either have degenerate or lack
  if (!lack && !dgnr)
  {
    //: abv 29.08.01: if singularity not detected but edge is marked
    // as degenerated, report fail
    if (BRep_Tool::Degenerated(E2) && !sae.HasPCurve(E2, Face()))
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
    return false;
  }

  // OK, degenerated case detected; we will find its start and end in 2d

  if (lack)
    forward = true;

  //: 24 by abv 28 Nov 97:
  // make degenerative pcurve parametrized exactly from end of pcurve of the
  // previous edge to the start of the next one
  if (lack || n1 != n2)
  { //: i8 abv 18 Sep 98: ProSTEP TR9 r0501-ug.stp #182180: single degedge is a wire at apex of a
    //: cone
    double                    a, b;
    occ::handle<Geom2d_Curve> c2d;
    if (sae.PCurve(E1, myFace, c2d, a, b, true))
    {
      p2d1 = c2d->Value(b);
    }
    else
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    // pdn pcurves (fixing regression in f0 in degenerated case)
    if (sae.PCurve((dgnr ? E3 : E2), myFace, c2d, a, b, true))
    {
      p2d2 = c2d->Value(a);
    }
    else
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
  }

  // #84 rln 18.03.99 if pcurve is not degenerate anymore, the fix is postponned
  // to ShapeFix_Wire::FixLacking
  if (!mySurf->IsDegenerated(p2d1, p2d2, precVtx, 10.))
  { //: s1 abv 22 Apr 99: PRO7226 #489490 //smh#9
    //: abv 24.05.02: OCC320 - fail (to remove edge) if two consecutive degenerated edges w/o
    //: pcurves
    if (BRep_Tool::Degenerated(E2))
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
    return false;
  }

  // added by rln 18/12/97 CSR# CTS18544 entity 13638
  // the situation when degenerated edge already exists but flag is not set
  //(i.e. the parametric space is closed)
  GeomAdaptor_Surface& Ads = *mySurf->Adaptor3d();
  double               max = std::max(Ads.UResolution(myPrecision), Ads.VResolution(myPrecision));
  if (p2d1.Distance(p2d2) /*Abs (par1 - par2)*/ <= max + gp::Resolution())
    return false;

  myStatus = ShapeExtend::EncodeStatus(dgnr ? ShapeExtend_DONE2 : ShapeExtend_DONE1);
  return true;
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckDegenerated(const int num)
{
  gp_Pnt2d p2d1, p2d2;
  return CheckDegenerated(num, p2d1, p2d2);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckGap3d(const int num)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  // szv#4:S4163:12Mar99 optimized
  if (!IsLoaded() || NbEdges() < 1)
    return false; // szvsh was nbedges < 2
  int                     n2 = (num > 0 ? num : NbEdges());
  int                     n1 = (n2 > 1 ? n2 - 1 : NbEdges());
  TopoDS_Edge             E1 = myWire->Edge(n1);
  TopoDS_Edge             E2 = myWire->Edge(n2);
  double                  uf1, ul1, uf2, ul2;
  occ::handle<Geom_Curve> C1, C2;
  ShapeAnalysis_Edge      SAE;
  if (!SAE.Curve3d(E1, C1, uf1, ul1) || !SAE.Curve3d(E2, C2, uf2, ul2))
  {
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return false;
  }
  gp_Pnt p1 = C1->Value(ul1);
  gp_Pnt p2 = C2->Value(uf2);
  myMin3d = myMax3d = p1.Distance(p2);
  if (myMin3d > myPrecision)
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  return LastCheckStatus(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckGap2d(const int num)
{
  if (myFace.IsNull() || mySurf.IsNull())
  {
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return false;
  }

  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  // szv#4:S4163:12Mar99 optimized
  if (!IsReady() || NbEdges() < 1)
    return false; // szvsh was nbedges < 2
  int                       n2 = (num > 0 ? num : NbEdges());
  int                       n1 = (n2 > 1 ? n2 - 1 : NbEdges());
  TopoDS_Edge               E1 = myWire->Edge(n1);
  TopoDS_Edge               E2 = myWire->Edge(n2);
  double                    uf1, ul1, uf2, ul2;
  occ::handle<Geom2d_Curve> C1, C2;
  ShapeAnalysis_Edge        SAE;
  if (!SAE.PCurve(E1, myFace, C1, uf1, ul1) || !SAE.PCurve(E2, myFace, C2, uf2, ul2))
  {
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return false;
  }
  gp_Pnt2d p1 = C1->Value(ul1);
  gp_Pnt2d p2 = C2->Value(uf2);
  myMin2d = myMax2d       = p1.Distance(p2);
  GeomAdaptor_Surface& SA = *mySurf->Adaptor3d();
  if (myMin2d > (std::max(SA.UResolution(myPrecision), SA.VResolution(myPrecision))
                 + Precision::PConfusion()))
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  return LastCheckStatus(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckCurveGap(const int num)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsLoaded() || NbEdges() < 1)
    return false;
  int                     n = (num > 0 ? num : NbEdges());
  TopoDS_Edge             E = myWire->Edge(n);
  double                  cuf, cul, pcuf, pcul;
  occ::handle<Geom_Curve> c;
  ShapeAnalysis_Edge      SAE;
  if (!SAE.Curve3d(E, c, cuf, cul, false))
  {
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return false;
  }
  occ::handle<Geom2d_Curve> pc;
  if (!SAE.PCurve(E, myFace, pc, pcuf, pcul, false))
  {
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return false;
  }
  occ::handle<Geom2dAdaptor_Curve> AC = new Geom2dAdaptor_Curve(pc, pcuf, pcul);
  occ::handle<GeomAdaptor_Surface> AS = new GeomAdaptor_Surface(mySurf->Surface());
  Adaptor3d_CurveOnSurface         ACS(AC, AS);
  gp_Pnt                           cpnt, pcpnt;
  int                              nbp = 45;
  double                           dist, maxdist = 0.;
  for (int i = 0; i < nbp; i++)
  {
    cpnt  = c->Value(cuf + i * (cul - cuf) / (nbp - 1));
    pcpnt = ACS.Value(pcuf + i * (pcul - pcuf) / (nbp - 1));
    dist  = cpnt.SquareDistance(pcpnt);
    if (maxdist < dist)
      maxdist = dist;
  }
  myMin3d = myMax3d = std::sqrt(maxdist);
  if (myMin3d > myPrecision)
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  return LastCheckStatus(ShapeExtend_DONE);
}

//=================================================================================================

// auxiliary function
//: h0 abv 29 May 98: PRO10105 1949: like in BRepCheck, point is to be taken
// from 3d curve (but only if edge is SameParameter)
static gp_Pnt GetPointOnEdge(const TopoDS_Edge&                        edge,
                             const occ::handle<ShapeAnalysis_Surface>& surf,
                             const Geom2dAdaptor_Curve&                Crv2d,
                             const double                              param)
{
  if (BRep_Tool::SameParameter(edge))
  {
    double                        f, l;
    TopLoc_Location               L;
    const occ::handle<Geom_Curve> ConS = BRep_Tool::Curve(edge, L, f, l);
    if (!ConS.IsNull())
      return ConS->Value(param).Transformed(L.Transformation());
  }
  gp_Pnt2d aP2d = Crv2d.Value(param);
  return surf->Adaptor3d()->Value(aP2d.X(), aP2d.Y());
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckSelfIntersectingEdge(
  const int                                         num,
  NCollection_Sequence<IntRes2d_IntersectionPoint>& points2d,
  NCollection_Sequence<gp_Pnt>&                     points3d)
{
  points2d.Clear();
  points3d.Clear();
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;

  TopoDS_Edge        edge = WireData()->Edge(num > 0 ? num : NbEdges());
  ShapeAnalysis_Edge sae;

  double                    a, b;
  occ::handle<Geom2d_Curve> Crv;
  if (!sae.PCurve(edge, myFace, Crv, a, b, false))
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return false;
  }
  if (std::abs(a - b) <= ::Precision::PConfusion())
    return false;

  double tolint = 1.0e-10;
  // szv#4:S4163:12Mar99 warning
  IntRes2d_Domain     domain(Crv->Value(a), a, tolint, Crv->Value(b), b, tolint);
  Geom2dAdaptor_Curve AC(Crv);
  Geom2dInt_GInter    Inter(AC, domain, tolint, tolint);

  if (!Inter.IsDone())
    return false;

  TopoDS_Vertex V1 = sae.FirstVertex(edge);
  TopoDS_Vertex V2 = sae.LastVertex(edge);
  if (V1.IsNull() || V2.IsNull())
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
    return false;
  }
  double tol1 = BRep_Tool::Tolerance(V1);
  double tol2 = BRep_Tool::Tolerance(V2);

  gp_Pnt pnt1 = BRep_Tool::Pnt(V1);
  gp_Pnt pnt2 = BRep_Tool::Pnt(V2);

  for (int i = 1; i <= Inter.NbPoints(); i++)
  {
    const IntRes2d_IntersectionPoint& IP  = Inter.Point(i);
    const IntRes2d_Transition&        Tr1 = IP.TransitionOfFirst();
    const IntRes2d_Transition&        Tr2 = IP.TransitionOfSecond();
    if (Tr1.PositionOnCurve() != IntRes2d_Middle && Tr2.PositionOnCurve() != IntRes2d_Middle)
      continue;
    gp_Pnt pint   = GetPointOnEdge(edge, mySurf, AC, IP.ParamOnFirst());
    double dist21 = pnt1.SquareDistance(pint);
    double dist22 = pnt2.SquareDistance(pint);
    if (dist21 > tol1 * tol1 && dist22 > tol2 * tol2)
    {
      points2d.Append(IP);
      points3d.Append(pint);
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    }
  }

  return LastCheckStatus(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckSelfIntersectingEdge(const int num)
{
  NCollection_Sequence<IntRes2d_IntersectionPoint> points2d;
  NCollection_Sequence<gp_Pnt>                     points3d;
  return CheckSelfIntersectingEdge(num, points2d, points3d);
}

//=======================================================================
// function : CheckIntersectingEdges
// purpose  : Test if two consequent edges are intersecting
//           It is made in accordance with the following check in BRepCheck:
//         - in BRepCheck_Wire::Orientation(), test for self-intersection
//=======================================================================

bool ShapeAnalysis_Wire::CheckIntersectingEdges(
  const int                                         num,
  NCollection_Sequence<IntRes2d_IntersectionPoint>& points2d,
  NCollection_Sequence<gp_Pnt>&                     points3d,
  NCollection_Sequence<double>&                     errors)
{
  points2d.Clear();
  points3d.Clear();
  errors.Clear();
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady() || NbEdges() < 2)
    return false;

  // szv#4:S4163:12Mar99 optimized
  int         n2    = (num > 0) ? num : NbEdges();
  int         n1    = (n2 > 1) ? n2 - 1 : NbEdges();
  TopoDS_Edge edge1 = myWire->Edge(n1);
  TopoDS_Edge edge2 = myWire->Edge(n2);

  ShapeAnalysis_Edge sae;
  TopoDS_Vertex      V1 = sae.LastVertex(edge1);
  TopoDS_Vertex      V2 = sae.FirstVertex(edge2);
  if (V1.IsNull() || V2.IsNull())
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return false;
  }
  if (!BRepTools::Compare(V1, V2))
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
    return false;
  }

  TopoDS_Vertex Vp = sae.FirstVertex(edge1);
  TopoDS_Vertex Vn = sae.LastVertex(edge2);

  double                    a1, b1, a2, b2;
  occ::handle<Geom2d_Curve> Crv1, Crv2;
  if (!sae.PCurve(edge1, myFace, Crv1, a1, b1, false))
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
    return false;
  }
  if (!sae.PCurve(edge2, myFace, Crv2, a2, b2, false))
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
    return false;
  }
  if (std::abs(a1 - b1) <= ::Precision::PConfusion() ||
      // clang-format off
       std::abs( a2 - b2 ) <= ::Precision::PConfusion() ) return false; //:f7 abv 6 May 98: BUC50070 on #42276
  // clang-format on

  bool isForward1 = (edge1.Orientation() == TopAbs_FORWARD);
  bool isForward2 = (edge2.Orientation() == TopAbs_FORWARD);

  double tol0 = std::max(BRep_Tool::Tolerance(V1), BRep_Tool::Tolerance(V2));
  double tol  = tol0;

  gp_Pnt pnt = BRep_Tool::Pnt(V1);

  //  bool Status = false;

  double tolint = 1.0e-10;

  // szv#4:S4163:12Mar99 warning
  Geom2dAdaptor_Curve C1(Crv1), C2(Crv2);
  IntRes2d_Domain     d1(C1.Value(a1), a1, tolint, C1.Value(b1), b1, tolint);
  IntRes2d_Domain     d2(C2.Value(a2), a2, tolint, C2.Value(b2), b2, tolint);

  //: 64 abv 25 Dec 97: Attention!
  // Since Intersection algorithm is not symmetrical, for consistency with BRepCheck
  // edge with lower order number should be intersecting with edge with higher one
  // i.e., for intersection of last and first edges, they should go in reversed order
  // Example: entity #38285 from bug CSR #CTS17806
  // NOTE: Tr1 and Tr2 are not reordered because they are used in the same manner
  Geom2dInt_GInter Inter;
  if (num == 1)
    Inter.Perform(C2, d2, C1, d1, tolint, tolint);
  else
    Inter.Perform(C1, d1, C2, d2, tolint, tolint);
  if (!Inter.IsDone())
    return false;

  //: 86 abv 22 Jan 98: fix self-intersection even if tolerance of vertex is enough
  // to annihilate it. This is done to prevent wrong effects if vertex tolerance
  // will be decreased (e.g., in FixLacking)
  double tole = std::max((BRep_Tool::SameParameter(edge1) ? BRep_Tool::Tolerance(edge1) : tol0),
                         (BRep_Tool::SameParameter(edge2) ? BRep_Tool::Tolerance(edge2) : tol0));
  double tolt = std::min(tol, std::max(tole, myPrecision));
  // double prevRange1 = RealLast(), prevRange2 = RealLast(); //SK
  int isLacking = -1; //: l0 abv: CATIA01 #1727: protect against adding lacking
  // #83 rln 19.03.99 sim2.igs, entity 4292
  // processing also segments as in BRepCheck
  int NbPoints = Inter.NbPoints(), NbSegments = Inter.NbSegments();
  for (int i = 1; i <= NbPoints + NbSegments; i++)
  {
    IntRes2d_IntersectionPoint IP;
    IntRes2d_Transition        Tr1, Tr2;
    if (i <= NbPoints)
      IP = Inter.Point(i);
    else
    {
      const IntRes2d_IntersectionSegment& Seg = Inter.Segment(i - NbPoints);
      if (!Seg.HasFirstPoint() || !Seg.HasLastPoint())
        continue;
      IP  = Seg.FirstPoint();
      Tr1 = IP.TransitionOfFirst();
      Tr2 = IP.TransitionOfSecond();
      if (Tr1.PositionOnCurve() == IntRes2d_Middle || Tr2.PositionOnCurve() == IntRes2d_Middle)
        IP = Seg.LastPoint();
    }
    Tr1 = IP.TransitionOfFirst();
    Tr2 = IP.TransitionOfSecond();

    if (Tr1.PositionOnCurve() != IntRes2d_Middle && Tr2.PositionOnCurve() != IntRes2d_Middle)
      continue;
    double param1, param2;
    param1 = (num == 1 ? IP.ParamOnSecond() : IP.ParamOnFirst());
    param2 = (num == 1 ? IP.ParamOnFirst() : IP.ParamOnSecond());

    //: r6 abv 8 Apr 99: r_47-sd.stp #173850: protect against working out of curve range
    if (a1 - param1 > ::Precision::PConfusion() || param1 - b1 > ::Precision::PConfusion()
        || a2 - param2 > ::Precision::PConfusion() || param2 - b2 > ::Precision::PConfusion())
      continue;

    //: 82 abv 21 Jan 98: point of intersection on Crv1 and Crv2 is different
    // clang-format off
    gp_Pnt pi1 = GetPointOnEdge ( edge1, mySurf, C1, param1 ); //:h0: thesurf.Value ( Crv1->Value ( param1 ) );
    gp_Pnt pi2 = GetPointOnEdge ( edge2, mySurf, C2, param2 ); //:h0: thesurf.Value ( Crv2->Value ( param2 ) );
    // clang-format on
    gp_Pnt pint  = 0.5 * (pi1.XYZ() + pi2.XYZ());
    double di1   = pi1.SquareDistance(pnt);
    double di2   = pi2.SquareDistance(pnt);
    double dist2 = std::max(di1, di2);

    // rln 03/02/98: CSR#BUC50004 entity 56 (to avoid later inserting lacking edge)
    if (isLacking < 0)
    { //: l0
      gp_Pnt2d end1 = Crv1->Value(isForward1 ? b1 : a1);
      gp_Pnt2d end2 = Crv2->Value(isForward2 ? a2 : b2);
      //: l0      double distab2 = mySurf->Value ( end1 ).SquareDistance ( mySurf->Value (
      //: end2 ) ); l0: test like in BRepCheck
      GeomAdaptor_Surface& Ads   = *mySurf->Adaptor3d();
      double               tol2d = 2 * std::max(Ads.UResolution(tol), Ads.VResolution(tol));
      isLacking                  = (end1.SquareDistance(end2) >= tol2d * tol2d);
    }

    if ((dist2 > tolt * tolt || //: 86: tol -> tolt
         isLacking)
        && //: l0
        //: l0	   distab2 > BRep_Tool::Tolerance ( edge1 ) + BRep_Tool::Tolerance ( edge2 ) ) &&
        //: //rln
        (!BRepTools::Compare(Vp, Vn) || //: 63
         dist2 < pint.SquareDistance(BRep_Tool::Pnt(Vp))))
    { //: 63
      points2d.Append(IP);
      points3d.Append(pint);
      errors.Append(0.5 * pi1.Distance(pi2));
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    }
  }

  return LastCheckStatus(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckIntersectingEdges(const int num)
{
  NCollection_Sequence<IntRes2d_IntersectionPoint> points2d;
  NCollection_Sequence<gp_Pnt>                     points3d;
  NCollection_Sequence<double>                     errors;
  return CheckIntersectingEdges(num, points2d, points3d, errors);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckIntersectingEdges(
  const int                                         num1,
  const int                                         num2,
  NCollection_Sequence<IntRes2d_IntersectionPoint>& points2d,
  NCollection_Sequence<gp_Pnt>&                     points3d,
  NCollection_Sequence<double>&                     errors)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;
  occ::handle<ShapeExtend_WireData> sbwd = WireData();
  int                               n2   = (num2 > 0 ? num2 : sbwd->NbEdges());
  int                               n1   = (num1 > 0 ? num1 : sbwd->NbEdges());

  TopoDS_Edge edge1 = sbwd->Edge(n1);
  TopoDS_Edge edge2 = sbwd->Edge(n2);

  ShapeAnalysis_Edge        sae;
  double                    a1, b1, a2, b2;
  occ::handle<Geom2d_Curve> Crv1, Crv2;
  if (!sae.PCurve(edge1, myFace, Crv1, a1, b1, false))
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
    return false;
  }

  if (!sae.PCurve(edge2, myFace, Crv2, a2, b2, false))
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
    return false;
  }

  if (std::abs(a1 - b1) <= ::Precision::PConfusion()
      || std::abs(a2 - b2) <= ::Precision::PConfusion())
    return false;

  points2d.Clear();
  points3d.Clear();
  errors.Clear();
  NCollection_Array1<gp_Pnt> vertexPoints(1, 4);
  NCollection_Array1<double> vertexTolers(1, 4);
  vertexPoints(1) = BRep_Tool::Pnt(sae.FirstVertex(edge1));
  vertexTolers(1) = BRep_Tool::Tolerance(sae.FirstVertex(edge1));
  vertexPoints(2) = BRep_Tool::Pnt(sae.LastVertex(edge1));
  vertexTolers(2) = BRep_Tool::Tolerance(sae.LastVertex(edge1));
  vertexPoints(3) = BRep_Tool::Pnt(sae.FirstVertex(edge2));
  vertexTolers(3) = BRep_Tool::Tolerance(sae.FirstVertex(edge2));
  vertexPoints(4) = BRep_Tool::Pnt(sae.LastVertex(edge2));
  vertexTolers(4) = BRep_Tool::Tolerance(sae.LastVertex(edge2));

  double tolint = 1.0e-10;

  IntRes2d_Domain     d1(Crv1->Value(a1), a1, tolint, Crv1->Value(b1), b1, tolint);
  IntRes2d_Domain     d2(Crv2->Value(a2), a2, tolint, Crv2->Value(b2), b2, tolint);
  Geom2dAdaptor_Curve C1(Crv1), C2(Crv2);

  Geom2dInt_GInter Inter;
  Inter.Perform(C1, d1, C2, d2, tolint, tolint);
  if (!Inter.IsDone())
    return false;

  // #83 rln 19.03.99 sim2.igs, entity 4292
  // processing also segments as in BRepCheck
  int NbPoints = Inter.NbPoints(), NbSegments = Inter.NbSegments();
  for (int i = 1; i <= NbPoints + NbSegments; i++)
  {
    IntRes2d_IntersectionPoint IP;
    IntRes2d_Transition        Tr1, Tr2;
    if (i <= NbPoints)
      IP = Inter.Point(i);
    else
    {
      const IntRes2d_IntersectionSegment& Seg = Inter.Segment(i - NbPoints);
      if (!Seg.HasFirstPoint() || !Seg.HasLastPoint())
        continue;
      IP  = Seg.FirstPoint();
      Tr1 = IP.TransitionOfFirst();
      Tr2 = IP.TransitionOfSecond();
      if (Tr1.PositionOnCurve() == IntRes2d_Middle || Tr2.PositionOnCurve() == IntRes2d_Middle)
        IP = Seg.LastPoint();
    }
    Tr1 = IP.TransitionOfFirst();
    Tr2 = IP.TransitionOfSecond();
    if (Tr1.PositionOnCurve() != IntRes2d_Middle && Tr2.PositionOnCurve() != IntRes2d_Middle)
      continue;
    double param1 = IP.ParamOnFirst();
    double param2 = IP.ParamOnSecond();
    // clang-format off
    gp_Pnt pi1 = GetPointOnEdge ( edge1, mySurf, C1, param1 ); //:h0: thesurf.Value ( Crv1->Value ( param1 ) );
    // clang-format on
    gp_Pnt pi2 = GetPointOnEdge(edge2, mySurf, C2, param2);
    bool   OK1 = false;
    bool   OK2 = false;

    for (int j = 1; (j <= 2) && !OK1; j++)
    {
      double di1 = pi1.SquareDistance(vertexPoints(j));
      if (di1 < vertexTolers(j) * vertexTolers(j))
        OK1 = true;
    }

    for (int j = 3; (j <= 4) && !OK2; j++)
    {
      double di2 = pi2.SquareDistance(vertexPoints(j));
      if (di2 < vertexTolers(j) * vertexTolers(j))
        OK2 = true;
    }

    if (!OK1 || !OK2)
    {
      gp_Pnt pint = 0.5 * (pi1.XYZ() + pi2.XYZ());
      points2d.Append(IP);
      points3d.Append(pint);
      errors.Append(0.5 * pi1.Distance(pi2));
      myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    }
  }
  return LastCheckStatus(ShapeExtend_DONE);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckIntersectingEdges(const int num1, const int num2)
{
  NCollection_Sequence<IntRes2d_IntersectionPoint> points2d;
  NCollection_Sequence<gp_Pnt>                     points3d;
  NCollection_Sequence<double>                     errors;
  return CheckIntersectingEdges(num1, num2, points2d, points3d, errors);
}

//=======================================================================
// function : CheckLacking
// purpose  : Test if two edges are disconnected in 2d according to the
//           Adaptor_Surface::Resolution
//=======================================================================

bool ShapeAnalysis_Wire::CheckLacking(const int    num,
                                      const double Tolerance,
                                      gp_Pnt2d&    p2d1,
                                      gp_Pnt2d&    p2d2)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;

  // szv#4:S4163:12Mar99 optimized
  int         n2 = (num > 0) ? num : NbEdges();
  int         n1 = (n2 > 1) ? n2 - 1 : NbEdges();
  TopoDS_Edge E1 = myWire->Edge(n1);
  TopoDS_Edge E2 = myWire->Edge(n2);

  ShapeAnalysis_Edge sae;
  TopoDS_Vertex      V1 = sae.LastVertex(E1);
  TopoDS_Vertex      V2 = sae.FirstVertex(E2);
  // CKY 4 MAR 1998 : protection against null vertex
  if (V1.IsNull() || V2.IsNull())
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return false;
  }
  if (!BRepTools::Compare(V1, V2))
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
    return false;
  }

  double                    a, b;
  gp_Vec2d                  v1, v2, v12;
  occ::handle<Geom2d_Curve> c2d;
  if (!sae.PCurve(E1, myFace, c2d, a, b, true))
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
    return false;
  }
  Geom2dAdaptor_Curve anAdapt(c2d);
  anAdapt.D1(b, p2d1, v1);
  if (E1.Orientation() == TopAbs_REVERSED)
    v1.Reverse();
  if (!sae.PCurve(E2, myFace, c2d, a, b, true))
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
    return false;
  }
  anAdapt.Load(c2d);
  anAdapt.D1(a, p2d2, v2);
  if (E2.Orientation() == TopAbs_REVERSED)
    v2.Reverse();
  v12     = p2d2.XY() - p2d1.XY();
  myMax2d = v12.SquareMagnitude();

  // test like in BRepCheck
  double tol                 = std::max(BRep_Tool::Tolerance(V1), BRep_Tool::Tolerance(V2));
  tol                        = (Tolerance > gp::Resolution() && Tolerance < tol ? Tolerance : tol);
  GeomAdaptor_Surface& Ads   = *mySurf->Adaptor3d();
  double               tol2d = 2 * std::max(Ads.UResolution(tol), Ads.VResolution(tol));
  if ( // tol2d < gp::Resolution() || //#2 smh 26.03.99 S4163 Zero divide
    myMax2d < tol2d * tol2d)
    return false;

  myMax2d = std::sqrt(myMax2d);
  myMax3d = tol * myMax2d / std::max(tol2d, gp::Resolution());
  myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);

  if (myMax2d < Precision::PConfusion() || //: abv 03.06.02 CTS21866.stp
      (v1.SquareMagnitude() > gp::Resolution() && std::abs(v12.Angle(v1)) > 0.9 * M_PI)
      || (v2.SquareMagnitude() > gp::Resolution() && std::abs(v12.Angle(v2)) > 0.9 * M_PI))
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
  return true;
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckLacking(const int num, const double Tolerance)
{
  gp_Pnt2d p1, p2;
  return CheckLacking(num, Tolerance, p1, p2);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckOuterBound(const bool APIMake)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;

  TopoDS_Wire wire;
  if (APIMake)
    wire = myWire->WireAPIMake();
  else
    wire = myWire->Wire();

  TopoDS_Shape sh   = myFace.EmptyCopied(); // szv#4:S4163:12Mar99 SGI warns
  TopoDS_Face  face = TopoDS::Face(sh);
  BRep_Builder B;
  B.Add(face, wire);
  if (ShapeAnalysis::IsOuterBound(face))
    return false;
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  return true;
}

//=================================================================================================

static double ProjectInside(const Adaptor3d_CurveOnSurface& AD,
                            const gp_Pnt&                   pnt,
                            const double                    preci,
                            gp_Pnt&                         proj,
                            double&                         param,
                            const bool                      adjustToEnds = true)
{
  ShapeAnalysis_Curve sac;
  double              dist   = sac.Project(AD, pnt, preci, proj, param, adjustToEnds);
  double              uFirst = AD.FirstParameter();
  double              uLast  = AD.LastParameter();
  if (param < uFirst)
  {
    param = uFirst;
    proj  = AD.Value(uFirst);
    return proj.Distance(pnt);
  }

  if (param > uLast)
  {
    param = uLast;
    proj  = AD.Value(uLast);
    return proj.Distance(pnt);
  }
  return dist;
}

bool ShapeAnalysis_Wire::CheckNotchedEdges(const int    num,
                                           int&         shortNum,
                                           double&      param,
                                           const double Tolerance)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsReady())
    return false;

  int         n2 = (num > 0) ? num : NbEdges();
  int         n1 = (n2 > 1) ? n2 - 1 : NbEdges();
  TopoDS_Edge E1 = myWire->Edge(n1);
  TopoDS_Edge E2 = myWire->Edge(n2);

  if (BRep_Tool::Degenerated(E1) || BRep_Tool::Degenerated(E2))
    return false;

  ShapeAnalysis_Edge sae;
  TopoDS_Vertex      V1 = sae.LastVertex(E1);
  TopoDS_Vertex      V2 = sae.FirstVertex(E2);

  if (V1.IsNull() || V2.IsNull())
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return false;
  }
  if (!BRepTools::Compare(V1, V2))
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
    return false;
  }

  double                    a1, b1, a2, b2;
  gp_Pnt2d                  p2d1, p2d2;
  gp_Vec2d                  v1, v2;
  occ::handle<Geom2d_Curve> c2d1, c2d2;
  if (!sae.PCurve(E1, myFace, c2d1, a1, b1, false))
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
    return false;
  }

  if (E1.Orientation() == TopAbs_REVERSED)
    c2d1->D1(a1, p2d1, v1);
  else
  {
    c2d1->D1(b1, p2d1, v1);
    v1.Reverse();
  }

  if (!sae.PCurve(E2, myFace, c2d2, a2, b2, false))
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
    return false;
  }
  if (E2.Orientation() == TopAbs_REVERSED)
  {
    c2d2->D1(b2, p2d2, v2);
    v2.Reverse();
  }
  else
    c2d2->D1(a2, p2d2, v2);

  if (v2.Magnitude() < gp::Resolution() || v1.Magnitude() < gp::Resolution())
    return false;

  if (std::abs(v2.Angle(v1)) > 0.1 || p2d1.Distance(p2d2) > Tolerance)
    return false;

  occ::handle<Geom2dAdaptor_Curve> AC2d1 = new Geom2dAdaptor_Curve(c2d1, a1, b1);
  occ::handle<GeomAdaptor_Surface> AdS1  = new GeomAdaptor_Surface(new Geom_Plane(gp_Pln()));
  Adaptor3d_CurveOnSurface         Ad1(AC2d1, AdS1);

  occ::handle<Geom2dAdaptor_Curve> AC2d2 = new Geom2dAdaptor_Curve(c2d2, a2, b2);
  occ::handle<GeomAdaptor_Surface> AdS2  = new GeomAdaptor_Surface(new Geom_Plane(gp_Pln()));
  Adaptor3d_CurveOnSurface         Ad2(AC2d2, AdS2);

  Adaptor3d_CurveOnSurface longAD, shortAD;
  double                   lenP, firstP;

  ShapeAnalysis_Curve sac;

  gp_Pnt Proj1, Proj2;
  double param1 = 0., param2 = 0.;
  p2d2         = c2d2->Value(E2.Orientation() == TopAbs_FORWARD ? b2 : a2);
  p2d1         = c2d1->Value(E1.Orientation() == TopAbs_FORWARD ? a1 : b1);
  double dist1 = ProjectInside(Ad1, gp_Pnt(p2d2.X(), p2d2.Y(), 0), Tolerance, Proj1, param1, false);
  double dist2 = ProjectInside(Ad2, gp_Pnt(p2d1.X(), p2d1.Y(), 0), Tolerance, Proj2, param2, false);

  if (dist1 > Tolerance && dist2 > Tolerance)
    return false;

  if (dist1 < dist2)
  {
    shortAD  = Ad2;
    longAD   = Ad1;
    lenP     = b2 - a2;
    firstP   = a2;
    shortNum = n2;
    param    = param1;
  }
  else
  {
    shortAD  = Ad1;
    longAD   = Ad2;
    lenP     = b1 - a1;
    firstP   = a1;
    shortNum = n1;
    param    = param2;
  }

  double step = lenP / 23;
  for (int i = 1; i < 23; i++, firstP += step)
  {
    double d1 = sac.Project(longAD, shortAD.Value(firstP), Tolerance, Proj1, param1);
    if (d1 > Tolerance)
    {
      return false;
    }
  }

  return true;
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckSmallArea(const TopoDS_Wire& theWire)
{
  myStatus             = ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
  const int aNbControl = 23;
  const int NbEdges    = myWire->NbEdges();
  if (!IsReady() || NbEdges < 1)
    return false;
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);

  double       aF, aL, aLength(0.0);
  const double anInv = 1.0 / static_cast<double>(aNbControl - 1);
  gp_XY        aCenter2d(0., 0.);

  // try to find mid point for closed contour
  occ::handle<Geom2d_Curve> aCurve2d;
  for (int j = 1; j <= NbEdges; ++j)
  {
    const ShapeAnalysis_Edge anAnalyzer;
    if (!anAnalyzer.PCurve(myWire->Edge(j), myFace, aCurve2d, aF, aL))
    {
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
      return false;
    }

    for (int i = 1; i < aNbControl; ++i)
    {
      const double aV = anInv * ((aNbControl - 1 - i) * aF + i * aL);
      aCenter2d += aCurve2d->Value(aV).XY();
    }
  }
  aCenter2d *= 1.0 / static_cast<double>(NbEdges * (aNbControl - 1));

  // check approximated area in 3D
  gp_Pnt aPnt3d;
  gp_XYZ aPrev3d, aCross(0., 0., 0.);
  gp_XYZ aCenter(mySurf->Value(aCenter2d.X(), aCenter2d.Y()).XYZ());

  occ::handle<Geom_Curve> aCurve3d;
  for (int j = 1; j <= NbEdges; ++j)
  {
    const ShapeAnalysis_Edge anAnalizer;
    if (!anAnalizer.Curve3d(myWire->Edge(j), aCurve3d, aF, aL))
    {
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
      return false;
    }
    if (Precision::IsInfinite(aF) || Precision::IsInfinite(aL))
    {
      continue;
    }

    int aBegin = 0;
    if (j == 1)
    {
      aBegin  = 1;
      aPnt3d  = aCurve3d->Value(aF);
      aPrev3d = aPnt3d.XYZ() - aCenter;
    }
    for (int i = aBegin; i < aNbControl; ++i)
    {
      const double  anU      = anInv * ((aNbControl - 1 - i) * aF + i * aL);
      const gp_Pnt  aPnt     = aCurve3d->Value(anU);
      const gp_XYZ& aCurrent = aPnt.XYZ();
      const gp_XYZ  aVec     = aCurrent - aCenter;

      aCross += aPrev3d ^ aVec;
      aLength += aPnt3d.Distance(aPnt);

      aPnt3d  = aPnt;
      aPrev3d = aVec;
    }
  }

  double aTolerance = aLength * myPrecision;
  if (aCross.Modulus() < aTolerance)
  {
    // check real area in 3D
    GProp_GProps aProps;
    GProp_GProps aLProps;
    TopoDS_Face  aFace = TopoDS::Face(myFace.EmptyCopied());
    BRep_Builder().Add(aFace, theWire);
    BRepGProp::SurfaceProperties(aFace, aProps);
    BRepGProp::LinearProperties(aFace, aLProps);

    double aNewTolerance = aLProps.Mass() * myPrecision;
    if (std::abs(aProps.Mass()) < 0.5 * aNewTolerance)
    {
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      return true;
    }
  }

  return false;
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckShapeConnect(const TopoDS_Shape& shape, const double prec)
{
  double tailhead, tailtail, headhead, headtail;
  return CheckShapeConnect(tailhead, tailtail, headtail, headhead, shape, prec);
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckShapeConnect(double&             tailhead,
                                           double&             tailtail,
                                           double&             headtail,
                                           double&             headhead,
                                           const TopoDS_Shape& shape,
                                           const double        prec)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
  if (!IsLoaded() || shape.IsNull())
    return false;
  TopoDS_Vertex      V1, V2;
  TopoDS_Edge        E;
  TopoDS_Wire        W;
  ShapeAnalysis_Edge SAE;
  if (shape.ShapeType() == TopAbs_EDGE)
  {
    E  = TopoDS::Edge(shape);
    V1 = SAE.FirstVertex(E);
    V2 = SAE.LastVertex(E);
  }
  else if (shape.ShapeType() == TopAbs_WIRE)
  {
    W = TopoDS::Wire(shape);
    ShapeAnalysis::FindBounds(W, V1, V2);
  }
  else
    return false;
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  //  on va comparer les points avec ceux de thevfirst et thevlast
  gp_Pnt p1 = BRep_Tool::Pnt(V1);
  gp_Pnt p2 = BRep_Tool::Pnt(V2);

  TopoDS_Vertex vfirst = SAE.FirstVertex(myWire->Edge(1)),
                vlast  = SAE.LastVertex(myWire->Edge(NbEdges()));
  gp_Pnt pf            = BRep_Tool::Pnt(vfirst);
  gp_Pnt pl            = BRep_Tool::Pnt(vlast);

  tailhead   = p1.Distance(pl);
  tailtail   = p2.Distance(pl);
  headhead   = p1.Distance(pf);
  headtail   = p2.Distance(pf);
  double dm1 = tailhead, dm2 = headtail;
  int    res1 = 0, res2 = 0;

  if (tailhead > tailtail)
  {
    res1 = 1;
    dm1  = tailtail;
  }
  if (headtail > headhead)
  {
    res2 = 1;
    dm2  = headhead;
  }
  int result = res1;
  myMin3d    = std::min(dm1, dm2);
  myMax3d    = std::max(dm1, dm2);
  if (dm1 > dm2)
  {
    dm1    = dm2;
    result = res2 + 2;
  }
  switch (result)
  {
    case 1:
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
      break;
    case 2:
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE3);
      break;
    case 3:
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE4);
      break;
  }
  if (!res1)
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE5);
  if (!res2)
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE6);

  if (myMin3d > std::max(myPrecision, prec))
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
  return LastCheckStatus(ShapeExtend_DONE);
}

//=================================================================================================

bool isMultiVertex(const NCollection_List<TopoDS_Shape>&                         alshape,
                   const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapSmallEdges,
                   const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapSeemEdges)
{
  NCollection_List<TopoDS_Shape>::Iterator lIt1(alshape);
  int                                      nbNotAccount = 0;

  for (; lIt1.More(); lIt1.Next())
  {
    if (aMapSmallEdges.Contains(lIt1.Value()))
      nbNotAccount++;
    else if (aMapSeemEdges.Contains(lIt1.Value()))
      nbNotAccount++;
  }
  return ((alshape.Extent() - nbNotAccount) > 2);
}

bool ShapeAnalysis_Wire::CheckLoop(
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapLoopVertices,
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
                                                          aMapVertexEdges,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapSmallEdges,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapSeemEdges)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  if (!IsLoaded() || NbEdges() < 2)
    return false;
  double aSavPreci = Precision();
  SetPrecision(Precision::Infinite());
  int i = 1;

  for (; i <= myWire->NbEdges(); i++)
  {
    TopoDS_Edge   aedge = myWire->Edge(i);
    TopoDS_Vertex aV1, aV2;
    TopExp::Vertices(aedge, aV1, aV2);
    if (aV1.IsNull() || aV2.IsNull())
    {
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
      return false;
    }
    bool isSame = aV1.IsSame(aV2);
    if (myWire->IsSeam(i))
      aMapSeemEdges.Add(aedge); /// continue;
    else if (BRep_Tool::Degenerated(aedge))
      aMapSmallEdges.Add(aedge);
    else if (isSame && CheckSmall(i, BRep_Tool::Tolerance(aV1)))
      aMapSmallEdges.Add(aedge);

    if (!aMapVertexEdges.IsBound(aV1))
    {
      NCollection_List<TopoDS_Shape> alshape;
      aMapVertexEdges.Bind(aV1, alshape);
    }
    if (!aMapVertexEdges.IsBound(aV2))
    {
      NCollection_List<TopoDS_Shape> alshape;
      aMapVertexEdges.Bind(aV2, alshape);
    }
    if (isSame)
    {
      NCollection_List<TopoDS_Shape>& alshape = aMapVertexEdges.ChangeFind(aV1);
      alshape.Append(aedge);
      alshape.Append(aedge);
      if (alshape.Extent() > 2 && isMultiVertex(alshape, aMapSmallEdges, aMapSeemEdges))
        aMapLoopVertices.Add(aV1);
    }
    else
    {
      NCollection_List<TopoDS_Shape>& alshape = aMapVertexEdges.ChangeFind(aV1);
      alshape.Append(aedge);
      if (alshape.Extent() > 2 && isMultiVertex(alshape, aMapSmallEdges, aMapSeemEdges))
        aMapLoopVertices.Add(aV1);
      NCollection_List<TopoDS_Shape>& alshape2 = aMapVertexEdges.ChangeFind(aV2);
      alshape2.Append(aedge);
      if (alshape2.Extent() > 2 && isMultiVertex(alshape2, aMapSmallEdges, aMapSeemEdges))
        aMapLoopVertices.Add(aV2);
    }
  }
  SetPrecision(aSavPreci);
  if (aMapLoopVertices.Extent())
  {
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    myStatusLoop |= myStatus;
    return true;
  }
  return false;
}

//=================================================================================================

static double Project(const occ::handle<Geom_Curve>& theCurve,
                      const double                   theFirstParameter,
                      const double                   theLastParameter,
                      const gp_Pnt&                  thePoint,
                      const double                   thePrecision,
                      double&                        theParameter,
                      gp_Pnt&                        theProjection)
{
  const double aDist = ShapeAnalysis_Curve().Project(theCurve,
                                                     thePoint,
                                                     thePrecision,
                                                     theProjection,
                                                     theParameter,
                                                     theFirstParameter,
                                                     theLastParameter);
  if (theParameter >= theFirstParameter && theParameter <= theLastParameter)
  {
    return aDist;
  }

  const double aParams[] = {theFirstParameter, theLastParameter};
  const gp_Pnt aPrjs[]   = {theCurve->Value(aParams[0]), theCurve->Value(aParams[1])};
  const double aDists[]  = {thePoint.Distance(aPrjs[0]), thePoint.Distance(aPrjs[1])};
  const int    aPI       = (aDists[0] <= aDists[1]) ? 0 : 1;
  theParameter           = aParams[aPI];
  theProjection          = aPrjs[aPI];
  return aDists[aPI];
}

//=================================================================================================

bool ShapeAnalysis_Wire::CheckTail(const TopoDS_Edge& theEdge1,
                                   const TopoDS_Edge& theEdge2,
                                   const double       theMaxSine,
                                   const double       theMaxWidth,
                                   const double       theMaxTolerance,
                                   TopoDS_Edge&       theEdge11,
                                   TopoDS_Edge&       theEdge12,
                                   TopoDS_Edge&       theEdge21,
                                   TopoDS_Edge&       theEdge22)
{
  const TopoDS_Edge aEs[] = {theEdge1, theEdge2};
  if (!IsReady() || BRep_Tool::Degenerated(aEs[0]) || BRep_Tool::Degenerated(aEs[1]))
  {
    return false;
  }

  // Check the distance between the edge common ends.
  const double            aTol2   = theMaxWidth + 0.5 * Precision::Confusion();
  const double            aTol3   = theMaxWidth + Precision::Confusion();
  const double            aTol4   = theMaxWidth + 1.5 * Precision::Confusion();
  const double            aSqTol2 = aTol2 * aTol2;
  const double            aSqTol3 = aTol3 * aTol3;
  occ::handle<Geom_Curve> aCs[2];
  double                  aLs[2][2];
  int                     aVIs[2];
  gp_Pnt                  aVPs[2];
  {
    for (int aEI = 0; aEI < 2; ++aEI)
    {
      if (!ShapeAnalysis_Edge().Curve3d(aEs[aEI], aCs[aEI], aLs[aEI][0], aLs[aEI][1], false))
      {
        return false;
      }

      aVIs[aEI] = (aEs[aEI].Orientation() == TopAbs_REVERSED) ? aEI : 1 - aEI;
      aVPs[aEI] = aCs[aEI]->Value(aLs[aEI][aVIs[aEI]]);
    }
    if (aVPs[0].SquareDistance(aVPs[1]) > aSqTol2)
    {
      return false;
    }
  }

  // Check the angle between the edges.
  if (theMaxSine >= 0)
  {
    const double aSqMaxSine = theMaxSine * theMaxSine;
    gp_XYZ       aDs[2];
    int          aReverse = 0;
    for (int aEI = 0; aEI < 2; ++aEI)
    {
      GeomAdaptor_Curve aCA(aCs[aEI]);
      if (GCPnts_AbscissaPoint::Length(aCA, aLs[aEI][0], aLs[aEI][1], 0.25 * Precision::Confusion())
          < 0.5 * Precision::Confusion())
      {
        return false;
      }

      GCPnts_AbscissaPoint aAP(0.25 * Precision::Confusion(),
                               aCA,
                               0.5 * Precision::Confusion() * (1 - 2 * aVIs[aEI]),
                               aLs[aEI][aVIs[aEI]]);
      if (!aAP.IsDone())
      {
        return false;
      }

      gp_XYZ aPs[2];
      aPs[aVIs[aEI]]     = aVPs[aEI].XYZ();
      aPs[1 - aVIs[aEI]] = aCs[aEI]->Value(aAP.Parameter()).XYZ();
      aDs[aEI]           = aPs[1] - aPs[0];
      const double aDN   = aDs[aEI].Modulus();
      if (aDN < 0.1 * Precision::Confusion())
      {
        return false;
      }

      aDs[aEI] *= 1 / aDN;
      aReverse ^= aVIs[aEI];
    }
    if (aReverse)
    {
      aDs[0].Reverse();
    }
    if (aDs[0] * aDs[1] < 0 || aDs[0].CrossSquareMagnitude(aDs[1]) > aSqMaxSine)
    {
      return false;
    }
  }

  // Calculate the tail bounds.
  gp_Pnt aPs[2], aPrjs[2];
  double aParams1[2], aParams2[2];
  double aDists[2];
  bool   isWholes[] = {true, true};
  for (int aEI = 0; aEI < 2; ++aEI)
  {
    double aParam1 = aLs[aEI][aVIs[aEI]];
    aParams1[aEI]  = aLs[aEI][1 - aVIs[aEI]];
    aCs[aEI]->D0(aParams1[aEI], aPs[aEI]);
    aDists[aEI] = Project(aCs[1 - aEI],
                          aLs[1 - aEI][0],
                          aLs[1 - aEI][1],
                          aPs[aEI],
                          0.25 * Precision::Confusion(),
                          aParams2[aEI],
                          aPrjs[aEI]);
    if (aDists[aEI] <= aTol2)
    {
      continue;
    }

    isWholes[aEI] = false;
    for (;;)
    {
      const double aParam = (aParam1 + aParams1[aEI]) * 0.5;
      aCs[aEI]->D0(aParam, aPs[aEI]);
      const double aDist = Project(aCs[1 - aEI],
                                   aLs[1 - aEI][0],
                                   aLs[1 - aEI][1],
                                   aPs[aEI],
                                   0.25 * Precision::Confusion(),
                                   aParams2[aEI],
                                   aPrjs[aEI]);
      if (aDist <= aTol2)
      {
        aParam1 = aParam;
      }
      else
      {
        aParams1[aEI] = aParam;
        if (aDist <= aTol3)
        {
          break;
        }
      }
    }
  }

  // Check the tail bounds.
  for (int aEI = 0; aEI < 2; ++aEI)
  {
    const double aParam1 = aLs[aEI][aVIs[aEI]];
    const double aParam2 = aParams1[aEI];
    const double aStepL  = (aParam2 - aParam1) / 23;
    for (int aStepN = 1; aStepN < 23; ++aStepN)
    {
      double aParam = aParam1 + aStepN * aStepL;
      gp_Pnt aP     = aCs[aEI]->Value(aParam), aPrj;
      if (Project(aCs[1 - aEI],
                  aLs[1 - aEI][0],
                  aLs[1 - aEI][1],
                  aP,
                  0.25 * Precision::Confusion(),
                  aParam,
                  aPrj)
          > aTol4)
      {
        return false;
      }
    }
  }

  // Check whether both edges must be removed.
  if (isWholes[0] && isWholes[1] && aPs[0].SquareDistance(aPs[1]) <= aSqTol3)
  {
    theEdge11 = theEdge1;
    theEdge21 = theEdge2;
    return true;
  }

  // Cut and remove the edges.
  int aFI = 0;
  if (isWholes[0] || isWholes[1])
  {
    // Determine an edge to remove and the other one to cut.
    aFI = isWholes[0] ? 0 : 1;
    if (aDists[1 - aFI] < aDists[aFI] && isWholes[1 - aFI])
    {
      aFI = 1 - aFI;
    }
  }
  double aParams[2];
  aParams[aFI]     = aParams1[aFI];
  aParams[1 - aFI] = aParams2[aFI];

  // Correct the cut for the parametrization tolerance.
  TopoDS_Edge* aEParts[][2] = {{&theEdge11, &theEdge12}, {&theEdge21, &theEdge22}};
  int          aResults[]   = {1, 1};
  for (int aEI = 0; aEI < 2; ++aEI)
  {
    if (std::abs(aParams[aEI] - aLs[aEI][1 - aVIs[aEI]]) <= Precision::PConfusion())
    {
      aResults[aEI]    = 2;
      *aEParts[aEI][0] = aEs[aEI];
    }
    else if (std::abs(aParams[aEI] - aLs[aEI][aVIs[aEI]]) <= Precision::PConfusion())
    {
      aResults[aEI] = 0;
    }
  }

  // Correct the cut for the distance tolerance.
  for (int aEI = 0; aEI < 2; ++aEI)
  {
    if (aResults[aEI] != 1)
    {
      continue;
    }

    // Create the parts of the edge.
    TopoDS_Edge                          aFE = TopoDS::Edge(aEs[aEI].Oriented(TopAbs_FORWARD));
    ShapeAnalysis_TransferParametersProj aSATPP(aFE, TopoDS_Face());
    aSATPP.SetMaxTolerance(theMaxTolerance);
    TopoDS_Vertex aSplitV;
    BRep_Builder().MakeVertex(aSplitV, aCs[aEI]->Value(aParams[aEI]), Precision::Confusion());
    TopoDS_Edge aEParts2[] = {
      ShapeBuild_Edge().CopyReplaceVertices(aFE,
                                            TopoDS_Vertex(),
                                            TopoDS::Vertex(aSplitV.Oriented(TopAbs_REVERSED))),
      ShapeBuild_Edge().CopyReplaceVertices(aFE, aSplitV, TopoDS_Vertex())};
    ShapeBuild_Edge().CopyPCurves(aEParts2[0], aFE);
    ShapeBuild_Edge().CopyPCurves(aEParts2[1], aFE);
    BRep_Builder().SameRange(aEParts2[0], false);
    BRep_Builder().SameRange(aEParts2[1], false);
    BRep_Builder().SameParameter(aEParts2[0], false);
    BRep_Builder().SameParameter(aEParts2[1], false);
    aSATPP.TransferRange(aEParts2[0], aLs[aEI][0], aParams[aEI], false);
    aSATPP.TransferRange(aEParts2[1], aParams[aEI], aLs[aEI][1], false);
    GProp_GProps aLinProps;
    BRepGProp::LinearProperties(aEParts2[1 - aVIs[aEI]], aLinProps);
    if (aLinProps.Mass() <= Precision::Confusion())
    {
      aResults[aEI]    = 2;
      *aEParts[aEI][0] = aEs[aEI];
    }
    else
    {
      BRepGProp::LinearProperties(aEParts2[aVIs[aEI]], aLinProps);
      if (aLinProps.Mass() <= Precision::Confusion())
      {
        aResults[aEI] = 0;
      }
      else
      {
        *aEParts[aEI][0] = aEParts2[0];
        *aEParts[aEI][1] = aEParts2[1];
      }
    }
  }

  return aResults[0] + aResults[1] != 0;
}
