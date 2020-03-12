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

#include <Prs3d_Drawer.hxx>

#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DatumAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_Drawer, Graphic3d_PresentationAttributes)

namespace
{
  static const Quantity_NameOfColor THE_DEF_COLOR_FreeBoundary   = Quantity_NOC_GREEN;
  static const Quantity_NameOfColor THE_DEF_COLOR_UnFreeBoundary = Quantity_NOC_YELLOW;
  static const Quantity_NameOfColor THE_DEF_COLOR_FaceBoundary   = Quantity_NOC_BLACK;
  static const Quantity_NameOfColor THE_DEF_COLOR_Wire           = Quantity_NOC_RED;
  static const Quantity_NameOfColor THE_DEF_COLOR_Line           = Quantity_NOC_YELLOW;
  static const Quantity_NameOfColor THE_DEF_COLOR_SeenLine       = Quantity_NOC_YELLOW;
  static const Quantity_NameOfColor THE_DEF_COLOR_HiddenLine     = Quantity_NOC_YELLOW;
  static const Quantity_NameOfColor THE_DEF_COLOR_Vector         = Quantity_NOC_SKYBLUE;
  static const Quantity_NameOfColor THE_DEF_COLOR_Section        = Quantity_NOC_ORANGE;
}

// =======================================================================
// function : Prs3d_Drawer
// purpose  :
// =======================================================================
Prs3d_Drawer::Prs3d_Drawer()
: myNbPoints                      (30),
  myHasOwnNbPoints                (Standard_False),
  myMaximalParameterValue         (500000.),
  myHasOwnMaximalParameterValue   (Standard_False),
  myChordialDeviation             (0.0001),
  myHasOwnChordialDeviation       (Standard_False),
  myTypeOfDeflection              (Aspect_TOD_RELATIVE),
  myHasOwnTypeOfDeflection        (Standard_False),
  myTypeOfHLR                     (Prs3d_TOH_NotSet),
  myDeviationCoefficient          (0.001),
  myHasOwnDeviationCoefficient    (Standard_False),
  myDeviationAngle                (20.0 * M_PI / 180.0),
  myHasOwnDeviationAngle          (Standard_False),
  myIsoOnPlane                    (Standard_False),
  myHasOwnIsoOnPlane              (Standard_False),
  myIsoOnTriangulation            (Standard_False),
  myHasOwnIsoOnTriangulation      (Standard_False),
  myIsAutoTriangulated            (Standard_True),
  myHasOwnIsAutoTriangulated      (Standard_False),

  myHasOwnUIsoAspect          (Standard_False),
  myHasOwnVIsoAspect          (Standard_False),
  myHasOwnWireAspect          (Standard_False),
  myWireDraw                  (Standard_True),
  myHasOwnWireDraw            (Standard_False),
  myHasOwnPointAspect         (Standard_False),
  myHasOwnLineAspect          (Standard_False),
  myHasOwnTextAspect          (Standard_False),
  myHasOwnShadingAspect       (Standard_False),
  myHasOwnPlaneAspect         (Standard_False),
  myHasOwnSeenLineAspect      (Standard_False),
  myHasOwnArrowAspect         (Standard_False),
  myLineArrowDraw             (Standard_False),
  myHasOwnLineArrowDraw       (Standard_False),
  myHasOwnHiddenLineAspect    (Standard_False),
  myDrawHiddenLine            (Standard_False),
  myHasOwnDrawHiddenLine      (Standard_False),
  myHasOwnVectorAspect        (Standard_False),
  myVertexDrawMode            (Prs3d_VDM_Inherited),
  myHasOwnDatumAspect         (Standard_False),
  myHasOwnSectionAspect       (Standard_False),

  myHasOwnFreeBoundaryAspect   (Standard_False),
  myFreeBoundaryDraw           (Standard_True),
  myHasOwnFreeBoundaryDraw     (Standard_False),
  myHasOwnUnFreeBoundaryAspect (Standard_False),
  myUnFreeBoundaryDraw         (Standard_True),
  myHasOwnUnFreeBoundaryDraw   (Standard_False),
  myFaceBoundaryUpperContinuity(-1),
  myHasOwnFaceBoundaryAspect   (Standard_False),
  myFaceBoundaryDraw           (Standard_False),
  myHasOwnFaceBoundaryDraw     (Standard_False),

  myHasOwnDimensionAspect       (Standard_False),
  myHasOwnDimLengthModelUnits   (Standard_False),
  myHasOwnDimAngleModelUnits    (Standard_False),
  myHasOwnDimLengthDisplayUnits (Standard_False),
  myHasOwnDimAngleDisplayUnits  (Standard_False)
{
  myDimensionModelUnits.SetLengthUnits ("m");
  myDimensionModelUnits.SetAngleUnits ("rad");
  myDimensionDisplayUnits.SetLengthUnits ("m");
  myDimensionDisplayUnits.SetAngleUnits ("deg");
}

// =======================================================================
// function : SetTypeOfDeflection
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetTypeOfDeflection (const Aspect_TypeOfDeflection theTypeOfDeflection)
{
  myHasOwnTypeOfDeflection = Standard_True;
  myTypeOfDeflection       = theTypeOfDeflection;
}

