// Created on: 2000-10-20
// Created by: Julia DOROVSKIKH
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

#include <PrsDim_MidPointRelation.hxx>

#include <PrsDim.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <DsgPrs_MidPointPresentation.hxx>
#include <ElCLib.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Prs3d_Presentation.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsDim_MidPointRelation, PrsDim_Relation)

//=================================================================================================

PrsDim_MidPointRelation::PrsDim_MidPointRelation(const TopoDS_Shape&            aMidPointTool,
                                                 const TopoDS_Shape&            FirstShape,
                                                 const TopoDS_Shape&            SecondShape,
                                                 const occ::handle<Geom_Plane>& aPlane)
    : PrsDim_Relation(),
      myTool(aMidPointTool)
{
  SetFirstShape(FirstShape);
  SetSecondShape(SecondShape);
  SetPlane(aPlane);
  myPosition = aPlane->Pln().Location();
}

//=================================================================================================

void PrsDim_MidPointRelation::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                                      const occ::handle<Prs3d_Presentation>& aprs,
                                      const int)
{
  if (myTool.ShapeType() == TopAbs_VERTEX)
  {
    gp_Pnt pp;
    bool   isonplane;
    if (PrsDim::ComputeGeometry(TopoDS::Vertex(myTool), pp, myPlane, isonplane))
    {
      if (!isonplane)
        ComputeProjVertexPresentation(aprs, TopoDS::Vertex(myTool), pp);
    }
    myMidPoint = pp;
  }
  else
    return;

  if (myAutomaticPosition)
    myPosition = myMidPoint;

  switch (myFShape.ShapeType())
  {
    case TopAbs_FACE: {
      ComputeFaceFromPnt(aprs, true);
    }
    break;
    case TopAbs_EDGE: {
      ComputeEdgeFromPnt(aprs, true);
    }
    break;
    case TopAbs_VERTEX: {
      ComputeVertexFromPnt(aprs, true);
    }
    break;
    default:
      break;
  }

  switch (mySShape.ShapeType())
  {
    case TopAbs_FACE: {
      ComputeFaceFromPnt(aprs, false);
    }
    break;
    case TopAbs_EDGE: {
      ComputeEdgeFromPnt(aprs, false);
    }
    break;
    case TopAbs_VERTEX: {
      ComputeVertexFromPnt(aprs, false);
    }
    break;
    default:
      break;
  }
}

//=================================================================================================

