// Created on: 1998-01-23
// Created by: Sergey ZARITCHNY
// Copyright (c) 1998-1999 Matra Datavision
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

#include <PrsDim.hxx>
#include <PrsDim_MaxRadiusDimension.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <DsgPrs_EllipseRadiusPresentation.hxx>
#include <ElCLib.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsDim_MaxRadiusDimension, PrsDim_EllipseRadiusDimension)

//=================================================================================================

PrsDim_MaxRadiusDimension::PrsDim_MaxRadiusDimension(const TopoDS_Shape&               aShape,
                                                     const Standard_Real               aVal,
                                                     const TCollection_ExtendedString& aText)
    : PrsDim_EllipseRadiusDimension(aShape, aText)
{
  myVal               = aVal;
  mySymbolPrs         = DsgPrs_AS_LASTAR;
  myAutomaticPosition = Standard_True;
  myArrowSize         = myVal / 100.;
}

//=================================================================================================

PrsDim_MaxRadiusDimension::PrsDim_MaxRadiusDimension(const TopoDS_Shape&               aShape,
                                                     const Standard_Real               aVal,
                                                     const TCollection_ExtendedString& aText,
                                                     const gp_Pnt&                     aPosition,
                                                     const DsgPrs_ArrowSide            aSymbolPrs,
                                                     const Standard_Real               anArrowSize)
    : PrsDim_EllipseRadiusDimension(aShape, aText)
{
  myVal               = aVal;
  mySymbolPrs         = aSymbolPrs;
  myPosition          = aPosition;
  myAutomaticPosition = Standard_False;
  SetArrowSize(anArrowSize);
}

//=================================================================================================

void PrsDim_MaxRadiusDimension::Compute(const Handle(PrsMgr_PresentationManager)&,
                                        const Handle(Prs3d_Presentation)& aPresentation,
                                        const Standard_Integer)
{
  //  if( myAutomaticPosition )
  { // ota : recompute in any case
    ComputeGeometry();
    myEllipse.SetMajorRadius(myVal);
    gp_Vec v1(myEllipse.XAxis().Direction());
    v1 *= myVal;
    myApexP = myEllipse.Location().Translated(v1);
    myApexN = myEllipse.Location().Translated(-v1);
  }
  if (myIsAnArc)
    ComputeArcOfEllipse(aPresentation);
  else
    ComputeEllipse(aPresentation);
}

//=================================================================================================

void PrsDim_MaxRadiusDimension::ComputeEllipse(const Handle(Prs3d_Presentation)& aPresentation)
{

  Handle(Prs3d_DimensionAspect) la  = myDrawer->DimensionAspect();
  Handle(Prs3d_ArrowAspect)     arr = la->ArrowAspect();

  // size
  if (!myArrowSizeIsDefined)
  {
    myArrowSize = Min(myArrowSize, myVal / 5.);
  }
  arr->SetLength(myArrowSize);

  Standard_Real U; //,V;
  gp_Pnt        curPos, Center;
  Center = myEllipse.Location();
  if (myAutomaticPosition)
  {
    myPosition          = Center;
    myEndOfArrow        = myApexP;
    myAutomaticPosition = Standard_True;

    if (myIsSetBndBox)
      myPosition =
        PrsDim::TranslatePointToBound(myPosition, gp_Dir(gp_Vec(Center, myPosition)), myBndBox);
    curPos = myPosition;
  }
  else //! AutomaticPosition
  {
    curPos = myPosition;
    gp_Lin L1(myEllipse.XAxis());
    U      = ElCLib::Parameter(L1, curPos);
    curPos = ElCLib::Value(U, L1);
    if (curPos.Distance(myApexP) < curPos.Distance(myApexN))
      myEndOfArrow = myApexP;
    else
      myEndOfArrow = myApexN;
  }

  // Presenatation
  DsgPrs_EllipseRadiusPresentation::Add(aPresentation,
                                        myDrawer,
                                        myVal,
                                        myText,
                                        curPos,
                                        myEndOfArrow,
                                        Center,
                                        Standard_True,
                                        mySymbolPrs);
}

//=================================================================================================

