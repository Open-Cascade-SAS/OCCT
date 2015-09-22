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
#include <V3d_BadValue.hxx>
#include <V3d_CircularGrid.hxx>
#include <V3d_Light.hxx>
#include <V3d_RectangularGrid.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

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