void PrsDim_MidPointRelation::ComputeSelection(const occ::handle<SelectMgr_Selection>& aSel,
                                               const int)
{
  occ::handle<Select3D_SensitiveSegment> seg;
  occ::handle<SelectMgr_EntityOwner>     own = new SelectMgr_EntityOwner(this, 7);

  if (!myMidPoint.IsEqual(myFAttach, Precision::Confusion()))
  {
    // segment from mid point to the first geometry
    seg = new Select3D_SensitiveSegment(own, myFAttach, myMidPoint);
    aSel->Add(seg);
    // segment from mid point to the second geometry
    seg = new Select3D_SensitiveSegment(own, mySAttach, myMidPoint);
    aSel->Add(seg);
  }
  if (!myMidPoint.IsEqual(myPosition, Precision::Confusion()))
  {
    // segment from mid point to the text position
    seg = new Select3D_SensitiveSegment(own, myMidPoint, myPosition);
    aSel->Add(seg);
  }

  // center of the symmetry - circle around the MidPoint
  gp_Ax2 ax = myPlane->Pln().Position().Ax2();
  ax.SetLocation(myMidPoint);
  double                               rad = myFAttach.Distance(myMidPoint) / 20.0;
  gp_Circ                              aCircleM(ax, rad);
  occ::handle<Geom_Curve>              thecir = new Geom_Circle(aCircleM);
  occ::handle<Select3D_SensitiveCurve> scurv  = new Select3D_SensitiveCurve(own, thecir);
  aSel->Add(scurv);

  occ::handle<Geom_Curve> curv;
  gp_Pnt                  firstp, lastp;
  bool                    isInfinite, isOnPlane;
  occ::handle<Geom_Curve> extCurv;

  // segment on first curve
  if (myFShape.ShapeType() == TopAbs_EDGE)
  {
    TopoDS_Edge E = TopoDS::Edge(myFShape);
    if (!PrsDim::ComputeGeometry(E, curv, firstp, lastp, extCurv, isInfinite, isOnPlane, myPlane))
      return;
    if (curv->IsInstance(STANDARD_TYPE(Geom_Line))) // case of line
    {
      // segment on line
      seg = new Select3D_SensitiveSegment(own, myFirstPnt1, myFirstPnt2);
      aSel->Add(seg);
    }
    else if (curv->IsInstance(STANDARD_TYPE(Geom_Circle))) // case of circle
    {
      // segment on circle
      occ::handle<Geom_Circle> thecirc = occ::down_cast<Geom_Circle>(curv);
      double                   udeb    = ElCLib::Parameter(thecirc->Circ(), myFirstPnt1);
      double                   ufin    = ElCLib::Parameter(thecirc->Circ(), myFirstPnt2);
      occ::handle<Geom_Curve>  thecu   = new Geom_TrimmedCurve(thecirc, udeb, ufin);

      scurv = new Select3D_SensitiveCurve(own, thecu);
      aSel->Add(scurv);
    }
    else if (curv->IsInstance(STANDARD_TYPE(Geom_Ellipse))) // case of ellipse
    {
      // segment on ellipse
      occ::handle<Geom_Ellipse> theEll = occ::down_cast<Geom_Ellipse>(curv);
      double                    udeb   = ElCLib::Parameter(theEll->Elips(), myFirstPnt1);
      double                    ufin   = ElCLib::Parameter(theEll->Elips(), myFirstPnt2);
      occ::handle<Geom_Curve>   thecu  = new Geom_TrimmedCurve(theEll, udeb, ufin);

      scurv = new Select3D_SensitiveCurve(own, thecu);
      aSel->Add(scurv);
    }
  }

  // segment on second curve
  if (mySShape.ShapeType() == TopAbs_EDGE)
  {
    TopoDS_Edge E = TopoDS::Edge(mySShape);
    if (!PrsDim::ComputeGeometry(E, curv, firstp, lastp, extCurv, isInfinite, isOnPlane, myPlane))
      return;
    if (curv->IsInstance(STANDARD_TYPE(Geom_Line))) // case of line
    {
      // segment on line
      seg = new Select3D_SensitiveSegment(own, mySecondPnt1, mySecondPnt2);
      aSel->Add(seg);
    }
    else if (curv->IsInstance(STANDARD_TYPE(Geom_Circle))) // case of circle
    {
      // segment on circle
      occ::handle<Geom_Circle> thecirc = occ::down_cast<Geom_Circle>(curv);
      double                   udeb    = ElCLib::Parameter(thecirc->Circ(), mySecondPnt1);
      double                   ufin    = ElCLib::Parameter(thecirc->Circ(), mySecondPnt2);
      occ::handle<Geom_Curve>  thecu   = new Geom_TrimmedCurve(thecirc, udeb, ufin);

      scurv = new Select3D_SensitiveCurve(own, thecu);
      aSel->Add(scurv);
    }
    else if (curv->IsInstance(STANDARD_TYPE(Geom_Ellipse))) // case of ellipse
    {
      // segment on ellipse
      occ::handle<Geom_Ellipse> theEll = occ::down_cast<Geom_Ellipse>(curv);
      double                    udeb   = ElCLib::Parameter(theEll->Elips(), mySecondPnt1);
      double                    ufin   = ElCLib::Parameter(theEll->Elips(), mySecondPnt2);
      occ::handle<Geom_Curve>   thecu  = new Geom_TrimmedCurve(theEll, udeb, ufin);

      scurv = new Select3D_SensitiveCurve(own, thecu);
      aSel->Add(scurv);
    }
  }
}

//=================================================================================================

