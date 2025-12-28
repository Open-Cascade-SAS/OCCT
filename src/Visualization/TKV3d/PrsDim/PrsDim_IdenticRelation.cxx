// Created on: 1997-03-03
// Created by: Jean-Pierre COMBE
// Copyright (c) 1997-1999 Matra Datavision
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

#include <PrsDim_IdenticRelation.hxx>

#include <PrsDim.hxx>
#include <AIS_Shape.hxx>
#include <BRep_Tool.hxx>
#include <DsgPrs_IdenticPresentation.hxx>
#include <ElCLib.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Prs3d_Presentation.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_List.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsDim_IdenticRelation, PrsDim_Relation)

// jfa 15/10/2000
static double Modulo2PI(const double ANGLE)
{
  if (ANGLE < 0)
    return Modulo2PI(ANGLE + 2 * M_PI);
  else if (ANGLE >= 2 * M_PI)
    return Modulo2PI(ANGLE - 2 * M_PI);
  return ANGLE;
}

static bool IsEqual2PI(const double angle1, const double angle2, const double precision)
{
  double diff = std::abs(angle1 - angle2);
  if (diff < precision)
    return true;
  else if (std::abs(diff - 2 * M_PI) < precision)
    return true;
  return false;
}

// jfa 15/10/2000 end

//=======================================================================
// function : PrsDim_Sort
// purpose  : sort an array of parameters <tab1> in increasing order
//           updates <tab2> and <tab3> according to <tab1>
//=======================================================================
static void PrsDim_Sort(double tab1[4], gp_Pnt tab2[4], int tab3[4])
{
  bool   found = true;
  double cur;
  gp_Pnt cur1;
  int    cur2;

  while (found)
  {
    found = false;
    for (int i = 0; i < 3; i++)
    {
      if (tab1[i + 1] < tab1[i])
      {
        found       = true;
        cur         = tab1[i];
        cur1        = tab2[i];
        cur2        = tab3[i];
        tab1[i]     = tab1[i + 1];
        tab2[i]     = tab2[i + 1];
        tab3[i]     = tab3[i + 1];
        tab1[i + 1] = cur;
        tab2[i + 1] = cur1;
        tab3[i + 1] = cur2;
      }
    }
  }
}

//=================================================================================================

static bool ConnectedEdges(const TopoDS_Wire&   WIRE,
                           const TopoDS_Vertex& V,
                           TopoDS_Edge&         E1,
                           TopoDS_Edge&         E2)
{
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    vertexMap;
  TopExp::MapShapesAndAncestors(WIRE, TopAbs_VERTEX, TopAbs_EDGE, vertexMap);

  bool          found(false);
  TopoDS_Vertex theVertex;
  for (int i = 1; i <= vertexMap.Extent() && !found; i++)
  {
    if (vertexMap.FindKey(i).IsSame(V))
    {
      theVertex = TopoDS::Vertex(vertexMap.FindKey(i));
      found     = true;
    }
  }
  if (!found)
  {
    E1.Nullify();
    E2.Nullify();
    return false;
  }

  NCollection_List<TopoDS_Shape>::Iterator iterator(vertexMap.FindFromKey(theVertex));
  if (iterator.More())
  {
    E1 = TopoDS::Edge(iterator.Value());
    iterator.Next();
  }
  else
  {
    E1.Nullify();
    return false;
  }

  if (iterator.More())
  {
    E2 = TopoDS::Edge(iterator.Value());
    iterator.Next();
  }
  else
  {
    E2.Nullify();
    return false;
  }

  if (iterator.More())
  {
    E1.Nullify();
    E2.Nullify();
    return false;
  }
  return true;
}

// jfa 16/10/2000
//=======================================================================
// function : ComputeAttach
// purpose  : Compute a point on the arc of <thecirc>
//             between <aFAttach> and <aSAttach>
//             corresponding to <aPosition>
//           Returns result into <aPosition>
// Note    : This function is to be used only in the case of circles.
//           The <aPosition> parameter is in/out.
//=======================================================================
static bool ComputeAttach(const gp_Circ& thecirc,
                          const gp_Pnt&  aFAttach,
                          const gp_Pnt&  aSAttach,
                          gp_Pnt&        aPosition)
{
  gp_Pnt curpos = aPosition;

  // Case of confusion between the current position and the center
  // of the circle -> we move the current position
  constexpr double confusion(Precision::Confusion());
  gp_Pnt           aCenter = thecirc.Location();
  if (aCenter.Distance(curpos) <= confusion)
  {
    gp_Vec vprec(aCenter, aFAttach);
    vprec.Normalize();
    curpos.Translate(vprec * 1e-5);
  }

  double pcurpos  = ElCLib::Parameter(thecirc, curpos);
  double pFAttach = ElCLib::Parameter(thecirc, aFAttach);
  double pSAttach = ElCLib::Parameter(thecirc, aSAttach);

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
      if (pcurpos1 > pmiddleout)
        pcurpos = pFAttach;
      else
        pcurpos = pSAttach;
    }
  }
  else if (pcurpos1 > (pFAttach + deltap)) // out
  {
    pcurpos1 -= pFAttach;
    if (pcurpos1 > pmiddleout)
      pcurpos = pFAttach;
    else
      pcurpos = pSAttach;
  }

  aPosition = ElCLib::Value(pcurpos, thecirc);
  return true;
}

//=======================================================================
// function : ComputeAttach
// purpose  : Compute a point on the arc of ellipse <theEll>
//             between <aFAttach> and <aSAttach>
//             corresponding to <aPosition>
//           Returns result into <aPosition>
// Note    : This function is to be used only in the case of ellipses.
//           The <aPosition> parameter is in/out.
//=======================================================================
static bool ComputeAttach(const gp_Elips& theEll,
                          const gp_Pnt&   aFAttach,
                          const gp_Pnt&   aSAttach,
                          gp_Pnt&         aPosition)
{
  gp_Pnt curpos = aPosition;

  // Case of confusion between the current position and the center
  // of the circle -> we move the current position
  constexpr double confusion(Precision::Confusion());
  gp_Pnt           aCenter = theEll.Location();
  if (aCenter.Distance(curpos) <= confusion)
  {
    gp_Vec vprec(aCenter, aFAttach);
    vprec.Normalize();
    curpos.Translate(vprec * 1e-5);
  }

  // for ellipses it's not good  double pcurpos  = ElCLib::Parameter(theEll,curpos);
  occ::handle<Geom_Ellipse>   theEllg = new Geom_Ellipse(theEll);
  GeomAPI_ProjectPointOnCurve aProj(curpos, theEllg);
  double                      pcurpos = aProj.LowerDistanceParameter();

  double pFAttach = ElCLib::Parameter(theEll, aFAttach);
  double pSAttach = ElCLib::Parameter(theEll, aSAttach);

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
      if (pcurpos1 > pmiddleout)
        pcurpos = pFAttach;
      else
        pcurpos = pSAttach;
    }
  }
  else if (pcurpos1 > (pFAttach + deltap)) // out
  {
    pcurpos1 -= pFAttach;
    if (pcurpos1 > pmiddleout)
      pcurpos = pFAttach;
    else
      pcurpos = pSAttach;
  }

  aPosition = ElCLib::Value(pcurpos, theEll);
  return true;
}

// jfa 16/10/2000 end

//=================================================================================================

