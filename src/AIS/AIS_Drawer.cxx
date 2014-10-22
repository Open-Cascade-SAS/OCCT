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

#include <AIS_Drawer.ixx>
#include <Standard_ProgramError.hxx>

// =======================================================================
// function : AIS_Drawer
// purpose  :
// =======================================================================
AIS_Drawer::AIS_Drawer()
: myLink (new Prs3d_Drawer()),
  hasLocalAttributes (Standard_False),
  myhasOwnDeviationCoefficient (Standard_False),
  myPreviousDeviationCoefficient (0.1),
  myhasOwnHLRDeviationCoefficient (Standard_False),
  myhasOwnDeviationAngle (Standard_False),
  myhasOwnHLRDeviationAngle (Standard_False),
  myHasOwnFaceBoundaryDraw (Standard_False),
  myHasOwnDimLengthModelUnits (Standard_False),
  myHasOwnDimLengthDisplayUnits (Standard_False),
  myHasOwnDimAngleModelUnits (Standard_False),
  myHasOwnDimAngleDisplayUnits (Standard_False)
{
  SetMaximalParameterValue (500000.0);
  myLink->SetMaximalParameterValue (500000.0);
  SetTypeOfHLR (Prs3d_TOH_NotSet);
  SetVertexDrawMode (Prs3d_VDM_Inherited);
}

Aspect_TypeOfDeflection AIS_Drawer::TypeOfDeflection () const 
{
  return myLink->TypeOfDeflection();
}

Standard_Boolean AIS_Drawer::IsoOnPlane() const 
{
    return myLink->IsoOnPlane();
}

Standard_Integer AIS_Drawer::Discretisation() const 
{
  return myLink->Discretisation();
}

Quantity_Length AIS_Drawer::MaximalChordialDeviation() const 
{
  return myLink->MaximalChordialDeviation();
}


//=======================================================================
//function : SetDeviationCoefficient
//purpose  : 
//=======================================================================

void AIS_Drawer::SetDeviationCoefficient ( const Standard_Real  aCoefficient )
{
  myPreviousDeviationCoefficient = DeviationCoefficient() ;
  myOwnDeviationCoefficient     = aCoefficient ;
  myhasOwnDeviationCoefficient    = Standard_True; 
}


//=======================================================================
//function : SetHLRDeviationCoefficient
//purpose  : 
//=======================================================================

void AIS_Drawer::SetHLRDeviationCoefficient ( const Standard_Real  aCoefficient ) 
{
  myPreviousHLRDeviationCoefficient = HLRDeviationCoefficient() ;
  myOwnHLRDeviationCoefficient     = aCoefficient ;
  myhasOwnHLRDeviationCoefficient    = Standard_True; 
}

//=======================================================================
//function : SetDeviationAngle
//purpose  : 
//=======================================================================

void AIS_Drawer::SetDeviationAngle ( const Standard_Real anAngle ) 
{
  myPreviousDeviationAngle = DeviationAngle() ;
  myOwnDeviationAngle     = anAngle ;
  myhasOwnDeviationAngle    = Standard_True;   
}

//=======================================================================
//function : SetHLRAngle
//purpose  : 
//=======================================================================

void AIS_Drawer::SetHLRAngle ( const Standard_Real anAngle ) 
{
  myPreviousHLRDeviationAngle = HLRAngle() ;
  myOwnHLRDeviationAngle     = anAngle ;
  myhasOwnHLRDeviationAngle    = Standard_True;   
}

/////// Get
//=======================================================================
//function : DeviationCoefficient
//purpose  : 
//=======================================================================

Standard_Real AIS_Drawer::DeviationCoefficient ()  const 
{
  return myhasOwnDeviationCoefficient? myOwnDeviationCoefficient :myLink->DeviationCoefficient();
}

//=======================================================================
//function : HLRDeviationCoefficient
//purpose  : 
//=======================================================================

Standard_Real AIS_Drawer::HLRDeviationCoefficient ()  const 
{
  return myhasOwnHLRDeviationCoefficient?myOwnHLRDeviationCoefficient : myLink->HLRDeviationCoefficient() ;
}