void PrsDim_MidPointRelation::ComputeFaceFromPnt(const occ::handle<Prs3d_Presentation>&,
                                                 const bool /*first*/)
{
}

//=================================================================================================

void PrsDim_MidPointRelation::ComputeEdgeFromPnt(const occ::handle<Prs3d_Presentation>& aprs,
                                                 const bool                             first)
{
  TopoDS_Edge E;
  if (first)
    E = TopoDS::Edge(myFShape);
  else
    E = TopoDS::Edge(mySShape);

  occ::handle<Geom_Curve> geom;
  gp_Pnt                  ptat1, ptat2;
  occ::handle<Geom_Curve> extCurv;
  bool                    isInfinite, isOnPlane;
  if (!PrsDim::ComputeGeometry(E, geom, ptat1, ptat2, extCurv, isInfinite, isOnPlane, myPlane))
    return;

  gp_Ax2 ax = myPlane->Pln().Position().Ax2();

  if (geom->IsInstance(STANDARD_TYPE(Geom_Line)))
  {
    if (!isInfinite)
      ComputePointsOnLine(ptat1, ptat2, first);
    else
    {
      const gp_Lin& line = occ::down_cast<Geom_Line>(geom)->Lin();
      ComputePointsOnLine(line, first);
    }
    if (first)
      DsgPrs_MidPointPresentation::Add(aprs,
                                       myDrawer,
                                       ax,
                                       myMidPoint,
                                       myPosition,
                                       myFAttach,
                                       myFirstPnt1,
                                       myFirstPnt2,
                                       first);
    else
      DsgPrs_MidPointPresentation::Add(aprs,
                                       myDrawer,
                                       ax,
                                       myMidPoint,
                                       myPosition,
                                       mySAttach,
                                       mySecondPnt1,
                                       mySecondPnt2,
                                       first);
  }
  else if (geom->IsInstance(STANDARD_TYPE(Geom_Circle)))
  {
    occ::handle<Geom_Circle> geom_cir(occ::down_cast<Geom_Circle>(geom));
    gp_Circ                  circ(geom_cir->Circ());
    ComputePointsOnCirc(circ, ptat1, ptat2, first);
    if (first)
      DsgPrs_MidPointPresentation::Add(aprs,
                                       myDrawer,
                                       circ,
                                       myMidPoint,
                                       myPosition,
                                       myFAttach,
                                       myFirstPnt1,
                                       myFirstPnt2,
                                       first);
    else
      DsgPrs_MidPointPresentation::Add(aprs,
                                       myDrawer,
                                       circ,
                                       myMidPoint,
                                       myPosition,
                                       mySAttach,
                                       mySecondPnt1,
                                       mySecondPnt2,
                                       first);
  }
  else if (geom->IsInstance(STANDARD_TYPE(Geom_Ellipse)))
  {
    occ::handle<Geom_Ellipse> geom_ell(occ::down_cast<Geom_Ellipse>(geom));
    gp_Elips                  elips(geom_ell->Elips());
    ComputePointsOnElips(elips, ptat1, ptat2, first);
    if (first)
      DsgPrs_MidPointPresentation::Add(aprs,
                                       myDrawer,
                                       elips,
                                       myMidPoint,
                                       myPosition,
                                       myFAttach,
                                       myFirstPnt1,
                                       myFirstPnt2,
                                       first);
    else
      DsgPrs_MidPointPresentation::Add(aprs,
                                       myDrawer,
                                       elips,
                                       myMidPoint,
                                       myPosition,
                                       mySAttach,
                                       mySecondPnt1,
                                       mySecondPnt2,
                                       first);
  }
  else
    return;

  // projection on myPlane
  if (!isOnPlane)
    ComputeProjEdgePresentation(aprs, E, geom, ptat1, ptat2);
}

//=================================================================================================

