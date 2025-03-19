// Created on: 1995-08-09
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

#include <AIS_Axis.hxx>

#include <Aspect_TypeOfLine.hxx>
#include <DsgPrs_XYZAxisPresentation.hxx>
#include <Geom_Axis1Placement.hxx>
#include <Geom_Axis2Placement.hxx>
#include <Geom_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Structure.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Quantity_Color.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_NumericError.hxx>
#include <StdPrs_Curve.hxx>
#include <UnitsAPI.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Axis, AIS_InteractiveObject)

//=================================================================================================

AIS_Axis::AIS_Axis(const Handle(Geom_Line)& aComponent)
    : myComponent(aComponent),
      myTypeOfAxis(AIS_TOAX_Unknown),
      myIsXYZAxis(Standard_False)
{
  myDrawer->SetLineAspect(new Prs3d_LineAspect(Quantity_NOC_RED, Aspect_TOL_DOTDASH, 1.));
  SetInfiniteState();

  gp_Dir        thedir  = myComponent->Position().Direction();
  gp_Pnt        loc     = myComponent->Position().Location();
  Standard_Real aLength = UnitsAPI::AnyToLS(250000., "mm");
  myPfirst              = loc.XYZ() + aLength * thedir.XYZ();
  myPlast               = loc.XYZ() - aLength * thedir.XYZ();
}

//=======================================================================
// function : AIS_Axis
// purpose  :  Xaxis, YAxis, ZAxis
//=======================================================================
AIS_Axis::AIS_Axis(const Handle(Geom_Axis2Placement)& aComponent, const AIS_TypeOfAxis anAxisType)
    : myAx2(aComponent),
      myTypeOfAxis(anAxisType),
      myIsXYZAxis(Standard_True)
{
  Handle(Prs3d_DatumAspect) DA = new Prs3d_DatumAspect();
  Standard_Real             aLength;
  try
  {
    aLength = UnitsAPI::AnyToLS(100., "mm");
  }
  catch (Standard_Failure const&)
  {
    aLength = 0.1;
  }
  DA->SetAxisLength(aLength, aLength, aLength);
  Quantity_Color col(Quantity_NOC_TURQUOISE);
  DA->LineAspect(Prs3d_DatumParts_XAxis)->SetColor(col);
  DA->LineAspect(Prs3d_DatumParts_YAxis)->SetColor(col);
  DA->LineAspect(Prs3d_DatumParts_ZAxis)->SetColor(col);
  myDrawer->SetDatumAspect(DA);

  ComputeFields();
}

//=================================================================================================

AIS_Axis::AIS_Axis(const Handle(Geom_Axis1Placement)& anAxis)
    : myComponent(new Geom_Line(anAxis->Ax1())),
      myTypeOfAxis(AIS_TOAX_Unknown),
      myIsXYZAxis(Standard_False)
{
  myDrawer->SetLineAspect(new Prs3d_LineAspect(Quantity_NOC_RED, Aspect_TOL_DOTDASH, 1.));
  SetInfiniteState();

  gp_Dir        thedir  = myComponent->Position().Direction();
  gp_Pnt        loc     = myComponent->Position().Location();
  Standard_Real aLength = UnitsAPI::AnyToLS(250000., "mm");
  myPfirst              = loc.XYZ() + aLength * thedir.XYZ();
  myPlast               = loc.XYZ() - aLength * thedir.XYZ();
}

//=================================================================================================

AIS_Axis::AIS_Axis(const gp_Ax1& theAxis, const Standard_Real theLength)
    : myComponent(new Geom_Line(theAxis)),
      myTypeOfAxis(AIS_TOAX_ZAxis),
      myIsXYZAxis(Standard_True)
{
  myDir    = theAxis.Direction();
  myPfirst = theAxis.Location();
  if (theLength <= 0 && theLength != -1)
  {
    throw Standard_NumericError("AIS_Axis::AIS_Axis : invalid value for theLength parameter");
  }
  myVal   = (theLength == -1) ? UnitsAPI::AnyToLS(250000., "mm") : theLength;
  myPlast = myPfirst.XYZ() + myVal * myDir.XYZ();
  SetInfiniteState();
  Handle(Prs3d_DatumAspect) aDatumAspect = new Prs3d_DatumAspect();
  aDatumAspect->SetDrawLabels(Standard_False);
  myDrawer->SetDatumAspect(aDatumAspect);
  Handle(Prs3d_LineAspect) aDefaultLineAspect =
    new Prs3d_LineAspect(Quantity_NOC_RED, Aspect_TOL_SOLID, 1.0);
  myDrawer->SetLineAspect(aDefaultLineAspect);
  myLineAspect = myDrawer->LineAspect();
}

