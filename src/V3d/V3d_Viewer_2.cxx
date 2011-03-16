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