void PrsDim_MidPointRelation::ComputeVertexFromPnt(const occ::handle<Prs3d_Presentation>& aprs,
                                                   const bool                             first)
{
  gp_Ax2 ax = myPlane->Pln().Position().Ax2();
  if (first)
  {
    bool          isOnPlane;
    TopoDS_Vertex V = TopoDS::Vertex(myFShape);
    PrsDim::ComputeGeometry(V, myFAttach, myPlane, isOnPlane);
    DsgPrs_MidPointPresentation::Add(aprs, myDrawer, ax, myMidPoint, myPosition, myFAttach, first);
    if (!isOnPlane)
      ComputeProjVertexPresentation(aprs, V, myFAttach);
  }
  else
  {
    bool          isOnPlane;
    TopoDS_Vertex V = TopoDS::Vertex(mySShape);
    PrsDim::ComputeGeometry(V, mySAttach, myPlane, isOnPlane);
    DsgPrs_MidPointPresentation::Add(aprs, myDrawer, ax, myMidPoint, myPosition, mySAttach, first);
    if (!isOnPlane)
      ComputeProjVertexPresentation(aprs, V, mySAttach);
  }
}

//=================================================================================================

void PrsDim_MidPointRelation::ComputePointsOnLine(const gp_Lin& aLin, const bool first)
{
  double ppar     = ElCLib::Parameter(aLin, myMidPoint);
  gp_Pnt anAttach = ElCLib::Value(ppar, aLin);

  double dist = anAttach.Distance(myMidPoint) / 10.0;
  if (dist < Precision::Confusion())
    dist = 10.0;

  double fpar = ppar + dist;
  double spar = ppar - dist;

  gp_Pnt aPnt1 = ElCLib::Value(fpar, aLin);
  gp_Pnt aPnt2 = ElCLib::Value(spar, aLin);

  if (first)
  {
    myFAttach   = anAttach;
    myFirstPnt1 = aPnt1;
    myFirstPnt2 = aPnt2;
  }
  else
  {
    mySAttach    = anAttach;
    mySecondPnt1 = aPnt1;
    mySecondPnt2 = aPnt2;
  }
}

//=================================================================================================

void PrsDim_MidPointRelation::ComputePointsOnLine(const gp_Pnt& pnt1,
                                                  const gp_Pnt& pnt2,
                                                  const bool    first)
{
  gp_Vec aVec(pnt1, pnt2);
  gp_Lin aLin(pnt1, gp_Dir(aVec));

  double fpar = ElCLib::Parameter(aLin, pnt1);
  double spar = ElCLib::Parameter(aLin, pnt2);
  double ppar = ElCLib::Parameter(aLin, myMidPoint);

  gp_Pnt aProjPnt = ElCLib::Value(ppar, aLin);
  double dist     = myMidPoint.Distance(aProjPnt);
  double ll       = pnt1.Distance(pnt2);
  double segm     = std::min(dist, ll) * 0.75;
  if (dist < Precision::Confusion())
    segm = ll * 0.75;

  gp_Pnt anAttach, aPnt1, aPnt2;
  anAttach = aProjPnt;
  gp_Vec aVecTr;
  if (ppar <= fpar)
  {
    aPnt2  = pnt1;
    aVecTr = gp_Vec(pnt2, pnt1);
    aVecTr.Normalize();
    aPnt1 = aProjPnt.Translated(aVecTr * segm);
  }
  else if (ppar >= spar)
  {
    aPnt1  = pnt2;
    aVecTr = gp_Vec(pnt1, pnt2);
    aVecTr.Normalize();
    aPnt2 = aProjPnt.Translated(aVecTr * segm);
  }
  else
  {
    double dp1 = aProjPnt.Distance(pnt1);
    double dp2 = aProjPnt.Distance(pnt2);

    segm   = std::min(dist, dp1) * 0.75;
    aVecTr = gp_Vec(aProjPnt, pnt1);
    aVecTr.Normalize();
    aPnt1 = aProjPnt.Translated(aVecTr * segm);

    segm   = std::min(dist, dp2) * 0.75;
    aVecTr = gp_Vec(aProjPnt, pnt2);
    aVecTr.Normalize();
    aPnt2 = aProjPnt.Translated(aVecTr * segm);
  }

  if (first)
  {
    myFAttach   = anAttach;
    myFirstPnt1 = aPnt1;
    myFirstPnt2 = aPnt2;
  }
  else
  {
    mySAttach    = anAttach;
    mySecondPnt1 = aPnt1;
    mySecondPnt2 = aPnt2;
  }
}