PrsDim_IdenticRelation::PrsDim_IdenticRelation(const TopoDS_Shape&            FirstShape,
                                               const TopoDS_Shape&            SecondShape,
                                               const occ::handle<Geom_Plane>& aPlane)
    : isCircle(false)
{
  myFShape = FirstShape;
  mySShape = SecondShape;
  myPlane  = aPlane;
}

//=================================================================================================

void PrsDim_IdenticRelation::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                                     const occ::handle<Prs3d_Presentation>& aprs,
                                     const int)
{
  switch (myFShape.ShapeType())
  {

    case TopAbs_VERTEX: {
      switch (mySShape.ShapeType())
      {
        case TopAbs_VERTEX: {
          ComputeTwoVerticesPresentation(aprs);
        }
        break;
        case TopAbs_EDGE: {
          ComputeOneEdgeOVertexPresentation(aprs);
        }
        break;
        default:
          break;
      }
    }
    break;

    case TopAbs_EDGE: {
      switch (mySShape.ShapeType())
      {
        case TopAbs_VERTEX: {
          ComputeOneEdgeOVertexPresentation(aprs);
        }
        break;
        case TopAbs_EDGE: {
          ComputeTwoEdgesPresentation(aprs);
        }
        break;
        default:
          break;
      }
    }
    break;
    default:
      break;
  }
}

//=======================================================================
// function : ComputeSelection
// purpose  : function used to compute the selection associated to the
//           "identic" presentation
// note    : if we are in the case of lines, we create a segment between
//           myFAttach and mySAttach. In the case of Circles, we create
//           an arc of circle between the sames points. We Add a segment
//           to link Position to its projection on the curve described
//           before.
//=======================================================================

void PrsDim_IdenticRelation::ComputeSelection(const occ::handle<SelectMgr_Selection>& aSelection,
                                              const int)
{
  occ::handle<SelectMgr_EntityOwner> own = new SelectMgr_EntityOwner(this, 7);

  occ::handle<Select3D_SensitiveSegment> seg;
  // attachment point of the segment linking position to the curve
  gp_Pnt           attach;
  constexpr double confusion(Precision::Confusion());

  if (myFAttach.IsEqual(mySAttach, confusion))
  {
    attach = myFAttach;
  }
  else
  {
    // jfa 24/10/2000
    if (myFShape.ShapeType() == TopAbs_EDGE)
    {
      occ::handle<Geom_Curve> curv1, curv2;
      gp_Pnt                  firstp1, lastp1, firstp2, lastp2;
      bool                    isInfinite1, isInfinite2;
      occ::handle<Geom_Curve> extCurv;
      if (!PrsDim::ComputeGeometry(TopoDS::Edge(myFShape),
                                   TopoDS::Edge(mySShape),
                                   myExtShape,
                                   curv1,
                                   curv2,
                                   firstp1,
                                   lastp1,
                                   firstp2,
                                   lastp2,
                                   extCurv,
                                   isInfinite1,
                                   isInfinite2,
                                   myPlane))
        return;

      if (isCircle) // case of Circles
      {
        occ::handle<Geom_Circle> thecirc = occ::down_cast<Geom_Circle>(curv1);
        double                   udeb    = ElCLib::Parameter(thecirc->Circ(), myFAttach);
        double                   ufin    = ElCLib::Parameter(thecirc->Circ(), mySAttach);
        occ::handle<Geom_Curve>  thecu   = new Geom_TrimmedCurve(thecirc, udeb, ufin);

        occ::handle<Select3D_SensitiveCurve> scurv = new Select3D_SensitiveCurve(own, thecu);
        aSelection->Add(scurv);

        attach = myPosition;
        ComputeAttach(thecirc->Circ(), myFAttach, mySAttach, attach);
      }
      else if (curv1->IsInstance(STANDARD_TYPE(Geom_Ellipse))) // case of ellipses
      {
        occ::handle<Geom_Ellipse> theEll = occ::down_cast<Geom_Ellipse>(curv1);

        double                  udeb  = ElCLib::Parameter(theEll->Elips(), myFAttach);
        double                  ufin  = ElCLib::Parameter(theEll->Elips(), mySAttach);
        occ::handle<Geom_Curve> thecu = new Geom_TrimmedCurve(theEll, udeb, ufin);

        occ::handle<Select3D_SensitiveCurve> scurv = new Select3D_SensitiveCurve(own, thecu);
        aSelection->Add(scurv);

        attach = myPosition;
        ComputeAttach(theEll->Elips(), myFAttach, mySAttach, attach);
      }
      else if (curv1->IsInstance(STANDARD_TYPE(Geom_Line))) // case of Lines
      {
        seg = new Select3D_SensitiveSegment(own, myFAttach, mySAttach);
        aSelection->Add(seg);

        // attach = projection of Position() on the curve;
        gp_Vec v1(myFAttach, mySAttach);
        gp_Vec v2(myFAttach, myPosition);
        if (v1.IsParallel(v2, Precision::Angular()))
        {
          attach = mySAttach;
        }
        else
        {
          gp_Lin ll(myFAttach, gp_Dir(v1));
          attach = ElCLib::Value(ElCLib::Parameter(ll, myPosition), ll);
        }
      }
      else
        return;
    }
    //      else if ( myFShape.ShapeType() == TopAbs_VERTEX )
    //	{
    //	}
    // jfa 24/10/2000 end
  }

  // Creation of the segment linking the attachment point with the
  // position
  if (!attach.IsEqual(myPosition, confusion))
  {
    seg = new Select3D_SensitiveSegment(own, attach, myPosition);
    aSelection->Add(seg);
  }
}

//=================================================================================================

void PrsDim_IdenticRelation::ComputeTwoEdgesPresentation(
  const occ::handle<Prs3d_Presentation>& aPrs)
{
  occ::handle<Geom_Curve> curv1, curv2;
  gp_Pnt                  firstp1, lastp1, firstp2, lastp2;
  bool                    isInfinite1, isInfinite2;

  occ::handle<Geom_Curve> extCurv;
  if (!PrsDim::ComputeGeometry(TopoDS::Edge(myFShape),
                               TopoDS::Edge(mySShape),
                               myExtShape,
                               curv1,
                               curv2,
                               firstp1,
                               lastp1,
                               firstp2,
                               lastp2,
                               extCurv,
                               isInfinite1,
                               isInfinite2,
                               myPlane))
    return;
  aPrs->SetInfiniteState((isInfinite1 || isInfinite2) && myExtShape != 0);

  // Treatment of the case of lines
  if (curv1->IsInstance(STANDARD_TYPE(Geom_Line)) && curv2->IsInstance(STANDARD_TYPE(Geom_Line)))
  {
    // we take the line curv1 like support
    occ::handle<Geom_Line> thelin;
    if (isInfinite1 && !isInfinite2)
      thelin = occ::down_cast<Geom_Line>(curv2);
    else if (!isInfinite1 && isInfinite2)
      thelin = occ::down_cast<Geom_Line>(curv1);
    else
      thelin = occ::down_cast<Geom_Line>(curv1);
    ComputeTwoLinesPresentation(aPrs,
                                thelin,
                                firstp1,
                                lastp1,
                                firstp2,
                                lastp2,
                                isInfinite1,
                                isInfinite2);
  }

  //  Treatment of the case of circles
  else if (curv1->IsInstance(STANDARD_TYPE(Geom_Circle))
           && curv2->IsInstance(STANDARD_TYPE(Geom_Circle)))
  {
    // gp_Pnt curpos;
    isCircle = true; // useful for ComputeSelection
    occ::handle<Geom_Circle> thecirc(occ::down_cast<Geom_Circle>(curv1));
    ComputeTwoCirclesPresentation(aPrs, thecirc, firstp1, lastp1, firstp2, lastp2);
  }

  // jfa 10/10/2000
  //  Treatment of the case of ellipses
  else if (curv1->IsInstance(STANDARD_TYPE(Geom_Ellipse))
           && curv2->IsInstance(STANDARD_TYPE(Geom_Ellipse)))
  {
    occ::handle<Geom_Ellipse> theEll(occ::down_cast<Geom_Ellipse>(curv1));
    ComputeTwoEllipsesPresentation(aPrs, theEll, firstp1, lastp1, firstp2, lastp2);
  }
  // jfa 10/10/2000 end
  else
    return;

  // Calculate presentation of projected edges
  if ((myExtShape != 0) && !extCurv.IsNull())
  {
    if (myExtShape == 1)
      ComputeProjEdgePresentation(aPrs, TopoDS::Edge(myFShape), curv1, firstp1, lastp1);
    else
      ComputeProjEdgePresentation(aPrs, TopoDS::Edge(mySShape), curv2, firstp2, lastp2);
  }
}

