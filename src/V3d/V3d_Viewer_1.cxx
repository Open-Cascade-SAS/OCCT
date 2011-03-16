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

