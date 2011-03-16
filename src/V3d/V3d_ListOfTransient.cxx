#include <TColStd_ListOfTransient.hxx>
#include <V3d_ListOfTransient.ixx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>

V3d_ListOfTransient::V3d_ListOfTransient():TColStd_ListOfTransient() {}

Standard_Boolean V3d_ListOfTransient::Contains(const Handle(Standard_Transient)& aTransient) const {
  if(IsEmpty()) return Standard_False;
  TColStd_ListIteratorOfListOfTransient i(*this);
  Standard_Boolean found = Standard_False;
  for(; i.More() && !found; i.Next()) {
    found = i.Value() == aTransient;
  }
  return found;
}

void V3d_ListOfTransient::Remove(const Handle(Standard_Transient)& aTransient){


  if(!IsEmpty()){
    TColStd_ListIteratorOfListOfTransient i(*this);
    Standard_Boolean found = Standard_False;
    while(i.More() && !found)
      if( i.Value() == aTransient ) {
	TColStd_ListOfTransient::Remove(i);}
      else {
	i.Next();}
  }
}
