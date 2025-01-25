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
} // namespace

// =======================================================================
// function : Prs3d_Drawer
// purpose  :
// =======================================================================
Prs3d_Drawer::Prs3d_Drawer()
    : myNbPoints(-1),
      myMaximalParameterValue(-1.0),
      myChordialDeviation(-1.0),
      myTypeOfDeflection(Aspect_TOD_RELATIVE),
      myHasOwnTypeOfDeflection(Standard_False),
      myTypeOfHLR(Prs3d_TOH_NotSet),
      myDeviationCoefficient(-1.0),
      myDeviationAngle(-1.0),
      myIsoOnPlane(Standard_False),
      myHasOwnIsoOnPlane(Standard_False),
      myIsoOnTriangulation(Standard_False),
      myHasOwnIsoOnTriangulation(Standard_False),
      myIsAutoTriangulated(Standard_True),
      myHasOwnIsAutoTriangulated(Standard_False),

      myWireDraw(Standard_True),
      myHasOwnWireDraw(Standard_False),
      myLineArrowDraw(Standard_False),
      myHasOwnLineArrowDraw(Standard_False),
      myDrawHiddenLine(Standard_False),
      myHasOwnDrawHiddenLine(Standard_False),
      myVertexDrawMode(Prs3d_VDM_Inherited),

      myFreeBoundaryDraw(Standard_True),
      myHasOwnFreeBoundaryDraw(Standard_False),
      myUnFreeBoundaryDraw(Standard_True),
      myHasOwnUnFreeBoundaryDraw(Standard_False),
      myFaceBoundaryUpperContinuity(-1),
      myFaceBoundaryDraw(Standard_False),
      myHasOwnFaceBoundaryDraw(Standard_False),

      myHasOwnDimLengthModelUnits(Standard_False),
      myHasOwnDimAngleModelUnits(Standard_False),
      myHasOwnDimLengthDisplayUnits(Standard_False),
      myHasOwnDimAngleDisplayUnits(Standard_False)
{
  myDimensionModelUnits.SetLengthUnits("m");
  myDimensionModelUnits.SetAngleUnits("rad");
  myDimensionDisplayUnits.SetLengthUnits("m");
  myDimensionDisplayUnits.SetAngleUnits("deg");
}

// =======================================================================
// function : SetupOwnDefaults
// purpose  :
// =======================================================================
void Prs3d_Drawer::SetupOwnDefaults()
{
  myNbPoints              = 30;
  myMaximalParameterValue = 500000.0;
  myChordialDeviation     = 0.0001;
  myDeviationCoefficient  = 0.001;
  myDeviationAngle        = 20.0 * M_PI / 180.0;
  SetupOwnShadingAspect();
  SetupOwnPointAspect();
  SetOwnDatumAspects();
  SetOwnLineAspects();
  SetTextAspect(new Prs3d_TextAspect());
  SetDimensionAspect(new Prs3d_DimensionAspect());
}

// =======================================================================
// function : SetTypeOfDeflection
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetTypeOfDeflection(const Aspect_TypeOfDeflection theTypeOfDeflection)
{
  myHasOwnTypeOfDeflection = Standard_True;
  myTypeOfDeflection       = theTypeOfDeflection;
}

// =======================================================================
// function : SetTypeOfHLR
// purpose  : set type of HLR algorithm
// =======================================================================

void Prs3d_Drawer::SetTypeOfHLR(const Prs3d_TypeOfHLR theTypeOfHLR)
{
  myTypeOfHLR = theTypeOfHLR;
}

// =======================================================================
// function : TypeOfHLR
// purpose  : gets type of HLR algorithm
// =======================================================================

Prs3d_TypeOfHLR Prs3d_Drawer::TypeOfHLR() const
{
  if (HasOwnTypeOfHLR())
  {
    return myTypeOfHLR;
  }
  else if (!myLink.IsNull())
  {
    return myLink->TypeOfHLR();
  }
  return Prs3d_TOH_PolyAlgo;
}

