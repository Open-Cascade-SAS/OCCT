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

// Modified	27/12/98 : FMN ; PERF: OPTIMISATION LOADER (LOPTIM)

#include <Viewer_Viewer.ixx>


#define LOPTIM
#ifndef LOPTIM
static TCollection_ExtendedString nm;
#else 
static TCollection_ExtendedString& _nm() {
    static TCollection_ExtendedString nm("");
return nm;
}
#define nm _nm()
#endif // LOPTIM

Viewer_Viewer::Viewer_Viewer(const Handle(Aspect_GraphicDevice)& aDevice,
			     const Standard_ExtString aName,
			     const Standard_CString aDomain,
			     const Standard_Integer aNextCount)
:myNextCount(aNextCount),
myDomain(TCollection_AsciiString(aDomain)),
myName(TCollection_ExtendedString(aName)),
myDevice(aDevice)
{}

Standard_CString Viewer_Viewer::Domain() const {
  return myDomain.ToCString();
}
Handle(Aspect_GraphicDevice) Viewer_Viewer::Device() const {
  return myDevice;
}

Standard_ExtString Viewer_Viewer::NextName () const {
  nm = TCollection_ExtendedString(myName.ToExtString());
  nm.AssignCat(TCollection_ExtendedString(myNextCount));
  
  return nm.ToExtString();
}
void Viewer_Viewer::IncrCount() {
  myNextCount++;
}
