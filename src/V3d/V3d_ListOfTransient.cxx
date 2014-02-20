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