// =======================================================================
// function : SetIsoOnTriangulation
// purpose  :
// =======================================================================
void Prs3d_Drawer::SetIsoOnTriangulation(const Standard_Boolean theToEnable)
{
  myHasOwnIsoOnTriangulation = Standard_True;
  myIsoOnTriangulation       = theToEnable;
}

// =======================================================================
// function : SetIsoOnPlane
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetIsoOnPlane(const Standard_Boolean theIsEnabled)
{
  myHasOwnIsoOnPlane = Standard_True;
  myIsoOnPlane       = theIsEnabled;
}

//=================================================================================================

void Prs3d_Drawer::SetDeviationCoefficient(const Standard_Real theCoefficient)
{
  myPreviousDeviationCoefficient = DeviationCoefficient();
  myDeviationCoefficient         = theCoefficient;
}

//=================================================================================================

void Prs3d_Drawer::SetDeviationAngle(const Standard_Real theAngle)
{
  myPreviousDeviationAngle = DeviationAngle();
  myDeviationAngle         = theAngle;
}

// =======================================================================
// function : SetAutoTriangulation
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetAutoTriangulation(const Standard_Boolean theIsEnabled)
{
  myHasOwnIsAutoTriangulated = Standard_True;
  myIsAutoTriangulated       = theIsEnabled;
}

// =======================================================================
// function : FreeBoundaryAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::FreeBoundaryAspect() const
{
  if (myFreeBoundaryAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->FreeBoundaryAspect();
  }
  return myFreeBoundaryAspect;
}

// =======================================================================
// function : SetFreeBoundaryDraw
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetFreeBoundaryDraw(const Standard_Boolean theIsEnabled)
{
  myHasOwnFreeBoundaryDraw = Standard_True;
  myFreeBoundaryDraw       = theIsEnabled;
}

// =======================================================================
// function : UnFreeBoundaryAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::UnFreeBoundaryAspect() const
{
  if (myUnFreeBoundaryAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->UnFreeBoundaryAspect();
  }
  return myUnFreeBoundaryAspect;
}

// =======================================================================
// function : SetUnFreeBoundaryDraw
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetUnFreeBoundaryDraw(const Standard_Boolean theIsEnabled)
{
  myHasOwnUnFreeBoundaryDraw = Standard_True;
  myUnFreeBoundaryDraw       = theIsEnabled;
}

// =======================================================================
// function : FaceBoundaryAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::FaceBoundaryAspect() const
{
  if (myFaceBoundaryAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->FaceBoundaryAspect();
  }
  return myFaceBoundaryAspect;
}

// =======================================================================
// function : SetFaceBoundaryDraw
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetFaceBoundaryDraw(const Standard_Boolean theIsEnabled)
{
  myHasOwnFaceBoundaryDraw = Standard_True;
  myFaceBoundaryDraw       = theIsEnabled;
}

// =======================================================================
// function : DimensionAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_DimensionAspect)& Prs3d_Drawer::DimensionAspect() const
{
  if (myDimensionAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->DimensionAspect();
  }
  return myDimensionAspect;
}

// =======================================================================
// function : SetDimLengthModelUnits
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetDimLengthModelUnits(const TCollection_AsciiString& theUnits)
{
  myHasOwnDimLengthModelUnits = Standard_True;
  myDimensionModelUnits.SetLengthUnits(theUnits);
}

// =======================================================================
// function : SetDimAngleModelUnits
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetDimAngleModelUnits(const TCollection_AsciiString& theUnits)
{
  myHasOwnDimAngleModelUnits = Standard_True;
  myDimensionModelUnits.SetAngleUnits(theUnits);
}

// =======================================================================
// function : SetDimLengthDisplayUnits
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetDimLengthDisplayUnits(const TCollection_AsciiString& theUnits)
{
  myHasOwnDimLengthDisplayUnits = Standard_True;
  myDimensionDisplayUnits.SetLengthUnits(theUnits);
}

