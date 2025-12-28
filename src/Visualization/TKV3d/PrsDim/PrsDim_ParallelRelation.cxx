// Created on: 1996-12-05
// Created by: Jean-Pierre COMBE/Odile Olivier
// Copyright (c) 1996-1999 Matra Datavision
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

#include <PrsDim_ParallelRelation.hxx>

#include <PrsDim.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <DsgPrs_LengthPresentation.hxx>
#include <ElCLib.hxx>
#include <gce_MakeLin.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_NotImplemented.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsDim_ParallelRelation, PrsDim_Relation)

//=================================================================================================

PrsDim_ParallelRelation::PrsDim_ParallelRelation(const TopoDS_Shape&       aFShape,
                                                 const TopoDS_Shape&       aSShape,
                                                 const occ::handle<Geom_Plane>& aPlane)
{
  myFShape            = aFShape;
  mySShape            = aSShape;
  myPlane             = aPlane;
  myAutomaticPosition = true;
  myArrowSize         = 0.01;
  mySymbolPrs         = DsgPrs_AS_BOTHAR;
}

//=================================================================================================

PrsDim_ParallelRelation::PrsDim_ParallelRelation(const TopoDS_Shape&       aFShape,
                                                 const TopoDS_Shape&       aSShape,
                                                 const occ::handle<Geom_Plane>& aPlane,
                                                 const gp_Pnt&             aPosition,
                                                 const DsgPrs_ArrowSide    aSymbolPrs,
                                                 const double       anArrowSize)
{
  myFShape            = aFShape;
  mySShape            = aSShape;
  myPlane             = aPlane;
  myAutomaticPosition = false;
  SetArrowSize(anArrowSize);
  myPosition  = aPosition;
  mySymbolPrs = aSymbolPrs;
}

//=================================================================================================

void PrsDim_ParallelRelation::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                                      const occ::handle<Prs3d_Presentation>& aPresentation,
                                      const int)
{
  switch (myFShape.ShapeType())
  {
    case TopAbs_FACE: {
      // parallel case between two faces
      ComputeTwoFacesParallel(aPresentation);
    }
    break;
    case TopAbs_EDGE: {
      // parallel case between two edges
      ComputeTwoEdgesParallel(aPresentation);
    }
    break;
    default:
      break;
  }
}

//=================================================================================================

void PrsDim_ParallelRelation::ComputeSelection(const occ::handle<SelectMgr_Selection>& aSelection,
                                               const int)
{
  gp_Lin L1(myFAttach, myDirAttach);
  gp_Lin L2(mySAttach, myDirAttach);
  gp_Pnt Proj1 = ElCLib::Value(ElCLib::Parameter(L1, myPosition), L1);
  gp_Pnt Proj2 = ElCLib::Value(ElCLib::Parameter(L2, myPosition), L2);

  gp_Lin                        L3;
  occ::handle<SelectMgr_EntityOwner> own = new SelectMgr_EntityOwner(this, 7);

  if (!Proj1.IsEqual(Proj2, Precision::Confusion()))
  {
    L3 = gce_MakeLin(Proj1, Proj2);
  }
  else
  {
    L3 = gce_MakeLin(Proj1, myDirAttach);
    double                 size(std::min(myVal / 100. + 1.e-6, myArrowSize + 1.e-6));
    occ::handle<Select3D_SensitiveBox> box = new Select3D_SensitiveBox(own,
                                                                  myPosition.X(),
                                                                  myPosition.Y(),
                                                                  myPosition.Z(),
                                                                  myPosition.X() + size,
                                                                  myPosition.Y() + size,
                                                                  myPosition.Z() + size);
    aSelection->Add(box);
  }
  double parmin, parmax, parcur;
  parmin = ElCLib::Parameter(L3, Proj1);
  parmax = parmin;

  parcur = ElCLib::Parameter(L3, Proj2);
  parmin = std::min(parmin, parcur);
  parmax = std::max(parmax, parcur);

  parcur = ElCLib::Parameter(L3, myPosition);
  parmin = std::min(parmin, parcur);
  parmax = std::max(parmax, parcur);

  gp_Pnt PointMin = ElCLib::Value(parmin, L3);
  gp_Pnt PointMax = ElCLib::Value(parmax, L3);

  occ::handle<Select3D_SensitiveSegment> seg;

  if (!PointMin.IsEqual(PointMax, Precision::Confusion()))
  {
    seg = new Select3D_SensitiveSegment(own, PointMin, PointMax);
    aSelection->Add(seg);
  }
  if (!myFAttach.IsEqual(Proj1, Precision::Confusion()))
  {
    seg = new Select3D_SensitiveSegment(own, myFAttach, Proj1);
    aSelection->Add(seg);
  }
  if (!mySAttach.IsEqual(Proj2, Precision::Confusion()))
  {
    seg = new Select3D_SensitiveSegment(own, mySAttach, Proj2);
    aSelection->Add(seg);
  }
}