// =======================================================================
// function : SetMaximalChordialDeviation
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetMaximalChordialDeviation (const Standard_Real theChordialDeviation)
{
  myHasOwnChordialDeviation = Standard_True;
  myChordialDeviation       = theChordialDeviation;
}

// =======================================================================
// function : SetTypeOfHLR
// purpose  : set type of HLR algorithm
// =======================================================================

void Prs3d_Drawer::SetTypeOfHLR (const Prs3d_TypeOfHLR theTypeOfHLR)
{
  myTypeOfHLR = theTypeOfHLR;
}

// =======================================================================
// function : TypeOfHLR
// purpose  : gets type of HLR algorithm
// =======================================================================

Prs3d_TypeOfHLR Prs3d_Drawer::TypeOfHLR()
{
  if (!HasOwnTypeOfHLR())
  {
    if (!myLink.IsNull())
    {
      return myLink->TypeOfHLR();
    }
    // Prs3d_TOH_PolyAlgo is default value for this setting.
    myTypeOfHLR = Prs3d_TOH_PolyAlgo;
  }
  return myTypeOfHLR;
}

// =======================================================================
// function : SetIsoOnTriangulation
// purpose  :
// =======================================================================
void Prs3d_Drawer::SetIsoOnTriangulation (const Standard_Boolean theToEnable)
{
  myHasOwnIsoOnTriangulation = Standard_True;
  myIsoOnTriangulation = theToEnable;
}

// =======================================================================
// function : SetMaximalParameterValue
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetMaximalParameterValue (const Standard_Real theValue)
{
  myHasOwnMaximalParameterValue = Standard_True;
  myMaximalParameterValue       = theValue;
}

// =======================================================================
// function : SetIsoOnPlane
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetIsoOnPlane (const Standard_Boolean theIsEnabled)
{
  myHasOwnIsoOnPlane = Standard_True;
  myIsoOnPlane       = theIsEnabled;
}

// =======================================================================
// function : SetDiscretisation
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetDiscretisation (const Standard_Integer theValue) 
{
  myHasOwnNbPoints = Standard_True;
  myNbPoints       = theValue;
}

//=======================================================================
//function : SetDeviationCoefficient
//purpose  : 
//=======================================================================

void Prs3d_Drawer::SetDeviationCoefficient (const Standard_Real theCoefficient)
{
  myPreviousDeviationCoefficient = DeviationCoefficient();
  myDeviationCoefficient         = theCoefficient;
  myHasOwnDeviationCoefficient   = Standard_True;
}

//=======================================================================
//function : SetDeviationAngle
//purpose  : 
//=======================================================================

void Prs3d_Drawer::SetDeviationAngle (const Standard_Real theAngle)
{
  myPreviousDeviationAngle = DeviationAngle();
  myDeviationAngle         = theAngle;
  myHasOwnDeviationAngle   = Standard_True;
}

// =======================================================================
// function : SetAutoTriangulation
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetAutoTriangulation (const Standard_Boolean theIsEnabled)
{
  myHasOwnIsAutoTriangulated = Standard_True;
  myIsAutoTriangulated       = theIsEnabled;
}

// =======================================================================
// function : FreeBoundaryAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::FreeBoundaryAspect()
{
  if (!HasOwnFreeBoundaryAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->FreeBoundaryAspect();
    }
    if (myFreeBoundaryAspect.IsNull())
    {
      myFreeBoundaryAspect = new Prs3d_LineAspect (THE_DEF_COLOR_FreeBoundary, Aspect_TOL_SOLID, 1.0);
    }
  }
  return myFreeBoundaryAspect;
}

// =======================================================================
// function : FreeBoundaryAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetFreeBoundaryAspect (const Handle(Prs3d_LineAspect)& theAspect)
{
  myFreeBoundaryAspect = theAspect;
  myHasOwnFreeBoundaryAspect = !myFreeBoundaryAspect.IsNull();
}

// =======================================================================
// function : SetFreeBoundaryDraw
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetFreeBoundaryDraw (const Standard_Boolean theIsEnabled)
{
  myHasOwnFreeBoundaryDraw = Standard_True;
  myFreeBoundaryDraw       = theIsEnabled;
}

// =======================================================================
// function : UnFreeBoundaryAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::UnFreeBoundaryAspect()
{
  if (!HasOwnUnFreeBoundaryAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->UnFreeBoundaryAspect();
    }
    if (myUnFreeBoundaryAspect.IsNull())
    {
      myUnFreeBoundaryAspect = new Prs3d_LineAspect (THE_DEF_COLOR_UnFreeBoundary, Aspect_TOL_SOLID, 1.0);
    }
  }
  return myUnFreeBoundaryAspect;
}

// =======================================================================
// function : SetUnFreeBoundaryAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetUnFreeBoundaryAspect (const Handle(Prs3d_LineAspect)& theAspect)
{
  myUnFreeBoundaryAspect = theAspect;
  myHasOwnUnFreeBoundaryAspect = !myUnFreeBoundaryAspect.IsNull();
}

// =======================================================================
// function : SetUnFreeBoundaryDraw
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetUnFreeBoundaryDraw (const Standard_Boolean theIsEnabled)
{
  myHasOwnUnFreeBoundaryDraw = Standard_True;
  myUnFreeBoundaryDraw       = theIsEnabled;
}