//=======================================================================
// function : ComputeTwoLinesPresentation
// purpose  : Compute the presentation of the 'identic' constraint
//           between two lines ( which are equal)
// input    : <thelin> : the
//           <firstp1>: first extremity of the 1st curve of the constraint
//           <lastp1> : last extremity of the 1st curve of the constraint
//           <firstp2>: first extremity of the 2nd curve of the constraint
//           <lastp2> :last extremity of the 2nd curve of the constraint
//=======================================================================
void PrsDim_IdenticRelation::ComputeTwoLinesPresentation(
  const occ::handle<Prs3d_Presentation>& aPrs,
  const occ::handle<Geom_Line>&          thelin,
  gp_Pnt&                                firstp1,
  gp_Pnt&                                lastp1,
  gp_Pnt&                                firstp2,
  gp_Pnt&                                lastp2,
  const bool                             isInfinite1,
  const bool                             isInfinite2)
{
  if (isInfinite1 && isInfinite2)
  {
    if (myAutomaticPosition)
    {
      myFAttach = mySAttach = thelin->Lin().Location();
      gp_Pnt curpos;
      gp_Pln pln(myPlane->Pln());
      gp_Dir dir(pln.XAxis().Direction());
      gp_Vec transvec     = gp_Vec(dir) * myArrowSize;
      curpos              = myFAttach.Translated(transvec);
      myPosition          = curpos;
      myAutomaticPosition = true;
    }
    else
    {
      myFAttach = mySAttach =
        ElCLib::Value(ElCLib::Parameter(thelin->Lin(), myPosition), thelin->Lin());
    }
    TCollection_ExtendedString vals(" ==");
    DsgPrs_IdenticPresentation::Add(aPrs, myDrawer, vals, myFAttach, myPosition);
  }
  else
  {
    // Computation of the parameters of the 4 points on the line <thelin>
    double pf1, pf2, pl1, pl2;

    pf1 = ElCLib::Parameter(thelin->Lin(), firstp1);
    pl1 = ElCLib::Parameter(thelin->Lin(), lastp1);

    pf2 = ElCLib::Parameter(thelin->Lin(), firstp2);
    pl2 = ElCLib::Parameter(thelin->Lin(), lastp2);

    if (isInfinite1)
    {
      pf1     = pf2;
      pl1     = pl2;
      firstp1 = firstp2;
      lastp1  = lastp2;
    }
    else if (isInfinite2)
    {
      pf2     = pf1;
      pl2     = pl1;
      firstp2 = firstp1;
      lastp2  = lastp1;
    }

    double tabRang1[4]; // array that contains the parameters of the 4 points
    // ordered by increasing abscisses.

    gp_Pnt tabRang2[4]; // array containing the points corresponding to the
    // parameters in tabRang1

    int tabRang3[4]; // array containing the number of the curve( 1 or 2)
    // of which belongs each point of tabRang2

    // Filling of the arrays
    tabRang1[0] = pf1;
    tabRang2[0] = firstp1;
    tabRang3[0] = 1;
    tabRang1[1] = pf2;
    tabRang2[1] = firstp2;
    tabRang3[1] = 2;
    tabRang1[2] = pl1;
    tabRang2[2] = lastp1;
    tabRang3[2] = 1;
    tabRang1[3] = pl2;
    tabRang2[3] = lastp2;
    tabRang3[3] = 2;

    // Sort of the array of parameters (tabRang1)
    PrsDim_Sort(tabRang1, tabRang2, tabRang3);

    // Computation of myFAttach and mySAttach according to the
    // position of the 2 linear edges
    gp_Pnt curpos;
    gp_Pnt middle;

    if ((tabRang1[0] == tabRang1[1]) && (tabRang1[2] == tabRang1[3]))
    {
      middle.SetXYZ((tabRang2[1].XYZ() + tabRang2[2].XYZ()) / 2.);
      double pmiddle = (tabRang1[1] + tabRang1[2]) / 2.;
      double delta   = (tabRang1[3] - tabRang1[0]) / 5.;
      myFAttach      = ElCLib::Value(pmiddle - delta, thelin->Lin());
      mySAttach      = ElCLib::Value(pmiddle + delta, thelin->Lin());
    }

    else if (tabRang1[1] == tabRang1[2])
    {
      middle        = tabRang2[1];
      double delta1 = tabRang1[1] - tabRang1[0];
      double delta2 = tabRang1[3] - tabRang1[2];
      if (delta1 > delta2)
        delta1 = delta2;
      myFAttach = ElCLib::Value(tabRang1[1] - delta1 / 2., thelin->Lin());
      mySAttach = ElCLib::Value(tabRang1[1] + delta1 / 2., thelin->Lin());
    }

    // Case of 2 disconnected segments -> the symbol completes the gap
    //                                    between the 2 edges
    //--------------------------------
    else if ((tabRang3[0] == tabRang3[1]) && (tabRang1[1] != tabRang1[2]))
    {
      middle.SetXYZ((tabRang2[1].XYZ() + tabRang2[2].XYZ()) / 2.);
      myFAttach = tabRang2[1];
      mySAttach = tabRang2[2];
    }
    else if ((tabRang3[0] != tabRang3[1]) && (tabRang3[1] != tabRang3[2]) // Intersection
             && (tabRang1[1] != tabRang1[2]))
    {
      middle.SetXYZ((tabRang2[1].XYZ() + tabRang2[2].XYZ()) / 2.);
      myFAttach = tabRang2[1];
      mySAttach = tabRang2[2];
    }
    else
    { // Inclusion
      myFAttach.SetXYZ((tabRang2[0].XYZ() + tabRang2[1].XYZ()) / 2.);
      mySAttach.SetXYZ((tabRang2[1].XYZ() + tabRang2[2].XYZ()) / 2.);
      middle.SetXYZ((myFAttach.XYZ() + mySAttach.XYZ()) / 2.);
    }

    if (myAutomaticPosition)
    {

      gp_Vec vtrans(myFAttach, mySAttach);
      vtrans.Normalize();
      vtrans.Cross(gp_Vec(myPlane->Pln().Axis().Direction()));
      vtrans *= ComputeSegSize();
      curpos              = middle.Translated(vtrans);
      myPosition          = curpos;
      myAutomaticPosition = true;
    }

    else
    {

      curpos                   = myPosition;
      double           pcurpos = ElCLib::Parameter(thelin->Lin(), curpos);
      double           dist    = thelin->Lin().Distance(curpos);
      gp_Pnt           proj    = ElCLib::Value(pcurpos, thelin->Lin());
      gp_Vec           trans;
      constexpr double confusion(Precision::Confusion());
      if (dist >= confusion)
      {
        trans = gp_Vec(proj, curpos);
        trans.Normalize();
      }
      double pf = ElCLib::Parameter(thelin->Lin(), myFAttach);
      double pl = ElCLib::Parameter(thelin->Lin(), mySAttach);
      if (pcurpos <= pf)
      {
        pcurpos = pf + 1e-5;
        curpos  = ElCLib::Value(pcurpos, thelin->Lin());
        if (dist >= confusion)
          curpos.Translate(trans * dist);
      }
      else if (pcurpos >= pl)
      {
        pcurpos = pl - 1e-5;
        curpos  = ElCLib::Value(pcurpos, thelin->Lin());
        if (dist >= confusion)
          curpos.Translate(trans * dist);
      }
      SetPosition(curpos);
    }

    // Display of the presentation
    TCollection_ExtendedString vals(" ==");
    DsgPrs_IdenticPresentation::Add(aPrs, myDrawer, vals, myFAttach, mySAttach, curpos);
  }
}

