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