// =======================================================================
// function : FaceBoundaryAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::FaceBoundaryAspect()
{
  if (!HasOwnFaceBoundaryAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->FaceBoundaryAspect();
    }
    if (myFaceBoundaryAspect.IsNull())
    {
      myFaceBoundaryAspect = new Prs3d_LineAspect (THE_DEF_COLOR_FaceBoundary, Aspect_TOL_SOLID, 1.0);
    }
  }
  return myFaceBoundaryAspect;
}

// =======================================================================
// function : SetFaceBoundaryAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetFaceBoundaryAspect (const Handle(Prs3d_LineAspect)& theAspect)
{
  myFaceBoundaryAspect = theAspect;
  myHasOwnFaceBoundaryAspect = !myFaceBoundaryAspect.IsNull();
}

// =======================================================================
// function : SetFaceBoundaryDraw
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetFaceBoundaryDraw (const Standard_Boolean theIsEnabled)
{
  myHasOwnFaceBoundaryDraw = Standard_True;
  myFaceBoundaryDraw       = theIsEnabled;
}

// =======================================================================
// function : DimensionAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_DimensionAspect)& Prs3d_Drawer::DimensionAspect()
{
  if (!HasOwnDimensionAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->DimensionAspect();
    }
    if (myDimensionAspect.IsNull())
    {
      myDimensionAspect = new Prs3d_DimensionAspect;
    }
  }
  return myDimensionAspect;
}

// =======================================================================
// function : SetDimensionAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetDimensionAspect (const Handle(Prs3d_DimensionAspect)& theAspect)
{
  myDimensionAspect = theAspect;
  myHasOwnDimensionAspect = !myDimensionAspect.IsNull();
}

// =======================================================================
// function : SetDimLengthModelUnits
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetDimLengthModelUnits (const TCollection_AsciiString& theUnits)
{
  myHasOwnDimLengthModelUnits = Standard_True;
  myDimensionModelUnits.SetLengthUnits (theUnits);
}

// =======================================================================
// function : SetDimAngleModelUnits
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetDimAngleModelUnits (const TCollection_AsciiString& theUnits)
{
  myHasOwnDimAngleModelUnits = Standard_True;
  myDimensionModelUnits.SetAngleUnits (theUnits);
}

// =======================================================================
// function : SetDimLengthDisplayUnits
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetDimLengthDisplayUnits (const TCollection_AsciiString& theUnits)
{
  myHasOwnDimLengthDisplayUnits = Standard_True;
  myDimensionDisplayUnits.SetLengthUnits (theUnits);
}

// =======================================================================
// function : SetDimAngleDisplayUnits
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetDimAngleDisplayUnits (const TCollection_AsciiString& theUnits)
{
  myHasOwnDimAngleDisplayUnits = Standard_True;
  myDimensionDisplayUnits.SetAngleUnits (theUnits);
}

// =======================================================================
// function : UIsoAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_IsoAspect)& Prs3d_Drawer::UIsoAspect()
{
  if (!HasOwnUIsoAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->UIsoAspect();
    }
    if (myUIsoAspect.IsNull())
    {
      myUIsoAspect = new Prs3d_IsoAspect (Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 1.0, 1);
    }
  }
  return myUIsoAspect;
}

// =======================================================================
// function : SetUIsoAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetUIsoAspect (const Handle(Prs3d_IsoAspect)& theAspect)
{
  myUIsoAspect = theAspect;
  myHasOwnUIsoAspect = !myUIsoAspect.IsNull();
}

// =======================================================================
// function : VIsoAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_IsoAspect)& Prs3d_Drawer::VIsoAspect()
{
  if (!HasOwnVIsoAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->VIsoAspect();
    }
    if (myVIsoAspect.IsNull())
    {
      myVIsoAspect = new Prs3d_IsoAspect (Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 1.0, 1);
    }
  }
  return myVIsoAspect;
}

// =======================================================================
// function : SetVIsoAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetVIsoAspect (const Handle(Prs3d_IsoAspect)& theAspect)
{
  myVIsoAspect = theAspect;
  myHasOwnVIsoAspect= !myVIsoAspect.IsNull();
}

// =======================================================================
// function : WireAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::WireAspect()
{
  if (!HasOwnWireAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->WireAspect();
    }
    if (myWireAspect.IsNull())
    {
      myWireAspect = new Prs3d_LineAspect (THE_DEF_COLOR_Wire, Aspect_TOL_SOLID, 1.0);
    }
  }
  return myWireAspect;
}

// =======================================================================
// function : SetWireAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetWireAspect (const Handle(Prs3d_LineAspect)& theAspect)
{
  myWireAspect = theAspect;
  myHasOwnWireAspect = !myWireAspect.IsNull();
}

// =======================================================================
// function : SetWireDraw
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetWireDraw (const Standard_Boolean theIsEnabled)
{
  myHasOwnWireDraw = Standard_True;
  myWireDraw       = theIsEnabled;
}

// =======================================================================
// function : PointAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_PointAspect)& Prs3d_Drawer::PointAspect()
{
  if (!HasOwnPointAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->PointAspect();
    }
    if (myPointAspect.IsNull())
    {
      myPointAspect = new Prs3d_PointAspect (Aspect_TOM_PLUS, Quantity_NOC_YELLOW, 1.0);
    }
  }
  return myPointAspect;
}

