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


#include <Aspect_Background.hxx>
#include <Aspect_GradientBackground.hxx>
#include <Aspect_Grid.hxx>
#include <gp_Ax3.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Structure.hxx>
#include <Quantity_Color.hxx>
#include <V3d.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_BadValue.hxx>
#include <V3d_CircularGrid.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_Light.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_RectangularGrid.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

void V3d_Viewer::UpdateLights() {

  for (InitActiveViews();MoreActiveViews();NextActiveViews()){
    ActiveView()->UpdateLights();
  }
}

void V3d_Viewer::SetLightOn( const Handle(V3d_Light)& TheLight ) {


  if(!MyActiveLights.Contains(TheLight)) {
//    V3d_BadValue_Raise_if( MyActiveLights.Extent() >= MyDriver->InquireLightLimit(),
//				 "too many lights");
      MyActiveLights.Append(TheLight) ;
  }
  
  for (InitActiveViews();MoreActiveViews();NextActiveViews()){
    ActiveView()->SetLightOn(TheLight);}
}

void V3d_Viewer::SetLightOff( const Handle(V3d_Light)& TheLight ) {

  MyActiveLights.Remove(TheLight);
  for (InitActiveViews();MoreActiveViews();NextActiveViews()) {
    ActiveView()->SetLightOff(TheLight);}
}


void V3d_Viewer::SetLightOn() {

  for (InitDefinedLights();MoreDefinedLights();NextDefinedLights()) {
    if(!MyActiveLights.Contains(DefinedLight())) {
//      V3d_BadValue_Raise_if( MyActiveLights.Extent() >= MyDriver->InquireLightLimit(),
//				   "too many lights");
      MyActiveLights.Append(DefinedLight());
      for (InitActiveViews();MoreActiveViews();NextActiveViews()) {
	ActiveView()->SetLightOn(DefinedLight());}
    }
  }
}

void V3d_Viewer::SetLightOff() {

  for (InitActiveLights();MoreActiveLights();NextActiveLights()) {
    for(InitActiveViews();MoreActiveViews();NextActiveViews()){
      ActiveView()->SetLightOff(ActiveLight());}
  }
  MyActiveLights.Clear();
}

void V3d_Viewer::SetCurrentSelectedLight(const Handle(V3d_Light)& TheLight) {
  MyCurrentSelectedLight = TheLight;
}

void V3d_Viewer::ClearCurrentSelectedLight() {
  MyCurrentSelectedLight.Nullify();
}


Handle(V3d_Light) V3d_Viewer::CurrentSelectedLight() const {

  return MyCurrentSelectedLight;
}

Standard_Boolean V3d_Viewer::IsGlobalLight(const Handle(V3d_Light)& TheLight)
 const {

   return MyActiveLights.Contains(TheLight);
}


void V3d_Viewer::AddLight( const Handle(V3d_Light)& TheLight )  {

  MyDefinedLights.Append(TheLight);
}
  
void V3d_Viewer::DelLight( const Handle(V3d_Light)& TheLight ) {
    
  SetLightOff(TheLight);   
  MyDefinedLights.Remove(TheLight);
}

void V3d_Viewer::SetDefaultLights()
{
  while (MyDefinedLights.Extent() > 0)
  {
    DelLight (Handle(V3d_Light)::DownCast (MyDefinedLights.First()));
  }

  SetLightOn (new V3d_DirectionalLight (this, V3d_Zneg, Quantity_NOC_WHITE, Standard_True));
  SetLightOn (new V3d_AmbientLight (this));
}
