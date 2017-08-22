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

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_Drawer, Graphic3d_PresentationAttributes)

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
  myHLRDeviationCoefficient       (0.02),
  myHasOwnHLRDeviationCoefficient (Standard_False),
  myDeviationAngle                (12.0 * M_PI / 180.0),
  myHasOwnDeviationAngle          (Standard_False),
  myHLRAngle                      (20.0 * M_PI / 180.0),
  myHasOwnHLRDeviationAngle       (Standard_False),
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
//function : SetHLRDeviationCoefficient
//purpose  : 
//=======================================================================

void Prs3d_Drawer::SetHLRDeviationCoefficient (const Standard_Real theCoefficient)
{
  myPreviousHLRDeviationCoefficient = HLRDeviationCoefficient();
  myHLRDeviationCoefficient         = theCoefficient;
  myHasOwnHLRDeviationCoefficient   = Standard_True;
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

//=======================================================================
//function : SetHLRAngle
//purpose  : 
//=======================================================================

void Prs3d_Drawer::SetHLRAngle (const Standard_Real theAngle)
{
  myPreviousHLRDeviationAngle = HLRAngle();
  myHLRAngle                  = theAngle;
  myHasOwnHLRDeviationAngle   = Standard_True;
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
      myFreeBoundaryAspect = new Prs3d_LineAspect (Quantity_NOC_GREEN, Aspect_TOL_SOLID, 1.0);
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
      myUnFreeBoundaryAspect = new Prs3d_LineAspect (Quantity_NOC_YELLOW, Aspect_TOL_SOLID, 1.0);
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
      myFaceBoundaryAspect = new Prs3d_LineAspect (Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0);
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
      myWireAspect = new Prs3d_LineAspect (Quantity_NOC_RED, Aspect_TOL_SOLID, 1.0);
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
      myLineAspect = new Prs3d_LineAspect (Quantity_NOC_YELLOW, Aspect_TOL_SOLID, 1.0);
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
      mySeenLineAspect = new Prs3d_LineAspect (Quantity_NOC_YELLOW, Aspect_TOL_SOLID, 1.0);
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
      myHiddenLineAspect = new Prs3d_LineAspect (Quantity_NOC_YELLOW, Aspect_TOL_DASH, 1.0);
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
      myVectorAspect = new Prs3d_LineAspect (Quantity_NOC_SKYBLUE, Aspect_TOL_SOLID, 1.0);
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
      mySectionAspect = new Prs3d_LineAspect (Quantity_NOC_ORANGE, Aspect_TOL_SOLID, 1.0);
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
  myHasOwnHLRDeviationCoefficient = Standard_False;
  myHasOwnDeviationAngle          = Standard_False;
  myHasOwnHLRDeviationAngle       = Standard_False;
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

//! Copy line aspect defaults from the Link.
inline void copyLineAspect (const Handle(Prs3d_Drawer)&     theLink,
                            Handle(Prs3d_LineAspect)&       theAspect,
                            const Handle(Prs3d_LineAspect)& theBaseAspect)
{
  Handle(Prs3d_LineAspect) aBaseAspect = theBaseAspect;
  if (!theLink.IsNull())
  {
    theAspect = new Prs3d_LineAspect (Quantity_NOC_WHITE, Aspect_TOL_SOLID, 1.0);
    *theAspect->Aspect() = *aBaseAspect->Aspect();
  }
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
void Prs3d_Drawer::SetShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProgram,
                                     const Graphic3d_GroupAspect            theAspect,
                                     const bool                             theToOverrideDefaults)
{
  switch (theAspect)
  {
    case Graphic3d_ASPECT_LINE:
    {
      if (theToOverrideDefaults)
      {
        if (!myHasOwnUIsoAspect)
        {
          Handle(Prs3d_IsoAspect) anAspect = UIsoAspect();
          if (!myLink.IsNull())
          {
            myUIsoAspect = new Prs3d_IsoAspect (Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 1.0, 1);
            *myUIsoAspect->Aspect() = *anAspect->Aspect();
            myUIsoAspect->SetNumber (anAspect->Number());
          }
          myHasOwnUIsoAspect = true;
        }
        if (!myHasOwnVIsoAspect)
        {
          Handle(Prs3d_IsoAspect) anAspect = VIsoAspect();
          if (!myLink.IsNull())
          {
            myVIsoAspect = new Prs3d_IsoAspect (Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 1.0, 1);
            *myVIsoAspect->Aspect() = *anAspect->Aspect();
            myUIsoAspect->SetNumber (anAspect->Number());
          }
          myHasOwnVIsoAspect = true;
        }
        if (!myHasOwnWireAspect)
        {
          copyLineAspect (myLink, myWireAspect, WireAspect());
          myHasOwnWireAspect = true;
        }
        if (!myHasOwnLineAspect)
        {
          copyLineAspect (myLink, myLineAspect, LineAspect());
          myHasOwnLineAspect = true;
        }
        if (!myHasOwnSeenLineAspect)
        {
          copyLineAspect (myLink, mySeenLineAspect, SeenLineAspect());
          myHasOwnSeenLineAspect = true;
        }
        if (!myHasOwnHiddenLineAspect)
        {
          copyLineAspect (myLink, myHiddenLineAspect, HiddenLineAspect());
          myHasOwnHiddenLineAspect = true;
        }
        if (!myHasOwnVectorAspect)
        {
          copyLineAspect (myLink, myVectorAspect, VectorAspect());
          myHasOwnVectorAspect = true;
        }
        if (!myHasOwnSectionAspect)
        {
          copyLineAspect (myLink, mySectionAspect, SectionAspect());
          myHasOwnSectionAspect = true;
        }
        if (!myHasOwnFreeBoundaryAspect)
        {
          copyLineAspect (myLink, myFreeBoundaryAspect, FreeBoundaryAspect());
          myHasOwnFreeBoundaryAspect = true;
        }
        if (!myHasOwnUnFreeBoundaryAspect)
        {
          copyLineAspect (myLink, myUnFreeBoundaryAspect, UnFreeBoundaryAspect());
          myHasOwnUnFreeBoundaryAspect = true;
        }
        if (!myHasOwnFaceBoundaryAspect)
        {
          copyLineAspect (myLink, myFaceBoundaryAspect, FaceBoundaryAspect());
          myHasOwnFaceBoundaryAspect = true;
        }

        if (!myHasOwnPlaneAspect)
        {
          myPlaneAspect = new Prs3d_PlaneAspect();
          myHasOwnPlaneAspect = true;
        }
        if (!myHasOwnArrowAspect)
        {
          myArrowAspect = new Prs3d_ArrowAspect();
          myHasOwnArrowAspect = true;
        }
        if (!myHasOwnDatumAspect)
        {
          myDatumAspect = new Prs3d_DatumAspect();
          myHasOwnDatumAspect = true;
        }
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
      return;
    }
    case Graphic3d_ASPECT_TEXT:
    {
      if (theToOverrideDefaults
      && !myHasOwnTextAspect)
      {
        myTextAspect = new Prs3d_TextAspect();
        myHasOwnTextAspect = true;
        if (!myLink.IsNull())
        {
          *myTextAspect->Aspect() = *myLink->TextAspect()->Aspect();
        }
      }

      setAspectProgram (theProgram, myHasOwnTextAspect, myTextAspect);
      return;
    }
    case Graphic3d_ASPECT_MARKER:
    {
      if (theToOverrideDefaults
      && !myHasOwnPointAspect)
      {
        myPointAspect = new Prs3d_PointAspect (Aspect_TOM_PLUS, Quantity_NOC_YELLOW, 1.0);
        myHasOwnPointAspect = true;
        if (!myLink.IsNull())
        {
          *myPointAspect->Aspect() = *myLink->PointAspect()->Aspect();
        }
      }

      setAspectProgram (theProgram, myHasOwnPointAspect, myPointAspect);
      return;
    }
    case Graphic3d_ASPECT_FILL_AREA:
    {
      if (theToOverrideDefaults
      && !myHasOwnShadingAspect)
      {
        myShadingAspect = new Prs3d_ShadingAspect();
        myHasOwnShadingAspect = true;
        if (!myLink.IsNull())
        {
          *myShadingAspect->Aspect() = *myLink->ShadingAspect()->Aspect();
        }
      }
      setAspectProgram (theProgram, myHasOwnShadingAspect, myShadingAspect);
      return;
    }
  }
}