// jfa 17/10/2000
//=======================================================================
// function : ComputeTwoCirclesPresentation
// purpose  : Compute the presentation of the 'identic' constraint
//           between two circles ( which are equal)
// input    : <thecirc>: the circle
//           <firstp1>: first extremity of the 1st curve of the constraint
//           <lastp1> : last extremity of the 1st curve of the constraint
//           <firstp2>: first extremity of the 2nd curve of the constraint
//           <lastp2> :last extremity of the 2nd curve of the constraint
//=======================================================================
void PrsDim_IdenticRelation::ComputeTwoCirclesPresentation(
  const occ::handle<Prs3d_Presentation>& aPrs,
  const occ::handle<Geom_Circle>&        thecirc,
  const gp_Pnt&                          firstp1,
  const gp_Pnt&                          lastp1,
  const gp_Pnt&                          firstp2,
  const gp_Pnt&                          lastp2)
{
  constexpr double confusion(Precision::Confusion());

  // Searching of complete circles
  bool circ1complete = (firstp1.IsEqual(lastp1, confusion));
  bool circ2complete = (firstp2.IsEqual(lastp2, confusion));

  myCenter        = thecirc->Location();
  double aSegSize = thecirc->Radius() / 5.0;
  double rad      = M_PI / 5.0;

  // I. Case of 2 complete circles
  if (circ1complete && circ2complete)
  {
    if (myAutomaticPosition)
    {
      double pfirst1 = ElCLib::Parameter(thecirc->Circ(), firstp1);
      myFAttach      = ElCLib::Value(Modulo2PI(pfirst1 - rad), thecirc->Circ());
      mySAttach      = ElCLib::Value(Modulo2PI(pfirst1 + rad), thecirc->Circ());

      gp_Pnt curpos = ElCLib::Value(pfirst1, thecirc->Circ());
      gp_Vec vtrans(myCenter, curpos);
      vtrans.Normalize();
      vtrans *= aSegSize;
      curpos.Translate(vtrans);
      myPosition = curpos;
    }
    else
      ComputeNotAutoCircPresentation(thecirc);
  }

  // II. Case of one complete circle and one arc
  else if ((circ1complete && !circ2complete) || (!circ1complete && circ2complete))
  {
    gp_Pnt firstp, lastp;
    if (circ1complete && !circ2complete)
    {
      firstp = firstp2;
      lastp  = lastp2;
    }
    else
    {
      firstp = firstp1;
      lastp  = lastp1;
    }

    if (myAutomaticPosition)
    {
      ComputeAutoArcPresentation(thecirc, firstp, lastp);
    }
    else
    {
      ComputeNotAutoArcPresentation(thecirc, firstp, lastp);
    }
  }

  // III and IV. Case of two arcs
  else if (!circ1complete && !circ2complete)
  {
    // We project all the points on the circle
    double pf1, pf2, pl1, pl2;
    pf1 = ElCLib::Parameter(thecirc->Circ(), firstp1);
    pf2 = ElCLib::Parameter(thecirc->Circ(), firstp2);
    pl1 = ElCLib::Parameter(thecirc->Circ(), lastp1);
    pl2 = ElCLib::Parameter(thecirc->Circ(), lastp2);

    // III. Arcs with common ends
    // III.1. First of one and last of another
    if (IsEqual2PI(pl1, pf2, confusion) || IsEqual2PI(pf1, pl2, confusion))
    {
      gp_Pnt curpos(0., 0., 0.);
      double att = 0.;
      if (IsEqual2PI(pl1, pf2, confusion))
      {
        att    = pl1;
        curpos = lastp1;
      }
      else if (IsEqual2PI(pf1, pl2, confusion))
      {
        att    = pf1;
        curpos = firstp1;
      }
      double maxrad = std::min(Modulo2PI(pl1 - pf1), Modulo2PI(pl2 - pf2)) * 3 / 4;
      if (rad > maxrad)
        rad = maxrad;
      double pFAttach = Modulo2PI(att - rad);
      double pSAttach = Modulo2PI(att + rad);
      myFAttach       = ElCLib::Value(pFAttach, thecirc->Circ());
      mySAttach       = ElCLib::Value(pSAttach, thecirc->Circ());
      if (myAutomaticPosition)
      {
        gp_Vec vtrans(myCenter, curpos);
        vtrans.Normalize();
        vtrans *= aSegSize;
        curpos.Translate(vtrans);
        myPosition = curpos;
      }
    }
    // III.2. Two first or two last
    else if (IsEqual2PI(pf1, pf2, confusion) || IsEqual2PI(pl1, pl2, confusion))
    {
      double l1 = Modulo2PI(pl1 - pf1);
      double l2 = Modulo2PI(pl2 - pf2);
      gp_Pnt firstp, lastp;
      if (l1 < l2)
      {
        firstp = firstp1;
        lastp  = lastp1;
      }
      else
      {
        firstp = firstp2;
        lastp  = lastp2;
      }

      if (myAutomaticPosition)
      {
        ComputeAutoArcPresentation(thecirc, firstp, lastp);
      }
      else
      {
        ComputeNotAutoArcPresentation(thecirc, firstp, lastp);
      }
    }
    // IV. All others arcs (without common ends)
    else
    {
      // order the parameters; first will be pf1
      double pl1m = Modulo2PI(pl1 - pf1);
      double pf2m = Modulo2PI(pf2 - pf1);
      double pl2m = Modulo2PI(pl2 - pf1);

      bool case1 = false;
      // 1 - not intersecting arcs
      // 2 - intersecting arcs, but one doesn't contain another
      // 3a - first arc contains the second one
      // 3b - second arc contains the first one
      // 4 - two intersections

      gp_Pnt firstp, lastp;

      if (pl1m < pf2m) // 1 or 2b or 3b
      {
        if (pl1m < pl2m) // 1 or 3b
        {
          if (pl2m < pf2m) // 3b
          {
            firstp = firstp1;
            lastp  = lastp1;
          }
          else // 1
          {
            case1          = true;
            double deltap1 = Modulo2PI(pf1 - pl2);
            double deltap2 = Modulo2PI(pf2 - pl1);
            if (((deltap1 < deltap2) && (deltap1 < 2 * rad))
                || ((deltap2 < deltap1) && (deltap2 > 2 * rad))) // deltap2
            {
              firstp = lastp1;
              lastp  = firstp2;
            }
            else // deltap1
            {
              firstp = lastp2;
              lastp  = firstp1;
            }
          }
        }
        else // 2b
        {
          firstp = firstp1;
          lastp  = lastp2;
        }
      }
      else // 2a or 3a or 4
      {
        if (pl1m < pl2m) // 2a
        {
          firstp = firstp2;
          lastp  = lastp1;
        }
        else // 3a or 4
        {
          if (pl2m > pf2m) // 3a
          {
            firstp = firstp2;
            lastp  = lastp2;
          }
          else // 4
          {
            double deltap1 = Modulo2PI(pl1 - pf2);
            double deltap2 = Modulo2PI(pl2 - pf1);
            if (((deltap1 < deltap2) && (deltap1 < 2 * rad))
                || ((deltap2 < deltap1) && (deltap2 > 2 * rad))) // deltap2
            {
              firstp = firstp1;
              lastp  = lastp2;
            }
            else // deltap1
            {
              firstp = firstp2;
              lastp  = lastp1;
            }
          }
        }
      }

      if (myAutomaticPosition)
      {
        ComputeAutoArcPresentation(thecirc, firstp, lastp, case1);
      }
      else
      {
        if (case1)
        {
          myFAttach = firstp;
          mySAttach = lastp;
        }
        else
          ComputeNotAutoArcPresentation(thecirc, firstp, lastp);
      }
    }
  }

  // Display of the presentation
  TCollection_ExtendedString vals(" ==");
  gp_Pnt                     attach = myPosition;
  ComputeAttach(thecirc->Circ(), myFAttach, mySAttach, attach);
  DsgPrs_IdenticPresentation::Add(aPrs,
                                  myDrawer,
                                  vals,
                                  myPlane->Pln().Position().Ax2(),
                                  myCenter,
                                  myFAttach,
                                  mySAttach,
                                  myPosition,
                                  attach);
}