//=======================================================================
//function : DeviationAngle
//purpose  : 
//=======================================================================

Standard_Real AIS_Drawer::DeviationAngle ()  const 
{
  return myhasOwnDeviationAngle ? myOwnDeviationAngle :  myLink->DeviationAngle();
}

//=======================================================================
//function : HLRAngle
//purpose  : 
//=======================================================================

Standard_Real AIS_Drawer::HLRAngle ()  const 
{
  
  return myhasOwnHLRDeviationAngle ?  myOwnHLRDeviationAngle : myLink->HLRAngle();
}

Standard_Real AIS_Drawer::MaximalParameterValue () const 
{ return myLink->MaximalParameterValue();}

Handle (Prs3d_IsoAspect) AIS_Drawer::UIsoAspect ()  
{ return myUIsoAspect.IsNull() ? myLink->UIsoAspect (): myUIsoAspect;}

Handle (Prs3d_IsoAspect) AIS_Drawer::VIsoAspect () 
{ return myVIsoAspect.IsNull() ? myLink->VIsoAspect (): myVIsoAspect;}

Handle (Prs3d_LineAspect) AIS_Drawer::FreeBoundaryAspect () 
{return myFreeBoundaryAspect.IsNull() ? myLink->FreeBoundaryAspect (): myFreeBoundaryAspect;}

Standard_Boolean AIS_Drawer::FreeBoundaryDraw () const 
{return myLink->FreeBoundaryDraw();}

Handle (Prs3d_LineAspect) AIS_Drawer::UnFreeBoundaryAspect ()  
{return myUnFreeBoundaryAspect.IsNull() ? myLink->UnFreeBoundaryAspect (): myUnFreeBoundaryAspect;}

Standard_Boolean AIS_Drawer::UnFreeBoundaryDraw ()  const  
{return myLink->UnFreeBoundaryDraw ();}

Handle (Prs3d_LineAspect) AIS_Drawer::WireAspect ()
{  return myWireAspect.IsNull()? myLink->WireAspect (): myWireAspect;}

Standard_Boolean AIS_Drawer::WireDraw ()  const 
{ return myLink->WireDraw();}

Handle (Prs3d_LineAspect) AIS_Drawer::LineAspect ()  
{ return myLineAspect.IsNull() ? myLink->LineAspect (): myLineAspect;}

Handle (Prs3d_TextAspect) AIS_Drawer::TextAspect ()  
{ return myTextAspect.IsNull() ? myLink->TextAspect () : myTextAspect ;}

Handle (Prs3d_ShadingAspect) AIS_Drawer::ShadingAspect ()
{
return myShadingAspect.IsNull() ? myLink->ShadingAspect ()  : myShadingAspect ;
}

Standard_Boolean AIS_Drawer::ShadingAspectGlobal()  const 
{ return myLink->ShadingAspectGlobal();}
 

Standard_Boolean AIS_Drawer::LineArrowDraw ()  const 
{ return myLink->LineArrowDraw();}

Handle (Prs3d_ArrowAspect) AIS_Drawer::ArrowAspect() 
{return myArrowAspect.IsNull()?  myLink->ArrowAspect () :  myArrowAspect;}

Handle (Prs3d_PointAspect) AIS_Drawer::PointAspect() 
{return myPointAspect.IsNull()?  myLink->PointAspect () :  myPointAspect;}

void AIS_Drawer::SetVertexDrawMode (const Prs3d_VertexDrawMode theMode)
{
  // Assuming that myLink always exists --> Prs3d_VDM_Inherited value is acceptable.
  // So we simply store the new mode, as opposed to Prs3d_Drawer::SetVertexDrawMode()
  myVertexDrawMode = theMode;
}

Prs3d_VertexDrawMode AIS_Drawer::VertexDrawMode () const
{
  return IsOwnVertexDrawMode() ? myVertexDrawMode : myLink->VertexDrawMode();
}

Standard_Boolean AIS_Drawer::DrawHiddenLine ()  const 
{return myLink->DrawHiddenLine();}

