// Created on: 1996-12-13
// Created by: Jean-Pierre COMBE
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

// + X/YAxis() returns AIS_Line instead of AIS_Axis
// + (-1) selection mode token into account
// (SAMTECH specific)

#include <AIS_Line.hxx>
#include <AIS_PlaneTrihedron.hxx>
#include <AIS_Point.hxx>
#include <DsgPrs_XYZAxisPresentation.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_Structure.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Quantity_Color.hxx>
#include <Select3D_SensitivePoint.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array1.hxx>
#include <TCollection_AsciiString.hxx>
#include <UnitsAPI.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_PlaneTrihedron, AIS_InteractiveObject)

void ExtremityPoints(NCollection_Array1<gp_Pnt>&      PP,
                     const occ::handle<Geom_Plane>&   myPlane,
                     const occ::handle<Prs3d_Drawer>& myDrawer);

//=================================================================================================

AIS_PlaneTrihedron::AIS_PlaneTrihedron(const occ::handle<Geom_Plane>& aPlane)
    : myPlane(aPlane)
{
  occ::handle<Prs3d_DatumAspect> DA = new Prs3d_DatumAspect();
  // POP  double aLength = UnitsAPI::CurrentFromLS (100. ,"LENGTH");
  double aLength = UnitsAPI::AnyToLS(100., "mm");
  DA->SetAxisLength(aLength, aLength, aLength);
  Quantity_Color col(Quantity_NOC_ROYALBLUE1);
  DA->LineAspect(Prs3d_DatumParts_XAxis)->SetColor(col);
  DA->LineAspect(Prs3d_DatumParts_YAxis)->SetColor(col);
  DA->SetDrawDatumAxes(Prs3d_DatumAxes_XYAxes);
  myDrawer->SetDatumAspect(DA); // odl - specific is created because it is modified
  myShapes[0] = Position();
  myShapes[1] = XAxis();
  myShapes[2] = YAxis();

  myXLabel = TCollection_AsciiString("X");
  myYLabel = TCollection_AsciiString("Y");
}

//=================================================================================================

occ::handle<Geom_Plane> AIS_PlaneTrihedron::Component()
{
  return myPlane;
}

//=================================================================================================

void AIS_PlaneTrihedron::SetComponent(const occ::handle<Geom_Plane>& aPlane)
{
  myPlane = aPlane;
}

//=================================================================================================

occ::handle<AIS_Line> AIS_PlaneTrihedron::XAxis() const
{
  occ::handle<Geom_Line> aGLine = new Geom_Line(myPlane->Pln().XAxis());
  occ::handle<AIS_Line>  aLine  = new AIS_Line(aGLine);
  aLine->SetColor(Quantity_NOC_ROYALBLUE1);
  return aLine;
}

//=================================================================================================

occ::handle<AIS_Line> AIS_PlaneTrihedron::YAxis() const
{
  occ::handle<Geom_Line> aGLine = new Geom_Line(myPlane->Pln().YAxis());
  occ::handle<AIS_Line>  aLine  = new AIS_Line(aGLine);
  aLine->SetColor(Quantity_NOC_ROYALBLUE1);
  return aLine;
}

//=================================================================================================

occ::handle<AIS_Point> AIS_PlaneTrihedron::Position() const
{
  gp_Pnt                  aPnt   = myPlane->Pln().Location();
  occ::handle<Geom_Point> aPoint = new Geom_CartesianPoint(aPnt);
  occ::handle<AIS_Point>  aPt    = new AIS_Point(aPoint);
  return aPt;
}

void AIS_PlaneTrihedron::SetLength(const double theLength)
{
  myDrawer->DatumAspect()->SetAxisLength(theLength, theLength, theLength);
  SetToUpdate();
}

double AIS_PlaneTrihedron::GetLength() const
{
  return myDrawer->DatumAspect()->AxisLength(Prs3d_DatumParts_XAxis);
}

//=================================================================================================