//=======================================================================
// function : ComputeAutoArcPresentation
// purpose  : Compute the presentation of the constraint where we are
//           not in the case of dragging.
//=======================================================================
void PrsDim_IdenticRelation::ComputeAutoArcPresentation(const occ::handle<Geom_Circle>& thecirc,
                                                        const gp_Pnt&                   firstp,
                                                        const gp_Pnt&                   lastp,
                                                        const bool                      isstatic)
{
  double aSegSize = thecirc->Radius() / 5.0;
  double rad      = M_PI / 5.0;

  double pFA    = ElCLib::Parameter(thecirc->Circ(), firstp);
  double pSA    = ElCLib::Parameter(thecirc->Circ(), lastp);
  double maxrad = Modulo2PI(pSA - pFA) / 2.0;

  if ((rad > maxrad) || isstatic)
    rad = maxrad;
  double pmiddle = Modulo2PI(pFA + Modulo2PI(pSA - pFA) / 2.0);

  myFAttach = ElCLib::Value(Modulo2PI(pmiddle - rad), thecirc->Circ());
  mySAttach = ElCLib::Value(Modulo2PI(pmiddle + rad), thecirc->Circ());

  gp_Pnt curpos = ElCLib::Value(pmiddle, thecirc->Circ());
  gp_Vec vtrans(myCenter, curpos);
  vtrans.Normalize();
  vtrans *= aSegSize;
  myPosition = curpos.Translated(vtrans);
}

//=======================================================================
// function : ComputeNotAutoCircPresentation
// purpose  : Compute the presentation of the constraint where we are
//           in the case of dragging.
// Note    : This function is to be used only in the case of full circles.
//           The symbol of the constraint moves together with arc
//           representing the constraint around all the circle.
//=======================================================================
void PrsDim_IdenticRelation::ComputeNotAutoCircPresentation(const occ::handle<Geom_Circle>& thecirc)
{
  gp_Pnt curpos = myPosition;

  occ::handle<Geom_Circle> cirNotAuto = new Geom_Circle(thecirc->Circ());

  // Case of confusion between the current position and the center
  // of the circle -> we move the current position
  constexpr double confusion(Precision::Confusion());
  if (myCenter.Distance(curpos) <= confusion)
  {
    gp_Vec vprec(myCenter, myFAttach);
    vprec.Normalize();
    curpos.Translate(vprec * 1e-5);
  }

  double rad      = M_PI / 5.0;
  double pcurpos  = ElCLib::Parameter(cirNotAuto->Circ(), curpos);
  double pFAttach = pcurpos - rad;
  double pSAttach = pcurpos + rad;
  myFAttach       = ElCLib::Value(pFAttach, cirNotAuto->Circ());
  mySAttach       = ElCLib::Value(pSAttach, cirNotAuto->Circ());
}

//=======================================================================
// function : ComputeNotAutoArcPresentation
// purpose  : Compute the presentation of the constraint where we are
//           in the case of dragging.
// Note    : This function is to be used only in the case of circles.
//           The symbol of the constraint moves only between myFAttach
//           and mySAttach.
//=======================================================================
void PrsDim_IdenticRelation::ComputeNotAutoArcPresentation(const occ::handle<Geom_Circle>& thecirc,
                                                           const gp_Pnt&                   pntfirst,
                                                           const gp_Pnt&                   pntlast)
{
  gp_Pnt curpos = myPosition;

  gp_Circ cirNotAuto = thecirc->Circ();

  double pFPnt  = ElCLib::Parameter(cirNotAuto, pntfirst);
  double pSPnt  = ElCLib::Parameter(cirNotAuto, pntlast);
  double deltap = Modulo2PI(pSPnt - pFPnt) / 2.0;

  double rad = M_PI / 5;
  if (deltap < rad)
  {
    myFAttach = pntfirst;
    mySAttach = pntlast;
  }
  else
  {
    gp_Pnt aFPnt = ElCLib::Value(Modulo2PI(pFPnt + rad), cirNotAuto);
    gp_Pnt aSPnt = ElCLib::Value(Modulo2PI(pSPnt - rad), cirNotAuto);

    ComputeAttach(cirNotAuto, aFPnt, aSPnt, curpos);

    double pcurpos = ElCLib::Parameter(cirNotAuto, curpos);
    myFAttach      = ElCLib::Value(pcurpos - rad, cirNotAuto);
    mySAttach      = ElCLib::Value(pcurpos + rad, cirNotAuto);
  }
}

// jfa 17/10/2000 end