void PrsDim_MaxRadiusDimension::ComputeArcOfEllipse(const Handle(Prs3d_Presentation)& aPresentation)
{

  Handle(Prs3d_DimensionAspect) la  = myDrawer->DimensionAspect();
  Handle(Prs3d_ArrowAspect)     arr = la->ArrowAspect();

  // size
  if (!myArrowSizeIsDefined)
  {
    myArrowSize = Min(myArrowSize, myVal / 5.);
  }
  arr->SetLength(myArrowSize);

  Standard_Real par;
  gp_Pnt        curPos, Center;
  Center                      = myEllipse.Location();
  Standard_Boolean IsInDomain = Standard_True;
  if (myAutomaticPosition)
  {
    myEndOfArrow =
      PrsDim::NearestApex(myEllipse, myApexP, myApexN, myFirstPar, myLastPar, IsInDomain);
    myPosition          = Center;
    myAutomaticPosition = Standard_True;
    if (myIsSetBndBox)
      myPosition =
        PrsDim::TranslatePointToBound(myPosition, gp_Dir(gp_Vec(Center, myPosition)), myBndBox);
    curPos = myPosition;
  }
  else //! AutomaticPosition
  {
    curPos = myPosition;
    //      ElSLib::Parameters ( myPlane->Pln(), curPos, U, V );
    //      curPos = ElSLib::Value (U, V, myPlane->Pln());
    gp_Lin L1(myEllipse.XAxis());
    par    = ElCLib::Parameter(L1, curPos);
    curPos = ElCLib::Value(par, L1);
    if (curPos.Distance(myApexP) < curPos.Distance(myApexN))
      myEndOfArrow = myApexP;
    else
      myEndOfArrow = myApexN;
    par        = ElCLib::Parameter(myEllipse, myEndOfArrow);
    IsInDomain = PrsDim::InDomain(myFirstPar, myLastPar, par);
    myPosition = curPos;
  }

  //  Standard_Real parEnd = ElCLib::Parameter ( myEllipse, myEndOfArrow );
  Standard_Real parStart = 0.;
  if (!IsInDomain)
  {
    if (PrsDim::DistanceFromApex(myEllipse, myEndOfArrow, myFirstPar)
        < PrsDim::DistanceFromApex(myEllipse, myEndOfArrow, myLastPar))
      parStart = myFirstPar;
    else
      parStart = myLastPar;
  }
  if (!myIsOffset)
    DsgPrs_EllipseRadiusPresentation::Add(aPresentation,
                                          myDrawer,
                                          myVal,
                                          myText,
                                          myEllipse,
                                          curPos,
                                          myEndOfArrow,
                                          Center,
                                          parStart,
                                          IsInDomain,
                                          Standard_True,
                                          mySymbolPrs);
  else
    DsgPrs_EllipseRadiusPresentation::Add(aPresentation,
                                          myDrawer,
                                          myVal,
                                          myText,
                                          myOffsetCurve,
                                          curPos,
                                          myEndOfArrow,
                                          Center,
                                          parStart,
                                          IsInDomain,
                                          Standard_True,
                                          mySymbolPrs);
}

//=================================================================================================

void PrsDim_MaxRadiusDimension::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                                 const Standard_Integer /*aMode*/)
{

  gp_Pnt        center          = myEllipse.Location();
  gp_Pnt        AttachmentPoint = myPosition;
  Standard_Real dist            = center.Distance(AttachmentPoint);
  Standard_Real aRadius         = myVal;
  // Standard_Real inside  = Standard_False;
  gp_Pnt pt1;
  if (dist > aRadius)
    pt1 = AttachmentPoint;
  else
    pt1 = myEndOfArrow;
  Handle(SelectMgr_EntityOwner)     own = new SelectMgr_EntityOwner(this, 7);
  Handle(Select3D_SensitiveSegment) seg = new Select3D_SensitiveSegment(own, center, pt1);
  aSelection->Add(seg);

  // Text
  Standard_Real                 size(Min(myVal / 100. + 1.e-6, myArrowSize + 1.e-6));
  Handle(Select3D_SensitiveBox) box = new Select3D_SensitiveBox(own,
                                                                AttachmentPoint.X(),
                                                                AttachmentPoint.Y(),
                                                                AttachmentPoint.Z(),
                                                                AttachmentPoint.X() + size,
                                                                AttachmentPoint.Y() + size,
                                                                AttachmentPoint.Z() + size);
  aSelection->Add(box);

  // Arc of Ellipse
  if (myIsAnArc)
  {

    Standard_Real parEnd = ElCLib::Parameter(myEllipse, myEndOfArrow);
    if (!PrsDim::InDomain(myFirstPar, myLastPar, parEnd))
    {
      Standard_Real parStart, par;
      if (PrsDim::DistanceFromApex(myEllipse, myEndOfArrow, myFirstPar)
          < PrsDim::DistanceFromApex(myEllipse, myEndOfArrow, myLastPar))
        par = myFirstPar;
      else
        par = myLastPar;
      gp_Vec Vapex(center, ElCLib::Value(parEnd, myEllipse));
      gp_Vec Vpnt(center, ElCLib::Value(par, myEllipse));
      gp_Dir dir(Vpnt ^ Vapex);
      if (myEllipse.Position().Direction().IsOpposite(dir, Precision::Angular()))
      {
        parStart = parEnd;
        parEnd   = par;
      }
      else
        parStart = par;

      Handle(Geom_Curve) TrimCurve;
      if (myIsOffset)
      {
        Handle(Geom_Curve) aCurve = myOffsetCurve;
        TrimCurve                 = new Geom_TrimmedCurve(aCurve, parStart, parEnd);
      }
      else
      {
        Handle(Geom_Ellipse) Ellipse = new Geom_Ellipse(myEllipse);
        TrimCurve                    = new Geom_TrimmedCurve(Ellipse, parStart, parEnd);
      }
      Handle(Select3D_SensitiveCurve) SensArc;
      SensArc = new Select3D_SensitiveCurve(own, TrimCurve);
      aSelection->Add(SensArc);
    }
  }
}