// =======================================================================
// function : SetDimAngleDisplayUnits
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetDimAngleDisplayUnits(const TCollection_AsciiString& theUnits)
{
  myHasOwnDimAngleDisplayUnits = Standard_True;
  myDimensionDisplayUnits.SetAngleUnits(theUnits);
}

// =======================================================================
// function : UIsoAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_IsoAspect)& Prs3d_Drawer::UIsoAspect() const
{
  if (myUIsoAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->UIsoAspect();
  }
  return myUIsoAspect;
}

// =======================================================================
// function : VIsoAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_IsoAspect)& Prs3d_Drawer::VIsoAspect() const
{
  if (myVIsoAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->VIsoAspect();
  }
  return myVIsoAspect;
}

// =======================================================================
// function : WireAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::WireAspect() const
{
  if (myWireAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->WireAspect();
  }
  return myWireAspect;
}

// =======================================================================
// function : SetWireDraw
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetWireDraw(const Standard_Boolean theIsEnabled)
{
  myHasOwnWireDraw = Standard_True;
  myWireDraw       = theIsEnabled;
}

// =======================================================================
// function : PointAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_PointAspect)& Prs3d_Drawer::PointAspect() const
{
  if (myPointAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->PointAspect();
  }
  return myPointAspect;
}

// =======================================================================
// function : SetupOwnPointAspect
// purpose  :
// =======================================================================
Standard_Boolean Prs3d_Drawer::SetupOwnPointAspect(const Handle(Prs3d_Drawer)& theDefaults)
{
  if (!myPointAspect.IsNull())
  {
    return Standard_False;
  }

  myPointAspect = new Prs3d_PointAspect(Aspect_TOM_PLUS, Quantity_NOC_YELLOW, 1.0);
  const Handle(Prs3d_Drawer)& aLink =
    (!theDefaults.IsNull() && theDefaults != this) ? theDefaults : myLink;
  if (const Prs3d_PointAspect* aLinked = !aLink.IsNull() ? aLink->PointAspect().get() : NULL)
  {
    *myPointAspect->Aspect() = *aLinked->Aspect();
  }

  return Standard_True;
}

// =======================================================================
// function : LineAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::LineAspect() const
{
  if (myLineAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->LineAspect();
  }
  return myLineAspect;
}

// =======================================================================
// function : TextAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_TextAspect)& Prs3d_Drawer::TextAspect() const
{
  if (myTextAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->TextAspect();
  }
  return myTextAspect;
}

// =======================================================================
// function : ShadingAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_ShadingAspect)& Prs3d_Drawer::ShadingAspect() const
{
  if (myShadingAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->ShadingAspect();
  }
  return myShadingAspect;
}

// =======================================================================
// function : SetupOwnShadingAspect
// purpose  :
// =======================================================================
Standard_Boolean Prs3d_Drawer::SetupOwnShadingAspect(const Handle(Prs3d_Drawer)& theDefaults)
{
  if (!myShadingAspect.IsNull())
  {
    return Standard_False;
  }

  myShadingAspect = new Prs3d_ShadingAspect();
  const Handle(Prs3d_Drawer)& aLink =
    (!theDefaults.IsNull() && theDefaults != this) ? theDefaults : myLink;
  if (const Prs3d_ShadingAspect* aLinked = !aLink.IsNull() ? aLink->ShadingAspect().get() : NULL)
  {
    *myShadingAspect->Aspect() = *aLinked->Aspect();
  }
  return Standard_True;
}

// =======================================================================
// function : PlaneAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_PlaneAspect)& Prs3d_Drawer::PlaneAspect() const
{
  if (myPlaneAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->PlaneAspect();
  }
  return myPlaneAspect;
}

// =======================================================================
// function : SeenLineAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::SeenLineAspect() const
{
  if (mySeenLineAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->SeenLineAspect();
  }
  return mySeenLineAspect;
}

// =======================================================================
// function : ArrowAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_ArrowAspect)& Prs3d_Drawer::ArrowAspect() const
{
  if (myArrowAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->ArrowAspect();
  }
  return myArrowAspect;
}