// jfa 18/10/2000
//=======================================================================
// function : ComputeTwoEllipsesPresentation
// purpose  : Compute the presentation of the 'identic' constraint
//           between two ellipses (which are equal)
// input    : <theEll>: the ellipse
//           <firstp1>: first extremity of the 1st curve of the constraint
//           <lastp1> : last extremity of the 1st curve of the constraint
//           <firstp2>: first extremity of the 2nd curve of the constraint
//           <lastp2> :last extremity of the 2nd curve of the constraint
//=======================================================================
void PrsDim_IdenticRelation::ComputeTwoEllipsesPresentation(
  const occ::handle<Prs3d_Presentation>& aPrs,
  const occ::handle<Geom_Ellipse>&       theEll,
  const gp_Pnt&                          firstp1,
  const gp_Pnt&                          lastp1,
  const gp_Pnt&                          firstp2,
  const gp_Pnt&                          lastp2)
{
  constexpr double confusion(Precision::Confusion());

  // Searching of complete ellipses
  bool circ1complete = (firstp1.IsEqual(lastp1, confusion));
  bool circ2complete = (firstp2.IsEqual(lastp2, confusion));

  myCenter        = theEll->Location();
  double aSegSize = theEll->MajorRadius() / 5.0;
  double rad      = M_PI / 5.0;

  // I. Case of 2 complete ellipses
  if (circ1complete && circ2complete)
  {
    if (myAutomaticPosition)
    {
      double pfirst1 = ElCLib::Parameter(theEll->Elips(), firstp1);
      myFAttach      = ElCLib::Value(Modulo2PI(pfirst1 - rad), theEll->Elips());
      mySAttach      = ElCLib::Value(Modulo2PI(pfirst1 + rad), theEll->Elips());

      gp_Pnt curpos = ElCLib::Value(pfirst1, theEll->Elips());
      gp_Vec vtrans(myCenter, curpos);
      vtrans.Normalize();
      vtrans *= aSegSize;
      curpos.Translate(vtrans);
      myPosition = curpos;
    }
    else
      ComputeNotAutoElipsPresentation(theEll);
  }

  // II. Case of one complete circle and one arc
  else if ((circ1complete && !circ2complete) || (!circ1complete && circ2complete))
  {
    gp_Pnt firstp, lastp;
    if (circ1complete && !circ2complete)
    {
      firstp = firstp2;
      lastp  = lastp2;
    }
    else
    {
      firstp = firstp1;
      lastp  = lastp1;
    }

    if (myAutomaticPosition)
    {
      ComputeAutoArcPresentation(theEll, firstp, lastp);
    }
    else
    {
      ComputeNotAutoArcPresentation(theEll, firstp, lastp);
    }
  }

  // III and IV. Case of two arcs
  else if (!circ1complete && !circ2complete)
  {
    // We project all the points on the circle
    double pf1, pf2, pl1, pl2;
    pf1 = ElCLib::Parameter(theEll->Elips(), firstp1);
    pf2 = ElCLib::Parameter(theEll->Elips(), firstp2);
    pl1 = ElCLib::Parameter(theEll->Elips(), lastp1);
    pl2 = ElCLib::Parameter(theEll->Elips(), lastp2);

    // III. Arcs with common ends
    // III.1. First of one and last of another
    if (IsEqual2PI(pl1, pf2, confusion) || IsEqual2PI(pf1, pl2, confusion))
    {
      gp_Pnt curpos;
      double att = 0.;
      if (IsEqual2PI(pl1, pf2, confusion))
      {
        att    = pl1;
        curpos = lastp1;
      }
      else if (IsEqual2PI(pf1, pl2, confusion))
      {
        att    = pf1;
        curpos = firstp1;
      }
      double maxrad = std::min(Modulo2PI(pl1 - pf1), Modulo2PI(pl2 - pf2)) * 3 / 4;
      if (rad > maxrad)
        rad = maxrad;
      double pFAttach = Modulo2PI(att - rad);
      double pSAttach = Modulo2PI(att + rad);
      myFAttach       = ElCLib::Value(pFAttach, theEll->Elips());
      mySAttach       = ElCLib::Value(pSAttach, theEll->Elips());
      if (myAutomaticPosition)
      {
        gp_Vec vtrans(myCenter, curpos);
        vtrans.Normalize();
        vtrans *= aSegSize;
        curpos.Translate(vtrans);
        myPosition = curpos;
      }
    }
    // III.2. Two first or two last
    else if (IsEqual2PI(pf1, pf2, confusion) || IsEqual2PI(pl1, pl2, confusion))
    {
      double l1 = Modulo2PI(pl1 - pf1);
      double l2 = Modulo2PI(pl2 - pf2);
      gp_Pnt firstp, lastp;
      if (l1 < l2)
      {
        firstp = firstp1;
        lastp  = lastp1;
      }
      else
      {
        firstp = firstp2;
        lastp  = lastp2;
      }

      if (myAutomaticPosition)
      {
        ComputeAutoArcPresentation(theEll, firstp, lastp);
      }
      else
      {
        ComputeNotAutoArcPresentation(theEll, firstp, lastp);
      }
    }
    // IV. All others arcs (without common ends)
    else
    {
      // order the parameters; first will be pf1
      double pl1m = Modulo2PI(pl1 - pf1);
      double pf2m = Modulo2PI(pf2 - pf1);
      double pl2m = Modulo2PI(pl2 - pf1);

      bool case1 = false;
      // 1 - not intersecting arcs
      // 2 - intersecting arcs, but one doesn't contain another
      // 3a - first arc contains the second one
      // 3b - second arc contains the first one
      // 4 - two intersections

      gp_Pnt firstp, lastp;

      if (pl1m < pf2m) // 1 or 2b or 3b
      {
        if (pl1m < pl2m) // 1 or 3b
        {
          if (pl2m < pf2m) // 3b
          {
            firstp = firstp1;
            lastp  = lastp1;
          }
          else // 1
          {
            case1          = true;
            double deltap1 = Modulo2PI(pf1 - pl2);
            double deltap2 = Modulo2PI(pf2 - pl1);
            if (((deltap1 < deltap2) && (deltap1 < 2 * rad))
                || ((deltap2 < deltap1) && (deltap2 > 2 * rad))) // deltap2
            {
              firstp = lastp1;
              lastp  = firstp2;
            }
            else // deltap1
            {
              firstp = lastp2;
              lastp  = firstp1;
            }
          }
        }
        else // 2b
        {
          firstp = firstp1;
          lastp  = lastp2;
        }
      }
      else // 2a or 3a or 4
      {
        if (pl1m < pl2m) // 2a
        {
          firstp = firstp2;
          lastp  = lastp1;
        }
        else // 3a or 4
        {
          if (pl2m > pf2m) // 3a
          {
            firstp = firstp2;
            lastp  = lastp2;
          }
          else // 4
          {
            double deltap1 = Modulo2PI(pl1 - pf2);
            double deltap2 = Modulo2PI(pl2 - pf1);
            if (((deltap1 < deltap2) && (deltap1 < 2 * rad))
                || ((deltap2 < deltap1) && (deltap2 > 2 * rad))) // deltap2
            {
              firstp = firstp1;
              lastp  = lastp2;
            }
            else // deltap1
            {
              firstp = firstp2;
              lastp  = lastp1;
            }
          }
        }
      }

      if (myAutomaticPosition)
      {
        ComputeAutoArcPresentation(theEll, firstp, lastp, case1);
      }
      else
      {
        if (case1)
        {
          myFAttach = firstp;
          mySAttach = lastp;
        }
        else
          ComputeNotAutoArcPresentation(theEll, firstp, lastp);
      }
    }
  }

  // Display of the presentation
  TCollection_ExtendedString vals(" ==");
  gp_Pnt                     attach = myPosition;
  ComputeAttach(theEll->Elips(), myFAttach, mySAttach, attach);
  DsgPrs_IdenticPresentation::Add(aPrs,
                                  myDrawer,
                                  vals,
                                  theEll->Elips(),
                                  myFAttach,
                                  mySAttach,
                                  myPosition,
                                  attach);
}

