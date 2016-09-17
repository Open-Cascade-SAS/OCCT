// Created on: 1993-03-10
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Geom_Transformation.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Transformation, Standard_Transient)

Geom_Transformation::Geom_Transformation () { }


Geom_Transformation::Geom_Transformation (const gp_Trsf& T) 
: gpTrsf (T) { }


Handle(Geom_Transformation) Geom_Transformation::Copy() const {

  Handle(Geom_Transformation) T;
  T = new Geom_Transformation (gpTrsf);
  return T; 
}

Handle(Geom_Transformation) Geom_Transformation::Inverted () const {

   return new Geom_Transformation (gpTrsf.Inverted());
}


Handle(Geom_Transformation) Geom_Transformation::Multiplied (
const Handle(Geom_Transformation)& Other) const {

   return new Geom_Transformation (gpTrsf.Multiplied (Other->Trsf()));
}

Handle(Geom_Transformation) Geom_Transformation::Powered (const Standard_Integer N) const {

  gp_Trsf T = gpTrsf;
  T.Power (N);  
  return new Geom_Transformation (T);
}


void Geom_Transformation::PreMultiply (const Handle(Geom_Transformation)& Other){

   gpTrsf.PreMultiply (Other->Trsf());
}