// =======================================================================
// function : SetupOwnPointAspect
// purpose  :
// =======================================================================
Standard_Boolean Prs3d_Drawer::SetupOwnPointAspect (const Handle(Prs3d_Drawer)& theDefaults)
{
  if (myHasOwnPointAspect)
  {
    return Standard_False;
  }

  myPointAspect = new Prs3d_PointAspect (Aspect_TOM_PLUS, Quantity_NOC_YELLOW, 1.0);
  if (!theDefaults.IsNull() && theDefaults != this)
  {
    *myPointAspect->Aspect() = *theDefaults->PointAspect()->Aspect();
  }
  else if (!myLink.IsNull())
  {
    *myPointAspect->Aspect() = *myLink->PointAspect()->Aspect();
  }
  myHasOwnPointAspect = Standard_True;
  return Standard_True;
}

// =======================================================================
// function : SetPointAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetPointAspect (const Handle(Prs3d_PointAspect)& theAspect)
{
  myPointAspect = theAspect;
  myHasOwnPointAspect = !myPointAspect.IsNull();
}

// =======================================================================
// function : LineAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::LineAspect()
{
  if (!HasOwnLineAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->LineAspect();
    }
    if (myLineAspect.IsNull())
    {
      myLineAspect = new Prs3d_LineAspect (THE_DEF_COLOR_Line, Aspect_TOL_SOLID, 1.0);
    }
  }
  return myLineAspect;
}

// =======================================================================
// function : SetLineAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetLineAspect (const Handle(Prs3d_LineAspect)& theAspect)
{
  myLineAspect = theAspect;
  myHasOwnLineAspect = !myLineAspect.IsNull();
}

// =======================================================================
// function : TextAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_TextAspect)& Prs3d_Drawer::TextAspect()
{
  if (!HasOwnTextAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->TextAspect();
    }
    if (myTextAspect.IsNull())
    {
      myTextAspect = new Prs3d_TextAspect();
    }
  }
  return myTextAspect;
}

// =======================================================================
// function : SetTextAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetTextAspect (const Handle(Prs3d_TextAspect)& theAspect)
{
  myTextAspect = theAspect;
  myHasOwnTextAspect = !myTextAspect.IsNull();
}

// =======================================================================
// function : ShadingAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_ShadingAspect)& Prs3d_Drawer::ShadingAspect()
{
  if (!HasOwnShadingAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->ShadingAspect();
    }
    if (myShadingAspect.IsNull())
    {
      myShadingAspect = new Prs3d_ShadingAspect();
    }
  }
  return myShadingAspect;
}

// =======================================================================
// function : SetupOwnShadingAspect
// purpose  :
// =======================================================================
Standard_Boolean Prs3d_Drawer::SetupOwnShadingAspect (const Handle(Prs3d_Drawer)& theDefaults)
{
  if (myHasOwnShadingAspect)
  {
    return Standard_False;
  }

  myShadingAspect = new Prs3d_ShadingAspect();
  if (!theDefaults.IsNull() && theDefaults != this)
  {
    *myShadingAspect->Aspect() = *theDefaults->ShadingAspect()->Aspect();
  }
  else if (!myLink.IsNull())
  {
    *myShadingAspect->Aspect() = *myLink->ShadingAspect()->Aspect();
  }
  myHasOwnShadingAspect = Standard_True;
  return Standard_True;
}

// =======================================================================
// function : SetShadingAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetShadingAspect (const Handle(Prs3d_ShadingAspect)& theAspect)
{
  myShadingAspect = theAspect;
  myHasOwnShadingAspect = !myShadingAspect.IsNull();
}

// =======================================================================
// function : PlaneAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_PlaneAspect)& Prs3d_Drawer::PlaneAspect()
{
  if (!HasOwnPlaneAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->PlaneAspect();
    }
    if (myPlaneAspect.IsNull())
    {
      myPlaneAspect = new Prs3d_PlaneAspect();
    }
  }
  return myPlaneAspect;
}

// =======================================================================
// function : SetPlaneAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetPlaneAspect (const Handle(Prs3d_PlaneAspect)& theAspect)
{
  myPlaneAspect = theAspect;
  myHasOwnPlaneAspect = !myPlaneAspect.IsNull();
}

// =======================================================================
// function : SeenLineAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::SeenLineAspect()
{
  if (!HasOwnSeenLineAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->SeenLineAspect();
    }
    if (mySeenLineAspect.IsNull())
    {
      mySeenLineAspect = new Prs3d_LineAspect (THE_DEF_COLOR_SeenLine, Aspect_TOL_SOLID, 1.0);
    }
  }
  return mySeenLineAspect;
}

// =======================================================================
// function : SetSeenLineAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetSeenLineAspect (const Handle(Prs3d_LineAspect)& theAspect)
{
  mySeenLineAspect = theAspect;
  myHasOwnSeenLineAspect = !mySeenLineAspect.IsNull();
}

// =======================================================================
// function : ArrowAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_ArrowAspect)& Prs3d_Drawer::ArrowAspect()
{
  if (!HasOwnArrowAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->ArrowAspect();
    }
    if (myArrowAspect.IsNull())
    {
      myArrowAspect = new Prs3d_ArrowAspect();
    }
  }
  return myArrowAspect;
}