//=================================================================================================

void PrsDim_ParallelRelation::ComputeTwoFacesParallel(const occ::handle<Prs3d_Presentation>&)
{
  throw Standard_NotImplemented("PrsDim_ParallelRelation::ComputeTwoFacesParallel not implemented");
}

//=================================================================================================

void PrsDim_ParallelRelation::ComputeTwoEdgesParallel(
  const occ::handle<Prs3d_Presentation>& aPresentation)
{
  TopoDS_Edge E1 = TopoDS::Edge(myFShape);
  TopoDS_Edge E2 = TopoDS::Edge(mySShape);

  gp_Pnt             ptat11, ptat12, ptat21, ptat22; //,pint3d;
  occ::handle<Geom_Curve> geom1, geom2;
  bool   isInfinite1, isInfinite2;
  occ::handle<Geom_Curve> extCurv;
  if (!PrsDim::ComputeGeometry(E1,
                               E2,
                               myExtShape,
                               geom1,
                               geom2,
                               ptat11,
                               ptat12,
                               ptat21,
                               ptat22,
                               extCurv,
                               isInfinite1,
                               isInfinite2,
                               myPlane))
  {
    return;
  }

  aPresentation->SetInfiniteState((isInfinite1 || isInfinite2) && (myExtShape != 0));

  gp_Lin           l1;
  gp_Lin           l2;
  bool isEl1 = false, isEl2 = false;

  if (geom1->IsInstance(STANDARD_TYPE(Geom_Ellipse)))
  {
    occ::handle<Geom_Ellipse> geom_el1(occ::down_cast<Geom_Ellipse>(geom1));
    // construct lines through focuses
    gp_Ax1 elAx            = geom_el1->XAxis();
    l1                     = gp_Lin(elAx);
    double focex    = geom_el1->MajorRadius() - geom_el1->Focal() / 2.0;
    gp_Vec        transvec = gp_Vec(elAx.Direction()) * focex;
    ptat11                 = geom_el1->Focus1().Translated(transvec);
    ptat12                 = geom_el1->Focus2().Translated(-transvec);
    isEl1                  = true;
  }
  else if (geom1->IsInstance(STANDARD_TYPE(Geom_Line)))
  {
    occ::handle<Geom_Line> geom_lin1(occ::down_cast<Geom_Line>(geom1));
    l1 = geom_lin1->Lin();
  }
  else
    return;

  if (geom2->IsInstance(STANDARD_TYPE(Geom_Ellipse)))
  {
    occ::handle<Geom_Ellipse> geom_el2(occ::down_cast<Geom_Ellipse>(geom2));
    // construct lines through focuses
    gp_Ax1 elAx            = geom_el2->XAxis();
    l2                     = gp_Lin(elAx);
    double focex    = geom_el2->MajorRadius() - geom_el2->Focal() / 2.0;
    gp_Vec        transvec = gp_Vec(elAx.Direction()) * focex;
    ptat21                 = geom_el2->Focus1().Translated(transvec);
    ptat22                 = geom_el2->Focus2().Translated(-transvec);
    isEl2                  = true;
  }
  else if (geom2->IsInstance(STANDARD_TYPE(Geom_Line)))
  {
    occ::handle<Geom_Line> geom_lin2(occ::down_cast<Geom_Line>(geom2));
    l2 = geom_lin2->Lin();
  }
  else
    return;

  const occ::handle<Geom_Line>& geom_lin1 = new Geom_Line(l1);
  const occ::handle<Geom_Line>& geom_lin2 = new Geom_Line(l2);

  myDirAttach = l1.Direction();
  // size
  if (!myArrowSizeIsDefined)
  {
    double arrSize1(myArrowSize), arrSize2(myArrowSize);
    if (!isInfinite1)
      arrSize1 = ptat11.Distance(ptat12) / 50.;
    if (!isInfinite2)
      arrSize2 = ptat21.Distance(ptat22) / 50.;
    myArrowSize = std::max(myArrowSize, std::max(arrSize1, arrSize2));
    //  myArrowSize = std::min(myArrowSize,Min(arrSize1,arrSize2));
  }

  if (myAutomaticPosition)
  {
    gp_Pnt curpos;
    if (!isInfinite1)
    {
      gp_Pnt p2 = ElCLib::Value(ElCLib::Parameter(l2, ptat11), l2);
      curpos.SetXYZ((ptat11.XYZ() + p2.XYZ()) / 2.);
    }
    else if (!isInfinite2)
    {
      gp_Pnt p2 = ElCLib::Value(ElCLib::Parameter(l1, ptat21), l1);
      curpos.SetXYZ((ptat21.XYZ() + p2.XYZ()) / 2.);
    }
    else
    {
      curpos.SetXYZ((l1.Location().XYZ() + l2.Location().XYZ()) / 2.);
    }
    // offset pour eviter confusion Edge et Dimension
    gp_Vec offset(myDirAttach);
    offset = offset * myArrowSize * (-10.);
    curpos.Translate(offset);
    myPosition = curpos;
  }

  // search for attachment points
  if (!isInfinite1)
  {
    if (isEl1)
    {
      if (myPosition.Distance(ptat11) < myPosition.Distance(ptat12))
        myFAttach = ptat12;
      else
        myFAttach = ptat11;
    }
    else
    {
      if (myPosition.Distance(ptat11) > myPosition.Distance(ptat12))
        myFAttach = ptat12;
      else
        myFAttach = ptat11;
    }
  }
  else
  {
    myFAttach = ElCLib::Value(ElCLib::Parameter(l1, myPosition), l1);
  }

  if (!isInfinite2)
  {
    if (isEl2)
    {
      if (myPosition.Distance(ptat21) < myPosition.Distance(ptat22))
        mySAttach = ptat22;
      else
        mySAttach = ptat21;
    }
    else
    {
      if (myPosition.Distance(ptat21) > myPosition.Distance(ptat22))
        mySAttach = ptat22;
      else
        mySAttach = ptat21;
    }
  }
  else
  {
    mySAttach = ElCLib::Value(ElCLib::Parameter(l2, myPosition), l2);
  }
  TCollection_ExtendedString aText(" //");

  if (l1.Distance(l2) <= Precision::Confusion())
  {
    myArrowSize = 0.;
  }
  occ::handle<Prs3d_DimensionAspect> la  = myDrawer->DimensionAspect();
  occ::handle<Prs3d_ArrowAspect>     arr = la->ArrowAspect();
  arr->SetLength(myArrowSize);
  arr = la->ArrowAspect();
  arr->SetLength(myArrowSize);
  if (myExtShape == 1)
    mySymbolPrs = DsgPrs_AS_FIRSTPT_LASTAR;
  else if (myExtShape == 2)
    mySymbolPrs = DsgPrs_AS_FIRSTAR_LASTPT;

  DsgPrs_LengthPresentation::Add(aPresentation,
                                 myDrawer,
                                 aText,
                                 myFAttach,
                                 mySAttach,
                                 myDirAttach,
                                 myPosition,
                                 mySymbolPrs);
  if ((myExtShape != 0) && !extCurv.IsNull())
  {
    gp_Pnt pf, pl;
    if (myExtShape == 1)
    {
      if (!isInfinite1)
      {
        pf = ptat11;
        pl = ptat12;
      }
      ComputeProjEdgePresentation(aPresentation, E1, geom_lin1, pf, pl);
    }
    else
    {
      if (!isInfinite2)
      {
        pf = ptat21;
        pl = ptat22;
      }
      ComputeProjEdgePresentation(aPresentation, E2, geom_lin2, pf, pl);
    }
  }
}