//=======================================================================
// function : ComputeAutoArcPresentation
// purpose  : Compute the presentation of the constraint where we are
//           not in the case of dragging.
//=======================================================================
void PrsDim_IdenticRelation::ComputeAutoArcPresentation(const occ::handle<Geom_Ellipse>& theEll,
                                                        const gp_Pnt&                    firstp,
                                                        const gp_Pnt&                    lastp,
                                                        const bool                       isstatic)
{
  double aSegSize = theEll->MajorRadius() / 5.0;
  double rad      = M_PI / 5.0;

  gp_Elips anEll = theEll->Elips();

  double pFA    = ElCLib::Parameter(anEll, firstp);
  double pSA    = ElCLib::Parameter(anEll, lastp);
  double maxrad = Modulo2PI(pSA - pFA) / 2.0;

  if ((rad > maxrad) || isstatic)
    rad = maxrad;
  double pmiddle = Modulo2PI(pFA + Modulo2PI(pSA - pFA) / 2.0);

  myFAttach = ElCLib::Value(Modulo2PI(pmiddle - rad), anEll);
  mySAttach = ElCLib::Value(Modulo2PI(pmiddle + rad), anEll);

  gp_Pnt curpos = ElCLib::Value(pmiddle, anEll);
  gp_Vec vtrans(myCenter, curpos);
  vtrans.Normalize();
  vtrans *= aSegSize;
  myPosition = curpos.Translated(vtrans);
}

//=======================================================================
// function : ComputeNotAutoElipsPresentation
// purpose  : Compute the presentation of the constraint where we are
//           in the case of dragging.
// Note    : This function is to be used only in the case of ellipses.
//           The symbol of the constraint moves only between myFAttach
//           and mySAttach.
//=======================================================================
void PrsDim_IdenticRelation::ComputeNotAutoElipsPresentation(
  const occ::handle<Geom_Ellipse>& theEll)
{
  gp_Pnt curpos = myPosition;

  gp_Elips anEll = theEll->Elips();

  // Case of confusion between the current position and the center
  // of the ellipse -> we move the current position
  constexpr double confusion(Precision::Confusion());
  if (myCenter.Distance(curpos) <= confusion)
  {
    gp_Vec vprec(myCenter, myFAttach);
    vprec.Normalize();
    curpos.Translate(vprec * 1e-5);
  }

  double rad = M_PI / 5.0;
  //  double pcurpos = ElCLib::Parameter(anEll,curpos);
  GeomAPI_ProjectPointOnCurve aProj(curpos, theEll);
  double                      pcurpos = aProj.LowerDistanceParameter();

  double pFAttach = pcurpos - rad;
  double pSAttach = pcurpos + rad;
  myFAttach       = ElCLib::Value(pFAttach, anEll);
  mySAttach       = ElCLib::Value(pSAttach, anEll);
}

//=======================================================================
// function : ComputeNotAutoArcPresentation
// purpose  : Compute the presentation of the constraint where we are
//           in the case of dragging.
// Note    : This function is to be used only in the case of ellipses.
//           The symbol of the constraint moves only between myFAttach
//           and mySAttach.
//=======================================================================
void PrsDim_IdenticRelation::ComputeNotAutoArcPresentation(const occ::handle<Geom_Ellipse>& theEll,
                                                           const gp_Pnt& pntfirst,
                                                           const gp_Pnt& pntlast)
{
  gp_Pnt curpos = myPosition;

  gp_Elips anEll = theEll->Elips();

  double pFPnt  = ElCLib::Parameter(anEll, pntfirst);
  double pSPnt  = ElCLib::Parameter(anEll, pntlast);
  double deltap = Modulo2PI(pSPnt - pFPnt) / 2.0;

  double rad = M_PI / 5;
  if (deltap < rad)
  {
    myFAttach = pntfirst;
    mySAttach = pntlast;
  }
  else
  {
    gp_Pnt aFPnt = ElCLib::Value(Modulo2PI(pFPnt + rad), anEll);
    gp_Pnt aSPnt = ElCLib::Value(Modulo2PI(pSPnt - rad), anEll);

    ComputeAttach(anEll, aFPnt, aSPnt, curpos);

    //      double pcurpos = ElCLib::Parameter(anEll,curpos);
    GeomAPI_ProjectPointOnCurve aProj(curpos, theEll);
    double                      pcurpos = aProj.LowerDistanceParameter();

    myFAttach = ElCLib::Value(pcurpos - rad, anEll);
    mySAttach = ElCLib::Value(pcurpos + rad, anEll);
  }
}

// jfa 18/10/2000 end

//=================================================================================================

void PrsDim_IdenticRelation::ComputeTwoVerticesPresentation(
  const occ::handle<Prs3d_Presentation>& aPrs)
{
  bool                 isOnPlane1, isOnPlane2;
  const TopoDS_Vertex& FVertex = TopoDS::Vertex(myFShape);
  const TopoDS_Vertex& SVertex = TopoDS::Vertex(mySShape);

  PrsDim::ComputeGeometry(FVertex, myFAttach, myPlane, isOnPlane1);
  PrsDim::ComputeGeometry(SVertex, mySAttach, myPlane, isOnPlane2);

  if (isOnPlane1 && isOnPlane2)
    myExtShape = 0;
  else if (isOnPlane1 && !isOnPlane2)
    myExtShape = 2;
  else if (!isOnPlane1 && isOnPlane2)
    myExtShape = 1;
  else
    return;

  // The attachment points are the points themselves that must be
  // identical
  myFAttach = BRep_Tool::Pnt(FVertex);
  mySAttach = myFAttach;

  gp_Pnt curpos;
  if (myAutomaticPosition)
  {
    // Computation of the size of the symbol
    double symbsize = ComputeSegSize();
    if (symbsize <= Precision::Confusion())
      symbsize = 1.;
    symbsize *= 5;
    // Computation of the direction of the segment of the presentation
    // we take the median of the edges connected to vertices
    gp_Dir                                                      dF, dS;
    gp_Dir                                                      myDir;
    NCollection_List<occ::handle<Standard_Transient>>::Iterator it(Users());
    if (it.More())
    {
      occ::handle<AIS_Shape> USER(occ::down_cast<AIS_Shape>(it.Value()));
      if (!USER.IsNull())
      {
        const TopoDS_Shape& SH = USER->Shape();
        if ((!SH.IsNull()) && (SH.ShapeType() == TopAbs_WIRE))
        {
          const TopoDS_Wire& WIRE = TopoDS::Wire(USER->Shape());
          bool               done = ComputeDirection(WIRE, FVertex, dF);
          if (!done)
            return;
          done = ComputeDirection(WIRE, SVertex, dS);
          if (!done)
            return;
        }
        else
          return;
      }
      else
        return;

      // computation of the segment direction like average
      // of the 2 computed directions.
      if (dF.IsParallel(dS, Precision::Angular()))
      {
        myDir = dF.Crossed(myPlane->Pln().Axis().Direction());
      }
      else
      {
        myDir.SetXYZ(dF.XYZ() + dS.XYZ());
      }
      curpos = myFAttach.Translated(gp_Vec(myDir) * symbsize);
    }
    // jfa 11/10/2000
    else
    {
      curpos = myFAttach;
    }
    // jfa 11/10/2000 end

    myPosition          = curpos;
    myAutomaticPosition = false;
  }
  else
  {
    curpos = myPosition;
  }

  // Presentation computation
  TCollection_ExtendedString vals(" ++");
  DsgPrs_IdenticPresentation::Add(aPrs, myDrawer, vals, myFAttach, curpos);
  // Calculate the projection of vertex
  if (myExtShape == 1)
    ComputeProjVertexPresentation(aPrs, FVertex, myFAttach);
  else if (myExtShape == 2)
    ComputeProjVertexPresentation(aPrs, SVertex, mySAttach);
}

//=================================================================================================

double PrsDim_IdenticRelation::ComputeSegSize() const
{
  return 1.;
}

