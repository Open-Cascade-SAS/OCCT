// Created on: 1995-08-02
// Created by: Arnaud BOUZY/Odile Olivier
// Copyright (c) 1995-1999 Matra Datavision
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

#include <AIS_Plane.hxx>

#include <AIS_InteractiveContext.hxx>
#include <DsgPrs_ShadedPlanePresentation.hxx>
#include <DsgPrs_XYZPlanePresentation.hxx>
#include <ElSLib.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Plane.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfQuadrangles.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_TransformPersScaledAbove.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Quantity_Color.hxx>
#include <Select3D_SensitiveTriangulation.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Type.hxx>
#include <StdPrs_Plane.hxx>
#include <NCollection_Array1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Plane, AIS_InteractiveObject)

//=================================================================================================

AIS_Plane::AIS_Plane(const occ::handle<Geom_Plane>& aComponent, const bool aCurrentMode)
    : myComponent(aComponent),
      myCenter(gp_Pnt(0., 0., 0.)),
      myCurrentMode(aCurrentMode),
      myAutomaticPosition(true),
      myTypeOfPlane(AIS_TOPL_Unknown),
      myIsXYZPlane(false),
      myTypeOfSensitivity(Select3D_TOS_BOUNDARY)
{
  InitDrawerAttributes();
}

//=================================================================================================

AIS_Plane::AIS_Plane(const occ::handle<Geom_Plane>& aComponent,
                     const gp_Pnt&                  aCenter,
                     const bool                     aCurrentMode)
    : myComponent(aComponent),
      myCenter(aCenter),
      myCurrentMode(aCurrentMode),
      myAutomaticPosition(true),
      myTypeOfPlane(AIS_TOPL_Unknown),
      myIsXYZPlane(false),
      myTypeOfSensitivity(Select3D_TOS_BOUNDARY)
{
  InitDrawerAttributes();
}

//=================================================================================================

AIS_Plane::AIS_Plane(const occ::handle<Geom_Plane>& aComponent,
                     const gp_Pnt&                  aCenter,
                     const gp_Pnt&                  aPmin,
                     const gp_Pnt&                  aPmax,
                     const bool                     aCurrentMode)
    : myComponent(aComponent),
      myCenter(aCenter),
      myPmin(aPmin),
      myPmax(aPmax),
      myCurrentMode(aCurrentMode),
      myAutomaticPosition(false),
      myTypeOfPlane(AIS_TOPL_Unknown),
      myIsXYZPlane(false),
      myTypeOfSensitivity(Select3D_TOS_BOUNDARY)
{
  InitDrawerAttributes();
}

//=======================================================================
// function : AIS_Plane
// purpose  : XYPlane, XZPlane, YZPlane
//=======================================================================
AIS_Plane::AIS_Plane(const occ::handle<Geom_Axis2Placement>& aComponent,
                     const AIS_TypeOfPlane                   aPlaneType,
                     const bool                              aCurrentMode)
    : myAx2(aComponent),
      myCurrentMode(aCurrentMode),
      myAutomaticPosition(true),
      myTypeOfPlane(aPlaneType),
      myIsXYZPlane(true),
      myTypeOfSensitivity(Select3D_TOS_BOUNDARY)
{
  InitDrawerAttributes();
  ComputeFields();
}

//=================================================================================================

void AIS_Plane::SetComponent(const occ::handle<Geom_Plane>& aComponent)
{
  myComponent   = aComponent;
  myTypeOfPlane = AIS_TOPL_Unknown;
  myIsXYZPlane  = false;
  // myCenter = gp_Pnt(0.,0.,0.);
  myAutomaticPosition = true;
}

//=================================================================================================

occ::handle<Geom_Axis2Placement> AIS_Plane::Axis2Placement()
{
  occ::handle<Geom_Axis2Placement> Bid;
  return IsXYZPlane() ? myAx2 : Bid;
}

//=================================================================================================

void AIS_Plane::SetAxis2Placement(const occ::handle<Geom_Axis2Placement>& aComponent,
                                  const AIS_TypeOfPlane                   aPlaneType)
{
  myTypeOfPlane       = aPlaneType;
  myIsXYZPlane        = true;
  myAx2               = aComponent;
  myAutomaticPosition = true;
  ComputeFields();
}

//=================================================================================================

