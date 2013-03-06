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

/***********************************************************************
 
     FONCTION :
     ----------
        Classe V3d_View_2.cxx :
 
     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      00-09-92 : GG  ; Creation.
      24-12-97 : FMN ; Suppression de GEOMLITE
      21-02-00 : GG  ; Add Transparency() method
      23-11-00 : GG  ; Add IsActiveLight() and IsActivePlane() methods

************************************************************************/

#define GER61454	//GG 13-09-99 Activates model clipping planes
//		Use myView->PlaneLimit() instead Visual3d_ClipPlane::Limit()
//		Use myView->LightLimit() instead Visual3d_Light::Limit()



/*----------------------------------------------------------------------*/
/*
 * Includes
 */

#include <V3d_View.jxx>
#include <Visual3d_View.hxx>
#include <Visual3d_Light.hxx>
#include <Visual3d_ClipPlane.hxx>
#include <V3d_Static.hxx>
#include <V3d.hxx>

/*----------------------------------------------------------------------*/

void V3d_View::SetLightOn( const Handle(V3d_Light)& TheLight ) {

  if( !MyActiveLights.Contains(TheLight)){
#ifdef GER61454
    V3d_BadValue_Raise_if( MyActiveLights.Extent() >= MyView->LightLimit(),
			  "too many lights");
#else
//    V3d_BadValue_Raise_if( MyActiveLights.Extent() >= Visual3d_Light::Limit(),
//			  "too many lights");
#endif
    MyActiveLights.Append(TheLight) ;
    MyViewContext.SetLightOn(TheLight->Light());
    MyView->SetContext(MyViewContext);
  }
}

void V3d_View::SetLightOff( const Handle(V3d_Light)& TheLight ) {

  Standard_TypeMismatch_Raise_if(MyViewer->IsGlobalLight(TheLight),"the light is global");
  
  MyActiveLights.Remove(TheLight);
  MyViewContext.SetLightOff(TheLight->Light()) ;
  MyView->SetContext(MyViewContext) ;
}

Standard_Boolean V3d_View::IsActiveLight(const Handle(V3d_Light)& aLight) const {
  if( aLight.IsNull() ) return Standard_False;
  return  MyActiveLights.Contains(aLight);
}

void V3d_View::SetLightOn( ) {

  for(MyViewer->InitDefinedLights();MyViewer->MoreDefinedLights();MyViewer->NextDefinedLights()){
    if(!MyActiveLights.Contains(MyViewer->DefinedLight())){
      MyActiveLights.Append(MyViewer->DefinedLight()) ;
      MyViewContext.SetLightOn(MyViewer->DefinedLight()->Light());
    }
  }
  MyView->SetContext(MyViewContext) ;
  
}

void V3d_View::SetLightOff( ) {
  
  InitActiveLights();
  while(MoreActiveLights()) {
    if (!MyViewer->IsGlobalLight(ActiveLight())) {
      MyActiveLights.Remove(ActiveLight());
      MyViewContext.SetLightOff(ActiveLight()->Light());
    }
    else
      NextActiveLights();
  }
  MyView->SetContext(MyViewContext) ;

}

void V3d_View::SetTransparency(const Standard_Boolean AnActivity) {

  MyTransparencyFlag = AnActivity;
  MyView->SetTransparency(AnActivity);
}


Standard_Boolean V3d_View::Transparency() const {

  return MyTransparencyFlag;
}

void V3d_View::SetPlaneOn( const Handle(V3d_Plane)& ThePlane ) {

#ifdef GER61454
  if( !MyActivePlanes.Contains(ThePlane)) {
    V3d_BadValue_Raise_if( MyActivePlanes.Extent() >= MyView->PlaneLimit(), "too many planes");
    MyActivePlanes.Append(ThePlane) ;
  }
  MyViewContext.SetClipPlaneOn(ThePlane->Plane()) ;
  MyView->SetContext(MyViewContext);
#else 	//GER61454
  if( !MyActivePlanes.Contains(ThePlane)) {
    V3d_BadValue_Raise_if( MyActivePlanes.Extent() >= Visual3d_ClipPlane::Limit(), "too many planes");
    MyActivePlanes.Append(ThePlane) ;
    MyViewContext.SetClipPlaneOn(ThePlane->Plane()) ;
    
    MyView->SetContext(MyViewContext);
  }
#endif	//GER61454
}