// =======================================================================
// function : SetArrowAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetArrowAspect (const Handle(Prs3d_ArrowAspect)& theAspect)
{
  myArrowAspect = theAspect;
  myHasOwnArrowAspect = !myArrowAspect.IsNull();
}

// =======================================================================
// function : SetLineArrowDraw
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetLineArrowDraw (const Standard_Boolean theIsEnabled)
{
  myHasOwnLineArrowDraw = Standard_True;
  myLineArrowDraw       = theIsEnabled;
}

// =======================================================================
// function : HiddenLineAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::HiddenLineAspect()
{
  if (!HasOwnHiddenLineAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->HiddenLineAspect();
    }
    if (myHiddenLineAspect.IsNull())
    {
      myHiddenLineAspect = new Prs3d_LineAspect (THE_DEF_COLOR_HiddenLine, Aspect_TOL_DASH, 1.0);
    }
  }
  return myHiddenLineAspect;
}

// =======================================================================
// function : SetHiddenLineAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetHiddenLineAspect (const Handle(Prs3d_LineAspect)& theAspect)
{
  myHiddenLineAspect = theAspect;
  myHasOwnHiddenLineAspect = !myHiddenLineAspect.IsNull();
}

// =======================================================================
// function : EnableDrawHiddenLineDraw
// purpose  :
// =======================================================================

void Prs3d_Drawer::EnableDrawHiddenLine()
{
    myHasOwnDrawHiddenLine = Standard_True;
    myDrawHiddenLine       = Standard_True;
}

// =======================================================================
// function : DisableDrawHiddenLine
// purpose  :
// =======================================================================

void Prs3d_Drawer::DisableDrawHiddenLine()
{
    myHasOwnDrawHiddenLine = Standard_True;
    myDrawHiddenLine       = Standard_False;
}

// =======================================================================
// function : VectorAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::VectorAspect()
{
  if (!HasOwnVectorAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->VectorAspect();
    }
    if (myVectorAspect.IsNull())
    {
      myVectorAspect = new Prs3d_LineAspect (THE_DEF_COLOR_Vector, Aspect_TOL_SOLID, 1.0);
    }
  }
  return myVectorAspect;
}

// =======================================================================
// function : SetVectorAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetVectorAspect (const Handle(Prs3d_LineAspect)& theAspect)
{
  myVectorAspect = theAspect;
  myHasOwnVectorAspect = !myVectorAspect.IsNull();
}

// =======================================================================
// function : SetVertexDrawMode
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetVertexDrawMode (const Prs3d_VertexDrawMode theMode)
{
  // Prs3d_VDM_Inherited is default value and means
  // that correct value should be taken from the Link if it exists.
  myVertexDrawMode = theMode;
}

// =======================================================================
// function : VertexDrawMode
// purpose  :
// =======================================================================

Prs3d_VertexDrawMode Prs3d_Drawer::VertexDrawMode()
{
  if (!HasOwnVertexDrawMode())
  {
      if (!myLink.IsNull())
      {
          return myLink->VertexDrawMode();
      }
      // Prs3d_VDM_Isolated is default value for this setting.
      myVertexDrawMode = Prs3d_VDM_Isolated;
  }
  return myVertexDrawMode;
}

// =======================================================================
// function : DatumAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_DatumAspect)& Prs3d_Drawer::DatumAspect()
{
  if (!HasOwnDatumAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->DatumAspect();
    }
    if (myDatumAspect.IsNull())
    {
      myDatumAspect = new Prs3d_DatumAspect();
    }
  }
  return myDatumAspect;
}

// =======================================================================
// function : SetDatumAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetDatumAspect (const Handle(Prs3d_DatumAspect)& theAspect)
{
  myDatumAspect = theAspect;
  myHasOwnDatumAspect = !myDatumAspect.IsNull();
}

// =======================================================================
// function : SectionAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::SectionAspect()
{
  if (!HasOwnSectionAspect())
  {
    if (!myLink.IsNull())
    {
      return myLink->SectionAspect();
    }
    if (mySectionAspect.IsNull())
    {
      mySectionAspect = new Prs3d_LineAspect (THE_DEF_COLOR_Section, Aspect_TOL_SOLID, 1.0);
    }
  }
  return mySectionAspect;
}

// =======================================================================
// function : SetSectionAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetSectionAspect (const Handle(Prs3d_LineAspect)& theAspect)
{
  mySectionAspect = theAspect;
  myHasOwnSectionAspect = !mySectionAspect.IsNull();
}

// =======================================================================
// function : SetSectionAspect
// purpose  :
// =======================================================================

