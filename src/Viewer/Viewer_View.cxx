#include <Viewer_View.ixx>

Viewer_View::Viewer_View():myImmediateUpdate (Standard_True) {}

void Viewer_View::ImmediateUpdate() const {
  if (myImmediateUpdate) Update();
}

Standard_Boolean Viewer_View::SetImmediateUpdate(const Standard_Boolean onoff) {
  Standard_Boolean p = myImmediateUpdate;
  myImmediateUpdate = onoff;
  return p;
}