//=======================================================================
// function : ComputeDirection
// purpose  : Compute a direction according to the different geometric
//           elements connected to the vertex <VERT>, in way to not have
//           overlap between the symbol and them.
//=======================================================================
bool PrsDim_IdenticRelation::ComputeDirection(const TopoDS_Wire&   aWire,
                                              const TopoDS_Vertex& VERT,
                                              gp_Dir&              dF) const
{
  // we take the median of the edges connected to vertices
  TopoDS_Edge edg1, edg2;
  ConnectedEdges(aWire, VERT, edg1, edg2);

  if (edg1.IsNull() && edg2.IsNull())
  {
    return false;
  }

  occ::handle<Geom_Curve> curv1, curv2;
  gp_Pnt                  firstp1, lastp1, firstp2, lastp2;

  // Case with 2 edges connected to the vertex <VERT>
  if (!edg1.IsNull() && !edg2.IsNull())
  {
    if (!PrsDim::ComputeGeometry(edg1,
                                 edg2,
                                 curv1,
                                 curv2,
                                 firstp1,
                                 lastp1,
                                 firstp2,
                                 lastp2,
                                 myPlane))
      return false;

    gp_Dir d1, d2;
    if (curv1->IsInstance(STANDARD_TYPE(Geom_Circle)))
    {
      d1 = ComputeCircleDirection(occ::down_cast<Geom_Circle>(curv1), VERT);
    }
    else if (curv1->IsInstance(STANDARD_TYPE(Geom_Line)))
    {
      d1 = ComputeLineDirection(occ::down_cast<Geom_Line>(curv1), firstp1);
    }
    else
      return false;

    if (curv2->IsInstance(STANDARD_TYPE(Geom_Circle)))
    {
      d2 = ComputeCircleDirection(occ::down_cast<Geom_Circle>(curv2), VERT);
    }
    else if (curv2->IsInstance(STANDARD_TYPE(Geom_Line)))
    {
      d2 = ComputeLineDirection(occ::down_cast<Geom_Line>(curv2), firstp2);
    }
    else
      return false;

    if (!d1.IsParallel(d2, Precision::Angular()))
      dF.SetXYZ((d1.XYZ() + d2.XYZ()) / 2);
    else
    {
      dF = d1.Crossed(myPlane->Pln().Axis().Direction());
    }
  }

  // Case where <VERT> is at an extremity of a wire.
  else
  {
    TopoDS_Edge VEdge;
    if (!edg1.IsNull())
      VEdge = edg1;
    else if (!edg2.IsNull())
      VEdge = edg2;
    else
      return false;

    if (!PrsDim::ComputeGeometry(VEdge, curv1, firstp1, lastp1))
      return false;
    if (curv1->IsInstance(STANDARD_TYPE(Geom_Circle)))
    {
      dF = ComputeCircleDirection(occ::down_cast<Geom_Circle>(curv1), VERT);
    }
    else if (curv1->IsInstance(STANDARD_TYPE(Geom_Line)))
    {
      dF = ComputeLineDirection(occ::down_cast<Geom_Line>(curv1), firstp1);
    }
    else
      return false;
  }

  return true;
}

//=================================================================================================

gp_Dir PrsDim_IdenticRelation::ComputeLineDirection(const occ::handle<Geom_Line>& lin,
                                                    const gp_Pnt&                 firstP) const
{
  gp_Dir dir;
  dir = lin->Lin().Direction();
  if (!myFAttach.IsEqual(firstP, Precision::Confusion()))
    dir.Reverse();
  return dir;
}

//=================================================================================================

gp_Dir PrsDim_IdenticRelation::ComputeCircleDirection(const occ::handle<Geom_Circle>& circ,
                                                      const TopoDS_Vertex&            VERT) const
{
  gp_Vec V(circ->Location(), BRep_Tool::Pnt(VERT));
  return gp_Dir(V);
}

//=================================================================================================

void PrsDim_IdenticRelation::ComputeOneEdgeOVertexPresentation(
  const occ::handle<Prs3d_Presentation>& aPrs)
{
  TopoDS_Vertex V;
  TopoDS_Edge   E;
  int           numedge;

  if (myFShape.ShapeType() == TopAbs_VERTEX)
  {
    V       = TopoDS::Vertex(myFShape);
    E       = TopoDS::Edge(mySShape);
    numedge = 2; // edge = 2nd shape
  }
  else
  {
    V       = TopoDS::Vertex(mySShape);
    E       = TopoDS::Edge(myFShape);
    numedge = 1; // edge = 1st shape
  }
  gp_Pnt                  ptonedge1, ptonedge2;
  occ::handle<Geom_Curve> aCurve;
  occ::handle<Geom_Curve> extCurv;
  bool                    isInfinite;
  bool                    isOnPlanEdge, isOnPlanVertex;
  if (!PrsDim::ComputeGeometry(E,
                               aCurve,
                               ptonedge1,
                               ptonedge2,
                               extCurv,
                               isInfinite,
                               isOnPlanEdge,
                               myPlane))
    return;
  aPrs->SetInfiniteState(isInfinite);
  PrsDim::ComputeGeometry(V, myFAttach, myPlane, isOnPlanVertex);

  // only the curve can be projected
  if (!isOnPlanEdge && !isOnPlanVertex)
    return;

  if (!isOnPlanEdge)
  {
    if (numedge == 1)
      myExtShape = 1;
    else
      myExtShape = 2;
  }
  else if (!isOnPlanVertex)
  {
    if (numedge == 1)
      myExtShape = 2;
    else
      myExtShape = 1;
  }
  // The attachment points are the point
  myFAttach = BRep_Tool::Pnt(V);
  mySAttach = myFAttach;

  gp_Pnt curpos;
  if (myAutomaticPosition)
  {
    // Computation of the size of the symbol
    double symbsize = ComputeSegSize();
    symbsize *= 5;
    // Computation of the direction of the segment of the presentation
    // we take the median of the edges connected to vertices
    gp_Dir myDir;
    if (aCurve->IsKind(STANDARD_TYPE(Geom_Line)))
    {
      myDir = occ::down_cast<Geom_Line>(aCurve)->Lin().Direction();
      myDir.Cross(myPlane->Pln().Axis().Direction());
    }
    else if (aCurve->IsKind(STANDARD_TYPE(Geom_Circle)))
    {
      occ::handle<Geom_Circle> CIR = occ::down_cast<Geom_Circle>(aCurve);
      myDir.SetXYZ(myFAttach.XYZ() - CIR->Location().XYZ());
    }
    // jfa 10/10/2000
    else if (aCurve->IsKind(STANDARD_TYPE(Geom_Ellipse)))
    {
      occ::handle<Geom_Ellipse> CIR = occ::down_cast<Geom_Ellipse>(aCurve);
      myDir.SetXYZ(myFAttach.XYZ() - CIR->Location().XYZ());
    }
    // jfa 10/10/2000 end

    curpos              = myFAttach.Translated(gp_Vec(myDir) * symbsize);
    myPosition          = curpos;
    myAutomaticPosition = true;
  }
  else
  {
    curpos = myPosition;
  }

  // Presentation computation
  TCollection_ExtendedString vals(" -+-");
  DsgPrs_IdenticPresentation::Add(aPrs, myDrawer, vals, myFAttach, curpos);
  if (myExtShape != 0)
  {
    if (!extCurv.IsNull())
    { // the edge is not in the WP
      ComputeProjEdgePresentation(aPrs, E, occ::down_cast<Geom_Line>(aCurve), ptonedge1, ptonedge2);
    }
  }
}