//=================================================================================================

void PrsDim_MidPointRelation::ComputePointsOnCirc(const gp_Circ& aCirc,
                                                  const gp_Pnt&  pnt1,
                                                  const gp_Pnt&  pnt2,
                                                  const bool     first)
{
  gp_Pnt curpos = myMidPoint;

  // Case of confusion between the current position and the center
  // of the circle -> we move the current position
  constexpr double confusion(Precision::Confusion());
  gp_Pnt           aCenter = aCirc.Location();
  if (aCenter.Distance(curpos) <= confusion)
  {
    gp_Vec vprec(aCenter, pnt1);
    vprec.Normalize();
    curpos.Translate(vprec * 1e-5);
  }

  double pcurpos = ElCLib::Parameter(aCirc, curpos);

  double rad = M_PI / 5.0;
  double segm;

  double pFPnt;
  double pSPnt;

  if (pnt1.IsEqual(pnt2, confusion)) // full circle
  {
    pFPnt = pcurpos - rad;
    pSPnt = pcurpos + rad;
  }
  else
  {
    double pFAttach = ElCLib::Parameter(aCirc, pnt1);
    double pSAttach = ElCLib::Parameter(aCirc, pnt2);

    double pSAttachM = pSAttach;
    double deltap    = pSAttachM - pFAttach;
    if (deltap < 0)
    {
      deltap += 2 * M_PI;
      pSAttachM += 2 * M_PI;
    }
    pSAttachM -= pFAttach;

    double pmiddleout = pSAttachM / 2.0 + M_PI;

    double pcurpos1 = pcurpos;
    // define where curpos lays
    if (pcurpos1 < pFAttach)
    {
      pcurpos1 = pcurpos1 + 2 * M_PI - pFAttach;
      if (pcurpos1 > pSAttachM) // out
      {
        segm = std::min(rad, deltap * 0.75);
        if (pcurpos1 > pmiddleout)
        {
          pcurpos = pFAttach;
          pFPnt   = pFAttach;
          pSPnt   = pFAttach + segm;
        }
        else
        {
          pcurpos = pSAttach;
          pFPnt   = pSAttach - segm;
          pSPnt   = pSAttach;
        }
      }
      else // on arc
      {
        double dp1 = pcurpos1 - pFAttach;
        double dp2 = pSAttachM - pcurpos1;

        segm  = std::min(rad, dp1 * 0.75);
        pFPnt = pcurpos - segm;

        segm  = std::min(rad, dp2 * 0.75);
        pSPnt = pcurpos + segm;
      }
    }
    else if (pcurpos1 > (pFAttach + deltap)) // out
    {
      pcurpos1 -= pFAttach;
      segm = std::min(rad, deltap * 0.75);
      if (pcurpos1 > pmiddleout)
      {
        pcurpos = pFAttach;
        pFPnt   = pFAttach;
        pSPnt   = pFAttach + segm;
      }
      else
      {
        pcurpos = pSAttach;
        pFPnt   = pSAttach - segm;
        pSPnt   = pSAttach;
      }
    }
    else // on arc
    {
      double dp1 = pcurpos1 - pFAttach;
      double dp2 = pSAttach - pcurpos1;

      segm  = std::min(rad, dp1 * 0.75);
      pFPnt = pcurpos - segm;

      segm  = std::min(rad, dp2 * 0.75);
      pSPnt = pcurpos + segm;
    }
  }

  if (first)
  {
    myFAttach   = ElCLib::Value(pcurpos, aCirc);
    myFirstPnt1 = ElCLib::Value(pFPnt, aCirc);
    myFirstPnt2 = ElCLib::Value(pSPnt, aCirc);
  }
  else
  {
    mySAttach    = ElCLib::Value(pcurpos, aCirc);
    mySecondPnt1 = ElCLib::Value(pFPnt, aCirc);
    mySecondPnt2 = ElCLib::Value(pSPnt, aCirc);
  }
}