bool AIS_Plane::PlaneAttributes(occ::handle<Geom_Plane>& aComponent,
                                gp_Pnt&                  aCenter,
                                gp_Pnt&                  aPmin,
                                gp_Pnt&                  aPmax)
{
  bool aStatus(false);
  if (!myAutomaticPosition)
  {
    aComponent = myComponent;
    aCenter    = myCenter;
    aPmin      = myPmin;
    aPmax      = myPmax;
    aStatus    = true;
  }
  return aStatus;
}

//=================================================================================================

void AIS_Plane::SetPlaneAttributes(const occ::handle<Geom_Plane>& aComponent,
                                   const gp_Pnt&                  aCenter,
                                   const gp_Pnt&                  aPmin,
                                   const gp_Pnt&                  aPmax)
{
  myAutomaticPosition = false;
  myComponent         = aComponent;
  myCenter            = aCenter;
  myPmin              = aPmin;
  myPmax              = aPmax;
  myTypeOfPlane       = AIS_TOPL_Unknown;
  myIsXYZPlane        = false;
}

//=================================================================================================

void AIS_Plane::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                        const occ::handle<Prs3d_Presentation>& thePrs,
                        const int                              theMode)
{
  ComputeFields();
  thePrs->SetInfiniteState(myInfiniteState);
  myDrawer->PlaneAspect()->EdgesAspect()->SetWidth(myCurrentMode == 0 ? 1 : 3);

  switch (theMode)
  {
    case 0: {
      if (!myIsXYZPlane)
      {
        ComputeFrame();
        const occ::handle<Geom_Plane>& pl = myComponent;
        occ::handle<Geom_Plane>        thegoodpl(
          occ::down_cast<Geom_Plane>(pl->Translated(pl->Location(), myCenter)));
        GeomAdaptor_Surface surf(thegoodpl);
        StdPrs_Plane::Add(thePrs, surf, myDrawer);
      }
      else
      {
        DsgPrs_XYZPlanePresentation::Add(thePrs, myDrawer, myCenter, myPmin, myPmax);
      }
      break;
    }
    case 1: {
      if (!myIsXYZPlane)
      {
        ComputeFrame();
        occ::handle<Prs3d_PlaneAspect> anAspect = myDrawer->PlaneAspect();
        occ::handle<Graphic3d_Group>   aGroup   = thePrs->CurrentGroup();
        aGroup->SetPrimitivesAspect(myDrawer->ShadingAspect()->Aspect());
        gp_Pnt       p1;
        const double Xmax = 0.5 * double(anAspect->PlaneXLength());
        const double Ymax = 0.5 * double(anAspect->PlaneYLength());

        occ::handle<Graphic3d_ArrayOfQuadrangles> aQuads = new Graphic3d_ArrayOfQuadrangles(4);

        myComponent->D0(-Xmax, Ymax, p1);
        aQuads->AddVertex(p1);
        myComponent->D0(Xmax, Ymax, p1);
        aQuads->AddVertex(p1);
        myComponent->D0(Xmax, -Ymax, p1);
        aQuads->AddVertex(p1);
        myComponent->D0(-Xmax, -Ymax, p1);
        aQuads->AddVertex(p1);

        aGroup->AddPrimitiveArray(aQuads);
      }
      else
      {
        DsgPrs_ShadedPlanePresentation::Add(thePrs, myDrawer, myCenter, myPmin, myPmax);
      }
      break;
    }
  }
}

//=================================================================================================

