// Modified	27/12/98 : FMN ; PERF: OPTIMISATION LOADER (LOPTIM)
//

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