//=================================================================================================

void PrsDim_MidPointRelation::ComputePointsOnElips(const gp_Elips& anEll,
                                                   const gp_Pnt&   pnt1,
                                                   const gp_Pnt&   pnt2,
                                                   const bool      first)
{
  gp_Pnt curpos = myMidPoint;

  // Case of confusion between the current position and the center
  // of the circle -> we move the current position
  constexpr double confusion(Precision::Confusion());
  gp_Pnt           aCenter = anEll.Location();
  if (aCenter.Distance(curpos) <= confusion)
  {
    gp_Vec vprec(aCenter, pnt1);
    vprec.Normalize();
    curpos.Translate(vprec * 1e-5);
  }

  double pcurpos = ElCLib::Parameter(anEll, curpos);

  double rad = M_PI / 5.0;
  double segm;

  double pFPnt;
  double pSPnt;

  if (pnt1.IsEqual(pnt2, confusion)) // full circle
  {
    pFPnt = pcurpos - rad;
    pSPnt = pcurpos + rad;
  }
  else
  {
    double pFAttach = ElCLib::Parameter(anEll, pnt1);
    double pSAttach = ElCLib::Parameter(anEll, pnt2);

    double pSAttachM = pSAttach;
    double deltap    = pSAttachM - pFAttach;
    if (deltap < 0)
    {
      deltap += 2 * M_PI;
      pSAttachM += 2 * M_PI;
    }
    pSAttachM -= pFAttach;

    double pmiddleout = pSAttachM / 2.0 + M_PI;

    double pcurpos1 = pcurpos;
    // define where curpos lays
    if (pcurpos1 < pFAttach)
    {
      pcurpos1 = pcurpos1 + 2 * M_PI - pFAttach;
      if (pcurpos1 > pSAttachM) // out
      {
        segm = std::min(rad, deltap * 0.75);
        if (pcurpos1 > pmiddleout)
        {
          pcurpos = pFAttach;
          pFPnt   = pFAttach;
          pSPnt   = pFAttach + segm;
        }
        else
        {
          pcurpos = pSAttach;
          pFPnt   = pSAttach - segm;
          pSPnt   = pSAttach;
        }
      }
      else // on arc
      {
        double dp1 = pcurpos1 - pFAttach;
        double dp2 = pSAttachM - pcurpos1;

        segm  = std::min(rad, dp1 * 0.75);
        pFPnt = pcurpos - segm;

        segm  = std::min(rad, dp2 * 0.75);
        pSPnt = pcurpos + segm;
      }
    }
    else if (pcurpos1 > (pFAttach + deltap)) // out
    {
      pcurpos1 -= pFAttach;
      segm = std::min(rad, deltap * 0.75);
      if (pcurpos1 > pmiddleout)
      {
        pcurpos = pFAttach;
        pFPnt   = pFAttach;
        pSPnt   = pFAttach + segm;
      }
      else
      {
        pcurpos = pSAttach;
        pFPnt   = pSAttach - segm;
        pSPnt   = pSAttach;
      }
    }
    else // on arc
    {
      double dp1 = pcurpos1 - pFAttach;
      double dp2 = pSAttach - pcurpos1;

      segm  = std::min(rad, dp1 * 0.75);
      pFPnt = pcurpos - segm;

      segm  = std::min(rad, dp2 * 0.75);
      pSPnt = pcurpos + segm;
    }
  }

  if (first)
  {
    myFAttach   = ElCLib::Value(pcurpos, anEll);
    myFirstPnt1 = ElCLib::Value(pFPnt, anEll);
    myFirstPnt2 = ElCLib::Value(pSPnt, anEll);
  }
  else
  {
    mySAttach    = ElCLib::Value(pcurpos, anEll);
    mySecondPnt1 = ElCLib::Value(pFPnt, anEll);
    mySecondPnt2 = ElCLib::Value(pSPnt, anEll);
  }
}
