#include <AIS_Drawer.ixx>
#include <Standard_ProgramError.hxx>

AIS_Drawer::AIS_Drawer(): 
myLink(new Prs3d_Drawer()),
hasLocalAttributes(Standard_False),
myhasOwnDeviationCoefficient(Standard_False),
myPreviousDeviationCoefficient(0.1),
myhasOwnHLRDeviationCoefficient (Standard_False),
myhasOwnDeviationAngle (Standard_False),
myhasOwnHLRDeviationAngle (Standard_False)
{
  SetMaximalParameterValue(500000.);
  myLink->SetMaximalParameterValue(500000.);
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

Handle (Prs3d_LengthAspect) AIS_Drawer::LengthAspect ()  
{return myLengthAspect.IsNull()? myLink->LengthAspect () : myLengthAspect ;}


Handle (Prs3d_AngleAspect) AIS_Drawer::AngleAspect () 
{return myAngleAspect.IsNull()? myLink->AngleAspect () :myAngleAspect ;}


Handle (Prs3d_RadiusAspect) AIS_Drawer::RadiusAspect ()  const 
{return myRadiusAspect.IsNull()? myLink->RadiusAspect():myRadiusAspect; }


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
  if(!myLengthAspect.IsNull())  myLengthAspect.Nullify();    
  if(!myAngleAspect.IsNull())  myAngleAspect.Nullify();     
  if(!myRadiusAspect.IsNull())  myRadiusAspect.Nullify();    
  if(!mySectionAspect.IsNull())  mySectionAspect.Nullify();   
  if( myhasOwnHLRDeviationCoefficient )  myhasOwnHLRDeviationCoefficient = Standard_False;   
  if(myhasOwnHLRDeviationAngle ) myhasOwnHLRDeviationAngle  = Standard_False;   
  
  hasLocalAttributes = Standard_False;

}


