// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <V3d_Viewer.jxx>
#include <Visual3d_Light.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_PositionalLight.hxx>
#include <V3d_AmbientLight.hxx>
#include <Viewer_BadValue.hxx>
#include <V3d.hxx>				       
void V3d_Viewer::UpdateLights() {

  for (InitActiveViews();MoreActiveViews();NextActiveViews()){
    ActiveView()->UpdateLights();
  }
}

void V3d_Viewer::SetLightOn( const Handle(V3d_Light)& TheLight ) {


  if(!MyActiveLights.Contains(TheLight)) {
//    Viewer_BadValue_Raise_if( MyActiveLights.Extent() >= Visual3d_Light::Limit(),
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
//      Viewer_BadValue_Raise_if( MyActiveLights.Extent() >= Visual3d_Light::Limit(),
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
  MyCurrentSelectedLight = NULL;
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

void V3d_Viewer::SetDefaultLights () {
//  SetLightOn ( new V3d_DirectionalLight(this,V3d_XposYpos));
//  SetLightOn ( new V3d_DirectionalLight(this,V3d_Zpos));
//  SetLightOn ( new V3d_DirectionalLight(this,V3d_XnegYneg));
//  SetLightOn ( new V3d_DirectionalLight(this,V3d_Zneg));
  SetLightOn( new V3d_DirectionalLight(this, V3d_Zneg, Quantity_NOC_WHITE, Standard_True) );
  SetLightOn ( new V3d_AmbientLight(this));
}
void V3d_Viewer::Init() {
  MyDefaultPerspectiveView = new V3d_PerspectiveView(this);
  MyDefaultOrthographicView = new V3d_OrthographicView(this);
  SetDefaultLights();
}