void AIS_Plane::ComputeSelection(const occ::handle<SelectMgr_Selection>& theSelection,
                                 const int /*theMode*/)
{
  theSelection->Clear();
  occ::handle<SelectMgr_EntityOwner> aSensitiveOwner = new SelectMgr_EntityOwner(this, 10);
  occ::handle<Poly_Triangulation>    aSensitivePoly;

  if (!myIsXYZPlane)
  {
    // plane representing rectangle
    double                  aLengthX = myDrawer->PlaneAspect()->PlaneXLength() / 2.0;
    double                  aLengthY = myDrawer->PlaneAspect()->PlaneYLength() / 2.0;
    occ::handle<Geom_Plane> aPlane =
      occ::down_cast<Geom_Plane>(myComponent->Translated(myComponent->Location(), myCenter));

    NCollection_Array1<gp_Pnt> aRectanglePoints(1, 4);
    aPlane->D0(aLengthX, aLengthY, aRectanglePoints.ChangeValue(1));
    aPlane->D0(aLengthX, -aLengthY, aRectanglePoints.ChangeValue(2));
    aPlane->D0(-aLengthX, -aLengthY, aRectanglePoints.ChangeValue(3));
    aPlane->D0(-aLengthX, aLengthY, aRectanglePoints.ChangeValue(4));

    NCollection_Array1<Poly_Triangle> aTriangles(1, 2);
    aTriangles.ChangeValue(1) = Poly_Triangle(1, 2, 3);
    aTriangles.ChangeValue(2) = Poly_Triangle(1, 3, 4);

    aSensitivePoly = new Poly_Triangulation(aRectanglePoints, aTriangles);
  }
  else
  {
    // plane representing triangle
    NCollection_Array1<gp_Pnt> aTrianglePoints(1, 3);
    aTrianglePoints.ChangeValue(1) = myCenter;
    aTrianglePoints.ChangeValue(2) = myPmin;
    aTrianglePoints.ChangeValue(3) = myPmax;

    NCollection_Array1<Poly_Triangle> aTriangles(1, 1);
    aTriangles.ChangeValue(1) = Poly_Triangle(1, 2, 3);

    aSensitivePoly = new Poly_Triangulation(aTrianglePoints, aTriangles);
  }

  bool isSensitiveInterior = myTypeOfSensitivity == Select3D_TOS_INTERIOR;

  occ::handle<Select3D_SensitiveTriangulation> aSensitive =
    new Select3D_SensitiveTriangulation(aSensitiveOwner,
                                        aSensitivePoly,
                                        TopLoc_Location(),
                                        isSensitiveInterior);

  theSelection->Add(aSensitive);
}

//=================================================================================================

void AIS_Plane::SetSize(const double aLength)
{
  SetSize(aLength, aLength);
}

void AIS_Plane::SetSize(const double aXLength, const double aYLength)
{
  // if the plane already has a proper color or size,
  // there is already a specific PlaneAspect and DatumAspect

  occ::handle<Prs3d_PlaneAspect> PA;
  occ::handle<Prs3d_DatumAspect> DA;

  PA = myDrawer->PlaneAspect();
  DA = myDrawer->DatumAspect();

  bool yenavaitPA(true), yenavaitDA(true);
  if (myDrawer->HasLink() && myDrawer->Link()->PlaneAspect() == PA)
  {
    yenavaitPA = false;
    PA         = new Prs3d_PlaneAspect();
  }
  if (myDrawer->HasLink() && myDrawer->Link()->DatumAspect() == DA)
  {
    yenavaitDA = false;
    DA         = new Prs3d_DatumAspect();
  }

  PA->SetPlaneLength(aXLength, aYLength);
  DA->SetAxisLength(aXLength, aYLength, aXLength);

  if (!yenavaitPA)
    myDrawer->SetPlaneAspect(PA);
  if (!yenavaitDA)
    myDrawer->SetDatumAspect(DA);

  myHasOwnSize = true;
  SetToUpdate();
  UpdatePresentations();
  UpdateSelection();
}

//=======================================================================
// function : UnsetSize
// purpose  : If there is a color, the size is restaured from the drawer of the context...
//=======================================================================
void AIS_Plane::UnsetSize()
{

  if (!myHasOwnSize)
    return;
  if (!hasOwnColor)
  {
    myDrawer->SetPlaneAspect(occ::handle<Prs3d_PlaneAspect>());
    myDrawer->SetDatumAspect(occ::handle<Prs3d_DatumAspect>());
  }
  else
  {
    const occ::handle<Prs3d_PlaneAspect> PA =
      myDrawer->HasLink() ? myDrawer->Link()->PlaneAspect() : new Prs3d_PlaneAspect();
    const occ::handle<Prs3d_DatumAspect> DA =
      myDrawer->HasLink() ? myDrawer->Link()->DatumAspect() : new Prs3d_DatumAspect();

    myDrawer->PlaneAspect()->SetPlaneLength(PA->PlaneXLength(), PA->PlaneYLength());
    myDrawer->DatumAspect()->SetAxisLength(DA->AxisLength(Prs3d_DatumParts_XAxis),
                                           DA->AxisLength(Prs3d_DatumParts_YAxis),
                                           DA->AxisLength(Prs3d_DatumParts_ZAxis));
  }

  myHasOwnSize = false;
  SetToUpdate();
  UpdatePresentations();
  UpdateSelection();
}