void V3d_View::SetPlaneOff( const Handle(V3d_Plane)& ThePlane ) {

  MyActivePlanes.Remove(ThePlane);
  MyViewContext.SetClipPlaneOff(ThePlane->Plane()) ;
  MyView->SetContext(MyViewContext) ;
}

Standard_Boolean V3d_View::IsActivePlane(const Handle(V3d_Plane)& aPlane) const {
  if( aPlane.IsNull() ) return Standard_False;
  return  MyActivePlanes.Contains(aPlane);
}

void V3d_View::SetPlaneOn( ) {

#ifdef GER61454
  for(MyViewer->InitDefinedPlanes();MyViewer->MoreDefinedPlanes();MyViewer->NextDefinedPlanes()) {
    if(!MyActivePlanes.Contains(MyViewer->DefinedPlane())) {
      V3d_BadValue_Raise_if( MyActivePlanes.Extent() >= MyView->PlaneLimit(), "too many planes");
      MyActivePlanes.Append(MyViewer->DefinedPlane());
    }
    MyViewContext.SetClipPlaneOn(MyViewer->DefinedPlane()->Plane());
  }
  MyView->SetContext(MyViewContext) ;
#else	//GER61454
  for(MyViewer->InitDefinedPlanes();MyViewer->MoreDefinedPlanes();MyViewer->NextDefinedPlanes()) {
    if(!MyActivePlanes.Contains(MyViewer->DefinedPlane())) {
      V3d_BadValue_Raise_if( MyActivePlanes.Extent() >=  Visual3d_ClipPlane::Limit(),
			    "too many planes");
      MyActivePlanes.Append(MyViewer->DefinedPlane());
      MyViewContext.SetClipPlaneOn(MyViewer->DefinedPlane()->Plane());
      MyView->SetContext(MyViewContext) ;
    }
  }
#endif	//GER61454
}

void V3d_View::SetPlaneOff( ) {

  for(InitActivePlanes();MoreActivePlanes();NextActivePlanes()) {
    MyViewContext.SetClipPlaneOff(ActivePlane()->Plane());}
  MyActivePlanes.Clear();
  MyView->SetContext(MyViewContext) ;
}

void V3d_View::InitActiveLights() {
myActiveLightsIterator.Initialize(MyActiveLights);
}
Standard_Boolean V3d_View::MoreActiveLights () const {
  return myActiveLightsIterator.More();
}
void V3d_View::NextActiveLights () {
  myActiveLightsIterator.Next();
}
Handle(V3d_Light) V3d_View::ActiveLight() const {
  return (Handle(V3d_Light)&)(myActiveLightsIterator.Value());}


void V3d_View::InitActivePlanes() {
myActivePlanesIterator.Initialize(MyActivePlanes);
}
Standard_Boolean V3d_View::MoreActivePlanes () const {
  return myActivePlanesIterator.More();
}
void V3d_View::NextActivePlanes () {
  myActivePlanesIterator.Next();
}

Handle(V3d_Plane) V3d_View::ActivePlane() const {
  return (Handle(V3d_Plane)&)(myActivePlanesIterator.Value());}

Standard_Boolean V3d_View::IfMorePlanes() const {

#ifdef GER61454
        return MyActivePlanes.Extent() < MyView->PlaneLimit();
#else
  	return MyActivePlanes.Extent() < Visual3d_ClipPlane::Limit();
#endif

}

Standard_Boolean V3d_View::IfMoreLights() const {

#ifdef GER61454
        return MyActiveLights.Extent() < MyView->LightLimit();
#else
//	return MyActiveLights.Extent() < Visual3d_Light::Limit();
	return MyActiveLights.Extent();
#endif
}
