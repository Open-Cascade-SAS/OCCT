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
#include <V3d.hxx>
void V3d_Viewer::InitActiveViews() {
myActiveViewsIterator.Initialize(MyActiveViews);
}
Standard_Boolean V3d_Viewer::MoreActiveViews () const {
  return myActiveViewsIterator.More();
}
void V3d_Viewer::NextActiveViews () {
  if(!MyActiveViews.IsEmpty())myActiveViewsIterator.Next();
}
Handle(V3d_View) V3d_Viewer::ActiveView() const {
  return (Handle(V3d_View)&)(myActiveViewsIterator.Value());}

Standard_Boolean V3d_Viewer::LastActiveView() const {
  return MyActiveViews.Extent() == 1;}

Standard_Boolean V3d_Viewer::IsActive(const Handle(V3d_View)& aView) const {
  return MyActiveViews.Contains(aView);
}
void V3d_Viewer::InitDefinedViews() {
myDefinedViewsIterator.Initialize(MyDefinedViews);
}
Standard_Boolean V3d_Viewer::MoreDefinedViews () const {
  return myDefinedViewsIterator.More();
}
void V3d_Viewer::NextDefinedViews () {
  if(!MyDefinedViews.IsEmpty())myDefinedViewsIterator.Next();
}
Handle(V3d_View) V3d_Viewer::DefinedView() const {
  return (Handle(V3d_View)&)(myDefinedViewsIterator.Value());}

void V3d_Viewer::InitActiveLights() {
myActiveLightsIterator.Initialize(MyActiveLights);
}
Standard_Boolean V3d_Viewer::MoreActiveLights () const {
  return myActiveLightsIterator.More();
}
void V3d_Viewer::NextActiveLights () {
  myActiveLightsIterator.Next();
}
Handle(V3d_Light) V3d_Viewer::ActiveLight() const {
  return (Handle(V3d_Light)&)(myActiveLightsIterator.Value());}

void V3d_Viewer::InitDefinedLights() {
myDefinedLightsIterator.Initialize(MyDefinedLights);
}
Standard_Boolean V3d_Viewer::MoreDefinedLights () const {
  return myDefinedLightsIterator.More();
}
void V3d_Viewer::NextDefinedLights () {
  if(!MyDefinedLights.IsEmpty())myDefinedLightsIterator.Next();
}
Handle(V3d_Light) V3d_Viewer::DefinedLight() const {
  return (Handle(V3d_Light)&)(myDefinedLightsIterator.Value());}

void V3d_Viewer::InitDefinedPlanes() {
myDefinedPlanesIterator.Initialize(MyDefinedPlanes);
}
Standard_Boolean V3d_Viewer::MoreDefinedPlanes () const {
  return myDefinedPlanesIterator.More();
}
void V3d_Viewer::NextDefinedPlanes () {
  myDefinedPlanesIterator.Next();
}
Handle(V3d_Plane) V3d_Viewer::DefinedPlane() const {
  return (Handle(V3d_Plane)&)(myDefinedPlanesIterator.Value());}