//=================================================================================================

bool AIS_Plane::Size(double& X, double& Y) const
{
  X = myDrawer->PlaneAspect()->PlaneXLength();
  Y = myDrawer->PlaneAspect()->PlaneYLength();
  return std::abs(X - Y) <= Precision::Confusion();
}

//=================================================================================================

void AIS_Plane::SetMinimumSize(const double theValue)
{
  if (theValue <= 0)
  {
    UnsetMinimumSize();
    return;
  }
  double aX, anY;
  Size(aX, anY);
  SetTransformPersistence(
    new Graphic3d_TransformPersScaledAbove(std::min(aX, anY) / theValue, myCenter));
}

//=================================================================================================

void AIS_Plane::UnsetMinimumSize()
{
  SetTransformPersistence(nullptr);
}

//=================================================================================================

bool AIS_Plane::HasMinimumSize() const
{
  return !occ::down_cast<Graphic3d_TransformPersScaledAbove>(TransformPersistence()).IsNull();
}

//=================================================================================================

void AIS_Plane::SetColor(const Quantity_Color& aCol)
{
  // if the plane already has its proper size, there is an already created planeaspect
  //  bool yenadeja = hasOwnColor || myHasOwnSize;
  occ::handle<Prs3d_PlaneAspect> PA;
  occ::handle<Prs3d_DatumAspect> DA;

  PA = myDrawer->PlaneAspect();
  DA = myDrawer->DatumAspect();

  bool yenavaitPA(true), yenavaitDA(true);
  if (myDrawer->HasLink() && myDrawer->Link()->PlaneAspect() == PA)
  {
    yenavaitPA = false;
    PA         = new Prs3d_PlaneAspect();
  }
  if (myDrawer->HasLink() && myDrawer->Link()->DatumAspect() == DA)
  {
    yenavaitDA = false;
    DA         = new Prs3d_DatumAspect();
  }

  PA->EdgesAspect()->SetColor(aCol);
  DA->LineAspect(Prs3d_DatumParts_XAxis)->SetColor(aCol);
  DA->LineAspect(Prs3d_DatumParts_YAxis)->SetColor(aCol);
  DA->LineAspect(Prs3d_DatumParts_ZAxis)->SetColor(aCol);

  if (!yenavaitPA)
    myDrawer->SetPlaneAspect(PA);
  if (!yenavaitDA)
    myDrawer->SetDatumAspect(DA);

  myDrawer->ShadingAspect()->SetColor(aCol);

  hasOwnColor = true;
  myDrawer->SetColor(aCol);
}

//=================================================================================================

void AIS_Plane::UnsetColor()
{
  if (!hasOwnColor)
    return;
  if (!myHasOwnSize)
  {
    myDrawer->SetPlaneAspect(occ::handle<Prs3d_PlaneAspect>());
    myDrawer->SetDatumAspect(occ::handle<Prs3d_DatumAspect>());
  }
  else
  {
    const occ::handle<Prs3d_PlaneAspect> PA =
      myDrawer->HasLink() ? myDrawer->Link()->PlaneAspect() : new Prs3d_PlaneAspect();
    Quantity_Color Col = PA->EdgesAspect()->Aspect()->Color();
    myDrawer->PlaneAspect()->EdgesAspect()->SetColor(Col);

    myDrawer->DatumAspect()->LineAspect(Prs3d_DatumParts_XAxis)->SetColor(Col);
    myDrawer->DatumAspect()->LineAspect(Prs3d_DatumParts_YAxis)->SetColor(Col);
    myDrawer->DatumAspect()->LineAspect(Prs3d_DatumParts_ZAxis)->SetColor(Col);
  }

  hasOwnColor = false;
}

//=================================================================================================