// =======================================================================
// function : SetLineArrowDraw
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetLineArrowDraw(const Standard_Boolean theIsEnabled)
{
  myHasOwnLineArrowDraw = Standard_True;
  myLineArrowDraw       = theIsEnabled;
}

// =======================================================================
// function : HiddenLineAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::HiddenLineAspect() const
{
  if (myHiddenLineAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->HiddenLineAspect();
  }
  return myHiddenLineAspect;
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

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::VectorAspect() const
{
  if (myVectorAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->VectorAspect();
  }
  return myVectorAspect;
}

// =======================================================================
// function : SetVertexDrawMode
// purpose  :
// =======================================================================

void Prs3d_Drawer::SetVertexDrawMode(const Prs3d_VertexDrawMode theMode)
{
  // Prs3d_VDM_Inherited is default value and means
  // that correct value should be taken from the Link if it exists.
  myVertexDrawMode = theMode;
}

// =======================================================================
// function : VertexDrawMode
// purpose  :
// =======================================================================

Prs3d_VertexDrawMode Prs3d_Drawer::VertexDrawMode() const
{
  if (HasOwnVertexDrawMode())
  {
    return myVertexDrawMode;
  }
  else if (!myLink.IsNull())
  {
    return myLink->VertexDrawMode();
  }
  return Prs3d_VDM_Isolated;
}

// =======================================================================
// function : DatumAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_DatumAspect)& Prs3d_Drawer::DatumAspect() const
{
  if (myDatumAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->DatumAspect();
  }
  return myDatumAspect;
}

// =======================================================================
// function : SectionAspect
// purpose  :
// =======================================================================

const Handle(Prs3d_LineAspect)& Prs3d_Drawer::SectionAspect() const
{
  if (mySectionAspect.IsNull() && !myLink.IsNull())
  {
    return myLink->SectionAspect();
  }
  return mySectionAspect;
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
  myVectorAspect.Nullify();
  myDatumAspect.Nullify();
  myDimensionAspect.Nullify();
  mySectionAspect.Nullify();

  UnsetOwnDiscretisation();
  UnsetOwnMaximalParameterValue();
  UnsetOwnTypeOfDeflection();
  UnsetOwnMaximalChordialDeviation();
  SetDeviationCoefficient();
  SetDeviationAngle();
  UnsetOwnIsoOnPlane();
  UnsetOwnIsoOnTriangulation();
  UnsetOwnIsAutoTriangulation();
  UnsetOwnWireDraw();
  UnsetOwnLineArrowDraw();
  UnsetOwnDrawHiddenLine();
  UnsetOwnFreeBoundaryDraw();
  UnsetOwnUnFreeBoundaryDraw();
  UnsetOwnFaceBoundaryDraw();
  UnsetOwnDimLengthModelUnits();
  UnsetOwnDimLengthDisplayUnits();
  UnsetOwnDimAngleModelUnits();
  UnsetOwnDimAngleDisplayUnits();

  myVertexDrawMode = Prs3d_VDM_Inherited;
  myTypeOfHLR      = Prs3d_TOH_NotSet;
}

// =======================================================================
// function : SetupOwnFaceBoundaryAspect
// purpose  :
// =======================================================================
Standard_Boolean Prs3d_Drawer::SetupOwnFaceBoundaryAspect(const Handle(Prs3d_Drawer)& theDefaults)
{
  if (!myFaceBoundaryAspect.IsNull())
  {
    return false;
  }

  myFaceBoundaryAspect = new Prs3d_LineAspect(THE_DEF_COLOR_FaceBoundary, Aspect_TOL_SOLID, 1.0);

  const Handle(Prs3d_Drawer)& aLink =
    (!theDefaults.IsNull() && theDefaults != this) ? theDefaults : myLink;
  if (const Prs3d_LineAspect* aLinked = !aLink.IsNull() ? aLink->FaceBoundaryAspect().get() : NULL)
  {
    *myFaceBoundaryAspect->Aspect() = *aLinked->Aspect();
  }
  return true;
}

// =======================================================================
// function : SetOwnLineAspects
// purpose  :
// =======================================================================
Standard_Boolean Prs3d_Drawer::SetOwnLineAspects(const Handle(Prs3d_Drawer)& theDefaults)
{
  bool                        isUpdateNeeded = false;
  const Handle(Prs3d_Drawer)& aLink =
    (!theDefaults.IsNull() && theDefaults != this) ? theDefaults : myLink;
  if (myUIsoAspect.IsNull())
  {
    isUpdateNeeded = true;
    myUIsoAspect   = new Prs3d_IsoAspect(Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 1.0, 1);
    if (const Prs3d_IsoAspect* aLinked = !aLink.IsNull() ? aLink->UIsoAspect().get() : NULL)
    {
      *myUIsoAspect->Aspect() = *aLinked->Aspect();
      myUIsoAspect->SetNumber(aLinked->Number());
    }
  }
  if (myVIsoAspect.IsNull())
  {
    isUpdateNeeded = true;
    myVIsoAspect   = new Prs3d_IsoAspect(Quantity_NOC_GRAY75, Aspect_TOL_SOLID, 1.0, 1);
    if (const Prs3d_IsoAspect* aLinked = !aLink.IsNull() ? aLink->VIsoAspect().get() : NULL)
    {
      *myVIsoAspect->Aspect() = *aLinked->Aspect();
      myVIsoAspect->SetNumber(aLinked->Number());
    }
  }
  if (myWireAspect.IsNull())
  {
    isUpdateNeeded = true;
    myWireAspect   = new Prs3d_LineAspect(THE_DEF_COLOR_Wire, Aspect_TOL_SOLID, 1.0);
    if (const Prs3d_LineAspect* aLinked = !aLink.IsNull() ? aLink->WireAspect().get() : NULL)
    {
      *myWireAspect->Aspect() = *aLinked->Aspect();
    }
  }
  if (myLineAspect.IsNull())
  {
    isUpdateNeeded = true;
    myLineAspect   = new Prs3d_LineAspect(THE_DEF_COLOR_Line, Aspect_TOL_SOLID, 1.0);
    if (const Prs3d_LineAspect* aLinked = !aLink.IsNull() ? aLink->LineAspect().get() : NULL)
    {
      *myLineAspect->Aspect() = *aLinked->Aspect();
    }
  }
  if (mySeenLineAspect.IsNull())
  {
    isUpdateNeeded   = true;
    mySeenLineAspect = new Prs3d_LineAspect(THE_DEF_COLOR_SeenLine, Aspect_TOL_SOLID, 1.0);
    if (const Prs3d_LineAspect* aLinked = !aLink.IsNull() ? aLink->SeenLineAspect().get() : NULL)
    {
      *mySeenLineAspect->Aspect() = *aLinked->Aspect();
    }
  }
  if (myHiddenLineAspect.IsNull())
  {
    isUpdateNeeded     = true;
    myHiddenLineAspect = new Prs3d_LineAspect(THE_DEF_COLOR_HiddenLine, Aspect_TOL_DASH, 1.0);
    if (const Prs3d_LineAspect* aLinked = !aLink.IsNull() ? aLink->HiddenLineAspect().get() : NULL)
    {
      *myHiddenLineAspect->Aspect() = *aLinked->Aspect();
    }
  }
  if (myFreeBoundaryAspect.IsNull())
  {
    isUpdateNeeded       = true;
    myFreeBoundaryAspect = new Prs3d_LineAspect(THE_DEF_COLOR_FreeBoundary, Aspect_TOL_SOLID, 1.0);
    if (const Prs3d_LineAspect* aLinked =
          !aLink.IsNull() ? aLink->FreeBoundaryAspect().get() : NULL)
    {
      *myFreeBoundaryAspect->Aspect() = *aLinked->Aspect();
    }
  }
  if (myUnFreeBoundaryAspect.IsNull())
  {
    isUpdateNeeded = true;
    myUnFreeBoundaryAspect =
      new Prs3d_LineAspect(THE_DEF_COLOR_UnFreeBoundary, Aspect_TOL_SOLID, 1.0);
    if (const Prs3d_LineAspect* aLinked =
          !aLink.IsNull() ? aLink->UnFreeBoundaryAspect().get() : NULL)
    {
      *myUnFreeBoundaryAspect->Aspect() = *aLinked->Aspect();
    }
  }
  isUpdateNeeded = SetupOwnFaceBoundaryAspect(theDefaults) || isUpdateNeeded;
  return isUpdateNeeded;
}

// =======================================================================
// function : SetOwnDatumAspects
// purpose  :
// =======================================================================
Standard_Boolean Prs3d_Drawer::SetOwnDatumAspects(const Handle(Prs3d_Drawer)& theDefaults)
{
  bool                        isUpdateNeeded = false;
  const Handle(Prs3d_Drawer)& aLink =
    (!theDefaults.IsNull() && theDefaults != this) ? theDefaults : myLink;
  if (myVectorAspect.IsNull())
  {
    isUpdateNeeded = true;
    myVectorAspect = new Prs3d_LineAspect(THE_DEF_COLOR_Vector, Aspect_TOL_SOLID, 1.0);
    if (const Prs3d_LineAspect* aLinked = !aLink.IsNull() ? aLink->VectorAspect().get() : NULL)
    {
      *myVectorAspect->Aspect() = *aLinked->Aspect();
    }
  }
  if (mySectionAspect.IsNull())
  {
    isUpdateNeeded  = true;
    mySectionAspect = new Prs3d_LineAspect(THE_DEF_COLOR_Section, Aspect_TOL_SOLID, 1.0);
    if (const Prs3d_LineAspect* aLinked = !aLink.IsNull() ? aLink->SectionAspect().get() : NULL)
    {
      *mySectionAspect->Aspect() = *aLinked->Aspect();
    }
  }
  if (myPlaneAspect.IsNull())
  {
    isUpdateNeeded = true;
    myPlaneAspect  = new Prs3d_PlaneAspect();
  }
  if (myArrowAspect.IsNull())
  {
    isUpdateNeeded = true;
    myArrowAspect  = new Prs3d_ArrowAspect();
  }
  if (myDatumAspect.IsNull())
  {
    isUpdateNeeded = true;
    myDatumAspect  = new Prs3d_DatumAspect();
    if (const Prs3d_DatumAspect* aLinked = !aLink.IsNull() ? aLink->DatumAspect().get() : NULL)
    {
      myDatumAspect->CopyAspectsFrom(aLinked);
    }
  }
  return isUpdateNeeded;
}

//! Assign the shader program.
template <typename T>
inline void setAspectProgram(const Handle(Graphic3d_ShaderProgram)& theProgram, T thePrsAspect)
{
  if (!thePrsAspect.IsNull())
  {
    thePrsAspect->Aspect()->SetShaderProgram(theProgram);
  }
}

// =======================================================================
// function : SetShaderProgram
// purpose  :
// =======================================================================
bool Prs3d_Drawer::SetShaderProgram(const Handle(Graphic3d_ShaderProgram)& theProgram,
                                    const Graphic3d_GroupAspect            theAspect,
                                    const bool                             theToOverrideDefaults)
{
  bool isUpdateNeeded = false;
  switch (theAspect)
  {
    case Graphic3d_ASPECT_LINE: {
      if (theToOverrideDefaults)
      {
        isUpdateNeeded = SetOwnLineAspects() || isUpdateNeeded;
        isUpdateNeeded = SetOwnDatumAspects() || isUpdateNeeded;
      }

      setAspectProgram(theProgram, myUIsoAspect);
      setAspectProgram(theProgram, myVIsoAspect);
      setAspectProgram(theProgram, myWireAspect);
      setAspectProgram(theProgram, myLineAspect);
      setAspectProgram(theProgram, mySeenLineAspect);
      setAspectProgram(theProgram, myHiddenLineAspect);
      setAspectProgram(theProgram, myVectorAspect);
      setAspectProgram(theProgram, mySectionAspect);
      setAspectProgram(theProgram, myFreeBoundaryAspect);
      setAspectProgram(theProgram, myUnFreeBoundaryAspect);
      setAspectProgram(theProgram, myFaceBoundaryAspect);
      if (!myPlaneAspect.IsNull())
      {
        setAspectProgram(theProgram, myPlaneAspect->EdgesAspect());
        setAspectProgram(theProgram, myPlaneAspect->IsoAspect());
        setAspectProgram(theProgram, myPlaneAspect->ArrowAspect());
      }
      if (!myDatumAspect.IsNull())
      {
        setAspectProgram(theProgram, myDatumAspect->LineAspect(Prs3d_DatumParts_XAxis));
        setAspectProgram(theProgram, myDatumAspect->LineAspect(Prs3d_DatumParts_YAxis));
        setAspectProgram(theProgram, myDatumAspect->LineAspect(Prs3d_DatumParts_ZAxis));
      }
      setAspectProgram(theProgram, myArrowAspect);
      return isUpdateNeeded;
    }
    case Graphic3d_ASPECT_TEXT: {
      if (theToOverrideDefaults && myTextAspect.IsNull())
      {
        isUpdateNeeded = true;
        myTextAspect   = new Prs3d_TextAspect();
        if (const Prs3d_TextAspect* aLinked = !myLink.IsNull() ? myLink->TextAspect().get() : NULL)
        {
          *myTextAspect->Aspect() = *aLinked->Aspect();
        }
      }

      setAspectProgram(theProgram, myTextAspect);
      return isUpdateNeeded;
    }
    case Graphic3d_ASPECT_MARKER: {
      if (theToOverrideDefaults && SetupOwnPointAspect())
      {
        isUpdateNeeded = true;
      }

      setAspectProgram(theProgram, myPointAspect);
      return isUpdateNeeded;
    }
    case Graphic3d_ASPECT_FILL_AREA: {
      if (theToOverrideDefaults && SetupOwnShadingAspect())
      {
        isUpdateNeeded = true;
      }
      setAspectProgram(theProgram, myShadingAspect);
      return isUpdateNeeded;
    }
  }
  return false;
}

// =======================================================================
// function : SetShadingModel
// purpose  :
// =======================================================================
bool Prs3d_Drawer::SetShadingModel(Graphic3d_TypeOfShadingModel theModel,
                                   bool                         theToOverrideDefaults)
{
  bool isUpdateNeeded = false;

  if (theToOverrideDefaults && SetupOwnShadingAspect())
  {
    isUpdateNeeded = true;
  }

  if (!myShadingAspect.IsNull())
  {
    myShadingAspect->Aspect()->SetShadingModel(theModel);
  }

  return isUpdateNeeded;
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void Prs3d_Drawer::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myLink.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMaximalParameterValue)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myChordialDeviation)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myTypeOfDeflection)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasOwnTypeOfDeflection)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myTypeOfHLR)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDeviationCoefficient)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPreviousDeviationCoefficient)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDeviationAngle)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPreviousDeviationAngle)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsoOnPlane)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasOwnIsoOnPlane)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsoOnTriangulation)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasOwnIsoOnTriangulation)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsAutoTriangulated)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasOwnIsAutoTriangulated)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myWireDraw)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasOwnWireDraw)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myShadingAspect.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myTextAspect.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myLineArrowDraw)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasOwnLineArrowDraw)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDrawHiddenLine)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasOwnDrawHiddenLine)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVertexDrawMode)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myFreeBoundaryDraw)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasOwnFreeBoundaryDraw)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myUnFreeBoundaryDraw)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasOwnUnFreeBoundaryDraw)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myFaceBoundaryUpperContinuity)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myFaceBoundaryDraw)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasOwnFaceBoundaryDraw)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasOwnDimLengthModelUnits)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasOwnDimAngleModelUnits)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasOwnDimLengthDisplayUnits)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHasOwnDimAngleDisplayUnits)
}