void Prs3d_Drawer::ClearLocalAttributes()
{
  if (myLink.IsNull())
  {
    return;
  }

  myUIsoAspect.Nullify();
  myVIsoAspect.Nullify();
  myFreeBoundaryAspect.Nullify();
  myUnFreeBoundaryAspect.Nullify();
  myFaceBoundaryAspect.Nullify();
  myWireAspect.Nullify();
  myLineAspect.Nullify();
  myTextAspect.Nullify();
  myShadingAspect.Nullify();
  myPointAspect.Nullify();
  myPlaneAspect.Nullify();
  myArrowAspect.Nullify();
  myHiddenLineAspect.Nullify();
  mySeenLineAspect.Nullify();
  myVectorAspect .Nullify();
  myDatumAspect.Nullify();
  myDimensionAspect.Nullify();
  mySectionAspect.Nullify();

  myHasOwnUIsoAspect           = Standard_False;
  myHasOwnVIsoAspect           = Standard_False;
  myHasOwnWireAspect           = Standard_False;
  myHasOwnPointAspect          = Standard_False;
  myHasOwnLineAspect           = Standard_False;
  myHasOwnTextAspect           = Standard_False;
  myHasOwnShadingAspect        = Standard_False;
  myHasOwnPlaneAspect          = Standard_False;
  myHasOwnSeenLineAspect       = Standard_False;
  myHasOwnArrowAspect          = Standard_False;
  myHasOwnHiddenLineAspect     = Standard_False;
  myHasOwnVectorAspect         = Standard_False;
  myHasOwnDatumAspect          = Standard_False;
  myHasOwnSectionAspect        = Standard_False;
  myHasOwnFreeBoundaryAspect   = Standard_False;
  myHasOwnUnFreeBoundaryAspect = Standard_False;
  myHasOwnFaceBoundaryAspect   = Standard_False;
  myHasOwnDimensionAspect      = Standard_False;

  myHasOwnNbPoints                = Standard_False;
  myHasOwnMaximalParameterValue   = Standard_False;
  myHasOwnTypeOfDeflection        = Standard_False;
  myHasOwnChordialDeviation       = Standard_False;
  myHasOwnDeviationCoefficient    = Standard_False;
  myHasOwnDeviationAngle          = Standard_False;
  myHasOwnIsoOnPlane              = Standard_False;
  myHasOwnIsoOnTriangulation      = Standard_False;
  myHasOwnIsAutoTriangulated      = Standard_False;
  myHasOwnWireDraw                = Standard_False;
  myHasOwnLineArrowDraw           = Standard_False;
  myHasOwnDrawHiddenLine          = Standard_False;
  myHasOwnFreeBoundaryDraw        = Standard_False;
  myHasOwnUnFreeBoundaryDraw      = Standard_False;
  myHasOwnFaceBoundaryDraw        = Standard_False;
  myHasOwnDimLengthModelUnits     = Standard_False;
  myHasOwnDimLengthDisplayUnits   = Standard_False;
  myHasOwnDimAngleModelUnits      = Standard_False;
  myHasOwnDimAngleDisplayUnits    = Standard_False;

  myVertexDrawMode = Prs3d_VDM_Inherited;
  myTypeOfHLR      = Prs3d_TOH_NotSet;
}

// =======================================================================
// function : SetupOwnFaceBoundaryAspect
// purpose  :
// =======================================================================
Standard_Boolean Prs3d_Drawer::SetupOwnFaceBoundaryAspect (const Handle(Prs3d_Drawer)& theDefaults)
{
  if (myHasOwnFaceBoundaryAspect)
  {
    return false;
  }

  myFaceBoundaryAspect = new Prs3d_LineAspect (THE_DEF_COLOR_FaceBoundary, Aspect_TOL_SOLID, 1.0);
  myHasOwnFaceBoundaryAspect = true;

  const Handle(Prs3d_Drawer)& aLink = (!theDefaults.IsNull() && theDefaults != this) ? theDefaults : myLink;
  if (!aLink.IsNull())
  {
    *myFaceBoundaryAspect->Aspect() = *aLink->FaceBoundaryAspect()->Aspect();
  }
  return true;
}