Handle (Prs3d_LineAspect) AIS_Drawer::HiddenLineAspect ()  
{return  myHiddenLineAspect.IsNull()?  myLink->HiddenLineAspect () : myHiddenLineAspect;}

Handle (Prs3d_LineAspect) AIS_Drawer::SeenLineAspect ()  
{ return mySeenLineAspect.IsNull() ? myLink->SeenLineAspect (): mySeenLineAspect ;}

Handle (Prs3d_LineAspect) AIS_Drawer::VectorAspect ()  
{return myVectorAspect.IsNull()?  myLink->VectorAspect () : myVectorAspect;}

Handle (Prs3d_DatumAspect) AIS_Drawer::DatumAspect () 
{return myDatumAspect.IsNull()?  myLink->DatumAspect () : myDatumAspect; }

Handle (Prs3d_PlaneAspect) AIS_Drawer::PlaneAspect () 
{return myPlaneAspect.IsNull() ? myLink->PlaneAspect (): myPlaneAspect;}

// =======================================================================
// function : DimensionAspect
// purpose  :
// =======================================================================
Handle (Prs3d_DimensionAspect) AIS_Drawer::DimensionAspect()
{
  return myDimensionAspect.IsNull()? myLink->DimensionAspect () : myDimensionAspect;
}

// =======================================================================
// function : DimAngleModelUnits
// purpose  :
// =======================================================================
const TCollection_AsciiString& AIS_Drawer::DimAngleModelUnits() const
{
  return myHasOwnDimAngleModelUnits
    ? Prs3d_Drawer::DimAngleModelUnits()
    : myLink->DimAngleModelUnits();
}

// =======================================================================
// function : DimensionModelUnits
// purpose  :
// =======================================================================
const TCollection_AsciiString& AIS_Drawer::DimLengthModelUnits() const
{
  return myHasOwnDimLengthModelUnits
    ? Prs3d_Drawer::DimLengthModelUnits()
    : myLink->DimLengthModelUnits();
}
// =======================================================================
// function : SetDimLengthModelUnits
// purpose  :
// =======================================================================
void AIS_Drawer::SetDimLengthModelUnits (const TCollection_AsciiString& theUnits)
{
  myHasOwnDimLengthModelUnits = Standard_True;
  Prs3d_Drawer::SetDimLengthModelUnits (theUnits);
}

// =======================================================================
// function : SetDimAngleModelUnits
// purpose  :
// =======================================================================
void AIS_Drawer::SetDimAngleModelUnits (const TCollection_AsciiString& theUnits)
{
  myHasOwnDimAngleModelUnits = Standard_True;
  Prs3d_Drawer::SetDimAngleModelUnits (theUnits);
}

// =======================================================================
// function : DimAngleDisplayUnits
// purpose  :
// =======================================================================
const TCollection_AsciiString& AIS_Drawer::DimAngleDisplayUnits() const
{
  return myHasOwnDimAngleDisplayUnits
    ? Prs3d_Drawer::DimAngleDisplayUnits()
    : myLink->DimAngleDisplayUnits();
}

// =======================================================================
// function : DimLengthDisplayUnits
// purpose  :
// =======================================================================
const TCollection_AsciiString& AIS_Drawer::DimLengthDisplayUnits() const
{
  return myHasOwnDimLengthDisplayUnits
    ? Prs3d_Drawer::DimLengthDisplayUnits()
    : myLink->DimLengthDisplayUnits();
}

// =======================================================================
// function : SetDimLengthDisplayUnits
// purpose  :
// =======================================================================
void AIS_Drawer::SetDimLengthDisplayUnits (const TCollection_AsciiString& theUnits)
{
  myHasOwnDimLengthDisplayUnits = Standard_True;
  Prs3d_Drawer::SetDimLengthDisplayUnits (theUnits);
}

// =======================================================================
// function : SetDimAngleDisplayUnits
// purpose  :
// =======================================================================
void AIS_Drawer::SetDimAngleDisplayUnits (const TCollection_AsciiString& theUnits)
{
  myHasOwnDimAngleDisplayUnits = Standard_True;
  Prs3d_Drawer::SetDimAngleDisplayUnits (theUnits);
}