void AIS_Plane::ComputeFrame()
{

  const occ::handle<Geom_Plane>& pl = myComponent;
  double                         U, V;

  if (myAutomaticPosition)
  {
    ElSLib::Parameters(pl->Pln(), myCenter, U, V);
    pl->D0(U, V, myCenter);
  }
  else
  {
    occ::handle<Geom_Plane> thegoodpl(
      occ::down_cast<Geom_Plane>(pl->Translated(pl->Location(), myCenter)));
    ElSLib::Parameters(thegoodpl->Pln(), myPmin, U, V);

    U = 2.4 * std::abs(U);
    V = 2.4 * std::abs(V);
    if (U < 10 * Precision::Confusion())
      U = 0.1;
    if (V < 10 * Precision::Confusion())
      V = 0.1;
    SetSize(U, V);
    myDrawer->PlaneAspect()->SetPlaneLength(U, V);
  }
}

//=================================================================================================

void AIS_Plane::ComputeFields()
{
  if (myIsXYZPlane)
  {
    occ::handle<Prs3d_DatumAspect> DA = myDrawer->DatumAspect();

    gp_Pnt Orig = myAx2->Ax2().Location();
    gp_Dir oX   = myAx2->Ax2().XDirection();
    gp_Dir oY   = myAx2->Ax2().YDirection();
    gp_Dir oZ   = myAx2->Ax2().Direction();
    myCenter    = Orig;
    double xo, yo, zo, x1, y1, z1, x2, y2, z2, x3, y3, z3, x4 = 0, y4 = 0, z4 = 0;
    double x5 = 0, y5 = 0, z5 = 0;
    Orig.Coord(xo, yo, zo);
    oX.Coord(x1, y1, z1);
    oY.Coord(x2, y2, z2);
    oZ.Coord(x3, y3, z3);
    double DS1 = DA->AxisLength(Prs3d_DatumParts_XAxis);
    double DS2 = DA->AxisLength(Prs3d_DatumParts_YAxis);
    double DS3 = DA->AxisLength(Prs3d_DatumParts_ZAxis);
    //    gp_Pnt aPt2,aPt3;

    switch (myTypeOfPlane)
    {
      case AIS_TOPL_XYPlane: {
        gp_Pln XYP(0, 0, 1, 0);
        myComponent = new Geom_Plane(XYP);
        x4          = xo + x1 * DS1;
        y4          = yo + y1 * DS1;
        z4          = zo + z1 * DS1;
        x5          = xo + x2 * DS2;
        y5          = yo + y2 * DS2;
        z5          = zo + z2 * DS2;
        break;
      }
      case AIS_TOPL_XZPlane: {
        gp_Pln XZP(0, 1, 0, 0);
        myComponent = new Geom_Plane(XZP);
        x4          = xo + x1 * DS1;
        y4          = yo + y1 * DS1;
        z4          = zo + z1 * DS1;
        x5          = xo + x3 * DS3;
        y5          = yo + y3 * DS3;
        z5          = zo + z3 * DS3;
        break;
      }
      case AIS_TOPL_YZPlane: {
        gp_Pln XZP(1, 0, 0, 0);
        myComponent = new Geom_Plane(XZP);
        x4          = xo + x2 * DS2;
        y4          = yo + y2 * DS2;
        z4          = zo + z2 * DS2;
        x5          = xo + x3 * DS3;
        y5          = yo + y3 * DS3;
        z5          = zo + z3 * DS3;
        break;
      }
      default:
        break;
    }
    myPmin.SetCoord(x4, y4, z4);
    myPmax.SetCoord(x5, y5, z5);
  }
}

//=================================================================================================

void AIS_Plane::InitDrawerAttributes()
{
  occ::handle<Prs3d_ShadingAspect> shasp = new Prs3d_ShadingAspect();
  shasp->SetMaterial(Graphic3d_NameOfMaterial_Plastified);
  shasp->SetColor(Quantity_NOC_GRAY40);
  myDrawer->SetShadingAspect(shasp);
  occ::handle<Graphic3d_AspectFillArea3d> asf = shasp->Aspect();
  Graphic3d_MaterialAspect                asp = asf->FrontMaterial();
  asp.SetTransparency(0.8f);
  asf->SetFrontMaterial(asp);
  asf->SetBackMaterial(asp);
}

//=================================================================================================

bool AIS_Plane::AcceptDisplayMode(const int aMode) const
{
  return aMode == 0;
}

//=================================================================================================

void AIS_Plane::SetContext(const occ::handle<AIS_InteractiveContext>& Ctx)
{
  AIS_InteractiveObject::SetContext(Ctx);
  ComputeFields();
}