// =======================================================================
// function : SetOwnLineAspects
// purpose  :
// =======================================================================
Standard_Boolean Prs3d_Drawer::SetOwnLineAspects (const Handle(Prs3d_Drawer)& theDefaults)
{
  bool isUpdateNeeded = false;
  const Handle(Prs3d_Drawer)& aLink = (!theDefaults.IsNull() && theDefaults != this) ? theDefaults : myLink;
  if (!myHasOwnUIsoAspect)
  {
    isUpdateNeeded = true;
    myUIsoAspect = new Prs3d_IsoAspect (Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 1.0, 1);
    if (!aLink.IsNull())
    {
      *myUIsoAspect->Aspect() = *aLink->UIsoAspect()->Aspect();
      myUIsoAspect->SetNumber (aLink->UIsoAspect()->Number());
    }
    myHasOwnUIsoAspect = true;
  }
  if (!myHasOwnVIsoAspect)
  {
    isUpdateNeeded = true;
    myVIsoAspect = new Prs3d_IsoAspect (Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 1.0, 1);
    if (!aLink.IsNull())
    {
      *myVIsoAspect->Aspect() = *aLink->VIsoAspect()->Aspect();
      myUIsoAspect->SetNumber (aLink->VIsoAspect()->Number());
    }
    myHasOwnVIsoAspect = true;
  }
  if (!myHasOwnWireAspect)
  {
    isUpdateNeeded = true;
    myWireAspect = new Prs3d_LineAspect (THE_DEF_COLOR_Wire, Aspect_TOL_SOLID, 1.0);
    myHasOwnWireAspect = true;
    if (!aLink.IsNull())
    {
      *myWireAspect->Aspect() = *aLink->WireAspect()->Aspect();
    }
  }
  if (!myHasOwnLineAspect)
  {
    isUpdateNeeded = true;
    myLineAspect = new Prs3d_LineAspect (THE_DEF_COLOR_Line, Aspect_TOL_SOLID, 1.0);
    myHasOwnLineAspect = true;
    if (!aLink.IsNull())
    {
      *myLineAspect->Aspect() = *aLink->LineAspect()->Aspect();
    }
  }
  if (!myHasOwnSeenLineAspect)
  {
    isUpdateNeeded = true;
    mySeenLineAspect = new Prs3d_LineAspect (THE_DEF_COLOR_SeenLine, Aspect_TOL_SOLID, 1.0);
    myHasOwnSeenLineAspect = true;
    if (!aLink.IsNull())
    {
      *mySeenLineAspect->Aspect() = *aLink->SeenLineAspect()->Aspect();
    }
  }
  if (!myHasOwnHiddenLineAspect)
  {
    isUpdateNeeded = true;
    myHiddenLineAspect = new Prs3d_LineAspect (THE_DEF_COLOR_HiddenLine, Aspect_TOL_DASH, 1.0);
    myHasOwnHiddenLineAspect = true;
    if (!aLink.IsNull())
    {
      *myHiddenLineAspect->Aspect() = *aLink->HiddenLineAspect()->Aspect();
    }
  }
  if (!myHasOwnFreeBoundaryAspect)
  {
    isUpdateNeeded = true;
    myFreeBoundaryAspect = new Prs3d_LineAspect (THE_DEF_COLOR_FreeBoundary, Aspect_TOL_SOLID, 1.0);
    myHasOwnFreeBoundaryAspect = true;
    if (!aLink.IsNull())
    {
      *myFreeBoundaryAspect->Aspect() = *aLink->FreeBoundaryAspect()->Aspect();
    }
  }
  if (!myHasOwnUnFreeBoundaryAspect)
  {
    isUpdateNeeded = true;
    myUnFreeBoundaryAspect = new Prs3d_LineAspect (THE_DEF_COLOR_UnFreeBoundary, Aspect_TOL_SOLID, 1.0);
    myHasOwnUnFreeBoundaryAspect = true;
    if (!aLink.IsNull())
    {
      *myUnFreeBoundaryAspect->Aspect() = *aLink->UnFreeBoundaryAspect()->Aspect();
    }
  }
  isUpdateNeeded = SetupOwnFaceBoundaryAspect (theDefaults) || isUpdateNeeded;
  return isUpdateNeeded;
}

// =======================================================================
// function : SetOwnDatumAspects
// purpose  :
// =======================================================================
Standard_Boolean Prs3d_Drawer::SetOwnDatumAspects (const Handle(Prs3d_Drawer)& theDefaults)
{
  bool isUpdateNeeded = false;
  const Handle(Prs3d_Drawer)& aLink = (!theDefaults.IsNull() && theDefaults != this) ? theDefaults : myLink;
  if (!myHasOwnVectorAspect)
  {
    isUpdateNeeded = true;
    myVectorAspect = new Prs3d_LineAspect (THE_DEF_COLOR_Vector, Aspect_TOL_SOLID, 1.0);
    myHasOwnVectorAspect = true;
    if (!aLink.IsNull())
    {
      *myVectorAspect->Aspect() = *aLink->VectorAspect()->Aspect();
    }
  }
  if (!myHasOwnSectionAspect)
  {
    isUpdateNeeded = true;
    mySectionAspect = new Prs3d_LineAspect (THE_DEF_COLOR_Section, Aspect_TOL_SOLID, 1.0);
    myHasOwnSectionAspect = true;
    if (!aLink.IsNull())
    {
      *mySectionAspect->Aspect() = *aLink->SectionAspect()->Aspect();
    }
  }
  if (!myHasOwnPlaneAspect)
  {
    isUpdateNeeded = true;
    myPlaneAspect = new Prs3d_PlaneAspect();
    myHasOwnPlaneAspect = true;
  }
  if (!myHasOwnArrowAspect)
  {
    isUpdateNeeded = true;
    myArrowAspect = new Prs3d_ArrowAspect();
    myHasOwnArrowAspect = true;
  }
  if (!myHasOwnDatumAspect)
  {
    isUpdateNeeded = true;
    myDatumAspect = new Prs3d_DatumAspect();
    myHasOwnDatumAspect = true;
  }
  return isUpdateNeeded;
}

//! Assign the shader program.
template <typename T>
inline void setAspectProgram (const Handle(Graphic3d_ShaderProgram)& theProgram,
                              bool theHasAspect,
                              T thePrsAspect)
{
  if (!thePrsAspect.IsNull()
    && theHasAspect)
  {
    thePrsAspect->Aspect()->SetShaderProgram (theProgram);
  }
}