Handle (Prs3d_LineAspect) AIS_Drawer::SectionAspect ()
{return mySectionAspect.IsNull()? myLink->SectionAspect (): mySectionAspect;}

const Handle (Prs3d_Drawer)& AIS_Drawer::Link()  
{ 
  if(myLink.IsNull()) myLink =  new Prs3d_Drawer();
  return myLink;
 }

void AIS_Drawer::ClearLocalAttributes()
{
  if(myLink.IsNull()) return;
// attention pas beau....
// The Handles

  if(!myUIsoAspect.IsNull()) myUIsoAspect.Nullify();
  if(!myVIsoAspect.IsNull()) myVIsoAspect.Nullify();
  if(!myFreeBoundaryAspect.IsNull())  myFreeBoundaryAspect.Nullify();  
  if(!myUnFreeBoundaryAspect.IsNull())  myUnFreeBoundaryAspect.Nullify();
  if(!myWireAspect.IsNull())  myWireAspect.Nullify();          
  if(!myLineAspect.IsNull())  myLineAspect.Nullify();          
  if(!myTextAspect.IsNull())  myTextAspect.Nullify();   
  if(!myShadingAspect.IsNull())  myShadingAspect.Nullify();
  if(!myPointAspect.IsNull())  myPointAspect.Nullify();  
  if(!myPlaneAspect.IsNull())  myPlaneAspect.Nullify();  
  if(!myArrowAspect.IsNull())  myArrowAspect.Nullify();  
  if(!myHiddenLineAspect.IsNull())  myHiddenLineAspect.Nullify();
  if(!mySeenLineAspect.IsNull())  mySeenLineAspect.Nullify();  
  if(!myVectorAspect.IsNull())  myVectorAspect .Nullify();   
  if(!myDatumAspect.IsNull())  myDatumAspect.Nullify();     
  if(!myDimensionAspect.IsNull())  myDimensionAspect.Nullify();    
  if(!mySectionAspect.IsNull())  mySectionAspect.Nullify();   
  if( myhasOwnHLRDeviationCoefficient )  myhasOwnHLRDeviationCoefficient = Standard_False;   
  if(myhasOwnHLRDeviationAngle ) myhasOwnHLRDeviationAngle  = Standard_False;
  if (!myFaceBoundaryAspect.IsNull()) myFaceBoundaryAspect.Nullify();
  
  myHasOwnFaceBoundaryDraw = Standard_False;
  myHasOwnDimLengthModelUnits = Standard_False;
  myHasOwnDimLengthDisplayUnits = Standard_False;
  myHasOwnDimAngleModelUnits = Standard_False;
  myHasOwnDimAngleDisplayUnits = Standard_False;

  hasLocalAttributes = Standard_False;

}

// =======================================================================
// function : SetFaceBoundaryDraw
// purpose  :
// =======================================================================
void AIS_Drawer::SetFaceBoundaryDraw (const Standard_Boolean theIsEnabled)
{
  myHasOwnFaceBoundaryDraw = Standard_True;
  myFaceBoundaryDraw       = theIsEnabled;
}

// =======================================================================
// function : IsFaceBoundaryDraw
// purpose  :
// =======================================================================
Standard_Boolean AIS_Drawer::IsFaceBoundaryDraw() const
{
  if (!IsOwnFaceBoundaryDraw ())
  {
    return myLink->IsFaceBoundaryDraw ();
  }

  return myFaceBoundaryDraw;
}

// =======================================================================
// function : SetFaceBoundaryAspect
// purpose  :
// =======================================================================
void AIS_Drawer::SetFaceBoundaryAspect (const Handle(Prs3d_LineAspect)& theAspect)
{
  myFaceBoundaryAspect = theAspect;
}

// =======================================================================
// function : FaceBoundaryAspect
// purpose  :
// =======================================================================
Handle(Prs3d_LineAspect) AIS_Drawer::FaceBoundaryAspect()
{
  if (!IsOwnFaceBoundaryAspect ())
  {
    return myLink->FaceBoundaryAspect ();
  }

  return myFaceBoundaryAspect;
}