void AIS_PlaneTrihedron::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                                 const occ::handle<Prs3d_Presentation>& thePrs,
                                 const int)
{
  // drawing axis in X direction
  gp_Pnt first, last;
  double value = myDrawer->DatumAspect()->AxisLength(Prs3d_DatumParts_XAxis);
  gp_Dir xDir  = myPlane->Position().Ax2().XDirection();

  gp_Pnt orig = myPlane->Position().Ax2().Location();
  double xo, yo, zo, x, y, z;
  orig.Coord(xo, yo, zo);
  xDir.Coord(x, y, z);
  first.SetCoord(xo, yo, zo);
  last.SetCoord(xo + x * value, yo + y * value, zo + z * value);

  DsgPrs_XYZAxisPresentation::Add(thePrs,
                                  myDrawer->DatumAspect()->LineAspect(Prs3d_DatumParts_XAxis),
                                  myDrawer->ArrowAspect(),
                                  myDrawer->TextAspect(),
                                  xDir,
                                  value,
                                  myXLabel.ToCString(),
                                  first,
                                  last);

  // drawing axis in Y direction
  value       = myDrawer->DatumAspect()->AxisLength(Prs3d_DatumParts_YAxis);
  gp_Dir yDir = myPlane->Position().Ax2().YDirection();

  yDir.Coord(x, y, z);
  last.SetCoord(xo + x * value, yo + y * value, zo + z * value);
  DsgPrs_XYZAxisPresentation::Add(thePrs,
                                  myDrawer->DatumAspect()->LineAspect(Prs3d_DatumParts_XAxis),
                                  myDrawer->ArrowAspect(),
                                  myDrawer->TextAspect(),
                                  yDir,
                                  value,
                                  myYLabel.ToCString(),
                                  first,
                                  last);

  thePrs->SetInfiniteState(true);
}

//=================================================================================================

void AIS_PlaneTrihedron::ComputeSelection(const occ::handle<SelectMgr_Selection>& aSelection,
                                          const int                               aMode)
{
  int                                Prior;
  occ::handle<SelectMgr_EntityOwner> eown;
  NCollection_Array1<gp_Pnt>         PP(1, 4), PO(1, 4);
  //  ExtremityPoints(PP);
  ExtremityPoints(PP, myPlane, myDrawer);
  switch (aMode)
  {
    case 0: { // triedre complet
      Prior = 5;
      //      gp_Ax2 theax = gp_Ax2(myPlane->Position().Ax2());
      //      gp_Pnt p1 = theax.Location();

      eown = new SelectMgr_EntityOwner(this, Prior);
      for (int i = 1; i <= 2; i++)
        aSelection->Add(new Select3D_SensitiveSegment(eown, PP(1), PP(i + 1)));

      break;
    }
    case 1: { // origine
      Prior                                                = 8;
      const occ::handle<SelectMgr_SelectableObject>& anObj = myShapes[0]; // to avoid ambiguity
      eown = new SelectMgr_EntityOwner(anObj, Prior);
      aSelection->Add(new Select3D_SensitivePoint(eown, myPlane->Location()));

      break;
    }
    case 2: { // axes ... priorite 7
      Prior = 7;
      for (int i = 1; i <= 2; i++)
      {
        const occ::handle<SelectMgr_SelectableObject>& anObj = myShapes[i]; // to avoid ambiguity
        eown = new SelectMgr_EntityOwner(anObj, Prior);
        aSelection->Add(new Select3D_SensitiveSegment(eown, PP(1), PP(i + 1)));
      }
      break;
    }
    case -1: {
      Prior = 5;
      aSelection->Clear();
      break;
    }
  }
}

void AIS_PlaneTrihedron::SetColor(const Quantity_Color& aCol)
{
  hasOwnColor = true;
  myDrawer->SetColor(aCol);
  myDrawer->DatumAspect()->LineAspect(Prs3d_DatumParts_XAxis)->SetColor(aCol);
  myDrawer->DatumAspect()->LineAspect(Prs3d_DatumParts_YAxis)->SetColor(aCol);
  SynchronizeAspects();
}

//=================================================================================================

// void  AIS_Trihedron::ExtremityPoints(NCollection_Array1<gp_Pnt>& PP) const
void ExtremityPoints(NCollection_Array1<gp_Pnt>&      PP,
                     const occ::handle<Geom_Plane>&   myPlane,
                     const occ::handle<Prs3d_Drawer>& myDrawer)
{
  //  gp_Ax2 theax(myPlane->Ax2());
  gp_Ax2 theax(myPlane->Position().Ax2());
  PP(1) = theax.Location();

  double len = myDrawer->DatumAspect()->AxisLength(Prs3d_DatumParts_XAxis);
  gp_Vec vec = theax.XDirection();
  vec *= len;
  PP(2) = PP(1).Translated(vec);

  len = myDrawer->DatumAspect()->AxisLength(Prs3d_DatumParts_YAxis);
  vec = theax.YDirection();
  vec *= len;
  PP(3) = PP(1).Translated(vec);
}

//=================================================================================================

bool AIS_PlaneTrihedron::AcceptDisplayMode(const int aMode) const
{
  return aMode == 0;
}