// =======================================================================
// function : SetShaderProgram
// purpose  :
// =======================================================================
bool Prs3d_Drawer::SetShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProgram,
                                     const Graphic3d_GroupAspect            theAspect,
                                     const bool                             theToOverrideDefaults)
{
  bool isUpdateNeeded = false;
  switch (theAspect)
  {
    case Graphic3d_ASPECT_LINE:
    {
      if (theToOverrideDefaults)
      {
        isUpdateNeeded = SetOwnLineAspects()  || isUpdateNeeded;
        isUpdateNeeded = SetOwnDatumAspects() || isUpdateNeeded;
      }

      setAspectProgram (theProgram, myHasOwnUIsoAspect, myUIsoAspect);
      setAspectProgram (theProgram, myHasOwnVIsoAspect, myVIsoAspect);
      setAspectProgram (theProgram, myHasOwnWireAspect, myWireAspect);
      setAspectProgram (theProgram, myHasOwnLineAspect, myLineAspect);
      setAspectProgram (theProgram, myHasOwnSeenLineAspect,       mySeenLineAspect);
      setAspectProgram (theProgram, myHasOwnHiddenLineAspect,     myHiddenLineAspect);
      setAspectProgram (theProgram, myHasOwnVectorAspect,         myVectorAspect);
      setAspectProgram (theProgram, myHasOwnSectionAspect,        mySectionAspect);
      setAspectProgram (theProgram, myHasOwnFreeBoundaryAspect,   myFreeBoundaryAspect);
      setAspectProgram (theProgram, myHasOwnUnFreeBoundaryAspect, myUnFreeBoundaryAspect);
      setAspectProgram (theProgram, myHasOwnFaceBoundaryAspect,   myFaceBoundaryAspect);
      if (myHasOwnPlaneAspect)
      {
        setAspectProgram (theProgram, true, myPlaneAspect->EdgesAspect());
        setAspectProgram (theProgram, true, myPlaneAspect->IsoAspect());
        setAspectProgram (theProgram, true, myPlaneAspect->ArrowAspect());
      }
      if (myHasOwnDatumAspect)
      {
        setAspectProgram (theProgram, true, myDatumAspect->LineAspect(Prs3d_DP_XAxis));
        setAspectProgram (theProgram, true, myDatumAspect->LineAspect(Prs3d_DP_YAxis));
        setAspectProgram (theProgram, true, myDatumAspect->LineAspect(Prs3d_DP_ZAxis));
      }
      setAspectProgram (theProgram, myHasOwnArrowAspect, myArrowAspect);
      return isUpdateNeeded;
    }
    case Graphic3d_ASPECT_TEXT:
    {
      if (theToOverrideDefaults
      && !myHasOwnTextAspect)
      {
        isUpdateNeeded = true;
        myTextAspect = new Prs3d_TextAspect();
        myHasOwnTextAspect = true;
        if (!myLink.IsNull())
        {
          *myTextAspect->Aspect() = *myLink->TextAspect()->Aspect();
        }
      }

      setAspectProgram (theProgram, myHasOwnTextAspect, myTextAspect);
      return isUpdateNeeded;
    }
    case Graphic3d_ASPECT_MARKER:
    {
      if (theToOverrideDefaults
       && SetupOwnPointAspect())
      {
        isUpdateNeeded = true;
      }

      setAspectProgram (theProgram, myHasOwnPointAspect, myPointAspect);
      return isUpdateNeeded;
    }
    case Graphic3d_ASPECT_FILL_AREA:
    {
      if (theToOverrideDefaults
       && SetupOwnShadingAspect())
      {
        isUpdateNeeded = true;
      }
      setAspectProgram (theProgram, myHasOwnShadingAspect, myShadingAspect);
      return isUpdateNeeded;
    }
  }
  return false;
}

// =======================================================================
// function : SetShadingModel
// purpose  :
// =======================================================================
bool Prs3d_Drawer::SetShadingModel (Graphic3d_TypeOfShadingModel theModel,
                                    bool theToOverrideDefaults)
{
  bool isUpdateNeeded = false;

  if (theToOverrideDefaults
   && SetupOwnShadingAspect())
  {
    isUpdateNeeded  = true;
  }

  if (!myShadingAspect.IsNull()
    && myHasOwnShadingAspect)
  {
    myShadingAspect->Aspect()->SetShadingModel (theModel);
  }

  return isUpdateNeeded;
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void Prs3d_Drawer::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myLink.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnNbPoints)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myMaximalParameterValue)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnMaximalParameterValue)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myChordialDeviation)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnChordialDeviation)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTypeOfDeflection)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnTypeOfDeflection)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTypeOfHLR)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDeviationCoefficient)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myPreviousDeviationCoefficient)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnDeviationCoefficient)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDeviationAngle)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnDeviationAngle)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myPreviousDeviationAngle)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsoOnPlane)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnIsoOnPlane)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsoOnTriangulation)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnIsoOnTriangulation)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsAutoTriangulated)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnIsAutoTriangulated)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnUIsoAspect)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnVIsoAspect)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnWireAspect)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myWireDraw)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnWireDraw)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnPointAspect)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnLineAspect)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myTextAspect.get())

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myShadingAspect.get())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnShadingAspect)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnPlaneAspect)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnSeenLineAspect)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnArrowAspect)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myLineArrowDraw)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnLineArrowDraw)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnHiddenLineAspect)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myDrawHiddenLine)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnDrawHiddenLine)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnVectorAspect)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myVertexDrawMode)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnDatumAspect)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnSectionAspect)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnFreeBoundaryAspect)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myFreeBoundaryDraw)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnFreeBoundaryDraw)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnUnFreeBoundaryAspect)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myUnFreeBoundaryDraw)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnUnFreeBoundaryDraw)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myFaceBoundaryUpperContinuity)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnFaceBoundaryAspect)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myFaceBoundaryDraw)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnFaceBoundaryDraw)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnDimensionAspect)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnDimLengthModelUnits)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnDimAngleModelUnits)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnDimLengthDisplayUnits)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myHasOwnDimAngleDisplayUnits)
}