//=================================================================================================

void AIS_Axis::SetComponent(const Handle(Geom_Line)& aComponent)
{
  myComponent  = aComponent;
  myTypeOfAxis = AIS_TOAX_Unknown;
  myIsXYZAxis  = Standard_False;
  SetInfiniteState();

  gp_Dir        thedir  = myComponent->Position().Direction();
  gp_Pnt        loc     = myComponent->Position().Location();
  Standard_Real aLength = UnitsAPI::AnyToLS(250000., "mm");
  myPfirst              = loc.XYZ() + aLength * thedir.XYZ();
  myPlast               = loc.XYZ() - aLength * thedir.XYZ();
}

//=================================================================================================

void AIS_Axis::SetAxis2Placement(const Handle(Geom_Axis2Placement)& aComponent,
                                 const AIS_TypeOfAxis               anAxisType)
{
  myAx2        = aComponent;
  myTypeOfAxis = anAxisType;
  myIsXYZAxis  = Standard_True;
  ComputeFields();
}

//=================================================================================================

void AIS_Axis::SetAxis1Placement(const Handle(Geom_Axis1Placement)& anAxis)
{
  SetComponent(new Geom_Line(anAxis->Ax1()));
}

//=================================================================================================

void AIS_Axis::Compute(const Handle(PrsMgr_PresentationManager)&,
                       const Handle(Prs3d_Presentation)& thePrs,
                       const Standard_Integer)
{
  thePrs->SetInfiniteState(myInfiniteState);
  if (!myIsXYZAxis)
  {
    GeomAdaptor_Curve curv(myComponent);
    StdPrs_Curve::Add(thePrs, curv, myDrawer);
  }
  else
  {
    DsgPrs_XYZAxisPresentation::Add(thePrs,
                                    myLineAspect,
                                    myDir,
                                    myVal,
                                    myDrawer->DatumAspect()->ToDrawLabels() ? myText : "",
                                    myPfirst,
                                    myPlast);
  }
}

//=================================================================================================

void AIS_Axis::ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                const Standard_Integer)
{
  Handle(SelectMgr_EntityOwner)     eown = new SelectMgr_EntityOwner(this, 3);
  Handle(Select3D_SensitiveSegment) seg  = new Select3D_SensitiveSegment(eown, myPfirst, myPlast);
  aSelection->Add(seg);
}

//=================================================================================================

void AIS_Axis::SetColor(const Quantity_Color& aCol)
{
  hasOwnColor = Standard_True;
  myDrawer->SetColor(aCol);
  myDrawer->LineAspect()->SetColor(aCol);

  const Handle(Prs3d_DatumAspect)& DA = myDrawer->DatumAspect();
  DA->LineAspect(Prs3d_DatumParts_XAxis)->SetColor(aCol);
  DA->LineAspect(Prs3d_DatumParts_YAxis)->SetColor(aCol);
  DA->LineAspect(Prs3d_DatumParts_ZAxis)->SetColor(aCol);
  SynchronizeAspects();
}

//=================================================================================================

void AIS_Axis::SetWidth(const Standard_Real aValue)
{
  if (aValue < 0.0)
    return;
  if (aValue == 0)
    UnsetWidth();

  myDrawer->LineAspect()->SetWidth(aValue);

  const Handle(Prs3d_DatumAspect)& DA = myDrawer->DatumAspect();
  DA->LineAspect(Prs3d_DatumParts_XAxis)->SetWidth(aValue);
  DA->LineAspect(Prs3d_DatumParts_YAxis)->SetWidth(aValue);
  DA->LineAspect(Prs3d_DatumParts_ZAxis)->SetWidth(aValue);
  SynchronizeAspects();
}

