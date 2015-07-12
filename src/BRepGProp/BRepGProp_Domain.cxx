// Created on: 2005-12-13
// Created by: Sergey KHROMOV
// Copyright (c) 1995-1999 Matra Datavision
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


#include <BRepGProp_Domain.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

//=======================================================================
//function : Next
//purpose  : Sets the index of the arc iterator to the next arc of curve.
//=======================================================================
void BRepGProp_Domain::Next () {

  // skip INTERNAL and EXTERNAL edges

  myExplorer.Next();
  while (myExplorer.More()) {
    TopAbs_Orientation Or = myExplorer.Current().Orientation();
    if ((Or == TopAbs_FORWARD) || (Or == TopAbs_REVERSED)) return;
    myExplorer.Next();
  }
}