//=================================================================================================

void AIS_Axis::SetDisplayAspect(const Handle(Prs3d_LineAspect)& theNewLineAspect)
{
  myDrawer->SetLineAspect(theNewLineAspect);
  myLineAspect = myDrawer->LineAspect();
  SetColor(theNewLineAspect->Aspect()->Color());
}

//=================================================================================================

void AIS_Axis::ComputeFields()
{
  if (myIsXYZAxis)
  {
    // calcul de myPFirst,myPlast
    Handle(Prs3d_DatumAspect) DA     = myDrawer->DatumAspect();
    gp_Ax2                    anAxis = myAx2->Ax2();
    const gp_Pnt&             Orig   = anAxis.Location();
    const gp_Dir&             oX     = anAxis.XDirection();
    const gp_Dir&             oY     = anAxis.YDirection();
    const gp_Dir&             oZ     = anAxis.Direction();
    Standard_Real             xo, yo, zo, x = 0., y = 0., z = 0.;
    Orig.Coord(xo, yo, zo);
    myPfirst.SetCoord(xo, yo, zo);

    switch (myTypeOfAxis)
    {
      case AIS_TOAX_XAxis: {
        oX.Coord(x, y, z);
        myVal        = DA->AxisLength(Prs3d_DatumParts_XAxis);
        myDir        = oX;
        myLineAspect = DA->LineAspect(Prs3d_DatumParts_XAxis);
        myText       = Standard_CString("X");
        break;
      }
      case AIS_TOAX_YAxis: {
        oY.Coord(x, y, z);
        myVal        = DA->AxisLength(Prs3d_DatumParts_YAxis);
        myDir        = oY;
        myLineAspect = DA->LineAspect(Prs3d_DatumParts_YAxis);
        myText       = Standard_CString("Y");
        break;
      }
      case AIS_TOAX_ZAxis: {
        oZ.Coord(x, y, z);
        myVal        = DA->AxisLength(Prs3d_DatumParts_ZAxis);
        myDir        = oZ;
        myLineAspect = DA->LineAspect(Prs3d_DatumParts_ZAxis);
        myText       = Standard_CString("Z");
        break;
      }
      default:
        break;
    }

    myComponent = new Geom_Line(Orig, myDir);
    x           = xo + x * myVal;
    y           = yo + y * myVal;
    z           = zo + z * myVal;
    myPlast.SetCoord(x, y, z);
    SetInfiniteState();
  }
}

//=================================================================================================

Standard_Boolean AIS_Axis::AcceptDisplayMode(const Standard_Integer aMode) const
{
  return aMode == 0;
}

//=================================================================================================

void AIS_Axis::UnsetColor()
{
  myDrawer->LineAspect()->SetColor(Quantity_NOC_RED);
  hasOwnColor = Standard_False;

  myDrawer->DatumAspect()->LineAspect(Prs3d_DatumParts_XAxis)->SetColor(Quantity_NOC_TURQUOISE);
  myDrawer->DatumAspect()->LineAspect(Prs3d_DatumParts_YAxis)->SetColor(Quantity_NOC_TURQUOISE);
  myDrawer->DatumAspect()->LineAspect(Prs3d_DatumParts_ZAxis)->SetColor(Quantity_NOC_TURQUOISE);
  SynchronizeAspects();
}

//=================================================================================================

void AIS_Axis::UnsetWidth()
{
  myOwnWidth = 0.0f;
  myDrawer->LineAspect()->SetWidth(1.);
  myDrawer->DatumAspect()->LineAspect(Prs3d_DatumParts_XAxis)->SetWidth(1.);
  myDrawer->DatumAspect()->LineAspect(Prs3d_DatumParts_YAxis)->SetWidth(1.);
  myDrawer->DatumAspect()->LineAspect(Prs3d_DatumParts_ZAxis)->SetWidth(1.);
  SynchronizeAspects();
}
